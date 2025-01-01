#include "TCPClient.hpp"

int main(int argc, char* argv[])
{
    NETAPP::TCPClient client;
    client.connect("127.0.0.1", 8080);
    
    client.exit();
    return 0;
}