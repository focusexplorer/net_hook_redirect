//2015-12-21 by focuszhang

#ifndef API_LOG_H_
#define API_LOG_H_

namespace ns_log
{

enum LogLevel
{
	CS_LOG_DEBUG,
	CS_LOG_ERROR,
	CS_LOG_NO_LOG,
};

#ifdef linux
class MLogInterface
{
public:
	virtual ~MLogInterface(){}
	virtual void log(LogLevel log_level,const char* file,int line,const char* func,const char* format,...) __attribute__((format(printf,6,7)))=0;
    virtual bool is_priority_enabled(LogLevel level)=0;
};
#endif

#if defined(_WIN32) || defined(_WIN64)
class MLogInterface
{
public:
	virtual ~MLogInterface(){}
	virtual void log(LogLevel log_level,const char* file,int line,const char* func,const char* format,...)=0;
    virtual bool is_priority_enabled(LogLevel level)=0;
};
#endif
}



#endif /* API_LOG_H_ */



