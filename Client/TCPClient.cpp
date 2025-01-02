#include "TCPClient.hpp"

#include "common.hpp"

#include <arpa/inet.h>

#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <iostream>
#include <string>

namespace NETAPP
{

    TCPClient::TCPClient()
    {
        m_sockDesc = -1;
        m_port = 0;
        m_ipAdr = "";
    }

    TCPClient::TCPClient(const std::string &ip, uint16_t port)
    {

    }

    bool TCPClient::connect(const std::string &ip, uint16_t port)
    {
        m_sockDesc = socket(AF_INET, SOCK_STREAM, 0);
        if(m_sockDesc == -1)
        {
            logErr();
            return false;
        }

        m_servInf.sin_family = AF_INET;
        m_servInf.sin_port = htons(port);

        in_addr ip2num;
        auto errCode = inet_pton(AF_INET, ip.c_str(), &ip2num);
        if(errCode <= 0)
        {
            logErr();
            return false;
        }

        m_servInf.sin_addr = ip2num;

        errCode = ::connect(m_sockDesc, reinterpret_cast<sockaddr*>(&m_servInf), sizeof(m_servInf));
        if(errCode != 0)
        {
            logErr();
            return false;
        }
        
        std::cout<< "Conection successful!" <<"\n";
        return true;
    }

    void TCPClient::exit()
    {
        std::cout<< "Close socket: " << m_sockDesc <<"\n";
        close(m_sockDesc);
    }

    bool TCPClient::send(const char *data, size_t size)
    {
        static int num = 0;
        if(m_sockDesc)
        {
            while (true)
            {
                std::string str = "pack num: ";
                str += std::to_string(num);
                num++;
                auto res = ::send(m_sockDesc, str.data(), str.size(), 0);
                std::cout<<"send: "<<res<<"\n";
                sleep(1);
            }            
        }
        return true;
    }
    void TCPClient::receive()
    {

    }
}