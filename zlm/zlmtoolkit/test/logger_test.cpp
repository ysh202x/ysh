

#include "logger.h"

using namespace ysh_toolkit;

int main()
{
    //std::cout << "main" << std::endl;
    Logger::Instance().add(std::make_shared<ConsoleChannel>("console", LDebug));

    
    PrintLogD("%s\n","hello");
    DebugL << 1 << "+" << 2 << '=' << 3;
}