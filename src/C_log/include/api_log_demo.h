//2015-12-21 by focuszhang

#ifndef API_LOG_DEMO_H_
#define API_LOG_DEMO_H_

#ifdef linux
#include <sys/time.h>
#elif defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

#include<stdarg.h>
#include<fstream>
#include<time.h>
#include"api_log.h"


namespace ns_log
{
class MLog:public MLogInterface
{
	std::ofstream error_file;
	std::ofstream debug_file;
	int debug_len;
	int error_len;
	int file_limit;
	LogLevel cur_log_level;
public:
	MLog(const char*log_file_name)
	{
		std::string t=log_file_name;
		error_file.open((t+".error").c_str(),std::ios::out);
		debug_file.open((t+".debug").c_str(),std::ios::out);
		debug_len=0;
		error_len=0;
		file_limit=100000000;
		cur_log_level=CS_LOG_DEBUG;
	}
	virtual ~MLog()
	{
		error_file.close();
		debug_file.close();
	}
	virtual void log(ns_log::LogLevel log_level,const char* file,int line,const char* func,const char* format,...)
	{
		char buf[10240];
		int len=snprintf(buf,sizeof(buf),"[%s](%s:%d)(%s)] [%s]: ",
				get_log_time(),
				file,
				line,
				func,
				log_level==ns_log::CS_LOG_DEBUG?"DEBUG":"ERROR"
				);

		va_list ap;
		va_start(ap,format);
		len=vsnprintf(buf+len,sizeof(buf)-len,format,ap)+len;
		va_end(ap);

		buf[len]='\n';len++;

		if(log_level==ns_log::CS_LOG_DEBUG)
		{
			debug_file.write(buf,len);
			debug_len+=len;
		}
		else
		{
			debug_file.write(buf,len);
			error_file.write(buf,len);
			debug_len+=len;
			error_len+=len;
		}
		debug_file.flush();
		error_file.flush();

		if(debug_len>file_limit)
		{
			debug_file.close();
			debug_file.open("log.debug",std::ios::out);
			debug_len=0;
		}
		if(error_len>file_limit)
		{
			error_file.close();
			error_file.open("log.error",std::ios::out);
			error_len=0;
			
		}
		return;
	}
    virtual bool is_priority_enabled(ns_log::LogLevel level)
    {
    	return level>=cur_log_level;
    }
#ifdef linux
    const char* get_log_time()
    {
    	time_t now=time(0);
    	struct timeval tv;
    	struct tm* tm=localtime(&now);
    	gettimeofday(&tv,0);
    	static char buf[100];
    	snprintf(buf,sizeof(buf),"%04d%02d%02d-%02d%02d%02d %06d",
    			tm->tm_year+1900,
    			tm->tm_mon+1,
    			tm->tm_mday,
    			tm->tm_hour,
    			tm->tm_min,
    			tm->tm_sec,
    			int(tv.tv_usec));
    	return buf;

    }
#elif defined(_WIN32) || defined(_WIN64)
    const char* get_log_time()
    {
    	time_t now=time(0);
    	struct tm* tm=localtime(&now);
    	static char buf[100];
    	snprintf(buf,sizeof(buf),"%04d%02d%02d-%02d%02d%02d %06d",
    			tm->tm_year+1900,
    			tm->tm_mon+1,
    			tm->tm_mday,
    			tm->tm_hour,
    			tm->tm_min,
    			tm->tm_sec,
    			GetTickCount()%1000);
    	return buf;

    }
#endif
    void set_log_level(LogLevel ll)
    {
    	cur_log_level=ll;
    }
    void set_log_file_limit_MB(int s)
    {
    	file_limit=s*1000000;
    }
};
}//namespace


#endif /* API_LOG_DEMO_H_ */
