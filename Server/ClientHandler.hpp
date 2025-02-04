#pragma once

#include <string>
#include <vector>

namespace NETAPP
{
    class ClientHandler
    {
        public:
            std::string parseData(const std::vector<char>& buff);
        private:
    };
}