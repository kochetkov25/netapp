#pragma once

#include <netinet/in.h>

namespace NETAPP
{
    class TCPServer
    {
        public:
            TCPServer();

            bool openSocket(uint16_t port);

            void start();
        private:
            int m_sockDesc;
            sockaddr_in m_servInf;
            sockaddr_in m_clientInf;
    };
}