#ifndef __OPENCODE_PORTABLE_TIME_H__
#define __OPENCODE_PORTABLE_TIME_H__

#include "port/ioc_G_config.h"

#if defined(PORT_WINDOWS)
#  include <time.h>
#else
#  include <sys/time.h>
#endif

#if defined(PORT_WINDOWS)
//From January 1, 1601 (UTC). to January 1,1970
#  define FILETIME_TO_TIMVAL_SKEW XLONG_LITERAL(0x19db1ded53e8000)
#  if defined(OPENCODE_WIN64)
#    define __WIN_APIHOOK_LOCALTIME	 _localtime64_s
#  else
#    define __WIN_APIHOOK_LOCALTIME	 _localtime64_s	
#  endif	
#endif

#if defined(PORT_WINDOWS)
#  define  OPENCODE_APIHOOK_LOCALTIME(__api_TIME,__api_TM)     __WIN_APIHOOK_LOCALTIME(__api_TM,__api_TIME)
#else
#  define  OPENCODE_APIHOOK_LOCALTIME(__api_TIME,__api_TM)     localtime_r(__api_TIME,__api_TM)
#endif

#endif



