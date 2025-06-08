

#include "logger.h"

using namespace ysh_toolkit;

int main()
{
    //std::cout << "main" << std::endl;
    Logger::Instance().add(std::make_shared<ConsoleChannel>("console", LDebug));
    Logger::Instance().add(std::make_shared<FileChannel>("FileChannel","./" ,LDebug));

    Logger::Instance().setWriter(std::make_shared<AsyncLogWriter>());

    for(int i = 0; i < 3 ; ++i)
    {
        PrintLogD("%s","hello");
        DebugL << 1 << "+" << 2 << '=' << 3;
    }
    sleep(1);

}