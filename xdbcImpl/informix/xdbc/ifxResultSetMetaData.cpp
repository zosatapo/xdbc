#include "xdbc/ifxResultSetMetaData.h"
#include "xdbc/ifxResultSet.h"
#include "xdbc/ifxConversion.h"

#include "port/ioc_sbuf.h"

__OPENCODE_BEGIN_NAMESPACE

IfxResultSetMetaData::IfxResultSetMetaData(const vector<IfxColumn>& __vector_columns)
:__vector_columns(__vector_columns)
{
  __mi_column_count = (int)(__vector_columns.size() - 1);
}

IfxResultSetMetaData::~IfxResultSetMetaData()
{
}

//----------------------------------------------
// ResultSetMetaData Interfaces
//----------------------------------------------
int IfxResultSetMetaData::getColumnCount(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{
  return __mi_column_count;
}

int IfxResultSetMetaData::isNullable(__XDBC_HOOK_DECLARE_MORE int column)  __XDBC_THROW_DECLARE(SQLException)
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

int IfxResultSetMetaData::isCharSemantics(__XDBC_HOOK_DECLARE_MORE int column)  __XDBC_THROW_DECLARE(SQLException)
{
  checkColumnIndex(__XDBC_HOOK_INVOKE_MORE column);
  __XDBC_HOOK_CHECK(return -1);
  
  if(__vector_columns[column].__mb_char_semantics)
  {
    return ResultSetMetaData::columnCharSemantics;
  }

  return ResultSetMetaData::columnNoCharSemantics;
}

int IfxResultSetMetaData::getColumnDisplaySize(__XDBC_HOOK_DECLARE_MORE int column)  __XDBC_THROW_DECLARE(SQLException)
{
  checkColumnIndex(__XDBC_HOOK_INVOKE_MORE column);
  __XDBC_HOOK_CHECK(return -1);
  
  if(__vector_columns[column].__mb_char_semantics)
  {
    return (__vector_columns[column].__mi_length )*(INFORMIX_CHAR_TO_BYTE_MAX);
  }
  else
  {
    return __vector_columns[column].__mi_length;
  }
}
string IfxResultSetMetaData::getColumnName(__XDBC_HOOK_DECLARE_MORE int column)  __XDBC_THROW_DECLARE(SQLException)
{
  checkColumnIndex(__XDBC_HOOK_INVOKE_MORE column);
  __XDBC_HOOK_CHECK(return string());
  
  return __vector_columns[column].__ms_name;
}

string IfxResultSetMetaData::getSchemaName(__XDBC_HOOK_DECLARE_MORE int column)  __XDBC_THROW_DECLARE(SQLException)
{
  checkColumnIndex(__XDBC_HOOK_INVOKE_MORE column);
  __XDBC_HOOK_CHECK(return string());
  
  return __vector_columns[column].__ms_schema_name;
}

int IfxResultSetMetaData::getPrecision(__XDBC_HOOK_DECLARE_MORE int column)  __XDBC_THROW_DECLARE(SQLException)
{
  checkColumnIndex(__XDBC_HOOK_INVOKE_MORE column);
  __XDBC_HOOK_CHECK(return -1);
  
  return __vector_columns[column].__mi_precision;
}

int IfxResultSetMetaData::getScale(__XDBC_HOOK_DECLARE_MORE int column)  __XDBC_THROW_DECLARE(SQLException)
{
  checkColumnIndex(__XDBC_HOOK_INVOKE_MORE column);
  __XDBC_HOOK_CHECK(return -1);
  
  return __vector_columns[column].__mi_scale;
}


string IfxResultSetMetaData::getTableName(__XDBC_HOOK_DECLARE_MORE int column)  __XDBC_THROW_DECLARE(SQLException)
{
  checkColumnIndex(__XDBC_HOOK_INVOKE_MORE column);
  __XDBC_HOOK_CHECK(return string());
  
  return __vector_columns[column].__ms_table_name;
}

int IfxResultSetMetaData::getColumnType(__XDBC_HOOK_DECLARE_MORE int column)  __XDBC_THROW_DECLARE(SQLException)
{
  checkColumnIndex(__XDBC_HOOK_INVOKE_MORE column);
  __XDBC_HOOK_CHECK(return -1);
  
  return IfxConversion::DATABASE_TO_XDBC(__vector_columns[column]);
}

string IfxResultSetMetaData::getColumnTypeName(__XDBC_HOOK_DECLARE_MORE int column)  __XDBC_THROW_DECLARE(SQLException)
{
  checkColumnIndex(__XDBC_HOOK_INVOKE_MORE column);
  __XDBC_HOOK_CHECK(return string());

  return __vector_columns[column].__ms_type_name; 
}

int IfxResultSetMetaData::getColumnRawType(__XDBC_HOOK_DECLARE_MORE int column)  __XDBC_THROW_DECLARE(SQLException)
{
  checkColumnIndex(__XDBC_HOOK_INVOKE_MORE column);
  __XDBC_HOOK_CHECK(return -1);
  
  return __vector_columns[column].__mi_type;
}

void IfxResultSetMetaData::checkColumnIndex(__XDBC_HOOK_DECLARE_MORE int column) __XDBC_THROW_DECLARE(SQLException)
{
  if(column < 1 || column > __mi_column_count)
  {
    StringBuffer __sb("[IfxResultSetMetaData] IndexOutOfBoundary : column index  must be in range (1,");
    __sb << __mi_column_count << ") - "<< column;
    __XDBC_HOOK_RETURN(return,__sb.str());
  }
}

__OPENCODE_END_NAMESPACE
