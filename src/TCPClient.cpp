#include "TCPClient.hpp"

#include "common.hpp"

#include <arpa/inet.h>

#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <iostream>

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
        return false;
    }

    void TCPClient::receive()
    {

    }
}