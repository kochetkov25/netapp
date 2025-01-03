#include "TCPClient.hpp"

#include <string>
#include <unistd.h>
#include <signal.h>

int main(int argc, char* argv[])
{
    signal(SIGPIPE, SIG_IGN);
    
    NETAPP::TCPClient client;
    client.connect("127.0.0.1", 8080);

    std::string msg = "start";
    int num = 0;
    while(client.send(msg.data(), msg.size()))
    {
        msg.clear();
        msg += "pack num: " + std::to_string(num);
        num++;
        sleep(1);
    }

    client.exit();
    return 0;
}