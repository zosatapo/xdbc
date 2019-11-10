#include "xdbc/odbcResultSet.h"
#include "xdbc/odbcConnection.h"
#include "xdbc/odbcStatement.h"
#include "xdbc/odbcResultSetMetaData.h"
#include "xdbc/odbcClob.h"
#include "xdbc/odbcBlob.h"
#include "xdbc/odbcTypes.h"
#include "xdbc/xsmart.h" 
#include "xdbc/XDBCUtilities.h" 
#include "port/utilities.h"
#include "port/ioc_sbuf.h"
#include "port/ioc_smart.h"
#include "port/libnumber.h"

__OPENCODE_BEGIN_NAMESPACE

#define __ODBCRESULTSET_CLASS_NAME   "OdbcResultSet"

OdbcResultSet::~OdbcResultSet()
{
  __XDBC_TRACE(__ODBCRESULTSET_CLASS_NAME,"destroy()");
}

OdbcResultSet::OdbcResultSet(OdbcConnection* odbc_conn,SQLImplementation* pXimpl,SQLHSTMT odbcHstmt,
              int resultSetType, int resultSetConcurrency,
              OdbcResultSetCollection* cachedCollection,bool adoptIt,
              bool selfDelete)  
{
   __mi_prefetch = 0;

   OdbcResultSet::resetResultSet(this,odbc_conn,pXimpl,odbcHstmt,
                               resultSetType,resultSetConcurrency,
                               cachedCollection,adoptIt,
                               selfDelete);
 
   __mb_fintAvailable      = __mp_conn->__odbc_options->__mb_fintAvailable;
   __api_SQLFETCH           = __mp_conn->__odbc_apis->__api_SQLFETCH; 
   __api_SQLFETCHSCROLL     = __mp_conn->__odbc_apis->__api_SQLFETCHSCROLL;    
}

ResultSetMetaData* OdbcResultSet::getMetaData(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException)
{
  return new OdbcResultSetMetaData(__cachedCollection->__vector_columns,__mp_conn->__odbc_options);
}

bool OdbcResultSet::next(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException)
{
  __ODBC_RSET_CHECKCLOSED(__mb_isClosed);

  SQLRETURN __odbc_sqlreturn = (__api_SQLFETCH)(odbc_hstmt);
  if(__odbc_sqlreturn == SQL_NO_DATA_FOUND)
  {
    __mi_cur_row = -1;
    return false;
  }

  if(__ODBC_HOOK_CHECK_FAILED(__odbc_sqlreturn))
  {
    OdbcKit::checkException(__XDBC_HOOK_INVOKE_MORE  __mp_conn,odbc_hstmt,__odbc_sqlreturn,__ODBCRESULTSET_CLASS_NAME);
    __XDBC_HOOK_CHECK(return false);
  }    
  
  ++__mi_cur_row;
  
  return true;
}


void OdbcResultSet::close()
{
  __XDBC_TRACE(__ODBCRESULTSET_CLASS_NAME,"close()");

  if(__mb_isClosed)
  {
    return;
  }

  __odbc_SQLCloseCursor(__mp_conn,odbc_hstmt);
   
  __mb_isClosed = true;   
   
}

void OdbcResultSet::destroy()
{
  if(__adoptIt && (__cachedCollection != 0))
  {
    delete __cachedCollection;
  }
    
  if(__mb_selfDelete)
  {
    delete this;
  }  
}

bool OdbcResultSet::wasNull(__XDBC_HOOK_DECLARE_MORE int columnIndex) __XDBC_THROW_DECLARE(SQLException)
{
   __ODBC_RSET_CHECKCOLUMN(columnIndex);
 
  if(__mi_cur_row == 0 || __mi_cur_row == -1)
  {
    __XDBC_HOOK_RETURN(return true,"[OdbcResultSet] no data indicated by current position,calling Traversal/Positioning first");
  }

  if(__mb_fintAvailable)
  {
    return (__cachedCollection->__vector_defines[columnIndex].__mi_define_ind_int == SQL_NULL_DATA);  
  }
  else
  {
    return (__cachedCollection->__vector_defines[columnIndex].__mi_define_ind_len == SQL_NULL_DATA);  
  }
}

//======================================================================
// Methods for accessing results by column index
//======================================================================

bool OdbcResultSet::getByte(__XDBC_HOOK_DECLARE_MORE int columnIndex,char& colval) __XDBC_THROW_DECLARE(SQLException)
{
  __ODBC_RSET_CHECKNULL_BOOLEAN(columnIndex);
  
  if( (__p_odbc_define->__mi_define_type == SQL_TINYINT) || (__p_odbc_define->__mi_define_type == SQL_BIT) )
  {
    colval = (char)__p_odbc_define->__mu_value.odbc_TINYINT;
    return true;
  }
    
  SQLCHAR __col_value = 0;
  if(checkConversion(__XDBC_HOOK_INVOKE_MORE columnIndex,__p_odbc_define,SQL_TINYINT,(char*)&__col_value))
  {
    colval = (char)(__col_value);
    return true;
  }
  
  return false;  
}

bool OdbcResultSet::getShort(__XDBC_HOOK_DECLARE_MORE int columnIndex,short& colval) __XDBC_THROW_DECLARE(SQLException)
{
  __ODBC_RSET_CHECKNULL_BOOLEAN(columnIndex);
  
  if(__p_odbc_define->__mi_define_type == SQL_SMALLINT)
  {
    colval = (short)__p_odbc_define->__mu_value.odbc_SMALLINT;
    return true;
  }
  
  SQLSMALLINT __col_value = 0;
  if(checkConversion(__XDBC_HOOK_INVOKE_MORE columnIndex,__p_odbc_define,SQL_SMALLINT,(char*)&__col_value))
  {
    colval = (short)(__col_value);
    return true;
  }
  
  return false;  
}

bool OdbcResultSet::getInt(__XDBC_HOOK_DECLARE_MORE int columnIndex,int& colval) __XDBC_THROW_DECLARE(SQLException)
{ 
  __ODBC_RSET_CHECKNULL_BOOLEAN(columnIndex);
 
  if(__p_odbc_define->__mi_define_type == SQL_INTEGER)
  {
    colval = (int)__p_odbc_define->__mu_value.odbc_INTEGER;
    return true;
  }
  
  SQLINTEGER   __col_value = 0;  
  if(checkConversion(__XDBC_HOOK_INVOKE_MORE columnIndex,__p_odbc_define,SQL_INTEGER,(char*)&__col_value))
  {
    colval = (int)__col_value;
    return true;
  }

  return false;  
}

bool OdbcResultSet::getLong(__XDBC_HOOK_DECLARE_MORE int columnIndex,XLONG& colval) __XDBC_THROW_DECLARE(SQLException)
{
  __ODBC_RSET_CHECKNULL_BOOLEAN(columnIndex);
  
  if(__p_odbc_define->__mi_define_type == SQL_BIGINT)
  {
    colval = (XLONG)__p_odbc_define->__mu_value.odbc_BIGINT;
    return true;
  }
  
  SQLBIGINT __col_value = 0;
  if(checkConversion(__XDBC_HOOK_INVOKE_MORE columnIndex,__p_odbc_define,SQL_BIGINT,(char*)&__col_value))
  {
    colval = (XLONG)__col_value;
    return true;
  }

  return false;
}

bool OdbcResultSet::getFloat(__XDBC_HOOK_DECLARE_MORE int columnIndex,float& colval) __XDBC_THROW_DECLARE(SQLException)
{
  __ODBC_RSET_CHECKNULL_BOOLEAN(columnIndex);
  
  if(__p_odbc_define->__mi_define_type == SQL_REAL)
  {
    colval = (float)__p_odbc_define->__mu_value.odbc_REAL;
    return true;
  }
  
  SQLREAL  __col_value = 0;
  if(checkConversion(__XDBC_HOOK_INVOKE_MORE columnIndex,__p_odbc_define,SQL_REAL,(char*)&__col_value))
  {
    colval = (float)__col_value;
    return true;
  }
 
  return false;  
}

bool OdbcResultSet::getDouble(__XDBC_HOOK_DECLARE_MORE int columnIndex,double& colval) __XDBC_THROW_DECLARE(SQLException)
{
  __ODBC_RSET_CHECKNULL_BOOLEAN(columnIndex);
  
  if(__p_odbc_define->__mi_define_type == SQL_DOUBLE)
  {
    colval = (double)__p_odbc_define->__mu_value.odbc_DOUBLE;
    return true;
  }
  
  SQLDOUBLE  __col_value = 0;
  if(checkConversion(__XDBC_HOOK_INVOKE_MORE columnIndex,__p_odbc_define,SQL_DOUBLE,(char*)&__col_value))
  {
    colval = (double)__col_value;
    return true;
  }
 
  return false;  
}

bool OdbcResultSet::getDate(__XDBC_HOOK_DECLARE_MORE int columnIndex,XDBC_DATE& colval) __XDBC_THROW_DECLARE(SQLException)
{
  __ODBC_RSET_CHECKNULL_BOOLEAN(columnIndex);
  
  if(__p_odbc_define->__mi_define_type == SQL_TYPE_DATE)
  {
    DATE_STRUCT& odbc_DATE = __p_odbc_define->__mu_value.odbc_DATE; 

    colval.year     = odbc_DATE.year;
    colval.month    = odbc_DATE.month;
    colval.day      = odbc_DATE.day;
    
    return true;
  }
  
  return checkConversion(__XDBC_HOOK_INVOKE_MORE columnIndex,__p_odbc_define,SQL_TYPE_DATE,(char*)&colval); 
}

bool OdbcResultSet::getTime(__XDBC_HOOK_DECLARE_MORE int columnIndex,XDBC_TIME& colval) __XDBC_THROW_DECLARE(SQLException)
{
  __ODBC_RSET_CHECKNULL_BOOLEAN(columnIndex);
  
  if(__p_odbc_define->__mi_define_type == SQL_TYPE_TIME)
  {
    TIME_STRUCT& odbc_TIME = __p_odbc_define->__mu_value.odbc_TIME;

    colval.hour     = odbc_TIME.hour;
    colval.minute   = odbc_TIME.minute;
    colval.second   = odbc_TIME.second;
    
    return true;
  }
  
  return checkConversion(__XDBC_HOOK_INVOKE_MORE columnIndex,__p_odbc_define,SQL_TYPE_TIME,(char*)&colval);  
}

bool OdbcResultSet::getTimestamp(__XDBC_HOOK_DECLARE_MORE int columnIndex,XDBC_TIMESTAMP& colval) __XDBC_THROW_DECLARE(SQLException)
{
  __ODBC_RSET_CHECKNULL_BOOLEAN(columnIndex);

  if(__p_odbc_define->__mi_define_type == SQL_TYPE_TIMESTAMP)
  {
    TIMESTAMP_STRUCT& odbc_TIMESTAMP = __p_odbc_define->__mu_value.odbc_TIMESTAMP;
    OdbcConverter::setXdbcTimestamp(colval,odbc_TIMESTAMP,&(__cachedCollection->__vector_columns[columnIndex]));
    return true;
  }
  
  return checkConversion(__XDBC_HOOK_INVOKE_MORE columnIndex,__p_odbc_define,SQL_TYPE_TIMESTAMP,(char*)&colval);  
}

bool OdbcResultSet::getInterval(__XDBC_HOOK_DECLARE_MORE int  columnIndex,XDBC_INTERVAL& colval) __XDBC_THROW_DECLARE(SQLException)
{
  __XDBC_HOOK_RETURN(return false,"getInterval not supported");  
}

const char*  OdbcResultSet::getString(__XDBC_HOOK_DECLARE_MORE int columnIndex,int& sizeBytes) __XDBC_THROW_DECLARE(SQLException)
{
  sizeBytes = 0;
  const char* start_charP = 0;
  
  __ODBC_RSET_CHECKNULL_POINTER(columnIndex); 
  
  ODBC_COLUMN* __p_odbc_column = &(__cachedCollection->__vector_columns[columnIndex]);
  
  switch(__p_odbc_define->__mi_define_type)
  {
    case SQL_CHAR:
    case SQL_VARCHAR:
    case SQL_LONGVARCHAR:
    {
      start_charP = __cachedCollection->__p_rowbuf + __p_odbc_define->__mi_buf_offset;
      __ODBC_GETDEFINABLE_IND(__p_odbc_define,sizeBytes);  
      return start_charP;     
    }
    break;
//-----------------------------
    case SQL_BINARY:
    case SQL_VARBINARY:
    case SQL_LONGVARBINARY:
    {
      start_charP = __cachedCollection->__p_rowbuf + __p_odbc_define->__mi_buf_offset;
      __ODBC_GETDEFINABLE_IND(__p_odbc_define,sizeBytes);  
      return start_charP; 
    }
    break;  
//-----------------------------
    default:
    {
    }
    break;  
  } 
  
  string& __tmp_str = __p_odbc_define->__tmp_std_str;  
  if(checkConversion(__XDBC_HOOK_INVOKE_MORE columnIndex,__p_odbc_define,SQL_VARCHAR,(char*)&__tmp_str))
  {
    sizeBytes = (int)__tmp_str.size();
    return __tmp_str.data();
  }

  return 0; 
}

Blob* OdbcResultSet::getBlob(__XDBC_HOOK_DECLARE_MORE int columnIndex) __XDBC_THROW_DECLARE(SQLException)
{
  __ODBC_RSET_CHECKNULL_POINTER(columnIndex);
  
  char* start = __cachedCollection->__p_rowbuf + __p_odbc_define->__mi_buf_offset;
  int ret_count = 0;
  __ODBC_GETDEFINABLE_IND(__p_odbc_define,ret_count);  
  
  Blob* __p_blob = new OdbcBlob();
  __p_blob->write(__XDBC_HOOK_INVOKE_MORE 1, start, 0, ret_count);  

  return __p_blob;
}

Clob* OdbcResultSet::getClob(__XDBC_HOOK_DECLARE_MORE int columnIndex) __XDBC_THROW_DECLARE(SQLException)
{
  __ODBC_RSET_CHECKNULL_POINTER(columnIndex);
  
  char* start = __cachedCollection->__p_rowbuf + __p_odbc_define->__mi_buf_offset;
  int ret_count = 0;
  __ODBC_GETDEFINABLE_IND(__p_odbc_define,ret_count);
 
  Clob* __p_clob = new OdbcClob();
  __p_clob->write(__XDBC_HOOK_INVOKE_MORE 1, start, 0, ret_count);  

  return __p_clob;
}

int OdbcResultSet::findColumn(__XDBC_HOOK_DECLARE_MORE const string& columnName) __XDBC_THROW_DECLARE(SQLException)
{
  if(__cachedCollection->__map_columns.empty())
  {
    for(int i = 1;i <= __mi_column_count; ++i)
    {
      __cachedCollection->__map_columns[StringsKit::toUpperCase(__cachedCollection->__vector_columns[i].__ms_name)] = i;
    }    
  }  
  
  OPENCODE_MAP<string,int>::iterator __pos = __cachedCollection->__map_columns.find(StringsKit::toUpperCase(columnName));
  if(__pos != __cachedCollection->__map_columns.end())
  {
    return __pos->second;
  } 

  StringBuffer __sb;
  __sb.append("[OdbcResultSet] Column '");
  __sb.append(columnName);
  __sb.append("' not found in SELECT-LIST");
  __XDBC_HOOK_RETURN(return -1,__sb.str());  
}

int OdbcResultSet::getType(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException)
{
  return __mi_resultSetType;
}

int OdbcResultSet::getConcurrency(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException)
{
  return __mi_resultSetConcurrency;
}

SQLImplementation* OdbcResultSet::getImplementation(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{
  return __p_ximpl;
}
  
int OdbcResultSet::getRow(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException)
{
  return __mi_cur_row;
}

bool OdbcResultSet::checkConversion(__XDBC_HOOK_DECLARE_MORE int columnIndex,ODBC_DEFINE* odbc_define,int outType,char* outBuffer)__XDBC_THROW_DECLARE(SQLException)
{
  ODBC_DEFINE* __p_odbc_define = odbc_define;
  if(__p_odbc_define == 0)
  {
    __p_odbc_define = &(__cachedCollection->__vector_defines[columnIndex]);
  }
  
  int inType =__p_odbc_define->__mi_define_type;
 
  if(inType == outType)
  {
    return false;
  }
  
  char* start=__cachedCollection->__p_rowbuf+__p_odbc_define->__mi_buf_offset;
  int count=__p_odbc_define->__mi_define_size;

  int ret_count = 0;
  __ODBC_GETDEFINABLE_IND(__p_odbc_define,ret_count);

  switch(inType)
  {
//----------------------------------------------------------------    
  case SQL_CHAR:
  case SQL_VARCHAR:
  case SQL_LONGVARCHAR:
  {
    if(outType == SQL_VARCHAR)
    {
      string* __p_string=(string*)outBuffer;
      if ( *(start+count) == 0 )
      {
        *__p_string = start;
      }
      else
      {
        __p_string->assign(start, count);
      }
      return true;
    }
  }
  break;
//----------------------------------------------------------------  
  case SQL_BINARY:
  case SQL_VARBINARY:
  case SQL_LONGVARBINARY:
  {
    if(outType == SQL_VARCHAR)
    {
      string* __p_string=(string*)outBuffer;
      __p_string->assign(start, ret_count);
      return true;
    }
  }
  break;
//----------------------------------------------------------------   
  case SQL_BIT:
  case SQL_TINYINT:
  {
    if(outType == SQL_VARCHAR)
    {
      string* __p_string=(string*)outBuffer;      
      *__p_string = Number::toString(__p_odbc_define->__mu_value.odbc_TINYINT);
        
      return true;
    }
    else if (outType == SQL_INTEGER)
    {
      SQLINTEGER *_sql_int = (SQLINTEGER *)outBuffer;
      *_sql_int = (SQLINTEGER)(__p_odbc_define->__mu_value.odbc_TINYINT);
      return true;
    }
    else if (outType == SQL_BIGINT)
    {
      SQLBIGINT* _big_int = (SQLBIGINT *)outBuffer;
      *_big_int = __p_odbc_define->__mu_value.odbc_TINYINT;
      
      return true;
    }
  }
  break;
//----------------------------------------------------------------  
  case SQL_SMALLINT:
  {
    if(outType == SQL_VARCHAR)
    {
      string* __p_string=(string*)outBuffer;      
      *__p_string = Number::toString(__p_odbc_define->__mu_value.odbc_SMALLINT);
      
      return true;
    }
    else if (outType == SQL_INTEGER)
    {
      SQLINTEGER *_sql_int = (SQLINTEGER *)outBuffer;
      *_sql_int = (SQLINTEGER)(__p_odbc_define->__mu_value.odbc_SMALLINT);
      return true;
    }
    else if (outType == SQL_BIGINT)
    {
      SQLBIGINT *_big_int = (SQLBIGINT *)outBuffer;     
      *_big_int = __p_odbc_define->__mu_value.odbc_SMALLINT;
      
      return true;
    }
  }
  break;
//----------------------------------------------------------------  
  case SQL_INTEGER:
  {
    if(outType == SQL_VARCHAR)
    {
      string* __p_string=(string*)outBuffer;
      *__p_string = Number::toString(__p_odbc_define->__mu_value.odbc_INTEGER);

      return true;
    }
    else if (outType == SQL_BIGINT)
    {
      SQLBIGINT *_big_int = (SQLBIGINT *)outBuffer;
      *_big_int = __p_odbc_define->__mu_value.odbc_INTEGER;
      
      return true;
    }
  }
  break;
//----------------------------------------------------------------  
  case SQL_BIGINT:
  {
    if(outType == SQL_VARCHAR)
    {
      string* __p_string=(string*)outBuffer;      
      *__p_string = Number::toString(__p_odbc_define->__mu_value.odbc_BIGINT);
      
      return true;
    }
    else if (outType == SQL_INTEGER)
    {
      SQLINTEGER *_sql_int = (SQLINTEGER *)outBuffer;
      *_sql_int = (SQLINTEGER)(__p_odbc_define->__mu_value.odbc_BIGINT);
      return true;
    }
  }
  break;
//----------------------------------------------------------------  
  case SQL_REAL:
  {
    if(outType == SQL_VARCHAR)
    {
      string* __p_string=(string*)outBuffer;      
      *__p_string = Number::toString(__p_odbc_define->__mu_value.odbc_REAL);
      
      return true;
    }
    else if (outType == SQL_BIGINT)
    {
      SQLBIGINT *_big_int = (SQLBIGINT *)outBuffer;
      *_big_int = (SQLBIGINT)__p_odbc_define->__mu_value.odbc_REAL;
      
      return true;
    }
  }
  break;
//----------------------------------------------------------------  
  case SQL_FLOAT:
  case SQL_DOUBLE:
  {
    if(outType == SQL_VARCHAR)
    {
      string* __p_string=(string*)outBuffer;
      *__p_string = Number::toString(__p_odbc_define->__mu_value.odbc_DOUBLE);

      return true;
    }
    else if (outType == SQL_INTEGER)
    {
      SQLINTEGER *_sql_int = (SQLINTEGER *)outBuffer;
      *_sql_int = (SQLINTEGER)__p_odbc_define->__mu_value.odbc_DOUBLE;
          
      return true;     
    }
    else if (outType == SQL_BIGINT)
    {
      SQLBIGINT *_big_int = (SQLBIGINT *)outBuffer;
      *_big_int = (SQLBIGINT)__p_odbc_define->__mu_value.odbc_DOUBLE;
      
      return true;
    }
    else if (outType == SQL_REAL)
    {
      SQLREAL *_sql_real = (SQLREAL *)outBuffer;
      *_sql_real = (SQLREAL)__p_odbc_define->__mu_value.odbc_DOUBLE;

      return true;
    }
    else if (outType == SQL_DOUBLE)
    {
      SQLDOUBLE *_sql_double = (SQLDOUBLE *)outBuffer;
      *_sql_double = __p_odbc_define->__mu_value.odbc_DOUBLE;

      return true;
    }
  }
  break;
//----------------------------------------------------------------   
  case SQL_NUMERIC:
  case SQL_DECIMAL:
  {
    XLONG __temp = 0;
    XLONG __temp_divisor = 1;
    char __temp_val[SQL_MAX_NUMERIC_LEN + 1];
    __temp_val[SQL_MAX_NUMERIC_LEN] = '\0';
    memcpy(__temp_val,__p_odbc_define->__mu_value.odbc_NUMBERIC.val,SQL_MAX_NUMERIC_LEN);  
                
    if(__p_odbc_define->__mu_value.odbc_NUMBERIC.val[0] != 0)
    {
   
      __temp = Number::parseLong(__temp_val,16);
      
      int __temp_scale = __p_odbc_define->__mu_value.odbc_NUMBERIC.scale;
      if(__temp_scale > 0)
      {
        for (int i = 0;i < __temp_scale; i++)  
        {
          __temp_divisor = __temp_divisor * 10;
        }
      }  
    }
    
    SQL_NUMERIC_STRUCT* __p_sql_numberic = &(__p_odbc_define->__mu_value.odbc_NUMBERIC); 

    if(outType == SQL_VARCHAR)
    {   
      string* __p_string=(string*)outBuffer;
      XDBC_NUMERIC __o_xdbc_numberic;
      XDBC_NUMERIC::init(&__o_xdbc_numberic);
      OdbcConverter::setXdbcNumberic(__o_xdbc_numberic,*__p_sql_numberic);  
      *__p_string = XDBC_NUMERIC::stringValue(&__o_xdbc_numberic);
      return true;
    }
    else if (outType == SQL_INTEGER)
    {
      SQLINTEGER *_sql_int = (SQLINTEGER *)outBuffer;
      XDBC_NUMERIC __o_xdbc_numberic;
      XDBC_NUMERIC::init(&__o_xdbc_numberic);
      OdbcConverter::setXdbcNumberic(__o_xdbc_numberic,*__p_sql_numberic);  
      *_sql_int = (SQLINTEGER)(XDBC_NUMERIC::longValue(&__o_xdbc_numberic));
      return true;     
    }
    else if (outType == SQL_BIGINT)
    {
      SQLBIGINT *_big_int = (SQLBIGINT *)outBuffer;
      XDBC_NUMERIC __o_xdbc_numberic;
      XDBC_NUMERIC::init(&__o_xdbc_numberic);
      OdbcConverter::setXdbcNumberic(__o_xdbc_numberic,*__p_sql_numberic);  
      *_big_int = (SQLBIGINT)(XDBC_NUMERIC::longValue(&__o_xdbc_numberic));      
      return true;
    }
    else if (outType == SQL_DOUBLE)
    {       
      SQLDOUBLE *_sql_double = (SQLDOUBLE *)outBuffer;
      XDBC_NUMERIC __o_xdbc_numberic;
      XDBC_NUMERIC::init(&__o_xdbc_numberic);
      OdbcConverter::setXdbcNumberic(__o_xdbc_numberic,*__p_sql_numberic);  
      *_sql_double = (SQLDOUBLE)(XDBC_NUMERIC::doubleValue(&__o_xdbc_numberic));      
      return true;
    }
    
  }
  break;
//----------------------------------------------------------------  
  case SQL_TYPE_DATE:
  {
    if(outType == SQL_VARCHAR)
    {
      string* __p_string=(string*)outBuffer;
      DATE_STRUCT& __tt_dt = __p_odbc_define->__mu_value.odbc_DATE; 
      XDBC_DATE __date;
      __date.year =  __tt_dt.year;
      __date.month = __tt_dt.month;
      __date.day = __tt_dt.day;
 
      *__p_string = XDBCUtilities::toString(__date);
      
      return true;
    }
  }
  break;
//----------------------------------------------------------------   
  case SQL_TYPE_TIME:
  {
    if(outType == SQL_VARCHAR)
    {
      string* __p_string = (string*)outBuffer;

      TIME_STRUCT& __tt_dt = __p_odbc_define->__mu_value.odbc_TIME;
      XDBC_TIME __time;
      __time.hour =  __tt_dt.hour;
      __time.minute = __tt_dt.minute;
      __time.second = __tt_dt.second;

      *__p_string = XDBCUtilities::toString(__time);
      
      return true;
    }
  }
  break;
//----------------------------------------------------------------  
  case SQL_TYPE_TIMESTAMP:
  {
    if(outType == SQL_VARCHAR)
    {
      string* __p_string = (string*)outBuffer;

      ODBC_COLUMN* __p_odbc_column = &(__cachedCollection->__vector_columns[columnIndex]);
      TIMESTAMP_STRUCT& __tt_dt = __p_odbc_define->__mu_value.odbc_TIMESTAMP;
      XDBC_TIMESTAMP __timestamp;

      OdbcConverter::setXdbcTimestamp(__timestamp,__tt_dt,__p_odbc_column);
      *__p_string = XDBCUtilities::toString(__timestamp);

      return true;
    }
    else if(outType == SQL_TYPE_DATE)
    {
      XDBC_DATE* __p_date = (XDBC_DATE*)outBuffer;

      TIMESTAMP_STRUCT& _ts = __p_odbc_define->__mu_value.odbc_TIMESTAMP;  

      __p_date->year     = _ts.year;
      __p_date->month    = _ts.month;
      __p_date->day      = _ts.day;

      return true;
    }

  }
  break;
//----------------------------------------------------------------  
    default:
    {
      if(outType == SQL_VARCHAR)
      {
        string* __p_string=(string*)outBuffer;
        __p_string->assign(start, ret_count);
        return true;
      }
    }
    break;
//----------------------------------------------------------------    
  }//~end switch(inType)

  {
    ODBC_COLUMN* __p_odbc_column = &(__cachedCollection->__vector_columns[columnIndex]);
    OdbcTypes* __odbcTypes = &(OdbcTypesImpl::getInstance());
    struct ODBC_DRIVER_OPTIONS* __odbc_options = __mp_conn->__odbc_options;
    
    StringBuffer __sb_0;
    __sb_0.append("Conversion not supported. columnName:");
    __sb_0.append("(");
    __sb_0.append(__p_odbc_column->__ms_name);
    __sb_0.append("),SRC_SQL_TYPE:");
    __sb_0.append(__p_odbc_column->__ms_type_name);
    __sb_0.append("(");
    __sb_0.append(inType);
    __sb_0.append("),DST_SQL_TYPE:");
    __sb_0.append(__odbcTypes->getTypeName(__odbc_options,outType));
    __sb_0.append("(");
    __sb_0.append(outType);
    __sb_0.append(")");
  
    StringBuffer __sb;
    __sb << "[OdbcResultSet] - " << __sb_0;
    __XDBC_HOOK_RETURN(return false,__sb.str());
  }
}

//---------------------------------------------------------------------
// Traversal/Positioning
//---------------------------------------------------------------------
  
bool OdbcResultSet::isBeforeFirst(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException)
{
  return (__mi_cur_row == 0);
}
    
bool OdbcResultSet::isAfterLast(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException)
{
  return (__mi_cur_row == -1);
}
 
bool OdbcResultSet::isFirst(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException)
{
  return (__mi_cur_row == 1);
}
 
bool OdbcResultSet::isLast(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException)
{
  int old_row_no=__mi_cur_row;
  last(__XDBC_HOOK_INVOKE_ONCE);
  __XDBC_HOOK_CHECK(return false);
 
  int new_row_no=__mi_cur_row;
  absolute(__XDBC_HOOK_INVOKE_MORE old_row_no); 
  __XDBC_HOOK_CHECK(return false);

  return (old_row_no == new_row_no);
}
 
bool OdbcResultSet::first(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException)
{
  __XDBC_HOOK_RETURN(return false,"first() not supported");
}
 
bool OdbcResultSet::last(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException)
{
  __XDBC_HOOK_RETURN(return false,"last() not supported");
}
  
bool OdbcResultSet::absolute(__XDBC_HOOK_DECLARE_MORE int row ) __XDBC_THROW_DECLARE(SQLException)
{
  __XDBC_HOOK_RETURN(return false,"absolute() not supported");
}
  
bool OdbcResultSet::relative(__XDBC_HOOK_DECLARE_MORE int rows ) __XDBC_THROW_DECLARE(SQLException)
{
  __XDBC_HOOK_RETURN(return false,"relative() not supported");
}
  
bool OdbcResultSet::previous(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException)
{
  __XDBC_HOOK_RETURN(return false,"previous() not supported");
}
  
void OdbcResultSet::setFetchSize(__XDBC_HOOK_DECLARE_MORE int rows) __XDBC_THROW_DECLARE(SQLException)
{
  if(rows < 0 )
  {
    return;
  }

  SQLRETURN   __odbc_sqlreturn = SQL_SUCCESS;

  SQLINTEGER __option = SQL_ATTR_MAX_ROWS ;
  SQLINTEGER __param = rows;
  __odbc_sqlreturn = __odbc_SQLSetStmtAttr(__mp_conn,odbc_hstmt,__option, (SQLPOINTER )__param,(SQLINTEGER)SQL_IS_INTEGER,(SQLPOINTER )&__param,(SQLINTEGER)SQL_IS_POINTER); 
  if(__ODBC_HOOK_CHECK_FAILED(__odbc_sqlreturn))
  {
    OdbcKit::checkException(__XDBC_HOOK_INVOKE_MORE  __mp_conn,odbc_hstmt,__odbc_sqlreturn,__ODBCRESULTSET_CLASS_NAME);
    __XDBC_HOOK_CHECK(return);
  } 
  
  __mi_prefetch = rows;
}

int OdbcResultSet::getFetchSize(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException)
{
  return __mi_prefetch;
}

bool OdbcResultSet::isSupported(__XDBC_HOOK_DECLARE_MORE int resultSetType, int resultSetConcurrency) __XDBC_THROW_DECLARE(SQLException)
{
  if( (resultSetType!=TYPE_FORWARD_ONLY) && (resultSetType!=TYPE_SCROLL_INSENSITIVE ))
  {
    StringBuffer __sb;
    __sb.append("not supported resultset type {");
    __sb.append(resultSetType);
    __sb.append("}");
    __XDBC_HOOK_RETURN(return false,__sb.str());
  }
  
  if(resultSetConcurrency!=CONCUR_READ_ONLY)
  {
    StringBuffer __sb;
    __sb.append("not supported concurrency mode {");
    __sb.append(resultSetConcurrency);
    __sb.append("}");
    __XDBC_HOOK_RETURN(return false,__sb.str());
  }

  return true;
}

void OdbcResultSet::prepareColumnData(__XDBC_HOOK_DECLARE_MORE OdbcConnection* odbc_conn,const string& odbcSQL,SQLHSTMT odbc_hstmt,OdbcResultSetCollection* cachedCollection) __XDBC_THROW_DECLARE(SQLException)
{
  OdbcResultSetCollection* __cachedCollection = cachedCollection;
  int __column_count = (int)(__cachedCollection->__vector_columns.size() - 1);   
  
  __cachedCollection->__vector_defines.resize(__column_count + 1);
  
  
  int __column_offset = 0;
  int __column_type = 0;
  ODBC_DEFINE* __odbc_define  = 0;
  ODBC_COLUMN* __odbc_column  = 0;
  size_t __oc_sizeof_void = sizeof(void*);
  SQLRETURN __odbc_sqlreturn  = SQL_ERROR;
  ODBCSQLOPTIONS_ADJUSTDEFINABLE __options_ADJUSTDEFINABLE = odbc_conn->__odbc_options->__options_ADJUSTDEFINABLE;
  
  for(int i = 1;i <= __column_count;++i)
  {
    __odbc_define = &(__cachedCollection->__vector_defines[i]);
    __odbc_column = &(__cachedCollection->__vector_columns[i]);
    
    __odbc_sqlreturn = (__options_ADJUSTDEFINABLE)((SQLCHAR*)odbcSQL.c_str(),(SQLPOINTER)__odbc_column,(SQLPOINTER)__odbc_define);
    if(__odbc_sqlreturn != SQL_SUCCESS)
    {
      OdbcKit::adjustDefinable(odbc_conn,__odbc_column,__odbc_define);
    }
          
    __column_type = __odbc_define->__mi_define_type;
    __odbc_define->__mi_buf_offset = __column_offset;
 
    switch(__column_type)
    {
      case SQL_CHAR:
      case SQL_VARCHAR:
      case SQL_LONGVARCHAR:
      case SQL_BINARY:
      case SQL_VARBINARY:
      case SQL_LONGVARBINARY:
      {       
        if(__odbc_column->__mb_char_semantics)
        {
          __odbc_define->__mi_buf_size=__odbc_define->__mi_buf_size * (4);
        }  
        else
        {
          __odbc_define->__mi_buf_size =__odbc_column->__mi_length;
        }   
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
        __odbc_define->__mi_buf_size = 0;
      }
      break;
  //-----------------------------
      default:
      {
        __odbc_define->__mi_buf_size =__odbc_column->__mi_length;
      }
      break;
              
    } //~end switch(__column_type) 
    
    if(__odbc_define->__mi_buf_size > odbc_conn->__odbc_options->__mi_xlob_max)
    {
    	__odbc_define->__mi_buf_size = odbc_conn->__odbc_options->__mi_xlob_max;
    }
    		
    __odbc_define->__mi_define_size = __odbc_define->__mi_buf_size;   
    if(__odbc_define->__mi_define_size != 0)
    {
      __odbc_define->__mi_define_size = (int)MemoryKit::roundUp((__odbc_define->__mi_buf_size + 1),__oc_sizeof_void);  
    }    
    
    __column_offset += __odbc_define->__mi_define_size;
    __cachedCollection->__rowbuf_size +=__odbc_define->__mi_define_size;
  }
  
  __cachedCollection->__p_rowbuf = new char[__cachedCollection->__rowbuf_size];
  memset(__cachedCollection->__p_rowbuf,0,__cachedCollection->__rowbuf_size);
}

void OdbcResultSet::defineColumnData(__XDBC_HOOK_DECLARE_MORE OdbcConnection* __mp_conn,SQLHSTMT odbc_hstmt,OdbcResultSetCollection* cachedCollection) __XDBC_THROW_DECLARE(SQLException)
{
  OdbcResultSetCollection* __cachedCollection = cachedCollection;
  int __column_count = (int)(__cachedCollection->__vector_columns.size() - 1);   
  
  char* buf_start = 0;
  int buf_size = 0;
  
  SQLSMALLINT data_type = 0;
  SQLRETURN __odbc_sqlreturn = SQL_SUCCESS;
  
  ODBC_DEFINE* __p_odbc_define = 0;
  SQLPOINTER __p_define_ind = (SQLPOINTER)0;

  for(int i = 1;i <= __column_count;++i)
  {
    __p_odbc_define = &(__cachedCollection->__vector_defines[i]);
   
    if(__mp_conn->__odbc_options->__mb_fintAvailable)
    {
       __p_define_ind = (SQLPOINTER)&(__p_odbc_define->__mi_define_ind_int);
    }  
    else
    {
       __p_define_ind = (SQLPOINTER)&(__p_odbc_define->__mi_define_ind_len);
    }
        
    switch(__p_odbc_define->__mi_define_type)
    {
      case SQL_BIT:
      case SQL_TINYINT:
      {
        __odbc_sqlreturn = __odbc_SQLBindCol(__mp_conn,odbc_hstmt, i, 
                                                 __p_odbc_define->__mi_define_c_type, 
                                                 &(__p_odbc_define->__mu_value.odbc_TINYINT),
                                                 (SQLINTEGER)sizeof(__p_odbc_define->__mu_value.odbc_TINYINT),
                                                 __p_define_ind);       
      }
      break; 
  //-----------------------------
      case SQL_SMALLINT:
      {
        __odbc_sqlreturn = __odbc_SQLBindCol(__mp_conn,odbc_hstmt, i, 
                                                 __p_odbc_define->__mi_define_c_type, 
                                                 &(__p_odbc_define->__mu_value.odbc_SMALLINT),
                                                 (SQLINTEGER)sizeof(__p_odbc_define->__mu_value.odbc_SMALLINT),
                                                 __p_define_ind);
  
      }
      break; 
  //-----------------------------
      case SQL_INTEGER:
      {
        __odbc_sqlreturn = __odbc_SQLBindCol(__mp_conn,odbc_hstmt, i, 
                                                 __p_odbc_define->__mi_define_c_type, 
                                                 &(__p_odbc_define->__mu_value.odbc_INTEGER),
                                                 (SQLINTEGER)sizeof(__p_odbc_define->__mu_value.odbc_INTEGER),
                                                 __p_define_ind);  
      }
      break; 
  //-----------------------------
      case SQL_BIGINT:
      {
        __odbc_sqlreturn = __odbc_SQLBindCol(__mp_conn,odbc_hstmt, i, 
                                                 __p_odbc_define->__mi_define_c_type, 
                                                 &(__p_odbc_define->__mu_value.odbc_BIGINT),
                                                 (SQLINTEGER)sizeof(__p_odbc_define->__mu_value.odbc_BIGINT),
                                                 __p_define_ind);
      }
      break; 
  //-----------------------------
      case SQL_REAL:
      {
        __odbc_sqlreturn = __odbc_SQLBindCol(__mp_conn,odbc_hstmt, i, 
                                                 __p_odbc_define->__mi_define_c_type, 
                                                 &(__p_odbc_define->__mu_value.odbc_REAL),
                                                 (SQLINTEGER)sizeof(__p_odbc_define->__mu_value.odbc_REAL),
                                                 __p_define_ind);
  
      }
      break; 
  //-----------------------------
      case SQL_FLOAT:
      case SQL_DOUBLE:
      {
        __odbc_sqlreturn = __odbc_SQLBindCol(__mp_conn,odbc_hstmt, i, 
                                                 __p_odbc_define->__mi_define_c_type, 
                                                 &(__p_odbc_define->__mu_value.odbc_DOUBLE),
                                                 (SQLINTEGER)sizeof(__p_odbc_define->__mu_value.odbc_DOUBLE),
                                                 __p_define_ind);
      }                                                       
      break; 
  //-----------------------------
      case SQL_NUMERIC:
      case SQL_DECIMAL:
      {
        __odbc_sqlreturn = __odbc_SQLBindCol(__mp_conn,odbc_hstmt, i, 
                                                 __p_odbc_define->__mi_define_c_type, 
                                                 &(__p_odbc_define->__mu_value.odbc_NUMBERIC),
                                                 (SQLINTEGER)sizeof(__p_odbc_define->__mu_value.odbc_NUMBERIC),
                                                 __p_define_ind);
      }
      break; 
  //-----------------------------
      case SQL_TYPE_DATE:
      {
         __odbc_sqlreturn = __odbc_SQLBindCol(__mp_conn,odbc_hstmt, i, 
                                                  __p_odbc_define->__mi_define_c_type, 
                                                  &(__p_odbc_define->__mu_value.odbc_DATE),
                                                  (SQLINTEGER)sizeof(__p_odbc_define->__mu_value.odbc_DATE),
                                                  __p_define_ind);
      }
      break; 
  //-----------------------------
      case SQL_TYPE_TIME:
      {
        __odbc_sqlreturn = __odbc_SQLBindCol(__mp_conn,odbc_hstmt, i, 
                                                 __p_odbc_define->__mi_define_c_type, 
                                                 &(__p_odbc_define->__mu_value.odbc_TIME),
                                                 (SQLINTEGER)sizeof(__p_odbc_define->__mu_value.odbc_TIME),
                                                 __p_define_ind);
      }                                                       
      break; 
  //-----------------------------
      case SQL_TYPE_TIMESTAMP:
      {
        __odbc_sqlreturn = __odbc_SQLBindCol(__mp_conn,odbc_hstmt, i, 
                                                 __p_odbc_define->__mi_define_c_type, 
                                                 &(__p_odbc_define->__mu_value.odbc_TIMESTAMP),
                                                 (SQLINTEGER)sizeof(__p_odbc_define->__mu_value.odbc_TIMESTAMP),
                                                 __p_define_ind);
      }
      break;                                          
  //-----------------------------
      default:
      {
        buf_start = __cachedCollection->__p_rowbuf + __p_odbc_define->__mi_buf_offset;
        buf_size = __p_odbc_define->__mi_define_size;
        
        //printf("OdbcResultSet::defineColumnData =>\n buf_start = %p , buf_size = %d\n",buf_start,buf_size);
        __odbc_sqlreturn = __odbc_SQLBindCol(__mp_conn,odbc_hstmt, i, 
                                                 __p_odbc_define->__mi_define_c_type, 
                                                 buf_start, 
                                                 buf_size, 
                                                 __p_define_ind);       
      }
      break;  
    }

    if(__ODBC_HOOK_CHECK_FAILED(__odbc_sqlreturn))
    {
      OdbcKit::checkException(__XDBC_HOOK_INVOKE_MORE  __mp_conn,odbc_hstmt,__odbc_sqlreturn,__ODBCRESULTSET_CLASS_NAME);        
      __XDBC_HOOK_CHECK(return);
    }  

  }//~end for(int i=1 ;i <= __column_count;++i)    
}

void OdbcResultSet::resetResultSet(OdbcResultSet* pResultSet,OdbcConnection* odbc_conn,SQLImplementation* pXimpl,SQLHSTMT odbcHstmt,
            int resultSetType, int resultSetConcurrency,
            OdbcResultSetCollection* cachedCollection,bool adoptIt,
            bool selfDelete)
{
  pResultSet->__mp_conn = odbc_conn;
  pResultSet->__p_ximpl = pXimpl;
  pResultSet->odbc_hstmt = odbcHstmt;
  
  pResultSet->__mi_resultSetType = resultSetConcurrency;  
  pResultSet->__cachedCollection = cachedCollection;
  pResultSet->__adoptIt = adoptIt;

  pResultSet->__mb_selfDelete = selfDelete;


  pResultSet->__mi_column_count = (int)(pResultSet->__cachedCollection->__vector_columns.size() - 1);

  pResultSet->__mb_isClosed = false;
  pResultSet->__mi_cur_row = 0;
  
}

__OPENCODE_END_NAMESPACE
