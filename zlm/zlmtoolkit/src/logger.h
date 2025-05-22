#ifndef __UTIL_LOGGER_H__
#define __UTIL_LOGGER_H__

#include <iostream>
#include <thread>
#include <sstream>
#include <memory>

namespace ysh_toolkit{

class LogContext;
class Logger;

using LogContextPtr = std::shared_ptr<LogContext>;

typedef enum{LTrace,LDebug,LInfo,LWarn,LError}LogLevel;

Logger &getLogger();

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

class Logger:public noncopyable
{
public:
    /*
        日志单例
    */
    static Logger& Instance();

    explicit Logger(const std::string &loggername);
    ~Logger();


    /*添加日志通道*/
    void add(const std::shared_ptr<LogChannel> &channel) { _channels[channel->name()] = channel; }
    /*删除日志通道*/
    void del(const std::string &name) { _channels.erase(name); }

    std::shared_ptr<LogChannel> get(const std::string &name)
    {
        auto it = _channels.find(name)
        if(it != _channels.end)
        {
            return it->second;
        }
        return nullptr;
    }

    /*写日志*/
    void write(const LogContextPtr &log_context)
    {
        //whitechannel
        if(_channels.empty())
        {
            return;
        }
        for(auto &it : channels)
        {
            if(it->second)
            {
                it->second->write(log_context);
            }
        }
    }
    
private:
    std::string _logger_name;
    std::map<std::string, std::shared_ptr<LogChannel>> _channels;
};


class LogContext : public std::ostringstream
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
    bool _b_got_content = false;
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

// 日志通道抽象基类，负责将日志输出到不同目标（如控制台、文件、系统日志等）
class LogChannel : public noncopyable
{
public:
    LogChannel(const std::string &name, LogLevel level) : _name(name), _level(level) {}
    virtual ~LogChannel() = default;
    virtual void write(const LogContextPtr &ctx) = 0;
    const std::string &name() const { return _name; }
    void setLevel(LogLevel level) { _level = level; }

protected:
    std::string _name;
    LogLevel _level;
};


class ConsoleChannel : public LogChannel
{
public:
    ConsoleChannel(const std::string &name, LogLevel level) : LogChannel(name, level) {}
    void write(const LogContextPtr &ctx) override 
    {
        std::cout << ctx->str() << std::endl;
    } 
};

class LoggerWrapper{
public:
    static void printLogV(Logger &logger,LogLevel level, const char *file, const char *function, 
                            int line, const char *fmt, va_list ap);
    static void printLog(Logger &logger,LogLevel level,const char *file,
                         const char *function,int line,const char  *fmt,...);
};



//用法: DebugL << 1 << "+" << 2 << '=' << 3;  [AUTO-TRANSLATED:e6efe6cb]
//Usage: DebugL << 1 << "+" << 2 << '=' << 3;

#define WriteL(level) ::ysh_toolkit::LogContextCapture(::ysh_toolkit::getLogger(), level, __FILE__, __FUNCTION__, __LINE__, "test_flag")
#define DebugL WriteL(::ysh_toolkit::LDebug)


#define PrintLog(level, ...) ::ysh_toolkit::LoggerWrapper::printLog(::ysh_toolkit::getLogger(), level, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define PrintLogD(...) PrintLog(::ysh_toolkit::LDebug, ##__VA_ARGS__)

}
#endif
