#include "port/libsys.h"
#include "port/ioc_sbuf.h"
#include "port/stringTokenizer.h"
#include "port/utilities.h"
#include "port/logging.h"
#include "port/libnumber.h"
#include "port/booleanComponent.h"
#include "port/libproc.h"
#include "port/file.h"
#include "port/lineparser.h" 

#include "port/ioc_time.h" 
#include "port/ioc_fdir.h" 
#include "port/ioc_sysinfo.h"

#include <algorithm>

__OPENCODE_BEGIN_NAMESPACE

#if defined(PORT_WINDOWS)

struct __syswin_timeval 
{
   XLONG    tv_sec;         /* seconds */
   XLONG    tv_usec;        /* and microseconds */
};

int __syswin_gettimeofday(struct __syswin_timeval* tp,void *tz)
{
  FILETIME tfile;
  ::GetSystemTimeAsFileTime(&tfile);
  
  ULARGE_INTEGER __100ns;
  __100ns.HighPart = tfile.dwHighDateTime;
  __100ns.LowPart = tfile.dwLowDateTime;
  
  __100ns.QuadPart -= FILETIME_TO_TIMVAL_SKEW;
 
   // Convert 100ns units to seconds;
  tp->tv_sec = (XLONG) (__100ns.QuadPart / (10000 * 1000));
  
  // Convert remainder to microseconds;
  tp->tv_usec = (XLONG) ((__100ns.QuadPart % (10000 * 1000)) / 10);

  return 0;
}


const char* System::getSystemProperty(const char* key, const char* def)
{
  const char* prop = ::getenv(key);
  
  if(prop != 0)
  {
    return prop;
  }

  return def;
}

void  System::setSystemProperty(const char* key, const char* value)
{
  if(key == 0 || value == 0 )
  {
    return;
  }

  ::SetEnvironmentVariable(key,value);
}

XLONG  System::currentTimeMillis()
{
  struct __syswin_timeval  tp;
  __syswin_gettimeofday(&tp, 0);
  return  (((XLONG)tp.tv_sec) * 1000) + (tp.tv_usec / 1000); 
}

XLONG  System::currentTimeSeconds()
{
  struct __syswin_timeval tp;
  __syswin_gettimeofday(&tp, 0);
  return (XLONG)tp.tv_sec;
}

XLONG  System::currentTimeMicroSeconds()
{
  struct __syswin_timeval tp;
  __syswin_gettimeofday(&tp, 0);
  XLONG seconds = tp.tv_sec;
  return (seconds * 1000 * 1000) + tp.tv_usec;
}

void System::currentTime(XLONG& seconds,XLONG& nanoseconds)
{
  struct __syswin_timeval tp;
  __syswin_gettimeofday(&tp, 0); 
  seconds = tp.tv_sec;
  nanoseconds = ((XLONG)tp.tv_usec)*1000;
}
 
void      System::currentTimestamp(char* buffer,int style)
{  
  struct __syswin_timeval tp;
  __syswin_gettimeofday(&tp, 0);
  XLONG seconds = tp.tv_sec;
 
  if(style == STAMP_MICROSECONDS)
  {
    int microseconds = (int)(tp.tv_usec);
    char __msg_format[20]= {0};
    sprintf(__msg_format,"%s.%%06d",XLONG_FORMAT_SPECIFIER);
    sprintf(buffer,__msg_format,seconds,microseconds);

    return;
  }
  
  time_t time_seconds = (time_t)seconds;
  struct tm fast_tm;
  OPENCODE_APIHOOK_LOCALTIME(&time_seconds,&fast_tm);   
  switch(style)
  {
    case STAMP_DATE:
    {
       sprintf(buffer,"%04d-%02d-%02d",
               fast_tm.tm_year + 1900,
               fast_tm.tm_mon + 1,
               fast_tm.tm_mday); 
    }
    break;  
    case STAMP_TIME:
    {
       sprintf(buffer,"%02d:%02d:%02d",
               fast_tm.tm_hour,
               fast_tm.tm_min,
               fast_tm.tm_sec); 
    }
    break;
    case STAMP_DATETIME:
    {
        sprintf(buffer,"%04d-%02d-%02d %02d:%02d:%02d",
               fast_tm.tm_year + 1900,
               fast_tm.tm_mon + 1,
               fast_tm.tm_mday,
               fast_tm.tm_hour,
               fast_tm.tm_min,
               fast_tm.tm_sec); 
    }
    break;
    case STAMP_CLOSED_DATE:
    {
       sprintf(buffer,"%04d%02d%02d",
               fast_tm.tm_year + 1900,
               fast_tm.tm_mon + 1,
               fast_tm.tm_mday); 
    }
    break;  
    case STAMP_CLOSED_DATETIME:
    {
       sprintf(buffer,"%04d%02d%02d%02d%02d%02d",
               fast_tm.tm_year + 1900,
               fast_tm.tm_mon + 1,
               fast_tm.tm_mday,
               fast_tm.tm_hour,
               fast_tm.tm_min,
               fast_tm.tm_sec); 
    }
    break; 
    case STAMP_FULL_FRACTION:
    {
       int nanoseconds =  (int)(tp.tv_usec * 1000); 
       sprintf(buffer,"%04d-%02d-%02d %02d:%02d:%02d.%09d",
               fast_tm.tm_year + 1900,
               fast_tm.tm_mon + 1,
               fast_tm.tm_mday,
               fast_tm.tm_hour,
               fast_tm.tm_min,
               fast_tm.tm_sec,
               nanoseconds);      
    }
    break;
    case STAMP_NORM_FRACTION:
    {
       int microseconds = (int)(tp.tv_usec); 
       sprintf(buffer,"%04d-%02d-%02d %02d:%02d:%02d.%03d",
               fast_tm.tm_year + 1900,
               fast_tm.tm_mon + 1,
               fast_tm.tm_mday,
               fast_tm.tm_hour,
               fast_tm.tm_min,
               fast_tm.tm_sec,
               (microseconds/1000)); 
    }
    break;                     
    default:
    {  
       int microseconds = (int)(tp.tv_usec);
       sprintf(buffer,"%04d-%02d-%02d %02d:%02d:%02d.%06d",
               fast_tm.tm_year + 1900,
               fast_tm.tm_mon + 1,
               fast_tm.tm_mday,
               fast_tm.tm_hour,
               fast_tm.tm_min,
               fast_tm.tm_sec,
               microseconds); 
    }
    break;
  }  
}
#endif
  
__OPENCODE_END_NAMESPACE
