#include "xdbc/pooledConnection.h"
#include "xdbc/resultset.h"
#include "xdbc/preparedStatement.h"
#include "xdbc/pooledPreparedStatement.h"
#include "xdbc/basicDataSource.h"
#include "xdbc/XDBCUtilities.h"

__OPENCODE_BEGIN_NAMESPACE

PooledConnection::~PooledConnection()
{ 
}

PooledConnection::PooledConnection(BasicDataSource *pDataSource,Connection*  pConnection)
:__p_dataSource(pDataSource),__p_connection(pConnection)
{
  __bol_isClosed = false;
  __bol_oldAutoCommit = true;

  if(__p_connection != 0)
  {
    __OPENCODE_TRY
    {
      __XDBC_HOOK_DEFINE();
      __bol_oldAutoCommit = __p_connection->getAutoCommit(__XDBC_HOOK_INVOKE_ONCE);
      if(!__XDBC_HOOK_INVOKE_CHECKPASSED)
      {
       __bol_oldAutoCommit = true;
      }
    }
    __OPENCODE_CATCH(SQLException& ex)
    {
      __bol_oldAutoCommit = true;
    }  
  }
}

bool PooledConnection::connect(__XDBC_HOOK_DECLARE_MORE const string& url) __XDBC_THROW_DECLARE(SQLException)
{
  return __p_connection->connect(__XDBC_HOOK_INVOKE_MORE url);
}

bool PooledConnection::connect(__XDBC_HOOK_DECLARE_MORE const string& url, const string& user, const string& password) __XDBC_THROW_DECLARE(SQLException)
{
  return __p_connection->connect(__XDBC_HOOK_INVOKE_MORE url,user,password);
}

bool PooledConnection::connect(__XDBC_HOOK_DECLARE_MORE const string& url, __XDBC_ATTRIBUTES& info) __XDBC_THROW_DECLARE(SQLException)
{
  return __p_connection->connect(__XDBC_HOOK_INVOKE_MORE url,info);
}

string PooledConnection::normalizeSQL(const string& sql,int& paramCount)
{
  return __p_connection->normalizeSQL(sql,paramCount);
}

Statement* PooledConnection::createStatement(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException)
{
  return __p_connection->createStatement(__XDBC_HOOK_INVOKE_ONCE);
}

PreparedStatement* PooledConnection::prepareStatement(__XDBC_HOOK_DECLARE_MORE const string& sql,int paramCount) __XDBC_THROW_DECLARE(SQLException)
{
  PreparedStatement* __p_pstmt = 0;
  
#if !defined(__XDBC_NO_PSTMTCACHE)
  __p_pstmt = findPooledPreparedStatement(sql);
  if(__p_pstmt != 0)
  {
    return __p_pstmt; 
  }
#endif

  __p_pstmt = __p_connection->prepareStatement(__XDBC_HOOK_INVOKE_MORE sql,paramCount); 
  
  return __p_pstmt;
}

CallableStatement* PooledConnection::prepareCall(__XDBC_HOOK_DECLARE_MORE const string& sql,int paramCount) __XDBC_THROW_DECLARE(SQLException)
{
  return __p_connection->prepareCall(__XDBC_HOOK_INVOKE_MORE sql,paramCount);
}

ArrayStatement* PooledConnection::prepareArray(__XDBC_HOOK_DECLARE_MORE const string& sql,int paramCount) __XDBC_THROW_DECLARE(SQLException)
{
  return __p_connection->prepareArray(__XDBC_HOOK_INVOKE_MORE sql,paramCount);
}

void PooledConnection::setAutoCommit(__XDBC_HOOK_DECLARE_MORE bool autoCommit) __XDBC_THROW_DECLARE(SQLException)
{
  __p_connection->setAutoCommit(__XDBC_HOOK_INVOKE_MORE autoCommit);
}

bool PooledConnection::getAutoCommit(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException)
{
  return __p_connection->getAutoCommit(__XDBC_HOOK_INVOKE_ONCE);
}


void PooledConnection::commit(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException)
{
  __p_connection->commit(__XDBC_HOOK_INVOKE_ONCE);
}

void PooledConnection::rollback(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException)
{
  __p_connection->rollback(__XDBC_HOOK_INVOKE_ONCE);
}

int PooledConnection::pingDatabase(int timeout)
{
  
  return __p_connection->pingDatabase(timeout); 
}

void PooledConnection::close()
{  
  //数据库对象缓存机制关键技术
  
  if(__bol_isClosed)
  {
    return;
  } 

  __OPENCODE_TRY
  {
    __XDBC_HOOK_DEFINE();
    
    bool __xx_autocommit = __p_connection->getAutoCommit(__XDBC_HOOK_INVOKE_ONCE); 
    if(__XDBC_HOOK_INVOKE_CHECKPASSED)
    {
      if(__bol_oldAutoCommit != __xx_autocommit)
      {
        __p_connection->setAutoCommit(__XDBC_HOOK_INVOKE_MORE __bol_oldAutoCommit);
      }  
    }    
  }
  __OPENCODE_CATCH(SQLException& ex)
  {
  } 
 
  if(__p_dataSource != 0)
  {
    __p_dataSource->returnDataSource(this);
  }
  else
  {
    __XDBC_INTERFACE_CLOSE(__p_connection);
  }
  
  __bol_isClosed = true;  
}

void PooledConnection::destroy()
{  
  //数据库对象缓存机制关键技术
  if(__p_dataSource == 0)
  {
    __XDBC_INTERFACE_DESTROY(__p_connection);
    __p_connection = 0;
    delete this;
  }  
}

bool PooledConnection::isClosed()
{
  return __bol_isClosed;
}


//======================================================================
//   XDBC Connection Advanced features:
//======================================================================

void PooledConnection::setTransactionIsolation(__XDBC_HOOK_DECLARE_MORE int level) __XDBC_THROW_DECLARE(SQLException)
{
  __p_connection->setTransactionIsolation(__XDBC_HOOK_INVOKE_MORE level);
}

int PooledConnection::getTransactionIsolation(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException)
{
  return __p_connection->getTransactionIsolation(__XDBC_HOOK_INVOKE_ONCE);
}


Statement* PooledConnection::createStatement(__XDBC_HOOK_DECLARE_MORE int resultSetType, int resultSetConcurrency)__XDBC_THROW_DECLARE(SQLException)
{
  return __p_connection->createStatement(__XDBC_HOOK_INVOKE_MORE resultSetType,resultSetConcurrency);
}

PreparedStatement* PooledConnection::prepareStatement(__XDBC_HOOK_DECLARE_MORE const string& sql, int resultSetType, int resultSetConcurrency,int paramCount)__XDBC_THROW_DECLARE(SQLException)
{
  PreparedStatement* __p_pstmt = 0;
  
#if !defined(__XDBC_NO_PSTMTCACHE)
  __p_pstmt = findPooledPreparedStatement(sql);
  if(__p_pstmt != 0)
  {
    return __p_pstmt; 
  }
#endif

  __p_pstmt = __p_connection->prepareStatement(__XDBC_HOOK_INVOKE_MORE sql,resultSetType,resultSetConcurrency,paramCount); 
  
  return __p_pstmt;
}

CallableStatement* PooledConnection::prepareCall(__XDBC_HOOK_DECLARE_MORE const string& sql, int resultSetType, int resultSetConcurrency,int paramCount) __XDBC_THROW_DECLARE(SQLException)
{
  return __p_connection->prepareCall(__XDBC_HOOK_INVOKE_MORE sql,resultSetType,resultSetConcurrency,paramCount);
}

ArrayStatement* PooledConnection::prepareArray(__XDBC_HOOK_DECLARE_MORE const string& sql, int resultSetType, int resultSetConcurrency,int paramCount)__XDBC_THROW_DECLARE(SQLException)
{
  return __p_connection->prepareArray(__XDBC_HOOK_INVOKE_MORE sql,resultSetType,resultSetConcurrency,paramCount);
}

PreparedStatement*  PooledConnection::findPooledPreparedStatement(const string& key)
{
  OPENCODE_MAP<string,PooledPreparedStatement*>::iterator __cached = __pstmt_cache.find(key);
  if(__cached != __pstmt_cache.end())
  {
    return __cached->second;
  } 
  
  return 0; 
}

PreparedStatement*  PooledConnection::findPooledPreparedStatement(int sqlid)
{
  OPENCODE_MAP<int,PooledPreparedStatement*>::iterator __cached = __pstmt_cache_fast.find(sqlid);
  if(__cached != __pstmt_cache_fast.end())
  {
    return __cached->second;
  } 
  
  return 0;   
}
  
bool  PooledConnection::insertPooledPreparedStatement(const string& key,int sqlid,PooledPreparedStatement* pstmt)
{
  OPENCODE_MAP<string,PooledPreparedStatement*>::iterator __cached = __pstmt_cache.find(key);
  if(__cached != __pstmt_cache.end())
  {
    return false;
  }
  
  __pstmt_cache[key] = pstmt;
  pstmt->__pooled_ref_count++;

  OPENCODE_MAP<int,PooledPreparedStatement*>::iterator __cached_fast = __pstmt_cache_fast.find(sqlid);
  if(__cached_fast == __pstmt_cache_fast.end())
  {
    __pstmt_cache_fast[sqlid] = pstmt;
  }
  
  return true;
}

void PooledConnection::pooledClose()
{
  OPENCODE_MAP<string,PooledPreparedStatement*>::iterator __pos = __pstmt_cache.begin();
  OPENCODE_MAP<string,PooledPreparedStatement*>::iterator __end = __pstmt_cache.end();

  while(__pos != __end)
  {
    __pos->second->pooledClose();    
    ++__pos;
  }

  __XDBC_INTERFACE_CLOSE(__p_connection);
}
  
void PooledConnection::pooledDestroy()
{  
  OPENCODE_MAP<string,PooledPreparedStatement*>::iterator __pos = __pstmt_cache.begin();
  OPENCODE_MAP<string,PooledPreparedStatement*>::iterator __end = __pstmt_cache.end();

  while(__pos != __end)
  {
    __pos->second->__pooled_ref_count--;
    if(__pos->second->__pooled_ref_count == 0)
    {
      __pos->second->pooledDestroy();  
    }  
    ++__pos;
  }

  __pstmt_cache.clear();
  
  __XDBC_INTERFACE_DESTROY(__p_connection); 
  delete this;
}   
__OPENCODE_END_NAMESPACE
