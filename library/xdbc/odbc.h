/**
 *  odbc.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */

#ifndef __OPENCODE_XDBC_ODBC_H__
#define __OPENCODE_XDBC_ODBC_H__

#include "xdbc/xdefine.h"
#include "xdbc/sqltypes.h"
#include "xdbc/sql.h"
#include "xdbc/sqlext.h"
#include "xdbc/xmarco.h"
#include "xdbc/xdbc.h"
#include "xdbc/odbcdfn.h"

#include "xdbc/xtypes.h"
#include "port/systemLibrary.h"
#include "port/lifetime.h"

__OPENCODE_BEGIN_NAMESPACE

class OdbcConnection;

/** structure to contain the loaded options library entry points */
struct ODBC_DRIVER_OPTIONS
{
bool   __mb_fintAvailable;  // SQLINTEGER (true)  or SQLLEN (false) used in some special functions ?
bool   __mb_tbintAvailable; // support SQLBIGINT type
bool   __mb_fntsAvailable;  // set indicator to NTS
int    __mi_driver_unicode;
int    __mi_xlob_max;

SystemLibrary                        __options_syslib;

ODBCSQLOPTIONS_SUPPORTSCONNECTATTR       __options_SUPPORTSCONNECTATTR;  /* "odbcSupportsConnectAttr" */
ODBCSQLOPTIONS_SUPPORTSSTMTATTR          __options_SUPPORTSSTMTATTR;     /* "odbcSupportsStmtAttr" */
ODBCSQLOPTIONS_SUPPORTSENVATTR           __options_SUPPORTSENVATTR;      /* "odbcSupportsEnvAttr" */

ODBCSQLOPTIONS_ODBCGETTYPENAME           __options_ODBCGETTYPENAME;      /* "odbcGetTypeName" */
ODBCSQLOPTIONS_ADJUSTDEFINABLE           __options_ADJUSTDEFINABLE;      /* "odbcAdjustDefinable" */
ODBCSQLOPTIONS_ADJUSTBINDABLE            __options_ADJUSTBINDABLE;       /* "odbcAdjustBindable" */

ODBCSQLOPTIONS_ANSITOUNICODE             __options_ANSITOUNICODE;        /* "odbcAnsiToUnicode"  */ 
ODBCSQLOPTIONS_UNICODETOANSI             __options_UNICODETOANSI;        /* "odbcUnicodeToAnsi"  */

ODBCSQLOPTIONS_FORMATSQL                 __options_FORMATSQL;            /* "odbcFormatSQL"  */
};

/** structure to contain the loaded library entry points */
struct ODBC_LIBRARY_APIS
{
int  __mi_odbc_version;
int  __mi_driver_version;

/********************************************************************  Core Functions           */
ODBCSQLAPI_SQLALLOCCONNECT           __api_SQLALLOCCONNECT;         /* "SQLAllocConnect" */
ODBCSQLAPI_SQLALLOCENV               __api_SQLALLOCENV;             /* "SQLAllocEnv" */
ODBCSQLAPI_SQLALLOCSTMT              __api_SQLALLOCSTMT;            /* "SQLAllocStmt" */
ODBCSQLAPI_SQLFREECONNECT            __api_SQLFREECONNECT;          /* "SQLFreeConnect" */
ODBCSQLAPI_SQLFREEENV                __api_SQLFREEENV;              /* "SQLFreeEnv" */
ODBCSQLAPI_SQLFREESTMT               __api_SQLFREESTMT;             /* "SQLFreeStmt" */
ODBCSQLAPI_SQLBINDCOL_INT            __api_SQLBINDCOL_INT;          /* "SQLBindCol" */
ODBCSQLAPI_SQLBINDCOL_LEN            __api_SQLBINDCOL_LEN;          /* "SQLBindCol" */
ODBCSQLAPI_SQLDESCRIBECOL_INT        __api_SQLDESCRIBECOL_INT;      /* "SQLDescribeCol" */
ODBCSQLAPI_SQLDESCRIBECOL_LEN        __api_SQLDESCRIBECOL_LEN;      /* "SQLDescribeCol" */
ODBCSQLAPI_SQLDISCONNECT             __api_SQLDISCONNECT;           /* "SQLDisconnect" */
ODBCSQLAPI_SQLERROR                  __api_SQLERROR;                /* "SQLError" */
ODBCSQLAPI_SQLEXECDIRECT             __api_SQLEXECDIRECT;           /* "SQLExecDirect" */
ODBCSQLAPI_SQLEXECUTE                __api_SQLEXECUTE;              /* "SQLExecute" */
ODBCSQLAPI_SQLFETCH                  __api_SQLFETCH;                /* "SQLFetch" */
ODBCSQLAPI_SQLNUMRESULTCOLS          __api_SQLNUMRESULTCOLS;        /* "SQLNumResultCols" */
ODBCSQLAPI_SQLPREPARE                __api_SQLPREPARE;              /* "SQLPrepare" */
ODBCSQLAPI_SQLROWCOUNT_INT           __api_SQLROWCOUNT_INT;         /* "SQLRowCount" */
ODBCSQLAPI_SQLROWCOUNT_LEN           __api_SQLROWCOUNT_LEN;         /* "SQLRowCount" */
ODBCSQLAPI_SQLTRANSACT               __api_SQLTRANSACT;             /* "SQLTransact" */

/********************************************************************  Level 1 Functions        */
ODBCSQLAPI_SQLDRIVERCONNECT          __api_SQLDRIVERCONNECT;        /* "SQLDriverConnect" */
ODBCSQLAPI_SQLGETCONNECTOPTION       __api_SQLGETCONNECTOPTION;     /* "SQLGetConnectOption" */
ODBCSQLAPI_SQLSETCONNECTOPTION_INT   __api_SQLSETCONNECTOPTION_INT; /* "SQLSetConnectOption" */
ODBCSQLAPI_SQLSETCONNECTOPTION_LEN   __api_SQLSETCONNECTOPTION_LEN; /* "SQLSetConnectOption" */
ODBCSQLAPI_SQLSETSTMTOPTION_INT      __api_SQLSETSTMTOPTION_INT;    /* "SQLSetStmtOption" */
ODBCSQLAPI_SQLSETSTMTOPTION_LEN      __api_SQLSETSTMTOPTION_LEN;    /* "SQLSetStmtOption" */
ODBCSQLAPI_SQLGETINFO                __api_SQLGETINFO;              /* "SQLGetInfo" */

/********************************************************************  Level 2 Functions        */
ODBCSQLAPI_SQLDESCRIBEPARAM_INT      __api_SQLDESCRIBEPARAM_INT;    /* "SQLDescribeParam" */
ODBCSQLAPI_SQLDESCRIBEPARAM_LEN      __api_SQLDESCRIBEPARAM_LEN ;   /* "SQLDescribeParam" */
ODBCSQLAPI_SQLNUMPARAMS              __api_SQLNUMPARAMS;            /* "SQLNumParams" */
ODBCSQLAPI_SQLBINDPARAMETER_INT      __api_SQLBINDPARAMETER_INT;    /* "SQLBindParameter" */
ODBCSQLAPI_SQLBINDPARAMETER_LEN      __api_SQLBINDPARAMETER_LEN;    /* "SQLBindParameter" */
ODBCSQLAPI_SQLPARAMOPTIONS           __api_SQLPARAMOPTIONS;         /* "SQLParamOptions" */
ODBCSQLAPI_SQLPROCEDURECOLUMNS       __api_SQLPROCEDURECOLUMNS;     /* "SQLProcedureColumns" */

/********************************************************************  ODBC3.x Functions        */
ODBCSQLAPI_SQLALLOCHANDLE            __api_SQLALLOCHANDLE;          /* "SQLAllocHandle" */
ODBCSQLAPI_SQLFREEHANDLE             __api_SQLFREEHANDLE;           /* "SQLFreeHandle" */
ODBCSQLAPI_SQLGETDIAGREC             __api_SQLGETDIAGREC;           /* "SQLGetDiagRec" */
ODBCSQLAPI_SQLENDTRAN                __api_SQLENDTRAN;              /* "SQLEndTran" */
ODBCSQLAPI_SQLGETCONNECTATTR         __api_SQLGETCONNECTATTR;       /* "SQLGetConnectAttr" */
ODBCSQLAPI_SQLSETCONNECTATTR         __api_SQLSETCONNECTATTR;       /* "SQLSetConnectAttr" */
ODBCSQLAPI_SQLGETSTMTATTR            __api_SQLGETSTMTATTR;          /* "SQLGetStmtAttr" */
ODBCSQLAPI_SQLSETSTMTATTR            __api_SQLSETSTMTATTR;          /* "SQLSetStmtAttr" */
ODBCSQLAPI_SQLFETCHSCROLL            __api_SQLFETCHSCROLL;          /* "SQLFetchScroll" */
ODBCSQLAPI_SQLCLOSECURSOR            __api_SQLCLOSECURSOR;          /* "SQLCloseCursor" */
ODBCSQLAPI_SQLGETENVATTR             __api_SQLGETENVATTR;           /* "SQLGetEnvAttr" */
ODBCSQLAPI_SQLSETENVATTR             __api_SQLSETENVATTR;           /* "SQLSetEnvAttr" */


/********************************************************************  UNICODE Functions        */
ODBCSQLAPI_SQLDESCRIBECOLW_INT       __api_SQLDESCRIBECOLW_INT;     /* "SQLDescribeColW" */
ODBCSQLAPI_SQLDESCRIBECOLW_LEN       __api_SQLDESCRIBECOLW_LEN;     /* "SQLDescribeColW" */
ODBCSQLAPI_SQLGETCONNECTATTRW        __api_SQLGETCONNECTATTRW;      /* "SQLGetConnectAttrW" */
ODBCSQLAPI_SQLSETCONNECTATTRW        __api_SQLSETCONNECTATTRW;      /* "SQLSetConnectAttrW" */
ODBCSQLAPI_SQLGETSTMTATTRW           __api_SQLGETSTMTATTRW;         /* "SQLGetStmtAttrW" */
ODBCSQLAPI_SQLSETSTMTATTRW           __api_SQLSETSTMTATTRW;         /* "SQLSetStmtAttrW" */
ODBCSQLAPI_SQLGETDIAGRECW            __api_SQLGETDIAGRECW;          /* "SQLGetDiagRecW" */
ODBCSQLAPI_SQLDRIVERCONNECTW         __api_SQLDRIVERCONNECTW;       /* "SQLDriverConnectW" */
ODBCSQLAPI_SQLEXECDIRECTW            __api_SQLEXECDIRECTW;          /* "SQLExecDirectW" */
ODBCSQLAPI_SQLGETINFOW               __api_SQLGETINFOW;             /* "SQLGetInfoW" */
ODBCSQLAPI_SQLPREPAREW               __api_SQLPREPAREW;             /* "SQLPrepareW" */
ODBCSQLAPI_SQLPROCEDURECOLUMNSW      __api_SQLPROCEDURECOLUMNSW;    /* "SQLProcedureColumnsW" */
};

class __OPENCODE_XDBC_API_DECL OdbcDriver
{
public:
  ~OdbcDriver();

  OdbcDriver();
  OdbcDriver(const SystemLibrary& __odbc_syslib);

public:
  bool load() __XDBC_THROW_DECLARE(SQLException);

  SystemLibrary                __odbc_syslib;
  struct ODBC_LIBRARY_APIS*    __odbc_apis;
};

class __OPENCODE_XDBC_API_DECL OdbcDriverMapDoImpl
{
  DECLARE_LIFETIME_BEGIN(OdbcDriverMapDoImpl)
  DECLARE_LIFETIME_END(OdbcDriverMapDoImpl)
  
public:
  ~OdbcDriverMapDoImpl();
  OdbcDriverMapDoImpl();

public:
  struct ODBC_DRIVER_OPTIONS*  findOptions(const string& dsn,__XDBC_ATTRIBUTES& xdbc_attributes);
  void         registerOptions(const string& dsn,struct ODBC_DRIVER_OPTIONS* options);
  void         deregisterOptions(const string& dsn);

  OdbcDriver*  findDriver(const string& libpathname);
  void         registerDriver(const string& libpathname,OdbcDriver* driver);
  void         deregisterDriver(const string& libpathname);

private:
  OPENCODE_MAP<string,OdbcDriver*>                 __oc_map_drivers;
  OPENCODE_MAP<string,struct ODBC_DRIVER_OPTIONS*> __oc_map_options;
  Mutex                                            __oc_dm_mutex;
};

typedef SingletonHolder<OdbcDriverMapDoImpl,CreateUsingNew<OdbcDriverMapDoImpl>,SingletonWithLongevity<OdbcDriverMapDoImpl>,MultiThread<OdbcDriverMapDoImpl> > OdbcDriverMap;
__OPENCODE_INLINE unsigned int getLongevity(OdbcDriverMapDoImpl* impl){return __XDBC_LONGEVITY_ODBCDRIVERMAP;}

class __OPENCODE_XDBC_API_DECL OdbcDriverManager
{
public:
  static void getFunctions(__XDBC_HOOK_DECLARE_MORE const string& dsn,__XDBC_ATTRIBUTES& props,struct ODBC_DRIVER_OPTIONS*& odbcOptions,struct ODBC_LIBRARY_APIS*& odbcApis)__XDBC_THROW_DECLARE(SQLException);
  static struct ODBC_DRIVER_OPTIONS* loadOptions(__XDBC_HOOK_DECLARE_MORE OdbcDriver* odbcDriver,__XDBC_ATTRIBUTES& xdbc_attributes) __XDBC_THROW_DECLARE(SQLException);
};

class __OPENCODE_XDBC_API_DECL ODBC_VALUE
{
public:
  ODBC_VALUE();
  ODBC_VALUE(const ODBC_VALUE& rv);
  ODBC_VALUE& operator = (const ODBC_VALUE& rv);

public:
  union
  {
    SQLCHAR      odbc_TINYINT;
    SQLSMALLINT  odbc_SMALLINT;
    SQLINTEGER   odbc_INTEGER;
    SQLBIGINT    odbc_BIGINT;
    SQLREAL      odbc_REAL;
    SQLDOUBLE    odbc_DOUBLE;
  };

  SQL_NUMERIC_STRUCT odbc_NUMBERIC;
  TIMESTAMP_STRUCT   odbc_TIMESTAMP;
  DATE_STRUCT        odbc_DATE;
  TIME_STRUCT        odbc_TIME;
};

/**
 * Column name           Data type           Description
 * PROCEDURE_QUALIFIER   sysname             Procedure qualifier name. This column can be NULL.
 *
 * PROCEDURE_OWNER       sysname             Procedure owner name. This column always returns a value.
 *
 * PROCEDURE_NAME        nvarchar(134)       Procedure name. This column always returns a value.
 *
 * COLUMN_NAME           sysname             Column name for each column of the TABLE_NAME returned.
 *                                           This column always returns a value.
 *
 * COLUMN_TYPE           smallint            This field always returns a value:
 *                                           0 = SQL_PARAM_TYPE_UNKNOWN
 *                                           1 = SQL_PARAM_TYPE_INPUT
 *                                           2 = SQL_PARAM_TYPE_OUTPUT
 *                                           3 = SQL_RESULT_COL
 *                                           4 = SQL_PARAM_OUTPUT
 *                                           5 = SQL_RETURN_VALUE
 *
 * DATA_TYPE             smallint            Integer code for an ODBC data type. If this data type
 *                                           cannot be mapped to an SQL-92 type, the value is NULL.
 *                                           The native data type name is returned in the TYPE_NAME column.
 *
 * TYPE_NAME             sysname             String representation of the data type. This is the data type
 *                                           name as presented by the underlying DBMS.
 *
 * PRECISION             int                 Number of significant digits. The return value for
 *                                           the PRECISION column is in base 10.
 *
 * LENGTH                int                 Transfer size of the data.
 *
 * SCALE                 smallint            Number of digits to the right of the decimal point.
 *
 * RADIX                 smallint            Is the base for numeric types.
 *
 * NULLABLE              smallint            Specifies nullability:
 *                                           1 = Data type can be created allowing null values
 *                                           0 = Null values are not allowed
 *
 * REMARKS               varchar(254)        Description of the procedure column.
 *
 * COLUMN_DEF            nvarchar(4000)      Default value of the column.
 * SQL_DATA_TYPE         smallint            Value of the SQL data type as it appears in the TYPE
 *                                           field of the descriptor. This column is the same as the DATA_TYPE column,
 *                                           except for the datetime and SQL-92 interval data types.
 *                                           This column always returns a value.
 *
 * SQL_DATETIME_SUB      smallint            The datetime SQL-92 interval subcode if the value of SQL_DATA_TYPE
 *                                           is SQL_DATETIME or SQL_INTERVAL. For data types other than datetime
 *                                           and SQL-92 interval, this field is NULL.
 *
 * CHAR_OCTET_LENGTH     int                 Maximum length in bytes of a character or binary data type column.
 *                                           For all other data types, this column returns a NULL.
 *
 * ORDINAL_POSITION      int                 Ordinal position of the column in the table. The first column in
 *                                           the table is 1. This column always returns a value.
 *
 * IS_NULLABLE           varchar(254)        Nullability of the column in the table. ISO rules are followed to
 *                                           determine nullability. An ISO SQL-compliant DBMS cannot return an
 *                                           empty string. Displays YES if the column can include NULLS and NO
 *                                           if the column cannot include NULLS.This column returns a zero-length
 *                                           string if nullability is unknown.The value returned for this column
 *                                           is different from the value returned for the NULLABLE column.
 */

class __OPENCODE_XDBC_API_DECL ODBC_PARAM
{
public:
  ODBC_PARAM();
  ODBC_PARAM(const ODBC_PARAM& rv);
  ODBC_PARAM& operator=(const ODBC_PARAM& rv);

public:
  void dump(StringBuffer& __sb);

public:
  int         __mi_index;
  SQLINTEGER  __mi_length;
  SQLSMALLINT __mi_type;
  SQLINTEGER  __mi_precision;
  SQLSMALLINT __mi_scale;
  SQLSMALLINT __mi_nullable;

  string      __ms_name;
  string      __ms_schema_name;
  string      __ms_type_name;

  ODBC_VALUE  __mu_value;
  int         __mi_buf_offset;
  int         __mi_buf_size;
  SQLINTEGER  __mi_bind_size;
  bool        __mb_has_bound;
  SQLSMALLINT __mi_bind_type;
  SQLSMALLINT __mi_bind_c_type;

  SQLINTEGER  __mi_bind_ind_int;
  SQLLEN      __mi_bind_ind_len;

//-------------------------------------------------
// ADDED TO STOREDPROCEDURE
//-------------------------------------------------
  bool         __mb_stored_proc;
  string       __ms_catalog_name;
  string       __ms_proc_name;
  SQLSMALLINT  __mi_iomode;
  SQLSMALLINT  __mi_NUM_PREC_RADIX;   /* Either 10 or 2 or NULL */
  string       __ms_REMARK;
  string       __ms_COLUMN_DEF;
  SQLSMALLINT  __mi_SQL_DATA_TYPE;
  SQLSMALLINT  __mi_SQL_DATETIME_SUB;
  SQLINTEGER   __mi_CHAR_OCTET_LENGTH;
  SQLSMALLINT  __mi_IS_NULLABLE;

  string       __tmp_std_str;
};

class __OPENCODE_XDBC_API_DECL ODBC_COLUMN
{
public:
  ODBC_COLUMN();
  ODBC_COLUMN(const ODBC_COLUMN& rv);
  ODBC_COLUMN& operator=(const ODBC_COLUMN& rv);

public:
  void dump(StringBuffer& __sb);

public:
  int            __mi_index;

  SQLINTEGER     __mi_length;
  SQLSMALLINT    __mi_type;
  SQLSMALLINT    __mi_scale;
  SQLSMALLINT    __mi_nullable;

  int            __mi_precision;
  bool           __mb_char_semantics;

  string         __ms_name;
  string         __ms_schema_name;
  string         __ms_type_name;
  string         __ms_table_name;
};

class __OPENCODE_XDBC_API_DECL ODBC_DEFINE
{
public:
   ODBC_DEFINE();
   ODBC_DEFINE(const ODBC_DEFINE& rv);
   ODBC_DEFINE& operator=(const ODBC_DEFINE& rv);

public:
  ODBC_VALUE    __mu_value;
  int           __mi_buf_offset;
  int           __mi_buf_size;
  string        __tmp_std_str;

  int           __mi_define_size;
  SQLSMALLINT   __mi_define_type;
  SQLSMALLINT   __mi_define_c_type;

  SQLINTEGER    __mi_define_ind_int;
  SQLLEN        __mi_define_ind_len;
};

class __OPENCODE_XDBC_API_DECL OdbcResultSetCollection : public ResultSetCollection
{
public:
  virtual ~OdbcResultSetCollection() ;

  OdbcResultSetCollection();
  OdbcResultSetCollection(const OdbcResultSetCollection& rv);
  OdbcResultSetCollection& operator=(const OdbcResultSetCollection& rv);

public:
  char* __p_rowbuf;
  int   __rowbuf_size;

  vector<ODBC_DEFINE>       __vector_defines;
  vector<ODBC_COLUMN>       __vector_columns;
  OPENCODE_MAP<string,int>  __map_columns;

};

class __OPENCODE_XDBC_API_DECL OdbcConverter
{
public:
  static void setXdbcDate(XDBC_DATE& xdbcDate,const SQL_DATE_STRUCT& odbcDate);
  static void setOdbcDate(SQL_DATE_STRUCT& odbcDate,const XDBC_DATE& xdbcDate);

  static void setXdbcTime(XDBC_TIME& xdbcTime,const SQL_TIME_STRUCT& odbcTime);
  static void setOdbcTime(SQL_TIME_STRUCT& odbcTime,const XDBC_TIME& xdbcTime);

  static void setXdbcInterval(XDBC_INTERVAL& xdbcInterval,const SQL_INTERVAL_STRUCT& odbcInterval);
  static void setOdbcInterval(SQL_INTERVAL_STRUCT& odbcInterval,const XDBC_INTERVAL& xdbcInterval);

  static void setXdbcTimestamp(XDBC_TIMESTAMP& xdbcTimestamp,const SQL_TIMESTAMP_STRUCT& odbcTimestamp,ODBC_COLUMN* odbcColumn);
  static void setXdbcTimestamp(XDBC_TIMESTAMP& xdbcTimestamp,const SQL_TIMESTAMP_STRUCT& odbcTimestamp,ODBC_PARAM* odbcParam);
  static void setOdbcTimestamp(SQL_TIMESTAMP_STRUCT& odbcTimestamp,const XDBC_TIMESTAMP& xdbcTimestamp,ODBC_PARAM* odbcParam);

  static void setXdbcNumberic(XDBC_NUMERIC& xdbcNumberic,const SQL_NUMERIC_STRUCT& odbcNumberic);
  static void setOdbcNumberic(SQL_NUMERIC_STRUCT& odbcNumberic,const XDBC_NUMERIC& xdbcNumberic);
};

class __OPENCODE_XDBC_API_DECL OdbcKit
{
public:
  static void    checkException(__XDBC_HOOK_DECLARE_MORE OdbcConnection* odbc_conn,SQLHSTMT odbc_hstmt,SQLRETURN odbc_sqlreturn,const char* className = 0) __XDBC_THROW_DECLARE(SQLException);
  
  static bool    formatSQL(struct ODBC_DRIVER_OPTIONS* options,const string& inputSQL,string& outputSQL);

  static void    parseResultSetMetaData(__XDBC_HOOK_DECLARE_MORE OdbcConnection* odbc_conn,SQLHSTMT odbc_stmthp,vector<ODBC_COLUMN>& vector_columns) __XDBC_THROW_DECLARE(SQLException);
  static void    prepareVariableData(__XDBC_HOOK_DECLARE_MORE OdbcConnection* odbc_conn,SQLHSTMT odbc_stmthp,vector<ODBC_PARAM*>& __vector_params) __XDBC_THROW_DECLARE(SQLException);

  static void    adjustDefinable(OdbcConnection* odbc_conn,ODBC_COLUMN* odbc_column,ODBC_DEFINE* odbc_define);
  static void    adjustBindable(OdbcConnection* odbc_conn,ODBC_PARAM* odbc_param);
  
protected:
  static void fixColumnSize(OdbcConnection* odbc_conn,ODBC_COLUMN* odbc_column);
};

#include "xdbc/odbcmac.h"

__OPENCODE_END_NAMESPACE

#endif

