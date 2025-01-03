#include "TCPServer.hpp"

#include <google/protobuf/stubs/common.h>

int main(int argc, char* argv[])
{
    NETAPP::TCPServer server;
    server.openPort(8080);
    server.start();
    
    google::protobuf::ShutdownProtobufLibrary();
    return 0;
}