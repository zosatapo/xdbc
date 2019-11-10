/** 
 *  logging.h 
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 

#ifndef __OPENCODE_LOGGING_H__
#define __OPENCODE_LOGGING_H__

#include "port/libdefs.h"
#include "port/ioc_sbuf.h"

__OPENCODE_BEGIN_NAMESPACE

/**
 This class used to output log statements from within the opencode library.

<p> However, it is sometimes useful for the user to learn about what opencode is
doing. You can enable opencode internal logging by calling the
<b>#setInternalDebugging</b> method.

<p>All opencode internal debug calls go to standard output
where as internal error messages are sent to
standard error output. 
*/
class __OPENCODE_CORE_API_DECL Logging
{
public:
  /** All internal messages are prepended with the string "opencode" */
  static const char OPENCODE_NAMESPACE[];

  /**
  Defining this value makes opencode print opencode-internal debug
  statements to <code>System.out</code>.

  <p> The value of this string is <b>opencode.debug</b>.

  <p>Note that the search for all option names is case sensitive.  */
  static const char DEBUG_KEY[];

public:
  
  /** This is the output stream where we will write to.*/
  static FILE* os;

  static bool debugEnabled;
 
  /**
   In quietMode not even errors generate any output.
   */
  static bool quietMode; 
  
public:
  /**
  Allows to enable/disable opencode internal logging.
  */
  static void setInternalDebugging(bool enabled);
  static bool getInternalDebugging();

  /**
  In quite mode Logging generates strictly no output, not even
  for errors. 

  @param quietMode <code>true</code> for no output.
  */
  static void setQuietMode(bool quietMode);
  static bool getQuietMode();
    
  /**
  This method is used to output opencode internal debug
  statements. Output goes to the standard output.
  */
  static void debug(const string& msg);
  static void debug(const string& msg, BaseException& e);
  static void debug(const char* msg);
  static void debug(const char* msg, BaseException& e);

 /**
  This method is used to opencode  internal error
  statements. There is no way to disable error statements.
  Output goes to stderr.
  */
  static void error(const string& msg);
  static void error(const string& msg, BaseException& e);
  static void error(const char* msg);
  static void error(const char* msg, BaseException& e);

  /**
  This method is used to output opencode internal warning
  statements. There is no way to disable warning statements.
  Output goes to stderr.
  */
  static void warn(const string&  msg);
  static void warn(const string&  msg, BaseException& e);
  static void warn(const char*  msg);
  static void warn(const char*  msg, BaseException& e);

public:
  /**
  This method  logs the event without further checks.
  @param level the level to log.
  @param message the message string to log.
  */
  static void forcedLog(const string& prefix,const string& level,const string& message);
  static void forcedLog(const char* prefix,const char* level,const string& message);
  static void forcedLog(const char* prefix,const char* level,const char* message);

  static void forcedLog(const string& prefix,const string& level,const string& message,const string& classname); 
  static void forcedLog(const char* prefix,const char* level,const string& message,const char* classname);
  static void forcedLog(const char* prefix,const char* level,const char* message,const char* classname);

public:
  static FILE* setFile(const string& fileName, bool append); 
  static void  closeFile(); 
  
};

#define LOGGING_DEBUG(log)\
{\
  if(Logging::debugEnabled && !Logging::quietMode)\
  {\
    StringBuffer oss;\
    oss << log;\
    Logging::debug(oss.str());\
  }\
}

#define LOGGING_WARN(log)\
{\
  if(!Logging::quietMode)\
  {\
    StringBuffer oss; \
    oss << log; \
    Logging::warn(oss.str()) ;\
  }\
}

#define LOGGING_ERROR(log)\
{\
  if(!Logging::quietMode)\
  {\
    StringBuffer oss; \
    oss << log; \
    Logging::warn(oss.str());\
  }\
}

__OPENCODE_END_NAMESPACE

#endif //OPENCODE_LOGGING_H__

