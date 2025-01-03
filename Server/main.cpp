#include "TCPServer.hpp"

int main(int argc, char* argv[])
{
    NETAPP::TCPServer server;
    server.openPort(8080);
    server.start();
    
    return 0;
}