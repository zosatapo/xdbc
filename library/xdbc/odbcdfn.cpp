#include "xdbc/odbcdfn.h" 
#include "xdbc/odbcConnection.h"

#ifdef __cplusplus
extern "C" {                /* Assume C declarations for C++   */
#endif  /* __cplusplus */

SQLRETURN  __default_odbcSupportsConnectAttr(SQLINTEGER Attribute,SQLPOINTER ValuePtr,SQLINTEGER  StringLength)
{
  return SQL_SUCCESS;
}

SQLRETURN  __default_odbcSupportsStmtAttr(SQLINTEGER Attribute,SQLPOINTER ValuePtr,SQLINTEGER  StringLength)
{
  return SQL_SUCCESS;
}

SQLRETURN  __default_odbcSupportsEnvAttr(SQLINTEGER Attribute,SQLPOINTER ValuePtr,SQLINTEGER  StringLength)
{
  return SQL_SUCCESS;
}

SQLRETURN  __default_odbcGetTypeName(SQLINTEGER OdbcTypeCode,SQLCHAR* Buffer,SQLINTEGER BufferLen,SQLINTEGER* BufferLenPtr)
{
  return SQL_ERROR; 
}

SQLRETURN  __default_odbcAdjustDefinable(SQLCHAR* SQLText,SQLPOINTER OdbcColumnPtr,SQLPOINTER OdbcDefinePtr)
{
  return SQL_ERROR; 
}

SQLRETURN  __default_odbcAdjustBindable(SQLCHAR* SQLText,SQLPOINTER OdbcParamPtr)
{
  return SQL_ERROR; 
}

SQLRETURN __default_odbcFormatSQL(SQLCHAR* srcSQLBuffer,SQLINTEGER BufferLenth1,SQLCHAR* dstSQLBuffer, SQLINTEGER* BufferLenth2)
{
	return SQL_ERROR;
}

#ifdef __cplusplus
}                                    /* End of extern "C" { */
#endif  /* __cplusplus */

__OPENCODE_BEGIN_NAMESPACE 

SQLRETURN __odbc_SQLAllocHandle   (OdbcConnection*   ODBCConnection,
                                   SQLSMALLINT       HandleType,
                                   SQLHANDLE         InputHandle,
                                   SQLHANDLE*        OutputHandlePtr)
{
  struct ODBC_LIBRARY_APIS* __odbc_apis = ODBCConnection->__odbc_apis;  
  
  if(__odbc_apis->__api_SQLALLOCHANDLE != 0)
  {
    if(HandleType != SQL_HANDLE_ENV)
    {
      return (__odbc_apis->__api_SQLALLOCHANDLE)(HandleType,InputHandle,OutputHandlePtr);
    }
    else
    {
      SQLRETURN __odbc_sqlreturn = SQL_SUCCESS;
      __odbc_sqlreturn = (__odbc_apis->__api_SQLALLOCHANDLE)(HandleType,InputHandle,OutputHandlePtr);
      if(!__ODBC_HOOK_CHECK_FAILED(__odbc_sqlreturn))
      {
        SQLHENV* __p_odbc_henv = (SQLHENV*)OutputHandlePtr;
         __odbc_sqlreturn = (__odbc_apis->__api_SQLSETENVATTR)(*__p_odbc_henv,SQL_ATTR_ODBC_VERSION,(SQLPOINTER)SQL_OV_ODBC3,0);
         
      }//~end if(!__ODBC_HOOK_CHECK_FAILED(__odbc_sqlreturn))
      
      return __odbc_sqlreturn; 
       
    }//~end if(HandleType != SQL_HANDLE_ENV)  
  }
  else
  {
    switch( HandleType)
    {
      case SQL_HANDLE_ENV:
      {
        return (__odbc_apis->__api_SQLALLOCENV)((SQLHENV*)OutputHandlePtr); 
      }
      break;  
  
      case SQL_HANDLE_DBC:
      {
        return (__odbc_apis->__api_SQLALLOCCONNECT)((SQLHENV)InputHandle,(SQLHDBC*)OutputHandlePtr); 
      }
      break; 
  
      case SQL_HANDLE_STMT:
      {
        return (__odbc_apis->__api_SQLALLOCSTMT)((SQLHDBC)InputHandle,(SQLHSTMT*)OutputHandlePtr); 
      }
      break;  
             
    }//~end switch( HandleType)  
    
  }//~end if(__odbc_apis->__api_SQLALLOCHANDLE != 0) 
  
  return SQL_ERROR; /*never reach here*/
}
                                                     
SQLRETURN __odbc_SQLFreeHandle    (OdbcConnection*   ODBCConnection,
                                   SQLSMALLINT       HandleType,
                                   SQLHANDLE         Handle)
{
  struct ODBC_LIBRARY_APIS* __odbc_apis = ODBCConnection->__odbc_apis;
  
  if(__odbc_apis->__api_SQLFREEHANDLE != 0)
  {
    return (__odbc_apis->__api_SQLFREEHANDLE)(HandleType,Handle);
  }
  else
  {
    switch( HandleType)
    {
      case SQL_HANDLE_ENV:
      {
        return (__odbc_apis->__api_SQLFREEENV)((SQLHENV)Handle); 
      }
      break;  
  
      case SQL_HANDLE_DBC:
      {
        return (__odbc_apis->__api_SQLDISCONNECT)((SQLHDBC)Handle); 
      }
      break; 
  
      case SQL_HANDLE_STMT:
      {
        return (__odbc_apis->__api_SQLFREESTMT)((SQLHSTMT)Handle,SQL_DROP); 
      }
      break;  
             
    }//~end switch( HandleType)
     
  }//~end if(__odbc_apis->__api_SQLFREEHANDLE != 0)
  
  return SQL_ERROR; /*never reach here*/    
}

SQLRETURN __odbc_SQLEndTran       (OdbcConnection*   ODBCConnection,
                                   SQLSMALLINT       HandleType,
                                   SQLHANDLE         Handle,
                                   SQLSMALLINT       CompletionType)
{
  struct ODBC_LIBRARY_APIS* __odbc_apis = ODBCConnection->__odbc_apis;
  
  if(__odbc_apis->__api_SQLENDTRAN != 0)
  {    
    return (__odbc_apis->__api_SQLENDTRAN)(HandleType,Handle,CompletionType);    
  }
  else
  {
    return (__odbc_apis->__api_SQLTRANSACT)(ODBCConnection->odbc_henv, ODBCConnection->odbc_hdbc, CompletionType);
 
  }//~end if(__odbc_apis->__api_SQLENDTRAN != 0)
  
  return SQL_ERROR; /*never reach here*/  
}
                                            
SQLRETURN __odbc_SQLGetEnvAttr    (OdbcConnection*   ODBCConnection,
                                   SQLHENV           EnvironmentHandle,
                                   SQLINTEGER        Attribute,
                                   SQLPOINTER        ValuePtr,
                                   SQLINTEGER        BufferLength,
                                   SQLINTEGER*       StringLengthPtr)
{
  struct ODBC_LIBRARY_APIS* __odbc_apis = ODBCConnection->__odbc_apis;
  if(__odbc_apis->__api_SQLGETENVATTR != 0)
  {    
    return (__odbc_apis->__api_SQLGETENVATTR)(EnvironmentHandle,
                                            Attribute,
                                            ValuePtr,
                                            BufferLength,
                                            StringLengthPtr);
  }
  else
  {
    return SQL_ERROR;
    
  }//~end if(__odbc_apis->__api_SQLGETENVATTR != 0)
  
  return SQL_ERROR; /*never reach here*/  
}
                                  
SQLRETURN __odbc_SQLSetEnvAttr    (OdbcConnection*   ODBCConnection,
                                   SQLHENV           EnvironmentHandle,
                                   SQLINTEGER        Attribute,
                                   SQLPOINTER        ValuePtr,
                                   SQLINTEGER        StringLength,
                                   SQLPOINTER        AdditionValuePtr,
                                   SQLINTEGER        AdditionStringLength)
{
  struct ODBC_LIBRARY_APIS* __odbc_apis = ODBCConnection->__odbc_apis;
  if(__odbc_apis->__api_SQLSETENVATTR != 0)
  {
    
    return (__odbc_apis->__api_SQLSETENVATTR)(EnvironmentHandle,
                                            Attribute,
                                            ValuePtr,
                                            StringLength);
  }
  else
  {
    return SQL_ERROR;
    
  }//~end if(__odbc_apis->__api_SQLSETENVATTR != 0)
  
  return SQL_ERROR; /*never reach here*/  
}

SQLRETURN __odbc_SQLFetchScroll   (OdbcConnection*   ODBCConnection,
                                   SQLHSTMT          StatementHandle,
                                   SQLSMALLINT       FetchOrientation,
                                   SQLINTEGER        FetchOffset)
{
  struct ODBC_LIBRARY_APIS* __odbc_apis = ODBCConnection->__odbc_apis;
  
  if(__odbc_apis->__api_SQLFETCHSCROLL != 0)
  {
    
    return (__odbc_apis->__api_SQLFETCHSCROLL)(StatementHandle,FetchOrientation,FetchOffset);
  }
  else
  {
    return SQL_ERROR;
    
  }//~end if(__odbc_apis->__api_SQLFETCHSCROLL != 0)
  
  return SQL_ERROR; /*never reach here*/  
}
                       
SQLRETURN __odbc_SQLCloseCursor   (OdbcConnection*   ODBCConnection,
                                   SQLHSTMT          StatementHandle)
{
  struct ODBC_LIBRARY_APIS* __odbc_apis = ODBCConnection->__odbc_apis;
  
  if(__odbc_apis->__api_SQLCLOSECURSOR != 0)
  {
    
    return (__odbc_apis->__api_SQLCLOSECURSOR)(StatementHandle);
  }
  else
  {    
    return (__odbc_apis->__api_SQLFREESTMT)(StatementHandle,SQL_CLOSE);
    
  }//~end if(__odbc_apis->__api_SQLCLOSECURSOR != 0)
  
  return SQL_ERROR; /*never reach here*/  
}
  
//------------------------------------------------------------------------------------------
                                

SQLRETURN __odbc_SQLDescribeParam (OdbcConnection*   ODBCConnection,
                                   SQLHSTMT          StatementHandle,
                                   SQLUSMALLINT      ParameterNumber,
                                   SQLSMALLINT*      DataTypePtr,
                                   SQLUINTEGER*      ParameterSizePtr,
                                   SQLSMALLINT*      DecimalDigitsPtr,
                                   SQLSMALLINT*      NullablePtr)
{
  struct ODBC_LIBRARY_APIS*   __odbc_apis    = ODBCConnection->__odbc_apis;
  struct ODBC_DRIVER_OPTIONS* __odbc_options = ODBCConnection->__odbc_options;
  
  if(__odbc_options->__mb_fintAvailable)
  { 
    return (__odbc_apis->__api_SQLDESCRIBEPARAM_INT)(StatementHandle,
                                                  ParameterNumber,
                                                  DataTypePtr,
                                                  ParameterSizePtr,
                                                  DecimalDigitsPtr,
                                                  NullablePtr);
  }
  else
  {
    SQLULEN __val_ParameterSize = 0;
    SQLRETURN __odbc_sqlreturn =  (__odbc_apis->__api_SQLDESCRIBEPARAM_LEN)(StatementHandle,
                                                                         ParameterNumber,
                                                                         DataTypePtr,
                                                                         &__val_ParameterSize,
                                                                         DecimalDigitsPtr,
                                                                         NullablePtr);
                                                
    *ParameterSizePtr = (SQLUINTEGER)__val_ParameterSize;   
    return __odbc_sqlreturn;  
  } 
}

SQLRETURN __odbc_SQLRowCount      (OdbcConnection*   ODBCConnection,
                                   SQLHSTMT          StatementHandle,
                                   SQLINTEGER *      RowCountPtr)
{
  struct ODBC_LIBRARY_APIS*   __odbc_apis    = ODBCConnection->__odbc_apis;
  struct ODBC_DRIVER_OPTIONS* __odbc_options = ODBCConnection->__odbc_options;
  
  if(__odbc_options->__mb_fintAvailable)
  { 
    return (__odbc_apis->__api_SQLROWCOUNT_INT)(StatementHandle,RowCountPtr);
  }
  else
  {
    SQLLEN __val_RowCount = 0;    
    SQLRETURN __odbc_sqlreturn = (__odbc_apis->__api_SQLROWCOUNT_LEN)(StatementHandle,&__val_RowCount);
    *RowCountPtr = (SQLINTEGER)__val_RowCount;    
    return __odbc_sqlreturn;   
  } 
}                                   


SQLRETURN __odbc_SQLBindCol       (OdbcConnection*   ODBCConnection,
                                   SQLHSTMT          StatementHandle,
                                   SQLUSMALLINT      ColumnNumber,
                                   SQLSMALLINT       TargetType,
                                   SQLPOINTER        TargetValuePtr,
                                   SQLLEN            BufferLength,
                                   SQLPOINTER        StrLen_or_Ind)
{
  struct ODBC_LIBRARY_APIS*   __odbc_apis    = ODBCConnection->__odbc_apis;
  struct ODBC_DRIVER_OPTIONS* __odbc_options = ODBCConnection->__odbc_options;

  SQLSMALLINT __target_type  = TargetType;
  
  if(__odbc_apis->__mi_driver_version < 0x0300)
  {  
    switch(TargetType)
    {
      case SQL_C_TYPE_DATE:
      {
        __target_type = SQL_C_DATE;
      }
      break;
//-----------------------------
      case SQL_C_TYPE_TIME:
      {
        __target_type = SQL_C_TIME;
      }
      break;          
//-----------------------------
      case SQL_C_TYPE_TIMESTAMP:
      {
        __target_type = SQL_C_TIMESTAMP;
      }
      break;
    }  
  }
  

  if(__odbc_options->__mb_fintAvailable)
  {
     return (__odbc_apis->__api_SQLBINDCOL_INT)(StatementHandle,
                                          ColumnNumber,
                                          __target_type,
                                          TargetValuePtr,
                                          (SQLINTEGER)BufferLength,
                                          (SQLINTEGER*)StrLen_or_Ind);
  }
  else
  {
     return (__odbc_apis->__api_SQLBINDCOL_LEN)(StatementHandle,
                                          ColumnNumber,
                                          __target_type,
                                          TargetValuePtr,
                                          BufferLength,
                                          (SQLLEN*)StrLen_or_Ind);
  }
}



SQLRETURN __odbc_SQLBindCol_INT   (OdbcConnection*   ODBCConnection,
                                   SQLHSTMT          StatementHandle,
                                   SQLUSMALLINT      ColumnNumber,
                                   SQLSMALLINT       TargetType,
                                   SQLPOINTER        TargetValuePtr,
                                   SQLINTEGER        BufferLength,
                                   SQLINTEGER*       StrLen_or_Ind)
{
  struct ODBC_LIBRARY_APIS* __odbc_apis = ODBCConnection->__odbc_apis;  
  SQLSMALLINT __target_type  = TargetType;
  
  if(__odbc_apis->__mi_driver_version < 0x0300)
  {  
    switch(TargetType)
    {
      case SQL_C_TYPE_DATE:
      {
        __target_type = SQL_C_DATE;
      }
      break;
//-----------------------------
      case SQL_C_TYPE_TIME:
      {
        __target_type = SQL_C_TIME;
      }
      break;          
//-----------------------------
      case SQL_C_TYPE_TIMESTAMP:
      {
        __target_type = SQL_C_TIMESTAMP;
      }
      break;
    }  
  }
  
  return (__odbc_apis->__api_SQLBINDCOL_INT)(StatementHandle,
                                          ColumnNumber,
                                          __target_type,
                                          TargetValuePtr,
                                          BufferLength,
                                          StrLen_or_Ind);
}  

SQLRETURN __odbc_SQLBindCol_LEN   (OdbcConnection*   ODBCConnection,
                                   SQLHSTMT          StatementHandle,
                                   SQLUSMALLINT      ColumnNumber,
                                   SQLSMALLINT       TargetType,
                                   SQLPOINTER        TargetValuePtr,
                                   SQLLEN            BufferLength,
                                   SQLLEN*           StrLen_or_Ind)
{
  struct ODBC_LIBRARY_APIS* __odbc_apis = ODBCConnection->__odbc_apis;  
  SQLSMALLINT __target_type  = TargetType;
  
  if(__odbc_apis->__mi_driver_version < 0x0300)
  {  
    switch(TargetType)
    {
      case SQL_C_TYPE_DATE:
      {
        __target_type = SQL_C_DATE;
      }
      break;
//-----------------------------
      case SQL_C_TYPE_TIME:
      {
        __target_type = SQL_C_TIME;
      }
      break;          
//-----------------------------
      case SQL_C_TYPE_TIMESTAMP:
      {
        __target_type = SQL_C_TIMESTAMP;
      }
      break;
    }  
  }
  

  return (__odbc_apis->__api_SQLBINDCOL_LEN)(StatementHandle,
                                          ColumnNumber,
                                          __target_type,
                                          TargetValuePtr,
                                          BufferLength,
                                          StrLen_or_Ind);
}              


SQLRETURN __odbc_SQLBindParameter    (OdbcConnection*   ODBCConnection,
                                      SQLHSTMT          StatementHandle,
                                      SQLUSMALLINT      ParameterNumber,
                                      SQLSMALLINT       InputOutputType,
                                      SQLSMALLINT       ValueType,
                                      SQLSMALLINT       ParameterType,
                                      SQLULEN           ColumnSize,
                                      SQLSMALLINT       DecimalDigits,
                                      SQLPOINTER        ParameterValuePtr,
                                      SQLLEN            BufferLength,
                                      SQLPOINTER        StrLen_or_IndPtr)
{
  struct ODBC_LIBRARY_APIS*   __odbc_apis    = ODBCConnection->__odbc_apis;
  struct ODBC_DRIVER_OPTIONS* __odbc_options = ODBCConnection->__odbc_options;
  
  SQLSMALLINT __valueType = ValueType;
  SQLSMALLINT __parameterType = ParameterType;

  if(__odbc_apis->__mi_driver_version < 0x0300)
  {  
    switch(ValueType)
    {
      case SQL_C_TYPE_DATE:
      {
        __valueType = SQL_C_DATE;
        __parameterType = SQL_DATE;
      }
      break;
//-----------------------------
      case SQL_C_TYPE_TIME:
      {
        __valueType = SQL_C_TIME;
        __parameterType = SQL_TIME;
      }
      break;          
//-----------------------------
      case SQL_C_TYPE_TIMESTAMP:
      {
        __valueType = SQL_C_TIMESTAMP;
        __parameterType = SQL_TIMESTAMP;
      }
      break;
    } 
  } 
 
  if(__odbc_options->__mb_fintAvailable)
  { 
     return (__odbc_apis->__api_SQLBINDPARAMETER_INT)(StatementHandle,
                                                ParameterNumber,
                                                InputOutputType,
                                                __valueType,
                                                __parameterType,
                                                ColumnSize,
                                                DecimalDigits,
                                                ParameterValuePtr,
                                                (SQLINTEGER)BufferLength,
                                                (SQLINTEGER*)StrLen_or_IndPtr);     
   }
   else
   {
     return (__odbc_apis->__api_SQLBINDPARAMETER_LEN)(StatementHandle,
                                                ParameterNumber,
                                                InputOutputType,
                                                __valueType,
                                                __parameterType,
                                                ColumnSize,
                                                DecimalDigits,
                                                ParameterValuePtr,
                                                BufferLength,
                                                (SQLLEN*)StrLen_or_IndPtr);

   }
}

__OPENCODE_END_NAMESPACE 
