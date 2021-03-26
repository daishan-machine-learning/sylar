#include"log.h"
namespace sylar{
    // --------------------------Logger define-----------------------------------
Logger::Logger(const std::string name="root")
    :m_name(name){

}
void Logger::log(LogLevel::Level level,LogEvent::ptr event){
    if(level >= m_level){
        for(auto& i:m_apds){
            i->log(level,event);
        }
    }
}
    // 各种级别的输出
void Logger::debug(LogEvent::ptr event){
    log(LogLevel::Level::DEBUG,event);
}
void Logger::info(LogEvent::ptr event){
    log(LogLevel::Level::INFO,event);
}
void Logger::Logger::warn(LogEvent::ptr event){
    log(LogLevel::Level::WARN,event);
}
void Logger::fatal(LogEvent::ptr event){
    log(LogLevel::Level::FATAL,event);
}
void Logger::error(LogEvent::ptr event){
    log(LogLevel::Level::ERROR,event);
}

void Logger::addAppender(LogAppender::ptr appender){
    m_apds.insert(m_apds.begin(),appender);

}
void Logger::deleteAppender(LogAppender::ptr appender){
    for(auto it = m_apds.begin();it != m_apds.end();++it){
        if(*it == appender){
            m_apds.erase(it);
            break;
        }
    }
}

// ----------------------------------LogAppender define------------------------------------

//StdoutLogAppender
void StdoutLogAppender::log(LogLevel::Level level,LogEvent::ptr event){
    if(level >= m_level){
        std::cout << m_formatter->format(event)<<std::endl;
    }
}

// FileLogAppender
FileLogAppender::FileLogAppender(std::string filepath)
    :m_filepath(filepath){
    m_out.open(filepath);
}

bool FileLogAppender::fileReopen(){
    if(m_out.is_open()){
        m_out.close();
    }
    m_out.open(m_filepath);
    return !!m_out;
}
void FileLogAppender::log(LogLevel::Level level,LogEvent::ptr event){
    if(level >= m_level){
        m_out<<m_formatter->format(event);
    }
}

// -----------------------------------LogFormatter-------------------------------


}