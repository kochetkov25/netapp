#include "TCPServer.hpp"

#include <iostream>

#include <signal.h>

#include <google/protobuf/stubs/common.h>

int main(int argc, char* argv[])
{
    signal(SIGPIPE, SIG_IGN);
    
    NETAPP::TCPServer server;
    server.start();

    bool work = true;
    std::string cmd;
    while(work)
    {
        std::cin >> cmd;
        if(cmd == "stop")
        {
            server.stop();
            work = false;
        }
    }

    google::protobuf::ShutdownProtobufLibrary();
    return 0;
}