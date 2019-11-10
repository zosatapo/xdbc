#include "xdbc/odbcStatement.h"
#include "xdbc/odbcResultSet.h"
#include "xdbc/odbcConnection.h"
#include "xdbc/xsmart.h"

__OPENCODE_BEGIN_NAMESPACE

#define __ODBCSTATEMENT_CLASS_NAME   "OdbcStatement"

//-------------------------------------------------------------------------

OdbcStatement::~OdbcStatement()
{
  __XDBC_TRACE(__ODBCSTATEMENT_CLASS_NAME,"destroy()");
}


OdbcStatement::OdbcStatement(OdbcConnection* pConnection)
{
  __mp_conn = pConnection;
  __options_SUPPORTSSTMTATTR = __mp_conn->__odbc_options->__options_SUPPORTSSTMTATTR;

  __mi_resultSetType=ResultSet::TYPE_FORWARD_ONLY;
  __mi_resultSetConcurrency=ResultSet::CONCUR_READ_ONLY;

  odbc_hstmt = SQL_NULL_HSTMT;
  __mb_initialized=false;
  __mb_isClosed=false;
}

OdbcStatement::OdbcStatement(OdbcConnection* pConnection,int resultSetType, int resultSetConcurrency)
{
  __mp_conn = pConnection;

  __mi_resultSetType=resultSetType;
  __mi_resultSetConcurrency=resultSetConcurrency;

  odbc_hstmt = SQL_NULL_HSTMT;
  __mb_initialized=false;
  __mb_isClosed=false;
}

void OdbcStatement::initialize(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException)
{
  if(__mb_initialized)
  {
    return;
  }

  __mb_initialized=true;

  SQLRETURN __odbc_sqlreturn = SQL_SUCCESS;

  __odbc_sqlreturn = __odbc_SQLAllocHandle(__mp_conn,SQL_HANDLE_STMT,__mp_conn->odbc_hdbc, &odbc_hstmt);
  if(__ODBC_HOOK_CHECK_FAILED(__odbc_sqlreturn))
  {
    OdbcKit::checkException(__XDBC_HOOK_INVOKE_MORE  __mp_conn, odbc_hstmt,__odbc_sqlreturn,__ODBCSTATEMENT_CLASS_NAME);
    __XDBC_HOOK_CHECK(return);
  }

  SQLINTEGER __option = SQL_ATTR_NOSCAN;
  SQLINTEGER __param = SQL_NOSCAN_ON;
  if((__options_SUPPORTSSTMTATTR)(__option, (SQLPOINTER )__param,(SQLINTEGER)SQL_IS_INTEGER) == SQL_SUCCESS)
  {
    __odbc_sqlreturn = __odbc_SQLSetStmtAttr(__mp_conn,odbc_hstmt,__option, (SQLPOINTER )__param,(SQLINTEGER)SQL_IS_INTEGER,(SQLPOINTER )&__param,(SQLINTEGER)SQL_IS_POINTER);
    if(__ODBC_HOOK_CHECK_FAILED(__odbc_sqlreturn))
    {
      OdbcKit::checkException(__XDBC_HOOK_INVOKE_MORE  __mp_conn, odbc_hstmt,__odbc_sqlreturn,__ODBCSTATEMENT_CLASS_NAME);
    }
  }
}

ResultSet* OdbcStatement::executeQuery(__XDBC_HOOK_DECLARE_MORE const string& sql) __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
     StringBuffer sb;
     sb.append("executeQuery|");
     sb.append(sql);

     __XDBC_FORCED_TRACE(__ODBCSTATEMENT_CLASS_NAME,sb.str());
  }

  string __ms_odbc_sql;
  OdbcKit::formatSQL(__mp_conn->__odbc_options,sql,__ms_odbc_sql);   	
  const char* char_sql = __ms_odbc_sql.c_str();
  
  SQLRETURN __odbc_sqlreturn = SQL_SUCCESS;

  if(__mi_resultSetType == ResultSet::TYPE_FORWARD_ONLY)
  {

    __odbc_sqlreturn = __odbc_SQLExecDirect(__mp_conn,odbc_hstmt,(SQLCHAR*)char_sql,SQL_NTS);
  }
  else
  {
    SQLINTEGER __option = SQL_ATTR_CURSOR_SCROLLABLE ;
    SQLINTEGER __param = SQL_SCROLLABLE;
    if((__options_SUPPORTSSTMTATTR)(__option, (SQLPOINTER )__param,(SQLINTEGER)SQL_IS_INTEGER) == SQL_SUCCESS)
    {
      __odbc_sqlreturn = __odbc_SQLSetStmtAttr(__mp_conn,odbc_hstmt,__option, (SQLPOINTER )__param,(SQLINTEGER)SQL_IS_INTEGER,(SQLPOINTER )&__param,(SQLINTEGER)SQL_IS_POINTER);
      if(__ODBC_HOOK_CHECK_FAILED(__odbc_sqlreturn))
      {
        OdbcKit::checkException(__XDBC_HOOK_INVOKE_MORE  __mp_conn, odbc_hstmt,__odbc_sqlreturn,__ODBCSTATEMENT_CLASS_NAME);
        __XDBC_HOOK_CHECK(return 0);
      }
    }

    __odbc_sqlreturn = __odbc_SQLExecDirect(__mp_conn,odbc_hstmt,(SQLCHAR*)char_sql,SQL_NTS);
  }

  if(__ODBC_HOOK_CHECK_FAILED(__odbc_sqlreturn))
  {
    OdbcKit::checkException(__XDBC_HOOK_INVOKE_MORE  __mp_conn, odbc_hstmt,__odbc_sqlreturn,__ODBCSTATEMENT_CLASS_NAME);
    __XDBC_HOOK_CHECK(return 0);
  }

  OdbcResultSetCollection* __cachedCollection = new OdbcResultSetCollection();

  OdbcKit::parseResultSetMetaData(__XDBC_HOOK_INVOKE_MORE __mp_conn,odbc_hstmt,__cachedCollection->__vector_columns);
  __XDBC_HOOK_CHECK(return 0);

  OdbcResultSet::prepareColumnData(__XDBC_HOOK_INVOKE_MORE __mp_conn,sql,odbc_hstmt,__cachedCollection);
  __XDBC_HOOK_CHECK(return 0);
  
  OdbcResultSet::defineColumnData(__XDBC_HOOK_INVOKE_MORE __mp_conn,odbc_hstmt,__cachedCollection);
  __XDBC_HOOK_CHECK(return 0);

  OdbcResultSet* __p_rs = new OdbcResultSet(__mp_conn,this,odbc_hstmt,__mi_resultSetType,__mi_resultSetConcurrency,__cachedCollection,true,true);
  return __p_rs;
}

int OdbcStatement::executeUpdate(__XDBC_HOOK_DECLARE_MORE const string& sql) __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
     StringBuffer sb;
     sb.append("executeUpdate|");
     sb.append(sql);

     __XDBC_FORCED_TRACE(__ODBCSTATEMENT_CLASS_NAME,sb.str());
  }

  string __ms_odbc_sql;
  OdbcKit::formatSQL(__mp_conn->__odbc_options,sql,__ms_odbc_sql);   	
  const char* char_sql = __ms_odbc_sql.c_str();
  
  SQLRETURN __odbc_sqlreturn = SQL_SUCCESS;

  __odbc_sqlreturn = SQL_SUCCESS;
  if(__mi_resultSetType == ResultSet::TYPE_FORWARD_ONLY)
  {

    __odbc_sqlreturn = __odbc_SQLExecDirect(__mp_conn,odbc_hstmt,(SQLCHAR*)char_sql,SQL_NTS);
  }
  else
  {
    __odbc_sqlreturn = __odbc_SQLExecDirect(__mp_conn,odbc_hstmt,(SQLCHAR*)char_sql,SQL_NTS);
  }

  if(__ODBC_HOOK_CHECK_FAILED(__odbc_sqlreturn))
  {
    OdbcKit::checkException(__XDBC_HOOK_INVOKE_MORE  __mp_conn, odbc_hstmt,__odbc_sqlreturn,__ODBCSTATEMENT_CLASS_NAME);
    __XDBC_HOOK_CHECK(return 0);
  }

  SQLINTEGER rowsFetched=0;
  __odbc_sqlreturn = __odbc_SQLRowCount(__mp_conn,odbc_hstmt,&rowsFetched);

  if(__ODBC_HOOK_CHECK_FAILED(__odbc_sqlreturn))
  {
    OdbcKit::checkException(__XDBC_HOOK_INVOKE_MORE  __mp_conn, odbc_hstmt,__odbc_sqlreturn,__ODBCSTATEMENT_CLASS_NAME);
    __XDBC_HOOK_CHECK(return 0);
  }

  return rowsFetched;
}

void OdbcStatement::close()
{
  __XDBC_TRACE(__ODBCSTATEMENT_CLASS_NAME,"close()");

  if(__mb_isClosed)
  {
    return;
  }

  __mb_isClosed=true;

  if(odbc_hstmt != SQL_NULL_HSTMT)
  {
    __odbc_SQLFreeHandle(__mp_conn,SQL_HANDLE_STMT,odbc_hstmt);
    odbc_hstmt = SQL_NULL_HSTMT;
  }
}

void OdbcStatement::destroy()
{
  delete this;
}

int OdbcStatement::getResultSetType(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{
  return __mi_resultSetType;
}

Connection* OdbcStatement::getConnection(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{
  return (Connection*)__mp_conn;
}

__OPENCODE_END_NAMESPACE
