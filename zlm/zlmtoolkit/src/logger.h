#ifndef __UTIL_LOGGER_H__
#define __UTIL_LOGGER_H__

typedef enum
{
    LTrace,
    LDebug,
    LInfo,
    LWarn,
    LError,
}LogLevel;


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


class log_channel
{
public:
    log_channel(const std::string &channel_name,LogLevel level = LInfo);
    virtual ~log_channel();

//保护继承 可以在派生类使用    
protected:
    std::string _channel_name;
    LogLevel _level;
}


#endif
