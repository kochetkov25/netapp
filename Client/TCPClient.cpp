#include "TCPClient.hpp"

#include "common.hpp"

#include <arpa/inet.h>

#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <iostream>
#include <string>

#include "pack.pb.h"
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
        size_t res = ::send(m_sockDesc, data, size, 0);
        if(res != -1)
        {
            return true;
        }
        else
        {
            logErr();
            return false;
        }
    }

    bool TCPClient::sendProto(const char *data, size_t size)
    {
        Pack pack;
        pack.set_id(30125);
        pack.set_data("TEST: proto msg from client.");
        pack.set_name("Test Client #1");
        pack.set_value(221730125);
        std::string sendPack;
        pack.SerializeToString(&sendPack);
        auto res = send(sendPack.data(), sendPack.size());
        if(res)
        {
            std::cout<<"client sent: "<<sendPack.size()<<"bytes"<<"\n";
            return true;
        }
        return false;
    }

    void TCPClient::receive()
    {

    }
}