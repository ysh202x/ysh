#ifndef __UTIL_LOGGER_H__
#define __UTIL_LOGGER_H__

#include <iostream>

namespace ysh_toolkit{

typedef enum
{
    LTrace,
    LDebug,
    LInfo,
    LWarn,
    LError,
}LogLevel;


class LogContext
{
public:
    LogContext() = default;
    LogContext(LogLevel level,const char *file,const char *function,int line,
                              const char *module_name,const char *flag);
    ~LogContext() = default;

    LogLevel _level;
    int _line;
    std::string _file;
    std::string _function;
    std::string _module_name;
    std::string _thread_name;
    std::string _flag;

    struct timeval _tv;

    const std::string &str();

private:
    std::string _content;
}

class LogWriter
{
public:
    LogWriter() = default;
    virtual ~LogWriter() = default;
    virtual void write(const LogContext &log_context,const std::string &message) = 0;   
    
};

class AsyncLogWriter:public LogWriter
{
public:
    AsyncLogWriter();
    ~AsyncLogWriter();

private:
    void write(const LogContext &log_context,const std::string &message) override;
    void run();

private:
    std::thread _thread;

    
}

class LogChannel
{
public:
    log_channel(const std::string &channel_name,LogLevel level = LInfo);
    virtual ~log_channel();

//保护继承 可以在派生类使用    
protected:
    std::string _channel_name;
    LogLevel _level;
}



class Logger
{
public:
    /*
        日志单例
    */
    static Logger& Instance();

    explicit Logger(const std::string &loggername);
    ~Logger();

private:
    std::string _logger_name;
}



#define  PrintLog(level,...) 

#endif