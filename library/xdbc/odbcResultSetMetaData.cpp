#include "xdbc/odbcResultSetMetaData.h"
#include "xdbc/odbcTypes.h"
#include "port/ioc_sbuf.h"

__OPENCODE_BEGIN_NAMESPACE

OdbcResultSetMetaData::OdbcResultSetMetaData(const vector<ODBC_COLUMN>& vectorColumnsValue,struct ODBC_DRIVER_OPTIONS* odbcOptions)
:__vector_columns(vectorColumnsValue),__odbc_options(odbcOptions)
{
  __mi_column_count = (int)vectorColumnsValue.size() - 1;
  __odbcTypes = &(OdbcTypesImpl::getInstance());
}

OdbcResultSetMetaData::~OdbcResultSetMetaData()
{
}

int OdbcResultSetMetaData::getColumnCount(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException)
{
  return __mi_column_count;
}

int OdbcResultSetMetaData::isNullable(__XDBC_HOOK_DECLARE_MORE int column) __XDBC_THROW_DECLARE(SQLException)
{
  checkColumnIndex(__XDBC_HOOK_INVOKE_MORE column);
  __XDBC_HOOK_CHECK(return 0);

  int is_nullable = __vector_columns[column].__mi_nullable;
  
  if(is_nullable == SQL_NULLABLE)
  {
    return ResultSetMetaData::columnNullable;
  }
  else if(is_nullable == SQL_NO_NULLS)
  { 
    return ResultSetMetaData::columnNoNulls;
  }

  return ResultSetMetaData::columnNullableUnknown;
}

int OdbcResultSetMetaData::isCharSemantics(__XDBC_HOOK_DECLARE_MORE int column) __XDBC_THROW_DECLARE(SQLException)
{
  checkColumnIndex(__XDBC_HOOK_INVOKE_MORE column);
  __XDBC_HOOK_CHECK(return 0);

  if(__vector_columns[column].__mb_char_semantics)
  {
    return ResultSetMetaData::columnCharSemantics;
  }

  return ResultSetMetaData::columnNoCharSemantics;
}

int OdbcResultSetMetaData::getColumnDisplaySize(__XDBC_HOOK_DECLARE_MORE int column) __XDBC_THROW_DECLARE(SQLException)
{
  checkColumnIndex(__XDBC_HOOK_INVOKE_MORE column);
  __XDBC_HOOK_CHECK(return 0);

  if(__vector_columns[column].__mb_char_semantics)
  {
    return (__vector_columns[column].__mi_length )*(4);
  }
  else
  {
    return __vector_columns[column].__mi_length;
  }
}
string OdbcResultSetMetaData::getColumnName(__XDBC_HOOK_DECLARE_MORE int column) __XDBC_THROW_DECLARE(SQLException)
{
  checkColumnIndex(__XDBC_HOOK_INVOKE_MORE column);
  __XDBC_HOOK_CHECK(return string());

  return __vector_columns[column].__ms_name;
}

string OdbcResultSetMetaData::getSchemaName(__XDBC_HOOK_DECLARE_MORE int column) __XDBC_THROW_DECLARE(SQLException)
{
  checkColumnIndex(__XDBC_HOOK_INVOKE_MORE column);
  __XDBC_HOOK_CHECK(return string());

  return __vector_columns[column].__ms_schema_name;
}

int OdbcResultSetMetaData::getPrecision(__XDBC_HOOK_DECLARE_MORE int column) __XDBC_THROW_DECLARE(SQLException)
{
  checkColumnIndex(__XDBC_HOOK_INVOKE_MORE column);
  __XDBC_HOOK_CHECK(return 0);

  return __vector_columns[column].__mi_nullable;
}

int OdbcResultSetMetaData::getScale(__XDBC_HOOK_DECLARE_MORE int column) __XDBC_THROW_DECLARE(SQLException)
{
  checkColumnIndex(__XDBC_HOOK_INVOKE_MORE column);
  __XDBC_HOOK_CHECK(return 0);

  return __vector_columns[column].__mi_scale;
}


string OdbcResultSetMetaData::getTableName(__XDBC_HOOK_DECLARE_MORE int column) __XDBC_THROW_DECLARE(SQLException)
{
  checkColumnIndex(__XDBC_HOOK_INVOKE_MORE column);
  __XDBC_HOOK_CHECK(return string());
  
  return __vector_columns[column].__ms_table_name;
}

int OdbcResultSetMetaData::getColumnType(__XDBC_HOOK_DECLARE_MORE int column) __XDBC_THROW_DECLARE(SQLException)
{
  checkColumnIndex(__XDBC_HOOK_INVOKE_MORE column);
  __XDBC_HOOK_CHECK(return 0);
  
  return __odbcTypes->odbc_cnvt_xdbc(__odbc_options,__vector_columns[column]);
}

string OdbcResultSetMetaData::getColumnTypeName(__XDBC_HOOK_DECLARE_MORE int column) __XDBC_THROW_DECLARE(SQLException)
{
  checkColumnIndex(__XDBC_HOOK_INVOKE_MORE column);
  __XDBC_HOOK_CHECK(return string());

  return __vector_columns[column].__ms_type_name; 

}

int OdbcResultSetMetaData::getColumnRawType(__XDBC_HOOK_DECLARE_MORE int column) __XDBC_THROW_DECLARE(SQLException)
{
  checkColumnIndex(__XDBC_HOOK_INVOKE_MORE column);
  __XDBC_HOOK_CHECK(return 0);
  
  return __vector_columns[column].__mi_type;
}

void OdbcResultSetMetaData::checkColumnIndex(__XDBC_HOOK_DECLARE_MORE int column) __XDBC_THROW_DECLARE(SQLException)
{
  if(column < 1 || column > __mi_column_count)
  {
    StringBuffer __sb;
    __sb  << ("[OdbcResultSetMetaData] IndexOutOfBoundary : column index  must be in range (1,");
    __sb << __mi_column_count <<") - " << column;
    __XDBC_HOOK_RETURN(return,__sb.str());
  }
}

__OPENCODE_END_NAMESPACE
