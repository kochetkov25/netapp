#pragma once

//includes
#include <string>
#include <netinet/in.h>
//forward decl

namespace NETAPP
{
    class TCPClient
    {
        public:
            TCPClient();
            TCPClient(const std::string& ip, uint16_t port);
    
            bool connect(const std::string& ip, uint16_t port);
            void exit();
    
            bool send(const char* data, size_t size);
            void receive();
    
        private:
            int m_sockDesc;
    
            std::string m_ipAdr;
            int m_port;
    
            sockaddr_in m_servInf;

        void logErr();
    };
}