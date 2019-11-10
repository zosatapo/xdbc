#include "xdbc/basicDataSource.h"
#include "xdbc/pooledPreparedStatement.h"
#include "xdbc/statement.h"
#include "xdbc/preparedStatement.h"
#include "xdbc/driverManager.h"
#include "xdbc/xsmart.h"
#include "xdbc/SQLSource.h"
#include "xdbc/XDBCUtilities.h"

#include "port/utilities.h"
#include "port/libnumber.h"

__OPENCODE_BEGIN_NAMESPACE

const int BasicDataSource::MAX_CONNS = 200;
const int BasicDataSource::MAX_CONNS_MIN = 1;
  
const int BasicDataSource::MAX_SPARE_CONNS = 100;
const int BasicDataSource::MIN_SPARE_CONNS = 1;
  
const int BasicDataSource::WORK_WAIT_TIMEOUT = 900*1000;
  
//------------------------------------------------------------------
//
//------------------------------------------------------------------
__DataSourceJob::~__DataSourceJob() {}
__DataSourceJob::__DataSourceJob(){}

void __DataSourceJob::run(JobData* jobData) {}

void __DataSourceJob::stop(){}

//------------------------------------------------------------------
//
//------------------------------------------------------------------

__DataSourceMonitorJob::~__DataSourceMonitorJob() 
{
}

__DataSourceMonitorJob::__DataSourceMonitorJob(BasicDataSource* ds)
:m_ds(ds)
{
  interval = BasicDataSource::WORK_WAIT_TIMEOUT;
  shouldTerminate = false;
}

void __DataSourceMonitorJob::setInterval(int i)
{
  interval = i;
}
  
void __DataSourceMonitorJob::stop()
{
  shouldTerminate = true;
}

void  __DataSourceMonitorJob::run(JobData* jobData) 
{
  while(true) 
  {
    if(shouldTerminate) 
    {
      break;
    }
    
    setInterval(m_ds->interval);
    Thread::sleep(interval);

    if(shouldTerminate) 
    {
      break;
    }

    if(m_ds != 0)
    {
      m_ds->checkSpareConns();
    }
  }
}


//------------------------------------------------------------------
//
//------------------------------------------------------------------

BasicDataSource::~BasicDataSource() 
{
}

BasicDataSource::BasicDataSource(const string& driver_name,const string& driver_version,const string& url,const string& user,const string& password)
:driver_name(driver_name),driver_version(driver_version),url(url),user(user),password(password)
{
  maxConns = MAX_CONNS;
  maxSpareConns = MAX_SPARE_CONNS;
  minSpareConns = MIN_SPARE_CONNS;

  currentConnCount = 0;
  currentConnsBusy = 0;

  stopTheDataSource = false;
  started = false;
  interval = -1;

  monitorThread = 0;
  __SQL_normalized = false;
}

//------------------------------------------------------------------
//
//------------------------------------------------------------------

void BasicDataSource::setMaxConns(int maxConnsValue) 
{
  this->maxConns = maxConnsValue;
}

int BasicDataSource::getMaxConns() 
{
  return maxConns;
}

void BasicDataSource::setMinSpareConns(int minSpareConnsValue) 
{
  this->minSpareConns = minSpareConnsValue;
}

int BasicDataSource::getMinSpareConns() 
{
  return minSpareConns;
}

void BasicDataSource::setMaxSpareConns(int maxSpareConnsValue) 
{
  this->maxSpareConns = maxSpareConnsValue;
}

int BasicDataSource::getMaxSpareConns() 
{
  return maxSpareConns;
}

int BasicDataSource::getCurrentConnCount() 
{
  return currentConnCount;
}

int BasicDataSource::getCurrentConnsBusy() 
{
  return currentConnsBusy;
}

void BasicDataSource::setInterval(int i)
{
  interval = i;
}

void BasicDataSource::setName(const string& name)
{
  __ds_name = name;
}

void BasicDataSource::normalizeSQL()
{
  if(__SQL_normalized)
  {
    return;
  }  
  
  Connection* __p_connection = 0; 
  
  __OPENCODE_TRY
  {
    __XDBC_HOOK_DEFINE();
    __p_connection = DriverManager::getConnection(__XDBC_HOOK_INVOKE_MORE driver_name,driver_version);  
  } 
  __OPENCODE_CATCH(SQLException& ex)
  {
    xdbc_smart_ptr<Connection> __sp_conn(__p_connection);
    __p_connection = 0;
  }
  
  xdbc_smart_ptr<Connection> __sp_conn(__p_connection);
  
  if(__p_connection != 0)
  {
    vector<string> __sql_keys ;
    SQLSourceImpl::getInstance().keys(__sql_keys);
    int __size = (int)__sql_keys.size();
    
    for(int i = 0; i < __size ; ++i)
    {
      const SQLData& __sqlData = SQLSourceImpl::getInstance().getSQLData(__sql_keys[i]);
      SQLData* __p_sqlData = (SQLData*)(&__sqlData);
      
      if((__sqlData.dbname.empty()) || (__sqlData.dbname == __ds_name))
      {          
        if(__p_sqlData->paramCount == SQLImplementation::paramCountUnknown)
        {
          __p_sqlData->SQL = __p_connection->normalizeSQL(__p_sqlData->raw_SQL,__p_sqlData->paramCount);
          
        }//~end  if(__p_sqlData->paramCount == SQLImplementation::paramCountUnknown)
        
      }//~end    if(__sqlData.dbname.emtpy() || (__sqlData.dbname == __ds_name))
      
    }// ~end for(int i = 0; i < __size ; ++i)  
        
    __SQL_normalized = true;
  }   
}

//------------------------------------------------------------------
//
//------------------------------------------------------------------

Connection* BasicDataSource::getConnection(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException)
{
  PooledConnection* __p_conn = 0;

  if ( stopTheDataSource || !started) 
  {
    __XDBC_HOOK_RETURN(return 0,"BasicDataSource::getConnection() - stopped"); 
  }

  Mutex::Guard g(*(__p_ds_mutex.get()));

  while (currentConnsBusy == currentConnCount) 
  {
     // All connections are busy
    if (currentConnCount < maxConns) 
    {
      // Not all connections were open,
      // Open new threads up to the max number of idle connections
      int toOpen = currentConnCount + minSpareConns;
      openConns(__XDBC_HOOK_INVOKE_MORE toOpen);
      __XDBC_HOOK_CHECK(return 0);
    } 
    else 
    {
      // Wait for a connection to become idel.
      __p_ds_condition->wait(*(__p_ds_mutex.get()));
      
      // Pool was stopped. Get away of the pool.
      if( stopTheDataSource) 
      {
         break;
      }
    }

  }//~end while (currentConnsBusy == currentConnCount)

  // Pool was stopped. Get away of the DataSource.
  if(0 == currentConnCount || stopTheDataSource) 
  {

    __XDBC_HOOK_RETURN(return 0,"BasicDataSource stopped"); 
  }
          
  // If we are here it means that there is a free connection. Take it.
  int pos = currentConnCount - currentConnsBusy - 1;
  __p_conn = ds_conns_idle[pos];

  ds_conns_idle[pos] = 0;
  currentConnsBusy++;

  return __p_conn;   
}

//------------------------------------------------------------------
//
//------------------------------------------------------------------

void BasicDataSource::start(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException)
{
  if(started)
  {
    return;
  }

  __p_ds_mutex.reset(new Mutex());
  __p_ds_condition.reset(new Condition());
  
  stopTheDataSource = false;

  currentConnCount  = 0;
  currentConnsBusy  = 0;
  
  adjustLimits();

  ds_conns_idle.resize(maxConns);

  normalizeSQL();
    
  openConns(__XDBC_HOOK_INVOKE_MORE minSpareConns);
  __XDBC_HOOK_CHECK(return);
  
  if(interval > 0)
  {
    __DataSourceMonitorJob monitorJob(this);
    monitorThread = new Thread(&monitorJob,0);
    monitorThread->start();
  }
  
  started = true;
}

void BasicDataSource::stop(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException)
{
  if(!started)
  {
    return;
  }

  if(!stopTheDataSource) 
  {
    stopTheDataSource = true;

    
    if(monitorThread != 0)
    {
      monitorThread->stop();
      monitorThread->join();
    }
    
    currentConnsBusy = currentConnCount = 0;
    __p_ds_condition->broadcast(); 
  }

  cleanup();  
  started = false;
}

//------------------------------------------------------------------
//
//------------------------------------------------------------------

void BasicDataSource::adjustLimits()
{
  if(maxConns <= 0) 
  {
    maxConns = MAX_CONNS;
  } 
  else if (maxConns < MAX_CONNS_MIN) 
  {      
    maxConns = MAX_CONNS_MIN;
  }
  
  if(maxSpareConns >= maxConns) 
  {
    maxSpareConns = maxConns;
  }
  
  if(maxSpareConns <= 0) 
  {
    if(1 == maxConns) 
    {
      maxSpareConns = 1;
    } 
    else 
    {
      maxSpareConns = maxConns/2;
    }
  }
  
  if(minSpareConns >  maxSpareConns) 
  {
    minSpareConns =  maxSpareConns;
  }
  
  if(minSpareConns <= 0) 
  {
    if(1 == maxSpareConns) 
    {
      minSpareConns = 1;
    } 
    else 
    {
      minSpareConns = maxSpareConns / 2;
    }
  }
}

void BasicDataSource::openConns(__XDBC_HOOK_DECLARE_MORE int toOpen) __XDBC_THROW_DECLARE(SQLException)
{
  XLONG __begin = System::currentTimeMillis();
  
  if(toOpen > maxConns) 
  {
    toOpen = maxConns;
  }
          
  SQLException __sqlex_save;
  
  int __toOpen = 0;
  Connection* __p_connection = 0;
  
  for(int i = currentConnCount ; i < toOpen ; i++) 
  {
    __p_connection = DriverManager::getConnection(__XDBC_HOOK_INVOKE_MORE driver_name,driver_version);
    __XDBC_HOOK_CHECK(return);  

    __OPENCODE_TRY
    {
      __p_connection->connect(__XDBC_HOOK_INVOKE_MORE url,user,password);
      if(!__XDBC_HOOK_INVOKE_CHECKPASSED)
      {
        #ifdef OPENCODE_DISABLE_XDBC_SEH  
        __sqlex_save = __XDBC_HOOK_INVOKE_ONCE;
        #endif

        break;
      }
    }
    __OPENCODE_CATCH(SQLException& ex)
    {
      __sqlex_save = ex;
      xdbc_smart_ptr<Connection> __sp(__p_connection);
      break;
    }    
    
    PooledConnection* __p_pooled_conn = new PooledConnection(this,__p_connection);
  
//////////////////////////////////////////////////////////////////////////////////////////////

#if !defined(__XDBC_NO_PSTMTCACHE)

    PreparedStatement* __p_pstmt = 0;  
    
    vector<string> __sql_keys ;
    SQLSourceImpl::getInstance().keys(__sql_keys);
    int __size = (int)__sql_keys.size(); 
    
    for(int k = 0; k < __size ; ++k)
    {
      const SQLData& __sqlData = SQLSourceImpl::getInstance().getSQLData(__sql_keys[k]);
      
      if((__sqlData.dbname.empty()) || (__sqlData.dbname == __ds_name))
      {          
        __OPENCODE_TRY
        {
          __p_pstmt = __p_connection->prepareStatement(__XDBC_HOOK_INVOKE_MORE __sqlData.SQL,__sqlData.paramCount);
          if(__XDBC_HOOK_INVOKE_CHECKPASSED)
          {
            __p_pstmt->setAttribute(XDBC_SQLVALUE_KEY,__sqlData.SQL);        
            __p_pstmt->setAttribute(XDBC_SQLKEY_KEY,__sql_keys[k]);
  
            PooledPreparedStatement* __p_pooled_pstmt = new PooledPreparedStatement(__p_pstmt);
            if(!__p_pooled_conn->insertPooledPreparedStatement(__sql_keys[k],__sqlData.SQLID,__p_pooled_pstmt))
            {
              __p_pooled_pstmt->pooledClose();
              __p_pooled_pstmt->pooledDestroy();        
            }
            else 
            {
              StringBuffer __sb;
              __sb << "BasicDataSource::openConns|preparedStatement[" << __sql_keys[k] << "]"  ;
              LOGGING_DEBUG(__sb.str());
            }
          }
          else
          {
            StringBuffer __sb;
            #ifdef OPENCODE_DISABLE_XDBC_SEH     
            __sb << "BasicDataSource::openConns|prepareStatement[" << __sql_keys[k] << "]::" << __XDBC_HOOK_INVOKE_ONCE.getMessage();
            #endif

            LOGGING_ERROR(__sb.str());             
          }    
        }             
        __OPENCODE_CATCH(SQLException& ex)
        {
          StringBuffer __sb;
          __sb << "BasicDataSource::openConns|prepareStatement[" << __sql_keys[k] << "]::" << ex.getMessage();
          LOGGING_ERROR(__sb.str());
        }
                    
      }//~end    if(__sqlData.dbname.emtpy() || (__sqlData.dbname == __ds_name))
      
    }// ~end for(int k = 0; k < __size ; ++k)  
    
#endif
    
//////////////////////////////////////////////////////////////////////////////////////////////    
    ds_conns.push_back(__p_pooled_conn);
    ds_conns_idle[i - currentConnsBusy] = __p_pooled_conn;
    ++__toOpen;
    ++currentConnCount;    
  }

  {
    XLONG __end = System::currentTimeMillis(); 
    StringBuffer __sb;
    __sb << "BasicDataSource::openConns|" << __toOpen << "::" << (__end - __begin) << " ms"  ;
    LOGGING_DEBUG(__sb.str());
  }  
  
  if(__toOpen == 0)
  {
    StringBuffer __sb;
    __sb << "[BasicDataSource::openConns] " << __sqlex_save.getMessage();
    __XDBC_HOOK_RETURN(return,__sb.str()); 
  }    
}

void BasicDataSource::checkSpareConns()
{
  if(stopTheDataSource) 
  {
    return;
  }
  
  Mutex::Guard g(*(__p_ds_mutex.get()));
  
  if((currentConnCount - currentConnsBusy) > maxSpareConns) 
  {
    int toFree = currentConnCount - currentConnsBusy - maxSpareConns;
    
    for(int i = 0 ; i < toFree ; i++) 
    {
      PooledConnection* __p_conn =  ds_conns_idle[currentConnCount - currentConnsBusy - 1];
      removeDataSource(__p_conn);

      ds_conns_idle[currentConnCount - currentConnsBusy - 1] = 0;
      currentConnCount --;

    }//~end for(int i = 0 ; i < toFree ; i++) 

  }//~end if((currentConnCount - currentConnsBusy) > maxSpareConns) 
}

void BasicDataSource::returnDataSource(PooledConnection* pConnection)
{
  Mutex::Guard g(*(__p_ds_mutex.get()));

//----------------------------------------------------------
//
//----------------------------------------------------------
  if(0 == currentConnCount || stopTheDataSource) 
  {
    removeDataSource(pConnection);
    return;
  }

//----------------------------------------------------------
//
//----------------------------------------------------------
  if(pConnection->pingDatabase(0) != 0)
  {
    removeDataSource(pConnection);
    currentConnCount --;
    currentConnsBusy--;
    return;
  }
    
  // atomic
  currentConnsBusy--;
  ds_conns_idle[currentConnCount - currentConnsBusy - 1] = pConnection;  
  
  __p_ds_condition->signal();  
}

void BasicDataSource::removeDataSource(PooledConnection* pConnection)
{
  vector<PooledConnection*>::iterator pos = std::find(ds_conns.begin(),ds_conns.end(),pConnection);
  if(pos != ds_conns.end())
  {
    ds_conns.erase(pos);
  }
  
  pConnection->pooledClose();
  pConnection->pooledDestroy();
}

void BasicDataSource::cleanup()
{
  int size = (int)ds_conns.size();
  for(int i = 0;i < size;++i)
  {
    if(ds_conns[i] != 0)
    {
      ds_conns[i]->pooledClose();
      ds_conns[i]->pooledDestroy();
    }
  }
  ds_conns.clear();

  if(monitorThread != 0)
  {
    delete monitorThread;
    monitorThread = 0;
  }

  stopTheDataSource = true;
}

BasicDataSource*  BasicDataSource::createDataSource(const __XDBC_ATTRIBUTES& props)
{
  string drvier;
  string version;
  string database;
  string user;
  string password;

  string  __maxConns ;
  string  __minSpareConns ;
  string  __maxSpareConns ;
  string  __interval ;
  
  __XDBC_ATTRIBUTES::const_iterator __pos = props.begin();
  __XDBC_ATTRIBUTES::const_iterator __end = props.end();
 
  while(__pos != __end)
  {

    if(StringsKit::equalsIgnoreCase(__pos->first.c_str(),"driverSchema"))
    {
      drvier = StringsKit::trim(__pos->second);
    }
    else if(StringsKit::equalsIgnoreCase(__pos->first.c_str(),"version"))
    {
      version = StringsKit::trim(__pos->second);
    }
    else if(StringsKit::equalsIgnoreCase(__pos->first.c_str(),"database"))
    {
      database = StringsKit::trim(__pos->second);
    }
    else if(StringsKit::equalsIgnoreCase(__pos->first.c_str(),"user"))
    {
      user = StringsKit::trim(__pos->second);
    }
    else if(StringsKit::equalsIgnoreCase(__pos->first.c_str(),"password"))
    {
      password = StringsKit::trim(__pos->second);
    }
    else if(StringsKit::equalsIgnoreCase(__pos->first.c_str(),"maxconns"))
    {
      __maxConns = StringsKit::trim(__pos->second);
    }
    else if(StringsKit::equalsIgnoreCase(__pos->first.c_str(),"minspareconns"))
    {
      __minSpareConns = StringsKit::trim(__pos->second);
    }
    else if(StringsKit::equalsIgnoreCase(__pos->first.c_str(),"maxspareconns"))
    {
      __maxSpareConns = StringsKit::trim(__pos->second);
    }
    else if(StringsKit::equalsIgnoreCase(__pos->first.c_str(),"checkinterval"))
    {
      __interval = StringsKit::trim(__pos->second);
    }
 
    ++__pos;
  }
    
    
  BasicDataSource* __pDataSource = new BasicDataSource(drvier,version,database,user,password);
  
  if(!__maxConns.empty())
  {
    __OPENCODE_TRY
    {
      int maxConns = Number::parseInt(__maxConns);
      __pDataSource->setMaxConns(maxConns);
    }
    __OPENCODE_CATCH(NumberFormatException& ex)
    {
    }  
  }
  

  if(!__minSpareConns.empty())
  {
    __OPENCODE_TRY
    {
      int minSpareConns = Number::parseInt(__minSpareConns);
      __pDataSource->setMinSpareConns(minSpareConns);
    }
    __OPENCODE_CATCH(NumberFormatException& ex)
    {
    }  
  }

  if(!__maxSpareConns.empty())
  {
    __OPENCODE_TRY
    {
      int maxSpareConns = Number::parseInt(__maxSpareConns);
      __pDataSource->setMaxSpareConns(maxSpareConns);
    }
    __OPENCODE_CATCH(NumberFormatException& ex)
    {
    }  
  }

  if(!__interval.empty())
  {
    __OPENCODE_TRY
    {
      int interval = Number::parseInt(__interval);
      __pDataSource->setInterval(interval);
    }
    __OPENCODE_CATCH(NumberFormatException& ex)
    {
    }  
  }

  return __pDataSource;
}

__OPENCODE_END_NAMESPACE

