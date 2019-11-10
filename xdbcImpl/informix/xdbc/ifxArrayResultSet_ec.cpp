#include <sqlhdr.h>
#include <sqliapi.h>
#line 1 "ifxArrayResultSet.ec"
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

/*
 *   EXEC SQL BEGIN DECLARE SECTION;
 */
#line 55 "ifxArrayResultSet.ec"
#line 56 "ifxArrayResultSet.ec"
  char *ec_cursor_name;
  char *ec_cursor_id;
/*
 *   EXEC SQL END DECLARE SECTION;
 */
#line 58 "ifxArrayResultSet.ec"


  ec_cursor_name  = (char*)__ms_cursor_name.c_str();
  ec_cursor_id    = (char*)__ms_cursor_id.c_str();

/*
 *   EXEC SQL DESCRIBE :ec_cursor_id into __mp_ec_sqlda_columns;
 */
#line 63 "ifxArrayResultSet.ec"
  {
#line 63 "ifxArrayResultSet.ec"
  sqli_describe_stmt(ESQLINTVERSION, sqli_curs_locate(ESQLINTVERSION, ec_cursor_id, 257), &__mp_ec_sqlda_columns, 0);
#line 63 "ifxArrayResultSet.ec"
  }
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
/*
 *     EXEC SQL BEGIN DECLARE SECTION;
 */
#line 78 "ifxArrayResultSet.ec"
#line 79 "ifxArrayResultSet.ec"
  char *ec_cursor_desc_id = 0;
mint ec_sel_cnt = 0;
mint ec_i = 0;
short ec_scale = 0;
short ec_precision = 0;
short ec_nullable = 0;
/*
 *     EXEC SQL END DECLARE SECTION;
 */
#line 85 "ifxArrayResultSet.ec"


    ec_cursor_desc_id = (char*)str_cursor_desc_id.c_str();

    ec_sel_cnt = numcols;
/*
 *     EXEC SQL allocate descriptor :ec_cursor_desc_id with max :ec_sel_cnt;
 */
#line 90 "ifxArrayResultSet.ec"
  {
#line 90 "ifxArrayResultSet.ec"
  sqli_desc_alloc(ec_cursor_desc_id, ec_sel_cnt);
#line 90 "ifxArrayResultSet.ec"
  }
/*
 *     EXEC SQL describe :ec_cursor_id using sql descriptor :ec_cursor_desc_id;
 */
#line 91 "ifxArrayResultSet.ec"
  {
#line 91 "ifxArrayResultSet.ec"
  sqli_describe_stmt(ESQLINTVERSION, sqli_curs_locate(ESQLINTVERSION, ec_cursor_id, 257), (ifx_sqlda_t **)0, ec_cursor_desc_id);
#line 91 "ifxArrayResultSet.ec"
  }

    int sql_code = (int)SQLCODE;
    string sql_state= SQLSTATE;
    if(sql_code != 0 )
    {
/*
 *       EXEC SQL deallocate descriptor :ec_cursor_desc_id;
 */
#line 97 "ifxArrayResultSet.ec"
  {
#line 97 "ifxArrayResultSet.ec"
  sqli_desc_dealloc(ec_cursor_desc_id);
#line 97 "ifxArrayResultSet.ec"
  }
    }
    IfxConversion::checkException(__XDBC_HOOK_INVOKE_MORE SQLCODE,SQLSTATE,IFXARRAYRESULTSET_CLASS_NAME);
    __XDBC_HOOK_CHECK(return);

/*
 *     EXEC SQL get descriptor :ec_cursor_desc_id :ec_sel_cnt = COUNT;
 */
#line 102 "ifxArrayResultSet.ec"
  {
#line 102 "ifxArrayResultSet.ec"
  static ifx_hostvar_t _SQhtab[] =
    {
      { 0, 0, 102, sizeof(ec_sel_cnt), 0, 0, 0, 0 }, 
      { 0, 0, 0, 0, 0, 0, 0, 0 }
#line 102 "ifxArrayResultSet.ec"
    };
#line 102 "ifxArrayResultSet.ec"
  _SQhtab[0].hostaddr = (char *)&ec_sel_cnt;
#line 102 "ifxArrayResultSet.ec"
  sqli_desc_get(ESQLINTVERSION, ec_cursor_desc_id, -1, _SQhtab, 0);
#line 102 "ifxArrayResultSet.ec"
  }

    for(ec_i = 1; ec_i <= ec_sel_cnt; ++ec_i)
    {
/*
 *        EXEC SQL get descriptor :ec_cursor_desc_id VALUE :ec_i
 *                :ec_scale = SCALE ,
 *                :ec_precision = PRECISION ,
 *                :ec_nullable = NULLABLE;
 */
#line 106 "ifxArrayResultSet.ec"
  {
#line 109 "ifxArrayResultSet.ec"
  static ifx_hostvar_t _SQhtab[] =
    {
      { 0, 8, 101, sizeof(ec_scale), 0, 0, 0, 0 }, 
      { 0, 3, 101, sizeof(ec_precision), 0, 0, 0, 0 }, 
      { 0, 4, 101, sizeof(ec_nullable), 0, 0, 0, 0 }, 
      { 0, 0, 0, 0, 0, 0, 0, 0 }
#line 109 "ifxArrayResultSet.ec"
    };
#line 109 "ifxArrayResultSet.ec"
  _SQhtab[0].hostaddr = (char *)&ec_scale;
#line 109 "ifxArrayResultSet.ec"
  _SQhtab[1].hostaddr = (char *)&ec_precision;
#line 109 "ifxArrayResultSet.ec"
  _SQhtab[2].hostaddr = (char *)&ec_nullable;
#line 109 "ifxArrayResultSet.ec"
  sqli_desc_get(ESQLINTVERSION, ec_cursor_desc_id, ec_i, _SQhtab, 0);
#line 109 "ifxArrayResultSet.ec"
  }

       __vector_columns[ec_i].__mi_scale = ec_scale;
       __vector_columns[ec_i].__mi_precision = ec_precision;
       __vector_columns[ec_i].__mi_nullable = ec_nullable;
    }

/*
 *     EXEC SQL deallocate descriptor :ec_cursor_desc_id;
 */
#line 116 "ifxArrayResultSet.ec"
  {
#line 116 "ifxArrayResultSet.ec"
  sqli_desc_dealloc(ec_cursor_desc_id);
#line 116 "ifxArrayResultSet.ec"
  }
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

/*
 *   EXEC SQL BEGIN DECLARE SECTION;
 */
#line 176 "ifxArrayResultSet.ec"
#line 177 "ifxArrayResultSet.ec"
  char *ec_cursor_name = (char*)__ms_cursor_name.c_str();
/*
 *   EXEC SQL END DECLARE SECTION;
 */
#line 178 "ifxArrayResultSet.ec"


  int rowcount = __mp_dataTable->getRowCount();

  for(int i=1;i<=rowcount;++i)
  {
    setDefineData(__XDBC_HOOK_INVOKE_MORE __mp_dataTable,i);
    __XDBC_HOOK_CHECK(return 0);

/*
 *     EXEC SQL FETCH :ec_cursor_name USING DESCRIPTOR __mp_ec_sqlda_columns;
 */
#line 187 "ifxArrayResultSet.ec"
  {
#line 187 "ifxArrayResultSet.ec"
  static _FetchSpec _FS0 = { 0, 1, 0 };
  sqli_curs_fetch(ESQLINTVERSION, sqli_curs_locate(ESQLINTVERSION, ec_cursor_name, 256), (ifx_sqlda_t *)0, __mp_ec_sqlda_columns, (char *)0, &_FS0);
#line 187 "ifxArrayResultSet.ec"
  }
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

/*
 *   EXEC SQL BEGIN DECLARE SECTION;
 */
#line 215 "ifxArrayResultSet.ec"
#line 216 "ifxArrayResultSet.ec"
  char *ec_cursor_name;
  char *ec_cursor_id;
/*
 *   EXEC SQL END DECLARE SECTION;
 */
#line 218 "ifxArrayResultSet.ec"


  ec_cursor_name  = (char*)__ms_cursor_name.c_str();
  ec_cursor_id    = (char*)__ms_cursor_id.c_str();

/*
 *   EXEC SQL CLOSE :ec_cursor_name;
 */
#line 223 "ifxArrayResultSet.ec"
  {
#line 223 "ifxArrayResultSet.ec"
  sqli_curs_close(ESQLINTVERSION, sqli_curs_locate(ESQLINTVERSION, ec_cursor_name, 256));
#line 223 "ifxArrayResultSet.ec"
  }
/*
 *   EXEC SQL FREE :ec_cursor_name;
 */
#line 224 "ifxArrayResultSet.ec"
  {
#line 224 "ifxArrayResultSet.ec"
  sqli_curs_free(ESQLINTVERSION, sqli_curs_locate(ESQLINTVERSION, ec_cursor_name, 258));
#line 224 "ifxArrayResultSet.ec"
  }

  if(__mb_close_stmt)
  {
/*
 *     EXEC SQL FREE :ec_cursor_id;
 */
#line 228 "ifxArrayResultSet.ec"
  {
#line 228 "ifxArrayResultSet.ec"
  sqli_curs_free(ESQLINTVERSION, sqli_curs_locate(ESQLINTVERSION, ec_cursor_id, 258));
#line 228 "ifxArrayResultSet.ec"
  }
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

#line 330 "ifxArrayResultSet.ec"
