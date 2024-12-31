#include <errno.h>
#include <string.h>
#include <iostream>

namespace NETAPP
{
    void logErr()
    {
        std::cout<< "Error: " << strerror(errno) <<"\n";
    }  
} 
