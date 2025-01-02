#pragma once

#include <netinet/in.h>

namespace NETAPP
{
    class TCPServer
    {
        public:
            TCPServer();

            bool openPort(uint16_t port);

            void start();

            void exit();
        private:
            int m_sockDesc;
            int m_clientSockDesc;
            sockaddr_in m_servInf;
            sockaddr_in m_clientInf;
    };
}