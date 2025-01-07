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
    TCPServer::TCPServer()
    {
        m_servPort = 8080;
        m_status.store(ServerStatus::DOWN);

        m_epollDesc = epoll_create1(0);
    }

    TCPServer::~TCPServer()
    {
        stop();
    }

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
        fcntl(m_serverSockDesc, F_SETFL, O_NONBLOCK);

        m_servInf.sin_family = AF_INET;
        m_servInf.sin_addr.s_addr = htonl(INADDR_ANY);
        m_servInf.sin_port = htons(m_servPort);

        auto errCode = bind(m_serverSockDesc, reinterpret_cast<sockaddr*>(&m_servInf), sizeof(m_servInf));
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
        std::cout<<"Server listening on port: " << m_servPort << "\n";
        return true;
    }

    void TCPServer::start()
    {
        if(m_status.load() == ServerStatus::UP)
        {
            std::cout<<"Server already runnig!"<<"\n";
            return;
        }   
        
        if(!openPort())
        {
            std::cout<<"Unable to open port: "<<m_servPort<<std::endl;
            return;
        }
        std::cout<<"Ready for new clients!"<<"\n";
        m_status.store(ServerStatus::UP);

        m_mainThrd = std::thread([this](){this->mainLoop();});
    }

    void TCPServer::setEpoll(int sockDesc, epoll_event ev)
    {
        std::lock_guard<std::mutex> grd(m_epollMtx);
        epoll_ctl(m_epollDesc, EPOLL_CTL_ADD, sockDesc, &ev);
    }

    void TCPServer::unsetEpoll(int sockDesc)
    {
        std::lock_guard<std::mutex> grd(m_epollMtx);
        epoll_ctl(m_epollDesc, EPOLL_CTL_DEL, sockDesc, nullptr);
    }

    int TCPServer::waitEpoll()
    {
        return epoll_wait(m_epollDesc, m_epollEvents, EVENT_SIZE, -1);
    }

    void TCPServer::disconnectClient(int sockDesc)
    {
        std::cout<<"disconnect on socket: "<<sockDesc<<"\n";
        unsetEpoll(sockDesc);
        close(sockDesc);
    }

    void TCPServer::awakeEpoll()
    {
        int epollKiller = eventfd(0,0);
        epoll_event ev;
        ev.events = EPOLLIN;
        ev.data.fd = epollKiller;

        setEpoll(epollKiller, ev);

        uint64_t value = 1;
        write(epollKiller, &value, sizeof(value));
        std::cout<<"epoll awake!"<<"\n";
    }

    void TCPServer::stop()
    {
        if(m_status.load() == ServerStatus::DOWN)
        {
            std::cout<<"Server already stoped!"<<"\n";
            return;
        }

        m_status.store(ServerStatus::DOWN);
        
        shutdown(m_serverSockDesc, SHUT_RDWR);
        close(m_serverSockDesc);

        awakeEpoll();
        close(m_epollDesc);

        
        joinThrds();

        std::cout<<"Server stoped!"<<"\n";
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
            std::cout<<"main loop is running..."<<"\n";
            int cntEvents = waitEpoll();
            std::cout<<"main loop got event!"<<"\n";

            for(int ev = 0; ev < cntEvents; ev++)
            {
                /*new connection on server socket*/
                if(m_epollEvents[ev].data.fd == m_serverSockDesc)
                {
                    clientSockDesc = accept(m_serverSockDesc, reinterpret_cast<sockaddr*>(&clientInf), &clientAddrLen);
                    if(clientSockDesc >= 0)
                    {
                        std::cout<<"New client on socket: "<<clientSockDesc<<"\n";
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
                        ssize_t cntBytes = recv(m_epollEvents[ev].data.fd, buff.data(), buff.size(), 0);
                        if(cntBytes > 0)
                            std::cout<<"get: "<<cntBytes<<" on socket: "<<m_epollEvents[ev].data.fd<<"\n";
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