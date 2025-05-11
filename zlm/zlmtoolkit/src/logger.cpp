#include <iostream>
#include <memory>
#include <sys/time.h>
#include <stdarg.h>
#include "logger.h"

//使用名称空间
using namespace ysh_toolkit;

#define INSTANCE_IMP(class_name, ...) \
class_name &class_name::Instance() { \
    static std::shared_ptr<class_name> s_instance(new class_name(__VA_ARGS__)); \
    static class_name &s_instance_ref = *s_instance; \
    return s_instance_ref; \
}

LogContext::LogContext(LogLevel level,const char *file,const char *function,int line,
                              const char *module_name,const char *flag)
{
    _level = level;
    _file = file;
    _function = function;
    _line = line;
    _module_name = module_name;
    _flag = flag;
    gettimeofday(&_tv, nullptr);
    //_time = print_time(_tv);
}

const std::string & LogContext::str()
{
    if(_content.empty())
    {
        _content = " " + _module_name + " " + _flag + " " + _file + " " + _function + " " + std::to_string(_line) + " " + _flag + " ";
    }

    return _content;
}

void AsyncLogWriter::run()
{
    pthread_setname_np(pthread_self(), "async log");
    while(!_exit_flag)
    {
        //wait sem
        
    }
}

AsyncLogWriter::AsyncLogWriter() :_exit_flag(false)
{
    std::cout << "AsyncLogWriter" << std::endl;
    _thread = std::thread([this]() {
        this->run();
    });
}

AsyncLogWriter::~AsyncLogWriter()
{
    _exit_flag = true;
    _thread.join();
}


INSTANCE_IMP(Logger,"ysh_logger");

Logger::Logger(const std::string &loggername)
{
    _logger_name = loggername;
}

Logger::~Logger()
{
    std::cout << "Logger destructor" << std::endl;
}


/*-----------------log_channel------------------- */
LogChannel::LogChannel(const std::string &channel_name,LogLevel level)
{
    _channel_name = channel_name;
    _level = level;
}

LogChannel::~LogChannel()
{
    std::cout << "log_channel destructor" << std::endl;
}
#if 0
std::string LogChannel::print_time(const timeval & tv)
{
    auto tm = localtime(&tv.tv_sec);
    char buf[64] = {0};
    snprintf(buf, sizeof(buf), "%d-%02d-%02d %02d:%02d:%02d.%03d",
            1900 + tm.tm_year,
            1 + tm.tm_mon,
            tm.tm_mday,
            tm.tm_hour,
            tm.tm_min,
            tm.tm_sec,
            (int)(tv.tv_usec / 1000));

    return buf;   
    
}
#endif

#if 0
static int vasprintf(char **strp, const char *fmt, va_list ap) {
    // _vscprintf tells you how big the buffer needs to be
    int len = _vscprintf(fmt, ap);
    if (len == -1) {
        return -1;
    }
    size_t size = (size_t)len + 1;
    char *str = (char*)malloc(size);
    if (!str) {
        return -1;
    }
    // _vsprintf_s is the "secure" version of vsprintf
    int r = vsprintf_s(str, len + 1, fmt, ap);
    if (r == -1) {
        free(str);
        return -1;
    }
    *strp = str;
    return r;
}
#endif

void LoggerWrapper::printLogV(int level, const char *file, const char *function, 
                            int line, const char *fmt, va_list ap)
{
    char *str = nullptr;
    if(vasprintf(&str, fmt, ap) >= 0 && str)
    {
        free(str);
    }
}

void LoggerWrapper::printLog(int level,const char *file,
                         const char *function,int line,const char  *fmt,...)
{
    va_list ap;
    va_start(ap,fmt);
    printLogV(level, file, function, line, fmt, ap);
    va_end(ap);
}


