#include "xdbc/oracleStatement.h"
#include "xdbc/oracleConnection.h"
#include "xdbc/oracleResultSet.h"
#include "xdbc/oracleArrayResultSet.h"
#include "xdbc/oracleSQL.h"
#include "xdbc/oracleConversion.h"
#include "xdbc/xsmart.h"

#include "port/ioc_sbuf.h"

#include <algorithm>
USING_OPENCODE_STL_NS

__OPENCODE_BEGIN_NAMESPACE

#define ORACLESTATEMENT_CLASS_NAME "OracleStatement"

OracleStatement::OracleStatement(OracleConnection* ora_conn)
{
  __mp_conn=ora_conn;
  
  __mi_resultSetType=ResultSet::TYPE_FORWARD_ONLY;
  __mi_resultSetConcurrency=ResultSet::CONCUR_READ_ONLY;
  
  oci_stmthp=0;
  __mb_initialized=false;
  __mb_isClosed=false;
  
}

OracleStatement::OracleStatement(OracleConnection* ora_conn,int resultSetType, int resultSetConcurrency)
{
  __mp_conn=ora_conn;

  __mi_resultSetType=resultSetType;
  __mi_resultSetConcurrency=resultSetConcurrency;
  
  oci_stmthp=0;
  __mb_initialized=false;
  __mb_isClosed=false;
}


void OracleStatement::initialize(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{
  if(__mb_initialized)
  {
    return;
  } 
   
  __mb_initialized=true;
  sword ret_code = OCIHandleAlloc( (dvoid *) __mp_conn->oci_envhp, (dvoid **) &oci_stmthp,OCI_HTYPE_STMT, (size_t) 0, (dvoid **) 0);
  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLESTATEMENT_CLASS_NAME);
}

OracleStatement::~OracleStatement()
{
  __XDBC_TRACE(ORACLESTATEMENT_CLASS_NAME,"destroy()");
}

ResultSet* OracleStatement::executeQuery(__XDBC_HOOK_DECLARE_MORE const string& sql)  __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
     StringBuffer __sb;
     __sb.append("executeQuery|");
     __sb.append(sql);
     
     __XDBC_FORCED_TRACE(ORACLESTATEMENT_CLASS_NAME,__sb.str()); 
  }
  
  const char* char_sql = sql.c_str();

  sword ret_code = OCIStmtPrepare(oci_stmthp, __mp_conn->oci_errhp,
                (text*)char_sql, (ub4) strlen(char_sql), (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLESTATEMENT_CLASS_NAME);
  __XDBC_HOOK_CHECK(return 0);
  
  if(__mi_resultSetType == ResultSet::TYPE_FORWARD_ONLY)
  {

    ret_code=OCIStmtExecute(__mp_conn->oci_svchp, oci_stmthp, __mp_conn->oci_errhp, 
                           (ub4) 0, (ub4) 0, 
                           (CONST OCISnapshot *) 0, (OCISnapshot *) 0, 
                           OCI_DEFAULT); 
  }
  else
  {
    ret_code=OCIStmtExecute(__mp_conn->oci_svchp, oci_stmthp, __mp_conn->oci_errhp,
                           (ub4) 0, (ub4) 0, 
                           (CONST OCISnapshot *) 0, (OCISnapshot *) 0, 
                            OCI_STMT_SCROLLABLE_READONLY);

  }
  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLESTATEMENT_CLASS_NAME);
  __XDBC_HOOK_CHECK(return 0);

  OracleResultSetCollection* __cachedCollection = new OracleResultSetCollection(); 
  
  OracleConversion::parseResultSetMetaData(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,oci_stmthp,__cachedCollection->__vector_columns);
  __XDBC_HOOK_CHECK(return 0);
  
  OracleResultSet::prepareColumnData(__XDBC_HOOK_INVOKE_MORE __mp_conn,oci_stmthp,__cachedCollection);
  __XDBC_HOOK_CHECK(return 0);
  
  OracleResultSet::defineColumnData(__XDBC_HOOK_INVOKE_MORE __mp_conn,oci_stmthp,__cachedCollection);
  __XDBC_HOOK_CHECK(return 0);
       
  OracleResultSet* __p_rs = new OracleResultSet(__mp_conn,this,oci_stmthp,__mi_resultSetType,__mi_resultSetConcurrency,__cachedCollection,true,true);
  
  return __p_rs;
}

int OracleStatement::executeUpdate(__XDBC_HOOK_DECLARE_MORE const string& sql)  __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
     StringBuffer __sb;
     __sb.append("executeUpdate|");
     __sb.append(sql);

     __XDBC_FORCED_TRACE(ORACLESTATEMENT_CLASS_NAME,__sb.str()); 
  }

  const char* char_sql=sql.c_str();

  sword ret_code = OCIStmtPrepare(oci_stmthp, __mp_conn->oci_errhp, (text*)char_sql, (ub4) strlen(char_sql), (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLESTATEMENT_CLASS_NAME);
  __XDBC_HOOK_CHECK(return 0);
  
  if(__mp_conn->__mb_autoCommit)
  {
     ret_code=OCIStmtExecute(__mp_conn->oci_svchp, oci_stmthp, __mp_conn->oci_errhp,
                            (ub4) 1, (ub4) 0, (CONST OCISnapshot *) 0, (OCISnapshot *) 0,
                            OCI_DEFAULT|OCI_COMMIT_ON_SUCCESS);
  }
  else
  {
     ret_code = OCIStmtExecute(__mp_conn->oci_svchp, oci_stmthp, __mp_conn->oci_errhp,
                            (ub4) 1, (ub4) 0, (CONST OCISnapshot *) 0, (OCISnapshot *) 0,
                            OCI_DEFAULT);
  }

  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLESTATEMENT_CLASS_NAME);
  __XDBC_HOOK_CHECK(return 0);
  
  ub4 rowsFetched = 0;
  ub4 size = sizeof(rowsFetched);
  OCIAttrGet ( (dvoid *) oci_stmthp,(ub4) OCI_HTYPE_STMT, (dvoid*)&rowsFetched, (ub4*)&size, (ub4) OCI_ATTR_ROW_COUNT, __mp_conn->oci_errhp);

  return rowsFetched;
}

void OracleStatement::close()
{
  __XDBC_TRACE(ORACLESTATEMENT_CLASS_NAME,"close()");
  
  if(__mb_isClosed)
  {
    return;
  }
  __mb_isClosed=true;

  if(oci_stmthp!=0)
  {
    OCIHandleFree((dvoid*)oci_stmthp,OCI_HTYPE_STMT);
    oci_stmthp=0;
  }
}

void OracleStatement::destroy()
{
  delete this;  
}

int OracleStatement::getResultSetType(__XDBC_HOOK_DECLARE_ONCE)   __XDBC_THROW_DECLARE(SQLException)
{
  return __mi_resultSetType;
}

Connection* OracleStatement::getConnection(__XDBC_HOOK_DECLARE_ONCE)   __XDBC_THROW_DECLARE(SQLException)
{
  return (Connection*)__mp_conn;
}

__OPENCODE_END_NAMESPACE
