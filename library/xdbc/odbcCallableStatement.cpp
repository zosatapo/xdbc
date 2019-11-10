#include "xdbc/odbcCallableStatement.h"
#include "xdbc/odbcClob.h"
#include "xdbc/odbcBlob.h"
#include "xdbc/odbcConnection.h"
#include "xdbc/odbcTypes.h"
#include "xdbc/xsmart.h"

#include "port/utilities.h"
#include "port/file.h"
#include "port/ioc_sbuf.h"

__OPENCODE_BEGIN_NAMESPACE

#define ODBCCALLABLESTATEMENT_CLASS_NAME "OdbcCallableStatement"

OdbcCallableStatement::~OdbcCallableStatement()
{
  __XDBC_TRACE(ODBCCALLABLESTATEMENT_CLASS_NAME,"destroy()");
}

OdbcCallableStatement::OdbcCallableStatement(OdbcConnection* conn,const string& sql,int paramCount)
{
  __mp_conn = conn;
  __api_SQLEXECUTE       = __mp_conn->__odbc_apis->__api_SQLEXECUTE;
  __mb_fintAvailable      = __mp_conn->__odbc_options->__mb_fintAvailable;
  __options_SUPPORTSSTMTATTR = __mp_conn->__odbc_options->__options_SUPPORTSSTMTATTR;
    
  __mi_resultSetType=ResultSet::TYPE_FORWARD_ONLY;
  __mi_resultSetConcurrency=ResultSet::CONCUR_READ_ONLY;
  
  odbc_hstmt = SQL_NULL_HSTMT;
  __mb_initialized = false;
  __mb_isClosed = false;
  __mb_stored_proc = true;
  
//--------------------------
  __ms_odbc_sql = sql;
  __mi_param_count = paramCount;   
  
  __p_parambuf = 0;
  __parambuf_size = 0;
}

OdbcCallableStatement::OdbcCallableStatement(OdbcConnection* conn,const string& sql, int resultSetType, int resultSetConcurrency,int paramCount)
{
  __mp_conn = conn;
  __api_SQLEXECUTE       = __mp_conn->__odbc_apis->__api_SQLEXECUTE;
  __mb_fintAvailable      = __mp_conn->__odbc_options->__mb_fintAvailable;
  
  __mi_resultSetType=resultSetType;
  __mi_resultSetConcurrency=resultSetConcurrency;
  
  odbc_hstmt = SQL_NULL_HSTMT;
  __mb_initialized = false;
  __mb_isClosed = false;
  __mb_stored_proc = true;
  
//--------------------------
  __ms_odbc_sql = sql;
  __mi_param_count = paramCount;   
  
  __p_parambuf = 0;
  __parambuf_size = 0;
}

//-------------------------------------------------
// Odbc Interfaces
//-------------------------------------------------
void OdbcCallableStatement::initialize(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{
  if(__mb_initialized)
  {
    return;
  }  
  
  __mb_initialized=true;
  
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
    OdbcKit::checkException(__XDBC_HOOK_INVOKE_MORE  __mp_conn, odbc_hstmt,__odbc_sqlreturn,ODBCCALLABLESTATEMENT_CLASS_NAME);
    __XDBC_HOOK_CHECK(return);
  }
           
  SQLINTEGER __option = SQL_ATTR_NOSCAN;
  SQLINTEGER __param = SQL_NOSCAN_ON;
  if((__options_SUPPORTSSTMTATTR)(__option, (SQLPOINTER )__param,(SQLINTEGER)SQL_IS_INTEGER) == SQL_SUCCESS)
  {
    __odbc_sqlreturn = __odbc_SQLSetStmtAttr(__mp_conn,odbc_hstmt,__option, (SQLPOINTER )__param,(SQLINTEGER)SQL_IS_INTEGER,(SQLPOINTER )&__param,(SQLINTEGER)SQL_IS_POINTER);
    if(__ODBC_HOOK_CHECK_FAILED(__odbc_sqlreturn))
    {
      OdbcKit::checkException(__XDBC_HOOK_INVOKE_MORE  __mp_conn, odbc_hstmt,__odbc_sqlreturn,ODBCCALLABLESTATEMENT_CLASS_NAME);
      __XDBC_HOOK_CHECK(return);
    }
  }

  //------------------------------------------------------------
  // 1. parse procedure name
  //------------------------------------------------------------  
  parseProcName(__XDBC_HOOK_INVOKE_ONCE);
  __XDBC_HOOK_CHECK(return);

  //------------------------------------------------------------
  // 2. retrive param info 
  //------------------------------------------------------------
  __odbc_sqlreturn = __odbc_SQLPrepare(__mp_conn,odbc_hstmt,(SQLCHAR*)__ms_odbc_sql.c_str(),SQL_NTS);
  if(__ODBC_HOOK_CHECK_FAILED(__odbc_sqlreturn))
  {
    OdbcKit::checkException(__XDBC_HOOK_INVOKE_MORE  __mp_conn, odbc_hstmt,__odbc_sqlreturn,ODBCCALLABLESTATEMENT_CLASS_NAME);
    __XDBC_HOOK_CHECK(return);
  }

  parseProcVariables();
  __mi_param_count= (int)(__vector_params.size() - 1);

  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    __sb << "parseProcVariables(" << __mi_param_count <<  ")";
    if(__mi_param_count > 0) {__sb << "|" << File::lineSeparator;}

    ODBC_PARAM* __odbc_param = 0;
    for(int i = 1;i <= __mi_param_count;++i)
    {
      __odbc_param = __vector_params[i];
      __odbc_param->dump(__sb);
    }

    __XDBC_FORCED_TRACE(ODBCCALLABLESTATEMENT_CLASS_NAME,__sb.str());
  }
    
  if(__mi_param_count == 0)
  {
    OdbcKit::prepareVariableData(__XDBC_HOOK_INVOKE_MORE __mp_conn,odbc_hstmt,__vector_params);
    __XDBC_HOOK_CHECK(return);
    __mi_param_count= (int)(__vector_params.size() - 1);   
  }//~end if(__mi_param_count == 0)

  //------------------------------------------------------------
  // 3. allocate param data buffer 
  //------------------------------------------------------------
  int offset = 0;
  size_t __oc_sizeof_void = sizeof(void*);
  __odbc_sqlreturn  = SQL_ERROR;
  ODBCSQLOPTIONS_ADJUSTBINDABLE  __options_ADJUSTBINDABLE = __mp_conn->__odbc_options->__options_ADJUSTBINDABLE;
  ODBC_PARAM* __odbc_param = 0; 
  for(int i = 1;i <= __mi_param_count;++i)
  {
    __odbc_param = __vector_params[i];
    __odbc_sqlreturn = (__options_ADJUSTBINDABLE)((SQLCHAR*)__ms_odbc_sql.c_str(),(SQLPOINTER)__odbc_param);
    if(__odbc_sqlreturn != SQL_SUCCESS)
    {
      OdbcKit::adjustBindable(__mp_conn,__odbc_param);
    }
                                   
    __odbc_param->__mi_buf_offset = offset;
    
    switch(__vector_params[i]->__mi_type)
    {
      case SQL_CHAR:
      case SQL_VARCHAR:
      case SQL_LONGVARCHAR:
      case SQL_BINARY:
      case SQL_VARBINARY:
      case SQL_LONGVARBINARY:
      {
        __odbc_param->__mi_buf_size = __odbc_param->__mi_length + 1;
      }
      break;
  //-----------------------------
      default:
      {
        __odbc_param->__mi_buf_size = 0;
      }
      break;
    }  

    if(__odbc_param->__mi_buf_size > __mp_conn->__odbc_options->__mi_xlob_max)
    {
    	__odbc_param->__mi_buf_size = __mp_conn->__odbc_options->__mi_xlob_max;
    }
     
    __odbc_param->__mi_bind_size = __odbc_param->__mi_buf_size;
    
    if(__odbc_param->__mi_bind_size != 0)
    {
      __odbc_param->__mi_bind_size = (int)MemoryKit::roundUp((__odbc_param->__mi_buf_size + 1),__oc_sizeof_void);  
    }  

    offset += __odbc_param->__mi_bind_size;
    __parambuf_size += __odbc_param->__mi_bind_size;
  }
  
  __p_parambuf = new char[__parambuf_size];
  memset(__p_parambuf,0,__parambuf_size);
}

void OdbcCallableStatement::registerOutParameter(__XDBC_HOOK_DECLARE_MORE int paramIndex, int xdbcType)  __XDBC_THROW_DECLARE(SQLException)
{
  registerOutParameter(__XDBC_HOOK_INVOKE_MORE paramIndex,xdbcType,-1);
}

void OdbcCallableStatement::registerOutParameter(__XDBC_HOOK_DECLARE_MORE int paramIndex, int xdbcType, int scale)  __XDBC_THROW_DECLARE(SQLException)
{
  __ODBC_CALL_CHECKPARAM(paramIndex);
  
  ODBC_PARAM* __odbc_param  = __vector_params[paramIndex];

  if(  (__odbc_param->__mi_iomode != SQL_PARAM_OUTPUT)
     &&(__odbc_param->__mi_iomode != SQL_PARAM_INPUT_OUTPUT)) 
  {
    StringBuffer __sb;
    __sb.append("[OdbcCallableStatement]  *ONLY* can registerOutParameter OUT parameter : ");
    __sb.append(paramIndex);
    __XDBC_HOOK_RETURN(return,__sb.str());
  }
  
  
  OdbcTypes* __odbcTypes = &(OdbcTypesImpl::getInstance());
  struct ODBC_DRIVER_OPTIONS* __odbc_options = __mp_conn->__odbc_options;
  int odbcType = __odbcTypes->xdbc_cnvt_odbc(__odbc_options,xdbcType); 
  
  if(DriverManager::isTracingOn)
  {
     StringBuffer __sb;
     __sb.append("registerOutParameter|paramIndex=");
     __sb.append(paramIndex);
     __sb.append(",XDBC=");
     __sb.append(xdbcType);
     __sb.append(",ODBC=");
     __sb.append(odbcType);

     __XDBC_FORCED_TRACE(ODBCCALLABLESTATEMENT_CLASS_NAME,__sb.str());
  }
  
  SQLRETURN __odbc_sqlreturn = SQL_SUCCESS;
  SQLPOINTER __p_bind_ind = (SQLPOINTER)0;
  if(__mb_fintAvailable)
  {
     __p_bind_ind = &(__odbc_param->__mi_bind_ind_int);
  }
  else
  {
  	 __p_bind_ind = &(__odbc_param->__mi_bind_ind_len);
	}	
   	
  switch(odbcType)
  {
//-----------------------------
    case SQL_BIT:
    case SQL_TINYINT:
    {                                               
      __odbc_param->__mi_bind_type = SQL_TINYINT;
      __odbc_sqlreturn = __odbc_SQLBindParameter(__mp_conn,odbc_hstmt, 
                                                     paramIndex, 
                                                     __odbc_param->__mi_iomode, 
                                                     SQL_C_TINYINT, 
                                                     SQL_TINYINT, 
                                                     0, 
                                                     0, 
                                                     &(__odbc_param->__mu_value.odbc_TINYINT), 
                                                     0, 
                                                     __p_bind_ind);
    }
    break;
//-----------------------------
    case SQL_SMALLINT:
    {
      __odbc_param->__mi_bind_type = SQL_SMALLINT;
      __odbc_sqlreturn = __odbc_SQLBindParameter(__mp_conn,odbc_hstmt, 
                                                    paramIndex, 
                                                    __odbc_param->__mi_iomode, 
                                                    SQL_C_SHORT, 
                                                    SQL_SMALLINT, 
                                                    0, 
                                                    0, 
                                                    &(__odbc_param->__mu_value.odbc_SMALLINT), 
                                                    0, 
                                                    __p_bind_ind);
    }
    break;
//-----------------------------
    case SQL_INTEGER:
    {
      __odbc_param->__mi_bind_type = SQL_INTEGER;
      __odbc_sqlreturn = __odbc_SQLBindParameter(__mp_conn,odbc_hstmt, 
                                                     paramIndex, 
                                                     __odbc_param->__mi_iomode, 
                                                     SQL_C_LONG, 
                                                     SQL_INTEGER, 
                                                     0, 
                                                     0, 
                                                     &(__odbc_param->__mu_value.odbc_INTEGER), 
                                                     0, 
                                                     __p_bind_ind);
    }                                                
    break;
//-----------------------------
    case SQL_BIGINT:
    {
      __odbc_param->__mi_bind_type = SQL_BIGINT;
      __odbc_sqlreturn = __odbc_SQLBindParameter(__mp_conn,odbc_hstmt, 
                                                     paramIndex, 
                                                     __odbc_param->__mi_iomode, 
                                                     SQL_C_BIGINT, 
                                                     SQL_BIGINT, 
                                                     0, 
                                                     0, 
                                                     &(__odbc_param->__mu_value.odbc_BIGINT), 
                                                     0, 
                                                     __p_bind_ind);
    }
    break;
//-----------------------------
    case SQL_REAL:
    {
      __odbc_param->__mi_bind_type = SQL_REAL;
      __odbc_sqlreturn = __odbc_SQLBindParameter(__mp_conn,odbc_hstmt, 
                                                     paramIndex, 
                                                     __odbc_param->__mi_iomode, 
                                                     SQL_C_FLOAT, 
                                                     SQL_REAL, 
                                                     0, 
                                                     0, 
                                                     &(__odbc_param->__mu_value.odbc_REAL), 
                                                     0, 
                                                     __p_bind_ind);
    }
    break;
//-----------------------------
    case SQL_FLOAT:
    case SQL_DOUBLE:
    {
      __odbc_param->__mi_bind_type = SQL_DOUBLE;
      __odbc_sqlreturn = __odbc_SQLBindParameter(__mp_conn,odbc_hstmt, 
                                                     paramIndex, 
                                                     __odbc_param->__mi_iomode, 
                                                     SQL_C_DOUBLE, 
                                                     SQL_DOUBLE, 
                                                     0, 
                                                     0, 
                                                     &(__odbc_param->__mu_value.odbc_DOUBLE), 
                                                     0, 
                                                     __p_bind_ind);
    }                                                
    break;
//-----------------------------
    case SQL_NUMERIC:
    case SQL_DECIMAL:
    { 
      __odbc_param->__mi_bind_type = SQL_DOUBLE;
      __odbc_sqlreturn = __odbc_SQLBindParameter(__mp_conn,odbc_hstmt, 
                                                     paramIndex, 
                                                     __odbc_param->__mi_iomode, 
                                                     __odbc_param->__mi_bind_c_type, 
                                                     __odbc_param->__mi_bind_type, 
                                                     0, 
                                                     0, 
                                                     &(__odbc_param->__mu_value.odbc_DOUBLE), 
                                                     0, 
                                                     __p_bind_ind);
    }                                                
    break;
//-----------------------------
    case SQL_TYPE_DATE:
    {
      __odbc_param->__mi_bind_type = SQL_TYPE_DATE;
      __odbc_sqlreturn = __odbc_SQLBindParameter(__mp_conn,odbc_hstmt, 
                                                     paramIndex, 
                                                     __odbc_param->__mi_iomode, 
                                                     SQL_C_TYPE_DATE, 
                                                     SQL_TYPE_DATE, 
                                                     0, 
                                                     0, 
                                                     &(__odbc_param->__mu_value.odbc_DATE), 
                                                     0, 
                                                     __p_bind_ind);
    }
    break;
//-----------------------------
    case SQL_TYPE_TIME:
    {
      __odbc_param->__mi_bind_type = SQL_TYPE_TIME;
      __odbc_sqlreturn = __odbc_SQLBindParameter(__mp_conn,odbc_hstmt, 
                                                     paramIndex, 
                                                     __odbc_param->__mi_iomode, 
                                                     SQL_C_TYPE_TIME, 
                                                     SQL_TYPE_TIME, 
                                                     0, 
                                                     0, 
                                                     &(__odbc_param->__mu_value.odbc_TIME), 
                                                     0, 
                                                     __p_bind_ind);
    }
    break;
//-----------------------------
    case SQL_TYPE_TIMESTAMP:
    {
      __odbc_param->__mi_bind_type = SQL_TYPE_TIMESTAMP;
      __odbc_sqlreturn = __odbc_SQLBindParameter(__mp_conn,odbc_hstmt, 
                                                     paramIndex, 
                                                     __odbc_param->__mi_iomode, 
                                                     SQL_C_TYPE_TIMESTAMP, 
                                                     SQL_TYPE_TIMESTAMP, 
                                                     0, 
                                                     0, 
                                                     &(__odbc_param->__mu_value.odbc_TIMESTAMP), 
                                                     0, 
                                                     __p_bind_ind);
    }
    break;
//-----------------------------
    case SQL_BINARY:
    case SQL_VARBINARY:
    case SQL_LONGVARBINARY:
    {
      char* __buf_start = __p_parambuf + __odbc_param->__mi_buf_offset;
      int   __buf_size  = __odbc_param->__mi_bind_size;    
      __odbc_sqlreturn = __odbc_SQLBindParameter(__mp_conn,odbc_hstmt, 
                                                     paramIndex, 
                                                     __odbc_param->__mi_iomode, 
                                                     SQL_C_BINARY, 
                                                     SQL_VARBINARY, 
                                                     0, 0, 
                                                     (void*)__buf_start, 
                                                     __buf_size, 
                                                     __p_bind_ind);
    }
    break;
//-----------------------------
    case SQL_CHAR:
    case SQL_VARCHAR:
    case SQL_LONGVARCHAR:
    {
      char* __buf_start = __p_parambuf + __odbc_param->__mi_buf_offset;
      int   __buf_size  = __odbc_param->__mi_bind_size;    
      __odbc_sqlreturn = __odbc_SQLBindParameter(__mp_conn,odbc_hstmt, 
                                                     paramIndex, 
                                                     __odbc_param->__mi_iomode, 
                                                     SQL_C_CHAR, 
                                                     SQL_VARCHAR, 
                                                     0, 0, 
                                                     (void*)__buf_start, 
                                                     __buf_size, 
                                                     __p_bind_ind);
    }
    break;
//-----------------------------
    default:
    {
      char* __buf_start = __p_parambuf + __odbc_param->__mi_buf_offset;
      int   __buf_size  = __odbc_param->__mi_bind_size;    
      __odbc_sqlreturn = __odbc_SQLBindParameter(__mp_conn,odbc_hstmt, 
                                                     paramIndex, 
                                                     __odbc_param->__mi_iomode, 
                                                     __odbc_param->__mi_bind_c_type, 
                                                     __odbc_param->__mi_bind_type, 
                                                     0, 0, 
                                                     (void*)__buf_start, 
                                                     __buf_size, 
                                                     __p_bind_ind);
    }
    break;
  }

  if(__ODBC_HOOK_CHECK_FAILED(__odbc_sqlreturn))
  {
    OdbcKit::checkException(__XDBC_HOOK_INVOKE_MORE  __mp_conn, odbc_hstmt,__odbc_sqlreturn,ODBCCALLABLESTATEMENT_CLASS_NAME);
    __XDBC_HOOK_CHECK(return);
  }  
}

bool OdbcCallableStatement::wasNull(__XDBC_HOOK_DECLARE_MORE int paramIndex)   __XDBC_THROW_DECLARE(SQLException)
{
  if(__mb_fintAvailable)
  {
    return __vector_params[paramIndex ]->__mi_bind_ind_int == SQL_NULL_DATA;  
  }
  else
  {
    return __vector_params[paramIndex ]->__mi_bind_ind_len == SQL_NULL_DATA;  
  } 
}

bool OdbcCallableStatement::getByte(__XDBC_HOOK_DECLARE_MORE int paramIndex,char& paramval)  __XDBC_THROW_DECLARE(SQLException)
{
  XLONG __val_int = 0;
  bool __ret_code = getLong(__XDBC_HOOK_INVOKE_MORE paramIndex,__val_int);
  __XDBC_HOOK_CHECK(return false);
  
  paramval = (char)__val_int; 
  return __ret_code;
}

bool OdbcCallableStatement::getShort(__XDBC_HOOK_DECLARE_MORE int paramIndex,short& paramval)  __XDBC_THROW_DECLARE(SQLException)
{
  XLONG __val_int = 0;
  bool __ret_code = getLong(__XDBC_HOOK_INVOKE_MORE paramIndex,__val_int);
  __XDBC_HOOK_CHECK(return false);
    
  paramval = (short)__val_int; 
  return __ret_code;
}

bool OdbcCallableStatement::getInt(__XDBC_HOOK_DECLARE_MORE int paramIndex,int& paramval)  __XDBC_THROW_DECLARE(SQLException)
{
  XLONG __val_int = 0;
  bool __ret_code = getLong(__XDBC_HOOK_INVOKE_MORE paramIndex,__val_int);
  __XDBC_HOOK_CHECK(return false);
  
  paramval = (int)__val_int; 
  return __ret_code;
}


bool OdbcCallableStatement::getLong(__XDBC_HOOK_DECLARE_MORE int paramIndex,XLONG& paramval)  __XDBC_THROW_DECLARE(SQLException)
{
  __ODBC_CALL_CHECKNULL_BOOLEAN(paramIndex);
  
  if(__odbc_param->__mi_bind_type == SQL_BIGINT)
  {
    paramval = (XLONG)__odbc_param->__mu_value.odbc_BIGINT;
    return true;
  }
  
  SQLBIGINT __col_value = 0;
  if(checkConversion(__XDBC_HOOK_INVOKE_MORE paramIndex,__odbc_param,SQL_BIGINT,(char*)&__col_value))
  {
    paramval = (XLONG)__col_value;
    return true;
  }

  return false;
}

bool OdbcCallableStatement::getFloat(__XDBC_HOOK_DECLARE_MORE int paramIndex,float& paramval)  __XDBC_THROW_DECLARE(SQLException)
{
  double __val_double = 0;
  bool __ret_code = getDouble(__XDBC_HOOK_INVOKE_MORE paramIndex,__val_double);
  __XDBC_HOOK_CHECK(return __val_double);
  
  paramval = __val_double; 
  return __ret_code;
}

bool OdbcCallableStatement::getDouble(__XDBC_HOOK_DECLARE_MORE int paramIndex,double& paramval)  __XDBC_THROW_DECLARE(SQLException)
{
  __ODBC_CALL_CHECKNULL_BOOLEAN(paramIndex);
  
  if(__odbc_param->__mi_bind_type == SQL_DOUBLE)
  {
    paramval = (XLONG)__odbc_param->__mu_value.odbc_DOUBLE;
    return true;
  }
  
  SQLBIGINT __col_value = 0;
  if(checkConversion(__XDBC_HOOK_INVOKE_MORE paramIndex,__odbc_param,SQL_BIGINT,(char*)&__col_value))
  {
    paramval = (XLONG)__col_value;
    return true;
  }

  return false;
}

bool OdbcCallableStatement::getDate(__XDBC_HOOK_DECLARE_MORE int paramIndex,XDBC_DATE& paramval)  __XDBC_THROW_DECLARE(SQLException)
{
  __ODBC_CALL_CHECKNULL_BOOLEAN(paramIndex);
  
  if(__odbc_param->__mi_bind_type == SQL_TYPE_DATE)
  {
    DATE_STRUCT& __tt_dt = __odbc_param->__mu_value.odbc_DATE; 
    paramval.year =  __tt_dt.year;
    paramval.month = __tt_dt.month;
    paramval.day = __tt_dt.day;
  }
  
  return checkConversion(__XDBC_HOOK_INVOKE_MORE paramIndex,__odbc_param,SQL_TYPE_DATE,(char*)&paramval);
}

bool OdbcCallableStatement::getTime(__XDBC_HOOK_DECLARE_MORE int paramIndex,XDBC_TIME& paramval)  __XDBC_THROW_DECLARE(SQLException)
{
  __ODBC_CALL_CHECKNULL_BOOLEAN(paramIndex);
  
  if(__odbc_param->__mi_bind_type == SQL_TYPE_TIME)
  {
    TIME_STRUCT& __tt_dt = __odbc_param->__mu_value.odbc_TIME;
    paramval.hour =  __tt_dt.hour;
    paramval.minute = __tt_dt.minute;
    paramval.second = __tt_dt.second;
  }
  
  return checkConversion(__XDBC_HOOK_INVOKE_MORE paramIndex,__odbc_param,SQL_TYPE_TIME,(char*)&paramval);
}

bool OdbcCallableStatement::getTimestamp(__XDBC_HOOK_DECLARE_MORE int paramIndex,XDBC_TIMESTAMP& paramval)  __XDBC_THROW_DECLARE(SQLException)
{
  __ODBC_CALL_CHECKNULL_BOOLEAN(paramIndex);
  
  if(__odbc_param->__mi_bind_type == SQL_TYPE_TIMESTAMP)
  {
    TIMESTAMP_STRUCT& __tt_dt = __odbc_param->__mu_value.odbc_TIMESTAMP;
    OdbcConverter::setXdbcTimestamp(paramval,__tt_dt,__odbc_param);
  }
  
  return checkConversion(__XDBC_HOOK_INVOKE_MORE paramIndex,__odbc_param,SQL_TYPE_TIMESTAMP,(char*)&paramval);
}

bool OdbcCallableStatement::getInterval(__XDBC_HOOK_DECLARE_MORE int paramIndex,XDBC_INTERVAL& paramval)  __XDBC_THROW_DECLARE(SQLException)
{
  __XDBC_HOOK_RETURN(return false,"getInterval not supported");
}

const char* OdbcCallableStatement::getString(__XDBC_HOOK_DECLARE_MORE int paramIndex,int& sizeBytes)  __XDBC_THROW_DECLARE(SQLException)
{
  sizeBytes = 0;
  const char* start_charP = 0; 
  __ODBC_CALL_CHECKNULL_POINTER(paramIndex);
  
  switch(__odbc_param->__mi_type)
  {
    case SQL_CHAR:
    case SQL_VARCHAR:
    case SQL_LONGVARCHAR:
    {
      start_charP = __p_parambuf + __odbc_param->__mi_buf_offset;
      __ODBC_GETBINDABLE_IND(__odbc_param,sizeBytes);
     
     // printf("(int : %d) - (len : %d) - (size : %d)\n",__odbc_param->__mi_bind_ind_int,__odbc_param->__mi_bind_ind_len,__odbc_param->__mi_bind_size);  
      return start_charP;     
    }
    break;
//-----------------------------
    case SQL_BINARY:
    case SQL_VARBINARY:
    case SQL_LONGVARBINARY:
    {
      start_charP = __p_parambuf + __odbc_param->__mi_buf_offset;
      __ODBC_GETBINDABLE_IND(__odbc_param,sizeBytes);
  
      return start_charP; 
    }
    break;  
//-----------------------------
    default:
    {
    }
    break;  
  } 
  
  string& __tmp_str = __odbc_param->__tmp_std_str;  
  if(checkConversion(__XDBC_HOOK_INVOKE_MORE paramIndex,__odbc_param,SQL_VARCHAR,(char*)&__tmp_str))
  {
    sizeBytes = (int)__tmp_str.size();
    return __tmp_str.data();
  }

  return 0; 
}

Blob* OdbcCallableStatement::getBlob(__XDBC_HOOK_DECLARE_MORE int paramIndex)  __XDBC_THROW_DECLARE(SQLException)
{
  __ODBC_CALL_CHECKNULL_POINTER(paramIndex);
 
  char* start = __p_parambuf+__odbc_param->__mi_buf_offset;

  int count = 0;
  __ODBC_GETBINDABLE_IND(__odbc_param,count);
  
  Blob* __p_blob = new OdbcBlob();
  __p_blob->write(__XDBC_HOOK_INVOKE_MORE 1, start, 0, count);  

  return __p_blob;
}

Clob* OdbcCallableStatement::getClob(__XDBC_HOOK_DECLARE_MORE int paramIndex)  __XDBC_THROW_DECLARE(SQLException)
{
  __ODBC_CALL_CHECKNULL_POINTER(paramIndex);
 
  char* start = __p_parambuf+__odbc_param->__mi_buf_offset;
  int count = 0;
  if(__mb_fintAvailable)
  {
    count  = (int)__odbc_param->__mi_bind_ind_int;    
  }
  else
  {
    count  = (int)__odbc_param->__mi_bind_ind_len;      
  }
  
  Clob* __p_clob = new OdbcClob();
  __p_clob->write(__XDBC_HOOK_INVOKE_MORE 1, start, 0, count);  

  return __p_clob;
}

///////=================================================================/////
ResultSet*  OdbcCallableStatement::executeQuery(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
     StringBuffer sb;
     sb.append("executeQuery|");
     sb.append(__ms_odbc_sql);
     
     __XDBC_FORCED_TRACE(ODBCCALLABLESTATEMENT_CLASS_NAME,sb.str()); 
  }
  
  __XDBC_HOOK_RETURN(return 0,"executeQuery not supported");
}

int OdbcCallableStatement::executeUpdate(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
     StringBuffer sb;
     sb.append("executeUpdate|");
     sb.append(__ms_odbc_sql);
     
     __XDBC_FORCED_TRACE(ODBCCALLABLESTATEMENT_CLASS_NAME,sb.str()); 
  }
 
  SQLRETURN __odbc_sqlreturn=(__api_SQLEXECUTE)(odbc_hstmt); 
  if(__ODBC_HOOK_CHECK_FAILED(__odbc_sqlreturn))
  {
    OdbcKit::checkException(__XDBC_HOOK_INVOKE_MORE  __mp_conn, odbc_hstmt,__odbc_sqlreturn,ODBCCALLABLESTATEMENT_CLASS_NAME);
    __XDBC_HOOK_CHECK(return 0);
  }

  SQLINTEGER rowsFetched=0;
  __odbc_sqlreturn = __odbc_SQLRowCount(__mp_conn,odbc_hstmt,&rowsFetched);

  if(__ODBC_HOOK_CHECK_FAILED(__odbc_sqlreturn))
  {
    OdbcKit::checkException(__XDBC_HOOK_INVOKE_MORE  __mp_conn, odbc_hstmt,__odbc_sqlreturn,ODBCCALLABLESTATEMENT_CLASS_NAME);
  }

  return rowsFetched;
}

void OdbcCallableStatement::setByte(__XDBC_HOOK_DECLARE_MORE  int paramIndex,const char& x,bool is_null) __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    __sb << "setByte|index=" << paramIndex << ",value=" << x << ",null=" << is_null;
    __XDBC_FORCED_TRACE(ODBCCALLABLESTATEMENT_CLASS_NAME,__sb.str()); 
  }

  __ODBC_CALL_CHECKPARAM(paramIndex); 
  ODBC_PARAM* __odbc_param = __vector_params[paramIndex];
  __odbc_param->__mu_value.odbc_TINYINT = x;
  __ODBC_SETBINDABLE_IND(__odbc_param,sizeof(SQLCHAR));
  
  __ODBC_CALL_CHECKBOUND_BEGIN_2(paramIndex);

  SQLRETURN __odbc_sqlreturn = SQL_SUCCESS;
  __ODBC_SQLBINDPARAMETER(SQL_PARAM_INPUT,SQL_C_DEFAULT,SQL_TINYINT,0, 0, &(__odbc_param->__mu_value.odbc_TINYINT),0);

  if(__ODBC_HOOK_CHECK_FAILED(__odbc_sqlreturn))
  {
    OdbcKit::checkException(__XDBC_HOOK_INVOKE_MORE  __mp_conn, odbc_hstmt,__odbc_sqlreturn,ODBCCALLABLESTATEMENT_CLASS_NAME);
  }
  
  __ODBC_CALL_CHECKBOUND_END_2(paramIndex);
  
}

void OdbcCallableStatement::setShort(__XDBC_HOOK_DECLARE_MORE  int paramIndex,const short& x,bool is_null) __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    __sb << "setShort|index=" << paramIndex << ",value=" << x << ",null=" << is_null;
    
    __XDBC_FORCED_TRACE(ODBCCALLABLESTATEMENT_CLASS_NAME,__sb.str()); 
  }

  __ODBC_CALL_CHECKPARAM(paramIndex); 
  ODBC_PARAM* __odbc_param = __vector_params[paramIndex];
  __odbc_param->__mu_value.odbc_SMALLINT = x;
  __ODBC_SETBINDABLE_IND(__odbc_param,sizeof(SQLSMALLINT));
    
  __ODBC_CALL_CHECKBOUND_BEGIN_2(paramIndex);

  SQLRETURN __odbc_sqlreturn = SQL_SUCCESS;
  __ODBC_SQLBINDPARAMETER(SQL_PARAM_INPUT,SQL_C_DEFAULT,SQL_SMALLINT,0, 0, &(__odbc_param->__mu_value.odbc_SMALLINT),0);
  
  if(__ODBC_HOOK_CHECK_FAILED(__odbc_sqlreturn))
  {
    OdbcKit::checkException(__XDBC_HOOK_INVOKE_MORE  __mp_conn, odbc_hstmt,__odbc_sqlreturn,ODBCCALLABLESTATEMENT_CLASS_NAME);
  }
   
  __ODBC_CALL_CHECKBOUND_END_2(paramIndex);
}

void OdbcCallableStatement::setInt(__XDBC_HOOK_DECLARE_MORE  int paramIndex,const int& x,bool is_null) __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    __sb << "setInt|index=" << paramIndex << ",value=" << x << ",null=" << is_null;
    
    __XDBC_FORCED_TRACE(ODBCCALLABLESTATEMENT_CLASS_NAME,__sb.str()); 
  }

  __ODBC_CALL_CHECKPARAM(paramIndex);
  ODBC_PARAM* __odbc_param = __vector_params[paramIndex];
  __odbc_param->__mu_value.odbc_INTEGER = x;
  __ODBC_SETBINDABLE_IND(__odbc_param,sizeof(SQLINTEGER));
     
  __ODBC_CALL_CHECKBOUND_BEGIN_2(paramIndex) ;
  
  SQLRETURN __odbc_sqlreturn = SQL_SUCCESS;
  __ODBC_SQLBINDPARAMETER(SQL_PARAM_INPUT,SQL_C_LONG,SQL_INTEGER,0, 0, &(__odbc_param->__mu_value.odbc_INTEGER),0);
  
  if(__ODBC_HOOK_CHECK_FAILED(__odbc_sqlreturn))
  {
    OdbcKit::checkException(__XDBC_HOOK_INVOKE_MORE  __mp_conn, odbc_hstmt,__odbc_sqlreturn,ODBCCALLABLESTATEMENT_CLASS_NAME);
  }
  
  __ODBC_CALL_CHECKBOUND_END_2(paramIndex);
}

void OdbcCallableStatement::setLong(__XDBC_HOOK_DECLARE_MORE  int paramIndex,const XLONG& x,bool is_null) __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    __sb << "setLong|index=" << paramIndex << ",value=" << x << ",null=" << is_null;
    
    __XDBC_FORCED_TRACE(ODBCCALLABLESTATEMENT_CLASS_NAME,__sb.str()); 
  }

  __ODBC_CALL_CHECKPARAM(paramIndex);
  ODBC_PARAM* __odbc_param = __vector_params[paramIndex];
  __odbc_param->__mu_value.odbc_BIGINT = x;
  __ODBC_SETBINDABLE_IND(__odbc_param,sizeof(SQLBIGINT));
    
  __ODBC_CALL_CHECKBOUND_BEGIN_2(paramIndex);

  SQLRETURN __odbc_sqlreturn = SQL_SUCCESS;
  __ODBC_SQLBINDPARAMETER(SQL_PARAM_INPUT,SQL_C_BIGINT,SQL_BIGINT,0, 0, &(__odbc_param->__mu_value.odbc_BIGINT),0);
  
  if(__ODBC_HOOK_CHECK_FAILED(__odbc_sqlreturn))
  {
    OdbcKit::checkException(__XDBC_HOOK_INVOKE_MORE  __mp_conn, odbc_hstmt,__odbc_sqlreturn,ODBCCALLABLESTATEMENT_CLASS_NAME);
  }
  
  __ODBC_CALL_CHECKBOUND_END_2(paramIndex);
}

void OdbcCallableStatement::setFloat(__XDBC_HOOK_DECLARE_MORE  int paramIndex,const float& x,bool is_null) __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    __sb << "setFloat|index=" << paramIndex << ",value=" << x << ",null=" << is_null;
    __XDBC_FORCED_TRACE(ODBCCALLABLESTATEMENT_CLASS_NAME,__sb.str()); 
  }

  __ODBC_CALL_CHECKPARAM(paramIndex);
  ODBC_PARAM* __odbc_param = __vector_params[paramIndex]; 
  __odbc_param->__mu_value.odbc_REAL = x;
  __ODBC_SETBINDABLE_IND(__odbc_param,sizeof(SQLREAL));
      
  __ODBC_CALL_CHECKBOUND_BEGIN_2(paramIndex);

  SQLRETURN __odbc_sqlreturn = SQL_SUCCESS;
  __ODBC_SQLBINDPARAMETER(SQL_PARAM_INPUT,SQL_C_DEFAULT,SQL_FLOAT,0, 0, &(__odbc_param->__mu_value.odbc_REAL),0);
  
  if(__ODBC_HOOK_CHECK_FAILED(__odbc_sqlreturn))
  {
    OdbcKit::checkException(__XDBC_HOOK_INVOKE_MORE  __mp_conn, odbc_hstmt,__odbc_sqlreturn,ODBCCALLABLESTATEMENT_CLASS_NAME);
  }
    
  __ODBC_CALL_CHECKBOUND_END_2(paramIndex);
}

void OdbcCallableStatement::setDouble(__XDBC_HOOK_DECLARE_MORE  int paramIndex,const double& x,bool is_null) __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    __sb << "setDouble|index=" << paramIndex << ",value=" << x << ",null=" << is_null;
    
    __XDBC_FORCED_TRACE(ODBCCALLABLESTATEMENT_CLASS_NAME,__sb.str()); 
  }


  __ODBC_CALL_CHECKPARAM(paramIndex);
  ODBC_PARAM* __odbc_param = __vector_params[paramIndex];
  __odbc_param->__mu_value.odbc_DOUBLE = x;
  __ODBC_SETBINDABLE_IND(__odbc_param,sizeof(SQLDOUBLE));
    
  __ODBC_CALL_CHECKBOUND_BEGIN_2(paramIndex);

  SQLRETURN __odbc_sqlreturn = SQL_SUCCESS;
  __ODBC_SQLBINDPARAMETER(SQL_PARAM_INPUT,SQL_C_DEFAULT,SQL_DOUBLE,0, 0, &(__odbc_param->__mu_value.odbc_DOUBLE),0);
  
  if(__ODBC_HOOK_CHECK_FAILED(__odbc_sqlreturn))
  {
    OdbcKit::checkException(__XDBC_HOOK_INVOKE_MORE  __mp_conn, odbc_hstmt,__odbc_sqlreturn,ODBCCALLABLESTATEMENT_CLASS_NAME);
  }
    
  __ODBC_CALL_CHECKBOUND_END_2(paramIndex);
}

void OdbcCallableStatement::setDate(__XDBC_HOOK_DECLARE_MORE  int paramIndex,const XDBC_DATE* x,bool is_null)__XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    if(x != 0)
    {
      __sb << "setDate|index=" << paramIndex << ",value=" << XDBCUtilities::toString(*x) << ",null=" << is_null;
    }
    else
    {
      __sb << "setDate|index=" << paramIndex << ",value=<NULL>,null=" << is_null;
    }
    
    __XDBC_FORCED_TRACE(ODBCCALLABLESTATEMENT_CLASS_NAME,__sb.str()); 
  }

  __ODBC_CALL_CHECKPARAM(paramIndex);
  ODBC_PARAM* __odbc_param = __vector_params[paramIndex];  
  
  if(x != 0) 
  {
    DATE_STRUCT& odbc_DATE =  __odbc_param->__mu_value.odbc_DATE;
    odbc_DATE.year = x->year;
    odbc_DATE.month = x->month;
    odbc_DATE.day = x->day;
  }

  __ODBC_SETBINDABLE_IND(__odbc_param,sizeof(DATE_STRUCT));
    
  __ODBC_CALL_CHECKBOUND_BEGIN_2(paramIndex);
  
  SQLRETURN __odbc_sqlreturn = SQL_SUCCESS;
  __ODBC_SQLBINDPARAMETER(SQL_PARAM_INPUT,SQL_C_TYPE_DATE,SQL_TYPE_DATE,0, 0, &(__odbc_param->__mu_value.odbc_DATE),0);
  
  if(__ODBC_HOOK_CHECK_FAILED(__odbc_sqlreturn))
  {
    OdbcKit::checkException(__XDBC_HOOK_INVOKE_MORE  __mp_conn, odbc_hstmt,__odbc_sqlreturn,ODBCCALLABLESTATEMENT_CLASS_NAME);
  }
    
  __ODBC_CALL_CHECKBOUND_END_2(paramIndex);
}

void OdbcCallableStatement::setTime(__XDBC_HOOK_DECLARE_MORE  int paramIndex,const XDBC_TIME* x,bool is_null) __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    if(x != 0)
    {
      __sb << "setTime|index=" << paramIndex << ",value=" << XDBCUtilities::toString(*x) << ",null=" << is_null;
    }
    else
    {
      __sb << "setTime|index=" << paramIndex << ",value=<NULL>,null=" << is_null;
    }
    
    __XDBC_FORCED_TRACE(ODBCCALLABLESTATEMENT_CLASS_NAME,__sb.str()); 
  }


  __ODBC_CALL_CHECKPARAM(paramIndex);
  ODBC_PARAM* __odbc_param = __vector_params[paramIndex];

  if(x != 0)
  { 
    TIME_STRUCT& odbc_TIME =  __odbc_param->__mu_value.odbc_TIME;
    
    odbc_TIME.hour = x->hour;
    odbc_TIME.minute = x->minute;
    odbc_TIME.second = x->second;
  }

  __ODBC_SETBINDABLE_IND(__odbc_param,sizeof(TIME_STRUCT));
  
  __ODBC_CALL_CHECKBOUND_BEGIN_2(paramIndex);
  
  SQLRETURN __odbc_sqlreturn = SQL_SUCCESS;
  __ODBC_SQLBINDPARAMETER(SQL_PARAM_INPUT,SQL_C_TYPE_TIME,SQL_TYPE_TIME,0, 0, &(__odbc_param->__mu_value.odbc_TIME),0);
      
  if(__ODBC_HOOK_CHECK_FAILED(__odbc_sqlreturn))
  {
    OdbcKit::checkException(__XDBC_HOOK_INVOKE_MORE  __mp_conn, odbc_hstmt,__odbc_sqlreturn,ODBCCALLABLESTATEMENT_CLASS_NAME);
  }
   
  __ODBC_CALL_CHECKBOUND_END_2(paramIndex);  
}

void OdbcCallableStatement::setTimestamp(__XDBC_HOOK_DECLARE_MORE  int paramIndex,const XDBC_TIMESTAMP* x,bool is_null) __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    if(x != 0)
    {
      __sb << "setTimestamp|index=" << paramIndex << ",value=" << XDBCUtilities::toString(*x) << ",null=" << is_null;
    }
    else
    {
      __sb << "setTimestamp|index=" << paramIndex << ",value=<NULL>,null=" << is_null;
    }
    
    __XDBC_FORCED_TRACE(ODBCCALLABLESTATEMENT_CLASS_NAME,__sb.str());     
  }

  __ODBC_CALL_CHECKPARAM(paramIndex);
  ODBC_PARAM* __odbc_param = __vector_params[paramIndex];
  
  if(x != 0) 
  {
    TIMESTAMP_STRUCT& odbc_TIMESTAMP =  __odbc_param->__mu_value.odbc_TIMESTAMP; 
    OdbcConverter::setOdbcTimestamp(odbc_TIMESTAMP,*x,__odbc_param); 
  }//~end if(x != 0) 
  __ODBC_SETBINDABLE_IND(__odbc_param,sizeof(TIMESTAMP_STRUCT));
  
  __ODBC_CALL_CHECKBOUND_BEGIN_2(paramIndex);

  SQLRETURN __odbc_sqlreturn = SQL_SUCCESS;
  __ODBC_SQLBINDPARAMETER(SQL_PARAM_INPUT,SQL_C_TYPE_TIMESTAMP,SQL_TYPE_TIMESTAMP,0, 0, &(__odbc_param->__mu_value.odbc_TIMESTAMP),0);
      
  if(__ODBC_HOOK_CHECK_FAILED(__odbc_sqlreturn))
  {
    OdbcKit::checkException(__XDBC_HOOK_INVOKE_MORE  __mp_conn, odbc_hstmt,__odbc_sqlreturn,ODBCCALLABLESTATEMENT_CLASS_NAME);
  }
  
  __ODBC_CALL_CHECKBOUND_END_2(paramIndex);
}

void OdbcCallableStatement::setInterval(__XDBC_HOOK_DECLARE_MORE  int paramIndex,  const XDBC_INTERVAL* x,bool is_null) __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    if(x != 0)
    {
      __sb << "setInterval|index=" << paramIndex << ",value=" << XDBCUtilities::toString(*x) << ",null=" << is_null;
    }
    else
    {
      __sb << "setInterval|index=" << paramIndex << ",value=<NULL>,null=" << is_null;
    }
    
    __XDBC_FORCED_TRACE(ODBCCALLABLESTATEMENT_CLASS_NAME,__sb.str());       
  }
  
  __XDBC_HOOK_RETURN(return,"setInterval() not supported");
}

void OdbcCallableStatement::setString(__XDBC_HOOK_DECLARE_MORE  int paramIndex,const string* x,bool is_null) __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    if(x != 0)
    {
      __sb << "setString|index=" << paramIndex << ",value=" << (*x) << ",null=" << is_null;
    }
    else
    {
      __sb << "setString|index=" << paramIndex << ",value=<NULL>,null=" << is_null;
    }
    
    __XDBC_FORCED_TRACE(ODBCCALLABLESTATEMENT_CLASS_NAME,__sb.str()); 
  }

  __ODBC_CALL_CHECKPARAM(paramIndex);
  ODBC_PARAM* __odbc_param = __vector_params[paramIndex];
  char* buf_start = __p_parambuf + __odbc_param->__mi_buf_offset;
  int   buf_size  = __odbc_param->__mi_bind_size;     
  if(x != 0)
  {
    __ODBC_DATA_MEMCPY(x->data(),(int)x->size(),buf_start,buf_size);
  }
  
  __ODBC_CALL_CHECKBOUND_BEGIN_2(paramIndex);

  SQLRETURN __odbc_sqlreturn = SQL_SUCCESS;
  __ODBC_SQLBINDPARAMETER(SQL_PARAM_INPUT,__odbc_param->__mi_bind_c_type, __odbc_param->__mi_bind_type,0, 0, buf_start,buf_size);
    
  if(__ODBC_HOOK_CHECK_FAILED(__odbc_sqlreturn))
  {
    OdbcKit::checkException(__XDBC_HOOK_INVOKE_MORE  __mp_conn, odbc_hstmt,__odbc_sqlreturn,ODBCCALLABLESTATEMENT_CLASS_NAME);
  }
  
  __ODBC_CALL_CHECKBOUND_END_2(paramIndex);
}

void OdbcCallableStatement::setString(__XDBC_HOOK_DECLARE_MORE  int paramIndex,  const char* x,int count,bool is_null) __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    if(x != 0)
    {
      __sb << "setString|index=" << paramIndex << ",value=" << x << ",count=" << count << ",null=" << is_null;
    }
    else
    {
      __sb << "setString|index=" << paramIndex << ",value=<NULL>,count=" << count << ",null=" << is_null;
    }
    
    __XDBC_FORCED_TRACE(ODBCCALLABLESTATEMENT_CLASS_NAME,__sb.str()); 
  }


  __ODBC_CALL_CHECKPARAM(paramIndex);
  ODBC_PARAM* __odbc_param = __vector_params[paramIndex];
  char* buf_start = __p_parambuf + __odbc_param->__mi_buf_offset;
  int   buf_size  = __odbc_param->__mi_bind_size;     
  if(x != 0)
  {
    __ODBC_DATA_MEMCPY(x,count,buf_start,buf_size);
  }
  
  __ODBC_CALL_CHECKBOUND_BEGIN_2(paramIndex);

  SQLRETURN __odbc_sqlreturn = SQL_SUCCESS;
  __ODBC_SQLBINDPARAMETER(SQL_PARAM_INPUT,__odbc_param->__mi_bind_c_type, __odbc_param->__mi_bind_type,0, 0, buf_start,buf_size);
  
  if(__ODBC_HOOK_CHECK_FAILED(__odbc_sqlreturn))
  {
    OdbcKit::checkException(__XDBC_HOOK_INVOKE_MORE  __mp_conn, odbc_hstmt,__odbc_sqlreturn,ODBCCALLABLESTATEMENT_CLASS_NAME);
  }
  
  __ODBC_CALL_CHECKBOUND_END_2(paramIndex);  
}

void OdbcCallableStatement::setBlob (__XDBC_HOOK_DECLARE_MORE  int paramIndex, const char* x,int count,bool is_null) __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    if(x != 0)
    {
      __sb << "setBlob|index=" << paramIndex << ",value=<BLOB>[" << count << "],null=" << is_null;
    }
    else
    {
      __sb << "setBlob|index=" << paramIndex << ",value=<NULL>,null=" << is_null;
    }

    __XDBC_FORCED_TRACE(ODBCCALLABLESTATEMENT_CLASS_NAME,__sb.str());
  }

  __ODBC_CALL_CHECKPARAM(paramIndex);
  ODBC_PARAM* __odbc_param = __vector_params[paramIndex];
  
  char* buf_start = __p_parambuf + __odbc_param->__mi_buf_offset;
  int   buf_size  = __odbc_param->__mi_bind_size;     
  if(x != 0)
  { 
    __ODBC_DATA_MEMCPY(x,count,buf_start,buf_size);
  } 
      
  __ODBC_CALL_CHECKBOUND_BEGIN_2(paramIndex);

  SQLRETURN __odbc_sqlreturn = SQL_SUCCESS;
  __ODBC_SQLBINDPARAMETER(SQL_PARAM_INPUT,SQL_C_BINARY, SQL_VARBINARY,0, 0, buf_start,buf_size);
  
  if(__ODBC_HOOK_CHECK_FAILED(__odbc_sqlreturn))
  {
    OdbcKit::checkException(__XDBC_HOOK_INVOKE_MORE  __mp_conn, odbc_hstmt,__odbc_sqlreturn,ODBCCALLABLESTATEMENT_CLASS_NAME);
  }
    
  __ODBC_CALL_CHECKBOUND_END_2(paramIndex);
}

void OdbcCallableStatement::setBlob (__XDBC_HOOK_DECLARE_MORE  int paramIndex,const Blob* x,bool is_null) __XDBC_THROW_DECLARE(SQLException)
{
  string __blobData;
  if(x != 0)
  {
    OdbcBlob* __p_blob = (OdbcBlob *)x;
    int __length = __p_blob->length(__XDBC_HOOK_INVOKE_ONCE);  
    __XDBC_HOOK_CHECK(return);
        
    __blobData.resize(__length);
    __p_blob->read(__XDBC_HOOK_INVOKE_MORE 1, (char*)__blobData.data(),0,__length); 
    __XDBC_HOOK_CHECK(return);    
  }
  
  setBlob (__XDBC_HOOK_INVOKE_MORE paramIndex, __blobData.data(),(int) __blobData.size(), is_null);
}

void OdbcCallableStatement::setClob (__XDBC_HOOK_DECLARE_MORE  int paramIndex, const char* x,int count,bool is_null) __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    if(x != 0)
    {
      __sb << "setClob|index=" << paramIndex << ",value=<CLOB>[" << count << "],null=" << is_null;
    }
    else
    {
      __sb << "setClob|index=" << paramIndex << ",value=<NULL>,null=" << is_null;
    }

    __XDBC_FORCED_TRACE(ODBCCALLABLESTATEMENT_CLASS_NAME,__sb.str());
  }


  __ODBC_CALL_CHECKPARAM(paramIndex);
  ODBC_PARAM* __odbc_param = __vector_params[paramIndex];
  char* buf_start = __p_parambuf + __odbc_param->__mi_buf_offset;
  int   buf_size  = __odbc_param->__mi_bind_size;     
  if(x != 0)
  { 
    __ODBC_DATA_MEMCPY(x,count,buf_start,buf_size);
  } 
      
  __ODBC_CALL_CHECKBOUND_BEGIN_2(paramIndex);

  SQLRETURN __odbc_sqlreturn = SQL_SUCCESS;
  __ODBC_SQLBINDPARAMETER(SQL_PARAM_INPUT,SQL_C_CHAR, SQL_VARCHAR,0, 0, buf_start,buf_size);  

  if(__ODBC_HOOK_CHECK_FAILED(__odbc_sqlreturn))
  {
    OdbcKit::checkException(__XDBC_HOOK_INVOKE_MORE  __mp_conn, odbc_hstmt,__odbc_sqlreturn,ODBCCALLABLESTATEMENT_CLASS_NAME);
  }
  
  __ODBC_CALL_CHECKBOUND_END_2(paramIndex);
}

void OdbcCallableStatement::setClob (__XDBC_HOOK_DECLARE_MORE  int paramIndex,const Clob* x,bool is_null) __XDBC_THROW_DECLARE(SQLException)
{
  string __clobData;
  if(x != 0)
  {
    OdbcClob* __p_clob = (OdbcClob *)x;
    int __length = __p_clob->length(__XDBC_HOOK_INVOKE_ONCE);  
    __XDBC_HOOK_CHECK(return);
    
    __clobData.resize(__length);
    __p_clob->read(__XDBC_HOOK_INVOKE_MORE 1, (char*)__clobData.data(),0,__length); 
    __XDBC_HOOK_CHECK(return);
  }
  
  setClob(__XDBC_HOOK_INVOKE_MORE paramIndex, __clobData.data(), (int)__clobData.size(), is_null);
}

///////=================================================================/////

void  OdbcCallableStatement::close()
{
  __XDBC_TRACE(ODBCCALLABLESTATEMENT_CLASS_NAME,"close()");

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

  for(int i = 1; i <= __mi_param_count;++i)
  {
    delete __vector_params[i];
    __vector_params[i] = 0;
  }
}

void  OdbcCallableStatement::destroy()
{
  if(__p_parambuf != 0)
  {
    delete []__p_parambuf;
  }  
  
  delete this;
}
  
int OdbcCallableStatement::getResultSetType(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{
  return __mi_resultSetType;
}

Connection* OdbcCallableStatement::getConnection(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{
  return (Connection*)__mp_conn;
}


void  OdbcCallableStatement::parseProcName(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{
  size_t  __call_begin = __ms_odbc_sql.find("call");
  if(__call_begin == string::npos)
  {
    __XDBC_HOOK_RETURN(return,"procedure not found");
  }

  size_t __call_end = __ms_odbc_sql.find("(",__call_begin);
  if(__call_end == string::npos)
  {
    __XDBC_HOOK_RETURN(return,"procedure not found");
  }

  size_t __proc_start = __call_begin + 4 /* strlen("call") */;
  while(__ms_odbc_sql.at(__proc_start) == ' ')
  {
    __proc_start++;
  }

  while(__ms_odbc_sql.at(__call_end) == ' ')
  {
    __call_end--;
  }

  string __proc_name = StringsKit::toUpperCase(__ms_odbc_sql.substr(__proc_start,__call_end - __proc_start));
  
  __ms_proc_name = __proc_name;
  __ms_schema_name = "%";
   
  /* separate the schema name if specified */
  size_t separator = 0;
  const char *temp_str = 0;
  const char* __P_proc_name = __proc_name.c_str();  
 
  if ((temp_str = strstr(__P_proc_name, ".")) != 0)
  {
    separator = strlen(__P_proc_name) - strlen(temp_str);
    __ms_schema_name.assign(__P_proc_name,separator);
    __ms_proc_name = temp_str + 1;
  }

  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    __sb << "parseProcName|" << __proc_name;
    __XDBC_FORCED_TRACE(ODBCCALLABLESTATEMENT_CLASS_NAME,__sb.str());
  }

}

void OdbcCallableStatement::parseProcVariables(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{
  SQLHSTMT   odbc_hstmt_for_proc;
  SQLRETURN __odbc_sqlreturn = SQL_SUCCESS;

  __odbc_sqlreturn = __odbc_SQLAllocHandle(__mp_conn,SQL_HANDLE_STMT,__mp_conn->odbc_hdbc, &odbc_hstmt_for_proc); 
  if(__ODBC_HOOK_CHECK_FAILED(__odbc_sqlreturn))
  {
    OdbcKit::checkException(__XDBC_HOOK_INVOKE_MORE  __mp_conn, odbc_hstmt_for_proc,__odbc_sqlreturn,ODBCCALLABLESTATEMENT_CLASS_NAME);
    __XDBC_HOOK_CHECK(return);
  } 
  
  SQLINTEGER __option = SQL_ATTR_METADATA_ID;
  SQLINTEGER __param = SQL_FALSE;
  if((__options_SUPPORTSSTMTATTR)(__option, (SQLPOINTER )__param,(SQLINTEGER)SQL_IS_INTEGER) == SQL_SUCCESS)
  {
    __odbc_sqlreturn = __odbc_SQLSetStmtAttr(__mp_conn,odbc_hstmt,__option, (SQLPOINTER )__param,(SQLINTEGER)SQL_IS_INTEGER,(SQLPOINTER )&__param,(SQLINTEGER)SQL_IS_POINTER);
    if(__ODBC_HOOK_CHECK_FAILED(__odbc_sqlreturn))
    {
      OdbcKit::checkException(__XDBC_HOOK_INVOKE_MORE  __mp_conn, odbc_hstmt,__odbc_sqlreturn,ODBCCALLABLESTATEMENT_CLASS_NAME);
      __XDBC_HOOK_CHECK(return);
    }
  }
  
  if(__vector_params.empty()) 
  {
    __vector_params.push_back((ODBC_PARAM*)0); //dummy param . index starts from 1
  }
 
  __OPENCODE_TRY
  {  
    parseProcVariablesImpl(__XDBC_HOOK_INVOKE_MORE odbc_hstmt_for_proc);      
    __odbc_SQLFreeHandle(__mp_conn,SQL_HANDLE_STMT,odbc_hstmt_for_proc); 
  }
  __OPENCODE_CATCH(SQLException& ex)
  {
    __odbc_SQLFreeHandle(__mp_conn,SQL_HANDLE_STMT,odbc_hstmt_for_proc);
    __OPENCODE_RETHROW;    
  }   
}

void OdbcCallableStatement::parseProcVariablesImpl(__XDBC_HOOK_DECLARE_MORE SQLHSTMT odbc_hstmt_for_proc)  __XDBC_THROW_DECLARE(SQLException)
{
  SQLRETURN __odbc_sqlreturn = 0;    
  ODBCSQLAPI_SQLFETCH   __api_SQLFETCH   = __mp_conn->__odbc_apis->__api_SQLFETCH;
    
  /* get input/output parameter information for a procedure */
  __odbc_sqlreturn = __odbc_SQLProcedureColumns(__mp_conn,odbc_hstmt_for_proc,
                                           0,
                                           0, /* catalog name not used */
                                           (SQLCHAR *)__ms_schema_name.c_str(),
                                           SQL_NTS, /* schema name not currently used */
                                           (SQLCHAR *)__ms_proc_name.c_str(),
                                           SQL_NTS,
                                           (SQLCHAR*)"%",
                                           SQL_NTS); /* all params */

//  __odbc_sqlreturn = __odbc_SQLProcedureColumns(__mp_conn,odbc_hstmt_for_proc,
//                                           0,
//                                           0, /* catalog name not used */
//                                           0,
//                                           0, /* schema name not currently used */
//                                           (SQLCHAR *)__ms_proc_name.c_str(),
//                                           SQL_NTS,
//                                           0,
//                                           0); /* all params */
                                              
  
  if(__ODBC_HOOK_CHECK_FAILED(__odbc_sqlreturn))
  {
    OdbcKit::checkException(__XDBC_HOOK_INVOKE_MORE  __mp_conn, odbc_hstmt_for_proc,__odbc_sqlreturn,ODBCCALLABLESTATEMENT_CLASS_NAME);
    __XDBC_HOOK_CHECK(return);
  } 
  
  struct 
  {
  	SQLINTEGER   columnIndex;
  	SQLSMALLINT  columnType;
  	SQLPOINTER   columnPtr;
    SQLLEN       columnSize;
    SQLPOINTER   columnInd;
    bool         columnAvailable;
  }paramArrays[20];
  
  struct
  {
  	SQLINTEGER  ind_sqlint;
    SQLLEN      ind_sqllen;
    SQLCHAR     val[129];
  }catalogName,schemaName,procedureName, paramName, paramTypeName,paramIsNullable,paramRemarks,paramDef;
          
  struct
  {
  	SQLINTEGER  ind_sqlint;
    SQLLEN      ind_sqllen;
    SQLINTEGER  val;
  }paramPrecision, paramOrdinal,paramLength,paramCharOctetLength;

  struct
  {
  	SQLINTEGER  ind_sqlint;
    SQLLEN      ind_sqllen;
    SQLSMALLINT val;
  }paramScale, paramType, paramDataType,paramSQLDATATYPE,paramSQLDATETIMESUB,paramNUMRADIX,paramNULLABLE;
  
  for(int i = 0; i <20;i++)
  {
  	paramArrays[i].columnAvailable = false;
  }
 
   /* bind the result param for the catalog name */  
  paramArrays[1].columnIndex     = 1;
  paramArrays[1].columnType      = SQL_C_CHAR;
  paramArrays[1].columnPtr       = catalogName.val;
  paramArrays[1].columnSize      = 129;
  paramArrays[1].columnInd       = __mb_fintAvailable ? (SQLPOINTER)&catalogName.ind_sqlint : (SQLPOINTER)&catalogName.ind_sqllen;
  paramArrays[1].columnAvailable = true;
   
  /* bind the result param for the schema name */  
  paramArrays[2].columnIndex     = 2;
  paramArrays[2].columnType      = SQL_C_CHAR;
  paramArrays[2].columnPtr       = schemaName.val;
  paramArrays[2].columnSize      = 129;
  paramArrays[2].columnInd       = __mb_fintAvailable ? (SQLPOINTER)&schemaName.ind_sqlint : (SQLPOINTER)&schemaName.ind_sqllen;
  paramArrays[2].columnAvailable = true;

  /* bind the result param for the procedure name */  
  paramArrays[3].columnIndex     = 3;
  paramArrays[3].columnType      = SQL_C_CHAR;
  paramArrays[3].columnPtr       = procedureName.val;
  paramArrays[3].columnSize      = 129;
  paramArrays[3].columnInd       = __mb_fintAvailable ? (SQLPOINTER)&procedureName.ind_sqlint : (SQLPOINTER)&procedureName.ind_sqllen;
  paramArrays[3].columnAvailable = true;

  /* bind the result param for the param name */
  paramArrays[4].columnIndex     = 4;
  paramArrays[4].columnType      = SQL_C_CHAR;
  paramArrays[4].columnPtr       = paramName.val;
  paramArrays[4].columnSize      = 129;
  paramArrays[4].columnInd       = __mb_fintAvailable ? (SQLPOINTER)&paramName.ind_sqlint : (SQLPOINTER)&paramName.ind_sqllen;
  paramArrays[4].columnAvailable = true;
  
  /* bind the result param for param type (IN, OUT, INOUT) */
  paramArrays[5].columnIndex     = 5;
  paramArrays[5].columnType      = SQL_C_SHORT;
  paramArrays[5].columnPtr       = (SQLPOINTER)&paramType.val;
  paramArrays[5].columnSize      = (SQLLEN)sizeof(paramType.val);
  paramArrays[5].columnInd       = __mb_fintAvailable ? (SQLPOINTER)&paramType.ind_sqlint : (SQLPOINTER)&paramType.ind_sqllen;
  paramArrays[5].columnAvailable = true;
  
  /* bind the result param for data type (SQL data type) */
  paramArrays[6].columnIndex     = 6;
  paramArrays[6].columnType      = SQL_C_SHORT;
  paramArrays[6].columnPtr       = (SQLPOINTER)&paramDataType.val;
  paramArrays[6].columnSize      = (SQLLEN)sizeof(paramDataType.val);
  paramArrays[6].columnInd       = __mb_fintAvailable ? (SQLPOINTER)&paramDataType.ind_sqlint : (SQLPOINTER)&paramDataType.ind_sqllen;
  paramArrays[6].columnAvailable = true;
  
  /* bind the result param for type name */
  paramArrays[7].columnIndex     = 7;
  paramArrays[7].columnType      = SQL_C_CHAR;
  paramArrays[7].columnPtr       = (SQLPOINTER)paramTypeName.val;
  paramArrays[7].columnSize      = (SQLLEN)129;
  paramArrays[7].columnInd       = __mb_fintAvailable ? (SQLPOINTER)&paramTypeName.ind_sqlint : (SQLPOINTER)&paramTypeName.ind_sqllen;
  paramArrays[7].columnAvailable = true;
                                       
  /* bind the result param for param size */
  paramArrays[8].columnIndex     = 8;
  paramArrays[8].columnType      = SQL_C_LONG;
  paramArrays[8].columnPtr       = (SQLPOINTER)&paramPrecision.val;
  paramArrays[8].columnSize      = (SQLLEN)sizeof(paramPrecision.val);
  paramArrays[8].columnInd       = __mb_fintAvailable ? (SQLPOINTER)&paramPrecision.ind_sqlint : (SQLPOINTER)&paramPrecision.ind_sqllen;
  paramArrays[8].columnAvailable = true;

  /* bind the result param for param buffer length */
  paramArrays[9].columnIndex     = 9;
  paramArrays[9].columnType      = SQL_C_LONG;
  paramArrays[9].columnPtr       = (SQLPOINTER)&paramLength.val;
  paramArrays[9].columnSize      = (SQLLEN)sizeof(paramLength.val);
  paramArrays[9].columnInd       = __mb_fintAvailable ? (SQLPOINTER)&paramLength.ind_sqlint : (SQLPOINTER)&paramLength.ind_sqllen;
  paramArrays[9].columnAvailable = true;
                                    
  /* bind the result param for the scale of the parameter */
  paramArrays[10].columnIndex     = 10;
  paramArrays[10].columnType      = SQL_C_SHORT;
  paramArrays[10].columnPtr       = (SQLPOINTER)&paramScale.val;
  paramArrays[10].columnSize      = (SQLLEN)sizeof(paramScale.val);
  paramArrays[10].columnInd       = __mb_fintAvailable ? (SQLPOINTER)&paramScale.ind_sqlint : (SQLPOINTER)&paramScale.ind_sqllen;
  paramArrays[10].columnAvailable = true;

  paramArrays[11].columnIndex     = 11;
  paramArrays[11].columnType      = SQL_C_SHORT;
  paramArrays[11].columnPtr       = (SQLPOINTER)&paramNUMRADIX.val;
  paramArrays[11].columnSize      = (SQLLEN)sizeof(paramNUMRADIX.val);
  paramArrays[11].columnInd       = __mb_fintAvailable ? (SQLPOINTER)&paramNUMRADIX.ind_sqlint : (SQLPOINTER)&paramNUMRADIX.ind_sqllen;
  paramArrays[11].columnAvailable = true;
  
  paramArrays[12].columnIndex     = 12;
  paramArrays[12].columnType      = SQL_C_SHORT;
  paramArrays[12].columnPtr       = (SQLPOINTER)&paramNULLABLE.val;
  paramArrays[12].columnSize      = (SQLLEN)sizeof(paramNULLABLE.val);
  paramArrays[12].columnInd       = __mb_fintAvailable ? (SQLPOINTER)&paramNULLABLE.ind_sqlint : (SQLPOINTER)&paramNULLABLE.ind_sqllen;
  paramArrays[12].columnAvailable = true;
  
  paramArrays[13].columnIndex     = 13;
  paramArrays[13].columnType      = SQL_C_CHAR;
  paramArrays[13].columnPtr       = (SQLPOINTER)paramRemarks.val;
  paramArrays[13].columnSize      = (SQLLEN)129;
  paramArrays[13].columnInd       = __mb_fintAvailable ? (SQLPOINTER)&paramRemarks.ind_sqlint : (SQLPOINTER)&paramRemarks.ind_sqllen;
  paramArrays[13].columnAvailable = true;
  
  paramArrays[14].columnIndex     = 14;
  paramArrays[14].columnType      = SQL_C_CHAR;
  paramArrays[14].columnPtr       = (SQLPOINTER)paramDef.val;
  paramArrays[14].columnSize      = (SQLLEN)129;
  paramArrays[14].columnInd       = __mb_fintAvailable ? (SQLPOINTER)&paramDef.ind_sqlint : (SQLPOINTER)&paramDef.ind_sqllen;
  paramArrays[14].columnAvailable = true;
  
  /* bind the result param for the SQLDATATYPE of the parameter */
  paramArrays[15].columnIndex     = 15;
  paramArrays[15].columnType      = SQL_C_SHORT;
  paramArrays[15].columnPtr       = (SQLPOINTER)&paramSQLDATATYPE.val;
  paramArrays[15].columnSize      = (SQLLEN)sizeof(paramSQLDATATYPE.val);
  paramArrays[15].columnInd       = __mb_fintAvailable ? (SQLPOINTER)&paramSQLDATATYPE.ind_sqlint : (SQLPOINTER)&paramSQLDATATYPE.ind_sqllen;
  paramArrays[15].columnAvailable = true;
                                       
  /* bind the result param for the SQLDATETIMESUB of the parameter */
  paramArrays[16].columnIndex     = 16;
  paramArrays[16].columnType      = SQL_C_SHORT;
  paramArrays[16].columnPtr       = (SQLPOINTER)&paramSQLDATETIMESUB.val;
  paramArrays[16].columnSize      = (SQLLEN)sizeof(paramSQLDATETIMESUB.val);
  paramArrays[16].columnInd       = __mb_fintAvailable ? (SQLPOINTER)&paramSQLDATETIMESUB.ind_sqlint : (SQLPOINTER)&paramSQLDATETIMESUB.ind_sqllen;
  paramArrays[16].columnAvailable = true;
        
  /* bind the result param for the CharOctetLength of the parameter */
  paramArrays[17].columnIndex     = 17;
  paramArrays[17].columnType      = SQL_C_LONG;
  paramArrays[17].columnPtr       = (SQLPOINTER)&paramCharOctetLength.val;
  paramArrays[17].columnSize      = (SQLLEN)sizeof(paramCharOctetLength.val);
  paramArrays[17].columnInd       = __mb_fintAvailable ? (SQLPOINTER)&paramCharOctetLength.ind_sqlint : (SQLPOINTER)&paramCharOctetLength.ind_sqllen;
  paramArrays[17].columnAvailable = true;        
        
                                                                              
  /* bind the result param for parameter ordinal */
  paramArrays[18].columnIndex     = 18;
  paramArrays[18].columnType      = SQL_C_LONG;
  paramArrays[18].columnPtr       = (SQLPOINTER)&paramOrdinal.val;
  paramArrays[18].columnSize      = (SQLLEN)sizeof(paramOrdinal.val);
  paramArrays[18].columnInd       = __mb_fintAvailable ? (SQLPOINTER)&paramOrdinal.ind_sqlint : (SQLPOINTER)&paramOrdinal.ind_sqllen;
  paramArrays[18].columnAvailable = true;

   /* bind the result param for the NULLABLE  */  
  paramArrays[19].columnIndex     = 19;
  paramArrays[19].columnType      = SQL_C_CHAR;
  paramArrays[19].columnPtr       = paramIsNullable.val;
  paramArrays[19].columnSize      = 129;
  paramArrays[19].columnInd       = __mb_fintAvailable ? (SQLPOINTER)&paramIsNullable.ind_sqlint : (SQLPOINTER)&paramIsNullable.ind_sqllen;
  paramArrays[19].columnAvailable = true;
    
  for(int i = 0; i <20;i++)
  {
  	if(paramArrays[i].columnAvailable)
  	{
		  __odbc_sqlreturn = __odbc_SQLBindCol(__mp_conn,odbc_hstmt_for_proc,
		                                       paramArrays[i].columnIndex,
		                                       paramArrays[i].columnType,
		                                       paramArrays[i].columnPtr,
		                                       paramArrays[i].columnSize,
		                                       paramArrays[i].columnInd);
		
		  if(__ODBC_HOOK_CHECK_FAILED(__odbc_sqlreturn))
		  {
		    OdbcKit::checkException(__XDBC_HOOK_INVOKE_MORE  __mp_conn, odbc_hstmt_for_proc,__odbc_sqlreturn,ODBCCALLABLESTATEMENT_CLASS_NAME);
		    __XDBC_HOOK_CHECK(return);
		  }  		
  	
  	}//~end if(paramArrays[i].columnAvailable)
  	
  }//~end for(int i = 0; i <20;i++)
                                        
                                        
  __odbc_sqlreturn = (__api_SQLFETCH)(odbc_hstmt_for_proc);

//  printf("__odbc_sqlreturn : %d,%d\n",__odbc_sqlreturn,SQL_NO_DATA_FOUND); 
 
  ODBCSQLOPTIONS_ADJUSTBINDABLE  __options_ADJUSTBINDABLE = __mp_conn->__odbc_options->__options_ADJUSTBINDABLE;  
    
  while (__odbc_sqlreturn != SQL_NO_DATA_FOUND)
  {
    ODBC_PARAM* __odbc_param = new ODBC_PARAM();
    __odbc_param->__mb_stored_proc = true;
    __vector_params.push_back(__odbc_param);
    
     /* store the param name for printing the stored procedure output */
    __odbc_param->__ms_name.assign((const char*)paramName.val, __mb_fintAvailable ? paramName.ind_sqlint:paramName.ind_sqllen);
    
    /* get the parameter type and data if input */
    __odbc_param->__mi_iomode = paramType.val;
    
    /* SQL data type of the parameter */
    __odbc_param->__mi_type =   paramDataType.val;
    __odbc_param->__ms_type_name.assign((const char*)paramTypeName.val, __mb_fintAvailable ? paramTypeName.ind_sqlint : paramTypeName.ind_sqllen);
    
     /* ColumnSize is only relevant for CHARACTER, DECIMAL, and NUMERIC */
    __odbc_param->__mi_precision = paramPrecision.val;
    
    __odbc_param->__mi_length = paramLength.val;
    
    /* DecimalDigits is only relevant for DECIMAL and NUMERIC */
    __odbc_param->__mi_scale = paramScale.val; /* scale */
                                           
    __odbc_param->__mi_index = paramOrdinal.val;
    __odbc_param->__mi_SQL_DATA_TYPE = paramSQLDATATYPE.val;
    __odbc_param->__mi_SQL_DATETIME_SUB = paramSQLDATETIMESUB.val;  

    __odbc_sqlreturn = (__api_SQLFETCH)(odbc_hstmt_for_proc);
    
  }//~end while (__odbc_sqlreturn != SQL_NO_DATA_FOUND)
  
}                          
                            
bool OdbcCallableStatement::checkConversion(__XDBC_HOOK_DECLARE_MORE int paramIndex,ODBC_PARAM* odbc_param,int outType,char* outBuffer) __XDBC_THROW_DECLARE(SQLException)
{
  ODBC_PARAM* __p_odbc_param = odbc_param;
  if(__p_odbc_param == 0)
  {
    __p_odbc_param = __vector_params[paramIndex];
  }
                            
  if((__p_odbc_param->__mi_iomode != SQL_PARAM_OUTPUT) || (__p_odbc_param->__mi_iomode != SQL_PARAM_INPUT_OUTPUT)) 
  {                           
    __XDBC_HOOK_RETURN(return false,"can only retrive return data from OUT parameter");
  }                             
                                 
  int inType = __p_odbc_param->__mi_bind_type;
 
  if(inType == outType)
  {
    return false;
  }
   
  OdbcTypes* __odbcTypes = &(OdbcTypesImpl::getInstance());
  struct ODBC_DRIVER_OPTIONS* __odbc_options = __mp_conn->__odbc_options;
 
  StringBuffer __sb;
  __sb.append("[OdbcCallableStatement] Conversion not supported . SRC_SQL_TYPE:");
  __sb.append(__p_odbc_param->__ms_type_name);
  __sb.append("(");
  __sb.append(inType);
  __sb.append("),DST_SQL_TYPE:");
  __sb.append(__odbcTypes->getTypeName(__odbc_options,outType));
  __sb.append("(");
  __sb.append(outType);
  __sb.append(")");
  
  __XDBC_HOOK_RETURN(return false,__sb.str());
}

__OPENCODE_END_NAMESPACE
