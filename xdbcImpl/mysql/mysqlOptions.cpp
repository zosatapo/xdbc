#include "xdbc/odbc.h"
#include "port/utilities.h"

USING_OPENCODE_NS

#ifdef __cplusplus
extern "C" {                /* Assume C declarations for C++   */
#endif  /* __cplusplus */

 __OPENCODE_EXPORT_DECL SQLRETURN  odbcSupportsConnectAttr(SQLINTEGER Attribute,SQLPOINTER ValuePtr,SQLINTEGER  StringLength)
{
  SQLRETURN __odbc_sqlreturn = SQL_ERROR;

  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    __sb << "<<" << Attribute << ">> , return <<" << __odbc_sqlreturn << ">>";
    __XDBC_FORCED_TRACE("mysql::odbcSupportsConnectAttr",__sb.str());
  }
    
  return __odbc_sqlreturn;
}

 __OPENCODE_EXPORT_DECL SQLRETURN  odbcSupportsStmtAttr(SQLINTEGER Attribute,SQLPOINTER ValuePtr,SQLINTEGER  StringLength)
{
  SQLRETURN __odbc_sqlreturn = SQL_ERROR;

  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    __sb << "<<" << Attribute << ">> , return <<" << __odbc_sqlreturn << ">>";
    __XDBC_FORCED_TRACE("mysql::odbcSupportsStmtAttr",__sb.str());
  }
    
  return __odbc_sqlreturn;
}

 __OPENCODE_EXPORT_DECL SQLRETURN  odbcSupportsEnvAttr(SQLINTEGER Attribute,SQLPOINTER ValuePtr,SQLINTEGER  StringLength)
{
  SQLRETURN __odbc_sqlreturn = SQL_ERROR;

  if(DriverManager::isTracingOn && (__odbc_sqlreturn == SQL_SUCCESS))
  {
    StringBuffer __sb;
    __sb << "<<" << Attribute << ">> , return <<" << __odbc_sqlreturn << ">>";
    __XDBC_FORCED_TRACE("mysql::odbcSupportsEnvAttr",__sb.str());
  }
    
  return __odbc_sqlreturn;
}

 __OPENCODE_EXPORT_DECL SQLRETURN  odbcGetTypeName(SQLINTEGER OdbcTypeCode,SQLCHAR* Buffer,SQLINTEGER BufferLen,SQLINTEGER* BufferLenPtr)
{
  SQLINTEGER __typeLen = 0;
  SQLRETURN __odbc_sqlreturn =  SQL_ERROR; 
  switch(OdbcTypeCode)
  {
    case SQL_CHAR:
    case SQL_VARCHAR:
    case SQL_LONGVARCHAR:
    {
    }
    break;
//-----------------------------
    case SQL_BINARY:
    case SQL_VARBINARY:
    case SQL_LONGVARBINARY:
    {
    }
    break;      
//-----------------------------
  }//~end switch(OdbcTypeCode)
    
  if(BufferLenPtr != 0)
  {
    *BufferLenPtr = __typeLen;
  }  

  if(DriverManager::isTracingOn && (__odbc_sqlreturn == SQL_SUCCESS))
  {
    StringBuffer __sb;
    __sb << "__mi_type = " << OdbcTypeCode << " , __ms_name = " << (const char*)Buffer;
    __XDBC_FORCED_TRACE("altibase::odbcGetTypeName",__sb.str());
  }
    
  return __odbc_sqlreturn;
}

 __OPENCODE_EXPORT_DECL SQLRETURN odbcAdjustDefinable(SQLCHAR* odbcSQL,SQLPOINTER odbcColumnPtr,SQLPOINTER odbcDefinePtr)
{
  ODBC_COLUMN* __p_column = (ODBC_COLUMN*)odbcColumnPtr;
  ODBC_DEFINE* __p_define = (ODBC_DEFINE*)odbcDefinePtr;
  SQLRETURN __odbc_sqlreturn = SQL_ERROR;
  
  switch(__p_column->__mi_type)
  {                                        
//-----------------------------
    default:
    {
    }
    break;
  }//~end switch(__p_column->__mi_type)

  if(DriverManager::isTracingOn && (__odbc_sqlreturn == SQL_SUCCESS))
  {
    StringBuffer __sb;
    __sb << "__mi_index = " << __p_column->__mi_index << " , __mi_define_type = " << __p_define->__mi_define_type;
    __sb << " , __mi_define_c_type = " << __p_define->__mi_define_c_type;
    __XDBC_FORCED_TRACE("mysql::odbcAdjustDefinable",__sb.str());
  }
     
  return __odbc_sqlreturn;
}

 __OPENCODE_EXPORT_DECL SQLRETURN odbcAdjustBindable(SQLCHAR* odbcSQL,SQLPOINTER odbcParamPtr)
{
  ODBC_PARAM* __p_param = (ODBC_PARAM*)odbcParamPtr;
  __p_param->__mi_bind_type  = __p_param->__mi_type;
  SQLRETURN __odbc_sqlreturn = SQL_ERROR;
  
  switch(__p_param->__mi_type)
  {
    case SQL_CHAR:
    case SQL_VARCHAR:
    case SQL_LONGVARCHAR:
    {
      __p_param->__mi_bind_c_type = SQL_C_CHAR;
      __p_param->__mi_bind_type   = __p_param->__mi_type;
      
      __p_param->__mi_bind_ind_int = SQL_NTS;
      __p_param->__mi_bind_ind_len = SQL_NTS;
      __odbc_sqlreturn = SQL_SUCCESS;
    }
    break;
//-----------------------------
    default:
    {
    }
    break;
    
  }//~end switch(__p_param->__mi_type)
 

  if(StringsKit::equalsIgnoreCase((const char*)odbcSQL,"INSERT INTO XDBC_DEMO(VC_COL,INT_COL) VALUES(?,?)"))
  {
    switch(__p_param->__mi_index)
    {
      case 1:
      {
        __p_param->__mi_length = 100;
        __odbc_sqlreturn = SQL_SUCCESS;
      }
      break;
//-----------------------------
      case 2:
      {
        __p_param->__mi_length = sizeof(SQLINTEGER);
        __odbc_sqlreturn = SQL_SUCCESS;
      }
      break;
    }
  }
  else if(StringsKit::equalsIgnoreCase((const char*)odbcSQL,"SELECT * FROM XDBC_PERF WHERE INT_COL = ?"))
  {
    __p_param->__mi_length = sizeof(SQLINTEGER);
    __p_param->__mi_type   = SQL_INTEGER;
    __odbc_sqlreturn = SQL_SUCCESS;
  }
  else if(StringsKit::equalsIgnoreCase((const char*)odbcSQL,"INSERT INTO XDBC_PERF(INT_COL) VALUES(?)"))
  {
    __p_param->__mi_length = sizeof(SQLINTEGER);
    __p_param->__mi_type   = SQL_INTEGER;
    __odbc_sqlreturn = SQL_SUCCESS;
  }
  else if(StringsKit::equalsIgnoreCase((const char*)odbcSQL,"UPDATE XDBC_PERF SET INT_COL2 = 1 WHERE INT_COL = ? "))
  {
    __p_param->__mi_length = sizeof(SQLINTEGER);
    __p_param->__mi_type   = SQL_INTEGER;
    __odbc_sqlreturn = SQL_SUCCESS;
  }
  else if(StringsKit::equalsIgnoreCase((const char*)odbcSQL,"DELETE FROM XDBC_PERF WHERE INT_COL = ?"))
  {
    __p_param->__mi_length = sizeof(SQLINTEGER);
    __p_param->__mi_type   = SQL_INTEGER;
    __odbc_sqlreturn = SQL_SUCCESS;
  }

  if(DriverManager::isTracingOn && (__odbc_sqlreturn == SQL_SUCCESS))
  {
    StringBuffer __sb;
    __sb << "__mi_index = " << __p_param->__mi_index << " , __mi_length = " << __p_param->__mi_length;
    __sb << " , __mi_iomode = " << __p_param->__mi_iomode << " , __mi_type = " << __p_param->__mi_type;
    __XDBC_FORCED_TRACE("mysql::odbcAdjustBindable",__sb.str());
  }
    
  return __odbc_sqlreturn;  
}

#ifdef __cplusplus
}                                    /* End of extern "C" { */
#endif  /* __cplusplus */
