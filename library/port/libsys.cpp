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
  
bool System::__M_multithreading =  true;
  
string System::getProperty(const string& key, const string& def)
{
  string prop = SystemImpl::getInstance().getProperty(key);
  if(! prop.empty())
  {
    return prop;
  }

  const char* __sys_prop = getSystemProperty(key.c_str());
  if(__sys_prop != 0)
  {
    return string(__sys_prop);
  }
  
  return def;  
}

string System::getProperty(const string& key)
{
  SystemDoImpl& __sysImpl = SystemImpl::getInstance();
  string prop = __sysImpl.getProperty(key);
  
  if(! prop.empty())
  {
    return prop;
  }

  const char* __sys_prop = getSystemProperty(key.c_str());
  if(__sys_prop != 0)
  {
    return string(__sys_prop);
  }

  return string();  
}

void  System::setProperty(const string& key, const string& value)
{
  SystemDoImpl& __sysImpl = SystemImpl::getInstance();  
  __sysImpl.setProperty(key,value);
}

void  System::propertyNames(vector<string>& names)
{
	SystemDoImpl& __sysImpl = SystemImpl::getInstance(); 	
  __sysImpl.propertyNames(names);
}

string  System::getLastError(XLONG errorNumber)
{
#if defined(PORT_WINDOWS)
   return string("");
#else
  char* err_buf = 0 ;
#ifdef OPENCODE_HAS_STRERROR
  err_buf = strerror((int)errorNumber);
#else
  char buffer[1024+1] = {0};
  err_buf = strerror_r((int)errorNumber,buffer,1024);
#endif

  if(err_buf == 0)
  {
    return string("");
  }
  else
  {
    return string(err_buf);
  }  
#endif
}

XLONG  System::getLastErrorNumber()
{
#if defined(PORT_WINDOWS)
  return (XLONG)::GetLastError();
#else
  return (XLONG)errno;
#endif
}

void System::setLastErrorNumber(XLONG errorNumber)
{
#if defined(PORT_WINDOWS)
  ::SetLastError((int)errorNumber);
#else
  errno = (int)errorNumber;
#endif
}
  
string   System::currentTimestamp(int style)
{
  char timestamp[30] = {0};
  System::currentTimestamp(timestamp,style); 
  return string(timestamp);
}

void  System::initializeEnvironment(int argc,char** argv)
{
  SystemDoImpl& __sysimpl = SystemImpl::getInstance();
  
  {
	  StringBuffer __sb;
	  for(int i = 1; i < argc;++i)
	  {
	  	if(i != 0){__sb << " ";}	
	  	__sb << argv[i];
	  }
	  __sysimpl.parseAppicationEnv(__sb.str());
	}
	
}  

string   System::getCommandLine(UXLONG processWanted)
{
  char __cmdline[OPENCODE_PATH_MAX + 1] = {0};
  
#if defined(PORT_WINDOWS)
  {
    const char* __cmdlineP = ::GetCommandLine();
    strncpy(__cmdline,__cmdlineP,OPENCODE_PATH_MAX);
  }
#elif defined(PORT_HPUX)
  {
    struct pst_status pst;
    if (pstat_getproc(&pst, sizeof(pst), (size_t)0, (pid_t)processWanted)!= -1)
    {
      strncpy(__cmdline,pst.pst_cmd,__M_min(OPENCODE_PATH_MAX,(int)strlen(pst.pst_cmd)));   
    }
  }
#elif defined(PORT_AIX)
  {
    char linkname[OPENCODE_PATH_MAX+1] = {0}; /* /proc/<pid>/psinfo */

    /* Get our PID and build the name of the link in /proc */
    if (snprintf(linkname, OPENCODE_PATH_MAX, "/proc/%u/psinfo", (pid_t)processWanted) != -1)
    {
      FILE* fp = fopen(linkname,"rb");
      if(fp != 0)
      {
        fseek(fp,0,SEEK_END);
        size_t __fileSize = ftell(fp);
        char* __pbuff = new char[__fileSize + 1];
        
        fseek(fp,0,SEEK_SET);
        fread(__pbuff,__fileSize,1,fp);
        fclose(fp);
        psinfo_t* __psinfo = (psinfo_t*)(__pbuff);
        strncpy(__cmdline,__psinfo->pr_psargs,__M_min(OPENCODE_PATH_MAX,(int)strlen(__psinfo->pr_psargs)));   
        delete []__pbuff;
      }
    }    
  }
#elif defined(PORT_SUN_SOLARIS)
  {
    char linkname[OPENCODE_PATH_MAX+1] = {0}; /* /proc/<pid>/psinfo */

    /* Get our PID and build the name of the link in /proc */
    if (snprintf(linkname, OPENCODE_PATH_MAX, "/proc/%u/psinfo", (pid_t)processWanted) != -1)
    {
      FILE* fp = fopen(linkname,"rb");
      if(fp != 0)
      {
        fseek(fp,0,SEEK_END);
        size_t __fileSize = ftell(fp);
        char* __pbuff = new char[__fileSize + 1];
        
        fseek(fp,0,SEEK_SET);
        fread(__pbuff,__fileSize,1,fp);
        fclose(fp);
        psinfo_t* __psinfo = (psinfo_t*)(__pbuff);
        strncpy(__cmdline,__psinfo->pr_psargs,__M_min(OPENCODE_PATH_MAX,(int)strlen(__psinfo->pr_psargs)));   
        delete []__pbuff;
      }
    }    
  }
#else    
  {
    char linkname[OPENCODE_PATH_MAX+1]; /* /proc/<pid>/cmdline */
  
    /* Get our PID and build the name of the link in /proc */
    if (snprintf(linkname, OPENCODE_PATH_MAX, "/proc/%u/cmdline", (pid_t)processWanted) != -1)
    {
      FILE* fp = fopen(linkname,"r");
      if(fp != 0)
      { 
        char line[OPENCODE_PATH_MAX+1] = {0};
        size_t count = fread (line, 1,(int) OPENCODE_PATH_MAX, fp);
        for(size_t i = 0;i<count;++i)
        {
          if(line[i] == '\0') { line[i] = ' '; }
        }        
        fclose(fp);
        strncpy(__cmdline,line,__M_min(OPENCODE_PATH_MAX,(int)strlen(line))); 
      } 
    } 
  }
#endif

  LineFile::chopCRLF(__cmdline);
  return StringsKit::trim(__cmdline);
}

void  System::setMultiThreading(bool multithreading)
{
  __M_multithreading = multithreading;
}

int  System::setProperties(const Properties& props)
{
  return SystemImpl::getInstance().setProperties(props);
}

void System::loggingAndExit(const string& msg,int exitCode)
{
  LOGGING_ERROR(msg);
  exit(exitCode);
}

int  System::waitAnyKey()
{
  puts("Press any key to continue...");
  int __wait_char ;
  __wait_char = getchar();
  return __wait_char;
}

#if !defined(PORT_WINDOWS)

const char* System::getSystemProperty(const char* key, const char* def)
{
  const char* prop = getenv(key);
  
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

#ifndef OPENCODE_HAS_SETENV
  string strEnv = string(key) + ("=") + value;
  char* __p_env = new char[strEnv.size() + 1];
  memcpy(__p_env,strEnv.c_str(),strEnv.size());
  __p_env[strEnv.size()] = '\0';
  
  ::putenv(__p_env);
#else
 /* WARNING ! 
  * We don't use putenv with glibc, because it doesn't make
  * a copy of the string, but try to keep the pointer cf. man 3 putenv.  
  */
 ::setenv(key, value, 1);
#endif
}

XLONG  System::currentTimeMillis()
{
  struct timeval tp;
  gettimeofday(&tp, 0);
  return  (((XLONG)tp.tv_sec) * 1000) + (tp.tv_usec / 1000); 
}

XLONG  System::currentTimeSeconds()
{
#ifndef OPENCODE_HAS_CLOCK_GETTIME
  struct timeval tp;
  gettimeofday(&tp, 0);
  return (XLONG)tp.tv_sec;
#else  
  struct timespec tsp;
  clock_gettime(CLOCK_REALTIME,&tsp);
  return tsp.tv_sec;
#endif 
}

XLONG  System::currentTimeMicroSeconds()
{
  struct timeval tp;
  gettimeofday(&tp, 0);
  XLONG seconds = tp.tv_sec;
  return (seconds * 1000 * 1000) + tp.tv_usec;
}

void System::currentTime(XLONG& seconds,XLONG& nanoseconds)
{
#ifndef OPENCODE_HAS_CLOCK_GETTIME
  struct timeval tp;
  gettimeofday(&tp, 0);
  
  seconds = tp.tv_sec;
  nanoseconds = ((XLONG)tp.tv_usec)*1000;
  
#else  
  struct timespec tsp;
  clock_gettime(CLOCK_REALTIME,&tsp);
  
  seconds = tsp.tv_sec;
  nanoseconds = tsp.tv_nsec;
#endif 
}
 
void      System::currentTimestamp(char* buffer,int style)
{  
#ifndef OPENCODE_HAS_CLOCK_GETTIME
  struct timeval tp;
  gettimeofday(&tp, 0);
  XLONG seconds = tp.tv_sec;
#else  
  struct timespec tsp;
  clock_gettime(CLOCK_REALTIME,&tsp); 
  XLONG seconds = tsp.tv_sec;
#endif 
 
  if(style == STAMP_MICROSECONDS)
  {
  #ifndef OPENCODE_HAS_CLOCK_GETTIME
    int microseconds = (int)(tp.tv_usec);
  #else  
    int microseconds = (int)(tsp.tv_nsec/1000);
  #endif 

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
    #ifndef OPENCODE_HAS_CLOCK_GETTIME
      int nanoseconds =  (int)(tp.tv_usec * 1000);
    #else  
      int nanoseconds =  tsp.tv_nsec;
    #endif 
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
    #ifndef OPENCODE_HAS_CLOCK_GETTIME
      int microseconds = (int)(tp.tv_usec);
    #else  
      int microseconds = (int)(tsp.tv_nsec/1000);
    #endif 
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
    #ifndef OPENCODE_HAS_CLOCK_GETTIME
      int microseconds = (int)(tp.tv_usec);
    #else  
      int microseconds = (int)(tsp.tv_nsec/1000);
    #endif 
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
