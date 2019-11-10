#include "xdbc/odbcArrayStatement.h"
#include "xdbc/odbcConnection.h"
#include "xdbc/dataTable.h"
#include "xdbc/odbcTypes.h"
#include "xdbc/xsmart.h"

#include "port/ioc_sbuf.h"
#include "port/utilities.h"

__OPENCODE_BEGIN_NAMESPACE


#define ODBCARRAYSTATEMENT_CLASS_NAME "OdbcArrayStatement"

OdbcArrayStatement::~OdbcArrayStatement()
{
  __XDBC_TRACE(ODBCARRAYSTATEMENT_CLASS_NAME,"destroy()"); 
}

OdbcArrayStatement::OdbcArrayStatement(OdbcConnection* pConnection,const string& sql,int paramCount)
{ 
  __mp_conn = pConnection;
  __api_SQLEXECUTE       = __mp_conn->__odbc_apis->__api_SQLEXECUTE;
  __options_SUPPORTSSTMTATTR = __mp_conn->__odbc_options->__options_SUPPORTSSTMTATTR;
    
  __mi_resultSetType = ResultSet::TYPE_FORWARD_ONLY;
  __mi_resultSetConcurrency = ResultSet::CONCUR_READ_ONLY;
  
  odbc_hstmt = SQL_NULL_HSTMT;
  __mb_initialized=false;
  __mb_isClosed=false;
    
  __ms_odbc_sql = sql;
  __mi_param_count = paramCount;
  
  __mi_array_size = 1;
  __mi_array_max  = __ODBC_ARRAY_MAX; 
}

OdbcArrayStatement::OdbcArrayStatement(OdbcConnection* pConnection,const string& sql, int resultSetType, int resultSetConcurrency,int paramCount)
{
  __mp_conn = pConnection;
  __api_SQLEXECUTE       = __mp_conn->__odbc_apis->__api_SQLEXECUTE;
    
  __mi_resultSetType = resultSetType;
  __mi_resultSetConcurrency = resultSetConcurrency;
  
  odbc_hstmt = SQL_NULL_HSTMT;
  __mb_initialized=false;
  __mb_isClosed=false;
    
  __ms_odbc_sql = sql;
  __mi_param_count = paramCount;
  
  __mi_array_size = 1;
  __mi_array_max  = __ODBC_ARRAY_MAX; 
}

void OdbcArrayStatement::initialize(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
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
    OdbcKit::checkException(__XDBC_HOOK_INVOKE_MORE  __mp_conn, odbc_hstmt,__odbc_sqlreturn,ODBCARRAYSTATEMENT_CLASS_NAME);
    __XDBC_HOOK_CHECK(return);
  }

  SQLINTEGER __option = SQL_ATTR_NOSCAN;
  SQLINTEGER __param = SQL_NOSCAN_ON;
  if((__options_SUPPORTSSTMTATTR)(__option, (SQLPOINTER )__param,(SQLINTEGER)SQL_IS_INTEGER) == SQL_SUCCESS)
  {
    __odbc_sqlreturn = __odbc_SQLSetStmtAttr(__mp_conn,odbc_hstmt,__option, (SQLPOINTER)__param,(SQLINTEGER)SQL_IS_INTEGER,(SQLPOINTER)&__param,(SQLINTEGER)SQL_IS_POINTER);
    
    if(__ODBC_HOOK_CHECK_FAILED(__odbc_sqlreturn))
    {
      OdbcKit::checkException(__XDBC_HOOK_INVOKE_MORE  __mp_conn, odbc_hstmt,__odbc_sqlreturn,ODBCARRAYSTATEMENT_CLASS_NAME);
      __XDBC_HOOK_CHECK(return);
    }
  }
  
  //-------------------------------------------------------------------------
  // 1. fetch  VariableData
  //-------------------------------------------------------------------------
  __odbc_sqlreturn = __odbc_SQLPrepare(__mp_conn,odbc_hstmt,(SQLCHAR*)__ms_odbc_sql.c_str(),SQL_NTS);
  if(__ODBC_HOOK_CHECK_FAILED(__odbc_sqlreturn))
  {
    OdbcKit::checkException(__XDBC_HOOK_INVOKE_MORE  __mp_conn, odbc_hstmt,__odbc_sqlreturn,ODBCARRAYSTATEMENT_CLASS_NAME);
    __XDBC_HOOK_CHECK(return);
  }

  OdbcKit::prepareVariableData(__XDBC_HOOK_INVOKE_MORE __mp_conn,odbc_hstmt,__vector_params);
  __XDBC_HOOK_CHECK(return);

  __mi_param_count= (int)(__vector_params.size() - 1);
}

int OdbcArrayStatement::executeUpdate(__XDBC_HOOK_DECLARE_MORE DataTable* paramTable,vector<ArrayException>* errors)  __XDBC_THROW_DECLARE(SQLException)
{
  __mi_array_size = 1;

  int rowcount = 0;
  int colcount = 0;

  if(paramTable != 0)
  {
    rowcount = paramTable->getRowCount();
    colcount = paramTable->getColumnCount();
    __mi_array_size = __M_min(__mi_array_max,rowcount);
  }
  
  if(DriverManager::isTracingOn)
  {
     StringBuffer __sb;
     __sb<<"executeUpdate|datarows[s/r/m] = ["<<__mi_array_size
       <<"/" <<rowcount<<"/"<<__mi_array_max <<"] , cols = ["<<colcount<<"]";

     __XDBC_FORCED_TRACE(ODBCARRAYSTATEMENT_CLASS_NAME,__sb.str());
  }

  if(paramTable != 0)
  {
    setBindData(__XDBC_HOOK_INVOKE_MORE paramTable);
    __XDBC_HOOK_CHECK(return);
  }

  SQLRETURN __odbc_sqlreturn = SQL_SUCCESS;   
  SQLUINTEGER __numdone_INT = __mi_array_size;
  SQLULEN     __numdone_LEN = __mi_array_size;
  int        __rows_return = __mi_array_size;

  struct ODBC_LIBRARY_APIS* __odbc_apis = __mp_conn->__odbc_apis;

  if(__odbc_apis->__api_SQLSETSTMTATTR != 0)
  {
    __odbc_sqlreturn = (__odbc_apis->__api_SQLSETSTMTATTR)(odbc_hstmt,SQL_ATTR_PARAMSET_SIZE,(SQLPOINTER)__numdone_INT,(SQLINTEGER)SQL_IS_INTEGER);
    if(__ODBC_HOOK_CHECK_FAILED(__odbc_sqlreturn))
    {
      OdbcKit::checkException(__XDBC_HOOK_INVOKE_MORE  __mp_conn, odbc_hstmt,__odbc_sqlreturn,ODBCARRAYSTATEMENT_CLASS_NAME);
      __XDBC_HOOK_CHECK(return 0);
    }
    __odbc_sqlreturn = (__odbc_apis->__api_SQLSETSTMTATTR)(odbc_hstmt,SQL_ATTR_PARAMS_PROCESSED_PTR,(SQLPOINTER)&__numdone_INT,SQL_IS_POINTER);
  }
  else
  {
    __odbc_sqlreturn = (__odbc_apis->__api_SQLPARAMOPTIONS)(odbc_hstmt,__mi_array_size,&__numdone_INT);
  }

  if(__ODBC_HOOK_CHECK_FAILED(__odbc_sqlreturn))
  {
    OdbcKit::checkException(__XDBC_HOOK_INVOKE_MORE  __mp_conn, odbc_hstmt,__odbc_sqlreturn,ODBCARRAYSTATEMENT_CLASS_NAME);
    __XDBC_HOOK_CHECK(return 0);
  }

  __odbc_sqlreturn = (__api_SQLEXECUTE)(odbc_hstmt); 
  if(__ODBC_HOOK_CHECK_FAILED(__odbc_sqlreturn))
  {
    OdbcKit::checkException(__XDBC_HOOK_INVOKE_MORE  __mp_conn, odbc_hstmt,__odbc_sqlreturn,ODBCARRAYSTATEMENT_CLASS_NAME);
    __XDBC_HOOK_CHECK(return 0);
  }

  return __rows_return;
}

ArrayResultSet*  OdbcArrayStatement::executeQuery(__XDBC_HOOK_DECLARE_MORE DataTable* dataTable,DataTable* paramTable)  __XDBC_THROW_DECLARE(SQLException)
{
  __XDBC_HOOK_RETURN(return 0,"OdbcArrayStatement::executeQuery(DataTable*,DataTable*) not supported");
}

void  OdbcArrayStatement::close()
{
  __XDBC_TRACE(ODBCARRAYSTATEMENT_CLASS_NAME,"close()");

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

void  OdbcArrayStatement::destroy()
{
  delete this;
}
  
int OdbcArrayStatement::getResultSetType(__XDBC_HOOK_DECLARE_ONCE)   __XDBC_THROW_DECLARE(SQLException)
{
  return __mi_resultSetType;
}

Connection* OdbcArrayStatement::getConnection(__XDBC_HOOK_DECLARE_ONCE)   __XDBC_THROW_DECLARE(SQLException)
{
  return (Connection*)__mp_conn;
}

int  OdbcArrayStatement::getMaxArraySize() const
{
  return __mi_array_max;
}

void  OdbcArrayStatement::setMaxArraySize(int size)
{
  __mi_array_max = size;
}

void OdbcArrayStatement::setBindData(__XDBC_HOOK_DECLARE_MORE DataTable* paramTable)  __XDBC_THROW_DECLARE(SQLException)
{
  int colcount = paramTable->getColumnCount();
  for(int i = 1; i <= colcount;++i)
  {
    setBindDataImpl(__XDBC_HOOK_INVOKE_MORE paramTable,i);
    __XDBC_HOOK_CHECK(return);
  }
}

#define __ODBC_BINDPARAM_CHECK(__param_index,__odbc_type,__param_size)\
if(sizeof(__odbc_type) != __param_size)\
{\
  StringBuffer __sb;\
  __sb << "OdbcArrayStatement::setBindData(#" << __param_index << ") paramSize != sizeof(" << #__odbc_type << ")";\
  __XDBC_HOOK_RETURN(return,__sb.str());\
} 

void  OdbcArrayStatement::setBindDataImpl(__XDBC_HOOK_DECLARE_MORE DataTable* paramTable,int paramIndex)  __XDBC_THROW_DECLARE(SQLException)
{
  struct ODBC_DRIVER_OPTIONS* __odbc_options = __mp_conn->__odbc_options;
 
  char* __colData = (char*)(paramTable->getModel()->getDataBuffer() + paramTable->getColumnOffset(paramIndex));
  int __colSize =  paramTable->getColumnSize(paramIndex);
  int __colType =  0;
  SQLPOINTER __indData = (SQLPOINTER)0;
  int __indSkip = 0;
      
  if(paramTable->getModel()->useColumnRawType(paramIndex))
  {
    __colType =  paramTable->getModel()->getColumnRawType(paramIndex);
  }
  else
  {
    OdbcTypes* __odbcTypes = &(OdbcTypesImpl::getInstance());
    __colType = __odbcTypes->xdbc_cnvt_odbc(__odbc_options,paramTable->getColumnType(paramIndex));
  }
  
  if(DriverManager::isTracingOn)
  {
     StringBuffer __sb;
     __sb<<"setBindData(#" << paramIndex << ")| (paramType,paramSize) = (" << __colType << "," << __colSize << ")";  
     __XDBC_FORCED_TRACE(ODBCARRAYSTATEMENT_CLASS_NAME,__sb.str());
  }

  if(paramTable->getModel()->useColumnIndicator(paramIndex))
  {
    __indData = (SQLPOINTER)(paramTable->getModel()->getIndicatorBuffer() + paramTable->getModel()->getColumnIndicatorOffset(paramIndex));
    __indSkip = paramTable->getModel()->getColumnIndicatorSkip(paramIndex);
  }
 
  if(__odbc_options->__mb_fintAvailable)
  { 
    if( (__indData != 0) && ((sizeof(SQLINTEGER) != __indSkip)))
    {
      StringBuffer __sb;
      __sb << "setBindData(#" << paramIndex << ")| sizeof(Indicator) != sizeof(SQLINTEGER))";
      __XDBC_HOOK_RETURN(return,__sb.str());
    }
  }
  else
  {
    if( (__indData != 0) && ((sizeof(SQLLEN) != __indSkip)))
    {
      StringBuffer __sb;
      __sb << "setBindData(#" << paramIndex << ")| sizeof(Indicator) != sizeof(SQLLEN))";
      __XDBC_HOOK_RETURN(return,__sb.str());
    }
  }

  SQLRETURN  __odbc_sqlreturn = SQL_SUCCESS;
    
  switch(__colType)
  {
//-----------------------------
    case SQL_BIT:
    case SQL_TINYINT:
    {                                               
      __ODBC_BINDPARAM_CHECK(paramIndex,SQLCHAR,__colSize); 
      __odbc_sqlreturn = __odbc_SQLBindParameter(__mp_conn,odbc_hstmt, 
                                                     paramIndex, 
                                                     SQL_PARAM_INPUT, 
                                                     SQL_C_TINYINT, 
                                                     SQL_TINYINT, 
                                                     0, 
                                                     0, 
                                                     __colData, 
                                                     __colSize, 
                                                     __indData);
    }
    break;
//-----------------------------
    case SQL_SMALLINT:
    {
      __ODBC_BINDPARAM_CHECK(paramIndex,SQLSMALLINT,__colSize); 
      __odbc_sqlreturn = __odbc_SQLBindParameter(__mp_conn,odbc_hstmt, 
                                                    paramIndex, 
                                                    SQL_PARAM_INPUT, 
                                                    SQL_C_SHORT, 
                                                    SQL_SMALLINT, 
                                                    0, 
                                                    0, 
                                                    __colData, 
                                                    __colSize, 
                                                    __indData);
    }
    break;
//-----------------------------
    case SQL_INTEGER:
    {
      __ODBC_BINDPARAM_CHECK(paramIndex,SQLINTEGER,__colSize);  
      __odbc_sqlreturn = __odbc_SQLBindParameter(__mp_conn,odbc_hstmt, 
                                                    paramIndex, 
                                                    SQL_PARAM_INPUT, 
                                                    SQL_C_LONG, 
                                                    SQL_INTEGER, 
                                                    0, 
                                                    0, 
                                                    __colData, 
                                                    __colSize, 
                                                   __indData);
    }
    break;
//-----------------------------
    case SQL_BIGINT:
    {
      __ODBC_BINDPARAM_CHECK(paramIndex,SQLBIGINT,__colSize); 
      __odbc_sqlreturn = __odbc_SQLBindParameter(__mp_conn,odbc_hstmt, 
                                                    paramIndex, 
                                                    SQL_PARAM_INPUT, 
                                                    SQL_C_BIGINT, 
                                                    SQL_BIGINT, 
                                                    0, 
                                                    0, 
                                                    __colData, 
                                                    __colSize, 
                                                    __indData);
    }
    break;
//-----------------------------
    case SQL_REAL:
    {
      __ODBC_BINDPARAM_CHECK(paramIndex,SQLREAL,__colSize);
      __odbc_sqlreturn = __odbc_SQLBindParameter(__mp_conn,odbc_hstmt, 
                                                    paramIndex, 
                                                    SQL_PARAM_INPUT, 
                                                    SQL_C_FLOAT, 
                                                    SQL_REAL, 
                                                    0, 
                                                    0, 
                                                    __colData, 
                                                    __colSize, 
                                                    __indData);
    }
    break;
//-----------------------------
    case SQL_FLOAT:
    case SQL_DOUBLE:
    {
      __ODBC_BINDPARAM_CHECK(paramIndex,SQLDOUBLE,__colSize);
      __odbc_sqlreturn = __odbc_SQLBindParameter(__mp_conn,odbc_hstmt, 
                                                    paramIndex, 
                                                    SQL_PARAM_INPUT, 
                                                    SQL_C_DOUBLE, 
                                                    SQL_DOUBLE, 
                                                    0, 
                                                    0, 
                                                    __colData, 
                                                    __colSize, 
                                                    __indData);
    }
    break;
//-----------------------------
    case SQL_NUMERIC:
    case SQL_DECIMAL:
    {
      __ODBC_BINDPARAM_CHECK(paramIndex,SQLDOUBLE,__colSize);
      __odbc_sqlreturn = __odbc_SQLBindParameter(__mp_conn,odbc_hstmt, 
                                                    paramIndex, 
                                                    SQL_PARAM_INPUT, 
                                                    SQL_C_DOUBLE, 
                                                    __colType, 
                                                    0, 
                                                    0, 
                                                    __colData, 
                                                    __colSize, 
                                                    __indData);
    }
    break;
//-----------------------------
    case SQL_TYPE_DATE:
    {
      __ODBC_BINDPARAM_CHECK(paramIndex,DATE_STRUCT,__colSize);
      __odbc_sqlreturn = __odbc_SQLBindParameter(__mp_conn,odbc_hstmt, 
                                                    paramIndex, 
                                                    SQL_PARAM_INPUT, 
                                                    SQL_C_TYPE_DATE, 
                                                    SQL_TYPE_DATE, 
                                                    0, 
                                                    0, 
                                                    __colData, 
                                                    __colSize, 
                                                    __indData);
    }
    break;
//-----------------------------
    case SQL_TYPE_TIME:
    {
      __ODBC_BINDPARAM_CHECK(paramIndex,TIME_STRUCT,__colSize); 
      __odbc_sqlreturn = __odbc_SQLBindParameter(__mp_conn,odbc_hstmt, 
                                                     paramIndex, 
                                                     SQL_PARAM_INPUT, 
                                                     SQL_C_TYPE_TIME, 
                                                     SQL_TYPE_TIME, 
                                                     0, 
                                                     0, 
                                                     __colData, 
                                                     __colSize, 
                                                     __indData);
    }
    break;
//-----------------------------
    case SQL_TYPE_TIMESTAMP:
    {
      __ODBC_BINDPARAM_CHECK(paramIndex,TIMESTAMP_STRUCT,__colSize);  
      __odbc_sqlreturn = __odbc_SQLBindParameter(__mp_conn,odbc_hstmt, 
                                                     paramIndex, 
                                                     SQL_PARAM_INPUT, 
                                                     SQL_C_TYPE_TIMESTAMP, 
                                                     SQL_TYPE_TIMESTAMP, 
                                                     0, 
                                                     0, 
                                                     __colData, 
                                                     __colSize, 
                                                     __indData);
    }
    break;
//-----------------------------
    case SQL_CHAR:
    case SQL_VARCHAR:
    case SQL_LONGVARCHAR:
    { 
      __odbc_sqlreturn = __odbc_SQLBindParameter(__mp_conn,odbc_hstmt, 
                                                    paramIndex, 
                                                    SQL_PARAM_INPUT, 
                                                    SQL_C_CHAR, 
                                                    SQL_VARCHAR, 
                                                    __colSize, 
                                                    0, 
                                                    __colData, 
                                                    __colSize, 
                                                    __indData);
    }
    break;
//-----------------------------
    case SQL_BINARY:
    case SQL_VARBINARY:
    case SQL_LONGVARBINARY:
    { 
      __odbc_sqlreturn = __odbc_SQLBindParameter(__mp_conn,odbc_hstmt, 
                                                     paramIndex, 
                                                     SQL_PARAM_INPUT, 
                                                     SQL_C_BINARY, 
                                                     SQL_VARBINARY, 
                                                     __colSize, 
                                                     0, 
                                                     __colData, 
                                                     __colSize, 
                                                     __indData);
    }
    break;
//-----------------------------
    default:  
    { 
      ODBC_PARAM* __odbc_param = __vector_params[paramIndex]; 
      __odbc_sqlreturn = __odbc_SQLBindParameter(__mp_conn,odbc_hstmt, 
                                                     paramIndex, 
                                                     SQL_PARAM_INPUT, 
                                                     __odbc_param->__mi_bind_c_type, 
                                                     __odbc_param->__mi_bind_type, 
                                                     __colSize, 
                                                     0, 
                                                     __colData, 
                                                     __colSize, 
                                                     __indData);
    }
    break;
  }
  
  if(__ODBC_HOOK_CHECK_FAILED(__odbc_sqlreturn))
  {
    OdbcKit::checkException(__XDBC_HOOK_INVOKE_MORE  __mp_conn, odbc_hstmt,__odbc_sqlreturn,ODBCARRAYSTATEMENT_CLASS_NAME);
    __XDBC_HOOK_CHECK(return);
  } 
}

__OPENCODE_END_NAMESPACE

