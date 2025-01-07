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

            bool openPort();

            void disconnectClient(int sockDesc);

            uint16_t m_servPort;
            sockaddr_in m_servInf;
            int m_serverSockDesc;

            std::atomic<ServerStatus> m_status;

            /*epoll*/
            static const int EVENT_SIZE = 20;
            int m_epollDesc;
            epoll_event m_epollEvents[EVENT_SIZE]; //GUARDED BY m_epollMtx
            std::mutex m_epollMtx;

            void setEpoll(int sockDesc, epoll_event ev);
            void unsetEpoll(int sockDesc);
            void awakeEpoll();
            int  waitEpoll();

            std::thread m_mainThrd;
            void mainLoop();
    };
}