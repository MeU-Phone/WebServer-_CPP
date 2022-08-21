#ifndef LOG_H
#define LOG_H

#include <mutex>
#include <string>
#include <thread>
#include <sys/time.h>
#include <string.h>
#include <stdarg.h>           // vastart va_end
#include <assert.h>
#include <sys/stat.h>         //mkdir
#include "blockqueue.h"
#include "../buffer/buffer.h"


#define LOG_BASE(level, format, ...) \
    do {\
        Log* log = Log::Instance();\
        if (log->IsOpen()/* && log->GetLevel() <= level*/) {\
            log->write(level, format, ##__VA_ARGS__); \
            log->flush();\
        }\
    } while(0);

#define LOG_DEBUG(format, ...) LOG_BASE(0, format, ##__VA_ARGS__);
#define LOG_INFO(format, ...) LOG_BASE(1, format, ##__VA_ARGS__);
#define LOG_WARN(format, ...) LOG_BASE(2, format, ##__VA_ARGS__);
#define LOG_ERROR(format, ...) LOG_BASE(3, format, ##__VA_ARGS__);



class Log{ 
public:
    void init(int level, const char* path = "./log",
                const char* suffix = "./log",
                int maxQueueCapacity = 1024);

    //*以静态方法返回实例
    static Log* Instance();
    static void FlushLogThread();

    void write(int level, const char* format, ...);
    void flush();

    int GetLevel();
    void SetLevel(int level);
    bool IsOpen() { return isOpen_; }

private:
    //*构造方法私有
    Log();
    
    void AppendLogLevelTitle_(int level);
    virtual ~Log();
    void AsyncWrite_();

private:
    static const int LOG_PATH_LEN = 256;
    static const int LOG_NAME_LEN = 256;
    static const int MAX_LINES = 50000;

    //存储日志的路径
    const char* path_;
    //日志的文件名
    const char* suffix_;

    int MAX_LINES_;

    int lineCount_;
    int toDay_;

    bool isOpen_;
 
    Buffer buff_;
    int level_;
    bool isAsync_;

    FILE* fp_;
    std::unique_ptr<BlockDeque<std::string>> deque_; 
    std::unique_ptr<std::thread> writeThread_;
    std::mutex mtx_;
};


#endif