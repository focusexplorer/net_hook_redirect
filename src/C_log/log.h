/*2015-12-21 by focuszhang*/

#ifndef LOG_H_
#define LOG_H_
#include <stdio.h>
#include "include/api_log.h"
extern ns_log::MLogInterface* gp_log;
namespace ns_log
{
}//namespace
#ifdef linux
#define LOG_ERROR(format,args...) \
	do{	\
		if(!gp_log){printf("[%s:%d]"format"\n",__FILE__,__LINE__,##args);break;}	\
		if(!gp_log->is_priority_enabled(ns_log::CS_LOG_ERROR)){break;}	\
		gp_log->log(ns_log::CS_LOG_ERROR,__FILE__,__LINE__,__FUNCTION__,format,##args);	\
	}while(0)

#define LOG_DEBUG(format,args...) \
	do{	\
		if(!gp_log){printf("[%s:%d]"format"\n",__FILE__,__LINE__,##args);break;}	\
		if(!gp_log->is_priority_enabled(ns_log::CS_LOG_DEBUG)){break;}	\
		gp_log->log(ns_log::CS_LOG_DEBUG,__FILE__,__LINE__,__FUNCTION__,format,##args);	\
	}while(0)

#elif defined(_WIN32) || defined(_WIN64)
#define LOG_ERROR(format,...) \
	do{	\
		if(!gp_log){printf("[%s:%d]"format"\n",__FILE__,__LINE__,##__VA_ARGS__);break;}	\
		if(!gp_log->is_priority_enabled(ns_log::CS_LOG_ERROR)){break;}	\
		gp_log->log(ns_log::CS_LOG_ERROR,__FILE__,__LINE__,__FUNCTION__,format,##__VA_ARGS__);	\
	}while(0)

#define LOG_DEBUG(format,...) \
	do{	\
		if(!gp_log){printf("[%s:%d]"format"\n",__FILE__,__LINE__,##__VA_ARGS__);break;}	\
		if(!gp_log->is_priority_enabled(ns_log::CS_LOG_DEBUG)){break;}	\
		gp_log->log(ns_log::CS_LOG_DEBUG,__FILE__,__LINE__,__FUNCTION__,format,##__VA_ARGS__);	\
	}while(0)
#endif
#endif /* LOG_H_ */





