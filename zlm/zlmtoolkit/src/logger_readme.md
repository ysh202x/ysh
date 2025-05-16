@startuml
namespace toolkit {
    class Logger {
        - LogContextPtr _last_log
        - std::string _logger_name
        - std::shared_ptr<LogWriter> _writer
        - std::shared_ptr<LogChannel> _default_channel
        - std::map<std::string, std::shared_ptr<LogChannel>> _channels
        + static Logger& Instance()
        + void add(const std::shared_ptr<LogChannel>& channel)
        + void del(const std::string& name)
        + std::shared_ptr<LogChannel> get(const std::string& name)
        + void setWriter(const std::shared_ptr<LogWriter>& writer)
        + void setLevel(LogLevel level)
        + const std::string& getName() const
        + void write(const LogContextPtr& ctx)
    }

    class LogContext {
        - LogLevel _level
        - int _line
        - int _repeat
        - std::string _file
        - std::string _function
        - std::string _thread_name
        - std::string _module_name
        - std::string _flag
        - struct timeval _tv
        - std::string _content
        + const std::string& str()
    }

    class LogContextCapture {
        - LogContextPtr _ctx
        - Logger& _logger
        + LogContextCapture(Logger& logger, LogLevel level, const char* file, const char* function, int line, const char* flag = "")
        + LogContextCapture(const LogContextCapture& that)
        + ~LogContextCapture()
        + LogContextCapture& operator<<(std::ostream& (*f)(std::ostream&))
        + template<typename T> LogContextCapture& operator<<(T&& data)
        + void clear()
    }

    class LogWriter {
        + virtual void write(const LogContextPtr& ctx, Logger& logger) = 0
    }

    class AsyncLogWriter {
        - bool _exit_flag
        - semaphore _sem
        - std::mutex _mutex
        - std::shared_ptr<std::thread> _thread
        - List<std::pair<LogContextPtr, Logger*>> _pending
        + void run()
        + void flushAll()
        + void write(const LogContextPtr& ctx, Logger& logger)
    }

    class LogChannel {
        - std::string _name
        - LogLevel _level
        + virtual void write(const Logger& logger, const LogContextPtr& ctx) = 0
        + const std::string& name() const
        + void setLevel(LogLevel level)
        + static std::string printTime(const timeval& tv)
    }

    class EventChannel {
        + void write(const Logger& logger, const LogContextPtr& ctx)
    }

    class ConsoleChannel {
        + void write(const Logger& logger, const LogContextPtr& logContext)
    }

    class FileChannelBase {
        - std::string _path
        - std::ofstream _fstream
        + void write(const Logger& logger, const LogContextPtr& ctx)
        + bool setPath(const std::string& path)
        + const std::string& path() const
    }

    class FileChannel {
        - size_t _log_max_day
        - size_t _log_max_size
        - size_t _log_max_count
        - size_t _index
        - int64_t _last_day
        - time_t _last_check_time
        - std::string _dir
        - std::set<std::string> _log_file_map
        + void clean()
        + void checkSize(time_t second)
        + void changeFile(time_t second)
    }

    Logger --> LogContext
    Logger --> LogWriter
    Logger --> LogChannel
    LogContextCapture --> Logger
    LogContextCapture --> LogContext
    AsyncLogWriter --> LogWriter
    EventChannel --> LogChannel
    ConsoleChannel --> LogChannel
    FileChannelBase --> LogChannel
    FileChannel --> FileChannelBase
}
@enduml
下面是一个基于 ZLToolKit 中 logger.h 的结构图，描述了日志模块中各个类及其继承、组合关系：
─────────────────────────────────────────────────────────────────────────────
【Logger 模块结构图】
─────────────────────────────────────────────────────────────────────────────
【Logger】
│
│ Logger 是日志模块的主体，它继承自 std::enable_shared_from_this<Logger> 和 noncopyable，
│ 内部持有：
│ – last_log（LogContextPtr，记录上一条日志，用于去重）
│ – logger_name（std::string，日志器名称）
│ – writer（std::shared_ptr<LogWriter>，自定义日志写入器，可选）
│ – default_channel（std::shared_ptr<LogChannel>，默认日志通道，例如 ConsoleChannel）
│ – channels（std::map<std::string, std::shared_ptr<LogChannel>>，管理多个日志通道）
│
│ Logger 提供以下主要接口：
│ – static Logger &Instance()：获取单例
│ – void add(const std::shared_ptr<LogChannel> &channel)：添加日志通道
│ – void del(const std::string &name)：删除日志通道
│ – std::shared_ptr<LogChannel> get(const std::string &name)：获取日志通道
│ – void setWriter(const std::shared_ptr<LogWriter> &writer)：设置日志写入器
│ – void setLevel(LogLevel level)：设置所有日志通道的日志级别
│ – void write(const LogContextPtr &ctx)：写日志（内部调用 writer->write 或 writeChannels）
│ – void writeChannels(const LogContextPtr &ctx)：分发日志到各通道（内部调用 writeChannels_l）
│ – void writeChannels_l(const LogContextPtr &ctx)：遍历 channels 或调用 _default_channel 输出日志
│
─────────────────────────────────────────────────────────────────────────────
【LogContext】
│
│ LogContext 是日志上下文，继承自 std::ostringstream，用于存储日志的详细信息：
│ – level（LogLevel，日志级别）
│ – line（int，日志行号）
│ – repeat（int，日志重复计数，用于去重）
│ – file（std::string，日志文件）
│ – function（std::string，日志函数名）
│ – thread_name（std::string，线程名）
│ – module_name（std::string，模块名）
│ – flag（std::string，日志标记）
│ – tv（struct timeval，日志时间）
│ – content（std::string，日志内容，由 str() 方法生成）
│
─────────────────────────────────────────────────────────────────────────────
【LogContextCapture】
│
│ LogContextCapture 是日志上下文捕获器，用于捕获日志上下文（LogContext），
│ 内部持有：
│ – ctx（LogContextPtr，日志上下文）
│ – logger（Logger &，日志器引用）
│
│ 在构造时，会构造一个 LogContext 对象，并调用 Logger::write(ctx) 将日志传入 Logger。
│
─────────────────────────────────────────────────────────────────────────────
【LogWriter】
│
│ LogWriter 是日志写入器的抽象基类，继承自 noncopyable，提供纯虚函数：
│ – virtual void write(const LogContextPtr &ctx, Logger &logger) = 0
│
│ AsyncLogWriter 是 LogWriter 的一个子类，内部持有一个线程，异步地将日志写入 Logger。
│
─────────────────────────────────────────────────────────────────────────────
【LogChannel】
│
│ LogChannel 是日志通道的抽象基类，继承自 noncopyable，提供纯虚函数：
│ – virtual void write(const Logger &logger, const LogContextPtr &ctx) = 0
│
│ 内部持有：
│ – name（std::string，通道名称）
│ – level（LogLevel，日志级别）
│
│ 子类包括：
│ – ConsoleChannel：输出日志到控制台
│ – FileChannel：输出日志到文件（内部调用 FileChannelBase，支持文件轮转、大小限制等）
│ – SysLogChannel：输出日志到系统日志（例如 Linux syslog）
│ – EventChannel：输出日志到广播（通过 NoticeCenter 分发）
│
─────────────────────────────────────────────────────────────────────────────
【总结】
─────────────────────────────────────────────────────────────────────────────
Logger 模块主要由 Logger、LogContext、LogContextCapture、LogWriter 和 LogChannel 组成。
– Logger 是日志主体，管理日志通道和日志写入器，负责分发日志。
– LogContext 存储日志的详细信息（级别、文件、函数、行号、时间、内容等）。
– LogContextCapture 捕获日志上下文，并调用 Logger::write 将日志传入 Logger。
– LogWriter 是日志写入器的抽象基类，AsyncLogWriter 是其异步实现。
– LogChannel 是日志通道的抽象基类，ConsoleChannel、FileChannel、SysLogChannel、EventChannel 是其子类，负责将日志输出到不同目标。
─────────────────────────────────────────────────────────────────────────────
（结构图结束）

# 1. 明确学习目标
    学习 C++ 的基础语法（类、继承、模板、智能指针等）。
    理解日志系统的设计思想（如日志上下文、日志通道、日志级别）。
    掌握多线程编程（如线程同步、锁机制）。
    学习如何设计模块化、可扩展的代码结构。
# 2. 分模块仿写
    将 logger 模块拆分为多个子模块，逐步实现：

## (1) 基础工具类
    仿写 noncopyable 类，禁止拷贝和赋值操作。
    理解 std::enable_shared_from_this 的用法，仿写一个简单的类来测试。
## (2) 日志上下文
    仿写 LogContext 类：
    学习如何继承 std::ostringstream。
    实现日志上下文的存储（如日志级别、文件名、函数名、行号等）。
    使用 gettimeofday 获取时间戳。
    使用 std::hash<std::thread::id> 获取线程 ID。
## (3) 日志通道
    仿写 LogChannel 抽象基类：
    定义日志输出接口 write。
    实现 ConsoleChannel，将日志输出到控制台。
    可扩展实现 FileChannel，将日志输出到文件。
## (4) 日志写入器
    仿写 LogWriter 抽象基类：
    定义日志写入接口。
    实现 AsyncLogWriter，学习如何使用线程和锁机制实现异步日志写入。
## (5) 日志捕获器
    仿写 LogContextCapture 类：
    实现流式日志输入（重载 operator<<）。
    在析构函数中触发日志写入。
## (6) 日志管理器
    仿写 Logger 类：
    管理日志通道和日志写入器。
    实现日志的分发逻辑。
    学习如何使用 std::map 管理多个日志通道。
# 3. 逐步优化
    添加日志级别过滤：在日志通道中根据日志级别决定是否输出。
    支持多种日志格式：如添加时间戳、线程 ID 等信息。
    实现日志切片：在文件日志中，按大小或时间切分日志文件。
    支持多线程安全：确保日志模块在多线程环境下正常工作。
# 4. 学习重点
    C++ 基础：
    智能指针（std::shared_ptr、std::unique_ptr）。
    模板编程（如 LogContextCapture 的模板方法）。
    流操作符重载（operator<<）。
    多线程编程：
    使用 std::thread 创建线程。
    使用 std::mutex 和 std::lock_guard 实现线程同步。
    设计模式：
    单例模式（Logger::Instance）。
    工厂模式（创建不同的日志通道）。
    策略模式（通过不同的日志通道实现不同的日志输出策略）。
# 5. 实践建议
    从简单到复杂：先实现一个简单的控制台日志系统，再逐步扩展功能。
    多写测试代码：为每个模块编写单元测试，验证其功能和线程安全性。
    阅读源码：对比你的实现和 ZLToolKit 的源码，找出差异并优化。
