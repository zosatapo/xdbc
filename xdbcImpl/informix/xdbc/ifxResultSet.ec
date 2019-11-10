#include "xdbc/ifxResultSet.h"
#include "xdbc/ifxConnection.h"
#include "xdbc/ifxStatement.h"
#include "xdbc/ifxResultSetMetaData.h"
#include "xdbc/xsmart.h"
#include "xdbc/ifxClob.h"
#include "xdbc/ifxBlob.h"
#include "xdbc/ifxConversion.h"
#include "xdbc/ifxConversion.h"

#include "port/ioc_sbuf.h"
#include "port/utilities.h"
#include "port/file.h"

__OPENCODE_BEGIN_NAMESPACE

//-------------------------------------------------------------------------

#define IFRESULTSET_CLASS_NAME "IfxResultSet"

IfxResultSetCollection::~IfxResultSetCollection() 
{
  if(__p_rowbuf != 0)
  {
    delete []__p_rowbuf;
  }  
}
  
IfxResultSetCollection::IfxResultSetCollection()
{
  __p_rowbuf = 0;
  __rowbuf_size = 0;
}

IfxResultSetCollection::IfxResultSetCollection(const IfxResultSetCollection& rv)
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

IfxResultSetCollection& IfxResultSetCollection::operator=(const IfxResultSetCollection& rv)
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

//-------------------------------------------------------------------------

IfxResultSet::~IfxResultSet()
{
  __XDBC_TRACE(IFRESULTSET_CLASS_NAME,"destroy()"); 
}

IfxResultSet::IfxResultSet(IfxConnection* pConn,SQLImplementation* pXimplValue,const string& cursor_name,const string& cursor_id,int resultSetType, int resultSetConcurrency,bool close_stmt,ResultSetCollection* cachedCollection)
:__ms_cursor_name(cursor_name),__ms_cursor_id(cursor_id)
{
  __mp_conn = pConn;
  __mp_impl=pXimplValue;

  __mi_resultSetType = resultSetType;
  __mi_resultSetConcurrency = resultSetConcurrency;
  
   __mb_isEmpty = false;
   __mb_isClosed = false;
   __mi_cur_row = 0;
   __mb_close_stmt = close_stmt;

   __mi_row_count = -1;

//--------------------------------------------
   __mp_ec_sqlda_columns = 0;
   __mi_column_count = 0;
   
   __cachedCollection = 0;
   __adoptIt = true;
   
   if(cachedCollection != 0)
   {
		#ifdef OPENCODE_HAS_RTTI
		  IfxResultSetCollection* __p_rscoll = dynamic_cast<IfxResultSetCollection*>(cachedCollection);
		#else
		  IfxResultSetCollection* __p_rscoll = (IfxResultSetCollection*)(cachedCollection);
		#endif
		
     if(__p_rscoll != 0)
     {
       __cachedCollection = __p_rscoll;
       __adoptIt = false;
     }
   }
   
}

void IfxResultSet::initialize(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{
  if(__mb_isEmpty)
  {
    return;
  }

  if(__cachedCollection != 0)
  {
    memset(__cachedCollection->__p_rowbuf,0,__cachedCollection->__rowbuf_size);
  }
  
  if(__cachedCollection == 0)
  {
    __cachedCollection = new  IfxResultSetCollection();
  }
    
  //-------------------------------------------------------------------------
  // 1. fetch  ResultSetMetaData
  //-------------------------------------------------------------------------

  EXEC SQL BEGIN DECLARE SECTION;
    char*  ec_cursor_name;
    char*  ec_cursor_id;
  EXEC SQL END DECLARE SECTION;

  ec_cursor_name  = (char*)__ms_cursor_name.c_str();
  ec_cursor_id    = (char*)__ms_cursor_id.c_str();

  EXEC SQL DESCRIBE :ec_cursor_id into __mp_ec_sqlda_columns;
  IfxConversion::checkException(__XDBC_HOOK_INVOKE_MORE SQLCODE,SQLSTATE,IFRESULTSET_CLASS_NAME);
  __XDBC_HOOK_CHECK(return);
  
  if(__mp_ec_sqlda_columns != 0)
  {
    __mi_column_count = __mp_ec_sqlda_columns->sqld;

    if(__adoptIt)
    {
      IfxConversion::parseResultSetMetaData(__mp_ec_sqlda_columns,__cachedCollection->__vector_columns);
    }
  }

  if(__mp_ec_sqlda_columns != 0)
  {
    string str_cursor_desc_id = __ms_cursor_id+"_desc"; 
    EXEC SQL BEGIN DECLARE SECTION;
      char*  ec_cursor_desc_id = 0;
      mint ec_sel_cnt = 0;
      mint ec_i = 0;
      short ec_scale = 0;
      short ec_precision = 0;
      short ec_nullable = 0;
    EXEC SQL END DECLARE SECTION;
    
    ec_cursor_desc_id = (char*)str_cursor_desc_id.c_str();
    
    ec_sel_cnt = __mi_column_count;
    EXEC SQL allocate descriptor :ec_cursor_desc_id with max :ec_sel_cnt;
    EXEC SQL describe :ec_cursor_id using sql descriptor :ec_cursor_desc_id;

    int sql_code = (int)SQLCODE;
    if(sql_code != 0 )
    {
      EXEC SQL deallocate descriptor :ec_cursor_desc_id;
    }
    
    IfxConversion::checkException(__XDBC_HOOK_INVOKE_MORE SQLCODE,SQLSTATE,IFRESULTSET_CLASS_NAME);
    __XDBC_HOOK_CHECK(return);
    
    EXEC SQL get descriptor :ec_cursor_desc_id :ec_sel_cnt = COUNT;
    
    for(ec_i = 1; ec_i <= ec_sel_cnt; ++ec_i)
    {
       EXEC SQL get descriptor :ec_cursor_desc_id VALUE :ec_i 
               :ec_scale = SCALE , 
               :ec_precision = PRECISION , 
               :ec_nullable = NULLABLE;   

       __cachedCollection->__vector_columns[ec_i-1].__mi_scale = ec_scale;
       __cachedCollection->__vector_columns[ec_i-1].__mi_precision = ec_precision;
       __cachedCollection->__vector_columns[ec_i-1].__mi_nullable = ec_nullable;     
    }

    EXEC SQL deallocate descriptor :ec_cursor_desc_id;    
  }

  if(DriverManager::isTracingOn)
  {  
    StringBuffer __sb;
    __sb << "<init>|" << File::lineSeparator;
    
    for(int i = 1;i <= __mi_column_count;++i)
    {
      __cachedCollection->__vector_columns[i].dump(__sb);
    }
    
    __XDBC_FORCED_TRACE(IFRESULTSET_CLASS_NAME,__sb.str());
  }

  //-------------------------------------------------------------------------
  // 2. allocate & define row data buffer 
  //-------------------------------------------------------------------------
  IfxConversion::prepareColumnData(__XDBC_HOOK_INVOKE_MORE __mp_ec_sqlda_columns,__cachedCollection->__vector_columns,__cachedCollection->__vector_defines,__cachedCollection->__p_rowbuf,__cachedCollection->__rowbuf_size,__cachedCollection,__adoptIt);
}

ResultSetMetaData* IfxResultSet::getMetaData(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{
  return new IfxResultSetMetaData(__cachedCollection->__vector_columns);;
}

bool IfxResultSet::next(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{
  if(__mb_isEmpty || (__mi_cur_row==-1))
  {
    return false;
  }

  __XDBC_INFORMIXRSET_CHECKCLOSED(__mb_isClosed);

  EXEC SQL BEGIN DECLARE SECTION;
    char* ec_cursor_name = (char*)__ms_cursor_name.c_str(); 
  EXEC SQL END DECLARE SECTION; 

  EXEC SQL FETCH :ec_cursor_name USING DESCRIPTOR __mp_ec_sqlda_columns;
  IfxConversion::checkException(__XDBC_HOOK_INVOKE_MORE SQLCODE,SQLSTATE,IFRESULTSET_CLASS_NAME);
  __XDBC_HOOK_CHECK(return false);  
  
  if(__mi_row_count == -1)
  {
    __mi_row_count = sqlca.sqlerrd[2];
  }

/*
  printf("sqlerrd :-");
  for(int i=0;i<6;++i)
  {
    printf("- [%d] -",sqlca.sqlerrd[i]);
  }
  printf("-\n");
  printf("SQLCODE : [%d] ,SQLSTATE : [%s]\n",SQLCODE,SQLSTATE);

*/

  if(SQLCODE == 100)
  {
    __mi_cur_row=-1;
    return false;
  }

  __mi_cur_row++;
 
  return true;
}


void IfxResultSet::close()
{
  __XDBC_TRACE(IFRESULTSET_CLASS_NAME,"close()");

  if(__mb_isClosed)
  {
    return;
  }

  if(__mp_ec_sqlda_columns != 0)
  {
    delete __mp_ec_sqlda_columns;
    __mp_ec_sqlda_columns = 0;
  } 

  EXEC SQL BEGIN DECLARE SECTION;
    char*  ec_cursor_name;
    char*  ec_cursor_id;
  EXEC SQL END DECLARE SECTION;

  ec_cursor_name  = (char*)__ms_cursor_name.c_str();
  ec_cursor_id    = (char*)__ms_cursor_id.c_str();

  EXEC SQL CLOSE :ec_cursor_name;
  EXEC SQL FREE :ec_cursor_name;

  if(__mb_close_stmt)
  {
    EXEC SQL FREE :ec_cursor_id;
  }
  
  __mb_isClosed=true; 
}

void IfxResultSet::destroy()
{
  if(__adoptIt && (__cachedCollection != 0))
  {
    delete __cachedCollection;
  }
  
  delete this;
}

bool IfxResultSet::wasNull(__XDBC_HOOK_DECLARE_MORE int columnIndex)  __XDBC_THROW_DECLARE(SQLException)
{
  __XDBC_INFORMIXRSET_CHECKCOLUMN(columnIndex);
 
  bool is_null = (__cachedCollection->__vector_defines[columnIndex].__mi_define_ind==-1);
 
  int col_type = __cachedCollection->__vector_columns[columnIndex].__mi_type & SQLTYPE;
  
  if(!is_null)
  {
    if(col_type == SQLTEXT || col_type == SQLBYTES)
    {
      is_null = (__cachedCollection->__vector_defines[columnIndex].ifx_value.ec_locator.loc_indicator == -1);
    }
  }

  return is_null;  
}

bool IfxResultSet::getByte(__XDBC_HOOK_DECLARE_MORE int columnIndex,char& colval)  __XDBC_THROW_DECLARE(SQLException)
{
  __XDBC_INFORMIXRSET_CHECKNULL_BOOLEAN(columnIndex);
  
  if(__cachedCollection->__vector_columns[columnIndex].__mi_type == SQLSMINT)
  {
    colval = (char)__cachedCollection->__vector_defines[columnIndex].ifx_value.ec_SMINT;
    return true;
  }
    
  short __col_value = 0;
  if(checkConversion(__XDBC_HOOK_INVOKE_MORE columnIndex,SQLSMINT,(char*)&__col_value))
  {
    colval = (char)(__col_value);
    return true;
  }
  
  return false;  
}

bool IfxResultSet::getShort(__XDBC_HOOK_DECLARE_MORE int columnIndex,short& colval)  __XDBC_THROW_DECLARE(SQLException)
{
  __XDBC_INFORMIXRSET_CHECKNULL_BOOLEAN(columnIndex);
  
  if(__cachedCollection->__vector_columns[columnIndex].__mi_type == SQLSMINT)
  {
    colval = (short)__cachedCollection->__vector_defines[columnIndex].ifx_value.ec_SMINT;
    return true;
  }
    
  short __col_value = 0;
  checkConversion(__XDBC_HOOK_INVOKE_MORE columnIndex,SQLSMINT,(char*)&__col_value);
  __XDBC_HOOK_CHECK(return false);
  
  colval = (short)__col_value;
  return true;
}

bool IfxResultSet::getInt(__XDBC_HOOK_DECLARE_MORE int columnIndex,int& colval)  __XDBC_THROW_DECLARE(SQLException)
{
  __XDBC_INFORMIXRSET_CHECKNULL_BOOLEAN(columnIndex);
  
  if(__cachedCollection->__vector_columns[columnIndex].__mi_type == SQLINT)
  {
    colval = (int)__cachedCollection->__vector_defines[columnIndex].ifx_value.ec_INT;
    return true;
  }
    
  int __col_value = 0;
  checkConversion(__XDBC_HOOK_INVOKE_MORE columnIndex,SQLINT,(char*)&__col_value);
  __XDBC_HOOK_CHECK(return false);
    
  colval = (int)__col_value;
  return true;
}

bool IfxResultSet::getLong(__XDBC_HOOK_DECLARE_MORE int columnIndex,XLONG& colval)  __XDBC_THROW_DECLARE(SQLException)
{
  __XDBC_INFORMIXRSET_CHECKNULL_BOOLEAN(columnIndex);
  
  if(__cachedCollection->__vector_columns[columnIndex].__mi_type == SQLINT8)
  {
    colval = (int)__cachedCollection->__vector_defines[columnIndex].ifx_value.ec_INT;
    IfxConversion::int8ToLong(&(__cachedCollection->__vector_defines[columnIndex].ifx_value.ec_int8),colval);
    
    return true;
  }
    
  XLONG __col_value = 0;
  checkConversion(__XDBC_HOOK_INVOKE_MORE columnIndex,SQLINT8,(char*)&__col_value);
  __XDBC_HOOK_CHECK(return false);

  colval = (XLONG)__col_value;
  return true;
}

bool IfxResultSet::getFloat(__XDBC_HOOK_DECLARE_MORE int columnIndex,float& colval)  __XDBC_THROW_DECLARE(SQLException)
{
  __XDBC_INFORMIXRSET_CHECKNULL_BOOLEAN(columnIndex);
  
  if(__cachedCollection->__vector_columns[columnIndex].__mi_type == SQLFLOAT)
  {
    colval = (float)__cachedCollection->__vector_defines[columnIndex].ifx_value.ec_DOUBLE;
    return true;
  }
    
  double __col_value = 0;
  checkConversion(__XDBC_HOOK_INVOKE_MORE columnIndex,SQLFLOAT,(char*)&__col_value);
  __XDBC_HOOK_CHECK(return false);

  colval = (float)__col_value;
  return true;
}

bool IfxResultSet::getDouble(__XDBC_HOOK_DECLARE_MORE int columnIndex,double& colval)  __XDBC_THROW_DECLARE(SQLException)
{
  __XDBC_INFORMIXRSET_CHECKNULL_BOOLEAN(columnIndex);
  
  if(__cachedCollection->__vector_columns[columnIndex].__mi_type == SQLFLOAT)
  {
    colval = (double)__cachedCollection->__vector_defines[columnIndex].ifx_value.ec_DOUBLE;
    return true;
  }
    
  double __col_value = 0;
  checkConversion(__XDBC_HOOK_INVOKE_MORE columnIndex,SQLFLOAT,(char*)&__col_value);
  __XDBC_HOOK_CHECK(return false);
  
  colval = (double)__col_value;
  return true;
}

bool IfxResultSet::getDate(__XDBC_HOOK_DECLARE_MORE int columnIndex,XDBC_DATE& colval)  __XDBC_THROW_DECLARE(SQLException)
{
  __XDBC_INFORMIXRSET_CHECKNULL_BOOLEAN(columnIndex);
  
  if(__cachedCollection->__vector_columns[columnIndex].__mi_type == SQLDATE)
  {
    short mdy_array[3];
    rjulmdy(__cachedCollection->__vector_defines[columnIndex].ifx_value.ec_INT, mdy_array);
    
    colval.year  = mdy_array[2];
    colval.month = mdy_array[0];
    colval.day   = mdy_array[1];
    
    return true;
  }
    
  XDBC_DATE __col_value;
  checkConversion(__XDBC_HOOK_INVOKE_MORE columnIndex,SQLDATE,(char*)&__col_value);
  __XDBC_HOOK_CHECK(return false);

  colval = __col_value;
  return true;
}

bool IfxResultSet::getTime(__XDBC_HOOK_DECLARE_MORE int columnIndex,XDBC_TIME& colval)  __XDBC_THROW_DECLARE(SQLException)
{
  XDBC_TIMESTAMP __sql_timestamp; 
  if(getTimestamp(__XDBC_HOOK_INVOKE_MORE columnIndex,__sql_timestamp))
  {
    colval.hour   = __sql_timestamp.hour;
    colval.minute = __sql_timestamp.minute;
    colval.second = __sql_timestamp.second;
    
    return true;
  }

  return false;
}

bool IfxResultSet::getTimestamp(__XDBC_HOOK_DECLARE_MORE int columnIndex,XDBC_TIMESTAMP& colval)  __XDBC_THROW_DECLARE(SQLException)
{
  __XDBC_INFORMIXRSET_CHECKNULL_BOOLEAN(columnIndex);
  
  if(__cachedCollection->__vector_columns[columnIndex].__mi_type == SQLDTIME)
  {    
    IfxConversion::parseDateTime(&(__cachedCollection->__vector_defines[columnIndex].ifx_value.ec_dtime),colval);   
        
    return true;
  }
    
  XDBC_TIMESTAMP __col_value;
  checkConversion(__XDBC_HOOK_INVOKE_MORE columnIndex,SQLDTIME,(char*)&__col_value);
  __XDBC_HOOK_CHECK(return false);

  colval = __col_value;
  return true;
}

bool IfxResultSet::getInterval(__XDBC_HOOK_DECLARE_MORE int  columnIndex,XDBC_INTERVAL& colval)  __XDBC_THROW_DECLARE(SQLException)
{
  __XDBC_INFORMIXRSET_CHECKNULL_BOOLEAN(columnIndex);
  
  if(__cachedCollection->__vector_columns[columnIndex].__mi_type == SQLINTERVAL)
  {    
    IfxConversion::parseInterval(&(__cachedCollection->__vector_defines[columnIndex].ifx_value.ec_interval),colval);
        
    return true;
  }
    
  XDBC_INTERVAL __col_value;
  XDBC_INTERVAL::init(&__col_value);
  
  checkConversion(__XDBC_HOOK_INVOKE_MORE columnIndex,SQLINTERVAL,(char*)&__col_value);
  __XDBC_HOOK_CHECK(return false);

  colval = __col_value;
  return true;
}

 
const char* IfxResultSet::getString(__XDBC_HOOK_DECLARE_MORE int columnIndex,int& sizeBytes)  __XDBC_THROW_DECLARE(SQLException)
{
  sizeBytes = 0;
  __XDBC_INFORMIXRSET_CHECKNULL_POINTER(columnIndex);

  if(__cachedCollection->__vector_columns[columnIndex].__mi_type == SQLVCHAR)
  {    
    char* start = __cachedCollection->__p_rowbuf+__cachedCollection->__vector_defines[columnIndex].int_buf_offset;
    sizeBytes = __cachedCollection->__vector_defines[columnIndex].__mi_define_size;
  
    return start;
  }
  
  string& __tmp_str = __cachedCollection->__vector_defines[columnIndex].__tmp_std_str;  
  checkConversion(__XDBC_HOOK_INVOKE_MORE columnIndex,SQLVCHAR,(char*)&__tmp_str);
  __XDBC_HOOK_CHECK(return 0);
  
  sizeBytes = (int)__tmp_str.size();
  return __tmp_str.data();
}

Blob* IfxResultSet::getBlob(__XDBC_HOOK_DECLARE_MORE int columnIndex)  __XDBC_THROW_DECLARE(SQLException)
{
  __XDBC_INFORMIXRSET_CHECKNULL_POINTER(columnIndex);

  if(__cachedCollection->__vector_columns[columnIndex].__mi_type == SQLUDTFIXED)
  {    
    struct ifx_lo_ts* p_ec_lo_ts = &(__cachedCollection->__vector_defines[columnIndex].ifx_value.ec_lo_ts);
    return new IfxBlob(__mp_conn,p_ec_lo_ts);    
  }
  
  IfxBlob blob(__mp_conn);
  checkConversion(__XDBC_HOOK_INVOKE_MORE columnIndex,SQLUDTFIXED,(char*)&blob);
  __XDBC_HOOK_CHECK(return 0);

  IfxBlob* p_blob = new IfxBlob(__mp_conn);
  p_blob->swap(blob);
  return p_blob;
}

Clob* IfxResultSet::getClob(__XDBC_HOOK_DECLARE_MORE int columnIndex)  __XDBC_THROW_DECLARE(SQLException)
{
  __XDBC_INFORMIXRSET_CHECKNULL_POINTER(columnIndex);

  if(__cachedCollection->__vector_columns[columnIndex].__mi_type == SQLUDTFIXED)
  {    
    struct ifx_lo_ts* p_ec_lo_ts = &(__cachedCollection->__vector_defines[columnIndex].ifx_value.ec_lo_ts);
    return new IfxClob(__mp_conn,p_ec_lo_ts);
  }
  
  IfxClob Clob(__mp_conn);
  checkConversion(__XDBC_HOOK_INVOKE_MORE columnIndex,SQLUDTFIXED,(char*)&Clob);
  __XDBC_HOOK_CHECK(return 0);
  
  IfxClob* p_clob = new IfxClob(__mp_conn);
  p_clob->swap(Clob);
  return p_clob;
}

int IfxResultSet::findColumn(__XDBC_HOOK_DECLARE_MORE const string& columnName)  __XDBC_THROW_DECLARE(SQLException)
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
  __sb.append("Column '");
  __sb.append(columnName);
  __sb.append("' not found in SELECT-LIST");

  __XDBC_HOOK_RETURN(return -1,__sb.str());  
}

int IfxResultSet::getType(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{
  return __mi_resultSetType;
}

int IfxResultSet::getConcurrency(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{
  return __mi_resultSetConcurrency;
}

SQLImplementation* IfxResultSet::getImplementation(__XDBC_HOOK_DECLARE_ONCE)   __XDBC_THROW_DECLARE(SQLException)
{
  return __mp_impl;
}

bool IfxResultSet::checkConversion(__XDBC_HOOK_DECLARE_MORE int columnIndex,int outType,char* outBuffer) __XDBC_THROW_DECLARE(SQLException)
{
  if(__mi_cur_row == 0 || __mi_cur_row==-1)
  {
    __XDBC_HOOK_RETURN(return false,"not data indicated by current position,calling Traversal/Positioning first");
  }

  int inType=__cachedCollection->__vector_columns[columnIndex].__mi_type & SQLTYPE;

  //printf("--column_name : [%s] ,outType : [%d] ,inType : [%d] \n",__cachedCollection->__vector_columns[columnIndex].__ms_name.c_str(),outType,inType);

  return IfxConversion::checkConversion(__XDBC_HOOK_INVOKE_MORE "IfxResultSet",
                                        __cachedCollection->__p_rowbuf,__cachedCollection->__vector_defines[columnIndex],inType,
                                        outType,outBuffer,__cachedCollection->__vector_columns[columnIndex].__ms_name);
}

//---------------------------------------------------------------------
// Traversal/Positioning
//---------------------------------------------------------------------
  
bool IfxResultSet::isBeforeFirst(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE (SQLException)
{
  return (__mi_cur_row == 0);
}
    
bool IfxResultSet::isAfterLast(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE (SQLException)
{
  return (__mi_cur_row == -1);
}
 
bool IfxResultSet::isFirst(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE (SQLException)
{
  return (__mi_cur_row == 1);
}
 
bool IfxResultSet::isLast(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE (SQLException)
{
  int old_row_no=__mi_cur_row;
  last(__XDBC_HOOK_INVOKE_ONCE);
  __XDBC_HOOK_CHECK(return false);
  int new_row_no=__mi_cur_row;

  absolute(__XDBC_HOOK_INVOKE_MORE old_row_no); 
  __XDBC_HOOK_CHECK(return false);
  
  return (old_row_no == new_row_no);
}
 
bool IfxResultSet::first(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE (SQLException)
{
  if(__mb_isEmpty)
  {
    return false;
  }

  if(__mi_resultSetType == ResultSet::TYPE_FORWARD_ONLY)
  {
    StringBuffer __sb;
    __sb.append("not supported operation on resultset type {");
    __sb.append(__mi_resultSetType);
    __sb.append("}");
    
    __XDBC_HOOK_RETURN(return false,__sb.str());
  }

   __XDBC_INFORMIXRSET_CHECKCLOSED(__mb_isClosed);

  EXEC SQL BEGIN DECLARE SECTION;
    char* ec_cursor_name = (char*)__ms_cursor_name.c_str();
  EXEC SQL END DECLARE SECTION;

  EXEC SQL FETCH  FIRST :ec_cursor_name USING DESCRIPTOR __mp_ec_sqlda_columns;
  IfxConversion::checkException(__XDBC_HOOK_INVOKE_MORE SQLCODE,SQLSTATE,IFRESULTSET_CLASS_NAME);
  __XDBC_HOOK_CHECK(return false);
  
  if(SQLCODE == 100)
  {
    __mi_cur_row=0;
    return false;
  }

  __mi_cur_row=1;

  return true;
}
 
bool IfxResultSet::last(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE (SQLException)
{
  if(__mb_isEmpty)
  {
    return false;
  }

  if(__mi_resultSetType == ResultSet::TYPE_FORWARD_ONLY)
  {
    StringBuffer __sb;
    __sb.append("not supported operation on resultset type {");
    __sb.append(__mi_resultSetType);
    __sb.append("}");

    __XDBC_HOOK_RETURN(return false,__sb.str());
  }

   __XDBC_INFORMIXRSET_CHECKCLOSED(__mb_isClosed);
 
  EXEC SQL BEGIN DECLARE SECTION;
    char* ec_cursor_name = (char*)__ms_cursor_name.c_str();
  EXEC SQL END DECLARE SECTION;

  EXEC SQL FETCH LAST :ec_cursor_name  USING DESCRIPTOR __mp_ec_sqlda_columns;
  IfxConversion::checkException(__XDBC_HOOK_INVOKE_MORE SQLCODE,SQLSTATE,IFRESULTSET_CLASS_NAME);
  __XDBC_HOOK_CHECK(return false);
  
  if(SQLCODE == 100)
  {
    __mi_cur_row=-1;
    return false;
  }

//--------------------------------------------
// fix me !! HOW TO GET CURRENT POSITION ?????
// ---------------
// 2005-07-28 Powered By ZosaTapo
//--------------------------------------------
  __mi_cur_row = __mi_row_count;

  return true;
}
  
int IfxResultSet::getRow(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE (SQLException)
{
  return __mi_cur_row;
}
  
bool IfxResultSet::absolute(__XDBC_HOOK_DECLARE_MORE int row ) __XDBC_THROW_DECLARE (SQLException)
{
  if(__mb_isEmpty)
  {
    return false;
  }

  if(__mi_resultSetType == ResultSet::TYPE_FORWARD_ONLY)
  {
    StringBuffer __sb;
    __sb.append("not supported operation on resultset type {");
    __sb.append(__mi_resultSetType);
    __sb.append("}");

    __XDBC_HOOK_RETURN(return false,__sb.str());
  }

   __XDBC_INFORMIXRSET_CHECKCLOSED(__mb_isClosed);

  EXEC SQL BEGIN DECLARE SECTION;
    char* ec_cursor_name = (char*)__ms_cursor_name.c_str();
    int ec_row = row;
  EXEC SQL END DECLARE SECTION;

  EXEC SQL FETCH  ABSOLUTE :ec_row :ec_cursor_name USING DESCRIPTOR __mp_ec_sqlda_columns;
  IfxConversion::checkException(__XDBC_HOOK_INVOKE_MORE SQLCODE,SQLSTATE,IFRESULTSET_CLASS_NAME);
  __XDBC_HOOK_CHECK(return false);
  
  if(SQLCODE == 100)
  {
    __mi_cur_row = 0 ;
    return false;
  }

  __mi_cur_row=row;

  return true;
}
  
bool IfxResultSet::relative(__XDBC_HOOK_DECLARE_MORE int rows ) __XDBC_THROW_DECLARE (SQLException)
{
  if(__mb_isEmpty)
  {
    return false;
  }

  if(__mi_resultSetType == ResultSet::TYPE_FORWARD_ONLY)
  {
    StringBuffer __sb;
    __sb.append("not supported operation on resultset type {");
    __sb.append(__mi_resultSetType);
    __sb.append("}");

    __XDBC_HOOK_RETURN(return false,__sb.str());
  }

   __XDBC_INFORMIXRSET_CHECKCLOSED(__mb_isClosed);
 
  EXEC SQL BEGIN DECLARE SECTION;
    char* ec_cursor_name = (char*)__ms_cursor_name.c_str();
    int ec_rows = rows;
  EXEC SQL END DECLARE SECTION;

  EXEC SQL FETCH RELATIVE :ec_rows :ec_cursor_name USING DESCRIPTOR __mp_ec_sqlda_columns;
  IfxConversion::checkException(__XDBC_HOOK_INVOKE_MORE SQLCODE,SQLSTATE,IFRESULTSET_CLASS_NAME);
  __XDBC_HOOK_CHECK(return false);
    
  if(SQLCODE == 100)
  {
    if(rows>=0) { __mi_cur_row=-1; }
    else { __mi_cur_row=0; }

    return false;
  }

  __mi_cur_row+=rows;
  return true;
}
  
bool IfxResultSet::previous(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE (SQLException)
{
  if(__mb_isEmpty || (__mi_cur_row==0))
  {
    return false;
  }

  if(__mi_resultSetType == ResultSet::TYPE_FORWARD_ONLY)
  {
    StringBuffer __sb;
    __sb.append("not supported operation on resultset type {");
    __sb.append(__mi_resultSetType);
    __sb.append("}");

    __XDBC_HOOK_RETURN(return false,__sb.str());
  }
 
  __XDBC_INFORMIXRSET_CHECKCLOSED(__mb_isClosed);
 
  EXEC SQL BEGIN DECLARE SECTION;
    char* ec_cursor_name = (char*)__ms_cursor_name.c_str();
  EXEC SQL END DECLARE SECTION;

  EXEC SQL FETCH PREVIOUS :ec_cursor_name USING DESCRIPTOR __mp_ec_sqlda_columns;
  IfxConversion::checkException(__XDBC_HOOK_INVOKE_MORE SQLCODE,SQLSTATE,IFRESULTSET_CLASS_NAME);
  __XDBC_HOOK_CHECK(return false);
  
  if(SQLCODE == 100)
  {
    __mi_cur_row=0;
    return false;
  }

  __mi_cur_row--;
  return true;
}

void IfxResultSet::setFetchSize(__XDBC_HOOK_DECLARE_MORE int rows) __XDBC_THROW_DECLARE (SQLException)
{
}

int IfxResultSet::getFetchSize(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE (SQLException)
{
  return 0;
}

bool IfxResultSet::isSupported(__XDBC_HOOK_DECLARE_MORE int resultSetType, int resultSetConcurrency)  __XDBC_THROW_DECLARE(SQLException)
{
  if( (resultSetType!=TYPE_FORWARD_ONLY) && (resultSetType!=TYPE_SCROLL_INSENSITIVE ))
  {
    StringBuffer __sb;
    __sb.append("[IfxResultSet] not supported resultset type {");
    __sb.append(resultSetType);
    __sb.append("}");

    __XDBC_HOOK_RETURN(return false,__sb.str());
  }
  
  if(resultSetConcurrency!=CONCUR_READ_ONLY)
  {
    StringBuffer __sb;
    __sb.append("[IfxResultSet] not supported concurrency mode {");
    __sb.append(resultSetConcurrency);
    __sb.append("}");
    
    __XDBC_HOOK_RETURN(return false,__sb.str());
  }

  return true;
}

__OPENCODE_END_NAMESPACE
