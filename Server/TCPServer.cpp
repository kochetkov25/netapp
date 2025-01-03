#include "TCPServer.hpp"

#include "common.hpp"

#include <vector>
#include <string>
#include <iostream>
#include <unistd.h>
#include <thread>

#include "pack.pb.h"
namespace NETAPP
{
    TCPServer::TCPServer()
    {

    }

    bool TCPServer::openPort(uint16_t port)
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

    void TCPServer::start()
    {
        std::cout<<"Ready for new clients..."<<"\n";
        while(true)
        {
            socklen_t clientAddrLen = 0;
            m_clientSockDesc = accept(m_sockDesc, reinterpret_cast<sockaddr*>(&m_clientInf), &clientAddrLen);
            if(m_clientSockDesc)
            {
                std::thread thrd([this](){
                    this->clientHandler();
                });
                thrd.detach();
            }
        }
    }

    void TCPServer::clientHandler()
    {
        std::vector<char> buff(1024);
        while(recv(m_clientSockDesc, buff.data(), buff.size(), 0))
        {
            std::lock_guard<std::mutex>grd(m_clientMtx);

            Pack recivedPack;
            recivedPack.ParseFromArray(buff.data(), buff.size());
            std::cout<<"[Recived id]: "<<recivedPack.id()<<"\n";
            std::cout<<"[Recived name]: "<<recivedPack.name()<<"\n";
            std::cout<<"[Recived data]: "<<recivedPack.data()<<"\n";
            std::cout<<"[Recived value]: "<<recivedPack.value()<<"\n";
        }
    }

    void TCPServer::exit()
    {
        close(m_sockDesc);
        close(m_clientSockDesc);
    }
}