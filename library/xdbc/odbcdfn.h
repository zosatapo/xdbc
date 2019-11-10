/**
 *  odbcdfn.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */

#ifndef __OPENCODE_XDBC_ODBCDFN_H__
#define __OPENCODE_XDBC_ODBCDFN_H__

#include "xdbc/xdefine.h"
#include "xdbc/xmarco.h"
#include "xdbc/xdbc.h"
#include "xdbc/sqltypes.h"
#include "xdbc/sql.h"
#include "xdbc/sqlext.h"

#ifdef __cplusplus
extern "C" {                /* Assume C declarations for C++   */
#endif  /* __cplusplus */

/****************** Driver Options Functions ***************************************************/
/** odbcSupportsConnectAttr*/
typedef SQLRETURN (*ODBCSQLOPTIONS_SUPPORTSCONNECTATTR)(SQLINTEGER,SQLPOINTER,SQLINTEGER);                      

/** odbcSupportsStmtAttr*/
typedef SQLRETURN (*ODBCSQLOPTIONS_SUPPORTSSTMTATTR)(SQLINTEGER,SQLPOINTER,SQLINTEGER);  

/** odbcSupportsEnvAttr*/                       
typedef SQLRETURN (*ODBCSQLOPTIONS_SUPPORTSENVATTR)(SQLINTEGER,SQLPOINTER,SQLINTEGER);  

/** odbcGetTypeName*/                        
typedef SQLRETURN (*ODBCSQLOPTIONS_ODBCGETTYPENAME)(SQLINTEGER,SQLCHAR*,SQLINTEGER,SQLINTEGER*);

/** odbcAdjustDefinable*/ 
typedef SQLRETURN (*ODBCSQLOPTIONS_ADJUSTDEFINABLE)(SQLCHAR*,SQLPOINTER,SQLPOINTER);

/** odbcAdjustBindable*/ 
typedef SQLRETURN (*ODBCSQLOPTIONS_ADJUSTBINDABLE)(SQLCHAR*,SQLPOINTER);

/** odbcAnsiToUnicode */
typedef SQLINTEGER (*ODBCSQLOPTIONS_ANSITOUNICODE)(SQLPOINTER*,SQLINTEGER*,SQLINTEGER*,SQLINTEGER,SQLCHAR*, SQLINTEGER);

/** odbcUnicodeToAnsi */
typedef SQLINTEGER (*ODBCSQLOPTIONS_UNICODETOANSI)(SQLCHAR*,SQLINTEGER,SQLINTEGER,SQLCHAR*, SQLINTEGER);

/** odbcFormatSQL */
typedef SQLRETURN (*ODBCSQLOPTIONS_FORMATSQL)(SQLCHAR*,SQLINTEGER,SQLCHAR*, SQLINTEGER*);
                                                            
SQLRETURN  __default_odbcSupportsConnectAttr(SQLINTEGER Attribute,SQLPOINTER ValuePtr,SQLINTEGER  StringLength);
SQLRETURN  __default_odbcSupportsStmtAttr(SQLINTEGER Attribute,SQLPOINTER ValuePtr,SQLINTEGER  StringLength);
SQLRETURN  __default_odbcSupportsEnvAttr(SQLINTEGER Attribute,SQLPOINTER ValuePtr,SQLINTEGER  StringLength);

SQLRETURN  __default_odbcGetTypeName(SQLINTEGER OdbcTypeCode,SQLCHAR* Buffer,SQLINTEGER BufferLen,SQLINTEGER* BufferLenPtr);
SQLRETURN  __default_odbcAdjustDefinable(SQLCHAR* SQLText,SQLPOINTER OdbcColumnPtr,SQLPOINTER OdbcDefinePtr);
SQLRETURN  __default_odbcAdjustBindable(SQLCHAR* SQLText,SQLPOINTER OdbcParamPtr);

SQLINTEGER __default_odbcAnsiToUnicode(SQLPOINTER* UnicodeBufferP,SQLINTEGER* AllocLength,SQLINTEGER* AvailableLength,SQLINTEGER AllocType,SQLCHAR* AnsiBuffer, SQLINTEGER BufferLenth);
SQLINTEGER __default_odbcUnicodeToAnsi(SQLCHAR* AnsiBuffer,SQLINTEGER BufferLenth1,SQLINTEGER AllocType,SQLCHAR* UnicodeBuffer, SQLINTEGER BufferLenth2);

SQLRETURN __default_odbcFormatSQL(SQLCHAR* srcSQLBuffer,SQLINTEGER BufferLenth1,SQLCHAR* dstSQLBuffer, SQLINTEGER* BufferLenth2);

/********************************************************************  Core Functions           */       
typedef SQLRETURN (*ODBCSQLAPI_SQLALLOCCONNECT)(SQLHENV,SQLHDBC*);
typedef SQLRETURN (*ODBCSQLAPI_SQLALLOCENV)(SQLHENV*);
typedef SQLRETURN (*ODBCSQLAPI_SQLALLOCSTMT)(SQLHDBC,SQLHSTMT*);
typedef SQLRETURN (*ODBCSQLAPI_SQLFREECONNECT)(SQLHDBC);
typedef SQLRETURN (*ODBCSQLAPI_SQLFREEENV)(SQLHENV);
typedef SQLRETURN (*ODBCSQLAPI_SQLFREESTMT)(SQLHSTMT,SQLUSMALLINT);

typedef SQLRETURN (*ODBCSQLAPI_SQLBINDCOL_INT)(SQLHSTMT,SQLUSMALLINT,SQLSMALLINT,SQLPOINTER,SQLINTEGER,SQLINTEGER*);
typedef SQLRETURN (*ODBCSQLAPI_SQLBINDCOL_LEN)(SQLHSTMT,SQLUSMALLINT,SQLSMALLINT,SQLPOINTER,SQLLEN,SQLLEN*);

typedef SQLRETURN (*ODBCSQLAPI_SQLDESCRIBECOL_INT)(SQLHSTMT,SQLUSMALLINT,SQLCHAR*,SQLSMALLINT,SQLSMALLINT*,SQLSMALLINT*,SQLUINTEGER*,SQLSMALLINT*,SQLSMALLINT*);
typedef SQLRETURN (*ODBCSQLAPI_SQLDESCRIBECOL_LEN)(SQLHSTMT,SQLUSMALLINT,SQLCHAR*,SQLSMALLINT,SQLSMALLINT*,SQLSMALLINT*,SQLULEN*,SQLSMALLINT*,SQLSMALLINT*);

typedef SQLRETURN (*ODBCSQLAPI_SQLDISCONNECT)(SQLHDBC);
typedef SQLRETURN (*ODBCSQLAPI_SQLERROR)(SQLHENV,SQLHDBC,SQLHSTMT,SQLCHAR*,SQLINTEGER*,SQLCHAR*,SQLSMALLINT,SQLSMALLINT*);
typedef SQLRETURN (*ODBCSQLAPI_SQLEXECDIRECT)(SQLHSTMT,SQLCHAR*,SQLINTEGER);
typedef SQLRETURN (*ODBCSQLAPI_SQLEXECUTE)(SQLHSTMT);
typedef SQLRETURN (*ODBCSQLAPI_SQLFETCH)(SQLHSTMT);
typedef SQLRETURN (*ODBCSQLAPI_SQLNUMRESULTCOLS)(SQLHSTMT,SQLSMALLINT*);
typedef SQLRETURN (*ODBCSQLAPI_SQLPREPARE)(SQLHSTMT,SQLCHAR*,SQLINTEGER);

typedef SQLRETURN (*ODBCSQLAPI_SQLROWCOUNT_INT)(SQLHSTMT,SQLINTEGER*);
typedef SQLRETURN (*ODBCSQLAPI_SQLROWCOUNT_LEN)(SQLHSTMT,SQLLEN*);

typedef SQLRETURN (*ODBCSQLAPI_SQLTRANSACT)(SQLHENV,SQLHDBC,SQLUSMALLINT);


/********************************************************************  Level 1 Functions        */
typedef SQLRETURN (*ODBCSQLAPI_SQLDRIVERCONNECT)(SQLHDBC,SQLHWND,SQLCHAR*,SQLSMALLINT,SQLCHAR*,SQLSMALLINT,SQLSMALLINT*,SQLUSMALLINT);
typedef SQLRETURN (*ODBCSQLAPI_SQLGETCONNECTOPTION)(SQLHDBC,SQLUSMALLINT,SQLPOINTER);

typedef SQLRETURN (*ODBCSQLAPI_SQLSETCONNECTOPTION_INT)(SQLHDBC,SQLUSMALLINT,SQLUINTEGER);
typedef SQLRETURN (*ODBCSQLAPI_SQLSETCONNECTOPTION_LEN)(SQLHDBC,SQLUSMALLINT,SQLULEN);

typedef SQLRETURN (*ODBCSQLAPI_SQLSETSTMTOPTION_INT)(SQLHSTMT,SQLUSMALLINT,SQLUINTEGER);
typedef SQLRETURN (*ODBCSQLAPI_SQLSETSTMTOPTION_LEN)(SQLHSTMT,SQLUSMALLINT,SQLULEN);

typedef SQLRETURN (*ODBCSQLAPI_SQLGETINFO)(SQLHDBC,SQLUSMALLINT,SQLPOINTER,SQLSMALLINT,SQLSMALLINT*);

/********************************************************************  Level 2 Functions        */
typedef SQLRETURN (*ODBCSQLAPI_SQLDESCRIBEPARAM_INT)(SQLHSTMT,SQLUSMALLINT,SQLSMALLINT*,SQLUINTEGER*,SQLSMALLINT*,SQLSMALLINT*);
typedef SQLRETURN (*ODBCSQLAPI_SQLDESCRIBEPARAM_LEN)(SQLHSTMT,SQLUSMALLINT,SQLSMALLINT*,SQLULEN*,SQLSMALLINT*,SQLSMALLINT*);

typedef SQLRETURN (*ODBCSQLAPI_SQLNUMPARAMS)(SQLHSTMT,SQLSMALLINT*);

typedef SQLRETURN (*ODBCSQLAPI_SQLBINDPARAMETER_INT)(SQLHSTMT,SQLUSMALLINT,SQLSMALLINT,SQLSMALLINT,SQLSMALLINT,SQLUINTEGER,SQLSMALLINT,SQLPOINTER,SQLINTEGER,SQLINTEGER*);
typedef SQLRETURN (*ODBCSQLAPI_SQLBINDPARAMETER_LEN)(SQLHSTMT,SQLUSMALLINT,SQLSMALLINT,SQLSMALLINT,SQLSMALLINT,SQLULEN,SQLSMALLINT,SQLPOINTER,SQLLEN,SQLLEN*);

typedef SQLRETURN (*ODBCSQLAPI_SQLPARAMOPTIONS)(SQLHSTMT,SQLROWSETSIZE ,SQLROWSETSIZE*);

typedef SQLRETURN (*ODBCSQLAPI_SQLPROCEDURECOLUMNS)(SQLHSTMT,SQLCHAR*,SQLSMALLINT,SQLCHAR* ,SQLSMALLINT,SQLCHAR*,SQLSMALLINT,SQLCHAR*,SQLSMALLINT);


/********************************************************************  ODBC3.x Functions        */
typedef SQLRETURN (*ODBCSQLAPI_SQLALLOCHANDLE)(SQLSMALLINT,SQLHANDLE,SQLHANDLE*);
typedef SQLRETURN (*ODBCSQLAPI_SQLFREEHANDLE)(SQLSMALLINT,SQLHANDLE);
typedef SQLRETURN (*ODBCSQLAPI_SQLGETDIAGREC)(SQLSMALLINT,SQLHANDLE,SQLSMALLINT,SQLCHAR*,SQLINTEGER*,SQLCHAR*,SQLSMALLINT,SQLSMALLINT*);
typedef SQLRETURN (*ODBCSQLAPI_SQLENDTRAN)(SQLSMALLINT,SQLHANDLE,SQLSMALLINT);
typedef SQLRETURN (*ODBCSQLAPI_SQLGETCONNECTATTR)(SQLHDBC,SQLINTEGER,SQLPOINTER,SQLINTEGER,SQLINTEGER*);
typedef SQLRETURN (*ODBCSQLAPI_SQLSETCONNECTATTR)(SQLHDBC,SQLINTEGER,SQLPOINTER,SQLINTEGER);
typedef SQLRETURN (*ODBCSQLAPI_SQLGETSTMTATTR)(SQLHSTMT,SQLINTEGER,SQLPOINTER,SQLINTEGER,SQLINTEGER*);
typedef SQLRETURN (*ODBCSQLAPI_SQLSETSTMTATTR)(SQLHSTMT,SQLINTEGER,SQLPOINTER,SQLINTEGER);
typedef SQLRETURN (*ODBCSQLAPI_SQLFETCHSCROLL)(SQLHSTMT,SQLSMALLINT,SQLINTEGER);
typedef SQLRETURN (*ODBCSQLAPI_SQLCLOSECURSOR)(SQLHSTMT);                                      
typedef SQLRETURN (*ODBCSQLAPI_SQLGETENVATTR)(SQLHENV,SQLINTEGER,SQLPOINTER,SQLINTEGER,SQLINTEGER*);
typedef SQLRETURN (*ODBCSQLAPI_SQLSETENVATTR)(SQLHENV,SQLINTEGER,SQLPOINTER,SQLINTEGER);



/********************************************************************  UNICODE Functions        */
typedef SQLRETURN (*ODBCSQLAPI_SQLDESCRIBECOLW_INT)(SQLHSTMT,SQLUSMALLINT,SQLWCHAR*,SQLSMALLINT,SQLSMALLINT*,SQLSMALLINT*,SQLUINTEGER*,SQLSMALLINT*,SQLSMALLINT*);
typedef SQLRETURN (*ODBCSQLAPI_SQLDESCRIBECOLW_LEN)(SQLHSTMT,SQLUSMALLINT,SQLWCHAR*,SQLSMALLINT,SQLSMALLINT*,SQLSMALLINT*,SQLULEN*,SQLSMALLINT*,SQLSMALLINT*);

typedef SQLRETURN (*ODBCSQLAPI_SQLGETCONNECTATTRW)(SQLHDBC,SQLINTEGER,SQLPOINTER,SQLINTEGER,SQLINTEGER*);
typedef SQLRETURN (*ODBCSQLAPI_SQLSETCONNECTATTRW)(SQLHDBC,SQLINTEGER,SQLPOINTER,SQLINTEGER);

typedef SQLRETURN (*ODBCSQLAPI_SQLGETSTMTATTRW)(SQLHSTMT,SQLINTEGER,SQLPOINTER,SQLINTEGER,SQLINTEGER*);
typedef SQLRETURN (*ODBCSQLAPI_SQLSETSTMTATTRW)(SQLHSTMT,SQLINTEGER,SQLPOINTER,SQLINTEGER);

typedef SQLRETURN (*ODBCSQLAPI_SQLGETDIAGRECW)(SQLSMALLINT,SQLHANDLE,SQLSMALLINT,SQLWCHAR*,SQLINTEGER*,SQLWCHAR*,SQLSMALLINT,SQLSMALLINT*);
typedef SQLRETURN (*ODBCSQLAPI_SQLDRIVERCONNECTW)(SQLHDBC,SQLHWND,SQLWCHAR*,SQLSMALLINT,SQLWCHAR*,SQLSMALLINT,SQLSMALLINT*,SQLUSMALLINT);
typedef SQLRETURN (*ODBCSQLAPI_SQLEXECDIRECTW)(SQLHSTMT,SQLWCHAR*,SQLINTEGER);
typedef SQLRETURN (*ODBCSQLAPI_SQLGETINFOW)(SQLHDBC,SQLUSMALLINT,SQLPOINTER,SQLSMALLINT,SQLSMALLINT*);
typedef SQLRETURN (*ODBCSQLAPI_SQLPREPAREW)(SQLHSTMT,SQLWCHAR*,SQLINTEGER);
typedef SQLRETURN (*ODBCSQLAPI_SQLPROCEDURECOLUMNSW)(SQLHSTMT,SQLWCHAR*,SQLSMALLINT,SQLWCHAR* ,SQLSMALLINT,SQLWCHAR*,SQLSMALLINT,SQLWCHAR*,SQLSMALLINT);
           
#ifdef __cplusplus
}                                    /* End of extern "C" { */
#endif  /* __cplusplus */

__OPENCODE_BEGIN_NAMESPACE

class OdbcConnection;

SQLRETURN __odbc_SQLAllocHandle      (OdbcConnection*   ODBCConnection,
                                      SQLSMALLINT       HandleType,
                                      SQLHANDLE         InputHandle,
                                      SQLHANDLE*        OutputHandlePtr);
                                                        
SQLRETURN __odbc_SQLFreeHandle       (OdbcConnection*   ODBCConnection,
                                      SQLSMALLINT       HandleType,
                                      SQLHANDLE         Handle);
                                     
                                     
SQLRETURN __odbc_SQLEndTran          (OdbcConnection*   ODBCConnection,
                                      SQLSMALLINT       HandleType,
                                      SQLHANDLE         Handle,
                                      SQLSMALLINT       CompletionType);

SQLRETURN __odbc_SQLFetchScroll      (OdbcConnection*   ODBCConnection,
                                      SQLHSTMT          StatementHandle,
                                      SQLSMALLINT       FetchOrientation,
                                      SQLINTEGER        FetchOffset);

SQLRETURN __odbc_SQLCloseCursor      (OdbcConnection*   ODBCConnection,
                                      SQLHSTMT          StatementHandle);
                                     
                                     
SQLRETURN __odbc_SQLGetEnvAttr       (OdbcConnection*   ODBCConnection,
                                      SQLHENV           EnvironmentHandle,
                                      SQLINTEGER        Attribute,
                                      SQLPOINTER        ValuePtr,
                                      SQLINTEGER        BufferLength,
                                      SQLINTEGER*       StringLengthPtr);
                                     
SQLRETURN __odbc_SQLSetEnvAttr       (OdbcConnection*   ODBCConnection,
                                      SQLHENV           EnvironmentHandle,
                                      SQLINTEGER        Attribute,
                                      SQLPOINTER        ValuePtr,
                                      SQLINTEGER        StringLength,
                                      SQLPOINTER        AdditionValuePtr = 0,
                                      SQLINTEGER        AdditionStringLength = 0);                                                                          
                                     
SQLRETURN __odbc_SQLDescribeParam    (OdbcConnection*   ODBCConnection,
                                      SQLHSTMT          StatementHandle,
                                      SQLUSMALLINT      ParameterNumber,
                                      SQLSMALLINT*      DataTypePtr,
                                      SQLUINTEGER*      ParameterSizePtr,
                                      SQLSMALLINT*      DecimalDigitsPtr,
                                      SQLSMALLINT*      NullablePtr);
                                     
SQLRETURN __odbc_SQLRowCount         (OdbcConnection*   ODBCConnection,
                                      SQLHSTMT          StatementHandle,
                                      SQLINTEGER *      RowCountPtr);
                                      

SQLRETURN __odbc_SQLBindCol          (OdbcConnection*   ODBCConnection,
                                      SQLHSTMT          StatementHandle,
                                      SQLUSMALLINT      ColumnNumber,
                                      SQLSMALLINT       TargetType,
                                      SQLPOINTER        TargetValuePtr,
                                      SQLLEN            BufferLength,
                                      SQLPOINTER        StrLen_or_Ind);
                                      
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
                                      SQLPOINTER        StrLen_or_IndPtr);

//------------------------------------------------------------------------------------

SQLRETURN __odbc_SQLDescribeCol      (OdbcConnection*   ODBCConnection,
                                      SQLHSTMT          StatementHandle,
                                      SQLSMALLINT       ColumnNumber,
                                      SQLCHAR*          ColumnName,
                                      SQLSMALLINT       BufferLength,
                                      SQLSMALLINT*      NameLengthPtr,
                                      SQLSMALLINT*      DataTypePtr,
                                      SQLUINTEGER*      ColumnSizePtr,
                                      SQLSMALLINT*      DecimalDigitsPtr,
                                      SQLSMALLINT*      NullablePtr);

SQLRETURN __odbc_SQLGetConnectAttr   (OdbcConnection*   ODBCConnection,
                                      SQLHDBC           ConnectionHandle,
                                      SQLINTEGER        Attribute,
                                      SQLPOINTER        ValuePtr,
                                      SQLINTEGER        BufferLength,
                                      SQLINTEGER*       StringLengthPtr);
                                     
SQLRETURN __odbc_SQLSetConnectAttr   (OdbcConnection*   ODBCConnection,
                                      SQLHDBC           ConnectionHandle,
                                      SQLINTEGER        Attribute,
                                      SQLPOINTER        ValuePtr,
                                      SQLINTEGER        StringLength,
                                      SQLPOINTER        AdditionValuePtr = 0,
                                      SQLINTEGER        AdditionStringLength = 0);
                                     
SQLRETURN __odbc_SQLGetStmtAttr      (OdbcConnection*   ODBCConnection,
                                      SQLHSTMT          StatementHandle,
                                      SQLINTEGER        Attribute,
                                      SQLPOINTER        ValuePtr,
                                      SQLINTEGER        BufferLength,
                                      SQLINTEGER*       StringLengthPtr);
                                     
SQLRETURN __odbc_SQLSetStmtAttr      (OdbcConnection*   ODBCConnection,
                                      SQLHSTMT          StatementHandle,
                                      SQLINTEGER        Attribute,
                                      SQLPOINTER        ValuePtr,
                                      SQLINTEGER        StringLength,
                                      SQLPOINTER        AdditionValuePtr = 0,
                                      SQLINTEGER        AdditionStringLength = 0);

SQLRETURN __odbc_SQLGetDiagRec       (OdbcConnection*   ODBCConnection,
                                      SQLSMALLINT       HandleType,
                                      SQLHANDLE         Handle,
                                      SQLSMALLINT       RecNumber,
                                      SQLCHAR*          SQLState,
                                      SQLSMALLINT       StateLength,
                                      SQLSMALLINT*      StateLengthPtr,
                                      SQLINTEGER*       NativeErrorPtr,
                                      SQLCHAR*          MessageText,
                                      SQLSMALLINT       BufferLength,
                                      SQLSMALLINT*      TextLengthPtr);

SQLRETURN  __odbc_SQLDriverConnect   (OdbcConnection*   ODBCConnection,   
                                       SQLHDBC          ConnectionHandle,             
                                       SQLHWND          WindowHandle,                 
                                       SQLCHAR*         InConnectionString,          
                                       SQLSMALLINT      InConnectionStringLength,     
                                       SQLCHAR*         OutConnectionString,         
                                       SQLSMALLINT      OutConnectionStringCapacity,  
                                       SQLSMALLINT*     OutConnectionStringLengthPtr,
                                       SQLUSMALLINT     DriverCompletion);            
        
SQLRETURN   __odbc_SQLExecDirect     (OdbcConnection*   ODBCConnection, 
                                      SQLHSTMT          StatementHandle,
                                      SQLCHAR*          StatementText,
                                      SQLINTEGER        TextLength);

SQLRETURN   __odbc_SQLGetInfo       (OdbcConnection*   ODBCConnection,
                                      SQLHDBC           ConnectionHandle,  
                                      SQLUSMALLINT      InfoType,          
                                      SQLPOINTER        InfoValuePtr,      
                                      SQLSMALLINT       BufferLength,      
                                      SQLSMALLINT*      StringLengthPtr); 

SQLRETURN   __odbc_SQLPrepare        (OdbcConnection*   ODBCConnection, 
                                      SQLHSTMT          StatementHandle, 
                                      SQLCHAR*          StatementText,  
                                      SQLINTEGER        TextLength);     


SQLRETURN   __odbc_SQLProcedureColumns(OdbcConnection*   ODBCConnection, 
                                       SQLHSTMT          StatementHandle,
                                       SQLCHAR*          CatalogName,   
                                       SQLSMALLINT       NameLength1,    
                                       SQLCHAR*          SchemaName,    
                                       SQLSMALLINT       NameLength2,    
                                       SQLCHAR*          ProcName,      
                                       SQLSMALLINT       NameLength3,    
                                       SQLCHAR*          ColumnName,    
                                       SQLSMALLINT       NameLength4);   
                                                                                                                                                                                                                                                                                 
__OPENCODE_END_NAMESPACE               
                                     
#endif                               






