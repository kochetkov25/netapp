#include <iostream>
#include <csignal>

#include "TCPClient.hpp"
#include "TCPServer.hpp"

int main(int argc, char* argv[])
{
    NETAPP::TCPServer server;
    server.openSocket(8080);
    //server.start();

    NETAPP::TCPClient client;
    client.connect("127.0.0.1", 8080);
    client.exit();
    return 0;
}