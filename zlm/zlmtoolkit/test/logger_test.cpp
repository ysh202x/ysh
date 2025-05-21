

#include "logger.h"

using namespace ysh_toolkit;

int main()
{
    //std::cout << "main" << std::endl;
    PrintLogD("%s\n","hello");
    DebugL << 1 << "+" << 2 << '=' << 3;
}