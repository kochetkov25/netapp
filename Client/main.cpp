#include "TCPClient.hpp"

#include<string>

int main(int argc, char* argv[])
{
    NETAPP::TCPClient client;
    client.connect("127.0.0.1", 8080);
    std::string msg = "Hello Server!";
    client.send(msg.data(), msg.size());
    // client.exit();
    return 0;
}