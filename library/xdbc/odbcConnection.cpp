#include "xdbc/odbcConnection.h"
#include "xdbc/odbcStatement.h"
#include "xdbc/odbcPreparedStatement.h"
#include "xdbc/odbcCallableStatement.h"
#include "xdbc/odbcArrayStatement.h"
#include "xdbc/odbcResultSet.h"
#include "xdbc/xsmart.h"
#include "port/utilities.h"
#include "port/libnumber.h"
#include "port/ioc_sbuf.h"

#define __ODBCCONNECTION_CLASS_NAME   "OdbcConnection"

__OPENCODE_BEGIN_NAMESPACE

OdbcConnection::~OdbcConnection()
{
  __XDBC_TRACE(__ODBCCONNECTION_CLASS_NAME,"destroy()");
}

OdbcConnection::OdbcConnection()
{
  __mb_closed = false;
  __mi_transLevel = Connection::TRANSACTION_READ_COMMITTED;
  __mb_autoCommit = true;
  __mb_proc_supported = true;

  odbc_henv = SQL_NULL_HENV;
  odbc_hdbc = SQL_NULL_HDBC;

  __odbc_apis = 0;
  __odbc_options = 0;
  __options_SUPPORTSCONNECTATTR = 0;
  __options_SUPPORTSENVATTR     = 0;

  __XDBC_TRACE(__ODBCCONNECTION_CLASS_NAME,"OdbcConnection()");
}

bool OdbcConnection::connect(__XDBC_HOOK_DECLARE_MORE const string& url) __XDBC_THROW_DECLARE(SQLException)
{
  __XDBC_ATTRIBUTES props;

  bool doChange = false;
  string newURL = XDBCUtilities::parseURL(props,url,doChange);

  return connect(__XDBC_HOOK_INVOKE_MORE newURL,props);
}

bool OdbcConnection::connect(__XDBC_HOOK_DECLARE_MORE const string& url, const string& user, const string& password) __XDBC_THROW_DECLARE(SQLException)
{
  __XDBC_ATTRIBUTES props;

  bool change_it = false;
  string newURL = XDBCUtilities::parseURL(props,url,change_it);

  props[XDBC_USER] = user;
  props[XDBC_PASSWORD] = password;

  return  connect(__XDBC_HOOK_INVOKE_MORE newURL,props);
}

bool OdbcConnection::connect(__XDBC_HOOK_DECLARE_MORE const string& dsn,__XDBC_ATTRIBUTES& info) __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    __sb << "connect|" << dsn ;
    __XDBC_FORCED_TRACE(__ODBCCONNECTION_CLASS_NAME,__sb.str());
  }

  __XDBC_ATTRIBUTES __xdbc_attributes;
  OdbcDriverManager::getFunctions(__XDBC_HOOK_INVOKE_MORE dsn,__xdbc_attributes,__odbc_options,__odbc_apis);
  __XDBC_HOOK_CHECK(return false);

  __options_SUPPORTSCONNECTATTR = __odbc_options->__options_SUPPORTSCONNECTATTR;
  __options_SUPPORTSENVATTR     = __odbc_options->__options_SUPPORTSENVATTR;
  
  string __db_libpathname;
  string __db_url;

  __XDBC_ATTRIBUTES::iterator __pos = __xdbc_attributes.begin();
  __XDBC_ATTRIBUTES::iterator __end = __xdbc_attributes.end();
    
  StringBuffer __db_sb;
  while(__pos != __end)
  {
    if(StringsKit::equalsIgnoreCase(__pos->first.c_str(),"driver"))
    {
      __db_libpathname = StringsKit::trim(__pos->second);
    }
    else
    {
      string __uppercase_value = StringsKit::toUpperCase(__pos->first);
      if(    (!StringsKit::equalsIgnoreCase(__pos->first.c_str(),"description"))
          && (!StringsKit::startsWith(__uppercase_value,"ODBCOPTIONS.")))
      {
          __db_sb << (__pos->first) << "=" << StringsKit::trim(__pos->second) << ";";
        } 
    } 
    
    ++__pos;
  }
  __db_url = __db_sb.str();
  if(__db_url[__db_url.size() -1] == ';')
  {
    __db_url = __db_url.substr(0,__db_url.size() -1);
  }

//---------------------------------------------------------
  SQLRETURN __odbc_sqlreturn = SQL_SUCCESS;

  __odbc_sqlreturn = __odbc_SQLAllocHandle(this,SQL_HANDLE_ENV,SQL_NULL_HANDLE,&odbc_henv);
  if(__ODBC_HOOK_CHECK_FAILED(__odbc_sqlreturn))
  {
    OdbcKit::checkException(__XDBC_HOOK_INVOKE_MORE  this,SQL_NULL_HSTMT,__odbc_sqlreturn,__ODBCCONNECTION_CLASS_NAME);
    __XDBC_HOOK_CHECK(return false);
  }
  
  __odbc_sqlreturn = __odbc_SQLAllocHandle(this,SQL_HANDLE_DBC,odbc_henv,&odbc_hdbc);
  if(__ODBC_HOOK_CHECK_FAILED(__odbc_sqlreturn))
  {
    OdbcKit::checkException(__XDBC_HOOK_INVOKE_MORE  this,SQL_NULL_HSTMT,__odbc_sqlreturn,__ODBCCONNECTION_CLASS_NAME);
    __XDBC_HOOK_CHECK(return false);
  }

  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    __sb << "connect to database using attributes below : " << File::lineSeparator
         << "DSN    = <<" << dsn << ">>"<< File::lineSeparator
         << "Driver = <<" << __db_libpathname << ">>" << File::lineSeparator
         << "URL    = <<" << __db_url << ">>" << File::lineSeparator;

    __XDBC_FORCED_TRACE(__ODBCCONNECTION_CLASS_NAME,__sb.str());
  }

//---------------------------------------------------------

  SQLCHAR* __connString = (SQLCHAR *)__db_url.c_str();
  SQLCHAR  __connOut[1024] = {0};
  SQLSMALLINT __connOutLen_I = 1024;
  SQLSMALLINT __connOutLen_O = 1024;

  __odbc_sqlreturn = __odbc_SQLDriverConnect(this,odbc_hdbc, NULL,__connString, SQL_NTS,__connOut, __connOutLen_I,&__connOutLen_O,SQL_DRIVER_NOPROMPT);
  if(__ODBC_HOOK_CHECK_FAILED(__odbc_sqlreturn))
  {
    OdbcKit::checkException(__XDBC_HOOK_INVOKE_MORE  this,SQL_NULL_HSTMT,__odbc_sqlreturn,__ODBCCONNECTION_CLASS_NAME);
    __XDBC_HOOK_CHECK(return false);
  }

//---------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------
  char __ret_proc_supported[20] = {0};
  SQLSMALLINT __buf_ind = 0;
  __odbc_SQLGetInfo(this,odbc_hdbc,SQL_PROCEDURES,__ret_proc_supported,20,&__buf_ind);
  if(!__ODBC_HOOK_CHECK_FAILED(__odbc_sqlreturn))
  {
    if(StringsKit::equalsIgnoreCase("N",__ret_proc_supported,__buf_ind))
    {
      __mb_proc_supported = false;
      __XDBC_TRACE(__ODBCCONNECTION_CLASS_NAME,"Procedure not supported");
    }
  }

  SQLINTEGER __option = SQL_ATTR_AUTOCOMMIT;
  SQLINTEGER __param;

  __odbc_sqlreturn = __odbc_SQLGetConnectAttr(this,odbc_hdbc,__option,&__param,(SQLINTEGER)SQL_IS_INTEGER,0);
  if(__ODBC_HOOK_CHECK_FAILED(__odbc_sqlreturn))
  {
    OdbcKit::checkException(__XDBC_HOOK_INVOKE_MORE  this,SQL_NULL_HSTMT,__odbc_sqlreturn,__ODBCCONNECTION_CLASS_NAME);
    __XDBC_HOOK_CHECK(return false);
  }

  if(SQL_AUTOCOMMIT_OFF == __param)
  {
    __mb_autoCommit = false;
  }

  __option = SQL_ATTR_TXN_ISOLATION;
  __odbc_sqlreturn = __odbc_SQLGetConnectAttr(this,odbc_hdbc,__option,&__param,(SQLINTEGER)SQL_IS_INTEGER,0);
  if(__ODBC_HOOK_CHECK_FAILED(__odbc_sqlreturn))
  {
    OdbcKit::checkException(__XDBC_HOOK_INVOKE_MORE  this,SQL_NULL_HSTMT,__odbc_sqlreturn,__ODBCCONNECTION_CLASS_NAME);
    __XDBC_HOOK_CHECK(return false);
  }

  if(SQL_TXN_READ_UNCOMMITTED == __param)
  {
    __mi_transLevel = Connection::TRANSACTION_READ_UNCOMMITTED;
  }
  else if(SQL_TXN_READ_COMMITTED == __param)
  {
    __mi_transLevel = Connection::TRANSACTION_READ_COMMITTED;
  }
  else if(SQL_TXN_REPEATABLE_READ == __param)
  {
    __mi_transLevel = Connection::TRANSACTION_REPEATABLE_READ;
  }
  else if(SQL_TXN_SERIALIZABLE == __param)
  {
    __mi_transLevel = Connection::TRANSACTION_SERIALIZABLE;
  }

//---------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------
  __mo_attributes[XDBC_DRIVER]  = "opencode/odbc";
  __mo_attributes[XDBC_VERSION] = "1.1.0";

  return true;
}

string OdbcConnection::normalizeSQL(const string& sql,int& paramCount)
{
  paramCount = SQLImplementation::paramCountUnknown;
  return sql;
}

void OdbcConnection::close()
{
  __XDBC_TRACE(__ODBCCONNECTION_CLASS_NAME,"close()");

  if(__mb_closed || __odbc_apis == 0)
  {
    return;
  }

  if(odbc_hdbc != SQL_NULL_HDBC)
  {
    (__odbc_apis->__api_SQLDISCONNECT)(odbc_hdbc);
    __odbc_SQLFreeHandle(this,SQL_HANDLE_DBC,odbc_hdbc);
    odbc_hdbc = SQL_NULL_HDBC;
  }

  if(odbc_henv != SQL_NULL_HENV)
  {
    __odbc_SQLFreeHandle(this,SQL_HANDLE_ENV,odbc_henv);
    odbc_henv = SQL_NULL_HENV;
  }

  __mb_closed=true;
}

void OdbcConnection::destroy()
{
  delete this;
}

bool OdbcConnection::isClosed()
{
  return __mb_closed;
}

int OdbcConnection::pingDatabase(int timeout)
{
  int retcode = 0;

  __OPENCODE_TRY
  {
    __XDBC_HOOK_DEFINE();
    Statement* __p_stmt = createStatement(__XDBC_HOOK_INVOKE_ONCE);
    xdbc_smart_ptr<Statement> __sp_stmt(__p_stmt);
    __XDBC_HOOK_CHECK(return -1);

    ResultSet*  __p_rs = __p_stmt->executeQuery(__XDBC_HOOK_INVOKE_MORE "SELECT 'x' FROM SYS.DUAL");
    xdbc_smart_ptr<ResultSet> __sp_rs(__p_rs);
    __XDBC_HOOK_CHECK(return -1);
  }
  __OPENCODE_CATCH(SQLException& ex)
  {
    retcode = -1;
  }

  return retcode;
}

Statement* OdbcConnection::createStatement(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException)
{
  OdbcStatement* p_stmt = new OdbcStatement(this);
  xdbc_smart_ptr<OdbcStatement> __sp_stmt(p_stmt);

  p_stmt->initialize(__XDBC_HOOK_INVOKE_ONCE);
  __XDBC_HOOK_CHECK(return 0);

  return __sp_stmt.release();
}

PreparedStatement* OdbcConnection::prepareStatement(__XDBC_HOOK_DECLARE_MORE const string& sql,int paramCount) __XDBC_THROW_DECLARE(SQLException)
{
  OdbcPreparedStatement* p_pstmt= new OdbcPreparedStatement(this,sql,paramCount);
  xdbc_smart_ptr<OdbcPreparedStatement> __sp_pstmt(p_pstmt);

  p_pstmt->initialize(__XDBC_HOOK_INVOKE_ONCE);
  __XDBC_HOOK_CHECK(return 0);

  return __sp_pstmt.release();
}

CallableStatement* OdbcConnection::prepareCall(__XDBC_HOOK_DECLARE_MORE const string& sql,int paramCount) __XDBC_THROW_DECLARE(SQLException)
{
  if(!__mb_proc_supported)
  {
    __XDBC_HOOK_RETURN(return 0,"Procedure not supported");
  }
  OdbcCallableStatement* p_call= new OdbcCallableStatement(this,sql,paramCount);
  xdbc_smart_ptr<OdbcCallableStatement> __sp_call(p_call);

  p_call->initialize(__XDBC_HOOK_INVOKE_ONCE);
  __XDBC_HOOK_CHECK(return 0);

  return __sp_call.release();
}

ArrayStatement* OdbcConnection::prepareArray(__XDBC_HOOK_DECLARE_MORE const string& sql,int paramCount) __XDBC_THROW_DECLARE(SQLException)
{
  OdbcArrayStatement* p_array = new OdbcArrayStatement(this,sql,paramCount);
  xdbc_smart_ptr<OdbcArrayStatement> __sp_array(p_array);

  p_array->initialize(__XDBC_HOOK_INVOKE_ONCE);
  __XDBC_HOOK_CHECK(return 0);
  return __sp_array.release();
}

void OdbcConnection::setAutoCommit(__XDBC_HOOK_DECLARE_MORE bool autoCommit) __XDBC_THROW_DECLARE(SQLException)
{
  if(autoCommit==__mb_autoCommit)
  {
    return;
  }

  SQLUSMALLINT   __option = SQL_ATTR_AUTOCOMMIT;
  SQLINTEGER __param;
  SQLRETURN __odbc_sqlreturn = SQL_SUCCESS;

  if(autoCommit)
  {
    __param = (SQLINTEGER)SQL_AUTOCOMMIT_ON;
  }
  else
  {
    __param = (SQLINTEGER)SQL_AUTOCOMMIT_OFF;
  }

  __odbc_sqlreturn = (__odbc_SQLSetConnectAttr)(this,odbc_hdbc,__option,(SQLPOINTER )__param,(SQLINTEGER)SQL_IS_INTEGER,(SQLPOINTER)&__param,(SQLINTEGER)SQL_IS_POINTER);
  if(__ODBC_HOOK_CHECK_FAILED(__odbc_sqlreturn))
  {
    OdbcKit::checkException(__XDBC_HOOK_INVOKE_MORE  this,SQL_NULL_HSTMT,__odbc_sqlreturn,__ODBCCONNECTION_CLASS_NAME);
    __XDBC_HOOK_CHECK(return);
  }

  __mb_autoCommit=autoCommit;
}

bool OdbcConnection::getAutoCommit(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    __sb.append("getAutoCommit|");
    __sb.append(__mb_autoCommit);

    __XDBC_FORCED_TRACE(__ODBCCONNECTION_CLASS_NAME,__sb.str());
  }

  return __mb_autoCommit;
}

void OdbcConnection::commit(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException)
{
  __XDBC_TRACE(__ODBCCONNECTION_CLASS_NAME,"commit()");

  SQLRETURN __odbc_sqlreturn = __odbc_SQLEndTran(this,SQL_HANDLE_DBC,odbc_hdbc,SQL_COMMIT);
  if(__ODBC_HOOK_CHECK_FAILED(__odbc_sqlreturn))
  {
    OdbcKit::checkException(__XDBC_HOOK_INVOKE_MORE  this,SQL_NULL_HSTMT,__odbc_sqlreturn,__ODBCCONNECTION_CLASS_NAME);
  }
}

void OdbcConnection::rollback(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException)
{
  __XDBC_TRACE(__ODBCCONNECTION_CLASS_NAME,"rollback()");

  SQLRETURN __odbc_sqlreturn = __odbc_SQLEndTran(this,SQL_HANDLE_DBC,odbc_hdbc,SQL_ROLLBACK);
  if(__ODBC_HOOK_CHECK_FAILED(__odbc_sqlreturn))
  {
    OdbcKit::checkException(__XDBC_HOOK_INVOKE_MORE  this,SQL_NULL_HSTMT,__odbc_sqlreturn,__ODBCCONNECTION_CLASS_NAME);
  }
}

void OdbcConnection::setTransactionIsolation(__XDBC_HOOK_DECLARE_MORE int level) __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    __sb.append("setTransactionIsolation|");
    __sb.append(level);

    __XDBC_FORCED_TRACE(__ODBCCONNECTION_CLASS_NAME,__sb.str());
  }

  SQLUSMALLINT   __option = SQL_ATTR_TXN_ISOLATION;
  SQLINTEGER     __param;
  SQLRETURN __odbc_sqlreturn = SQL_SUCCESS;

  if(Connection::TRANSACTION_READ_UNCOMMITTED == level)
  {
    __param = (SQLINTEGER)SQL_TXN_READ_UNCOMMITTED;
  }
  else if(Connection::TRANSACTION_READ_COMMITTED == level)
  {
    __param = (SQLINTEGER)SQL_TXN_READ_COMMITTED;
  }
  else if(Connection::TRANSACTION_REPEATABLE_READ == level)
  {
    __param = (SQLINTEGER)SQL_TXN_REPEATABLE_READ;
  }
  else if(Connection::TRANSACTION_SERIALIZABLE == level)
  {
    __param = (SQLINTEGER)SQL_TXN_SERIALIZABLE;
  }
  else
  {
    __param = (SQLINTEGER)SQL_TXN_READ_COMMITTED;
  }

  __odbc_sqlreturn = __odbc_SQLSetConnectAttr(this,odbc_hdbc,__option,(SQLPOINTER )__param,(SQLINTEGER)SQL_IS_INTEGER,(SQLPOINTER)&__param,(SQLINTEGER)SQL_IS_POINTER);
  if(__ODBC_HOOK_CHECK_FAILED(__odbc_sqlreturn))
  {
    OdbcKit::checkException(__XDBC_HOOK_INVOKE_MORE  this,SQL_NULL_HSTMT,__odbc_sqlreturn,__ODBCCONNECTION_CLASS_NAME);
    __XDBC_HOOK_CHECK(return);
  }

  __mi_transLevel=level;
}

int OdbcConnection::getTransactionIsolation(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException)
{
  return __mi_transLevel;
}

Statement* OdbcConnection::createStatement(__XDBC_HOOK_DECLARE_MORE int resultSetType, int resultSetConcurrency)__XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    __sb.append("createStatement|resultSetType=");
    __sb.append(resultSetConcurrency);
    __sb.append(",resultSetConcurrency=");
    __sb.append(resultSetConcurrency);

    __XDBC_FORCED_TRACE(__ODBCCONNECTION_CLASS_NAME,__sb.str());
  }

  OdbcResultSet::isSupported(__XDBC_HOOK_INVOKE_MORE resultSetType,resultSetConcurrency);
  __XDBC_HOOK_CHECK(return 0);

  OdbcStatement* p_stmt = new OdbcStatement(this,resultSetType,resultSetConcurrency);
  xdbc_smart_ptr<OdbcStatement> __sp_stmt(p_stmt);

  p_stmt->initialize(__XDBC_HOOK_INVOKE_ONCE);
  __XDBC_HOOK_CHECK(return 0);

  return __sp_stmt.release();
}

PreparedStatement* OdbcConnection::prepareStatement(__XDBC_HOOK_DECLARE_MORE const string& sql, int resultSetType, int resultSetConcurrency,int paramCount)__XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    __sb.append("prepareStatement|resultSetType=");
    __sb.append(resultSetConcurrency);
    __sb.append(",resultSetConcurrency=");
    __sb.append(resultSetConcurrency);

    __XDBC_FORCED_TRACE(__ODBCCONNECTION_CLASS_NAME,__sb.str());
  }

  OdbcResultSet::isSupported(__XDBC_HOOK_INVOKE_MORE resultSetType,resultSetConcurrency);
  __XDBC_HOOK_CHECK(return 0);

  OdbcPreparedStatement* p_pstmt= new OdbcPreparedStatement(this,sql,resultSetType,resultSetConcurrency,paramCount);
  xdbc_smart_ptr<OdbcPreparedStatement> __sp_pstmt(p_pstmt);

  p_pstmt->initialize(__XDBC_HOOK_INVOKE_ONCE);
  __XDBC_HOOK_CHECK(return 0);

  return __sp_pstmt.release();
}

CallableStatement* OdbcConnection::prepareCall(__XDBC_HOOK_DECLARE_MORE const string& sql, int resultSetType, int resultSetConcurrency,int paramCount) __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    __sb.append("prepareCall|resultSetType=");
    __sb.append(resultSetConcurrency);
    __sb.append(",resultSetConcurrency=");
    __sb.append(resultSetConcurrency);

    __XDBC_FORCED_TRACE(__ODBCCONNECTION_CLASS_NAME,__sb.str());
  }

  if(!__mb_proc_supported)
  {
    __XDBC_HOOK_RETURN(return 0,"Procedure not supported");
  }

  OdbcResultSet::isSupported(__XDBC_HOOK_INVOKE_MORE resultSetType,resultSetConcurrency);
  __XDBC_HOOK_CHECK(return 0);

  OdbcCallableStatement* p_call= new OdbcCallableStatement(this,sql,resultSetType,resultSetConcurrency,paramCount);
  xdbc_smart_ptr<OdbcCallableStatement> __sp_call(p_call);

  p_call->initialize(__XDBC_HOOK_INVOKE_ONCE);
  __XDBC_HOOK_CHECK(return 0);

  return __sp_call.release();
}

ArrayStatement* OdbcConnection::prepareArray(__XDBC_HOOK_DECLARE_MORE const string& sql,int resultSetType, int resultSetConcurrency,int paramCount) __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    __sb.append("prepareArray|resultSetType=");
    __sb.append(resultSetConcurrency);
    __sb.append(",resultSetConcurrency=");
    __sb.append(resultSetConcurrency);

    __XDBC_FORCED_TRACE(__ODBCCONNECTION_CLASS_NAME,__sb.str());
  }

  OdbcResultSet::isSupported(__XDBC_HOOK_INVOKE_MORE resultSetType,resultSetConcurrency);
  __XDBC_HOOK_CHECK(return 0);

  OdbcArrayStatement* p_array = new OdbcArrayStatement(this,sql,resultSetType,resultSetConcurrency,paramCount);
  xdbc_smart_ptr<OdbcArrayStatement> __sp_array(p_array);

  p_array->initialize(__XDBC_HOOK_INVOKE_ONCE);
  __XDBC_HOOK_CHECK(return 0);

  return __sp_array.release();
}

__OPENCODE_END_NAMESPACE
