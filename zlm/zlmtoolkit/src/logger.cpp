#include <iostream>
#include "logger.h"



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

AsyncLogWriter::AsyncLogWriter()
{
    std::cout << "AsyncLogWriter" << std::endl;
    _thread = std::thread(&AsyncLogWriter::run,this);
}

AsyncLogWriter::~AsyncLogWriter()
{

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
log_channel::log_channel(const std::string &channel_name,LogLevel level)
{
    _channel_name = channel_name;
    _level = level;
}

log_channel::~log_channel()
{
    std::cout << "log_channel destructor" << std::endl;
}

std::string log_channel::print_time(const timeval & tv)
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


