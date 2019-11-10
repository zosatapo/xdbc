/**
 *  basicDataSource.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */

#ifndef   __OPENCODE_XDBC_BASICDATASOURCE_H__
#define   __OPENCODE_XDBC_BASICDATASOURCE_H__

#include "xdbc/xdefine.h"

#include "xdbc/connection.h"
#include "xdbc/pooledConnection.h"
#include "xdbc/dataSource.h"

#include "port/mutex.h"
#include "port/condition.h"
#include "port/thread.h"
#include "port/ioc_vector.h"

__OPENCODE_BEGIN_NAMESPACE

class __DataSourceMonitorJob;

class __OPENCODE_XDBC_API_DECL BasicDataSource : public DataSource 
{
public:
  friend class PooledConnection;
  friend class __DataSourceMonitorJob;

protected:
  vector<PooledConnection*> ds_conns;
  vector<PooledConnection*> ds_conns_idle;
  vector<PooledConnection*> ds_to_removed;
  
  string driver_name;
  string driver_version;
  string url;
  string user;
  string password;

  mutable smart_ptr<Mutex> __p_ds_mutex;
  mutable smart_ptr<Condition>  __p_ds_condition;

  /*
   * name of datasource
   */
  string  __ds_name;
  bool   __SQL_normalized;
  
  /*
   * A monitor thread that monitors the pool for idel threads.
   */
  Thread* monitorThread;
 
  /*
   * Max number of threads that you can open in the pool.
   */
  int maxConns;
  
  /*
   * Min number of idel threads that you can leave in the pool.
   */
  int minSpareConns;
  
  /*
   * Max number of idel threads that you can leave in the pool.
   */
  int maxSpareConns;
  
  /*
   * Number of threads in the pool.
   */
  int currentConnCount;
  
  /*
   * Number of busy threads in the pool.
   */
  int currentConnsBusy;
  
  /*
   * Flag that the DataSource should terminate all the conns and stop.
   */
  bool stopTheDataSource;

  bool started;
  int interval;

//-------------------------------------------------------
//
//-------------------------------------------------------
public:
  static const int MAX_CONNS;
  static const int MAX_CONNS_MIN;
  
  static const int MAX_SPARE_CONNS;
  static const int MIN_SPARE_CONNS;
  
  static const int WORK_WAIT_TIMEOUT;

public:
  virtual ~BasicDataSource() ;
  BasicDataSource(const string& driver_name,const string& driver_version,const string& url,const string& user,const string& password);

//-------------------------------------------------------
//
//-------------------------------------------------------
public:
  void setMaxConns(int maxConns);
  int getMaxConns();
  
  void setMinSpareConns(int minSpareConns);
  int getMinSpareConns();
  
  void setMaxSpareConns(int maxSpareConns);
  int getMaxSpareConns();
  
  int getCurrentConnCount();
  
  int getCurrentConnsBusy();

  void setInterval(int i);

  void setName(const string& name);
  void normalizeSQL();
  
public:
  virtual Connection* getConnection(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException);

  virtual void start(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException);
  virtual void stop(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException);

public:
  static BasicDataSource* createDataSource(const __XDBC_ATTRIBUTES& props);
    
//-------------------------------------------------------
//
//-------------------------------------------------------
protected:
  void adjustLimits();
  void openConns(__XDBC_HOOK_DECLARE_MORE int toOpen) __XDBC_THROW_DECLARE(SQLException); 

  void checkSpareConns();
  void returnDataSource(PooledConnection* pConnection);
  void removeDataSource(PooledConnection* pConnection);

  void cleanup();
};

//=======================================================================
//
//=======================================================================

class __OPENCODE_XDBC_API_DECL __DataSourceJob : public Job
{
public:
  virtual ~__DataSourceJob() ;
  __DataSourceJob();

protected:
  virtual void run(JobData* jobData) ;
  virtual void stop(); 
};

class __OPENCODE_XDBC_API_DECL __DataSourceMonitorJob : public Job
{
public:
  virtual ~__DataSourceMonitorJob() ;

  __DataSourceMonitorJob(BasicDataSource* ds);

  void setInterval(int i);
  void stop();

protected:
  virtual void run(JobData* jobData) ;

private:
  BasicDataSource*  m_ds;
  int interval;

  bool    shouldTerminate;
};
__OPENCODE_END_NAMESPACE

#endif
