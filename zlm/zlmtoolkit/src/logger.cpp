#include <iostream>
#include <memory>
#include <sys/time.h>
#include <stdarg.h>
#include "logger.h"

//使用名称空间
using namespace std;

namespace ysh_toolkit
{

Logger *g_defaultLogger = nullptr;

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
    if(!_b_got_content)
    {
        _content = ostringstream::str();
        _b_got_content = true;
    }

    return _content;
}


LogContextCapture::LogContextCapture(Logger &logger, LogLevel level, const char *file, const char *function, int line, const char *flag):_logger(logger)
{
    _pLogContext = std::make_shared<LogContext>(level, file, function, line, " ", flag);
}

LogContextCapture::~LogContextCapture()
{
    *this << endl;
}

//std::endl
LogContextCapture& LogContextCapture::operator<<(std::ostream& (*f)(std::ostream&))
{
    if(!_pLogContext)
    {
        return *this;
    }

    _logger.write(_pLogContext);
    _pLogContext.reset();
    return *this;
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


Logger &getLogger()
{
    if(!g_defaultLogger)
    {
        g_defaultLogger = &Logger::Instance();
    }
    return *g_defaultLogger;
}

void Logger::add(const std::shared_ptr<LogChannel> &channel)
{
    _channels[channel->name()] = channel;
}

std::shared_ptr<LogChannel> Logger::get(const std::string &name)
{
    auto it = _channels.find(name);
    if(it != _channels.end())
    {
        return it->second;
    }
    return nullptr;
}

void Logger::write(const LogContextPtr &ctx)
{
    {
        //whitechannel
        if(_channels.empty())
        {
            return;
        }
        for(auto &it : _channels)
        {
            if(it.second)
            {
                it.second->write(*this,ctx);
            }
        }
    }
}



/*-----------------log_channel------------------- */

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

void LoggerWrapper::printLogV(Logger &logger,LogLevel level, const char *file, const char *function, 
                            int line, const char *fmt, va_list ap)
{
    LogContextCapture info(logger, level, file, function, line, " ");
    char *str = nullptr;
    if(vasprintf(&str,fmt,ap) >= 0 && str)
    {
        info << str;
        free(str);
    }
        
}

void LoggerWrapper::printLog(Logger &logger,LogLevel level,const char *file,
                         const char *function,int line,const char  *fmt,...)
{
    va_list ap;
    va_start(ap,fmt);
    printLogV(logger,level, file, function, line, fmt, ap);
    va_end(ap);
}

void LogChannel::format(const Logger & logger,std::ostream &ost, const LogContextPtr &ctx, bool enable_color, bool enable_detail)
{
    #define CLEAR_COLOR "\033[0m"
static const char *LOG_CONST_TABLE[][3] = {
        {"\033[44;37m", "\033[34m", "T"},
        {"\033[42;37m", "\033[32m", "D"},
        {"\033[46;37m", "\033[36m", "I"},
        {"\033[43;37m", "\033[33m", "W"},
        {"\033[41;37m", "\033[31m", "E"}};
    if(!enable_detail && ctx->str().empty())
    {
        return;
    }
    if(enable_color)
    {
        ost << LOG_CONST_TABLE[ctx->_level][1];
    }

    if(enable_detail)
    {
        // tag or process name
        //ost << "[" << (!ctx->_flag.empty() ? ctx->_flag : logger.getName()) << "] ";
        // pid and thread_name
        //ost << "[" << printf_pid() << "-" << ctx->_thread_name << "] ";
        // source file location
        ost << ctx->_file << ":" << ctx->_line << " " << ctx->_function << " | ";
    }

     // log content
    ost << ctx->str();
    ost << CLEAR_COLOR;
    ost << endl;
}

void ConsoleChannel::write(const Logger &logger,const LogContextPtr &ctx)
{
    if(_level > ctx->_level)
    {
        return;
    }
    format(logger, std::cout, ctx, true, true);
}

}

