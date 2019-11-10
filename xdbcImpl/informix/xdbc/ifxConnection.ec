#include "xdbc/ifxConnection.h"
#include "xdbc/ifxStatement.h"
#include "xdbc/ifxPreparedStatement.h"
#include "xdbc/ifxCallableStatement.h"
#include "xdbc/ifxArrayStatement.h"
#include "xdbc/ifxResultSet.h"
#include "xdbc/ifxConversion.h"
#include "xdbc/ifxSQL.h"
#include "xdbc/xsmart.h"

#include "port/ioc_sbuf.h"
#include "port/stringTokenizer.h"
#include "port/thread.h"

#include <algorithm>
USING_OPENCODE_STL_NS

__OPENCODE_BEGIN_NAMESPACE

//-------------------------------------------------------------------------

#define IFXCONNECTION_CLASS_NAME "IfxConnection"
Mutex   IfxConnection::__go_connNumber_mutex;
int     IfxConnection::__gi_connNumber = 1;

//-------------------------------------------------------------------------

IfxConnection::~IfxConnection()
{
  __XDBC_TRACE(IFXCONNECTION_CLASS_NAME,"destroy()");
}

IfxConnection::IfxConnection()
{
  __mb_isClosed=false;
  __mi_transLevel=Connection::TRANSACTION_NONE;
  __mb_autoCommit=true;

  __mb_explicit_trans = false;
}

bool IfxConnection::connect(__XDBC_HOOK_DECLARE_MORE const string& url)  __XDBC_THROW_DECLARE(SQLException)
{
  __XDBC_ATTRIBUTES props;

  bool doChange = true;
  string newURL = XDBCUtilities::parseURL(props,url,doChange);

  return connect(__XDBC_HOOK_INVOKE_MORE newURL,props);
}

bool IfxConnection::connect(__XDBC_HOOK_DECLARE_MORE const string& url, const string& user, const string& password)  __XDBC_THROW_DECLARE(SQLException)
{
  __XDBC_ATTRIBUTES props;
  
  bool doChange = true;
  string newURL = XDBCUtilities::parseURL(props,url,doChange);

  props[XDBC_USER] = user;
  props[XDBC_PASSWORD] = password;

  return  connect(__XDBC_HOOK_INVOKE_MORE url,props);
}

bool IfxConnection::connect(__XDBC_HOOK_DECLARE_MORE const string& url,__XDBC_ATTRIBUTES& info)  __XDBC_THROW_DECLARE(SQLException)
{
  string user ;
  __XDBC_ATTRIBUTES::iterator __pos = info.find(XDBC_USER);
  if(__pos != info.end())
  {
    user = __pos->second;
  }

  string password ;
  __pos = info.find(XDBC_PASSWORD);
  if(__pos != info.end())
  {
    password = __pos->second;
  }  

  const char* sz_user = user.c_str();
  const char* sz_password = password.c_str();

  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    __sb.append("connect|");
    __sb.append(url);
    __sb.append("::");
    __sb.append(user);

    __XDBC_FORCED_TRACE(IFXCONNECTION_CLASS_NAME,__sb.str());
  }

  StringTokenizer tokens(url,"/");
  if(tokens.countTokens() < 2)
  {
    StringBuffer __sb;
    __sb.append("connect() failed,bad connection URL : ");
    __sb.append(url);

    __XDBC_HOOK_RETURN(return false,__sb.str());
  }
 
  tokens.hasMoreTokens();
  string server = tokens.nextToken();

  tokens.hasMoreTokens();
  string database = tokens.nextToken();

  string full_database;
  full_database.append(database);
  full_database.append("@").append(server); 
   
  EXEC SQL BEGIN DECLARE SECTION;
    char* ec_database = 0;
    char* ec_username = 0; 
    char* ec_password = 0; 
    char* ec_connName = 0; 
  EXEC SQL END DECLARE SECTION;

  ec_database = (char*)full_database.c_str();
  //ec_database = (char*)database.c_str();

  ec_username = (char*)sz_user;
  ec_password = (char*)sz_password;
 
  __ms_connName = getConnName(); 
  ec_connName = (char*)__ms_connName.c_str();


  EXEC SQL CONNECT TO :ec_database  as :ec_connName USER :ec_username USING :ec_password;

/*
  if(old_informix_server != 0)
  {
    SystemTools::setProperty("INFORMIXSERVER",old_informix_server);    
    delete old_informix_server;
    old_informix_server = 0;
  }
*/

  IfxConversion::checkException(__XDBC_HOOK_INVOKE_MORE SQLCODE,SQLSTATE,IFXCONNECTION_CLASS_NAME);
  __XDBC_HOOK_CHECK(return false);

  __mo_attributes[XDBC_URL] = url;
  __mo_attributes[XDBC_USER] = user;

  __mo_attributes[XDBC_DRIVER] = "informix/esql";
  __mo_attributes[XDBC_VERSION] = "1.0.0";
  
  __mo_attributes["informix.server"] = server;
  __mo_attributes["informix.database"] = database.c_str();


  __XDBC_TRACE(IFXCONNECTION_CLASS_NAME,"connect|connected");

  return true;
}

string IfxConnection::normalizeSQL(const string& sql,int& paramCount)
{
  __XDBC_HOOK_DEFINE();
  string __ret_SQL = IfxSQL::parseSQL(__XDBC_HOOK_INVOKE_MORE sql,paramCount);
  return __ret_SQL;
}

void IfxConnection::close()
{
  __XDBC_TRACE(IFXCONNECTION_CLASS_NAME,"close()");

  if(__mb_isClosed)
  {
    return;
  }

  
  EXEC SQL BEGIN DECLARE SECTION;
    char* ec_connName = 0;
  EXEC SQL END DECLARE SECTION;

  ec_connName = (char*)__ms_connName.c_str();
  
  EXEC SQL DISCONNECT :ec_connName;
  
  __mb_isClosed=true;
}

void IfxConnection::destroy()
{
  delete this;
}

bool IfxConnection::isClosed()
{   
  return __mb_isClosed;
}

int IfxConnection::pingDatabase(int timeout)
{
  int retcode = 0;
  __XDBC_HOOK_DEFINE();

  __OPENCODE_TRY
  {  
    Statement* __p_stmt = createStatement(__XDBC_HOOK_INVOKE_ONCE);
    xdbc_smart_ptr<Statement> __sp_stmt(__p_stmt);
    __XDBC_HOOK_CHECK(return -1);
    
    ResultSet*  __p_rs = __p_stmt->executeQuery(__XDBC_HOOK_INVOKE_MORE "SELECT 'x' FROM DUAL");
    xdbc_smart_ptr<ResultSet> __sp_rs(__p_rs);
    __XDBC_HOOK_CHECK(return -1); 
  }
  __OPENCODE_CATCH(SQLException& ex)
  {
    retcode = -1;   
  }
  
  return retcode;
}

Statement* IfxConnection::createStatement(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{
  IfxStatement* __p_stmt = new IfxStatement(this);
  xdbc_smart_ptr<IfxStatement> __sp_stmt(__p_stmt);
  
  __p_stmt->initialize(__XDBC_HOOK_INVOKE_ONCE);
  __XDBC_HOOK_CHECK(return 0);
    
  return __sp_stmt.release();
}

PreparedStatement* IfxConnection::prepareStatement(__XDBC_HOOK_DECLARE_MORE const string& sql,int paramCount)  __XDBC_THROW_DECLARE(SQLException)
{
  IfxPreparedStatement* __p_pstmt = new IfxPreparedStatement(this,sql,paramCount);
  xdbc_smart_ptr<IfxPreparedStatement> __sp_pstmt(__p_pstmt);

  __p_pstmt->initialize(__XDBC_HOOK_INVOKE_ONCE);
  __XDBC_HOOK_CHECK(return 0);

  return __sp_pstmt.release();
}

CallableStatement* IfxConnection::prepareCall(__XDBC_HOOK_DECLARE_MORE const string& sql,int paramCount)  __XDBC_THROW_DECLARE(SQLException)
{
  IfxCallableStatement* __p_call= new IfxCallableStatement(this,sql,paramCount);
  xdbc_smart_ptr<IfxCallableStatement> __sp_call(__p_call);
 
  __p_call->initialize(__XDBC_HOOK_INVOKE_ONCE);
  __XDBC_HOOK_CHECK(return 0);
  
  return __sp_call.release();
}

ArrayStatement* IfxConnection::prepareArray(__XDBC_HOOK_DECLARE_MORE const string& sql,int paramCount)  __XDBC_THROW_DECLARE(SQLException)
{
  IfxArrayStatement* __p_array = new IfxArrayStatement(this,sql,paramCount);
  xdbc_smart_ptr<IfxArrayStatement> __sp_array(__p_array);

  __p_array->initialize(__XDBC_HOOK_INVOKE_ONCE);
  __XDBC_HOOK_CHECK(return 0);

  return __sp_array.release();
}

void IfxConnection::setAutoCommit(__XDBC_HOOK_DECLARE_MORE bool autoCommit)  __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    __sb.append("setAutoCommit|Old:");
    __sb.append(__mb_autoCommit);
    __sb.append("New:");
    __sb.append(autoCommit);

    __XDBC_FORCED_TRACE(IFXCONNECTION_CLASS_NAME,__sb.str());
  }

  if(autoCommit)
  {
    if(!__mb_autoCommit && __mb_explicit_trans)
    {
      commit(__XDBC_HOOK_INVOKE_ONCE);
    }
  }
  else
  {
    if(__mb_autoCommit || !__mb_explicit_trans)
    {
      EXEC SQL BEGIN WORK;
      IfxConversion::checkException(__XDBC_HOOK_INVOKE_MORE SQLCODE,SQLSTATE,IFXCONNECTION_CLASS_NAME);
      __XDBC_HOOK_CHECK(return);
      
      __mb_explicit_trans = true;
    }
  }
 
  __mb_autoCommit=autoCommit;
}

bool IfxConnection::getAutoCommit(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    __sb.append("getAutoCommit|");
    __sb.append(__mb_autoCommit);

    __XDBC_FORCED_TRACE(IFXCONNECTION_CLASS_NAME,__sb.str());
  }

  return __mb_autoCommit;
}

void IfxConnection::commit(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{
  __XDBC_TRACE(IFXCONNECTION_CLASS_NAME,"commit()");

  if(!__mb_autoCommit && !__mb_explicit_trans)
  {
    return;
  }

  EXEC SQL COMMIT WORK;
  IfxConversion::checkException(__XDBC_HOOK_INVOKE_MORE SQLCODE,SQLSTATE,IFXCONNECTION_CLASS_NAME);
  __XDBC_HOOK_CHECK(return);
  __mb_explicit_trans = false;
}

void IfxConnection::rollback(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{ 
  __XDBC_TRACE(IFXCONNECTION_CLASS_NAME,"rollback()");

  if(!__mb_autoCommit && !__mb_explicit_trans)
  {
    return;
  }

  EXEC SQL ROLLBACK WORK;
  IfxConversion::checkException(__XDBC_HOOK_INVOKE_MORE SQLCODE,SQLSTATE,IFXCONNECTION_CLASS_NAME);
  __XDBC_HOOK_CHECK(return);
  
  __mb_explicit_trans = false;
}

void  IfxConnection::beginTrans(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{
  EXEC SQL BEGIN WORK;
  IfxConversion::checkException(__XDBC_HOOK_INVOKE_MORE SQLCODE,SQLSTATE,IFXCONNECTION_CLASS_NAME);
}

void  IfxConnection::breakTrans(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{
  sqlbreak();
  IfxConversion::checkException(__XDBC_HOOK_INVOKE_MORE SQLCODE,SQLSTATE,IFXCONNECTION_CLASS_NAME); 
}

void IfxConnection::setTransactionIsolation(__XDBC_HOOK_DECLARE_MORE int level)  __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    __sb.append("setTransactionIsolation|");
    __sb.append(level);

    __XDBC_FORCED_TRACE(IFXCONNECTION_CLASS_NAME,__sb.str());
  }

  if(level == __mi_transLevel)
  {
    return;
  }

  if(__mb_explicit_trans)
  {
    commit(__XDBC_HOOK_INVOKE_ONCE);
  }
   
  bool canThrow=false;
 
  Statement* p_stmt=0;

  p_stmt=createStatement(__XDBC_HOOK_INVOKE_ONCE);
  xdbc_smart_ptr<Statement> __sp_stmt(p_stmt);
  __XDBC_HOOK_CHECK(return);
  
  switch(level)
  {
    case XCONNECTION_TRANSACTION_READ_UNCOMMITTED :
    {
      p_stmt->executeUpdate(__XDBC_HOOK_INVOKE_MORE "SET ISOLATION TO DIRTY READ");
    }
    break;

    case XCONNECTION_TRANSACTION_READ_COMMITTED :
    {
      p_stmt->executeUpdate(__XDBC_HOOK_INVOKE_MORE "SET ISOLATION TO COMMITTED READ");
    } 
    break;

    case XCONNECTION_TRANSACTION_REPEATABLE_READ: 
    case XCONNECTION_TRANSACTION_SERIALIZABLE :
    {
      p_stmt->executeUpdate(__XDBC_HOOK_INVOKE_MORE "SET ISOLATION TO REPEATABLE READ");
    }
    break;

    default:
    {
      canThrow=true;
    }
    break;
  }//~end switch(level)
  
  __XDBC_HOOK_CHECK(return);
    
  if(canThrow)
  {
    StringBuffer __sb;
    __sb.append("TransactionIsolation not supported {");
    __sb.append(level);
    __sb.append("}");
    
    __XDBC_HOOK_RETURN(return,__sb.str());
  }
  else
  {
    if(__mb_explicit_trans)
    {
      commit(__XDBC_HOOK_INVOKE_ONCE);
    }
  }

  __mi_transLevel=level;
}

int IfxConnection::getTransactionIsolation(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{
  return __mi_transLevel;
}

Statement* IfxConnection::createStatement(__XDBC_HOOK_DECLARE_MORE int resultSetType, int resultSetConcurrency) __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    __sb.append("createStatement|resultSetType=");
    __sb.append(resultSetConcurrency);
    __sb.append(",resultSetConcurrency=");
    __sb.append(resultSetConcurrency);

    __XDBC_FORCED_TRACE(IFXCONNECTION_CLASS_NAME,__sb.str());
  }

  IfxResultSet::isSupported(__XDBC_HOOK_INVOKE_MORE resultSetType,resultSetConcurrency);
  __XDBC_HOOK_CHECK(return 0);
  
  IfxStatement* __p_stmt = new IfxStatement(this,resultSetType,resultSetConcurrency);
  xdbc_smart_ptr<IfxStatement> __sp_stmt(__p_stmt);

  __p_stmt->initialize(__XDBC_HOOK_INVOKE_ONCE);
  __XDBC_HOOK_CHECK(return 0);

  return __sp_stmt.release();
}

PreparedStatement* IfxConnection::prepareStatement(__XDBC_HOOK_DECLARE_MORE const string& sql, int resultSetType, int resultSetConcurrency,int paramCount) __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    __sb.append("prepareStatement|resultSetType=");
    __sb.append(resultSetConcurrency);
    __sb.append(",resultSetConcurrency=");
    __sb.append(resultSetConcurrency);

    __XDBC_FORCED_TRACE(IFXCONNECTION_CLASS_NAME,__sb.str());
  }

  IfxResultSet::isSupported(__XDBC_HOOK_INVOKE_MORE resultSetType,resultSetConcurrency);
  __XDBC_HOOK_CHECK(return 0);

  IfxPreparedStatement* __p_pstmt= new IfxPreparedStatement(this,sql,resultSetType,resultSetConcurrency,paramCount);
  xdbc_smart_ptr<IfxPreparedStatement> __sp_pstmt(__p_pstmt);

  __p_pstmt->initialize(__XDBC_HOOK_INVOKE_ONCE);
  __XDBC_HOOK_CHECK(return 0);

  return __sp_pstmt.release();
}

CallableStatement* IfxConnection::prepareCall(__XDBC_HOOK_DECLARE_MORE const string& sql, int resultSetType, int resultSetConcurrency,int paramCount)  __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    __sb.append("prepareCall|resultSetType=");
    __sb.append(resultSetConcurrency);
    __sb.append(",resultSetConcurrency=");
    __sb.append(resultSetConcurrency);

    __XDBC_FORCED_TRACE(IFXCONNECTION_CLASS_NAME,__sb.str());
  }

  IfxResultSet::isSupported(__XDBC_HOOK_INVOKE_MORE resultSetType,resultSetConcurrency);
  __XDBC_HOOK_CHECK(return 0);
  
  IfxCallableStatement* __p_call= new IfxCallableStatement(this,sql,resultSetType,resultSetConcurrency,paramCount);
  xdbc_smart_ptr<IfxCallableStatement> __sp_call(__p_call);

  __p_call->initialize(__XDBC_HOOK_INVOKE_ONCE);
  __XDBC_HOOK_CHECK(return 0);

  return __sp_call.release();
}

ArrayStatement* IfxConnection::prepareArray(__XDBC_HOOK_DECLARE_MORE const string& sql, int resultSetType, int resultSetConcurrency,int paramCount)  __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    __sb.append("prepareArray|resultSetType=");
    __sb.append(resultSetConcurrency);
    __sb.append(",resultSetConcurrency=");
    __sb.append(resultSetConcurrency);

    __XDBC_FORCED_TRACE(IFXCONNECTION_CLASS_NAME,__sb.str());
  }

  IfxResultSet::isSupported(__XDBC_HOOK_INVOKE_MORE resultSetType,resultSetConcurrency);
  __XDBC_HOOK_CHECK(return 0);

  IfxArrayStatement* __p_array = new IfxArrayStatement(this,sql,resultSetType,resultSetConcurrency,paramCount);
  xdbc_smart_ptr<IfxArrayStatement> __sp_array(__p_array);
  
  __p_array->initialize(__XDBC_HOOK_INVOKE_ONCE);
  __XDBC_HOOK_CHECK(return 0);

  return __sp_array.release();
}

string  IfxConnection::getConnName()
{
  int i = 0;
  {
    Mutex::Guard g(__go_connNumber_mutex,System::__M_multithreading); 
    ++__gi_connNumber;
    if(__gi_connNumber == 0x7fffffff)
    {
      __gi_connNumber = 0;
    }
    i = __gi_connNumber;
  }

  StringBuffer s;
  s.append("ifx_con_");
  s.append(i);
  
  return s.str();
}

__OPENCODE_END_NAMESPACE
