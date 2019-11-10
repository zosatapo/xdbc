#include "xdbc/oracleTypes.h"
#include "xdbc/xtypes.h"

#include "port/ioc_sbuf.h"
#include "port/ioc_map.h" 

__OPENCODE_BEGIN_NAMESPACE

#define ORACLETYPES_CLASS_NAME  "OracleTypes"

OracleTypes OracleTypes::m_oracleTypes;
//--------------------------------------------------------

OracleTypes::OracleTypes()
{
//--------------------------------------------------------
// Oracle To XDBC Mapping
//--------------------------------------------------------
  oracle2XDBCTables.insert(pair<int,int>(SQLT_INT,XDBC_TYPE_BIGINT));

  oracle2XDBCTables.insert(pair<int,int>(SQLT_NUM,XDBC_TYPE_NUMERIC));
  oracle2XDBCTables.insert(pair<int,int>(SQLT_VNU,XDBC_TYPE_NUMERIC));
  oracle2XDBCTables.insert(pair<int,int>(SQLT_FLT,XDBC_TYPE_NUMERIC));

  oracle2XDBCTables.insert(pair<int,int>(SQLT_IBFLOAT,XDBC_TYPE_FLOAT));
  oracle2XDBCTables.insert(pair<int,int>(SQLT_IBDOUBLE,XDBC_TYPE_DOUBLE));

  oracle2XDBCTables.insert(pair<int,int>(SQLT_BFLOAT,XDBC_TYPE_FLOAT));
  oracle2XDBCTables.insert(pair<int,int>(SQLT_BDOUBLE,XDBC_TYPE_DOUBLE));

  oracle2XDBCTables.insert(pair<int,int>(SQLT_TIMESTAMP,XDBC_TYPE_TIMESTAMP));
  oracle2XDBCTables.insert(pair<int,int>(SQLT_TIMESTAMP_TZ,XDBC_TYPE_TIMESTAMP));
  oracle2XDBCTables.insert(pair<int,int>(SQLT_TIMESTAMP_LTZ,XDBC_TYPE_TIMESTAMP));

  oracle2XDBCTables.insert(pair<int,int>(SQLT_INTERVAL_YM,XDBC_TYPE_INTERVAL));
  oracle2XDBCTables.insert(pair<int,int>(SQLT_INTERVAL_DS,XDBC_TYPE_INTERVAL));
  oracle2XDBCTables.insert(pair<int,int>(SQLT_DATE,XDBC_TYPE_DATE));
  oracle2XDBCTables.insert(pair<int,int>(SQLT_DAT,XDBC_TYPE_DATE));

  oracle2XDBCTables.insert(pair<int,int>(SQLT_CLOB,XDBC_TYPE_CLOB));

  oracle2XDBCTables.insert(pair<int,int>(SQLT_BLOB,XDBC_TYPE_BLOB));

  oracle2XDBCTables.insert(pair<int,int>(SQLT_AFC,XDBC_TYPE_CHAR));
  oracle2XDBCTables.insert(pair<int,int>(SQLT_AVC,XDBC_TYPE_CHAR));

  oracle2XDBCTables.insert(pair<int,int>(SQLT_CHR,XDBC_TYPE_VARCHAR));
  oracle2XDBCTables.insert(pair<int,int>(SQLT_STR,XDBC_TYPE_VARCHAR));

  oracle2XDBCTables.insert(pair<int,int>(SQLT_VCS,XDBC_TYPE_VARCHAR));
  oracle2XDBCTables.insert(pair<int,int>(SQLT_VBI,XDBC_TYPE_VARBINARY));

  oracle2XDBCTables.insert(pair<int,int>(SQLT_LVC,XDBC_TYPE_LONGVARCHAR));
  oracle2XDBCTables.insert(pair<int,int>(SQLT_LVB,XDBC_TYPE_LONGVARBINARY));
  
  oracle2XDBCTables.insert(pair<int,int>(SQLT_LNG,XDBC_TYPE_LONGVARCHAR));

  oracle2XDBCTables.insert(pair<int,int>(SQLT_BIN,XDBC_TYPE_VARBINARY));
  oracle2XDBCTables.insert(pair<int,int>(SQLT_LBI,XDBC_TYPE_LONGVARBINARY));

  oracle2XDBCTables.insert(pair<int,int>(SQLT_RDD,XDBC_TYPE_VARCHAR));
  
//--------------------------------------------------------
// XDBC To Oracle Mapping
//--------------------------------------------------------
  xdbc2OracleTables.insert(pair<int,int>(XDBC_TYPE_TINYINT,SQLT_INT));
  xdbc2OracleTables.insert(pair<int,int>(XDBC_TYPE_SMALLINT,SQLT_INT));
  xdbc2OracleTables.insert(pair<int,int>(XDBC_TYPE_INTEGER,SQLT_INT));
  xdbc2OracleTables.insert(pair<int,int>(XDBC_TYPE_BIGINT,SQLT_INT));

  xdbc2OracleTables.insert(pair<int,int>(XDBC_TYPE_FLOAT,SQLT_FLT));
  xdbc2OracleTables.insert(pair<int,int>(XDBC_TYPE_DOUBLE,SQLT_FLT));

  xdbc2OracleTables.insert(pair<int,int>(XDBC_TYPE_CHAR,SQLT_AFC));
  xdbc2OracleTables.insert(pair<int,int>(XDBC_TYPE_VARCHAR,SQLT_CHR));

  xdbc2OracleTables.insert(pair<int,int>(XDBC_TYPE_LONGVARCHAR,SQLT_LVC));

  xdbc2OracleTables.insert(pair<int,int>(XDBC_TYPE_BINARY,SQLT_BIN));
  xdbc2OracleTables.insert(pair<int,int>(XDBC_TYPE_VARBINARY,SQLT_BIN));
  xdbc2OracleTables.insert(pair<int,int>(XDBC_TYPE_LONGVARBINARY,SQLT_LBI));

  xdbc2OracleTables.insert(pair<int,int>(XDBC_TYPE_TIMESTAMP,SQLT_TIMESTAMP));
  xdbc2OracleTables.insert(pair<int,int>(XDBC_TYPE_INTERVAL,SQLT_INTERVAL_DS));
  xdbc2OracleTables.insert(pair<int,int>(XDBC_TYPE_DATE,SQLT_DAT));
  xdbc2OracleTables.insert(pair<int,int>(XDBC_TYPE_TIME,SQLT_DAT));

  xdbc2OracleTables.insert(pair<int,int>(XDBC_TYPE_CLOB,SQLT_CLOB));
  xdbc2OracleTables.insert(pair<int,int>(XDBC_TYPE_BLOB,SQLT_BLOB));

//--------------------------------------------------------
// XDBC To Oracle Type Name Mapping
//--------------------------------------------------------
  xdbc2TypeNames.insert(pair<int,string>(XDBC_TYPE_CHAR,"CHAR"));
  xdbc2TypeNames.insert(pair<int,string>(XDBC_TYPE_VARCHAR,"VARCHAR2"));
  xdbc2TypeNames.insert(pair<int,string>(XDBC_TYPE_LONGVARCHAR,"LONG"));
  xdbc2TypeNames.insert(pair<int,string>(XDBC_TYPE_VARBINARY,"RAW"));
  xdbc2TypeNames.insert(pair<int,string>(XDBC_TYPE_LONGVARBINARY,"LONG RAW"));

  xdbc2TypeNames.insert(pair<int,string>(XDBC_TYPE_BIGINT,"INTEGER"));
  xdbc2TypeNames.insert(pair<int,string>(XDBC_TYPE_NUMERIC,"NUMBER"));
  xdbc2TypeNames.insert(pair<int,string>(XDBC_TYPE_INTEGER,"INTEGER"));
  xdbc2TypeNames.insert(pair<int,string>(XDBC_TYPE_SMALLINT,"NUMBER"));
  xdbc2TypeNames.insert(pair<int,string>(XDBC_TYPE_FLOAT,"FLOAT"));
  xdbc2TypeNames.insert(pair<int,string>(XDBC_TYPE_DOUBLE,"NUMBER"));

  xdbc2TypeNames.insert(pair<int,string>(XDBC_TYPE_DATE,"DATE"));
  xdbc2TypeNames.insert(pair<int,string>(XDBC_TYPE_TIMESTAMP,"TIMESTAMP"));

  xdbc2TypeNames.insert(pair<int,string>(XDBC_TYPE_BLOB,"BLOB"));
  xdbc2TypeNames.insert(pair<int,string>(XDBC_TYPE_CLOB,"CLOB"));

//--------------------------------------------------------
// SQLT_XXX To Oracle Type Name Mapping
//--------------------------------------------------------
  sqlt2TypeNames.insert(pair<int,string>(SQLT_INT,"INTEGER"));

  sqlt2TypeNames.insert(pair<int,string>(SQLT_NUM,"NUMBER"));
  sqlt2TypeNames.insert(pair<int,string>(SQLT_VNU,"NUMBER"));
  sqlt2TypeNames.insert(pair<int,string>(SQLT_FLT,"FLOAT"));

  sqlt2TypeNames.insert(pair<int,string>(SQLT_IBFLOAT,"BINARY_FLOAT"));
  sqlt2TypeNames.insert(pair<int,string>(SQLT_IBDOUBLE,"BINARY_DOUBLE"));

  sqlt2TypeNames.insert(pair<int,string>(SQLT_BFLOAT,"BINARY_FLOAT"));
  sqlt2TypeNames.insert(pair<int,string>(SQLT_BDOUBLE,"BINARY_DOUBLE"));

  sqlt2TypeNames.insert(pair<int,string>(SQLT_TIMESTAMP,"TIMESTAMP"));
  sqlt2TypeNames.insert(pair<int,string>(SQLT_TIMESTAMP_TZ,"TIMESTAMPTZ"));
  sqlt2TypeNames.insert(pair<int,string>(SQLT_TIMESTAMP_LTZ,"TIMESTAMPLTZ"));

  sqlt2TypeNames.insert(pair<int,string>(SQLT_INTERVAL_YM,"INTERVALYM"));
  sqlt2TypeNames.insert(pair<int,string>(SQLT_INTERVAL_DS,"INTERVALDS"));
  sqlt2TypeNames.insert(pair<int,string>(SQLT_DATE,"DATE"));
  sqlt2TypeNames.insert(pair<int,string>(SQLT_DAT,"DATE"));

  sqlt2TypeNames.insert(pair<int,string>(SQLT_CLOB,"CLOB"));
  sqlt2TypeNames.insert(pair<int,string>(SQLT_BLOB,"BLOB"));
  sqlt2TypeNames.insert(pair<int,string>(SQLT_FILE,"BFILE"));

  sqlt2TypeNames.insert(pair<int,string>(SQLT_AFC,"CHAR"));
  sqlt2TypeNames.insert(pair<int,string>(SQLT_AVC,"CHARZ"));

  sqlt2TypeNames.insert(pair<int,string>(SQLT_CHR,"VARCHAR2"));
  sqlt2TypeNames.insert(pair<int,string>(SQLT_STR,"VARCHAR2"));

  sqlt2TypeNames.insert(pair<int,string>(SQLT_VCS,"VARCHAR"));
  sqlt2TypeNames.insert(pair<int,string>(SQLT_VBI,"VARRAW"));

  sqlt2TypeNames.insert(pair<int,string>(SQLT_LVC,"LONG VARCHAR"));
  sqlt2TypeNames.insert(pair<int,string>(SQLT_LVB,"LONG VARRAW"));

  sqlt2TypeNames.insert(pair<int,string>(SQLT_LNG,"LONG"));

  sqlt2TypeNames.insert(pair<int,string>(SQLT_BIN,"RAW"));
  sqlt2TypeNames.insert(pair<int,string>(SQLT_LBI,"LONG RAW"));

  sqlt2TypeNames.insert(pair<int,string>(SQLT_RDD,"ROWID"));
  
  sqlt2TypeNames.insert(pair<int,string>(SQLT_REF,"REFCURSOR"));
}

OracleTypes& OracleTypes::getInstance()
{
  return m_oracleTypes;
}

int OracleTypes::XDBC_2_Oracle(int xtype)
{
  OPENCODE_MAP<int,int>::iterator pos;
  pos=xdbc2OracleTables.find(xtype);

  if(pos != xdbc2OracleTables.end())
  {
    return pos->second;
  }

  
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    __sb.append("XDBC_2_ORACLE|failed, XDBC = ");
    __sb.append(xtype);
    
    __XDBC_FORCED_TRACE(ORACLETYPES_CLASS_NAME,__sb.str());
  }

  return xtype;
}

int OracleTypes::Oracle_2_XDBC(int sqlt)
{
  OPENCODE_MAP<int,int>::iterator pos;
  pos=oracle2XDBCTables.find(sqlt);

  if(pos != oracle2XDBCTables.end())
  {
    return pos->second;
  }

  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    __sb.append("ORACLE_2_XDBC|failed, SQLT = ");
    __sb.append(sqlt);
    
    __XDBC_FORCED_TRACE(ORACLETYPES_CLASS_NAME,__sb.str());
  }

  return Types::OTHER;
}

int OracleTypes::Oracle_2_XDBC(int oracleType,int precision,int scale)
{
  if(oracleType == SQLT_NUM  || oracleType == SQLT_VNU || oracleType == SQLT_FLT)
  {
    if(precision != 0)
    {
      if(scale == -127)
      {
         return  Types::FLOAT;
      }
      else if ( scale == 0)
      {
        return Types::BIGINT;
      }
    }
  }
 
  return Oracle_2_XDBC(oracleType);
}

int OracleTypes::Oracle_2_XDBC(const OracleColumn& column)
{
  return Oracle_2_XDBC(column.__mi_type,column.__mi_precision,column.__mi_scale);
}

string  OracleTypes::getTypeName(int dataType,bool isXDBC)
{
  if(isXDBC)
  {
    return getXDBCTypeName(dataType);
  }

  return  getOracleTypeName(dataType);    
}

string OracleTypes::getTypeName(const OracleColumn& column)
{
  return getOracleTypeName(column.__mi_type,column.__mi_precision,column.__mi_scale);
}

string  OracleTypes::getTypeName(int oracleType,int precision,int scale)
{
  return getOracleTypeName(oracleType,precision,scale);
}

string  OracleTypes::getXDBCTypeName(int xdbcType)
{
  OPENCODE_MAP<int,string>::iterator pos=xdbc2TypeNames.find(xdbcType);

  if(pos != xdbc2TypeNames.end())
  {
    return pos->second;
  }

  return getOracleTypeName(XDBC_2_Oracle(xdbcType));
}

string  OracleTypes::getOracleTypeName(int oracleType,int precision,int scale)
{
  if(oracleType == SQLT_NUM  || oracleType == SQLT_VNU || oracleType == SQLT_FLT)
  {
    return getXDBCTypeName(Oracle_2_XDBC(oracleType));
  }

  return getOracleTypeName(oracleType);
}

string  OracleTypes::getOracleTypeName(int oracleType)
{
  OPENCODE_MAP<int,string>::iterator pos=sqlt2TypeNames.find(oracleType);

  if(pos != sqlt2TypeNames.end())
  {
    return pos->second;
  }

  return string("");
}

__OPENCODE_END_NAMESPACE
