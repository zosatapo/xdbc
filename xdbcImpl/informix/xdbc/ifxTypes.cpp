#include "xdbc/xtypes.h"
#include "xdbc/ifxTypes.h"

#include "port/ioc_sbuf.h"
#include "port/utilities.h"

USING_OPENCODE_STL_NS
__OPENCODE_BEGIN_NAMESPACE


#define IFXTYPES_CLASS_NAME "IfxTypes"

IfxTypes IfxTypes::m_ifxTypes;


IfxTypes::IfxTypes()
{
//--------------------------------------------------------
// Ifx To XDBC Mapping
//--------------------------------------------------------
  ifx2XDBCTables.insert(pair<int,int>(SQLCHAR,XDBC_TYPE_CHAR));
  ifx2XDBCTables.insert(pair<int,int>(SQLSMINT,XDBC_TYPE_SMALLINT));
  ifx2XDBCTables.insert(pair<int,int>(SQLINT,XDBC_TYPE_INTEGER));
  ifx2XDBCTables.insert(pair<int,int>(SQLFLOAT,XDBC_TYPE_DOUBLE));
  ifx2XDBCTables.insert(pair<int,int>(SQLSMFLOAT,XDBC_TYPE_REAL));
  ifx2XDBCTables.insert(pair<int,int>(SQLDECIMAL,XDBC_TYPE_DECIMAL));
  ifx2XDBCTables.insert(pair<int,int>(SQLSERIAL,XDBC_TYPE_INTEGER));
  ifx2XDBCTables.insert(pair<int,int>(SQLDATE,XDBC_TYPE_DATE));
  ifx2XDBCTables.insert(pair<int,int>(SQLMONEY,XDBC_TYPE_DECIMAL));
  ifx2XDBCTables.insert(pair<int,int>(SQLNULL,XDBC_TYPE_NULL));
  ifx2XDBCTables.insert(pair<int,int>(SQLDTIME,XDBC_TYPE_TIMESTAMP));
  ifx2XDBCTables.insert(pair<int,int>(SQLBYTES,XDBC_TYPE_LONGVARBINARY));
  ifx2XDBCTables.insert(pair<int,int>(SQLTEXT,XDBC_TYPE_LONGVARCHAR));
  ifx2XDBCTables.insert(pair<int,int>(SQLVCHAR,XDBC_TYPE_VARCHAR));
  ifx2XDBCTables.insert(pair<int,int>(SQLINTERVAL,XDBC_TYPE_INTERVAL));
  ifx2XDBCTables.insert(pair<int,int>(SQLNCHAR,XDBC_TYPE_CHAR));
  ifx2XDBCTables.insert(pair<int,int>(SQLNVCHAR,XDBC_TYPE_VARCHAR));
  ifx2XDBCTables.insert(pair<int,int>(SQLINT8,XDBC_TYPE_BIGINT));
  ifx2XDBCTables.insert(pair<int,int>(SQLSERIAL8,XDBC_TYPE_BIGINT));
  ifx2XDBCTables.insert(pair<int,int>(SQLSET,XDBC_TYPE_ARRAY));
  ifx2XDBCTables.insert(pair<int,int>(SQLMULTISET,XDBC_TYPE_ARRAY));
  ifx2XDBCTables.insert(pair<int,int>(SQLLIST,XDBC_TYPE_ARRAY));
  ifx2XDBCTables.insert(pair<int,int>(SQLROW,XDBC_TYPE_STRUCT));
  ifx2XDBCTables.insert(pair<int,int>(SQLUDTVAR,2000));
  ifx2XDBCTables.insert(pair<int,int>(SQLUDTFIXED,2000));
  ifx2XDBCTables.insert(pair<int,int>(SQLLVARCHAR,XDBC_TYPE_LONGVARCHAR));
  ifx2XDBCTables.insert(pair<int,int>(SQLSENDRECV,2000));
  ifx2XDBCTables.insert(pair<int,int>(SQLBOOL,XDBC_TYPE_SMALLINT));
  ifx2XDBCTables.insert(pair<int,int>(SQLIMPEXP,2000));
  ifx2XDBCTables.insert(pair<int,int>(SQLIMPEXPBIN,2000));
  ifx2XDBCTables.insert(pair<int,int>(101,XDBC_TYPE_CLOB));
  ifx2XDBCTables.insert(pair<int,int>(102,XDBC_TYPE_BLOB));


//--------------------------------------------------------
// XDBC To Ifx Mapping
//--------------------------------------------------------
  xdbc2IfDataTables.insert(pair<int,int>(XDBC_TYPE_BIGINT,SQLINT8));
  xdbc2IfDataTables.insert(pair<int,int>(XDBC_TYPE_BINARY,SQLBYTES));
  xdbc2IfDataTables.insert(pair<int,int>(XDBC_TYPE_BIT,SQLCHAR));
  xdbc2IfDataTables.insert(pair<int,int>(XDBC_TYPE_CHAR,SQLCHAR));
  xdbc2IfDataTables.insert(pair<int,int>(XDBC_TYPE_DATE,SQLDATE));
  xdbc2IfDataTables.insert(pair<int,int>(XDBC_TYPE_DECIMAL,SQLDECIMAL));
  xdbc2IfDataTables.insert(pair<int,int>(XDBC_TYPE_DOUBLE,SQLFLOAT));
  xdbc2IfDataTables.insert(pair<int,int>(XDBC_TYPE_FLOAT,SQLFLOAT));
  xdbc2IfDataTables.insert(pair<int,int>(XDBC_TYPE_INTEGER,SQLINT));
  xdbc2IfDataTables.insert(pair<int,int>(XDBC_TYPE_LONGVARBINARY,SQLBYTES));
  xdbc2IfDataTables.insert(pair<int,int>(XDBC_TYPE_LONGVARCHAR,SQLTEXT));
  xdbc2IfDataTables.insert(pair<int,int>(XDBC_TYPE_NULL,SQLNULL));
  xdbc2IfDataTables.insert(pair<int,int>(XDBC_TYPE_NUMERIC,SQLDECIMAL));
  xdbc2IfDataTables.insert(pair<int,int>(XDBC_TYPE_REAL,SQLSMFLOAT));
  xdbc2IfDataTables.insert(pair<int,int>(XDBC_TYPE_SMALLINT,SQLSMINT));
  xdbc2IfDataTables.insert(pair<int,int>(XDBC_TYPE_TIME,SQLDTIME));
  xdbc2IfDataTables.insert(pair<int,int>(XDBC_TYPE_TIMESTAMP,SQLDTIME));
  xdbc2IfDataTables.insert(pair<int,int>(XDBC_TYPE_INTERVAL,SQLINTERVAL));
  xdbc2IfDataTables.insert(pair<int,int>(XDBC_TYPE_TINYINT,SQLSMINT));
  xdbc2IfDataTables.insert(pair<int,int>(1111,SQLBOOL));
  xdbc2IfDataTables.insert(pair<int,int>(XDBC_TYPE_VARBINARY,SQLBYTES));
  xdbc2IfDataTables.insert(pair<int,int>(XDBC_TYPE_VARCHAR,SQLVCHAR));
  xdbc2IfDataTables.insert(pair<int,int>(2000,SQLSENDRECV));
  xdbc2IfDataTables.insert(pair<int,int>(XDBC_TYPE_STRUCT,SQLROW));
  xdbc2IfDataTables.insert(pair<int,int>(XDBC_TYPE_ARRAY,SQLSET));
  xdbc2IfDataTables.insert(pair<int,int>(XDBC_TYPE_BLOB,102));
  xdbc2IfDataTables.insert(pair<int,int>(XDBC_TYPE_CLOB,101)); 
}

IfxTypes& IfxTypes::getInstance()
{
  return m_ifxTypes;
}

int IfxTypes::XDBC_2_Ifx(int xdbcType)
{
  OPENCODE_MAP<int,int>::iterator  pos=xdbc2IfDataTables.find(xdbcType);

  if(pos != xdbc2IfDataTables.end())
  {
    return pos->second;
  }

  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    __sb.append("XDBC_2_IFX|failed, XDBC = ");
    __sb.append(xdbcType);
    __XDBC_FORCED_TRACE(IFXTYPES_CLASS_NAME,__sb.str());
  }
  
  return xdbcType;
}

int IfxTypes::Ifx_2_XDBC(int ifxType)
{
  OPENCODE_MAP<int,int>::iterator pos=ifx2XDBCTables.find(ifxType);

  if(pos != ifx2XDBCTables.end())
  {
    return pos->second;
  }

  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    __sb.append("IFX_2_XDBC|failed, SQLT = ");
    __sb.append(ifxType);
    __XDBC_FORCED_TRACE(IFXTYPES_CLASS_NAME,__sb.str());
  }

  return Types::OTHER;
}

int IfxTypes::Ifx_2_XDBC(const IfxColumn& column)
{
  int coltype = column.__mi_type;

  switch(coltype)
  {
    case SQLUDTFIXED:
    {
      if(column.__mi_xtype == XID_CLOB)
      {
        return Types::CLOB;
      }
      else if(column.__mi_xtype == XID_BLOB)
      {
        return Types::BLOB;
      }
    }
    break;

    default:
    {
    }
    break;
  }

  return Ifx_2_XDBC(coltype);
}

string  IfxTypes::getTypeName(int dataType,bool isXDBC)
{
  if(isXDBC)
  {
    OPENCODE_MAP<int,int>::iterator pos=ifx2XDBCTables.find(dataType);

    if(pos != ifx2XDBCTables.end())
    {
      return getTypeName(pos->second,false);
    }
  }
  else
  {
    const char* pname = rtypname(dataType);
    if(pname != XDBC_TYPE_NULL)
    {
      return StringsKit::toUpperCase(string(pname));
    }
  }

  return string();
}

string  IfxTypes::getTypeName(const IfxColumn& column)
{
  int ifxType = column.__mi_type;

  return getTypeName(ifxType,false);
}

__OPENCODE_END_NAMESPACE
