#include "util.h"
#include <cstring>
using namespace std;
namespace ysh_toolkit
{
std::string exe_path(bool  is_exe)
{
    char  buffer[PATH_MAX] = {0};
    int n = -1;
    n = readlink("/proc/self/exe", buffer, sizeof(buffer));
    string filePath;
    if (n <= 0) 
    {
        filePath = "./";
    } 
    else 
    {
        filePath = buffer;
    }

    return filePath;

}

std::string exe_dir(bool isExe)
{   
    auto path = exe_path();
    return path.substr(0, path.rfind('/') + 1);        
}


std::string getTimeStr(const char *fmt, time_t time) 
{
    if (!time) {
        time = ::time(nullptr);
    }
    auto tm = localtime(time);
    size_t size = strlen(fmt) + 64;
    string ret;
    ret.resize(size);
    size = std::strftime(&ret[0], size, fmt, tm);
    if (size > 0) {
        ret.resize(size);
    }
    else{
        ret = fmt;
    }
    return ret;
}



}