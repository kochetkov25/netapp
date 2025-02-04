#include "ClientHandler.hpp"


#include "pack.pb.h"

std::string NETAPP::ClientHandler::parseData(const std::vector<char> &buff)
{
    Pack requestPack;
    requestPack.ParseFromArray(buff.data(), buff.size());

    Pack responsePack;
    if(     requestPack.request() == "hello")
    {
        std::string str = "Hello_" + requestPack.name();
        responsePack.set_request(str.data());
        responsePack.set_name("from_server");
        responsePack.set_value(0);
    }
    else if(requestPack.request() == "echo")
    {
        std::string str = requestPack.name();
        responsePack.set_request(str.data());
        responsePack.set_name("from_server");
        responsePack.set_value(requestPack.value());
    }
    else
    {
        std::string str = "unknown_request";
        responsePack.set_request(str.data());
        responsePack.set_name("from_server");
        responsePack.set_value(0);
    }
    std::string responce;
    responsePack.SerializeToString(&responce);
    
    return responce;
}
