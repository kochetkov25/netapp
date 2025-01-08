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

    NETAPP::TCPClient::~TCPClient()
    {
        exit();
    }   

    TCPClient::TCPClient(const std::string &ip, uint16_t port)
    {
        connect(ip, port);
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
        
        spdlog::info("Connection succesful!");
        return true;
    }

    void TCPClient::exit()
    {
        spdlog::info("Close socket: {}", m_sockDesc);
        close(m_sockDesc);
    }

    bool TCPClient::send(const char *data, size_t size)
    {
        ssize_t res = ::send(m_sockDesc, data, size, 0);
        if(res != -1)
        {
            spdlog::debug("Client sent: {} bytes.", res);
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

        pack.set_socket(m_sockDesc);
        pack.set_size(size);
        pack.set_data(data);

        std::string sendPack;
        pack.SerializeToString(&sendPack);

        auto res = send(sendPack.data(), sendPack.size());
        if(res)
        {
            return true;
        }
        return false;
    }

    void TCPClient::receive()
    {

    }
}