#pragma once

#include <netinet/in.h>

#include <sys/epoll.h>

#include <thread>
#include <vector>
#include <memory>
#include <mutex>

#include <atomic>

namespace NETAPP
{
    class TCPServer
    {
        public:
            enum class ServerStatus
            {
                DOWN,
                UP
            };

            TCPServer();
            ~TCPServer();

            void start();

            void stop();

            void joinThrds();
        private:
            struct TClient
            {
                int m_clientSockDesc;
                sockaddr_in m_clientInf;  
                socklen_t m_clientAddrLen;
            };
            std::vector<std::unique_ptr<TClient>> m_clients; //GUARDED BY m_clientMtx
            std::mutex m_clientMtx;


            bool openPort();

            void handleClients();

            void acceptClients();

            void disconnectClient(const std::unique_ptr<TClient>& client);

            
            std::thread m_acceptThrd;
            std::thread m_handleThrd;

            std::thread m_mainThrd;

            uint16_t m_servPort;
            sockaddr_in m_servInf;
            int m_serverSockDesc;

            std::atomic<ServerStatus> m_status;

            /*epoll*/
            int m_epollDesc;
            epoll_event m_epollEvents[20]; //GUARDED BY m_epollMtx
            std::mutex m_epollMtx;
            void setEpoll(int sockDesc, epoll_event ev);
            void unsetEpoll(int sockDesc);
            int waitEpoll();


            void mainLoop();
    };
}