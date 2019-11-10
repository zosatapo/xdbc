#include "xdbc/odbcPreparedStatement.h"
#include "xdbc/odbcConnection.h"
#include "xdbc/odbcResultSet.h"
#include "xdbc/odbcClob.h"
#include "xdbc/odbcBlob.h"
#include "xdbc/odbcTypes.h"
#include "xdbc/xsmart.h" 
#include "xdbc/XDBCUtilities.h"
#include "port/ioc_sbuf.h"
#include "port/ioc_smart.h"
#include "port/utilities.h"

__OPENCODE_BEGIN_NAMESPACE

#define __ODBCPREPAREDSTATEMENT_CLASS_NAME   "OdbcPreparedStatement"

//-------------------------------------------------------------------------

OdbcPreparedStatement::~OdbcPreparedStatement()
{
  __XDBC_TRACE(__ODBCPREPAREDSTATEMENT_CLASS_NAME,"destroy()"); 
}

OdbcPreparedStatement::OdbcPreparedStatement(OdbcConnection* pConnection,const string& sql,int paramCount)
{
  __mp_conn=pConnection;
  __api_SQLEXECUTE       = __mp_conn->__odbc_apis->__api_SQLEXECUTE;
  __mb_fintAvailable      = __mp_conn->__odbc_options->__mb_fintAvailable;
  __options_SUPPORTSSTMTATTR = __mp_conn->__odbc_options->__options_SUPPORTSSTMTATTR;
    
  __mi_resultSetType=ResultSet::TYPE_FORWARD_ONLY;
  __mi_resultSetConcurrency=ResultSet::CONCUR_READ_ONLY;
  
  odbc_hstmt = SQL_NULL_HSTMT;
  __mb_initialized=false;
  __mb_isClosed=false;

//--------------------------
  
  __ms_odbc_sql=sql; 
  __mi_param_count=paramCount;   
  
  __p_parambuf = 0;
  __parambuf_size = 0;
 
//--------------------------
   
  __cachedCollection = 0;
  __adoptIt = false;
  __isDefined = false;  
  __p_rs_opened = 0;
}

OdbcPreparedStatement::OdbcPreparedStatement(OdbcConnection* pConnection,const string& sql, int resultSetType, int resultSetConcurrency,int paramCount)
{
  __mp_conn=pConnection;
  __api_SQLEXECUTE       = __mp_conn->__odbc_apis->__api_SQLEXECUTE;
  __mb_fintAvailable      = __mp_conn->__odbc_options->__mb_fintAvailable;
  
  __mi_resultSetType=resultSetType;
  __mi_resultSetConcurrency=resultSetConcurrency;
  
  odbc_hstmt = SQL_NULL_HSTMT;
  __mb_initialized=false;
  __mb_isClosed=false;
  
 //--------------------------
  
  __ms_odbc_sql=sql;
  __mi_param_count=paramCount;
  
  __p_parambuf = 0;
  __parambuf_size = 0;

//--------------------------
   
  __cachedCollection = 0;
  __adoptIt = false;
  __isDefined = false; 
  __p_rs_opened = 0;
}

void OdbcPreparedStatement::initialize(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException)
{ 
  if(__mb_initialized)
  {
    return;
  }
      
  __mb_initialized = true;
  

  string __temp_odc_sql;
  OdbcKit::formatSQL(__mp_conn->__odbc_options,__ms_odbc_sql,__temp_odc_sql);
  __ms_odbc_sql = __temp_odc_sql;
  
  //-------------------------------------------------------------------------
  // 0. SQLAllocStmt 
  //-------------------------------------------------------------------------
  SQLRETURN __odbc_sqlreturn = SQL_SUCCESS;
  __odbc_sqlreturn = __odbc_SQLAllocHandle(__mp_conn,SQL_HANDLE_STMT,__mp_conn->odbc_hdbc, &odbc_hstmt);  
  if(__ODBC_HOOK_CHECK_FAILED(__odbc_sqlreturn))
  {
    OdbcKit::checkException(__XDBC_HOOK_INVOKE_MORE  __mp_conn, odbc_hstmt,__odbc_sqlreturn,__ODBCPREPAREDSTATEMENT_CLASS_NAME);
    __XDBC_HOOK_CHECK(return);
  } 
              
  SQLINTEGER __option = SQL_ATTR_NOSCAN;
  SQLINTEGER __param = SQL_NOSCAN_ON;
  if((__options_SUPPORTSSTMTATTR)(__option, (SQLPOINTER )__param,(SQLINTEGER)SQL_IS_INTEGER) == SQL_SUCCESS)
  {
    __odbc_sqlreturn = __odbc_SQLSetStmtAttr(__mp_conn,odbc_hstmt,__option, (SQLPOINTER )__param,(SQLINTEGER)SQL_IS_INTEGER,(SQLPOINTER )&__param,(SQLINTEGER)SQL_IS_POINTER);
    if(__ODBC_HOOK_CHECK_FAILED(__odbc_sqlreturn))
    {
      OdbcKit::checkException(__XDBC_HOOK_INVOKE_MORE  __mp_conn, odbc_hstmt,__odbc_sqlreturn,__ODBCPREPAREDSTATEMENT_CLASS_NAME);
      __XDBC_HOOK_CHECK(return);
    }
  }
  
  //-------------------------------------------------------------------------
  // 1. Get  VariableData
  //-------------------------------------------------------------------------
  
  __odbc_sqlreturn = __odbc_SQLPrepare(__mp_conn,odbc_hstmt,(SQLCHAR*)__ms_odbc_sql.c_str(),SQL_NTS);    
  if(__ODBC_HOOK_CHECK_FAILED(__odbc_sqlreturn))
  {
    OdbcKit::checkException(__XDBC_HOOK_INVOKE_MORE  __mp_conn, odbc_hstmt,__odbc_sqlreturn,__ODBCPREPAREDSTATEMENT_CLASS_NAME);
    __XDBC_HOOK_CHECK(return);
  }    
  
  OdbcKit::prepareVariableData(__XDBC_HOOK_INVOKE_MORE __mp_conn,odbc_hstmt,__vector_params);
  __XDBC_HOOK_CHECK(return);
  
  __mi_param_count= (int)(__vector_params.size() - 1);

  //-------------------------------------------------------------------------
  // 2. allocate param data buffer 
  //-------------------------------------------------------------------------
  ODBC_PARAM* __odbc_param = 0;
  int offset = 0; 
  size_t __oc_sizeof_void = sizeof(void*);
  
  __odbc_sqlreturn  = SQL_ERROR;
  ODBCSQLOPTIONS_ADJUSTBINDABLE __options_ADJUSTBINDABLE = __mp_conn->__odbc_options->__options_ADJUSTBINDABLE;  
  
  for(int i = 1;i <= __mi_param_count;++i)
  {
    __odbc_param = __vector_params[i];

    __odbc_sqlreturn = (__options_ADJUSTBINDABLE)((SQLCHAR*)__ms_odbc_sql.c_str(),(SQLPOINTER)__odbc_param);
    if(__odbc_sqlreturn != SQL_SUCCESS)
    {
      OdbcKit::adjustBindable(__mp_conn,__odbc_param);
    }
      
    __odbc_param->__mi_buf_offset = offset;
    
    switch(__odbc_param->__mi_bind_type)
    {
      case SQL_CHAR:
      case SQL_VARCHAR:
      case SQL_LONGVARCHAR:
      case SQL_BINARY:
      case SQL_VARBINARY:
      case SQL_LONGVARBINARY:
      {
       __odbc_param->__mi_buf_size = (int)(__odbc_param->__mi_length + 1);
      }
      break;
  //-----------------------------
      case SQL_BIT:
      case SQL_TINYINT:
      case SQL_SMALLINT:
      case SQL_INTEGER:
      case SQL_BIGINT:
      case SQL_REAL:
      case SQL_FLOAT:
      case SQL_DOUBLE:
      case SQL_NUMERIC:
      case SQL_DECIMAL:
      case SQL_TYPE_DATE:
      case SQL_TYPE_TIME:
      case SQL_TYPE_TIMESTAMP:
      {
        __odbc_param->__mi_buf_size = 0;
      }
 //-----------------------------
      default:
      {
        __odbc_param->__mi_buf_size = (int)__odbc_param->__mi_length;
      }
      break;
    }  

    if(__odbc_param->__mi_buf_size > __mp_conn->__odbc_options->__mi_xlob_max)
    {
    	__odbc_param->__mi_buf_size = __mp_conn->__odbc_options->__mi_xlob_max;
    }
     
    __odbc_param->__mi_bind_size = __odbc_param->__mi_buf_size;
    
    if(__vector_params[i]->__mi_bind_size != 0)
    {
      __odbc_param->__mi_bind_size = (int)MemoryKit::roundUp((__odbc_param->__mi_buf_size + 1),__oc_sizeof_void);  
    }  

    offset += __odbc_param->__mi_bind_size;
    __parambuf_size += __odbc_param->__mi_bind_size;
  }
  
  __p_parambuf = new char[__parambuf_size];
  memset(__p_parambuf,0,__parambuf_size);

  //-------------------------------------------------------------------------
  // 3. bind param data  
  //------------------------------------------------------------------------- 
  for(int paramIndex = 1;paramIndex <= __mi_param_count;++paramIndex)
  {
    __odbc_param = __vector_params[paramIndex];
    __odbc_sqlreturn = SQL_SUCCESS;
     
    switch(__odbc_param->__mi_bind_type)
    {
      case SQL_BIT:
      case SQL_TINYINT:
      {
        __ODBC_SETBINDABLE_IND(__odbc_param,sizeof(SQLCHAR));
        __ODBC_SQLBINDPARAMETER(SQL_PARAM_INPUT,SQL_C_CHAR, SQL_TINYINT, 0,0, &(__odbc_param->__mu_value.odbc_TINYINT),0);
        __XDBC_HOOK_CHECK(return);
      }
      break;  
  //-----------------------------
      case SQL_SMALLINT:
      {
        __ODBC_SETBINDABLE_IND(__odbc_param,sizeof(SQLSMALLINT));
        __ODBC_SQLBINDPARAMETER(SQL_PARAM_INPUT,SQL_C_SHORT, SQL_SMALLINT, 0,0, &(__odbc_param->__mu_value.odbc_SMALLINT),0);
        __XDBC_HOOK_CHECK(return);
      }
      break;  
  //-----------------------------
      case SQL_INTEGER:
      {
        __ODBC_SETBINDABLE_IND(__odbc_param,sizeof(SQLINTEGER));
        __ODBC_SQLBINDPARAMETER(SQL_PARAM_INPUT,SQL_C_LONG, SQL_INTEGER, 0,0, &(__odbc_param->__mu_value.odbc_INTEGER),0);
        __XDBC_HOOK_CHECK(return);
      }
      break;  
  //-----------------------------
      case SQL_BIGINT:
      {
        __ODBC_SETBINDABLE_IND(__odbc_param,sizeof(SQLBIGINT));
        __ODBC_SQLBINDPARAMETER(SQL_PARAM_INPUT,SQL_C_BIGINT, SQL_BIGINT, 0,0, &(__odbc_param->__mu_value.odbc_BIGINT),0);
        __XDBC_HOOK_CHECK(return);
      }
      break;  
  //-----------------------------
      case SQL_REAL:
      {
        __ODBC_SETBINDABLE_IND(__odbc_param,sizeof(SQLREAL));
        __ODBC_SQLBINDPARAMETER(SQL_PARAM_INPUT,SQL_C_FLOAT, SQL_REAL, 0,0, &(__odbc_param->__mu_value.odbc_REAL),0);
        __XDBC_HOOK_CHECK(return);
      }
      break;  
  //-----------------------------
      case SQL_FLOAT:
      case SQL_DOUBLE:
      {
        __ODBC_SETBINDABLE_IND(__odbc_param,sizeof(SQLDOUBLE));
        __ODBC_SQLBINDPARAMETER(SQL_PARAM_INPUT,SQL_C_DOUBLE, SQL_DOUBLE, 0,0, &(__odbc_param->__mu_value.odbc_DOUBLE),0);
        __XDBC_HOOK_CHECK(return); 
      }
      break;  
  //-----------------------------
      case SQL_NUMERIC:
      case SQL_DECIMAL:
      {
        __ODBC_SETBINDABLE_IND(__odbc_param,sizeof(SQLDOUBLE));
        __ODBC_SQLBINDPARAMETER(SQL_PARAM_INPUT,__odbc_param->__mi_bind_c_type, __odbc_param->__mi_bind_type, 0,0, &(__odbc_param->__mu_value.odbc_DOUBLE),0);
        __XDBC_HOOK_CHECK(return); 
      }
      break;  
  //-----------------------------
      case SQL_TYPE_DATE:
      {
        __ODBC_SETBINDABLE_IND(__odbc_param,sizeof(DATE_STRUCT));
        __ODBC_SQLBINDPARAMETER(SQL_PARAM_INPUT,SQL_C_TYPE_DATE, SQL_TYPE_DATE,0, 0, &(__odbc_param->__mu_value.odbc_DATE),0);        
        __XDBC_HOOK_CHECK(return);
      }
      break;  
  //-----------------------------
      case SQL_TYPE_TIME:
      {
        __ODBC_SETBINDABLE_IND(__odbc_param,sizeof(TIME_STRUCT));  
        __ODBC_SQLBINDPARAMETER(SQL_PARAM_INPUT,SQL_C_TYPE_TIME, SQL_TYPE_TIME,0, 0, &(__odbc_param->__mu_value.odbc_TIME),0);
        __XDBC_HOOK_CHECK(return);
      }
      break;  
  //-----------------------------
      case SQL_TYPE_TIMESTAMP:
      {
        __ODBC_SETBINDABLE_IND(__odbc_param,sizeof(TIMESTAMP_STRUCT));
        __ODBC_SQLBINDPARAMETER(SQL_PARAM_INPUT,SQL_C_TYPE_TIMESTAMP, SQL_TYPE_TIMESTAMP,0, 0, &(__odbc_param->__mu_value.odbc_TIMESTAMP),0);  
        __XDBC_HOOK_CHECK(return);
      }
      break;
 //-----------------------------
      case SQL_CHAR:
      case SQL_VARCHAR:
      case SQL_LONGVARCHAR:
      case SQL_BINARY:
      case SQL_VARBINARY:
      case SQL_LONGVARBINARY:
      default:
      {
        char* buf_start = __p_parambuf + __odbc_param->__mi_buf_offset;
        int   buf_size  = __odbc_param->__mi_bind_size;     
        __ODBC_SETBINDABLE_IND(__odbc_param,buf_size);  
        __ODBC_SQLBINDPARAMETER(SQL_PARAM_INPUT,__odbc_param->__mi_bind_c_type, __odbc_param->__mi_bind_type,0, 0, buf_start,buf_size); 
        __XDBC_HOOK_CHECK(return);
      }
      break;
    }  
  }
  
  //-------------------------------------------------------------------------
  // 4. check & prepare column buffer 
  //-------------------------------------------------------------------------
  if(!__isDefined)
  {
    __cachedCollection = new OdbcResultSetCollection();
    __adoptIt = true;

    __isDefined = true;
    OdbcKit::parseResultSetMetaData(__XDBC_HOOK_INVOKE_MORE __mp_conn,odbc_hstmt,__cachedCollection->__vector_columns);
    __XDBC_HOOK_CHECK(return);
  
    OdbcResultSet::prepareColumnData(__XDBC_HOOK_INVOKE_MORE __mp_conn,__ms_odbc_sql,odbc_hstmt,__cachedCollection);
    __XDBC_HOOK_CHECK(return);     
    
    OdbcResultSet::defineColumnData(__XDBC_HOOK_INVOKE_MORE __mp_conn,odbc_hstmt,__cachedCollection);
    //__XDBC_HOOK_CHECK(return);   
  }
}

ResultSet* OdbcPreparedStatement::executeQuery(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
     StringBuffer sb;
     sb.append("executeQuery|");
     sb.append(__ms_odbc_sql);
     
     __XDBC_FORCED_TRACE(__ODBCPREPAREDSTATEMENT_CLASS_NAME,sb.str()); 
  }


  SQLRETURN __odbc_sqlreturn = SQL_SUCCESS;

  if(__mi_resultSetType == ResultSet::TYPE_FORWARD_ONLY)
  {
    __odbc_sqlreturn = (__api_SQLEXECUTE)(odbc_hstmt); 
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
        OdbcKit::checkException(__XDBC_HOOK_INVOKE_MORE  __mp_conn, odbc_hstmt,__odbc_sqlreturn,__ODBCPREPAREDSTATEMENT_CLASS_NAME);
        __XDBC_HOOK_CHECK(return 0);
      }
    }
    
    __odbc_sqlreturn = (__api_SQLEXECUTE)(odbc_hstmt);
  }
  
  
  if(__ODBC_HOOK_CHECK_FAILED(__odbc_sqlreturn))
  {
    OdbcKit::checkException(__XDBC_HOOK_INVOKE_MORE  __mp_conn, odbc_hstmt,__odbc_sqlreturn,__ODBCPREPAREDSTATEMENT_CLASS_NAME);
    __XDBC_HOOK_CHECK(return __p_rs_opened);
  }
    
  if(__p_rs_opened == 0)
  {
    __p_rs_opened  = new OdbcResultSet(__mp_conn,this,odbc_hstmt,__mi_resultSetType,__mi_resultSetConcurrency,__cachedCollection,false,false);
  }
  else
  {
    __p_rs_opened->__mb_isClosed =  false;
    __p_rs_opened->__mi_cur_row = 0;
  }

  return __p_rs_opened;
}

int OdbcPreparedStatement::executeUpdate(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
     StringBuffer sb;
     sb.append("executeUpdate|");
     sb.append(__ms_odbc_sql);
     
     __XDBC_FORCED_TRACE(__ODBCPREPAREDSTATEMENT_CLASS_NAME,sb.str()); 
  }
 
  SQLRETURN __odbc_sqlreturn=(__api_SQLEXECUTE)(odbc_hstmt); 
  if(__ODBC_HOOK_CHECK_FAILED(__odbc_sqlreturn))
  {
    OdbcKit::checkException(__XDBC_HOOK_INVOKE_MORE  __mp_conn, odbc_hstmt,__odbc_sqlreturn,__ODBCPREPAREDSTATEMENT_CLASS_NAME);
    __XDBC_HOOK_CHECK(return 0);
  }

  SQLINTEGER rowsFetched=0;
  __odbc_sqlreturn = __odbc_SQLRowCount(__mp_conn,odbc_hstmt,&rowsFetched);
  
  if(__ODBC_HOOK_CHECK_FAILED(__odbc_sqlreturn))
  {
    OdbcKit::checkException(__XDBC_HOOK_INVOKE_MORE  __mp_conn, odbc_hstmt,__odbc_sqlreturn,__ODBCPREPAREDSTATEMENT_CLASS_NAME);
  }
    
  return rowsFetched;
}

void OdbcPreparedStatement::setByte(__XDBC_HOOK_DECLARE_MORE  int paramIndex,const char& paramValue,bool is_null) __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    __sb << "setByte|index=" << paramIndex << ",value=" << paramValue << ",null=" << is_null;
    __XDBC_FORCED_TRACE(__ODBCPREPAREDSTATEMENT_CLASS_NAME,__sb.str()); 
  }

  __ODBC_PREPARED_CHECKPARAM(paramIndex); 
  ODBC_PARAM* __odbc_param = __vector_params[paramIndex];
  
  if( (__odbc_param->__mi_bind_type == SQL_TINYINT) || (__odbc_param->__mi_bind_type == SQL_BIT) )
  {
    __odbc_param->__mu_value.odbc_TINYINT = paramValue;
    __ODBC_SETBINDABLE_IND(__odbc_param,sizeof(SQLCHAR));
    return;
  }
  
  
  checkConversion(__XDBC_HOOK_INVOKE_MORE paramIndex,__odbc_param,SQL_TINYINT,(const char*)(&paramValue));
}

void OdbcPreparedStatement::setShort(__XDBC_HOOK_DECLARE_MORE  int paramIndex,const short& paramValue,bool is_null) __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    __sb << "setShort|index=" << paramIndex << ",value=" << paramValue << ",null=" << is_null;
    
    __XDBC_FORCED_TRACE(__ODBCPREPAREDSTATEMENT_CLASS_NAME,__sb.str()); 
  }

  __ODBC_PREPARED_CHECKPARAM(paramIndex); 
  ODBC_PARAM* __odbc_param = __vector_params[paramIndex];
  
  if(__odbc_param->__mi_bind_type == SQL_SMALLINT)
  {
    __odbc_param->__mu_value.odbc_SMALLINT = paramValue;
    __ODBC_SETBINDABLE_IND(__odbc_param,sizeof(SQLSMALLINT));
    return;
  }
  
  checkConversion(__XDBC_HOOK_INVOKE_MORE paramIndex,__odbc_param,SQL_SMALLINT,(const char*)(&paramValue));
}

void OdbcPreparedStatement::setInt(__XDBC_HOOK_DECLARE_MORE  int paramIndex,const int& paramValue,bool is_null) __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    __sb << "setInt|index=" << paramIndex << ",value=" << paramValue << ",null=" << is_null;
    
    __XDBC_FORCED_TRACE(__ODBCPREPAREDSTATEMENT_CLASS_NAME,__sb.str()); 
  }

  __ODBC_PREPARED_CHECKPARAM(paramIndex);
  ODBC_PARAM* __odbc_param = __vector_params[paramIndex];
  if(__odbc_param->__mi_bind_type == SQL_INTEGER)
  {
    __odbc_param->__mu_value.odbc_INTEGER = paramValue;
    __ODBC_SETBINDABLE_IND(__odbc_param,sizeof(SQLINTEGER));
    return;
  }
  
  checkConversion(__XDBC_HOOK_INVOKE_MORE paramIndex,__odbc_param,SQL_INTEGER,(const char*)(&paramValue));
}

void OdbcPreparedStatement::setLong(__XDBC_HOOK_DECLARE_MORE  int paramIndex,const XLONG& paramValue,bool is_null) __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    __sb << "setLong|index=" << paramIndex << ",value=" << paramValue << ",null=" << is_null;
    
    __XDBC_FORCED_TRACE(__ODBCPREPAREDSTATEMENT_CLASS_NAME,__sb.str()); 
  }

  __ODBC_PREPARED_CHECKPARAM(paramIndex);
  ODBC_PARAM* __odbc_param = __vector_params[paramIndex];
  if(__odbc_param->__mi_bind_type == SQL_BIGINT)
  {
    __odbc_param->__mu_value.odbc_BIGINT = paramValue;
    __ODBC_SETBINDABLE_IND(__odbc_param,sizeof(SQLBIGINT));
    return;
  }
  
  checkConversion(__XDBC_HOOK_INVOKE_MORE paramIndex,__odbc_param,SQL_BIGINT,(const char*)(&paramValue));
}

void OdbcPreparedStatement::setFloat(__XDBC_HOOK_DECLARE_MORE  int paramIndex,const float& paramValue,bool is_null) __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    __sb << "setFloat|index=" << paramIndex << ",value=" << paramValue << ",null=" << is_null;
    __XDBC_FORCED_TRACE(__ODBCPREPAREDSTATEMENT_CLASS_NAME,__sb.str()); 
  }

  __ODBC_PREPARED_CHECKPARAM(paramIndex);
  ODBC_PARAM* __odbc_param = __vector_params[paramIndex]; 
  if(__odbc_param->__mi_bind_type == SQL_REAL)
  {
    __odbc_param->__mu_value.odbc_REAL = paramValue;
    __ODBC_SETBINDABLE_IND(__odbc_param,sizeof(SQLREAL));
    return;
  }
   
  checkConversion(__XDBC_HOOK_INVOKE_MORE paramIndex,__odbc_param,SQL_REAL,(const char*)(&paramValue));
}

void OdbcPreparedStatement::setDouble(__XDBC_HOOK_DECLARE_MORE  int paramIndex,const double& paramValue,bool is_null) __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    __sb << "setDouble|index=" << paramIndex << ",value=" << paramValue << ",null=" << is_null;
    
    __XDBC_FORCED_TRACE(__ODBCPREPAREDSTATEMENT_CLASS_NAME,__sb.str()); 
  }

  __ODBC_PREPARED_CHECKPARAM(paramIndex);
  ODBC_PARAM* __odbc_param = __vector_params[paramIndex];  
  switch(__odbc_param->__mi_bind_type)
  {
    case SQL_FLOAT:
    case SQL_DOUBLE:
    case SQL_NUMERIC:
    case SQL_DECIMAL:
    {
      __odbc_param->__mu_value.odbc_DOUBLE = paramValue;
      __ODBC_SETBINDABLE_IND(__odbc_param,sizeof(SQLDOUBLE));
      return;
    } 
    break;
  }
   
  checkConversion(__XDBC_HOOK_INVOKE_MORE paramIndex,__odbc_param,SQL_DOUBLE,(const char*)(&paramValue));
}

void OdbcPreparedStatement::setDate(__XDBC_HOOK_DECLARE_MORE  int paramIndex,const XDBC_DATE* paramValue,bool is_null)__XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    if(paramValue != 0)
    {
      __sb << "setDate|index=" << paramIndex << ",value=" << XDBCUtilities::toString(*paramValue) << ",null=" << is_null;
    }
    else
    {
      __sb << "setDate|index=" << paramIndex << ",value=<NULL>,null=" << is_null;
    }
    
    __XDBC_FORCED_TRACE(__ODBCPREPAREDSTATEMENT_CLASS_NAME,__sb.str()); 
  }

  __ODBC_PREPARED_CHECKPARAM(paramIndex);
  ODBC_PARAM* __odbc_param = __vector_params[paramIndex];   
  if(__odbc_param->__mi_bind_type == SQL_TYPE_DATE )
  {
    if(paramValue != 0)
    {
      DATE_STRUCT& odbc_DATE =  __odbc_param->__mu_value.odbc_DATE;
      odbc_DATE.year = paramValue->year;
      odbc_DATE.month = paramValue->month;
      odbc_DATE.day = paramValue->day;
      __ODBC_SETBINDABLE_IND(__odbc_param,sizeof(DATE_STRUCT));
    }
    else
    {
    }
    return;
  }
  
  checkConversion(__XDBC_HOOK_INVOKE_MORE paramIndex,__odbc_param,SQL_TYPE_DATE,(const char*)(paramValue));
}

void OdbcPreparedStatement::setTime(__XDBC_HOOK_DECLARE_MORE  int paramIndex,const XDBC_TIME* paramValue,bool is_null) __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    if(paramValue != 0)
    {
      __sb << "setTime|index=" << paramIndex << ",value=" << XDBCUtilities::toString(*paramValue) << ",null=" << is_null;
    }
    else
    {
      __sb << "setTime|index=" << paramIndex << ",value=<NULL>,null=" << is_null;
    }
    
    __XDBC_FORCED_TRACE(__ODBCPREPAREDSTATEMENT_CLASS_NAME,__sb.str()); 
  }


  __ODBC_PREPARED_CHECKPARAM(paramIndex);
  ODBC_PARAM* __odbc_param = __vector_params[paramIndex];
  if(__odbc_param->__mi_bind_type == SQL_TYPE_TIME )
  {
    if(paramValue != 0)
    {
      TIME_STRUCT& odbc_TIME =  __odbc_param->__mu_value.odbc_TIME;
      
      odbc_TIME.hour = paramValue->hour;
      odbc_TIME.minute = paramValue->minute;
      odbc_TIME.second = paramValue->second;
      __ODBC_SETBINDABLE_IND(__odbc_param,sizeof(TIME_STRUCT));
    }
    else
    {
    }
    return;
  }

  checkConversion(__XDBC_HOOK_INVOKE_MORE paramIndex,__odbc_param,SQL_TYPE_TIME,(const char*)(paramValue));
}

void OdbcPreparedStatement::setTimestamp(__XDBC_HOOK_DECLARE_MORE  int paramIndex,const XDBC_TIMESTAMP* paramValue,bool is_null) __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    if(paramValue != 0)
    {
      __sb << "setTimestamp|index=" << paramIndex << ",value=" << XDBCUtilities::toString(*paramValue) << ",null=" << is_null;
    }
    else
    {
      __sb << "setTimestamp|index=" << paramIndex << ",value=<NULL>,null=" << is_null;
    }
    
    __XDBC_FORCED_TRACE(__ODBCPREPAREDSTATEMENT_CLASS_NAME,__sb.str());     
  }

  __ODBC_PREPARED_CHECKPARAM(paramIndex);
  ODBC_PARAM* __odbc_param = __vector_params[paramIndex]; 
  if(__odbc_param->__mi_bind_type == SQL_TYPE_TIMESTAMP )
  {
    if(paramValue != 0)
    {
      TIMESTAMP_STRUCT& odbc_TIMESTAMP =  __odbc_param->__mu_value.odbc_TIMESTAMP; 
      OdbcConverter::setOdbcTimestamp(odbc_TIMESTAMP,*paramValue,__odbc_param);
      __ODBC_SETBINDABLE_IND(__odbc_param,sizeof(TIMESTAMP_STRUCT));
    }
    else
    {
    }
    return;
  }
  
  checkConversion(__XDBC_HOOK_INVOKE_MORE paramIndex,__odbc_param,SQL_TYPE_TIMESTAMP,(const char*)(paramValue));
}

void OdbcPreparedStatement::setInterval(__XDBC_HOOK_DECLARE_MORE  int paramIndex,  const XDBC_INTERVAL* paramValue,bool is_null) __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    if(paramValue != 0)
    {
      __sb << "setInterval|index=" << paramIndex << ",value=" << XDBCUtilities::toString(*paramValue) << ",null=" << is_null;
    }
    else
    {
      __sb << "setInterval|index=" << paramIndex << ",value=<NULL>,null=" << is_null;
    }
    
    __XDBC_FORCED_TRACE(__ODBCPREPAREDSTATEMENT_CLASS_NAME,__sb.str());       
  }
  
  __XDBC_HOOK_RETURN(return,"setInterval() not supported");
}

void OdbcPreparedStatement::setString(__XDBC_HOOK_DECLARE_MORE  int paramIndex,const string* paramValue,bool is_null) __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    if(paramValue != 0)
    {
      __sb << "setString|index=" << paramIndex << ",value=" << (*paramValue) << ",null=" << is_null;
    }
    else
    {
      __sb << "setString|index=" << paramIndex << ",value=<NULL>,null=" << is_null;
    }
    
    __XDBC_FORCED_TRACE(__ODBCPREPAREDSTATEMENT_CLASS_NAME,__sb.str()); 
  }

  __ODBC_PREPARED_CHECKPARAM(paramIndex);
  ODBC_PARAM* __odbc_param = __vector_params[paramIndex];  

  switch(__odbc_param->__mi_bind_type)
  {
    case SQL_CHAR:
    case SQL_VARCHAR:
    case SQL_LONGVARCHAR:
    case SQL_BINARY:
    case SQL_VARBINARY:
    case SQL_LONGVARBINARY:
    {
      char* buf_start = __p_parambuf + __odbc_param->__mi_buf_offset;
      int   buf_size  = __odbc_param->__mi_bind_size;     
      if(paramValue != 0)
      {
        __ODBC_DATA_MEMCPY(paramValue->data(),(int)paramValue->size(),buf_start,buf_size);
      }
      else
      {
      }
      return;
    } 
    break;
  }
  
  const char* __dataP = 0;
  int __dataSize = 0;
  if(paramValue != 0)
  {
    __dataP = paramValue->data();
    __dataSize = paramValue->size();
  } 
  
  checkConversion(__XDBC_HOOK_INVOKE_MORE paramIndex,__odbc_param,SQL_VARCHAR,(const char*)(__dataP),__dataSize);
}

void OdbcPreparedStatement::setString(__XDBC_HOOK_DECLARE_MORE  int paramIndex,  const char* paramValue,int count,bool is_null) __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    if(paramValue != 0)
    {
      __sb << "setString|index=" << paramIndex << ",value=" << paramValue << ",count=" << count << ",null=" << is_null;
    }
    else
    {
      __sb << "setString|index=" << paramIndex << ",value=<NULL>,count=" << count << ",null=" << is_null;
    }
    
    __XDBC_FORCED_TRACE(__ODBCPREPAREDSTATEMENT_CLASS_NAME,__sb.str()); 
  }

  __ODBC_PREPARED_CHECKPARAM(paramIndex);
  ODBC_PARAM* __odbc_param = __vector_params[paramIndex];
  switch(__odbc_param->__mi_bind_type)
  {
    case SQL_CHAR:
    case SQL_VARCHAR:
    case SQL_LONGVARCHAR:
    case SQL_BINARY:
    case SQL_VARBINARY:
    case SQL_LONGVARBINARY:
    {
      char* buf_start = __p_parambuf + __odbc_param->__mi_buf_offset;
      int   buf_size  = __odbc_param->__mi_bind_size;     
      if(paramValue != 0)
      {
        __ODBC_DATA_MEMCPY(paramValue,count,buf_start,buf_size);
      }
      else
      {
      }
      return;
    } 
    break;
  }
  
  checkConversion(__XDBC_HOOK_INVOKE_MORE paramIndex,__odbc_param,SQL_VARCHAR,(const char*)(paramValue),count);
}

void OdbcPreparedStatement::setBlob (__XDBC_HOOK_DECLARE_MORE  int paramIndex, const char* paramValue,int count,bool is_null) __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    if(paramValue != 0)
    {
      __sb << "setBlob|index=" << paramIndex << ",value=<BLOB>[" << count << "],null=" << is_null;
    }
    else
    {
      __sb << "setBlob|index=" << paramIndex << ",value=<NULL>,null=" << is_null;
    }

    __XDBC_FORCED_TRACE(__ODBCPREPAREDSTATEMENT_CLASS_NAME,__sb.str());
  }

  __ODBC_PREPARED_CHECKPARAM(paramIndex);
  ODBC_PARAM* __odbc_param = __vector_params[paramIndex];
  switch(__odbc_param->__mi_bind_type)
  {
    case SQL_BINARY:
    case SQL_VARBINARY:
    case SQL_LONGVARBINARY:
    {
      char* buf_start = __p_parambuf + __odbc_param->__mi_buf_offset;
      int   buf_size  = __odbc_param->__mi_bind_size;     
      if(paramValue != 0)
      {
        __ODBC_DATA_MEMCPY(paramValue,count,buf_start,buf_size);
      }
      else
      {
      }
      return;
    } 
    break;
  }
  
  checkConversion(__XDBC_HOOK_INVOKE_MORE paramIndex,__odbc_param,SQL_VARBINARY,(const char*)(paramValue),count);
}

void OdbcPreparedStatement::setBlob (__XDBC_HOOK_DECLARE_MORE  int paramIndex,const Blob* paramValue,bool is_null) __XDBC_THROW_DECLARE(SQLException)
{
  string __blobData;
  if(paramValue != 0)
  {
    OdbcBlob* __p_blob = (OdbcBlob *)paramValue;
    int __length = __p_blob->length(__XDBC_HOOK_INVOKE_ONCE);  
    __XDBC_HOOK_CHECK(return);
        
    __blobData.resize(__length);
    __p_blob->read(__XDBC_HOOK_INVOKE_MORE 1, (char*)__blobData.data(),0,__length); 
    __XDBC_HOOK_CHECK(return);    
  }
  
  setBlob (__XDBC_HOOK_INVOKE_MORE paramIndex, __blobData.data(), (int)__blobData.size(), is_null);
}

void OdbcPreparedStatement::setClob (__XDBC_HOOK_DECLARE_MORE  int paramIndex, const char* paramValue,int count,bool is_null) __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    if(paramValue != 0)
    {
      __sb << "setClob|index=" << paramIndex << ",value=<CLOB>[" << count << "],null=" << is_null;
    }
    else
    {
      __sb << "setClob|index=" << paramIndex << ",value=<NULL>,null=" << is_null;
    }

    __XDBC_FORCED_TRACE(__ODBCPREPAREDSTATEMENT_CLASS_NAME,__sb.str());
  }


  __ODBC_PREPARED_CHECKPARAM(paramIndex);
  ODBC_PARAM* __odbc_param = __vector_params[paramIndex];
  switch(__odbc_param->__mi_bind_type)
  {
    case SQL_CHAR:
    case SQL_VARCHAR:
    case SQL_LONGVARCHAR:
    {
      char* buf_start = __p_parambuf + __odbc_param->__mi_buf_offset;
      int   buf_size  = __odbc_param->__mi_bind_size;     
      if(paramValue != 0)
      {
        __ODBC_DATA_MEMCPY(paramValue,count,buf_start,buf_size);
      }
      else
      {
      }
      return;
    } 
    break;
  }
  
  checkConversion(__XDBC_HOOK_INVOKE_MORE paramIndex,__odbc_param,SQL_VARCHAR,(const char*)(paramValue),count);
}

void OdbcPreparedStatement::setClob (__XDBC_HOOK_DECLARE_MORE  int paramIndex,const Clob* paramValue,bool is_null) __XDBC_THROW_DECLARE(SQLException)
{
  string __clobData;
  if(paramValue != 0)
  {
    OdbcClob* __p_clob = (OdbcClob *)paramValue;
    int __length = __p_clob->length(__XDBC_HOOK_INVOKE_ONCE);  
    __XDBC_HOOK_CHECK(return);
    
    __clobData.resize(__length);
    __p_clob->read(__XDBC_HOOK_INVOKE_MORE 1, (char*)__clobData.data(),0,__length); 
    __XDBC_HOOK_CHECK(return);
  }
  
  setClob(__XDBC_HOOK_INVOKE_MORE paramIndex, __clobData.data(), (int)__clobData.size(), is_null);
}


bool  OdbcPreparedStatement::checkConversion(__XDBC_HOOK_DECLARE_MORE int paramIndex,ODBC_PARAM* odbc_param,int funType,const char* funValue,int funValueSize)__XDBC_THROW_DECLARE(SQLException)
{
  ODBC_PARAM* __p_odbc_param = odbc_param;
  if(__p_odbc_param == 0)
  {
    __p_odbc_param = __vector_params[paramIndex];;
  }
  ODBC_PARAM* __odbc_param = __p_odbc_param; 
 
  int varType =__p_odbc_param->__mi_bind_type;
 
  if(funType == varType)
  {
    return false;
  }
  
  switch(varType)
  {
    case SQL_BIT:
    case SQL_TINYINT:
    {
    }
    break;  
//-----------------------------
    case SQL_SMALLINT:
    {
      switch(funType)
      {
        case SQL_TINYINT:
        {
          __p_odbc_param->__mu_value.odbc_SMALLINT = *funValue;
          return true;
        }
        break;  
      } 
    }
    break;  
//-----------------------------
    case SQL_INTEGER:
    {
      switch(funType)
      {
        case SQL_TINYINT:
        {
          __p_odbc_param->__mu_value.odbc_INTEGER = *funValue;
          return true;
        }
        break;  
        case SQL_SMALLINT:
        {
          const short* __P_funValue = (const short*)(funValue);
          __p_odbc_param->__mu_value.odbc_INTEGER = *__P_funValue;
          return true;
        }
        break;
      } 
    }
    break;  
//-----------------------------
    case SQL_BIGINT:
    {
      switch(funType)
      {
        case SQL_TINYINT:
        {
          __p_odbc_param->__mu_value.odbc_BIGINT = *funValue;
          return true;
        }
        break;  
        case SQL_SMALLINT:
        {
          const short* __P_funValue = (const short*)(funValue);
          __p_odbc_param->__mu_value.odbc_BIGINT = *__P_funValue;
          return true;
        }
        break;
        case SQL_INTEGER:
        {
          const int* __P_funValue = (const int*)(funValue);
          __p_odbc_param->__mu_value.odbc_BIGINT = *__P_funValue;
          return true;
        }
        break;
      } 
    }
    break;  
//-----------------------------
    case SQL_REAL:
    {
      switch(funType)
      {
        case SQL_TINYINT:
        {
          __p_odbc_param->__mu_value.odbc_REAL = *funValue;
          return true;
        }
        break;  
        case SQL_SMALLINT:
        {
          const short* __P_funValue = (const short*)(funValue);
          __p_odbc_param->__mu_value.odbc_REAL = *__P_funValue;
          return true;
        }
        break;
        case SQL_INTEGER:
        {
          const int* __P_funValue = (const int*)(funValue);
          __p_odbc_param->__mu_value.odbc_REAL = *__P_funValue;
          return true;
        }
        break;
      } 
    }
    break;  
//-----------------------------
    case SQL_FLOAT:
    case SQL_DOUBLE:
    case SQL_NUMERIC:
    case SQL_DECIMAL:
    {
      switch(funType)
      {
        case SQL_TINYINT:
        {
          __p_odbc_param->__mu_value.odbc_DOUBLE = *funValue;
          return true;
        }
        break;  
        case SQL_SMALLINT:
        {
          const short* __P_funValue = (const short*)(funValue);
          __p_odbc_param->__mu_value.odbc_DOUBLE = *__P_funValue;
          return true;
        }
        break;
        case SQL_INTEGER:
        {
          const int* __P_funValue = (const int*)(funValue);
          __p_odbc_param->__mu_value.odbc_DOUBLE = *__P_funValue;
          return true;
        }
        break;
        case SQL_BIGINT:
        {
          const XLONG* __P_funValue = (const XLONG*)(funValue);
          __p_odbc_param->__mu_value.odbc_DOUBLE = *__P_funValue;
          return true;
        }
        break;
        case SQL_REAL:
        {
          const float* __P_funValue = (const float*)(funValue);
          __p_odbc_param->__mu_value.odbc_DOUBLE = *__P_funValue;
          return true;
        }
        break;
      } 
    }
    break;  
//-----------------------------
    case SQL_TYPE_DATE:
    {     
    }
    break;  
//-----------------------------
    case SQL_TYPE_TIME:
    {
    }
    break;  
//-----------------------------
    case SQL_TYPE_TIMESTAMP:
    { 
    }
//-----------------------------
    case SQL_CHAR:
    case SQL_VARCHAR:
    case SQL_LONGVARCHAR:
    case SQL_BINARY:
    case SQL_VARBINARY:
    case SQL_LONGVARBINARY:
    default:
    {
      switch(funType)
      {
        case SQL_INTEGER:
        {
          const int* __P_funValue = (const int*)(funValue);
          string __str_funValue = Number::toString(*__P_funValue);
          char* buf_start = __p_parambuf + __p_odbc_param->__mi_buf_offset;
          int   buf_size  = __p_odbc_param->__mi_bind_size;
          __ODBC_DATA_MEMCPY(__str_funValue.data(),(int)__str_funValue.size(),buf_start,buf_size);
          return true;
        }
        break;
//-----------------------------------------------------------------------------
        case SQL_BIGINT:
        {
          const XLONG* __P_funValue = (const XLONG*)(funValue);
          string __str_funValue = Number::toString(*__P_funValue);
          char* buf_start = __p_parambuf + __p_odbc_param->__mi_buf_offset;
          int   buf_size  = __p_odbc_param->__mi_bind_size;
          __ODBC_DATA_MEMCPY(__str_funValue.data(),(int)__str_funValue.size(),buf_start,buf_size);
          return true;
        }
        break;
//-----------------------------------------------------------------------------
        case SQL_REAL:
        {
          const float* __P_funValue = (const float*)(funValue);
          string __str_funValue = Number::toString(*__P_funValue);
          char* buf_start = __p_parambuf + __p_odbc_param->__mi_buf_offset;
          int   buf_size  = __p_odbc_param->__mi_bind_size;
          __ODBC_DATA_MEMCPY(__str_funValue.data(),(int)__str_funValue.size(),buf_start,buf_size);
          return true;
        }
        break;
//-----------------------------------------------------------------------------
        case SQL_DOUBLE:
        {
          const double* __P_funValue = (const double*)(funValue);
          string __str_funValue = Number::toString(*__P_funValue);
          char* buf_start = __p_parambuf + __p_odbc_param->__mi_buf_offset;
          int   buf_size  = __p_odbc_param->__mi_bind_size;
          __ODBC_DATA_MEMCPY(__str_funValue.data(),(int)__str_funValue.size(),buf_start,buf_size);
          return true;
        }
        break;



      }
    }
    break;
  }  
  
  {
    OdbcTypes* __odbcTypes = &(OdbcTypesImpl::getInstance());
    struct ODBC_DRIVER_OPTIONS* __odbc_options = __mp_conn->__odbc_options;
    
    StringBuffer __sb_0;
    __sb_0.append("Conversion not supported. paramIndex:");
    __sb_0.append("#");
    __sb_0.append(paramIndex);
    __sb_0.append(",FUNC_SQL_TYPE:");
    __sb_0.append(__odbcTypes->getTypeName(__odbc_options,funType));
    __sb_0.append("(");
    __sb_0.append(funType);
    __sb_0.append("),VAR_SQL_TYPE:");
    __sb_0.append(__odbcTypes->getTypeName(__odbc_options,varType));
    __sb_0.append("(");
    __sb_0.append(varType);
    __sb_0.append(")");
  
    StringBuffer __sb;
    __sb << "[OdbcPreparedStatement] - " << __sb_0;
    __XDBC_HOOK_RETURN(return false,__sb.str());
  }
}
  
void  OdbcPreparedStatement::close()
{
  __XDBC_TRACE(__ODBCPREPAREDSTATEMENT_CLASS_NAME,"close()");

  if(__mb_isClosed)
  {
    return;
  }
  
  __mb_isClosed=true;

  for(int i = 1; i <= __mi_param_count;++i)
  {
    delete __vector_params[i];
    __vector_params[i] = 0;
  }
     
  if(odbc_hstmt != SQL_NULL_HSTMT)
  {
    __odbc_SQLFreeHandle(__mp_conn,SQL_HANDLE_STMT,odbc_hstmt); 
    odbc_hstmt = SQL_NULL_HSTMT;
  }
}

void  OdbcPreparedStatement::destroy()
{
  if(__p_parambuf != 0)
  {
    delete []__p_parambuf;
  }  
  
  if(__adoptIt && (__cachedCollection != 0))
  {
    delete __cachedCollection;
  }
  
  if(__p_rs_opened != 0)
  {
    __p_rs_opened->__mb_selfDelete = true;
    __XDBC_INTERFACE_CLOSEANDDESTROY(__p_rs_opened);
  }
  delete this;
}
  
int OdbcPreparedStatement::getResultSetType(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{
  return __mi_resultSetType;
}

Connection* OdbcPreparedStatement::getConnection(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{
  return (Connection*)__mp_conn;
}

__OPENCODE_END_NAMESPACE
