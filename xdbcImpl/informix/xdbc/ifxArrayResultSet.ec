#include "xdbc/ifxArrayResultSet.h"
#include "xdbc/ifxConnection.h"
#include "xdbc/ifxArrayStatement.h"
#include "xdbc/ifxResultSetMetaData.h"

#include "xdbc/ifxClob.h"
#include "xdbc/ifxBlob.h"
#include "xdbc/ifxConversion.h"

#include "port/ioc_sbuf.h"
#include "port/utilities.h"
#include "port/file.h"

__OPENCODE_BEGIN_NAMESPACE

//-------------------------------------------------------------------------

#define IFXARRAYRESULTSET_CLASS_NAME "IfxArrayResultSet"

//-------------------------------------------------------------------------

IfxArrayResultSet::~IfxArrayResultSet()
{
  __XDBC_TRACE(IFXARRAYRESULTSET_CLASS_NAME,"destroy()");
}

IfxArrayResultSet::IfxArrayResultSet(IfxArrayStatement* pStmt,const string& cursor_name,const string& cursor_id,int resultSetType, int resultSetConcurrency,DataTable* __mp_dataTable,bool close_stmt):__ms_cursor_name(cursor_name),__ms_cursor_id(cursor_id)
{
  __mp_statement=pStmt;

  __mi_resultSetType=resultSetType;
  __mi_resultSetConcurrency=resultSetConcurrency;

  __mp_dataTable = __mp_dataTable;

  __mb_close_stmt = close_stmt;
  __mb_isEmpty = false;
  __mb_isClosed = false;

  __mp_ec_sqlda_columns = 0;
  __mi_column_count = 0;
}

void IfxArrayResultSet::initialize(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException)
{
  if(__mb_isEmpty)
  {
    return;
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
  IfxConversion::checkException(__XDBC_HOOK_INVOKE_MORE SQLCODE,SQLSTATE,IFXARRAYRESULTSET_CLASS_NAME);
  __XDBC_HOOK_CHECK(return);
      
  int numcols = 0; 
  if(__mp_ec_sqlda_columns != 0)
  {
    numcols = __mp_ec_sqlda_columns->sqld;
    IfxConversion::parseResultSetMetaData(__mp_ec_sqlda_columns,__vector_columns);
    __mi_column_count = numcols;
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
    
    ec_sel_cnt = numcols;
    EXEC SQL allocate descriptor :ec_cursor_desc_id with max :ec_sel_cnt;
    EXEC SQL describe :ec_cursor_id using sql descriptor :ec_cursor_desc_id;

    int sql_code = (int)SQLCODE;
    string sql_state= SQLSTATE;
    if(sql_code != 0 )
    {
      EXEC SQL deallocate descriptor :ec_cursor_desc_id;
    }
    IfxConversion::checkException(__XDBC_HOOK_INVOKE_MORE SQLCODE,SQLSTATE,IFXARRAYRESULTSET_CLASS_NAME);
    __XDBC_HOOK_CHECK(return);
    
    EXEC SQL get descriptor :ec_cursor_desc_id :ec_sel_cnt = COUNT;
    
    for(ec_i = 1; ec_i <= ec_sel_cnt; ++ec_i)
    {
       EXEC SQL get descriptor :ec_cursor_desc_id VALUE :ec_i 
               :ec_scale = SCALE , 
               :ec_precision = PRECISION , 
               :ec_nullable = NULLABLE;   

       __vector_columns[ec_i].__mi_scale = ec_scale;
       __vector_columns[ec_i].__mi_precision = ec_precision;
       __vector_columns[ec_i].__mi_nullable = ec_nullable;     
    }

    EXEC SQL deallocate descriptor :ec_cursor_desc_id;    
  }

  if(DriverManager::isTracingOn)
  {  
    StringBuffer __sb;
    __sb << "<init>|" << File::lineSeparator;
    
    for(int i=1;i <= __mi_column_count;++i)
    {
      __vector_columns[i].dump(__sb);
    }
    
    __XDBC_FORCED_TRACE(IFXARRAYRESULTSET_CLASS_NAME,__sb.str());
  }

//-------------------------------------------------------------------------
// 2. define DataTable data 
//-------------------------------------------------------------------------
  __mi_array_size = 1;

  int rowcount = 0;
  int colcount = 0;

  int __mi_max_size = __mp_statement->getMaxArraySize();

  if(__mp_dataTable != 0)
  {
    rowcount = __mp_dataTable->getRowCount();
    colcount = __mp_dataTable->getColumnCount();

    __mi_array_size = __M_min(__mi_max_size,rowcount);
  }

  if(DriverManager::isTracingOn)
  {
     StringBuffer sb;
     sb <<"executeQuery|rows[s/r/m] = ["<<__mi_array_size
        <<"/" <<rowcount<<"/"<<__mi_max_size <<"] , cols = ["<<colcount<<"]";
     
     __XDBC_FORCED_TRACE(IFXARRAYRESULTSET_CLASS_NAME,sb.str()); 
  }

}

ResultSetMetaData* IfxArrayResultSet::getMetaData(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException)
{
  return new IfxResultSetMetaData(__vector_columns);
}

int  IfxArrayResultSet::next(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException)
{
  if(__mp_dataTable == 0 || __mb_isEmpty)
  {
    return 0;
  }

  checkClosed(__XDBC_HOOK_INVOKE_ONCE);
  __XDBC_HOOK_CHECK(return false);
  
  EXEC SQL BEGIN DECLARE SECTION;
    char* ec_cursor_name = (char*)__ms_cursor_name.c_str(); 
  EXEC SQL END DECLARE SECTION; 

  int rowcount = __mp_dataTable->getRowCount();

  for(int i=1;i<=rowcount;++i)
  { 
    setDefineData(__XDBC_HOOK_INVOKE_MORE __mp_dataTable,i);
    __XDBC_HOOK_CHECK(return 0);
    
    EXEC SQL FETCH :ec_cursor_name USING DESCRIPTOR __mp_ec_sqlda_columns;
    IfxConversion::checkException(__XDBC_HOOK_INVOKE_MORE SQLCODE,SQLSTATE,IFXARRAYRESULTSET_CLASS_NAME);
    __XDBC_HOOK_CHECK(return -1);
    
    if(SQLCODE == 100)
    {
      return i - 1;
    }
  }

  return rowcount;
}
 
void IfxArrayResultSet::close()
{
  __XDBC_TRACE(IFXARRAYRESULTSET_CLASS_NAME,"close()");

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

void IfxArrayResultSet::destroy()
{
  delete this;
}

int IfxArrayResultSet::findColumn(__XDBC_HOOK_DECLARE_MORE const string& columnName) __XDBC_THROW_DECLARE(SQLException)
{
  if(__map_columns.empty())
  {
    for(int i = 1;i <= __mi_column_count; ++i)
    {
      __map_columns[StringsKit::toUpperCase(__vector_columns[i].__ms_name)] = i;
    }    
  }  
  
  OPENCODE_MAP<string,int>::iterator __pos = __map_columns.find(StringsKit::toUpperCase(columnName));
  if(__pos != __map_columns.end())
  {
    return __pos->second;
  } 

  StringBuffer sb;
  sb.append("Column '");
  sb.append(columnName);
  sb.append("' not found in SELECT-LIST");

  __XDBC_HOOK_RETURN(return -1,sb.str());  
}

void  IfxArrayResultSet::checkClosed(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException)
{
  if(__mb_isClosed)
  {
    __XDBC_HOOK_RETURN(return,"[IfxResultSet] resultset has always been closed");
  }
}

void  IfxArrayResultSet::setDefineData(__XDBC_HOOK_DECLARE_MORE DataTable* __mp_dataTable,int row) __XDBC_THROW_DECLARE(SQLException)
{
  if(__mp_dataTable == 0)
  {
    return;
  }

  int colcount = __mp_dataTable->getColumnCount();
 
  for(int i = 1; i <= colcount ; ++i)
  {
    setDefineData(__XDBC_HOOK_INVOKE_MORE __mp_dataTable,row,i);
    __XDBC_HOOK_CHECK(return);
  } 
}

void  IfxArrayResultSet::setDefineData(__XDBC_HOOK_DECLARE_MORE DataTable* __mp_dataTable,int row,int parameterIndex) __XDBC_THROW_DECLARE(SQLException)
{
  const char* colData = __mp_dataTable->getModel()->getDataBuffer() + __mp_dataTable->getColumnOffset(parameterIndex);
  int colSize = __mp_dataTable->getColumnSize(parameterIndex);
  int colSkip = __mp_dataTable->getColumnSkip(parameterIndex);
  int colType = 0;
  if(__mp_dataTable->getModel()->useColumnRawType(parameterIndex))
  {
    colType =  __mp_dataTable->getModel()->getColumnRawType(parameterIndex);
  }
  else
  {
    colType = IfxConversion::XDBC_TO_DATABASE(__mp_dataTable->getColumnType(parameterIndex));
  }

  colData += (row - 1) * colSkip;

  const char* indData = 0;
  int indSize = 0;
  int indSkip = 0;

  if(__mp_dataTable->getModel()->useColumnIndicator(parameterIndex))
  {
    indData=__mp_dataTable->getModel()->getIndicatorBuffer()+__mp_dataTable->getModel()->getColumnIndicatorOffset(parameterIndex
);
    indSize = __mp_dataTable->getModel()->getColumnIndicatorSize(parameterIndex);
    indSkip = __mp_dataTable->getModel()->getColumnIndicatorSkip(parameterIndex);
  }

  colData += (row - 1) * colSkip;
  indData += (row - 1) * indSkip;

  struct sqlvar_struct* p_ec_column = &(__mp_ec_sqlda_columns->sqlvar[parameterIndex-1]);

  p_ec_column->sqlind = (int2*)indData;
  p_ec_column->sqlilen = indSize;
  p_ec_column->sqlitype = SQLSMINT;

  p_ec_column->sqldata = (char*)colData;
  p_ec_column->sqllen = colSize;
  p_ec_column->sqltype = colType;
}

__OPENCODE_END_NAMESPACE
