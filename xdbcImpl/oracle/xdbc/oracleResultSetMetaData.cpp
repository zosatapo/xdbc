#include "xdbc/oracleResultSetMetaData.h"
#include "xdbc/oracleResultSet.h"
#include "xdbc/oracleConversion.h"

#include "port/ioc_sbuf.h"

__OPENCODE_BEGIN_NAMESPACE

OracleResultSetMetaData::OracleResultSetMetaData(const vector<OracleColumn>& __vector_columns)
:__vector_columns(__vector_columns)
{
  __mi_column_count = (int)(__vector_columns.size() - 1);
}

OracleResultSetMetaData::~OracleResultSetMetaData()
{
}

//----------------------------------------------
// ResultSetMetaData Interfaces
//----------------------------------------------
int OracleResultSetMetaData::getColumnCount(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{
  return __mi_column_count;
}

int OracleResultSetMetaData::isNullable(__XDBC_HOOK_DECLARE_MORE int column)  __XDBC_THROW_DECLARE(SQLException)
{
  checkColumnIndex(__XDBC_HOOK_INVOKE_MORE column);
  __XDBC_HOOK_CHECK(return -1);

  int is_nullable = __vector_columns[column].__mi_nullable;
  
  if(is_nullable == 1)
  {
    return ResultSetMetaData::columnNullable;
  }
  
  return ResultSetMetaData::columnNoNulls;
}

int OracleResultSetMetaData::isCharSemantics(__XDBC_HOOK_DECLARE_MORE int column)  __XDBC_THROW_DECLARE(SQLException)
{
  checkColumnIndex(__XDBC_HOOK_INVOKE_MORE column);
  __XDBC_HOOK_CHECK(return -1);

  if(__vector_columns[column].__mb_char_semantics)
  {
    return ResultSetMetaData::columnCharSemantics;
  }

  return ResultSetMetaData::columnNoCharSemantics;
}

int OracleResultSetMetaData::getColumnDisplaySize(__XDBC_HOOK_DECLARE_MORE int column)  __XDBC_THROW_DECLARE(SQLException)
{
  checkColumnIndex(__XDBC_HOOK_INVOKE_MORE column);
  __XDBC_HOOK_CHECK(return -1);

  if(__vector_columns[column].__mb_char_semantics)
  {
    return (__vector_columns[column].__mi_length )*(ORACLE_CHAR_TO_BYTE_MAX);
  }
  else
  {
    return __vector_columns[column].__mi_length;
  }
}
string OracleResultSetMetaData::getColumnName(__XDBC_HOOK_DECLARE_MORE int column)  __XDBC_THROW_DECLARE(SQLException)
{
  checkColumnIndex(__XDBC_HOOK_INVOKE_MORE column);
  __XDBC_HOOK_CHECK(return string());

  return __vector_columns[column].__ms_name;
}

string OracleResultSetMetaData::getSchemaName(__XDBC_HOOK_DECLARE_MORE int column)  __XDBC_THROW_DECLARE(SQLException)
{
  checkColumnIndex(__XDBC_HOOK_INVOKE_MORE column);
  __XDBC_HOOK_CHECK(return string());

  return __vector_columns[column].__ms_schema_name;
}

int OracleResultSetMetaData::getPrecision(__XDBC_HOOK_DECLARE_MORE int column)  __XDBC_THROW_DECLARE(SQLException)
{
  checkColumnIndex(__XDBC_HOOK_INVOKE_MORE column);
  __XDBC_HOOK_CHECK(return -1);

  return __vector_columns[column].__mi_precision;
}

int OracleResultSetMetaData::getScale(__XDBC_HOOK_DECLARE_MORE int column)  __XDBC_THROW_DECLARE(SQLException)
{
  checkColumnIndex(__XDBC_HOOK_INVOKE_MORE column);
  __XDBC_HOOK_CHECK(return -1);

  return __vector_columns[column].__mi_scale;
}


string OracleResultSetMetaData::getTableName(__XDBC_HOOK_DECLARE_MORE int column)  __XDBC_THROW_DECLARE(SQLException)
{
  checkColumnIndex(__XDBC_HOOK_INVOKE_MORE column);
  __XDBC_HOOK_CHECK(return string());

  return __vector_columns[column].__ms_table_name;
}

int OracleResultSetMetaData::getColumnType(__XDBC_HOOK_DECLARE_MORE int column)  __XDBC_THROW_DECLARE(SQLException)
{
  checkColumnIndex(__XDBC_HOOK_INVOKE_MORE column);
  __XDBC_HOOK_CHECK(return);

  return OracleConversion::SQLT_TO_XDBC(__vector_columns[column]);
}

string OracleResultSetMetaData::getColumnTypeName(__XDBC_HOOK_DECLARE_MORE int column)  __XDBC_THROW_DECLARE(SQLException)
{
  checkColumnIndex(__XDBC_HOOK_INVOKE_MORE column);
  __XDBC_HOOK_CHECK(return string());

  return __vector_columns[column].__ms_type_name; 

}

int OracleResultSetMetaData::getColumnRawType(__XDBC_HOOK_DECLARE_MORE int column)  __XDBC_THROW_DECLARE(SQLException)
{
  checkColumnIndex(__XDBC_HOOK_INVOKE_MORE column);
  __XDBC_HOOK_CHECK(return -1);
  
  return __vector_columns[column].__mi_type;
}

void OracleResultSetMetaData::checkColumnIndex(__XDBC_HOOK_DECLARE_MORE int column) __XDBC_THROW_DECLARE(SQLException)
{
  if(column < 1 || column > __mi_column_count)
  {
    StringBuffer __sb("[OracleResultSetMetaData] IndexOutOfBoundary : column index  must be in range (1,");
    __sb <<__mi_column_count <<") - "<< column;
    __XDBC_HOOK_RETURN(return,__sb.str());
  }
}

__OPENCODE_END_NAMESPACE
