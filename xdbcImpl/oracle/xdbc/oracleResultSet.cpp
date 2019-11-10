#include "xdbc/oracleResultSet.h"
#include "xdbc/oracleConnection.h"
#include "xdbc/oracleResultSetMetaData.h"

#include "xdbc/oracleClob.h"
#include "xdbc/oracleBlob.h"
#include "xdbc/oracleConversion.h"
#include "xdbc/oracleTypes.h"
#include "xdbc/XDBCUtilities.h" 

#include "port/utilities.h"
#include "port/ioc_sbuf.h"
#include "port/ioc_smart.h"
#include "port/libnumber.h" 

__OPENCODE_BEGIN_NAMESPACE

#define ORACLERESULTSET_CLASS_NAME "OracleResultSet"

OracleResultSetCollection::~OracleResultSetCollection() 
{
  if(__p_rowbuf != 0)
  {
    delete []__p_rowbuf;
  }  
}
  
OracleResultSetCollection::OracleResultSetCollection()
{
  __p_rowbuf = 0;
  __rowbuf_size = 0;
}

OracleResultSetCollection::OracleResultSetCollection(const OracleResultSetCollection& rv)
{
  __vector_defines = rv.__vector_defines;
  __vector_columns = rv.__vector_columns;  
  __map_columns = rv.__map_columns; 
  
  __p_rowbuf = 0;
  __rowbuf_size = rv.__rowbuf_size;
  
  if(__rowbuf_size > 0)
  {
    __p_rowbuf = new char[__rowbuf_size];
    memcpy(__p_rowbuf,rv.__p_rowbuf,__rowbuf_size);
  }    
}

OracleResultSetCollection& OracleResultSetCollection::operator=(const OracleResultSetCollection& rv)
{
  if(this == &rv)
  {
    return *this;
  }  

  __vector_defines = rv.__vector_defines;
  __vector_columns = rv.__vector_columns;  
  __map_columns = rv.__map_columns; 
  
  if(__p_rowbuf != 0 )
  {
    delete []__p_rowbuf;
    __p_rowbuf = 0;
  }

  __rowbuf_size = rv.__rowbuf_size;
  
  if(__rowbuf_size > 0)
  {
    __p_rowbuf = new char[__rowbuf_size];
    memcpy(__p_rowbuf,rv.__p_rowbuf,__rowbuf_size);
  } 
    
  return *this;
}

//-------------------------------------------------------------------------

OracleResultSet::~OracleResultSet()
{
  __XDBC_TRACE(ORACLERESULTSET_CLASS_NAME,"destroy()");
}

OracleResultSet::OracleResultSet(OracleConnection* ora_conn,SQLImplementation* pXstmt,OCIStmt* ociStmthp,
            int resultSetType, int resultSetConcurrency,
            OracleResultSetCollection* cachedCollection,bool adoptIt,
            bool selfDelete)
{
   OracleResultSet::resetResultSet(this,ora_conn,pXstmt,ociStmthp,
                               resultSetType,resultSetConcurrency,
                               cachedCollection,adoptIt,
                               selfDelete);
}           

ResultSetMetaData* OracleResultSet::getMetaData(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{
  return new OracleResultSetMetaData(__cachedCollection->__vector_columns);
}

bool OracleResultSet::next(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{
  __XDBC_ORACLERSET_CHECKCLOSED(__mb_isClosed);

  sword  ret_code = OCIStmtFetch2(oci_stmthp,__mp_conn->oci_errhp,
                               (ub4)1,
                               OCI_FETCH_NEXT,
                               (ub4)0,
                               OCI_DEFAULT);

  if(ret_code == OCI_NO_DATA)
  {
    __mi_cur_row = -1;
    return false;
  }

  if(ret_code == OCI_SUCCESS_WITH_INFO)
  {
    text errbuf[10];
    sb4 errcode = 0;
    OCIErrorGet((dvoid *)__mp_conn->oci_errhp, (ub4) 1, (text *)0, &errcode,errbuf,10,OCI_HTYPE_ERROR);
    if(errcode != 24345)
    {
      OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLERESULTSET_CLASS_NAME);
      __XDBC_HOOK_CHECK(return false);
    }
  }
  else
  {
    OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLERESULTSET_CLASS_NAME);
    __XDBC_HOOK_CHECK(return false);
  }
 
  __mi_cur_row++;

  return true;
}


void OracleResultSet::close()
{
  __XDBC_TRACE(ORACLERESULTSET_CLASS_NAME,"close()");

  if(__mb_isClosed)
  {
    return;
  }
  
  __mb_isClosed=true; 
}

void OracleResultSet::destroy()
{
  if(__adoptIt && (__cachedCollection != 0))
  {
    __XDBC_HOOK_DEFINE();
    OracleResultSet::releaseColumnData(__XDBC_HOOK_INVOKE_MORE __mp_conn,oci_stmthp,__cachedCollection); 
    delete __cachedCollection;
  }
  
  if(__mb_selfDelete)
  {
    delete this;
  }
}
  
bool OracleResultSet::wasNull(__XDBC_HOOK_DECLARE_MORE int columnIndex)  __XDBC_THROW_DECLARE(SQLException)
{
   __XDBC_ORACLERSET_CHECKCOLUMN(columnIndex);
 
  if(__mi_cur_row==0 || __mi_cur_row==-1)
  {
    __XDBC_HOOK_RETURN(return false,"[OracleResultSet] no data indicated by current position,calling Traversal/Positioning first");
  }

  //printf("----- OracleResultSet::wasNull [%d]=>[%d]------\n",columnIndex,__cachedCollection->__vector_defines[columnIndex].__mi_define_ind);
  return __cachedCollection->__vector_defines[columnIndex].__mi_define_ind==-1;  
}

//======================================================================
// Methods for accessing results by column index
//======================================================================

bool OracleResultSet::getByte(__XDBC_HOOK_DECLARE_MORE int columnIndex,char& colval)  __XDBC_THROW_DECLARE(SQLException)
{
  __XDBC_ORACLERSET_CHECKNULL_BOOLEAN(columnIndex);
  
  if(__cachedCollection->__vector_columns[columnIndex].__mi_type == SQLT_INT)
  {
    colval = (char)__cachedCollection->__vector_defines[columnIndex].ora_value.ora_BIGINT;
    return true;
  }
    
  XLONG __col_value = 0;
  if(checkConversion(__XDBC_HOOK_INVOKE_MORE columnIndex,SQLT_INT,(char*)&__col_value))
  {
    colval = (char)(__col_value);
    return true;
  }
  
  return false;  
}

bool OracleResultSet::getShort(__XDBC_HOOK_DECLARE_MORE int columnIndex,short& colval)  __XDBC_THROW_DECLARE(SQLException)
{
  __XDBC_ORACLERSET_CHECKNULL_BOOLEAN(columnIndex);
  
  if(__cachedCollection->__vector_columns[columnIndex].__mi_type == SQLT_INT)
  {
    colval = (short)__cachedCollection->__vector_defines[columnIndex].ora_value.ora_BIGINT;
    return true;
  }
    
  XLONG __col_value = 0;
  if(checkConversion(__XDBC_HOOK_INVOKE_MORE columnIndex,SQLT_INT,(char*)&__col_value))
  {
    colval = (short)(__col_value);
    return true;
  }
  
  return false;  
}

bool OracleResultSet::getInt(__XDBC_HOOK_DECLARE_MORE int columnIndex,int& colval)  __XDBC_THROW_DECLARE(SQLException)
{
  __XDBC_ORACLERSET_CHECKNULL_BOOLEAN(columnIndex);
  
  if(__cachedCollection->__vector_columns[columnIndex].__mi_type == SQLT_INT)
  {
    colval = (int)__cachedCollection->__vector_defines[columnIndex].ora_value.ora_BIGINT;
    return true;
  }
    
  XLONG __col_value = 0;
  if(checkConversion(__XDBC_HOOK_INVOKE_MORE columnIndex,SQLT_INT,(char*)&__col_value))
  {
    colval = (int)(__col_value);
    return true;
  }
  
  return false;
}

bool OracleResultSet::getLong(__XDBC_HOOK_DECLARE_MORE int columnIndex,XLONG& colval)  __XDBC_THROW_DECLARE(SQLException)
{
  __XDBC_ORACLERSET_CHECKNULL_BOOLEAN(columnIndex);
  
  if(__cachedCollection->__vector_columns[columnIndex].__mi_type == SQLT_INT)
  {
    colval = (XLONG)__cachedCollection->__vector_defines[columnIndex].ora_value.ora_BIGINT;
    return true;
  }
    
  XLONG __col_value = 0;
  if(checkConversion(__XDBC_HOOK_INVOKE_MORE columnIndex,SQLT_INT,(char*)&__col_value))
  {
    colval = (XLONG)(__col_value);
    return true;
  }
  
  return false;
}

bool OracleResultSet::getFloat(__XDBC_HOOK_DECLARE_MORE int columnIndex,float& colval)  __XDBC_THROW_DECLARE(SQLException)
{
  __XDBC_ORACLERSET_CHECKNULL_BOOLEAN(columnIndex);
  
  if(__cachedCollection->__vector_columns[columnIndex].__mi_type == SQLT_FLT)
  {
    colval = (float)__cachedCollection->__vector_defines[columnIndex].ora_value.ora_DOUBLE;
    return true;
  }
    
  double __col_value = 0;
  if(checkConversion(__XDBC_HOOK_INVOKE_MORE columnIndex,SQLT_FLT,(char*)&__col_value))
  {
    colval = (float)(__col_value);
    return true;
  }
  
  return false; 
}

bool OracleResultSet::getDouble(__XDBC_HOOK_DECLARE_MORE int columnIndex,double& colval)  __XDBC_THROW_DECLARE(SQLException)
{
  __XDBC_ORACLERSET_CHECKNULL_BOOLEAN(columnIndex);
  
  if(__cachedCollection->__vector_columns[columnIndex].__mi_type == SQLT_FLT)
  {
    colval = (double)__cachedCollection->__vector_defines[columnIndex].ora_value.ora_DOUBLE;
    return true;
  }
    
  double __col_value = 0;
  if(checkConversion(__XDBC_HOOK_INVOKE_MORE columnIndex,SQLT_FLT,(char*)&__col_value))
  {
    colval = (double)(__col_value);
    return true;
  }
  
  return false; 
}

bool OracleResultSet::getDate(__XDBC_HOOK_DECLARE_MORE int columnIndex,XDBC_DATE& colval)  __XDBC_THROW_DECLARE(SQLException)
{
  __XDBC_ORACLERSET_CHECKNULL_BOOLEAN(columnIndex);
  
  if(__cachedCollection->__vector_columns[columnIndex].__mi_type == SQLT_DAT)
  {
    char* start = __cachedCollection->__p_rowbuf+__cachedCollection->__vector_defines[columnIndex].int_buf_offset;
    OracleConversion::parseDate(start,colval);

    return true;
  }
    
  return checkConversion(__XDBC_HOOK_INVOKE_MORE columnIndex,SQLT_DAT,(char*)&colval); 
}

bool OracleResultSet::getTime(__XDBC_HOOK_DECLARE_MORE int columnIndex,XDBC_TIME& colval)  __XDBC_THROW_DECLARE(SQLException)
{
  __XDBC_ORACLERSET_CHECKNULL_BOOLEAN(columnIndex);
  
  if(__cachedCollection->__vector_columns[columnIndex].__mi_type == SQLT_DAT)
  {
    char* start = __cachedCollection->__p_rowbuf+__cachedCollection->__vector_defines[columnIndex].int_buf_offset;
    OracleConversion::parseTime(start,colval);

    return true;
  }
   
  XDBC_TIMESTAMP __sql_ts;
  if(checkConversion(__XDBC_HOOK_INVOKE_MORE columnIndex,SQLT_TIMESTAMP,(char*)&__sql_ts))
  {
    colval.hour   = __sql_ts.hour;
    colval.minute = __sql_ts.minute;
    colval.second = __sql_ts.second;
            
    return true;
  }
  
  return false;
}

bool OracleResultSet::getTimestamp(__XDBC_HOOK_DECLARE_MORE int columnIndex,XDBC_TIMESTAMP& colval)  __XDBC_THROW_DECLARE(SQLException)
{
  __XDBC_ORACLERSET_CHECKNULL_BOOLEAN(columnIndex);
  
  if(__cachedCollection->__vector_columns[columnIndex].__mi_type == SQLT_TIMESTAMP)
  {
    OCIDateTime* ora_DATETIME=__cachedCollection->__vector_defines[columnIndex].ora_value.ora_DATETIME;
    OracleConversion::parseTimestamp(__XDBC_HOOK_INVOKE_MORE __mp_conn,ora_DATETIME,__cachedCollection->__vector_columns[columnIndex].__mi_type,colval);
    __XDBC_HOOK_CHECK(return false);
    
    return true;
  }
    
  return checkConversion(__XDBC_HOOK_INVOKE_MORE columnIndex,SQLT_TIMESTAMP,(char*)&colval); 
}

bool OracleResultSet::getInterval(__XDBC_HOOK_DECLARE_MORE int  columnIndex,XDBC_INTERVAL& colval)  __XDBC_THROW_DECLARE(SQLException)
{
  __XDBC_ORACLERSET_CHECKNULL_BOOLEAN(columnIndex);
  
  int outType=__cachedCollection->__vector_columns[columnIndex].__mi_type;
  
  if(checkConversion(__XDBC_HOOK_INVOKE_MORE columnIndex,outType,(char*)&colval))
  {
    return true;
  }
  __XDBC_HOOK_CHECK(return 0);
  
  OCIInterval* ora_INTERVAL=__cachedCollection->__vector_defines[columnIndex].ora_value.ora_INTERVAL;
  return OracleConversion::parseInterval(__XDBC_HOOK_INVOKE_MORE __mp_conn,ora_INTERVAL,outType,colval);
}

const char* OracleResultSet::getString(__XDBC_HOOK_DECLARE_MORE int columnIndex,int& sizeBytes)  __XDBC_THROW_DECLARE(SQLException)
{
  sizeBytes = 0;
  __XDBC_ORACLERSET_CHECKNULL_POINTER(columnIndex);
 
  OracleDefine* __oracle_define = &(__cachedCollection->__vector_defines[columnIndex]);
  int _col_type  = __cachedCollection->__vector_columns[columnIndex].__mi_type;
  char* start = 0;
  
  switch(_col_type)
  {
    case SQLT_CHR:
    case SQLT_AFC:
    case SQLT_AVC:
    case SQLT_STR:
    case SQLT_LNG:
    case SQLT_BIN:
    case SQLT_LBI:
    {
      sizeBytes  = __oracle_define->__mi_define_size;
      start = __cachedCollection->__p_rowbuf+__oracle_define->int_buf_offset;
      return start;
    }
    break;
    case SQLT_VCS:
    case SQLT_VBI:
    {
      XINT16 short_vcs_size = 0;
      memcpy((char*)&short_vcs_size,start,SHORT_PREFIX_LENGTH);
      sizeBytes = short_vcs_size;
      start = __cachedCollection->__p_rowbuf+__oracle_define->int_buf_offset + SHORT_PREFIX_LENGTH;
      return start;      
    }
    break;
    case SQLT_LVC:
    case SQLT_LVB:            
    {
      XINT32 int_vcs_size = 0;
      memcpy((char*)&int_vcs_size,start,INTEGER_PREFIX_LENGTH);
      sizeBytes = int_vcs_size;
      start = __cachedCollection->__p_rowbuf+__oracle_define->int_buf_offset + INTEGER_PREFIX_LENGTH;
      return start;
    }
    break;
  }
    
  string& __tmp_str = __oracle_define->__tmp_std_str;  
  if(checkConversion(__XDBC_HOOK_INVOKE_MORE columnIndex,SQLT_STR,(char*)&__tmp_str))
  {
    sizeBytes = (int)__tmp_str.size();
    return __tmp_str.data();
  }   
  else
  {
    sizeBytes  = __oracle_define->__mi_define_size; 
    start = __cachedCollection->__p_rowbuf+__oracle_define->int_buf_offset;    
    return start;
  }  
}

Blob* OracleResultSet::getBlob(__XDBC_HOOK_DECLARE_MORE int columnIndex)  __XDBC_THROW_DECLARE(SQLException)
{
  __XDBC_ORACLERSET_CHECKNULL_POINTER(columnIndex);
  
  OCILobLocator* ora_LOB=__cachedCollection->__vector_defines[columnIndex].ora_value.ora_LOB;

  OracleBlob Blob(__mp_conn,ora_LOB);
  if(checkConversion(__XDBC_HOOK_INVOKE_MORE columnIndex,SQLT_BLOB,(char*)&Blob))
  {
    OracleBlob* p_blob=new OracleBlob(__mp_conn,ora_LOB);
    p_blob->swap(Blob);

    return p_blob;
  }
  __XDBC_HOOK_CHECK(return 0);
  
  return new OracleBlob(__mp_conn,ora_LOB);
}

Clob* OracleResultSet::getClob(__XDBC_HOOK_DECLARE_MORE int columnIndex)  __XDBC_THROW_DECLARE(SQLException)
{
  __XDBC_ORACLERSET_CHECKNULL_POINTER(columnIndex);
  
  OCILobLocator* ora_LOB=__cachedCollection->__vector_defines[columnIndex].ora_value.ora_LOB;

  OracleClob Clob(__mp_conn,ora_LOB);
  if(checkConversion(__XDBC_HOOK_INVOKE_MORE columnIndex,SQLT_CLOB,(char*)&Clob))
  {
    OracleClob* p_clob = new OracleClob(__mp_conn,ora_LOB);
    p_clob->swap(Clob);

    return p_clob;
  }
  __XDBC_HOOK_CHECK(return 0);
  
  return new OracleClob(__mp_conn,ora_LOB);

}

//======================================================================
// Methods for accessing results by column name
//======================================================================

int OracleResultSet::findColumn(__XDBC_HOOK_DECLARE_MORE const string& columnName)  __XDBC_THROW_DECLARE(SQLException)
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
  __sb.append("[OracleResultSet] Column '");
  __sb.append(columnName);
  __sb.append("' not found in SELECT-LIST");

  __XDBC_HOOK_RETURN(return -1,__sb.str());  
}

int OracleResultSet::getType(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{
  return __mi_resultSetType;
}

int OracleResultSet::getConcurrency(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{
  return __mi_resultSetConcurrency;
}

SQLImplementation* OracleResultSet::getImplementation(__XDBC_HOOK_DECLARE_ONCE)   __XDBC_THROW_DECLARE(SQLException)
{
  return __p_xstmt;
}

bool OracleResultSet::checkConversion(__XDBC_HOOK_DECLARE_MORE int columnIndex,int outType,char* outBuffer) __XDBC_THROW_DECLARE(SQLException)
{
  OracleColumn* __oracle_column = &(__cachedCollection->__vector_columns[columnIndex]);
  OracleDefine* __oracle_define = &(__cachedCollection->__vector_defines[columnIndex]);
  
  int inType = __oracle_column->__mi_type;
 
  if(inType == outType)
  {
    return false;
  }

  int type=__oracle_column->__mi_type;


  char* start=__cachedCollection->__p_rowbuf + __oracle_define->int_buf_offset;
  int count=__oracle_define->__mi_define_size;

/*
  printf("__p_rowbuf = %p, int_buf_offset = %d, start = %p, count = %d\n",
          __cachedCollection->__p_rowbuf,
          __oracle_define->int_buf_offset,
          start,count);
*/

  switch(inType)
  {
//----------------------------------
    case SQLT_INT:
    {  
      if (outType == SQLT_STR)
      {
        string* __p_string=(string*)outBuffer;
        *__p_string = Number::toString(__oracle_define->ora_value.ora_BIGINT);
        
        return true;
      }
      else if(outType == SQLT_FLT)
      {
        double* __p_double=(double*)outBuffer;
        *__p_double = (double)(__oracle_define->ora_value.ora_BIGINT);
        
        return true;
      }
   }
   break;
     
//----------------------------------
   case SQLT_UIN:
   {
      if (outType == SQLT_STR)
      {
        string* __p_string=(string*)outBuffer;
        *__p_string = Number::toString(__oracle_define->ora_value.ora_UBIGINT);
        
        return true;
      }
      else if(outType == SQLT_INT)
      {        
        XLONG* __p_long = (XLONG*)outBuffer;
        *__p_long = __oracle_define->ora_value.ora_UBIGINT;
        
        return true;
      }
      else if(outType == SQLT_FLT)
      {
        double* __p_double=(double*)outBuffer;
        *__p_double = __oracle_define->ora_value.ora_UBIGINT;;
        
        return true;
      }
    }
    break;
//----------------------------------   
    case SQLT_BFLOAT:
    case SQLT_IBFLOAT:
    {
      if (outType == SQLT_STR)
      {
        string* __p_string=(string*)outBuffer;
        *__p_string = Number::toString(__oracle_define->ora_value.ora_FLOAT);
        
        return true;
      }
      else if(outType == SQLT_INT)
      {        
        XLONG* __p_long = (XLONG*)outBuffer;
        *__p_long = (XLONG)__oracle_define->ora_value.ora_FLOAT;
        
        return true;
      }
      else if(outType == SQLT_FLT)
      {
        double* __p_double=(double*)outBuffer;
        *__p_double = (double)__oracle_define->ora_value.ora_FLOAT;;
        
        return true;
      }
    }
    break;   
//----------------------------------
    case SQLT_FLT:
    case SQLT_BDOUBLE:
    case SQLT_IBDOUBLE:
    {
      if (outType == SQLT_STR)
      {
        string* __p_string=(string*)outBuffer;
        *__p_string = Number::toString(__oracle_define->ora_value.ora_DOUBLE);
        
        return true;
      }
      else if(outType == SQLT_INT)
      {        
        XLONG* __p_long = (XLONG*)outBuffer;
        *__p_long = (XLONG)__oracle_define->ora_value.ora_DOUBLE;
        
        return true;
      }
    }
    break;

//----------------------------------
    case SQLT_NUM:
    case SQLT_VNU:
    {
      OCINumber*  p_oci_number =  &__oracle_define->ora_value.ora_NUMBER;

      //printf("getLong : |%s|\n",MemoryKit::numDump((const char*)p_oci_number,sizeof(OCINumber)).c_str());

      if(outType == SQLT_STR)
      {
        string* __p_string=(string*)outBuffer;
        int  con_buf_size=1024;
        char con_buf[1024] = {0};

        int precision = __cachedCollection->__vector_columns[columnIndex].__mi_precision;
        int scale = __cachedCollection->__vector_columns[columnIndex].__mi_scale;

        //printf("precision = %d , scale = %d\n",precision,scale);
           
        if(precision <= 0)
        {
          precision = 38;
          if(scale > 0)
          {
            precision -= scale;
          }
        }

        string strFormat(precision,'9');
        if(scale > 0)
        {
          string strScale(scale,'9');
          strFormat.append(".");
          strFormat.append(strScale);
        }

        const char* fmt = strFormat.c_str();
        int   fmt_size = (int)strFormat.size();

        sword ret_code = OCINumberToText(__mp_conn->oci_errhp,p_oci_number,(const text*)fmt,(ub4)fmt_size,(const text*)0,(ub4)0,(ub4*)&con_buf_size,(text*)con_buf);
        OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLERESULTSET_CLASS_NAME);
        __XDBC_HOOK_CHECK(return false);
        
        __p_string->assign(StringsKit::trim(con_buf));

        return true;
      }
      else if(outType == SQLT_INT)
      {
        XLONG* __p_long=(XLONG*)outBuffer;
        sword ret_code = OCINumberToInt(__mp_conn->oci_errhp,p_oci_number,sizeof(XLONG),OCI_NUMBER_SIGNED,(dvoid*)__p_long);
        OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLERESULTSET_CLASS_NAME);
        __XDBC_HOOK_CHECK(return false);
                
        return true;
      }
      else if(outType == SQLT_FLT)
      {
        double* __p_double = (double*)outBuffer;
        sword ret_code = OCINumberToReal(__mp_conn->oci_errhp,p_oci_number,sizeof(double),(dvoid*)__p_double);
        OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLERESULTSET_CLASS_NAME);
        __XDBC_HOOK_CHECK(return false);
                
        return true;
      }
   }
   break;

//----------------------------------
   case SQLT_DAT:
   {    
     if(outType == SQLT_STR)
     {
       string* __p_string = (string*)outBuffer;       
       XDBC_DATE __sql_date;
       XDBC_DATE::init(&__sql_date);
        
       OracleConversion::parseDate(start,__sql_date);   
       *__p_string = XDBCUtilities::toString(__sql_date);
       
       return true;
     }
     else if(outType == SQLT_TIMESTAMP)
     {
       XDBC_TIMESTAMP* __p_ts =(XDBC_TIMESTAMP*)outBuffer;      
       OracleConversion::parseTimestamp(start,*__p_ts);
          
       return true;
     }
   }  
   break;

//----------------------------------
    case SQLT_CHR:
    case SQLT_AFC:
    case SQLT_AVC:
    case SQLT_STR:
    {
      if(outType == SQLT_STR)
      {   
        return false;
      }

    }   
    break;

//----------------------------------
    case SQLT_LNG:
    case SQLT_BIN:
    case SQLT_LBI:
    {
      if(outType == SQLT_STR)
      {
        return true;
      }

    }
    break;

//----------------------------------
    case SQLT_VCS:
    case SQLT_VBI:
    {
      if(outType == SQLT_STR)
      {
        return false;        
      }
    }
    break;


//----------------------------------
    case SQLT_LVC:
    case SQLT_LVB:
    {
      if(outType == SQLT_STR)
      {
        return false;
      }
    }
    break;

//----------------------------------    
    case SQLT_TIMESTAMP:
    {
      OCIDateTime* ora_DATETIME = __oracle_define->ora_value.ora_DATETIME;

      if(outType == SQLT_STR)
      {
        string* __p_string = (string*)outBuffer;

        XDBC_TIMESTAMP __sql_ts;
        XDBC_TIMESTAMP::init(&__sql_ts);
          
        OracleConversion::parseTimestamp(__XDBC_HOOK_INVOKE_MORE __mp_conn,ora_DATETIME,inType,__sql_ts);   
        __XDBC_HOOK_CHECK(return false);
        *__p_string = XDBCUtilities::toString(__sql_ts);
       
        return true;
      }
      else if(outType == SQLT_DAT)
      {
        XDBC_DATE* __p_sql_date = (XDBC_DATE*)outBuffer;
        OracleConversion::parseDate(__XDBC_HOOK_INVOKE_MORE __mp_conn,ora_DATETIME,inType,*__p_sql_date);   
        __XDBC_HOOK_CHECK(return false);
        
        return true;
      }
    }
    break;

//----------------------------------
    case SQLT_DATE:
    {
      OCIDateTime* ora_DATETIME=__oracle_define->ora_value.ora_DATETIME;

      if(outType == SQLT_STR)
      {
        string* __p_string =(string*)outBuffer;

        XDBC_DATE __sql_date;
        XDBC_DATE::init(&__sql_date);
          
        OracleConversion::parseDate(__XDBC_HOOK_INVOKE_MORE __mp_conn,ora_DATETIME,inType,__sql_date);   
        __XDBC_HOOK_CHECK(return false);
        *__p_string = XDBCUtilities::toString(__sql_date);
       
        return true;
      }
      else if(outType == SQLT_TIMESTAMP)
      {
        XDBC_TIMESTAMP* __p_sql_ts = (XDBC_TIMESTAMP*)outBuffer;
        OracleConversion::parseTimestamp(__XDBC_HOOK_INVOKE_MORE __mp_conn,ora_DATETIME,inType,*__p_sql_ts); 
        __XDBC_HOOK_CHECK(return false);
        
        return true;
      }
      else if(outType == SQLT_DAT)
      {
        XDBC_DATE* __p_sql_date = (XDBC_DATE*)outBuffer;
        OracleConversion::parseDate(__XDBC_HOOK_INVOKE_MORE __mp_conn,ora_DATETIME,inType,*__p_sql_date); 
        __XDBC_HOOK_CHECK(return false);
        
        return true;
      }
    }
    break;

//----------------------------------
    case SQLT_TIMESTAMP_TZ:
    case SQLT_TIMESTAMP_LTZ:
    {
      OCIDateTime* ora_DATETIME=__oracle_define->ora_value.ora_DATETIME;
      
      if(outType == SQLT_STR)
      {
        string* __p_string = (string*)outBuffer;

        XDBC_TIMESTAMP __sql_ts;
        XDBC_TIMESTAMP::init(&__sql_ts);
          
        OracleConversion::parseTimestamp(__XDBC_HOOK_INVOKE_MORE __mp_conn,ora_DATETIME,inType,__sql_ts);   
        __XDBC_HOOK_CHECK(return false);
        *__p_string = XDBCUtilities::toString(__sql_ts);

        return true;
      }
      else if(outType == SQLT_TIMESTAMP)
      {
        XDBC_TIMESTAMP* __p_sql_ts = (XDBC_TIMESTAMP*)outBuffer;
        OracleConversion::parseTimestamp(__XDBC_HOOK_INVOKE_MORE __mp_conn,ora_DATETIME,inType,*__p_sql_ts);  
        __XDBC_HOOK_CHECK(return false);
        
        return true;
      }
      else if(outType == SQLT_DAT)
      {
        XDBC_DATE* __p_sql_date = (XDBC_DATE*)outBuffer;
        OracleConversion::parseDate(__XDBC_HOOK_INVOKE_MORE __mp_conn,ora_DATETIME,inType,*__p_sql_date);   
        __XDBC_HOOK_CHECK(return false);
        
        return true;
      }
    }
    break;

//----------------------------------
    case SQLT_INTERVAL_YM:
    {
      OCIInterval* ora_INTERVAL = __oracle_define->ora_value.ora_INTERVAL;

      if(outType == SQLT_STR)
      {
        string* __p_string = (string*)outBuffer;
        
        XDBC_INTERVAL __interval;
        XDBC_INTERVAL::init(&__interval);
          
        OracleConversion::parseInterval(__XDBC_HOOK_INVOKE_MORE __mp_conn,ora_INTERVAL,inType,__interval);
        __XDBC_HOOK_CHECK(return false);
        
        *__p_string = XDBCUtilities::toString(__interval);
        
        return true;
      }
    }
    break;

//----------------------------------
    case SQLT_INTERVAL_DS:
    {
      OCIInterval* ora_INTERVAL=__oracle_define->ora_value.ora_INTERVAL;

      if(outType == SQLT_STR)
      {
        string* __p_string = (string*)outBuffer;
        
        XDBC_INTERVAL __interval;
        XDBC_INTERVAL::init(&__interval);
          
        OracleConversion::parseInterval(__XDBC_HOOK_INVOKE_MORE __mp_conn,ora_INTERVAL,inType,__interval);
        __XDBC_HOOK_CHECK(return false);
        *__p_string = XDBCUtilities::toString(__interval);
        
        return true;
      }
    }
    break;//----------------------------------
    case SQLT_CLOB:
    {
      if(outType == SQLT_STR)
      {
        string* __p_string = (string*)outBuffer;

        ub4 lenp=0;
        sword ret_code = OCILobGetLength(__mp_conn->oci_svchp,
                                       __mp_conn->oci_errhp,
                                       __oracle_define->ora_value.ora_LOB,
                                       &lenp);

        OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLERESULTSET_CLASS_NAME);
        __XDBC_HOOK_CHECK(return false);
                
        int x_size = lenp * ORACLE_CHAR_TO_BYTE_MAX;
        string str_x(x_size + 1,'\0');
        ub4 charRead = x_size;

        ret_code = OCILobRead(__mp_conn->oci_svchp,
                                  __mp_conn->oci_errhp,
                                  __oracle_define->ora_value.ora_LOB, 
                                  &charRead, 1,
                                  (char*)str_x.data(), x_size,
                                  0, 0,
                                  __mp_conn->__mi_env_cs, SQLCS_IMPLICIT); 
        
        OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLERESULTSET_CLASS_NAME);
        __XDBC_HOOK_CHECK(return false);
                
        __p_string->assign(str_x.c_str());
 
        return true;
      }       
    }
    break;

//----------------------------------
    case SQLT_BLOB:
    {
      if(outType == SQLT_STR)
      {
        string* __p_string = (string*)outBuffer;

        ub4 lenp=0;
        sword ret_code = OCILobGetLength(__mp_conn->oci_svchp,
                                       __mp_conn->oci_errhp,
                                       __oracle_define->ora_value.ora_LOB,
                                       &lenp);

        OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLERESULTSET_CLASS_NAME);
        __XDBC_HOOK_CHECK(return false);
                
        int x_size = lenp;
        string str_x(x_size + 1,'\0');
        ub4 charRead = x_size;

        ret_code = OCILobRead(__mp_conn->oci_svchp,
                                  __mp_conn->oci_errhp,
                                  __oracle_define->ora_value.ora_LOB,
                                  &charRead, 1,
                                  (char*)str_x.data(), x_size,
                                  0, 0,
                                  __mp_conn->__mi_env_cs, SQLCS_IMPLICIT);

        OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLERESULTSET_CLASS_NAME);
        __XDBC_HOOK_CHECK(return false);
                
        __p_string->assign(str_x.c_str(),x_size);

        return true;
      }      
    }

    break;
//----------------------------------
    case SQLT_RDD:
    {
      if(outType == SQLT_STR)
      {
        string* __p_string = (string*)outBuffer;

        char rowidbuf[SQLT_RDD_BYTE_MAX * 3 + 1];
        ub2 outbflp = (SQLT_RDD_BYTE_MAX * 3); 
        memset(rowidbuf,0,outbflp + 1);
       
        sword ret_code = OCIRowidToChar(__oracle_define->ora_value.ora_ROWID,
                       (OraText*)rowidbuf,
                       &outbflp,
                       __mp_conn->oci_errhp);
        OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLERESULTSET_CLASS_NAME);
        __XDBC_HOOK_CHECK(return false);
                
        __p_string->assign(rowidbuf,outbflp);

        return true;
      }
    }
    break;
//----------------------------------
    default:
      break;
  }

  StringBuffer __sb_0;
  __sb_0.append("Conversion not supported. columnName:");
  __sb_0.append("(");
  __sb_0.append(__oracle_column->__ms_name);
  __sb_0.append("),sourceSQLType:");
  __sb_0.append(__oracle_column->__ms_type_name);
  __sb_0.append("(");
  __sb_0.append(inType);
  __sb_0.append("),targetSQLType:");
  __sb_0.append(OracleTypes::getInstance().getTypeName(outType,false));
  __sb_0.append("(");
  __sb_0.append(outType);
  __sb_0.append(")");

  StringBuffer __sb;
  __sb << "[OracleResultSet] - " << __sb_0;
  __XDBC_HOOK_RETURN(return false,__sb.str()) ;
}

//---------------------------------------------------------------------
// Traversal/Positioning
//---------------------------------------------------------------------
  
bool OracleResultSet::isBeforeFirst(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE (SQLException)
{
  return (__mi_cur_row == 0);
}
    
bool OracleResultSet::isAfterLast(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE (SQLException)
{
  return (__mi_cur_row == -1);
}
 
bool OracleResultSet::isFirst(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE (SQLException)
{
  return (__mi_cur_row == 1);
}
 
bool OracleResultSet::isLast(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE (SQLException)
{
  int old_row_no=__mi_cur_row;
  last(__XDBC_HOOK_INVOKE_ONCE);
  __XDBC_HOOK_CHECK(return false);
  int new_row_no=__mi_cur_row;

  absolute(__XDBC_HOOK_INVOKE_MORE old_row_no); 
  __XDBC_HOOK_CHECK(return false);
  
  return (old_row_no == new_row_no);
}
 
bool OracleResultSet::first(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE (SQLException)
{
  if(__mi_resultSetType == ResultSet::TYPE_FORWARD_ONLY)
  {
    StringBuffer __sb;
    __sb.append("[OracleResultSet] not supported operation on resultset type {");
    __sb.append(__mi_resultSetType);
    __sb.append("}");
    
    __XDBC_HOOK_RETURN(return false,__sb.str()) ;
  }

  __XDBC_ORACLERSET_CHECKCLOSED(__mb_isClosed);

  sword  ret_code=OCIStmtFetch2(oci_stmthp,__mp_conn->oci_errhp,
                               (ub4)1,
                               OCI_FETCH_FIRST ,
                               (ub4)0,
                               OCI_DEFAULT);

  if(ret_code==OCI_NO_DATA)
  {
    __mi_cur_row=0;
    return false;
  }
  
  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLERESULTSET_CLASS_NAME);
  __XDBC_HOOK_CHECK(return false);
  
  __mi_cur_row=1;

  return true;
}
 
bool OracleResultSet::last(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE (SQLException)
{
  if(__mi_resultSetType == ResultSet::TYPE_FORWARD_ONLY)
  {
    StringBuffer __sb;
    __sb.append("[OracleResultSet] not supported operation on resultset type {");
    __sb.append(__mi_resultSetType);
    __sb.append("}");

    __XDBC_HOOK_RETURN(return false,__sb.str()) ;
  }

  __XDBC_ORACLERSET_CHECKCLOSED(__mb_isClosed);

  sword  ret_code=OCIStmtFetch2(oci_stmthp,__mp_conn->oci_errhp,
                               (ub4)1,
                               OCI_FETCH_LAST ,
                               (ub4)0,
                               OCI_DEFAULT);

  if(ret_code==OCI_NO_DATA)
  {
    __mi_cur_row=-1;
    return false;
  }
  
  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLERESULTSET_CLASS_NAME);
  __XDBC_HOOK_CHECK(return false);
  
  ub4 numrows = 0;
  ret_code=OCIAttrGet((dvoid *)oci_stmthp, OCI_HTYPE_STMT,
                  (dvoid *)&numrows, (ub4 *)0, OCI_ATTR_CURRENT_POSITION,
                   __mp_conn->oci_errhp);
  
  
  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLERESULTSET_CLASS_NAME);
  __XDBC_HOOK_CHECK(return false);
  
  __mi_cur_row=numrows;

  return true;
}
  
int OracleResultSet::getRow(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE (SQLException)
{
  return __mi_cur_row;
}
  
bool OracleResultSet::absolute(__XDBC_HOOK_DECLARE_MORE int row ) __XDBC_THROW_DECLARE (SQLException)
{
  if(__mi_resultSetType == ResultSet::TYPE_FORWARD_ONLY)
  {
    StringBuffer __sb;
    __sb.append("[OracleResultSet] not supported operation on resultset type {");
    __sb.append(__mi_resultSetType);
    __sb.append("}");

    __XDBC_HOOK_RETURN(return false,__sb.str()) ;
  }

  __XDBC_ORACLERSET_CHECKCLOSED(__mb_isClosed);

  sword  ret_code=OCIStmtFetch2(oci_stmthp,__mp_conn->oci_errhp,
                               (ub4)1,
                               OCI_FETCH_ABSOLUTE ,
                               (ub4)row,
                               OCI_DEFAULT);

  if(ret_code==OCI_NO_DATA)
  {
    __mi_cur_row=0;
    return false;
  }
  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLERESULTSET_CLASS_NAME);
  __XDBC_HOOK_CHECK(return false);
  
  __mi_cur_row=row;

  return true;
}
  
bool OracleResultSet::relative(__XDBC_HOOK_DECLARE_MORE int rows ) __XDBC_THROW_DECLARE (SQLException)
{
  if(__mi_resultSetType == ResultSet::TYPE_FORWARD_ONLY)
  {
    StringBuffer __sb;
    __sb.append("[OracleResultSet] not supported operation on resultset type {");
    __sb.append(__mi_resultSetType);
    __sb.append("}");

    __XDBC_HOOK_RETURN(return false,__sb.str()) ;
  }

  __XDBC_ORACLERSET_CHECKCLOSED(__mb_isClosed);

  sword  ret_code=OCIStmtFetch2(oci_stmthp,__mp_conn->oci_errhp,
                               (ub4)1,
                               OCI_FETCH_RELATIVE ,
                               (ub4)rows,
                               OCI_DEFAULT);

  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLERESULTSET_CLASS_NAME);
  __XDBC_HOOK_CHECK(return false);
  
  if(ret_code==OCI_NO_DATA)
  {
    if(rows>=0) { __mi_cur_row=-1; } 
    else { __mi_cur_row=0; }

    return false;
  }

  __mi_cur_row+=rows;
  return true;
}
  
bool OracleResultSet::previous(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE (SQLException)
{
  if(__mi_resultSetType == ResultSet::TYPE_FORWARD_ONLY)
  {
    StringBuffer __sb;
    __sb.append("[OracleResultSet] not supported operation on resultset type {");
    __sb.append(__mi_resultSetType);
    __sb.append("}");

    __XDBC_HOOK_RETURN(return false,__sb.str()) ;
  }
 
  __XDBC_ORACLERSET_CHECKCLOSED(__mb_isClosed);

  sword  ret_code=OCIStmtFetch2(oci_stmthp,__mp_conn->oci_errhp,
                               (ub4)1,
                               OCI_FETCH_PRIOR,
                               (ub4)0,
                               OCI_DEFAULT);
                               
  if(ret_code==OCI_NO_DATA)
  {
    __mi_cur_row=0;
    return false;
  }
  
  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLERESULTSET_CLASS_NAME);
  __XDBC_HOOK_CHECK(return false);
  
  __mi_cur_row--;
  return true;
}

bool OracleResultSet::isSupported(__XDBC_HOOK_DECLARE_MORE int resultSetType, int resultSetConcurrency)  __XDBC_THROW_DECLARE(SQLException)
{
  if( (resultSetType!=TYPE_FORWARD_ONLY) && (resultSetType!=TYPE_SCROLL_INSENSITIVE ))
  {
    StringBuffer __sb;
    __sb.append("not supported resultset type {");
    __sb.append(resultSetType);
    __sb.append("}");

    __XDBC_HOOK_RETURN(return false,__sb.str()) ;
  }
  
  if(resultSetConcurrency!=CONCUR_READ_ONLY)
  {
    StringBuffer __sb;
    __sb.append("not supported concurrency mode {");
    __sb.append(resultSetConcurrency);
    __sb.append("}");

    __XDBC_HOOK_RETURN(return false,__sb.str()) ;
  }

  return true;
}

void OracleResultSet::setFetchSize(__XDBC_HOOK_DECLARE_MORE int rows) __XDBC_THROW_DECLARE (SQLException)
{
  if(rows < 0 )
  {
    return;
  }
 
  ub4   prefetch = rows;
  sword  ret_code=OCIAttrSet(oci_stmthp, OCI_HTYPE_STMT, &prefetch,0, OCI_ATTR_PREFETCH_ROWS, __mp_conn->oci_errhp);
  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLERESULTSET_CLASS_NAME);
  __XDBC_HOOK_CHECK(return);
  
  __mi_prefetch = rows;
}

int OracleResultSet::getFetchSize(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException)
{
  return __mi_prefetch;
}

void OracleResultSet::prepareColumnData(__XDBC_HOOK_DECLARE_MORE OracleConnection* ora_conn,OCIStmt* ociStmthp,OracleResultSetCollection* cachedCollection)  __XDBC_THROW_DECLARE(SQLException)
{
  OracleResultSetCollection* __cachedCollection = cachedCollection;
  int __column_count = (int)(__cachedCollection->__vector_columns.size() - 1);   
  
  __cachedCollection->__vector_defines.resize(__column_count + 1);
   
  int __column_offset = 0;
  int __column_type = 0;
  OracleDefine* __oracle_define  = 0;
  
  for(int i = 1; i<= __column_count;++i)
  {
    __oracle_define = &(__cachedCollection->__vector_defines[i]);
    __oracle_define->__mi_index=__cachedCollection->__vector_columns[i].__mi_index;
    __oracle_define->int_buf_offset=__column_offset;
    __oracle_define->__mi_buf_size=__cachedCollection->__vector_columns[i].__mi_length;
  
    __column_type = __cachedCollection->__vector_columns[i].__mi_type;
    if(__cachedCollection->__vector_columns[i].__mb_char_semantics)
    {
      __oracle_define->__mi_buf_size=__oracle_define->__mi_buf_size * (ORACLE_CHAR_TO_BYTE_MAX);
    }
         
    switch(__column_type)
    {   
  //----------------------------- 
      case SQLT_VCS:
      case SQLT_VBI:
      {
        //first SHORT_PREFIX_LENGTH bytes contain the length of the character string
        __oracle_define->__mi_buf_size += SHORT_PREFIX_LENGTH; 
      }
      break;
  //-----------------------------
      case SQLT_LVC:
      case SQLT_LVB:
      {
        //first  INTEGER_PREFIX_LENGTH bytes contain the length of the character string
        __oracle_define->__mi_buf_size += INTEGER_PREFIX_LENGTH; 
      }
      break;  
  //-----------------------------
      case SQLT_LNG:
      {
        __oracle_define->__mi_buf_size = SQLT_LNG_BYTE_MAX; //fuck code!!
      }
      break;
  //-----------------------------
      case SQLT_LBI:
      {
        __oracle_define->__mi_buf_size = SQLT_LBI_BYTE_MAX; //fuck code!! 
      }
      break;
  //-----------------------------
      case SQLT_INT:
      case SQLT_UIN:
      case SQLT_BFLOAT:
      case SQLT_IBFLOAT:
      case SQLT_FLT:
      case SQLT_BDOUBLE:
      case SQLT_IBDOUBLE:
      case SQLT_VNU:
      case SQLT_CLOB:
      case SQLT_BLOB:
      case SQLT_FILE:
      case SQLT_TIMESTAMP:
      case SQLT_TIMESTAMP_TZ:
      case SQLT_TIMESTAMP_LTZ:
      case SQLT_DATE:
      case SQLT_INTERVAL_YM:
      case SQLT_INTERVAL_DS:
      case SQLT_RDD:
      {         
        __oracle_define->__mi_buf_size = 0;
      }
      break;  
            
    } //~end switch(__column_type)  
  
    __oracle_define->__mi_define_size=__oracle_define->__mi_buf_size;
    __oracle_define->__mi_define_ind=0;
    __oracle_define->__mi_define_code=0;
  
    int __addr_alignment = MemoryKit::roundUp(__oracle_define->__mi_buf_size,sizeof(void*));
    __column_offset += __addr_alignment;
    __cachedCollection->__rowbuf_size += __addr_alignment;
    
    
  }//~end for(int i = 1; i<= __column_count;++i)
  
  __cachedCollection->__p_rowbuf = new char[__cachedCollection->__rowbuf_size];
  memset(__cachedCollection->__p_rowbuf,0,__cachedCollection->__rowbuf_size);
}

void OracleResultSet::defineColumnData(__XDBC_HOOK_DECLARE_MORE OracleConnection* __mp_conn,OCIStmt* ociStmthp,OracleResultSetCollection* cachedCollection)  __XDBC_THROW_DECLARE(SQLException)
{
  OracleResultSetCollection* __cachedCollection = cachedCollection;
  int __column_count = (int)(__cachedCollection->__vector_columns.size() - 1);   
  
  char* buf_start = 0;
  sword buf_size = 0;
  
  sword data_type = 0;
  sword status = 0;
  OracleDefine* __oracle_define = 0;
  OracleColumn* __oracle_column = 0;
  
  for(int i = 1 ;i <= __column_count;++i)
  {   
    __oracle_column = &(__cachedCollection->__vector_columns[i]);
    __oracle_define = &(__cachedCollection->__vector_defines[i]);
    data_type = __oracle_column->__mi_type;
    
    
    switch(data_type)
    {
      case SQLT_INT:
      {
        status = OCIDefineByPos(ociStmthp, 
                                &(__oracle_define->oci_defnp), 
                                __mp_conn->oci_errhp,
                                i,&(__oracle_define->ora_value.ora_BIGINT),
                                (sword)sizeof(__oracle_define->ora_value.ora_BIGINT),data_type ,
                                (dvoid  *)&(__oracle_define->__mi_define_ind) ,
                                (ub2  *)&(__oracle_define->__mi_define_size),
                                (ub2 *)&(__oracle_define->__mi_define_code),
                                OCI_DEFAULT); 
      }
      break;
  //-----------------------------  
      case SQLT_UIN:
      {
        status = OCIDefineByPos(ociStmthp, 
                                &(__oracle_define->oci_defnp), 
                                __mp_conn->oci_errhp,
                                i,&(__oracle_define->ora_value.ora_UBIGINT),
                                (sword)sizeof(__oracle_define->ora_value.ora_UBIGINT),data_type ,
                                (dvoid  *)&(__oracle_define->__mi_define_ind) ,
                                (ub2  *)&(__oracle_define->__mi_define_size),
                                (ub2 *)&(__oracle_define->__mi_define_code),
                                OCI_DEFAULT); 
      }
      break;
  //-----------------------------       
      case SQLT_BFLOAT:
      case SQLT_IBFLOAT:
      {
        status = OCIDefineByPos(ociStmthp, 
                                &(__oracle_define->oci_defnp), 
                                __mp_conn->oci_errhp,
                                i,&(__oracle_define->ora_value.ora_FLOAT),
                                (sword)sizeof(__oracle_define->ora_value.ora_FLOAT),data_type ,
                                (dvoid  *)&(__oracle_define->__mi_define_ind) ,
                                (ub2  *)&(__oracle_define->__mi_define_size),
                                (ub2 *)&(__oracle_define->__mi_define_code),
                                OCI_DEFAULT);  
      }
      break;
  //----------------------------- 
      case SQLT_FLT:
      case SQLT_BDOUBLE:
      case SQLT_IBDOUBLE:
      {
        status = OCIDefineByPos(ociStmthp, 
                                &(__oracle_define->oci_defnp), 
                                __mp_conn->oci_errhp,
                                i,&(__oracle_define->ora_value.ora_DOUBLE),
                                (sword)sizeof(__oracle_define->ora_value.ora_DOUBLE),data_type ,
                                (dvoid  *)&(__oracle_define->__mi_define_ind) ,
                                (ub2  *)&(__oracle_define->__mi_define_size),
                                (ub2 *)&(__oracle_define->__mi_define_code),
                                OCI_DEFAULT);        
      }
      break;
  //----------------------------- 
      case SQLT_VNU:
      {
        status = OCIDefineByPos(ociStmthp, 
                                &(__oracle_define->oci_defnp), 
                                __mp_conn->oci_errhp,
                                i,&(__oracle_define->ora_value.ora_NUMBER),
                                (sword)sizeof(__oracle_define->ora_value.ora_NUMBER),data_type ,
                                (dvoid  *)&(__oracle_define->__mi_define_ind) ,
                                (ub2  *)&(__oracle_define->__mi_define_size),
                                (ub2 *)&(__oracle_define->__mi_define_code),
                                OCI_DEFAULT);
      }
      break;
  //-----------------------------
      case SQLT_CLOB:
      case SQLT_BLOB:
      case SQLT_FILE:
      {
       if(__oracle_define->ora_value.ora_LOB == 0)
       {
         OCIDescriptorAlloc((dvoid *)__mp_conn->oci_envhp, (dvoid **)&(__oracle_define->ora_value.ora_LOB),(ub4) OracleConversion::SQLT_TO_DTYPE(data_type), (size_t) 0, (dvoid **) 0);
       }
       
       status = OCIDefineByPos(ociStmthp, 
                              &(__oracle_define->oci_defnp), 
                              __mp_conn->oci_errhp,
                              i,&(__oracle_define->ora_value.ora_LOB),
                              (sword)sizeof(__oracle_define->ora_value.ora_LOB),data_type ,
                              (dvoid  *)&(__oracle_define->__mi_define_ind) ,
                              (ub2  *)&(__oracle_define->__mi_define_size),
                              (ub2 *)&(__oracle_define->__mi_define_code), 
                              OCI_DEFAULT);                              
      }
      break;
  //-----------------------------    
      case SQLT_TIMESTAMP:
      case SQLT_TIMESTAMP_TZ:
      case SQLT_TIMESTAMP_LTZ:
      case SQLT_DATE:
      {
        if(__oracle_define->ora_value.ora_DATETIME == 0)
        {
          OCIDescriptorAlloc((dvoid *)__mp_conn->oci_envhp, (dvoid **)&(__oracle_define->ora_value.ora_DATETIME),(ub4) OracleConversion::SQLT_TO_DTYPE(data_type), (size_t) 0, (dvoid **) 0);
        }
        
        status = OCIDefineByPos(ociStmthp, 
                                &(__oracle_define->oci_defnp), 
                                __mp_conn->oci_errhp,
                                i,&(__oracle_define->ora_value.ora_DATETIME), 
                                (sword)sizeof(__oracle_define->ora_value.ora_DATETIME),data_type ,
                                (dvoid  *)&(__oracle_define->__mi_define_ind) ,
                                (ub2  *)&(__oracle_define->__mi_define_size),
                                (ub2 *)&(__oracle_define->__mi_define_code),
                                OCI_DEFAULT);
      }
      break;
  //-----------------------------      
      case SQLT_INTERVAL_YM:
      case SQLT_INTERVAL_DS:
      {
        if(__oracle_define->ora_value.ora_INTERVAL == 0)
        {
          OCIDescriptorAlloc((dvoid *)__mp_conn->oci_envhp, (dvoid **)&(__oracle_define->ora_value.ora_INTERVAL),(ub4) OracleConversion::SQLT_TO_DTYPE(data_type), (size_t) 0, (dvoid **) 0);
        }
        
         status = OCIDefineByPos(ociStmthp, 
                                &(__oracle_define->oci_defnp), 
                                __mp_conn->oci_errhp,
                                i,&(__oracle_define->ora_value.ora_INTERVAL),
                                (sword)sizeof(__oracle_define->ora_value.ora_INTERVAL),data_type ,
                                (dvoid  *)&(__oracle_define->__mi_define_ind) ,
                                (ub2  *)&(__oracle_define->__mi_define_size),
                                (ub2 *)&(__oracle_define->__mi_define_code),
                                OCI_DEFAULT);      
      }
      break;
  //-----------------------------
      case SQLT_RDD:
      {
        if(__oracle_define->ora_value.ora_ROWID == 0)
        {
          OCIDescriptorAlloc((dvoid *)__mp_conn->oci_envhp, (dvoid **)&(__oracle_define->ora_value.ora_ROWID),(ub4) OracleConversion::SQLT_TO_DTYPE(data_type), (size_t) 0, (dvoid **) 0);
        }
        
        status = OCIDefineByPos(ociStmthp,
                                &(__oracle_define->oci_defnp),
                                __mp_conn->oci_errhp,
                                i,&(__oracle_define->ora_value.ora_ROWID),
                                (sword)sizeof(__oracle_define->ora_value.ora_ROWID),data_type ,
                                (dvoid  *)&(__oracle_define->__mi_define_ind) ,
                                (ub2  *)&(__oracle_define->__mi_define_size),
                                (ub2 *)&(__oracle_define->__mi_define_code),
                                OCI_DEFAULT);                        
      }
      break;
  //-----------------------------
      default:
      {         
         buf_start = __cachedCollection->__p_rowbuf + __oracle_define->int_buf_offset;
         buf_size = __oracle_define->__mi_buf_size;
         
         //printf("buf_size:%d , data_type : %d,define_index : %d\n",buf_size,data_type,i);
         
         status = OCIDefineByPos(ociStmthp, 
                                &(__oracle_define->oci_defnp), 
                                __mp_conn->oci_errhp,
                                i,(dvoid  *)((char*)buf_start),
                                (sword)buf_size,data_type ,
                                (dvoid  *)&(__oracle_define->__mi_define_ind) ,
                                (ub2  *)&(__oracle_define->__mi_define_size),
                                (ub2 *)&(__oracle_define->__mi_define_code),
                                OCI_DEFAULT);
                        
         OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,status,ORACLERESULTSET_CLASS_NAME);
         __XDBC_HOOK_CHECK(return); 
      }
      break;         
    }
    
    OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,status,ORACLERESULTSET_CLASS_NAME);
    __XDBC_HOOK_CHECK(return);
    
  }//~end for(int i = 1 ;i <= __column_count;++i)
}

void OracleResultSet::resetResultSet(OracleResultSet* pResultSet,OracleConnection* ora_conn,SQLImplementation* pXstmt,OCIStmt* ociStmthp,
                            int resultSetType, int resultSetConcurrency,
                            OracleResultSetCollection* cachedCollection,bool adoptIt,
                            bool selfDelete)
{
  pResultSet->__mp_conn = ora_conn;
  pResultSet->__p_xstmt = pXstmt;
  pResultSet->oci_stmthp = ociStmthp;
  
  pResultSet->__mi_resultSetType = resultSetConcurrency;  
  pResultSet->__cachedCollection = cachedCollection;
  pResultSet->__adoptIt = adoptIt;

  pResultSet->__mb_selfDelete = selfDelete;


  pResultSet->__mi_column_count = (int)(pResultSet->__cachedCollection->__vector_columns.size() -1 );

  pResultSet->__mi_prefetch = 0;
  pResultSet->__mb_isClosed = false;
  pResultSet->__mi_cur_row = 0;
}  

void OracleResultSet::releaseColumnData(__XDBC_HOOK_DECLARE_MORE OracleConnection* ora_conn,OCIStmt* ociStmthp,OracleResultSetCollection* cachedCollection)  __XDBC_THROW_DECLARE(SQLException)
{ 
  int numcols = (int)(cachedCollection->__vector_columns.size() - 1);
  for(int i = 1; i <= numcols;++i)
  {
    cachedCollection->__vector_defines[i].cleanup(cachedCollection->__vector_columns[i]);
  }
}

__OPENCODE_END_NAMESPACE  
