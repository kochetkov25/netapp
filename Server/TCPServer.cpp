#include "TCPServer.hpp"

#include "common.hpp"

#include <iostream>

namespace NETAPP
{
    TCPServer::TCPServer()
    {

    }

    bool TCPServer::openSocket(uint16_t port)
    {
        m_sockDesc = socket(AF_INET, SOCK_STREAM, 0);
        if(m_sockDesc == -1)
        {
            logErr();
            return false;
        }

        m_servInf.sin_family = AF_INET;
        m_servInf.sin_addr.s_addr = htonl(INADDR_ANY);
        m_servInf.sin_port = htons(port);

        auto errCode = bind(m_sockDesc, reinterpret_cast<sockaddr*>(&m_servInf), sizeof(m_servInf));
        if(errCode != 0)
        {
            logErr();
            return false;
        }

        errCode = listen(m_sockDesc, 5);
        if(errCode < 0)
        {
            logErr();
            return false;
        }
        std::cout<<"Server listening on port: " << port<< "\n";
        return true;
    }

    void NETAPP::TCPServer::start()
    {
        std::cout<<"Ready for new clients..."<<"\n";
        while(true)
        {
            socklen_t clientAddrLen = 0;
            auto clientDesc = accept(m_sockDesc, reinterpret_cast<sockaddr*>(&m_clientInf), &clientAddrLen);
            if(clientDesc)
                std::cout<<"New Client!" <<"\n";
        }
    }
}