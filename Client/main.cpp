#include "TCPClient.hpp"

#include <string>
#include <unistd.h>
#include <signal.h>

#include <google/protobuf/stubs/common.h>

 #include <spdlog/spdlog.h>

int main(int argc, char* argv[])
{
    spdlog::set_level(spdlog::level::debug);

    signal(SIGPIPE, SIG_IGN);
    
    NETAPP::TCPClient client;
    client.connect("127.0.0.1", 8080);

    bool wrk = true;
    std::string cmd;
    while(wrk)
    {
        std::cin>>cmd;
        if(cmd == "stop")
        {
            wrk = false;
            continue;
        }

        auto res = client.sendProto(cmd.data(), cmd.size());
        if(!res)
        {
            wrk = false;
            continue;
        }
    }

    client.exit();
    google::protobuf::ShutdownProtobufLibrary();
    return 0;
}