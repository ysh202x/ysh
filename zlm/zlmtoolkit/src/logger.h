#ifndef __UTIL_LOGGER_H__
#define __UTIL_LOGGER_H__

#include <iostream>
#include <thread>

namespace ysh_toolkit{

using LogContextPtr = std::shared_ptr<LogContext>;

typedef enum{LTrace,LDebug,LInfo,LWarn,LError}LogLevel;

class noncopyable
{
public :
    noncopyable() = default;
    ~noncopyable() = default;

public:

    //禁止拷贝赋值函数
    noncopyable(const noncopyable &) = delete;
    //禁止移动构造函数
    noncopyable(noncopyable &&) = delete;
    //禁止移动赋值函数
    noncopyable & operator=(noncopyable &&) = delete;
    //禁止拷贝构造函数
    noncopyable & operator=(const noncopyable &) = delete;
};

class Logger::public noncopyable
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
};



class LogContext :: public std::ostream
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
};

class LogContextCapture
{
public:
    using Ptr = std::shared_ptr<LogContextCapture>;
    LogContextCapture(Logger &logger, LogLevel level, const char *file, const char *function, int line, const char *flag = "");
    ~LogContextCapture();


    LogContextCapture &operator <<(std::ostream & (*f)(std::ostream &));

    template<typename T>
    LogContextCapture &operator << (T && data)
    {
        if(!_pLogContext)
        {
            return *this;
        }
        (*_pLogContext) << std::forward<T>(data);
        return *this;
    }

private:
    LogContextPtr _pLogContext;
    Logger & _logger;

};

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
    //void write(const LogContext &log_context,const std::string &message) override;
    void run();

private:
    std::thread _thread;
    bool _exit_flag;
};

class LogChannel
{
public:
    LogChannel(const std::string &channel_name,LogLevel level = LInfo);
    virtual ~LogChannel();

//保护继承 可以在派生类使用    
protected:
    std::string _channel_name;
    LogLevel _level;
};


class LoggerWrapper{
public:
    static void printLogV(int level, const char *file, const char *function, 
                            int line, const char *fmt, va_list ap);
    static void printLog(int level,const char *file,
                         const char *function,int line,const char  *fmt,...);
};




#define PrintLog(level, ...) ::ysh_toolkit::LoggerWrapper::printLog( level, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)

}
#endif