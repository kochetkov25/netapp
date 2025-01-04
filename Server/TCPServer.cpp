#include "TCPServer.hpp"

#include "common.hpp"

#include <vector>
#include <string>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>

#include "pack.pb.h"
namespace NETAPP
{
    TCPServer::TCPServer()
    {
        m_servPort = 8080;
        m_status.store(ServerStatus::DOWN);
    }

    TCPServer::~TCPServer()
    {
        stop();
    }

    void TCPServer::joinThrds()
    {
        if(m_acceptThrd.joinable())
            m_acceptThrd.join();
        
        if(m_handleThrd.joinable())
            m_handleThrd.join();
    }

    bool TCPServer::openPort()
    {
        m_serverSockDesc = socket(AF_INET, SOCK_STREAM, 0);
        if(m_serverSockDesc == -1)
        {
            logErr();
            return false;
        }

        /*set nonblock socket*/
        fcntl(m_serverSockDesc, F_SETFL, O_NONBLOCK);

        // int flag = true;
        // setsockopt(m_serverSockDesc, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));

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

        /*accept clients*/
        m_acceptThrd = std::thread([this](){this->acceptClients();});
        /*handle clients*/
        m_handleThrd = std::thread([this](){this->handleClients();});
    }

    void TCPServer::acceptClients()
    {
        socklen_t clientAddrLen = 0;
        sockaddr_in clientInf = {0};
        int clientSockDesc = -1;

        while(m_status.load() == ServerStatus::UP)
        {
            clientSockDesc = accept(m_serverSockDesc, reinterpret_cast<sockaddr*>(&clientInf), &clientAddrLen);
            if(clientSockDesc >= 0 && m_status.load() == ServerStatus::UP)
            {
                std::cout<<"New client on socket: "<<clientSockDesc<<"\n";
                std::unique_ptr<TClient> pClient(new TClient{clientSockDesc, clientInf, clientAddrLen});
                
                std::lock_guard<std::mutex> grd(m_clientMtx);
                m_clients.emplace_back(std::move(pClient));
            }
        }
    }

    void TCPServer::handleClients()
    {
        while(m_status.load() == ServerStatus::UP)
        {
            std::lock_guard<std::mutex> grd(m_clientMtx);
            for(auto it = m_clients.begin(); it != m_clients.end(); )
            {
                std::vector<char> buff(1024);
                ssize_t cntBytes = recv((*it)->m_clientSockDesc, buff.data(), buff.size(), 0);
                if(cntBytes)
                {
                    std::cout<<"Get: "<<cntBytes<<" on socket: "<<(*it)->m_clientSockDesc<<"\n";
                }
                else
                {
                    disconnectClient((*it));
                    it = m_clients.erase(it);
                    continue;
                }
                ++it;
            }
        }
    }

    void TCPServer::disconnectClient(const std::unique_ptr<TClient> &client)
    {
        std::cout<<"Disconnected on socket: "<<client->m_clientSockDesc<<"\n";
        shutdown(client->m_clientSockDesc, SHUT_RDWR);
        close(client->m_clientSockDesc);
    }

    void TCPServer::stop()
    {
        if(m_status.load() == ServerStatus::DOWN)
        {
            std::cout<<"Server already stoped!"<<"\n";
            return;
        }

        m_status.store(ServerStatus::DOWN);

        close(m_serverSockDesc);

        joinThrds();

        for(auto& client : m_clients)
            disconnectClient(client);

        m_clients.clear();


        std::cout<<"Server stoped!"<<"\n";
    }
}