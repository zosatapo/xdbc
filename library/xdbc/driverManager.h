/**
 *  driverManager.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef __OPENCODE_XDBC_XDRIVERMANAGER_H__
#define __OPENCODE_XDBC_XDRIVERMANAGER_H__

#include "xdbc/xdefine.h"

#include "port/lifetime.h"
#include "port/systemLibrary.h"
#include "port/libsys.h"

#include "port/ioc_map.h" 

__OPENCODE_BEGIN_NAMESPACE

class __OPENCODE_XDBC_API_DECL Driver
{
public:
  ~Driver();

  Driver();
  Driver(const string& driverSchema,const string& version,const string& lib_path);
  Driver(const Driver& rv);

public:
  Connection* getConnection(__XDBC_HOOK_DECLARE_ONCE)__XDBC_THROW_DECLARE(SQLException);

public:
  string getSchema();
  void   setSchema(const string& driverSchema);
  
  string getVersion();
  void   setVersion(const string& version);

  string getLibPath();
  void   setLibPath(const string& lib_path);

private:
  string driverSchema;
  string version;
  string so_lib_path;

//--------------------------
  Connection*   (*lib_func)();
  SystemLibrary  so_lib;
};

class __OPENCODE_XDBC_API_DECL DriverMapImpl
{
  DECLARE_LIFETIME_BEGIN(DriverMapImpl)
  DECLARE_LIFETIME_END(DriverMapImpl)
  
public:
  ~DriverMapImpl();
  DriverMapImpl();

public:
  void      registerDriver(const string& key,Driver* Driver);
  void      deregisterDriver(const string& key);

  Driver*  findDriverByKey(const string& key);
  Driver*  findDriverByPath(const string& so_lib_path);

private:
  OPENCODE_MAP<string,Driver*> __oc_map_drivers;
  Mutex  __oc_driver_mutex;
};

typedef SingletonHolder<DriverMapImpl,CreateUsingNew<DriverMapImpl>,SingletonWithLongevity<DriverMapImpl>,MultiThread<DriverMapImpl> > DriverMap;
__OPENCODE_INLINE unsigned int getLongevity(DriverMapImpl* impl){return __XDBC_LONGEVITY_DRIVERMAP;}

class __OPENCODE_XDBC_API_DECL DriverManager
{
public:
  static bool isTracingOn;
  
public:
  static Connection* getConnection(__XDBC_HOOK_DECLARE_MORE const string& driverSchema,const string& version)__XDBC_THROW_DECLARE(SQLException);
  static Connection* getConnection(__XDBC_HOOK_DECLARE_MORE  const string& driverSchema,const string& version,const string& xdbc_lib_path)__XDBC_THROW_DECLARE(SQLException);
  
  static Connection* getConnection(__XDBC_HOOK_DECLARE_MORE const string& dsname,__XDBC_ATTRIBUTES& attributes)__XDBC_THROW_DECLARE(SQLException);
  static Connection* getConnection(__XDBC_HOOK_DECLARE_MORE const string& dsn_name,string& __database,string& __dbuser,string& __password)__XDBC_THROW_DECLARE(SQLException);
  static bool        findDatabaseAttributes(const string& dsnname,const string& basefilename,const string& envname1,const string& envname2,__XDBC_ATTRIBUTES& props);

private:
  static Driver* findOrCreateDriver(__XDBC_HOOK_DECLARE_MORE const string& driverSchema,const string& version,const string& xdbc_lib_path)__XDBC_THROW_DECLARE(SQLException);
  static Driver* createDriver(const string& driverSchema,const string& version,const string& xdbc_lib_path);
  static Driver* createDriverInternal(const string& driverSchema,const string& version,const string& so_lib_path);
  static string  makeDriverKey(const string& driverSchema,const string& version);

  static bool    loadDatabaseAttributes(const string& dsnname,const string& confname,__XDBC_ATTRIBUTES& props);
  
};

__OPENCODE_END_NAMESPACE

#endif
