/*
 * 仿写 ZLToolKit 的日志模块，仅用于学习 C++。
 * 本文件仿写自 src/Util/logger.h，实现了一个简化版的日志框架。
 */

#ifndef MY_LOGGER_H_
#define MY_LOGGER_H_

#include <cstdarg>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>
#include <sys/time.h>
#include <thread>

// 仿写 noncopyable 基类，禁止拷贝构造、移动构造、拷贝赋值、移动赋值
class noncopyable {
protected:
    noncopyable() {}
    ~noncopyable() {}
private:
    noncopyable(const noncopyable &) = delete;
    noncopyable(noncopyable &&) = delete;
    noncopyable &operator=(const noncopyable &) = delete;
    noncopyable &operator=(noncopyable &&) = delete;
};

// 日志级别枚举
enum LogLevel { LTrace = 0, LDebug, LInfo, LWarn, LError };

// 日志上下文类，存储日志的详细信息（级别、文件、函数、行号、时间、内容等）
class LogContext : public std::ostringstream {
public:
    LogContext() = default;
    LogContext(LogLevel level, const char *file, const char *function, int line, const char *module_name, const char *flag)
        : _level(level), _line(line), _file(file), _function(function), _module_name(module_name), _flag(flag) {
        gettimeofday(&_tv, nullptr);
        _thread_name = std::to_string(std::hash<std::thread::id>()(std::this_thread::get_id()));
    }
    ~LogContext() = default;

    LogLevel _level;
    int _line;
    int _repeat = 0;
    std::string _file;
    std::string _function;
    std::string _thread_name;
    std::string _module_name;
    std::string _flag;
    struct timeval _tv;

    const std::string &str() {
        if (_got_content) {
            return _content;
        }
        _content = std::ostringstream::str();
        _got_content = true;
        return _content;
    }

private:
    bool _got_content = false;
    std::string _content;
};

using LogContextPtr = std::shared_ptr<LogContext>;

// 日志通道抽象基类，负责将日志输出到不同目标（如控制台、文件、系统日志等）
class LogChannel : public noncopyable {
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

// 控制台日志通道，输出日志到标准输出（stdout）
class ConsoleChannel : public LogChannel {
public:
    ConsoleChannel(const std::string &name, LogLevel level) : LogChannel(name, level) {}
    void write(const LogContextPtr &ctx) override {
        std::cout << ctx->str() << std::endl;
    }
};

// 日志写入器抽象基类，负责将日志写入 Logger（例如异步写入）
class LogWriter : public noncopyable {
public:
    LogWriter() = default;
    virtual ~LogWriter() = default;
    virtual void write(const LogContextPtr &ctx) = 0;
};

// 异步日志写入器，内部持有一个线程，异步将日志写入 Logger
class AsyncLogWriter : public LogWriter {
public:
    AsyncLogWriter() : _exit_flag(false) {
        _thread = std::make_shared<std::thread>(&AsyncLogWriter::run, this);
    }
    ~AsyncLogWriter() {
        _exit_flag = true;
        if (_thread && _thread->joinable()) {
            _thread->join();
        }
    }
    void write(const LogContextPtr &ctx) override {
        std::lock_guard<std::mutex> lck(_mutex);
        _pending.push_back(ctx);
    }

private:
    void run() {
        while (!_exit_flag) {
            decltype(_pending) tmp;
            {
                std::lock_guard<std::mutex> lck(_mutex);
                if (_pending.empty()) {
                    continue;
                }
                tmp.swap(_pending);
            }
            for (auto &ctx : tmp) {
                // 这里可以调用 Logger 的 writeChannels 分发日志，仿写中简化处理，直接输出到控制台
                std::cout << "AsyncLogWriter: " << ctx->str() << std::endl;
            }
        }
    }

private:
    bool _exit_flag;
    std::mutex _mutex;
    std::shared_ptr<std::thread> _thread;
    std::vector<LogContextPtr> _pending;
};

// 日志上下文捕获器，捕获日志上下文并调用 Logger::write 将日志传入 Logger
class LogContextCapture {
public:
    LogContextCapture(LogLevel level, const char *file, const char *function, int line, const char *module_name, const char *flag = "")
        : _ctx(std::make_shared<LogContext>(level, file, function, line, module_name, flag)) {}
    ~LogContextCapture() {
        if (_ctx) {
            // 仿写中，直接输出到控制台，实际中应调用 Logger::write(_ctx)
            std::cout << "LogContextCapture: " << _ctx->str() << std::endl;
        }
    }
    template<typename T>
    LogContextCapture &operator<<(T &&data) {
        if (_ctx) {
            (*_ctx) << std::forward<T>(data);
        }
        return *this;
    }

private:
    LogContextPtr _ctx;
};

// 日志主体类，管理日志通道、日志写入器，负责分发日志
class Logger : public std::enable_shared_from_this<Logger>, public noncopyable {
public:
    static Logger &Instance() {
        static Logger instance("my_logger");
        return instance;
    }

    explicit Logger(const std::string &loggerName) : _logger_name(loggerName), _default_channel(std::make_shared<ConsoleChannel>("default", LTrace)) {}
    ~Logger() { _writer.reset(); _channels.clear(); }

    void add(const std::shared_ptr<LogChannel> &channel) { _channels[channel->name()] = channel; }
    void del(const std::string &name) { _channels.erase(name); }
    std::shared_ptr<LogChannel> get(const std::string &name) {
        auto it = _channels.find(name);
        return (it != _channels.end()) ? it->second : nullptr;
    }
    void setWriter(const std::shared_ptr<LogWriter> &writer) { _writer = writer; }
    void setLevel(LogLevel level) { for (auto &chn : _channels) { chn.second->setLevel(level); } }
    const std::string &getName() const { return _logger_name; }

    void write(const LogContextPtr &ctx) {
        if (_writer) {
            _writer->write(ctx);
        } else {
            writeChannels(ctx);
        }
    }

private:
    void writeChannels(const LogContextPtr &ctx) {
        if (_channels.empty()) {
            _default_channel->write(ctx);
        } else {
            for (auto &chn : _channels) {
                chn.second->write(ctx);
            }
        }
    }

private:
    std::string _logger_name;
    std::shared_ptr<LogWriter> _writer;
    std::shared_ptr<LogChannel> _default_channel;
    std::map<std::string, std::shared_ptr<LogChannel>> _channels;
};

// 仿写日志宏，方便调用日志输出
#define MY_LOG(level, ...) do { LogContextCapture(level, __FILE__, __FUNCTION__, __LINE__, "", ""); } while(0)
#define MY_LOG_TRACE(...) MY_LOG(LTrace, __VA_ARGS__)
#define MY_LOG_DEBUG(...) MY_LOG(LDebug, __VA_ARGS__)
#define MY_LOG_INFO(...) MY_LOG(LInfo, __VA_ARGS__)
#define MY_LOG_WARN(...) MY_LOG(LWarn, __VA_ARGS__)
#define MY_LOG_ERROR(...) MY_LOG(LError, __VA_ARGS__)

#endif /* MY_LOGGER_H_ */ 
