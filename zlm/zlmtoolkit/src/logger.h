#ifndef __UTIL_LOGGER_H__
#define __UTIL_LOGGER_H__




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


#endif
