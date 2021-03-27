#include"log.h"
namespace sylar{
    // -------------------------LogLevel----------------------------------------
    const char* LogLevel::toString(LogLevel::Level level){
        switch(level){
#define XX(name) \
            case LogLevel::Level::name:\
                return #name;\
            break
      
        XX(DEBUG);
        XX(ERROR);
        XX(INFO);
        XX(FATAL);
        XX(WARN);
#undef XX
        default:
            return "UNKNOWN";
        }
        
    }

    // --------------------------Logger define-----------------------------------

Logger::Logger(const std::string name)
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
        std::cout << m_formatter->format(level,event)<<std::endl;
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
        m_out<<m_formatter->format(level,event);
    }
}

// -----------------------------------LogFormatter-------------------------------


std::vector<LogFormatter::pattern_struct> LogFormatter::init(){
    std::vector<pattern_struct> vec;
    std::string prefix ,str,format;
    // xxx(0) %xxx(1) %xxx{xxx}(2) %% 只有这三种是合法的
    int status =0; // 0:prefix  1:str  2:format
    for(int i=0;i<m_pattern.size();++i){
        if(!isspace(m_pattern[i])){
            if(status == 0){
                if(m_pattern[i] == '%'){
                    status = 1;
                    if(!prefix.empty()){
                        vec.push_back({prefix,"",0});
                        prefix ="";
                    }

                    continue;
                }
                prefix.append(1,m_pattern[i]);
                continue;
            }
            if(status == 1){
                if(m_pattern[i] == '%'){
                    if(str.empty()){
                        str.append(1,m_pattern[i]);
                        continue;
                    }
                    vec.push_back({str,"",1});
                    str = "";
                    status =0;
                    continue;
                }
                if(m_pattern[i] == '{'){
                    status =2;
                    continue;
                }
                str.append(1,m_pattern[i]);
                continue;
            }

            if(status == 2){
                if(m_pattern[i] == '}'){
                    vec.push_back({str,format,1});
                    str = "";
                    format ="";
                    status =0;
                    continue;
                }
                assert(!(m_pattern[i] == '%') && !(m_pattern[i]=='{'));
                format.append(1,m_pattern[i]);
            }
        }else{
            assert(status != 2);
            if(!prefix.empty()){
                vec.push_back({prefix,"",0}); // 0 :message
                prefix ="";
            }
            if(!str.empty()){
                vec.push_back({str,format,1});  // normal 
                str = "";
                prefix = "";
            }
            
            status =0;

        }
    }
    assert( status != 2);
    if(!prefix.empty())
        vec.push_back({prefix,"",0});
    if(!str.empty())
        vec.push_back({str,"",1});
    // %m ---消息体
    // %p ---level
    // %r --- 启动后的时间
    // %c --- 日志名称
    // %t --- 线程id
    // %n --- 回车换行
    // %d --- 时间
    // %f ---文件名
    // %l --- 行号
    return std::move(vec);
}



std::string LogFormatter::format(LogLevel::Level level,LogEvent::ptr event){
    std::stringstream ss;
    for(auto i:m_item){
        i->format(ss,level,event);
    }
    return ss.str();
}
// 日志的各种格式的格式对象
class MessageFormatItem:public LogFormatter::FormatItem{
public:
    virtual void format(std::ostream& os,LogLevel::Level level,LogEvent::ptr event){
        os<<event->getContent();
    }
};

class LevelFormatItem:public LogFormatter::FormatItem{
public:
    virtual void format(std::ostream& os,LogLevel::Level level,LogEvent::ptr event){
        os<<"["<<LogLevel::toString(level)<<"]:";
    }
};

class ElapseFormatItem:public LogFormatter::FormatItem{
public:
    virtual void format(std::ostream& os,LogLevel::Level level,LogEvent::ptr event){
        os<<event->getElapse()<<"ms ";
    }
};


LogFormatter::LogFormatter(std::string pattern):m_pattern(pattern){
    std::vector<pattern_struct> vec = init();
    for(auto item:vec){
        if(item.type == 0){
            m_item.push_back(FormatItem::ptr(new MessageFormatItem()));
        }else{
            if(item.str == "p")
                m_item.push_back(FormatItem::ptr(new LevelFormatItem()));
            if(item.str == "t")
                m_item.push_back(FormatItem::ptr(new ElapseFormatItem()));
        }
    }
   
}
}