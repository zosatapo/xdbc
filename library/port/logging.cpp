#include "port/logging.h"
#include "port/libsys.h"

__OPENCODE_BEGIN_NAMESPACE

const char Logging::DEBUG_KEY[]          = "opencode.debug";
const char Logging::OPENCODE_NAMESPACE[] = "opencode::port";


/** This is the output stream where we will write to.*/
FILE* Logging::os = stdout;

bool Logging::debugEnabled = false;  
bool Logging::quietMode = false;
    
void Logging::setQuietMode(bool quietModeValue)
{
  Logging::quietMode = quietModeValue;
}
 
bool Logging::getQuietMode()
{
  return Logging::quietMode;
}

void Logging::setInternalDebugging(bool debugEnabledValue)
{
  Logging::debugEnabled = debugEnabledValue;
}

bool Logging::getInternalDebugging()
{
  return Logging::debugEnabled;
}

void Logging::debug(const string& msg)
{
  if(debugEnabled && !quietMode)
  {
    forcedLog(OPENCODE_NAMESPACE,"DEBUG",msg); 
  }
}

void Logging::debug(const string& msg, BaseException& e)
{
  if(debugEnabled && !quietMode)
  {
    forcedLog(OPENCODE_NAMESPACE,"DEBUG",msg); 
  }
  forcedLog(OPENCODE_NAMESPACE,"DEBUG",e.getMessage()); 
}

void Logging::debug(const char* msg)
{
  if(debugEnabled && !quietMode)
  {
    forcedLog(OPENCODE_NAMESPACE,"DEBUG",msg); 
  }
}
 
void Logging::debug(const char* msg, BaseException& e)
{
  if(debugEnabled && !quietMode)
  {
    forcedLog(OPENCODE_NAMESPACE,"DEBUG",msg); 
  }
  forcedLog(OPENCODE_NAMESPACE,"DEBUG",e.getMessage()); 
}

void Logging::error(const string& msg)
{
  if(!quietMode)
  {
    forcedLog(OPENCODE_NAMESPACE,"ERROR",msg); 
  }
}  

void Logging::error(const string& msg, BaseException& e)
{
  if(!quietMode)
  {
    forcedLog(OPENCODE_NAMESPACE,"ERROR",msg); 
  }
  forcedLog(OPENCODE_NAMESPACE,"ERROR",e.getMessage());
}

void Logging::error(const char* msg)
{
  if(!quietMode)
  {
    forcedLog(OPENCODE_NAMESPACE,"ERROR",msg); 
  }
}
 
void Logging::error(const char* msg, BaseException& e)
{
  if(!quietMode)
  {
    forcedLog(OPENCODE_NAMESPACE,"ERROR",msg); 
  }
  forcedLog(OPENCODE_NAMESPACE,"ERROR",e.getMessage());
}



void Logging::warn(const string& msg) 
{
  if(!quietMode)
  {
    forcedLog(OPENCODE_NAMESPACE,"WARN",msg); 
  }
}

void Logging::warn(const string& msg, BaseException& e)
{
  if(!quietMode)
  {
    forcedLog(OPENCODE_NAMESPACE,"WARN",msg); 
  }
  forcedLog(OPENCODE_NAMESPACE,"WARN",e.getMessage());
}

void Logging::warn(const char* msg)
{
  if(!quietMode)
  {
    forcedLog(OPENCODE_NAMESPACE,"WARN",msg); 
  }
}
 
void Logging::warn(const char* msg, BaseException& e)
{
  if(!quietMode)
  {
    forcedLog(OPENCODE_NAMESPACE,"WARN",msg); 
  }
  forcedLog(OPENCODE_NAMESPACE,"WARN",e.getMessage());
}

void Logging::forcedLog(const string& prefix,const string& level,const string& message)
{
  if(os != 0)
  {
    StringBuffer __sb;   
    __sb << prefix << " - " << System::currentTimestamp() << " - " << level << " - " << message << File::lineSeparator;
    fprintf(os,"%s",__sb.str().c_str());
    fflush(os);
  }
}

void Logging::forcedLog(const char* prefix,const char* level,const string& message)
{
  if(os != 0)
  {
    StringBuffer __sb;   
    __sb << prefix << " - " << System::currentTimestamp() << " - " << level << " - " << message << File::lineSeparator;
    fprintf(os,"%s",__sb.str().c_str());
    fflush(os);
  }
}

void Logging::forcedLog(const char* prefix,const char* level,const char* message)
{
  if(os != 0)
  {
    StringBuffer __sb;   
    __sb << prefix << " - " << System::currentTimestamp() << " - " << level << " - " << message << File::lineSeparator;
    fprintf(os,"%s",__sb.str().c_str());
    fflush(os);
  }
}
   
void Logging::forcedLog(const string& prefix,const string& level,const string& message,const string& classname)
{
  if(os != 0)
  {
    StringBuffer __sb;   
    __sb << prefix << " - " << System::currentTimestamp() << " - " << level << " - " << classname << "::" << message << File::lineSeparator;
    fprintf(os,"%s",__sb.str().c_str());
    fflush(os);
  }
}

void Logging::forcedLog(const char* prefix,const char* level,const string& message,const char* classname)
{
  if(os != 0)
  {
    StringBuffer __sb;   
    __sb << prefix << " - " << System::currentTimestamp() << " - " << level << " - " << classname << "::" << message << File::lineSeparator;
    fprintf(os,"%s",__sb.str().c_str());
    fflush(os);
  }
}

void Logging::forcedLog(const char* prefix,const char* level,const char* message,const char* classname)
{
  if(os != 0)
  {
    StringBuffer __sb;   
    __sb << prefix << " - " << System::currentTimestamp() << " - " << level << " - " << classname << "::" << message << File::lineSeparator;
    fprintf(os,"%s",__sb.str().c_str());
    fflush(os);
  }
}

FILE* Logging::setFile(const string& aFileName, bool append)
{
  FILE* __old_os = os;
  
  if(aFileName.empty())
  {
    os = stdout;
  }
  else
  {
    if(append)
    {
      os = fopen(aFileName.c_str(),"a");
    }
    else
    {
      os = fopen(aFileName.c_str(),"w");
    }
  }
  
  return __old_os;
}

void  Logging::closeFile()
{
  if(os != stdout)
  {
    fclose(os);
    os = 0;
  }  
}

__OPENCODE_END_NAMESPACE

