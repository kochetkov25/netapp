#include <errno.h>
#include <string.h>
#include <iostream>

#include <spdlog/spdlog.h>
namespace NETAPP
{
    void logErr()
    {
        spdlog::error(strerror(errno));
    }  
} 
