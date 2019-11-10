#include "xdbc/driverManager.h"
#include "xdbc/odbcConnection.h"

#include "port/profile.h"
#include "port/file.h"
#include "port/libsys.h"
#include "port/stringTokenizer.h"
#include "port/ioc_sbuf.h"
#include "port/utilities.h"
#include "port/booleanComponent.h"
#include "port/ioc_vector.h"

__OPENCODE_BEGIN_NAMESPACE

Driver::~Driver()
{
  so_lib.close();
}

Driver::Driver()
{
  lib_func = 0;
}

Driver::Driver(const string& driverSchemaValue,const string& versionValue,const string& lib_pathValue)
{
  this->driverSchema = driverSchemaValue;
  this->version = versionValue;
  this->so_lib_path = lib_pathValue;

  lib_func = 0;
}

Driver::Driver(const Driver& rv)
{
  this->driverSchema = rv.driverSchema;
  this->version = rv.version;
  this->so_lib_path = rv.so_lib_path;

  lib_func = rv.lib_func;
  so_lib = rv.so_lib;
}


Connection* Driver::getConnection(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException)
{ 
  if(! so_lib.isOpened())
  {
    __OPENCODE_TRY
    {
      so_lib = SystemLibrary::getInstance(so_lib_path);
    }
    __OPENCODE_CATCH(IOException& ex)
    {
      StringBuffer __sb;
      __sb << "[SQLException] openLibrary - " << ex.getMessage();
      __XDBC_HOOK_RETURN(return 0,__sb.str());
    }
    
    lib_func=(Connection*(*)())so_lib.findSymbol("_xdbc_getConnection");

    if(lib_func == 0)
    {
      StringBuffer __sb;
      __sb << "[SQLException] symbol not found - _xdbc_getConnection";
      __XDBC_HOOK_RETURN(return 0,__sb.str());
    }
  }

  return (*lib_func)(); 
}

string Driver::getSchema()
{
  return driverSchema; 
}

void   Driver::setSchema(const string& driverSchemaValue)
{
  this->driverSchema=driverSchemaValue;
}

string Driver::getVersion()
{
  return version;
}

void   Driver::setVersion(const string& versionValue)
{
  this->version=versionValue;
}

string Driver::getLibPath()
{
  return so_lib_path;
}

void   Driver::setLibPath(const string& lib_pathValue)
{
  this->so_lib_path=lib_pathValue;
}

IMPLEMENT_LIFETIME_BEGIN(DriverMapImpl)
IMPLEMENT_LIFETIME_END(DriverMapImpl)

DriverMapImpl::DriverMapImpl()
{
  DriverManager::isTracingOn = BooleanComponent::getBoolean("xdbc.isTracingOn");
}

DriverMapImpl::~DriverMapImpl()
{
  OPENCODE_MAP<string,Driver*>::iterator __pos = __oc_map_drivers.begin();
  OPENCODE_MAP<string,Driver*>::iterator __end = __oc_map_drivers.end();

  while(__pos != __end)
  {

    if(__pos->second != 0)
    {
      delete __pos->second;
    }  
    ++__pos;
  }
}

void  DriverMapImpl::registerDriver(const string& key,Driver* driver)
{
  LOGGING_DEBUG("DriverMapImpl::registerDriver|" << key);
  Mutex::Guard g(__oc_driver_mutex,System::__M_multithreading);

  Driver* __p_driver = findDriverByKey(key);
  if(__p_driver != 0)
  {
    delete __p_driver;
    __p_driver = 0;
  }

  __oc_map_drivers[key] = driver;
}

void  DriverMapImpl::deregisterDriver(const string& key)
{
  LOGGING_DEBUG("DriverMapImpl::deregisterDriver|" << key);
  Mutex::Guard g(__oc_driver_mutex,System::__M_multithreading); 

  OPENCODE_MAP<string,Driver*>::iterator __pos = __oc_map_drivers.find(key);
  if(__pos == __oc_map_drivers.end())
  {
    return;
  }

  Driver* __p_driver = (__pos->second);
  delete __p_driver;
  __oc_map_drivers.erase(__pos);
}

Driver*  DriverMapImpl::findDriverByKey(const string& key)
{
  Mutex::Guard g(__oc_driver_mutex,System::__M_multithreading);

  OPENCODE_MAP<string,Driver*>::iterator __pos = __oc_map_drivers.find(key);

  if(__pos == __oc_map_drivers.end())
  {
    return 0;
  }

  return (__pos->second);
}

Driver* DriverMapImpl::findDriverByPath(const string& so_lib_path)
{
  OPENCODE_MAP<string,Driver*>::iterator __pos = __oc_map_drivers.begin();
  OPENCODE_MAP<string,Driver*>::iterator __end = __oc_map_drivers.end();

  while(__pos != __end)
  {

    if(so_lib_path.compare(__pos->second->getLibPath()) == 0)
    {
      return __pos->second;
    }
    ++__pos;
  }
 
  return 0;
}

bool DriverManager::isTracingOn = false;

Connection* DriverManager::getConnection(__XDBC_HOOK_DECLARE_MORE const string& dsn_name,__XDBC_ATTRIBUTES& __xdbc_attributes)__XDBC_THROW_DECLARE(SQLException)
{
  if(findDatabaseAttributes(dsn_name,"xdbc.ini","XDBC_CONFIG","XDBC.CONFIG",__xdbc_attributes))
  {
    __XDBC_ATTRIBUTES::iterator __pos = __xdbc_attributes.begin();
    __XDBC_ATTRIBUTES::iterator __end = __xdbc_attributes.end();

    string __drvierSchema ;
    string __version;

    while(__pos != __end)
    {
      if(StringsKit::equalsIgnoreCase(__pos->first.c_str(),"driverSchema"))
      {
        __drvierSchema = StringsKit::trim(__pos->second);
      }
      else if(StringsKit::equalsIgnoreCase(__pos->first.c_str(),"version"))
      {
        __version = StringsKit::trim(__pos->second);
      } 
      ++__pos;
    }
    
    return DriverManager::getConnection(__XDBC_HOOK_INVOKE_MORE __drvierSchema,__version);    
  }

  {
    StringBuffer __sb;
    __sb <<"Driver configuration not found . dsn_name = " << dsn_name;
    __XDBC_HOOK_RETURN(return 0,__sb.str());
  }    
}

Connection* DriverManager::getConnection(__XDBC_HOOK_DECLARE_MORE const string& dsn_name,string& __database,string& __dbuser,string& __password)__XDBC_THROW_DECLARE(SQLException)
{
  __XDBC_ATTRIBUTES __xdbc_attributes;
  Connection* __p_conn = DriverManager::getConnection(__XDBC_HOOK_INVOKE_MORE dsn_name,__xdbc_attributes);
  __XDBC_HOOK_CHECK(return 0);
    
  __XDBC_ATTRIBUTES::iterator __pos = __xdbc_attributes.begin();
  __XDBC_ATTRIBUTES::iterator __end = __xdbc_attributes.end();

  while(__pos != __end)
  {
    if(StringsKit::equalsIgnoreCase(__pos->first.c_str(),"database"))
    {
      __database = StringsKit::trim(__pos->second);
    }
    else if(StringsKit::equalsIgnoreCase(__pos->first.c_str(),"user"))
    {
      __dbuser = StringsKit::trim(__pos->second);
    }
    else if(StringsKit::equalsIgnoreCase(__pos->first.c_str(),"password"))
    {
      __password = StringsKit::trim(__pos->second);
    }

    ++__pos;
  }

  return __p_conn;
}

Connection* DriverManager::getConnection(__XDBC_HOOK_DECLARE_MORE const string& driverSchema,const string& version)__XDBC_THROW_DECLARE(SQLException)
{
  string os_library_path = System::getProperty("library.path");
  string xdbc_library_path  = System::getProperty("XDBC_LIBRARY_PATH");

  if(!xdbc_library_path.empty())
  {
    if(!os_library_path.empty())
    {
      os_library_path.append(File::pathSeparator);
    }
    os_library_path.append(xdbc_library_path);
  }
  
  return DriverManager::getConnection(__XDBC_HOOK_INVOKE_MORE driverSchema,version,os_library_path);  
}

Connection* DriverManager::getConnection(__XDBC_HOOK_DECLARE_MORE const string& driverSchema,const string& version,const string& xdbc_lib_path)__XDBC_THROW_DECLARE(SQLException)
{
  if(   driverSchema.empty() 
     || StringsKit::equalsIgnoreCase(driverSchema.c_str(),"XDBC_ODBC") 
     || StringsKit::equalsIgnoreCase(driverSchema.c_str(),"OPENCODE_ODBC")
     || StringsKit::equalsIgnoreCase(driverSchema.c_str(),"ODBC"))
  {
    if(!DriverManager::isTracingOn)
    {
       DriverManager::isTracingOn = BooleanComponent::getBoolean("xdbc.isTracingOn");
    }

    LOGGING_DEBUG("DriverManager::getConnection<odbc>|" << driverSchema << "::" << version);

    return new OdbcConnection();
  }


  Driver* __p_driver = findOrCreateDriver(__XDBC_HOOK_INVOKE_MORE driverSchema,version,xdbc_lib_path);
  __XDBC_HOOK_CHECK(return 0);
  
  return __p_driver->getConnection(__XDBC_HOOK_INVOKE_ONCE);
}

Driver* DriverManager::findOrCreateDriver(__XDBC_HOOK_DECLARE_MORE const string& driverSchema,const string& version,const string& xdbc_lib_path)__XDBC_THROW_DECLARE(SQLException)
{
  Driver* __p_driver = 0;

  // 1. find driver by full name
  if(!version.empty())
  {
     __p_driver =  DriverMap::getInstance().findDriverByKey(makeDriverKey(driverSchema,version));
  }

  // 2. create driver by full name
  if(__p_driver == 0)
  {
    __p_driver =  DriverManager::createDriver(driverSchema,version,xdbc_lib_path);
  }

  // 3. find driver by driver name
  if(__p_driver == 0)
  {
    __p_driver =  DriverMap::getInstance().findDriverByKey(driverSchema);
  }

  // 4. create driver by driver name
  if(__p_driver == 0)
  {
    string __empty_null;
    __p_driver =  DriverManager::createDriver(driverSchema,__empty_null,xdbc_lib_path);
  }

  if(__p_driver != 0)
  {
    LOGGING_DEBUG("DriverManager::findDriver|" << driverSchema << "::" << version);
    return __p_driver;
  }
  
  {
    StringBuffer __sb;
    __sb <<"Driver not found . driverSchema=" << driverSchema << ",version=" << version << ",xdbc_lib_path=" << xdbc_lib_path;
    __XDBC_HOOK_RETURN(return 0,__sb.str());
  }
}

Driver* DriverManager::createDriver(const string& driverSchema,const string& version,const string& xdbc_lib_path)
{
  string so_file_name="libxdbc_";
  so_file_name.append(makeDriverKey(driverSchema,version));
  so_file_name.append(".");
  so_file_name.append(System::getProperty("library.ext"));

  StringTokenizer tokens(xdbc_lib_path,File::pathSeparator);
  while(tokens.hasMoreTokens())
  {
    File file(tokens.nextToken());
    vector<File> so_files;
    file.listFiles(so_files);

    for(int i = 0;i < so_files.size();++i)
    {
      if( (so_files[i].isFile()) && ( so_files[i].getName().compare(so_file_name) == 0 ) )
      {
        Driver* __p_driver =  DriverMap::getInstance().findDriverByPath(so_files[i].getCanonicalFile().getAbsolutePath());
        if(__p_driver != 0)
        {
          LOGGING_DEBUG("DriverManager::findDriver|" << driverSchema << "::" << version);
          return __p_driver;
        }

        return createDriverInternal(driverSchema,version,so_files[i].getCanonicalFile().getAbsolutePath());

      }//~end if( (so_files[i].isFile()) && ( so_files[i].getName().compare(so_file_name)==0 ) )

    }//~end for(int i=0;i<size;++i)

  }//~end while(tokens.hasMoreTokens())

  return 0; 
}

Driver* DriverManager::createDriverInternal(const string& driverSchema,const string& version,const string& so_lib_path)
{
  LOGGING_DEBUG("DriverManager::createDriver|" << driverSchema << "::" << version << "::" << so_lib_path << "");

  Driver* __p_driver = new Driver(driverSchema,version,so_lib_path);
  DriverMap::getInstance().registerDriver(makeDriverKey(driverSchema,version),__p_driver);

  return __p_driver; 
}

string DriverManager::makeDriverKey(const string& driverSchema,const string& version)
{
  if(version.empty())
  {
    return driverSchema;
  }
  else
  {
    return driverSchema + "_" + version;
  }
}

bool  DriverManager::findDatabaseAttributes(const string& dsnname,const string& basefilename,const string& envname1,const string& envname2,__XDBC_ATTRIBUTES& __xdbc_attributes)
{
  LOGGING_DEBUG("DriverManager::findConfiguration|" << dsnname << "," << basefilename << "," << envname1 << "," << envname2);

  string  apphome = System::getProperty("application.home");

  string  dsnconf ;
  bool    __to_be_continued = true;    

  //-----------------------------------------------------------------------
  //1.search <XXX_CONFIG> and <XXX.CONFIG>
  //-----------------------------------------------------------------------
  const char* __sys_conf  = System::getSystemProperty(envname1.c_str(),0);

  if(__sys_conf != 0)
  {
    dsnconf.assign(__sys_conf);
  }
  else
  {
    dsnconf = System::getProperty(envname2);
  }  

  LOGGING_DEBUG("DriverManager::findConfiguration|env:" << (dsnconf.empty()?"<NULL>":dsnconf));

  if (!dsnconf.empty() && __M_exists(dsnconf))
  {
    __to_be_continued = !loadDatabaseAttributes(dsnname,dsnconf,__xdbc_attributes);
  }
      
  //-----------------------------------------------------------------------
  //2.search ${application.home} 
  //-----------------------------------------------------------------------
  if (__to_be_continued)
  {
    //2.1 search ${application.home}/etc/${basefilename}
    dsnconf = apphome + File::separator + "etc" + File::separator + basefilename;
    LOGGING_DEBUG("DriverManager::findConfiguration|etc:" << dsnconf); 
    if (__M_exists(dsnconf))
    {
      __to_be_continued = !loadDatabaseAttributes(dsnname,dsnconf,__xdbc_attributes);
    }
  }
    
  if (__to_be_continued)
  {
    //2.2 search ${application.home}/${basefilename}  
    dsnconf = apphome + File::separator + basefilename;
    LOGGING_DEBUG("DriverManager::findConfiguration|app:" << dsnconf);

    if (__M_exists(dsnconf))
    {
      __to_be_continued = !loadDatabaseAttributes(dsnname,dsnconf,__xdbc_attributes);
    }
  }

  //-----------------------------------------------------------------------
  //3. search current directory
  //-----------------------------------------------------------------------
  if (__to_be_continued)
  {
    dsnconf =  File::getCurrentDirectory() + File::separator + basefilename;
    LOGGING_DEBUG("DriverManager::findConfiguration|cur:" << dsnconf);
     
    if (__M_exists(dsnconf))
    {
      __to_be_continued = !loadDatabaseAttributes(dsnname,dsnconf,__xdbc_attributes);
    }
  } 

  //-----------------------------------------------------------------------
  //4. search user home directory
  //-----------------------------------------------------------------------
  if (__to_be_continued)
  {
    dsnconf =  System::getProperty("user.home") + File::separator + "." + basefilename;
    LOGGING_DEBUG("DriverManager::findConfiguration|dir:" << dsnconf);
    
    if (__M_exists(dsnconf))
    {
      __to_be_continued = !loadDatabaseAttributes(dsnname,dsnconf,__xdbc_attributes);
    }
  }
  
  return !__to_be_continued;
}

bool DriverManager::loadDatabaseAttributes(const string& dsnname,const string& confname,__XDBC_ATTRIBUTES& __xdbc_attributes)
{
  Profile __xdbc_ini(confname);
  if(__xdbc_ini.containsSection(dsnname))
  {
    Properties __xdbc_props = __xdbc_ini.getProperties(dsnname);
    
    vector<string> __prop_names;
    __xdbc_props.propertyNames(__prop_names);

    vector<string>::iterator __pos = __prop_names.begin();
    vector<string>::iterator __end   = __prop_names.end(); 
    while( __pos != __end)
    {
      __xdbc_attributes[*__pos] = __xdbc_props.getProperty(*__pos);
      ++__pos;
    }   
    
    return true;
  } 
  
  return false;
}

__OPENCODE_END_NAMESPACE

