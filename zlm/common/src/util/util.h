#ifndef __YSH_UTIL__
#define __YSH_UTIL__


#include <string>
#include <ctime>
#include <sys/time.h>
#include <unistd.h>
#include <limits.h>
#include <iostream>

namespace ysh_toolkit
{

std::string exe_path(bool  is_exe= true);
std::string exe_dir(bool is_exe= true);
struct tm getLocalTime(time_t sec);
std::string getTimeStr(const char *fmt, time_t time) ;


//禁止拷贝基类  [AUTO-TRANSLATED:a4ca4dcb]
//Prohibit copying of base classes
class noncopyable {
protected:
    noncopyable() {}
    ~noncopyable() {}
private:
    //禁止拷贝  [AUTO-TRANSLATED:e8af72e3]
    //Prohibit copying
    noncopyable(const noncopyable &that) = delete;
    noncopyable(noncopyable &&that) = delete;
    noncopyable &operator=(const noncopyable &that) = delete;
    noncopyable &operator=(noncopyable &&that) = delete;
};

}


#endif
