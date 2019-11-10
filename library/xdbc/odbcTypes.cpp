#include "xdbc/odbcTypes.h"
#include "xdbc/xtypes.h"
#include "port/ioc_sbuf.h"
#include "port/ioc_map.h" 

__OPENCODE_BEGIN_NAMESPACE

#define ODBCTYPES_CLASS_NAME   "OdbcTypes"

IMPLEMENT_LIFETIME_BEGIN(OdbcTypes)
IMPLEMENT_LIFETIME_END(OdbcTypes)

OdbcTypes::OdbcTypes()
{    
//--------------------------------------------------------
// ODBC To XDBC Mapping
//--------------------------------------------------------
  odbc2XDBCTables.insert(pair<int,int>(SQL_CHAR,XDBC_TYPE_CHAR));
  odbc2XDBCTables.insert(pair<int,int>(SQL_VARCHAR,XDBC_TYPE_VARCHAR));
  odbc2XDBCTables.insert(pair<int,int>(SQL_LONGVARCHAR,XDBC_TYPE_LONGVARCHAR));

  odbc2XDBCTables.insert(pair<int,int>(SQL_BINARY,XDBC_TYPE_BINARY));
  odbc2XDBCTables.insert(pair<int,int>(SQL_VARBINARY,XDBC_TYPE_VARBINARY));
  odbc2XDBCTables.insert(pair<int,int>(SQL_LONGVARBINARY,XDBC_TYPE_LONGVARBINARY));

  odbc2XDBCTables.insert(pair<int,int>(SQL_BIT,XDBC_TYPE_BIT));
  odbc2XDBCTables.insert(pair<int,int>(SQL_TINYINT,XDBC_TYPE_TINYINT));
  odbc2XDBCTables.insert(pair<int,int>(SQL_SMALLINT,XDBC_TYPE_SMALLINT));
  odbc2XDBCTables.insert(pair<int,int>(SQL_INTEGER,XDBC_TYPE_INTEGER));
  odbc2XDBCTables.insert(pair<int,int>(SQL_BIGINT,XDBC_TYPE_BIGINT));
  
  odbc2XDBCTables.insert(pair<int,int>(SQL_REAL,XDBC_TYPE_REAL));
  odbc2XDBCTables.insert(pair<int,int>(SQL_FLOAT,XDBC_TYPE_FLOAT));
  odbc2XDBCTables.insert(pair<int,int>(SQL_DOUBLE,XDBC_TYPE_DOUBLE));
  
  odbc2XDBCTables.insert(pair<int,int>(SQL_NUMERIC,XDBC_TYPE_NUMERIC));
  odbc2XDBCTables.insert(pair<int,int>(SQL_DECIMAL,XDBC_TYPE_DECIMAL));
  
  odbc2XDBCTables.insert(pair<int,int>(SQL_DATE,XDBC_TYPE_DATE));                  
  odbc2XDBCTables.insert(pair<int,int>(SQL_TIME,XDBC_TYPE_TIME)); 
  odbc2XDBCTables.insert(pair<int,int>(SQL_TIMESTAMP,XDBC_TYPE_TIMESTAMP)); 
  odbc2XDBCTables.insert(pair<int,int>(SQL_TYPE_DATE,XDBC_TYPE_DATE));
  odbc2XDBCTables.insert(pair<int,int>(SQL_TYPE_TIME,XDBC_TYPE_TIME));
  odbc2XDBCTables.insert(pair<int,int>(SQL_TYPE_TIMESTAMP,XDBC_TYPE_TIMESTAMP));

  odbc2XDBCTables.insert(pair<int,int>(SQL_INTERVAL,XDBC_TYPE_INTERVAL)); 
  odbc2XDBCTables.insert(pair<int,int>(SQL_INTERVAL_YEAR,XDBC_TYPE_INTERVAL));
  odbc2XDBCTables.insert(pair<int,int>(SQL_INTERVAL_MONTH,XDBC_TYPE_INTERVAL));
  odbc2XDBCTables.insert(pair<int,int>(SQL_INTERVAL_DAY,XDBC_TYPE_INTERVAL));
  odbc2XDBCTables.insert(pair<int,int>(SQL_INTERVAL_HOUR,XDBC_TYPE_INTERVAL));
  odbc2XDBCTables.insert(pair<int,int>(SQL_INTERVAL_MINUTE,XDBC_TYPE_INTERVAL));
  odbc2XDBCTables.insert(pair<int,int>(SQL_INTERVAL_SECOND,XDBC_TYPE_INTERVAL));
  odbc2XDBCTables.insert(pair<int,int>(SQL_INTERVAL_YEAR_TO_MONTH,XDBC_TYPE_INTERVAL));
  odbc2XDBCTables.insert(pair<int,int>(SQL_INTERVAL_DAY_TO_HOUR,XDBC_TYPE_INTERVAL));
  odbc2XDBCTables.insert(pair<int,int>(SQL_INTERVAL_DAY_TO_MINUTE,XDBC_TYPE_INTERVAL));
  odbc2XDBCTables.insert(pair<int,int>(SQL_INTERVAL_DAY_TO_SECOND,XDBC_TYPE_INTERVAL));
  odbc2XDBCTables.insert(pair<int,int>(SQL_INTERVAL_HOUR_TO_MINUTE,XDBC_TYPE_INTERVAL));
  odbc2XDBCTables.insert(pair<int,int>(SQL_INTERVAL_HOUR_TO_SECOND,XDBC_TYPE_INTERVAL));
  odbc2XDBCTables.insert(pair<int,int>(SQL_INTERVAL_MINUTE_TO_SECOND,XDBC_TYPE_INTERVAL));  
  
//--------------------------------------------------------
// XDBC To ODBC Mapping
//--------------------------------------------------------
  xdbc2ODBCTables.insert(pair<int,int>(XDBC_TYPE_CHAR,SQL_CHAR));
  xdbc2ODBCTables.insert(pair<int,int>(XDBC_TYPE_VARCHAR,SQL_VARCHAR));
  xdbc2ODBCTables.insert(pair<int,int>(XDBC_TYPE_LONGVARCHAR,SQL_LONGVARCHAR));

  xdbc2ODBCTables.insert(pair<int,int>(XDBC_TYPE_BINARY,SQL_BINARY));
  xdbc2ODBCTables.insert(pair<int,int>(XDBC_TYPE_VARBINARY,SQL_VARBINARY));
  xdbc2ODBCTables.insert(pair<int,int>(XDBC_TYPE_LONGVARBINARY,SQL_LONGVARBINARY));
  
  xdbc2ODBCTables.insert(pair<int,int>(XDBC_TYPE_BIT,SQL_BIT));
  xdbc2ODBCTables.insert(pair<int,int>(XDBC_TYPE_TINYINT,SQL_TINYINT));
  xdbc2ODBCTables.insert(pair<int,int>(XDBC_TYPE_SMALLINT,SQL_SMALLINT));
  xdbc2ODBCTables.insert(pair<int,int>(XDBC_TYPE_INTEGER,SQL_INTEGER));
  xdbc2ODBCTables.insert(pair<int,int>(XDBC_TYPE_BIGINT,SQL_BIGINT));
  
  xdbc2ODBCTables.insert(pair<int,int>(XDBC_TYPE_REAL,SQL_REAL));
  xdbc2ODBCTables.insert(pair<int,int>(XDBC_TYPE_FLOAT,SQL_FLOAT));
  xdbc2ODBCTables.insert(pair<int,int>(XDBC_TYPE_DOUBLE,SQL_DOUBLE));

  xdbc2ODBCTables.insert(pair<int,int>(XDBC_TYPE_NUMERIC,SQL_NUMERIC));
  xdbc2ODBCTables.insert(pair<int,int>(XDBC_TYPE_DECIMAL,SQL_DECIMAL));
  
  xdbc2ODBCTables.insert(pair<int,int>(XDBC_TYPE_DATE,SQL_TYPE_DATE));
  xdbc2ODBCTables.insert(pair<int,int>(XDBC_TYPE_TIME,SQL_TYPE_TIME));
  xdbc2ODBCTables.insert(pair<int,int>(XDBC_TYPE_TIMESTAMP,SQL_TYPE_TIMESTAMP));
  xdbc2ODBCTables.insert(pair<int,int>(XDBC_TYPE_INTERVAL,SQL_INTERVAL));

//--------------------------------------------------------
// SQL_XXX To ODBC Type Name Mapping
//--------------------------------------------------------
  odbc2TypeNames.insert(pair<int,string>(SQL_CHAR,"CHAR"));
  odbc2TypeNames.insert(pair<int,string>(SQL_VARCHAR,"VARCHAR"));
  odbc2TypeNames.insert(pair<int,string>(SQL_LONGVARCHAR,"VARCHAR"));
  
  odbc2TypeNames.insert(pair<int,string>(SQL_BINARY,"BINARY"));
  odbc2TypeNames.insert(pair<int,string>(SQL_VARBINARY,"VARBINARY"));
  odbc2TypeNames.insert(pair<int,string>(SQL_LONGVARBINARY,"VARBINARY"));
  
  odbc2TypeNames.insert(pair<int,string>(SQL_TINYINT,"TINYINT"));
  odbc2TypeNames.insert(pair<int,string>(SQL_SMALLINT,"SMALLINT"));
  odbc2TypeNames.insert(pair<int,string>(SQL_INTEGER,"INTEGER"));  
  odbc2TypeNames.insert(pair<int,string>(SQL_BIGINT,"BIGINT"));

  odbc2TypeNames.insert(pair<int,string>(SQL_REAL,"REAL"));
  odbc2TypeNames.insert(pair<int,string>(SQL_FLOAT,"FLOAT"));
  odbc2TypeNames.insert(pair<int,string>(SQL_DOUBLE,"DOUBLE"));

  odbc2TypeNames.insert(pair<int,string>(SQL_NUMERIC,"NUMERIC"));
  odbc2TypeNames.insert(pair<int,string>(SQL_DECIMAL,"DECIMAL"));

  odbc2TypeNames.insert(pair<int,string>(SQL_DATE,"DATE"));
  odbc2TypeNames.insert(pair<int,string>(SQL_TIME,"TIME"));
  odbc2TypeNames.insert(pair<int,string>(SQL_TIMESTAMP,"TIMESTAMP")); 
  odbc2TypeNames.insert(pair<int,string>(SQL_TYPE_DATE,"DATE"));
  odbc2TypeNames.insert(pair<int,string>(SQL_TYPE_TIME,"TIME"));
  odbc2TypeNames.insert(pair<int,string>(SQL_TYPE_TIMESTAMP,"TIMESTAMP"));

  odbc2TypeNames.insert(pair<int,string>(SQL_INTERVAL,"INTERVAL"));
  odbc2TypeNames.insert(pair<int,string>(SQL_INTERVAL_YEAR,"INTERVAL"));
  odbc2TypeNames.insert(pair<int,string>(SQL_INTERVAL_MONTH,"INTERVAL"));
  odbc2TypeNames.insert(pair<int,string>(SQL_INTERVAL_DAY,"INTERVAL"));
  odbc2TypeNames.insert(pair<int,string>(SQL_INTERVAL_HOUR,"INTERVAL"));
  odbc2TypeNames.insert(pair<int,string>(SQL_INTERVAL_MINUTE,"INTERVAL"));
  odbc2TypeNames.insert(pair<int,string>(SQL_INTERVAL_SECOND,"INTERVAL"));
  odbc2TypeNames.insert(pair<int,string>(SQL_INTERVAL_YEAR_TO_MONTH,"INTERVAL"));
  odbc2TypeNames.insert(pair<int,string>(SQL_INTERVAL_DAY_TO_HOUR,"INTERVAL"));
  odbc2TypeNames.insert(pair<int,string>(SQL_INTERVAL_DAY_TO_MINUTE,"INTERVAL"));
  odbc2TypeNames.insert(pair<int,string>(SQL_INTERVAL_DAY_TO_SECOND,"INTERVAL"));
  odbc2TypeNames.insert(pair<int,string>(SQL_INTERVAL_HOUR_TO_MINUTE,"INTERVAL"));
  odbc2TypeNames.insert(pair<int,string>(SQL_INTERVAL_HOUR_TO_SECOND,"INTERVAL"));
  odbc2TypeNames.insert(pair<int,string>(SQL_INTERVAL_MINUTE_TO_SECOND,"INTERVAL"));
}

int OdbcTypes::xdbc_cnvt_odbc(struct ODBC_DRIVER_OPTIONS* odbcOptions,int xdbcType)
{
  OPENCODE_MAP<int,int>::iterator pos = xdbc2ODBCTables.find(xdbcType);

  if(pos != xdbc2ODBCTables.end())
  {
    return pos->second;
  }

  return SQL_LONGVARBINARY;
}

int OdbcTypes::odbc_cnvt_xdbc(struct ODBC_DRIVER_OPTIONS* odbcOptions,int odbcType)
{
  OPENCODE_MAP<int,int>::iterator pos = odbc2XDBCTables.find(odbcType);

  if(pos != odbc2XDBCTables.end())
  {
    return pos->second;
  }

  return Types::OTHER;
}

int OdbcTypes::odbc_cnvt_xdbc(struct ODBC_DRIVER_OPTIONS* odbcOptions,const ODBC_COLUMN& column)
{
  return odbc_cnvt_xdbc(odbcOptions,column.__mi_type,column.__mi_nullable,column.__mi_scale);
}

int OdbcTypes::odbc_cnvt_xdbc(struct ODBC_DRIVER_OPTIONS* odbcOptions,int odbcType,int precision,int scale)
{
  OPENCODE_MAP<int,int>::iterator pos = odbc2XDBCTables.find(odbcType);

  if(pos != odbc2XDBCTables.end())
  {
    return pos->second;
  }

  return Types::OTHER;
}

string OdbcTypes::getTypeName(struct ODBC_DRIVER_OPTIONS* odbcOptions,const ODBC_COLUMN& column)
{
  return getTypeName(odbcOptions,column.__mi_type,column.__mi_nullable,column.__mi_scale);
}

string  OdbcTypes::getTypeName(struct ODBC_DRIVER_OPTIONS* odbcOptions,int odbcType)
{
  if((odbcOptions != 0) && (odbcOptions->__options_ODBCGETTYPENAME != 0))
  {
    SQLCHAR __type_name[1024] = {0};
    SQLINTEGER __type_len = 1024;
    
    if( (odbcOptions->__options_ODBCGETTYPENAME)(odbcType,__type_name,__type_len,&__type_len) == SQL_SUCCESS)
    {
      return string((const char*)__type_name,__type_len);
    }
  } 
  
  OPENCODE_MAP<int,string>::iterator pos = odbc2TypeNames.find(odbcType);
  if(pos != odbc2TypeNames.end())
  {
    return pos->second;
  }
  
  return string("UNKNOWN");
}

string  OdbcTypes::getTypeName(struct ODBC_DRIVER_OPTIONS* odbcOptions,int odbcType,int precision,int scale)
{
  if((odbcOptions != 0) && (odbcOptions->__options_ODBCGETTYPENAME != 0))
  {
    SQLCHAR __type_name[1024] = {0};
    SQLINTEGER __type_len = 1024;

    if( (odbcOptions->__options_ODBCGETTYPENAME)(odbcType,__type_name,__type_len,&__type_len) == SQL_SUCCESS)
    {
      return string((const char*)__type_name,__type_len);
    }
  }
    
  OPENCODE_MAP<int,string>::iterator pos = odbc2TypeNames.find(odbcType);
  if(pos != odbc2TypeNames.end())
  {
    return pos->second;
  }  
    
  return string("UNKNOWN");
}

__OPENCODE_END_NAMESPACE

