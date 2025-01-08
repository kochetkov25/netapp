#include "TCPServer.hpp"

#include "common.hpp"

#include <vector>
#include <string>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>

#include <sys/eventfd.h>

#include "pack.pb.h"
namespace NETAPP
{
    /*ctor*/
    TCPServer::TCPServer()
    {
        m_servPort = 8080;
        m_status.store(ServerStatus::DOWN);

        m_epollDesc = epoll_create1(0);
    }

    /*dtor*/
    TCPServer::~TCPServer()
    {
        stop();
    }

    void TCPServer::setPort(uint16_t port)
    {
        if(m_status.load() == ServerStatus::UP)
        {
            stop();
        }
        m_servPort = port;
        start();
    }

    /*main server loop*/
    void TCPServer::joinThrds()
    {
        if(m_mainThrd.joinable())
            m_mainThrd.join();
    }

    bool TCPServer::openPort()
    {
        m_serverSockDesc = socket(AF_INET, SOCK_STREAM, 0);
        if(m_serverSockDesc == -1)
        {
            logErr();
            return false;
        }

        /*set nonblock server socket*/
        auto errCode = fcntl(m_serverSockDesc, F_SETFL, O_NONBLOCK);
        if(errCode == -1)
        {
            logErr();
            return false;
        }

        m_servInf.sin_family = AF_INET;
        m_servInf.sin_addr.s_addr = htonl(INADDR_ANY);
        m_servInf.sin_port = htons(m_servPort);

        errCode = bind(m_serverSockDesc, reinterpret_cast<sockaddr*>(&m_servInf), sizeof(m_servInf));
        if(errCode != 0)
        {
            logErr();
            return false;
        }

        errCode = listen(m_serverSockDesc, 5);
        if(errCode < 0)
        {
            logErr();
            return false;
        }
        spdlog::info("Server listening on port: {}", m_servPort);
        return true;
    }

    void TCPServer::start()
    {
        if(m_status.load() == ServerStatus::UP)
        {
            spdlog::warn("Server already running!");
            return;
        }   
        
        if(!openPort())
        {
            spdlog::critical("Unable to open port: {}", m_servPort);
            return;
        }
        spdlog::info("Ready for new clients!");
        m_status.store(ServerStatus::UP);

        m_mainThrd = std::thread([this](){this->mainLoop();});
    }

    /*wrapper*/
    bool TCPServer::setEpoll(int sockDesc, epoll_event ev)
    {
        std::lock_guard<std::mutex> grd(m_epollMtx);
        auto err = epoll_ctl(m_epollDesc, EPOLL_CTL_ADD, sockDesc, &ev);
        if(err == -1)
        {
            logErr();
            return false;
        }
        return true;
    }

    /*wrapper*/
    bool TCPServer::unsetEpoll(int sockDesc)
    {
        std::lock_guard<std::mutex> grd(m_epollMtx);
        auto err = epoll_ctl(m_epollDesc, EPOLL_CTL_DEL, sockDesc, nullptr);
        if(err == -1)
        {
            logErr();
            return false;
        }
        return true;
    }

    /*wrapper*/
    int TCPServer::waitEpoll()
    {
        return epoll_wait(m_epollDesc, m_epollEvents, EVENT_SIZE, -1);
    }

    /*wrapper*/
    bool TCPServer::awakeEpoll()
    {
        int epollKiller = eventfd(0,0);
        epoll_event ev;
        ev.events = EPOLLIN;
        ev.data.fd = epollKiller;

        setEpoll(epollKiller, ev);

        uint64_t value = 1;
        auto err = write(epollKiller, &value, sizeof(value));
        if(err == -1)
        {
            logErr();
            return false;
        }
        return true;
    }

    void TCPServer::disconnectClient(int sockDesc)
    {
        spdlog::info("Disconnect on socket: {}", sockDesc);
        unsetEpoll(sockDesc);
        close(sockDesc);
    }


    void TCPServer::stop()
    {
        if(m_status.load() == ServerStatus::DOWN)
        {
            spdlog::warn("Server already stoped!");
            return;
        }

        m_status.store(ServerStatus::DOWN);
        
        shutdown(m_serverSockDesc, SHUT_RDWR);
        close(m_serverSockDesc);

        awakeEpoll();
        close(m_epollDesc);

        
        joinThrds();

        spdlog::info("Server stoped!");
    }

    void NETAPP::TCPServer::mainLoop()
    {
        /*inf about conected client*/
        socklen_t clientAddrLen = 0;
        sockaddr_in clientInf = {0};
        int clientSockDesc = -1;

        /*set server sock to epoll*/
        epoll_event epollEv{};
        epollEv.events = EPOLLIN;
        epollEv.data.fd = m_serverSockDesc;
        setEpoll(m_serverSockDesc, epollEv);

        while(m_status.load() == ServerStatus::UP)
        {
            spdlog::debug("main loop is runing...");
            int cntEvents = waitEpoll();
            spdlog::debug("main loop got event!");

            for(int ev = 0; ev < cntEvents; ev++)
            {
                /*new connection on server socket*/
                if(m_epollEvents[ev].data.fd == m_serverSockDesc)
                {
                    clientSockDesc = accept(m_serverSockDesc, reinterpret_cast<sockaddr*>(&clientInf), &clientAddrLen);
                    if(clientSockDesc >= 0)
                    {
                        spdlog::info("New client on socket: {}", clientSockDesc);
                        /*set client sock nonblock*/
                        fcntl(clientSockDesc, F_SETFL, O_NONBLOCK);
                        /*set client sock to epoll*/
                        epollEv.events = EPOLLIN | EPOLLET;
                        epollEv.data.fd = clientSockDesc;
                        setEpoll(clientSockDesc, epollEv);
                    }
                }
                else /*new data from client*/
                {
                    /*disconnect or error*/
                    if(m_epollEvents[ev].events & EPOLLERR || m_epollEvents[ev].events & EPOLLHUP)
                    {
                        disconnectClient(m_epollEvents[ev].data.fd);
                    }
                    else if(m_epollEvents[ev].events & EPOLLIN) /*data*/
                    {
                        std::vector<char> buff(1024);
                        int sd = m_epollEvents[ev].data.fd;
                        ssize_t cntBytes = recv(sd, buff.data(), buff.size(), 0);
                        if(cntBytes > 0)
                            spdlog::info("Got: {} bytes on socket: {}.", cntBytes, sd);
                        else /*client disconnected*/
                        {
                            disconnectClient(m_epollEvents[ev].data.fd);
                        }
                    }
                }
            }
        }
    }
}