#ifndef __SYLAR_H
#define __SYLAR_H
#include<string>
#include<stdint.h>
#include<memory>
#include<list>
#include<fstream>
#include<iostream>
#include<vector>
#include<sstream>
#include<cassert>
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
    static const char* toString(LogLevel::Level level);
};


// 日志事件
class LogEvent{
public:
    typedef std::shared_ptr<LogEvent> ptr;
    LogEvent(){}
    const char* getFile()const{return m_file;}
    uint32_t getLine()const{return m_line;}
    uint32_t getThreadId()const{return m_threadId;}
    uint32_t getFiberId()const{return m_fiberID;}
    uint32_t getTime()const{return m_time;}
    uint32_t getElapse()const{return m_elapse;}
    std::string getContent()const{return m_content;}
private:
    const char* m_file = nullptr;  // 文件名
    uint32_t m_line = 0;         // 行号
    uint32_t m_threadId = 0;     // 线程号
    uint32_t m_fiberID = 0;      // 协程号
    uint64_t m_time;             // 时间戳
    uint32_t m_elapse = 20;       // 程序启动到现在的毫秒数
    std::string m_content="hello,log";       //内容
};

// 日志格式 
class LogFormatter{
public:
    typedef std::shared_ptr<LogFormatter> ptr;
    struct pattern_struct{ // 每个输入字符串的格式信息
        std::string str;
        std::string format;
        int type;  // 0:error  1：normal
    };
   
    // %t %m%n%threadId
    std::string format(LogLevel::Level level,LogEvent::ptr event);
    LogFormatter(std::string pattern);
    std::vector<pattern_struct> init();  // pattern 的解析
public:
    class FormatItem{
    public:
        typedef std::shared_ptr<FormatItem> ptr;
        virtual ~FormatItem(){};
        virtual void format(std::ostream& os,LogLevel::Level level,LogEvent::ptr event)=0;
    };
   
private:
    std::string m_pattern;
    std::vector<FormatItem::ptr> m_item;
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