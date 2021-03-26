#ifndef __SYLAR_H
#define __SYLAR_H
#include<string>
#include<stdint.h>
#include<memory>
#include<list>
#include<string>
#include<fstream>
#include<iostream>
namespace sylar{
// 日志级别
class LogLevel{
public:
      enum class Level{
        DEBUG=1,
        INFO ,
        WARN,
        ERROR,
        FATAL
    };
};


// 日志事件
class LogEvent{
public:
    typedef std::shared_ptr<LogEvent> ptr;
    LogEvent();
private:
    const char* m_file = nullptr;  // 文件名
    uint32_t m_line = 0;         // 行号
    uint32_t m_threadId = 0;     // 线程号
    uint32_t m_fiberID = 0;      // 协程号
    uint64_t m_time;             // 时间戳
    uint32_t m_elapse = 0;       // 程序启动到现在的毫秒数
    std::string m_content;       //内容
};

// 日志格式 
class LogFormatter{
public:
    typedef std::shared_ptr<LogFormatter> ptr;
    std::string format(LogEvent::ptr event);
};



// 日志输出地
class LogAppender{
public:
    typedef std::shared_ptr<LogAppender> ptr;
    virtual ~LogAppender(){

    }
    virtual void log(LogLevel::Level level,LogEvent::ptr event) = 0;
    void setFormatter(LogFormatter::ptr formatter){m_formatter = formatter;}
    LogFormatter::ptr getFormatter()const{return m_formatter;}
protected:
    LogLevel::Level m_level;
    LogFormatter::ptr m_formatter;
};

// 日志输出器
class Logger{
public:
    typedef std::shared_ptr<Logger> ptr;
  
    Logger(const std::string name="root");
    void log(LogLevel::Level level,LogEvent::ptr event);  // 使用智能指针管理event
    // 各种级别的输出
    void debug(LogEvent::ptr event);
    void info(LogEvent::ptr event);
    void warn(LogEvent::ptr event);
    void fatal(LogEvent::ptr event);
    void error(LogEvent::ptr event);

    void addAppender(LogAppender::ptr appender);
    void deleteAppender(LogAppender::ptr appender);

    LogLevel::Level getLevel()const{return m_level;}
    void setLevel(LogLevel::Level level){ m_level = level;}
private:
    std::list<LogAppender::ptr> m_apds;  // 输出位置的列表
    std::string m_name;    // Logger日志名称
    LogLevel::Level m_level; // Logger日志级别
};





// 输出到控制台的Appender
class StdoutLogAppender:public LogAppender{
public:
    typedef std::shared_ptr<StdoutLogAppender> ptr;
    void log(LogLevel::Level level,LogEvent::ptr event) override;
};

// 输出到文件的Appender
class FileLogAppender:public LogAppender{
public:
    typedef std::shared_ptr<FileLogAppender> ptr;
    FileLogAppender(std::string filepath);
    void log(LogLevel::Level level,LogEvent::ptr event) override;
    bool fileReopen();
private:
    std::string m_filepath; 
    std::ofstream m_out;
};




}

#endif