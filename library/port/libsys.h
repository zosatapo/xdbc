/** 
 *  libsys.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef __OPENCODE_SYSTEM_H__
#define __OPENCODE_SYSTEM_H__

#include "port/libdefs.h"
#include "port/lifetime.h"
#include "port/properties.h"
#include "port/librandom.h"
#include "port/mutex.h"
#include "port/ioc_map.h" 
#include "port/ioc_vector.h"

__OPENCODE_BEGIN_NAMESPACE

class __OPENCODE_CORE_API_DECL SystemDoImpl
{
  DECLARE_LIFETIME_BEGIN(SystemDoImpl)
  DECLARE_LIFETIME_END(SystemDoImpl)
    
private:
  SystemDoImpl(const SystemDoImpl& rv);
  SystemDoImpl& operator=(const SystemDoImpl& rv);

public:
  ~SystemDoImpl() ;
  SystemDoImpl();

public:
  /** test readonly attribute of system property*/
  bool   isReadonly(const string& key);

  /**
   * Gets the system property indicated by the specified key.
   *
   * @param      key   the name of the system property.
   * @param      def   a default value.
   * @return     the string value of the system property,
   *             or the default value if there is no property with that key.
   *
   */
  string getProperty(const string& key, const string& def);
  const string& getProperty(const string& key);
  void  propertyNames(vector<string>& names);
  
  /**
   * Sets the system property indicated by the specified key.
   *
   * @param      key   the name of the system property.
   * @param      value the value of the system property.
   *
   */
  void  setProperty(const string& key, const string& value);
  int   setProperties(const Properties& props);
    
private:
  /**
   * System properties. The following properties are guaranteed to be defined:
   *
   * os.name            (readonly) Operating System Name
   * os.version         (readonly) Operating System Version
   * 
   * file.separator     (readonly) File separator ("/" on Unix)
   * path.separator     (readonly) Path separator (":" on Unix)
   * line.separator     (readonly) Line separator ("\n" on Unix)
   *
   * user.name          (readonly) User account name
   * user.home          (readonly) User home directory
   *
   */
  void   __OPENCODE_NATIVE_CODE  initializeProperties();
  void   __OPENCODE_NATIVE_CODE  initializeLogging();
  void   __OPENCODE_NATIVE_CODE  initializeLocale();  
  void   __OPENCODE_NATIVE_CODE  initializeOthers();
  
  void   __OPENCODE_NATIVE_CODE  parseCommandLine(string& appcmd,string& appenv,const string& cmdline);
  string __OPENCODE_NATIVE_CODE  parseAppicationName(const string& appcmd);
  string __OPENCODE_NATIVE_CODE  parseAppicationHome(const string& appcmd);  
  int    __OPENCODE_NATIVE_CODE  parseAppicationEnv(const vector<string>& appenv); 
  
public:  
  int    __OPENCODE_NATIVE_CODE  parseAppicationEnv(const string& envline); 
     
public:
  Mutex      __syn_mutex;
  Properties __props;
  OPENCODE_MAP<string,string> __map_readonlys;
};

typedef SingletonHolder<SystemDoImpl,CreateUsingNew<SystemDoImpl>,SingletonWithLongevity<SystemDoImpl>,MultiThread<SystemDoImpl> > SystemImpl;
__OPENCODE_INLINE unsigned int getLongevity(SystemDoImpl* impl){return __BASE_LONGEVITY_SYSTEM;}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
//YYYY-MM-DD
#define STAMP_DATE            (0)

//hh:mm:ss
#define STAMP_TIME            (1)

//YYYY-MM-DD hh:mm:ss
#define STAMP_DATETIME        (2)

//YYYY-MM-DD hh:mm:ss.xxxyyyzzz
#define STAMP_FULL_FRACTION   (3)
#define STAMP_TIMESTAMP       STAMP_FULL_FRACTION

//YYYY-MM-DD hh:mm:ss.xxxyyy
#define STAMP_HIGH_FRACTION    (4)

//YYYY-MM-DD hh:mm:ss.xxx
#define STAMP_NORM_FRACTION   (5)
#define STAMP_MILLISECONDS    STAMP_NORM_FRACTION

//YYYYMMDD
#define STAMP_CLOSED_DATE     (6)

//YYYYMMDDhhmmss
#define STAMP_CLOSED_DATETIME (7)

//seconds.microseconds
#define STAMP_MICROSECONDS    (8)

/**
 * The <code>System</code> class contains several useful class fields
 * and methods. It cannot be instantiated.
 * 
 */
class __OPENCODE_CORE_API_DECL System
{
private:
  /** Don't let anyone instantiate this class */
  System();
  System(const System& rv);
  System& operator=(const System& rv);

public:  
  /** Gets the system error information indicated by the specified errno.*/
  static string   getLastError(XLONG errorNumber = System::getLastErrorNumber());
  static XLONG    getLastErrorNumber();
  static void     setLastErrorNumber(XLONG errorNumber);
  
public:
  /**
   * Returns the current time in milliseconds.  Note that
   * while the unit of time of the return value is a millisecond,
   * the granularity of the value depends on the underlying
   * operating system and may be larger.  For example, many
   * operating systems measure time in units of tens of
   * milliseconds.
   *
   * <p> See the description of the class <code>Date</code> for
   * a discussion of slight discrepancies that may arise between
   * "computer time" and coordinated universal time (UTC).
   *
   * @return  the difference, measured in milliseconds, between
   *          the current time and midnight, January 1, 1970 UTC.
   */
  static  XLONG   __OPENCODE_NATIVE_CODE currentTimeMillis();
  static  XLONG   __OPENCODE_NATIVE_CODE currentTimeMicroSeconds();
  static  XLONG   __OPENCODE_NATIVE_CODE currentTimeSeconds();
  static  void    __OPENCODE_NATIVE_CODE currentTime(XLONG& seconds,XLONG& nanoseconds); 

  static string   __OPENCODE_NATIVE_CODE currentTimestamp(int style = STAMP_HIGH_FRACTION);
  static void     __OPENCODE_NATIVE_CODE currentTimestamp(char* buffer,int style = STAMP_HIGH_FRACTION);
  
public:  
  /**
   * Gets the system environment property indicated by the specified key.
   *
   * @param      key   the name of the system environment property.
   * @param      def   a default value.
   * @return     the string value of the system property,
   *                 or the default value if there is no property with that key.
   *
   */
  static const char* getSystemProperty(const char* key,const char* def = 0); 
 
  /**
   * Sets the system environment property indicated by the specified key.
   *
   * @param      key   the name of the system  environment property.
   * @param      value the value of the system environment property.
   */
  static void setSystemProperty(const char* key, const char* value);

  /**
   * Gets the system property indicated by the specified key.
   *
   * @param      key   the name of the system property.
   * @param      def   a default value.
   * @return     the string value of the system property,
   *                 or the default value if there is no property with that key.
   *
   */
  static string  getProperty(const string& key, const string& def);
  static string  getProperty(const string& key);
  static void    propertyNames(vector<string>& names);
  
  /**
   * Sets the system property indicated by the specified key.
   *
   * @param      key   the name of the system property.
   * @param      value the value of the system property.
   */
  static void   setProperty(const string& key, const string& value);  
  static int    setProperties(const Properties& props); 

public:     
  static string __OPENCODE_NATIVE_CODE getCommandLine(UXLONG processWanted);

  static void   initializeEnvironment(int argc,char** argv);
  static void   setMultiThreading(bool multithreading);   
  static void   loggingAndExit(const string& msg,int exitCode); 
  static int    waitAnyKey();

public:
  static bool       __M_multithreading;
};

__OPENCODE_END_NAMESPACE

#endif
