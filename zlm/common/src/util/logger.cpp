#include <iostream>
#include <memory>
#include <sys/time.h>
#include <stdarg.h>
#include <sys/types.h>
#include <dirent.h>
#include <cstring>
#include <functional>
#include "util.h"
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
        sem_wait(&_sem);

        flushAll();
    }
}

AsyncLogWriter::AsyncLogWriter() :  _exit_flag(false)
{
     sem_init(&_sem, 0, 0);
    _thread = std::make_shared<thread>([this]() { this->run(); });
}

AsyncLogWriter::~AsyncLogWriter()
{
    _exit_flag = true;
    sem_post(&_sem);
    _thread->join();
}

void AsyncLogWriter::write(const LogContextPtr &ctx, Logger &logger)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _pending.push_back(std::make_pair(ctx, &logger));
    sem_post(&_sem);
}

void AsyncLogWriter::flushAll()
{
    decltype (_pending) tmp = {0};
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _pending.swap(tmp);
    }

    tmp.for_each([this](const std::pair<LogContextPtr,Logger *> &item) {
        item.second->writeChannels(item.first);
    });

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

void Logger::setWriter(const std::shared_ptr<LogWriter> &writer)
{
    _writer = writer;
}

void Logger::write(const LogContextPtr &ctx)
{
    if(_writer)
    {
        _writer->write(ctx, *this);
    }
    else
    {
        writeChannels(ctx);
    }
}

void Logger::writeChannels_l(const LogContextPtr &ctx)
{
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

void Logger::writeChannels(const LogContextPtr &ctx)
{
    writeChannels_l(ctx);
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

std::string LogChannel::printTime(const timeval & tv)
{
    auto tm = getLocalTime(tv.tv_sec);
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

    ost << printTime(ctx->_tv) << " " << LOG_CONST_TABLE[ctx->_level][2] << " ";

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

FileChannelBase::FileChannelBase(const std::string &name, const std::string &path, LogLevel level)
    : LogChannel(name, level), _path(path)
{
    //std::cout << "FileChannelBase" << std::endl;

}

FileChannelBase::~FileChannelBase()
{
    close();
}

void FileChannelBase::write(const Logger &logger,const LogContextPtr &ctx)
{
    if(_level > ctx->_level)
    {
        return;
    }
    if(!_fstream.is_open())
    {
        if(!open())
        {
            return;
        }
    }

    format(logger, _fstream, ctx, false);
}

bool FileChannelBase::setPath(const std::string &path)
{
    _path = path;
    return open();
}


const std::string &FileChannelBase::path() const
{
    return _path;   
}

bool FileChannelBase::open()
{
    if(_path.empty())
    {
        throw std::runtime_error("path is empty");
    }

    _fstream.close();

    _fstream.open(_path, ios::out | ios::app);
    if(!_fstream.is_open())
    {
        std::cerr << "open file error" << std::endl;
        return false;
    }
    return true;
}

void FileChannelBase::close()
{
    if(_fstream.is_open())
    {
        _fstream.close();
    }
}

size_t FileChannelBase::size()
{
    return (_fstream << std::flush).tellp();
}

/*****************FileChannel******************************** */
//根据unix 时间戳生产日志文件名
static string _getLogFilePath(const string &dir,time_t sec,size_t index)
{
    auto tm = localtime(&sec);
    char buf[64] = {0};
    snprintf(buf, sizeof(buf), "%04d-%02d-%02d_%02ld.log",
            1900 + tm->tm_year,
            1 + tm->tm_mon,
            tm->tm_mday,
            index);
    return dir + buf;
}

static const char * _get_file_name(const char *file)
{
    auto pos = strrchr(file, '/');
    return pos ? pos + 1 : file;
}
//根据日志文件返回时间戳
static time_t _getLogFileTime(const string &full_path)
{
    printf("get log file time: %s\n", full_path.data());
    auto name = _get_file_name(full_path.data());
    struct tm tm{0};
    if(sscanf(name, "%04d-%02d-%02d.log",
            &tm.tm_year,
            &tm.tm_mon,
            &tm.tm_mday) != 3)
    {
        return 0;
    }
    tm.tm_year -= 1900;
    tm.tm_mon -= 1;
    return mktime(&tm);

}

static bool is_dir(const std::string &path) {
    auto dir = opendir(path.data());
    if (!dir) {
        return false;
    }
    closedir(dir);
    return true;
}

static void _scanDir(const std::string &dir,const function <bool (const string &path,bool is_dir)> &cb,
                        bool enter_subdir = false,bool show_hidden_file = false)
{
    string path = dir;
    if(path.back() != '/')
    {
        path += '/';
    }
    DIR *dirp ;
    dirent *pDirent;
    if((dirp = opendir(path.c_str())) == nullptr)
    {
        return;
    }
    while((pDirent = readdir(dirp)) != nullptr)
    {
        if (!show_hidden_file && pDirent->d_name[0] == '.') {
            //隐藏的文件  [AUTO-TRANSLATED:3b2eb642]
            //Hidden file
            continue;
        }
        string strAbsolutePath = path + "/" + pDirent->d_name;
        bool isDir = is_dir(strAbsolutePath);
        if (!cb(strAbsolutePath, isDir)) {
            //不再继续扫描  [AUTO-TRANSLATED:991bdb3f]
            //Stop scanning
            break;
        }
        if (isDir && enter_subdir)
        {
            //如果是文件夹并且扫描子文件夹，那么递归扫描  [AUTO-TRANSLATED:36773722]
            //If it's a folder and scanning subfolders, then recursively scan
            _scanDir(strAbsolutePath, cb, enter_subdir);
        }

    }
}

FileChannel::FileChannel(const std::string &name, const std::string &dir, LogLevel level)
    : FileChannelBase(name, "", level)
{
    _dir = dir;
    if(_dir.back() != '/')
    {
        _dir += '/';
    }

    //获取 dir下 所有的日志文件
    _scanDir(_dir,[this](const string &path,bool is_dir) -> bool{
        if(!is_dir && strstr(path.data(), ".log"))
        {
            _log_file_map.emplace(path);
        }
        return true;
    });

    //获取今天日志文件的最大index号
    auto log_name_prefix = getTimeStr("%Y-%m-%d_", time(nullptr));
    for(auto it = _log_file_map.begin(); it != _log_file_map.end(); ++it)
    {
        auto name = _get_file_name(it->data());
        //筛选出今天所有的日志文件
        if(strstr(name, log_name_prefix.data()))
        {
            int index = 0;
            if(sscanf(name ,"%*[^_]%d.log", &index) == 1)
            {
                _index = std::max(_index, (size_t)index);
            }
        }
    }
    std::cout << "log index:" << _index << std::endl;
}

void FileChannel::write(const Logger &logger,const LogContextPtr &ctx)
{
    time_t sec = ctx->_tv.tv_sec;
    //日志所在的天 localtime(&sec);
    auto tm = getLocalTime(sec);
    if(tm.tm_year != _last_tm.tm_year ||
       tm.tm_mon != _last_tm.tm_mon ||
       tm.tm_mday != _last_tm.tm_mday)
    {
        //如果是新的一天，那么重新生成日志文件
        _index = 0;
        _last_tm = tm;
        changeFile(sec);
    }
    else
    {
        //检查日志文件大小
        checkSize(sec);
    }
    if(_can_write)
    {
        FileChannelBase::write(logger, ctx);
    }
    
}

void FileChannel::clean()
{
    //删除过期的日志文件
    auto now = time(nullptr);
    auto log_name_prefix = getTimeStr("%Y-%m-%d_",now);
    for(auto it = _log_file_map.begin(); it != _log_file_map.end();it++)
    {
        auto file_time = _getLogFileTime(it->data());
        if(file_time > 0 && (now - file_time) > _log_max_day * 24 * 3600)
        {
            //删除过期的日志文件
            remove(it->data());
            it = _log_file_map.erase(it);
        }
    }

    //删除日志文件数量超过限制的日志文件
    while(_log_file_map.size() > _log_max_count)
    {
        //删除最早的日志文件
        auto it = _log_file_map.begin();
        if(*it == path())
            break;
        if(it != _log_file_map.end())
        {
            remove(it->data());
            _log_file_map.erase(it);
        }
    }
}

void FileChannel::checkSize(time_t sec)
{
    //1min 检查一次 
    if(sec - _last_check_time < 60)
    {
        return;
    }
    if(FileChannelBase::size() > _log_max_size)
    {
        //如果日志文件大小超过限制，那么切换到新的日志文件
        changeFile(sec);
    }

    _last_check_time = sec;
}

void FileChannel::changeFile(time_t sec)
{
    auto log_file = _getLogFilePath(_dir, sec, _index++);
    _log_file_map.emplace(log_file);
    _can_write = setPath(log_file);
    if(!_can_write)
    {
        std::cerr << "change file error: " << log_file << std::endl;
        return;
    }
    
    clean();
}

void FileChannel::setMaxDay(size_t max_day) 
{
    _log_max_day = max_day > 1 ? max_day : 1;
}

void FileChannel::setFileMaxSize(size_t max_size) 
{
    _log_max_size = max_size > 1 ? max_size : 1;
}

void FileChannel::setFileMaxCount(size_t max_count) 
{
    _log_max_count = max_count > 1 ? max_count : 1;
}

}

