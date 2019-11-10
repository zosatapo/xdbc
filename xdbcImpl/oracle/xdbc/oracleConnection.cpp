#include "xdbc/oracleConnection.h"

#include "xdbc/oracleStatement.h"
#include "xdbc/oraclePreparedStatement.h"
#include "xdbc/oracleCallableStatement.h"
#include "xdbc/oracleArrayStatement.h"
#include "xdbc/oracleSQL.h"
#include "xdbc/oracleResultSet.h"
#include "xdbc/oracleConversion.h"
#include "xdbc/xsmart.h"

#include "port/ioc_sbuf.h"
#include "port/libnumber.h"
#include "port/stringTokenizer.h" 

#include <algorithm>
USING_OPENCODE_STL_NS

__OPENCODE_BEGIN_NAMESPACE

#define ORACLECONNECTION_CLASS_NAME "OracleConnection"

OracleConnection::~OracleConnection()
{
  __XDBC_TRACE(ORACLECONNECTION_CLASS_NAME,"destroy()");  
}

OracleConnection::OracleConnection()
{
  __mb_isClosed=false;
  __mi_transLevel=Connection::TRANSACTION_READ_COMMITTED;
  __mb_autoCommit=true;

  oci_envhp=0;
  oci_errhp=0;

  oci_authp=0;
  oci_srvhp=0;
  oci_svchp=0;

  __mi_svr_cs=0;
  __mi_env_cs=0;
}

bool OracleConnection::connect(__XDBC_HOOK_DECLARE_MORE const string& url)  __XDBC_THROW_DECLARE(SQLException)
{
  __XDBC_ATTRIBUTES props;
  
  bool doChange = true;
  string newURL = XDBCUtilities::parseURL(props,url,doChange);

  return connect(__XDBC_HOOK_INVOKE_MORE newURL,props);
}

bool OracleConnection::connect(__XDBC_HOOK_DECLARE_MORE const string& url, const string& user, const string& password)  __XDBC_THROW_DECLARE(SQLException)
{
  __XDBC_ATTRIBUTES props;
  
  bool doChange = true;
  string newURL = XDBCUtilities::parseURL(props,url,doChange);

  props[XDBC_USER] = user;
  props[XDBC_PASSWORD] = password;
      
  return  connect(__XDBC_HOOK_INVOKE_MORE newURL,props);
}

bool OracleConnection::connect(__XDBC_HOOK_DECLARE_MORE const string& url,__XDBC_ATTRIBUTES& info)  __XDBC_THROW_DECLARE(SQLException)
{
  string user ;
  __XDBC_ATTRIBUTES::iterator __pos = info.find(XDBC_USER);
  if(__pos != info.end())
  {
    user = __pos->second;
  }

  string password ;
  __pos = info.find(XDBC_PASSWORD);
  if(__pos != info.end())
  {
    password = __pos->second;
  }    
 
  const char* sz_user = user.c_str();
  const char* sz_password = password.c_str(); 

  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    __sb << "connect|" << url << "::" << user;
    __XDBC_FORCED_TRACE(ORACLECONNECTION_CLASS_NAME,__sb.str());
  }
 
//----------------------------------------------------------------
/*
    OCIEnvNlsCreate((OCIEnv **)&oci_envhp,
                             (ub4)0,
                             (dvoid*)0
                             (void*(*)(void*,unsigned int)) 0,
                             (void*(*)(void*, void*,unsigned int)) 0,
                             (void (*)(void*, void*)) 0,
                             (size_t) 0,
                             (void **)0,
                             (ub2)0, //(ub2)OCI_UTF16ID, 
                             (ub2)0 );
*/


  if(System::__M_multithreading)
  {
    OCIEnvCreate(&oci_envhp, OCI_THREADED|OCI_OBJECT, (dvoid *)0, 0, 0, 0, (size_t) 0, (dvoid **)0);
  }
  else
  {
    OCIEnvCreate(&oci_envhp,OCI_OBJECT, (dvoid *)0, 0, 0, 0, (size_t) 0, (dvoid **)0);
  }

  OCIHandleAlloc( (dvoid *) oci_envhp, (dvoid **) &oci_errhp, OCI_HTYPE_ERROR,(size_t) 0, (dvoid **) 0);
  OCIHandleAlloc( (dvoid *) oci_envhp, (dvoid **) &oci_srvhp, OCI_HTYPE_SERVER, (size_t) 0, (dvoid **) 0);
  OCIHandleAlloc( (dvoid *) oci_envhp, (dvoid **) &oci_svchp, OCI_HTYPE_SVCCTX, (size_t) 0, (dvoid **) 0);
//----------------------------------------------------------------

  const char* server=url.c_str();
  sword ret_code = OCIServerAttach( oci_srvhp, oci_errhp, (text *)server, strlen(server), 0);
  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE  oci_errhp,ret_code,ORACLECONNECTION_CLASS_NAME);
  __XDBC_HOOK_CHECK(return false);
    
  /* set attribute server context in the service context */
  OCIAttrSet((dvoid *)oci_svchp, OCI_HTYPE_SVCCTX, (dvoid *)oci_srvhp, (ub4)0, OCI_ATTR_SERVER, (OCIError *) oci_errhp);

  OCIHandleAlloc((dvoid *) oci_envhp, (dvoid **)&oci_authp, (ub4) OCI_HTYPE_SESSION, (size_t) 0, (dvoid **) 0);

  OCIAttrSet((dvoid *) oci_authp, (ub4) OCI_HTYPE_SESSION,
                 (dvoid *) sz_user, (ub4) strlen((char *)sz_user),
                 (ub4) OCI_ATTR_USERNAME, oci_errhp);

  OCIAttrSet((dvoid *) oci_authp, (ub4) OCI_HTYPE_SESSION,
                 (dvoid *) sz_password, (ub4) strlen((char *)sz_password),
                 (ub4) OCI_ATTR_PASSWORD, oci_errhp);

#if !defined(OPENCODE_HAS_STMTPREPARE2)
  ret_code = OCISessionBegin ( oci_svchp,  oci_errhp, oci_authp, OCI_CRED_RDBMS,(ub4) OCI_DEFAULT);
  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE oci_errhp,ret_code,ORACLECONNECTION_CLASS_NAME);
  __XDBC_HOOK_CHECK(return false);
    
  ret_code = OCIAttrSet((dvoid *) oci_svchp, (ub4)OCI_HTYPE_SVCCTX, (dvoid *)oci_authp, (ub4)0, (ub4) OCI_ATTR_SESSION, oci_errhp);
  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE oci_errhp,ret_code,ORACLECONNECTION_CLASS_NAME);
  __XDBC_HOOK_CHECK(return false);
#else
  ret_code = OCISessionBegin ( oci_svchp,  oci_errhp, oci_authp, OCI_CRED_RDBMS,(ub4)OCI_STMT_CACHE);
  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE oci_errhp,ret_code,ORACLECONNECTION_CLASS_NAME);
  __XDBC_HOOK_CHECK(return false);
    
  ret_code = OCIAttrSet((dvoid *) oci_svchp, (ub4)OCI_HTYPE_SVCCTX, (dvoid *)oci_authp, (ub4)0, (ub4) OCI_ATTR_SESSION, oci_errhp);
  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE oci_errhp,ret_code,ORACLECONNECTION_CLASS_NAME);
  __XDBC_HOOK_CHECK(return false);
    
  /**
   * OCI_ATTR_STMTCACHESIZE
   * Mode                << READ/WRITE >> 
   * Attribute Datatype  << ub4 * OR ub4 >>
   *
   * The default value of the statement cache size is 20, for a statement cache enabled session. 
   * The user can increase or decrease this value, by setting this attribute on the service context handle.
   */
  ub4 __stmt_cachesize = 0;
  string __s_stmt_cachesize;
  __XDBC_ATTRIBUTES::iterator __pos_x = info.find("STMTCACHESIZE");
  if(__pos_x != info.end())
  {
    __s_stmt_cachesize = __pos_x->second;
  }
 
  if(!__s_stmt_cachesize.empty()) 
  {
    __OPENCODE_TRY
    {
      int __cachesize = Number::parseInt(__s_stmt_cachesize);
      if(__cachesize > 0)
      {
        __stmt_cachesize = __cachesize;
      }  
    }
    __OPENCODE_CATCH(NumberFormatException& ex)
    {
    }
  } 
  
  if(__stmt_cachesize != 0)
  {
    ret_code = OCIAttrSet((dvoid *) oci_svchp, (ub4)OCI_HTYPE_SVCCTX, 
                          (dvoid *)(&__stmt_cachesize), (ub4)0, 
                          (ub4) OCI_ATTR_STMTCACHESIZE, oci_errhp);
    OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE oci_errhp,ret_code,ORACLECONNECTION_CLASS_NAME);
    __XDBC_HOOK_CHECK(return false);
  }   

  ret_code = OCIAttrGet((dvoid *) oci_svchp, (ub4)OCI_HTYPE_SVCCTX,
                        (dvoid *)(&__stmt_cachesize), (ub4)0,
                        (ub4) OCI_ATTR_STMTCACHESIZE, oci_errhp);
  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE oci_errhp,ret_code,ORACLECONNECTION_CLASS_NAME);
  __XDBC_HOOK_CHECK(return false);
  
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    __sb << "connect|set OCI_ATTR_STMTCACHESIZE to " << __stmt_cachesize ;
    __XDBC_FORCED_TRACE(ORACLECONNECTION_CLASS_NAME,__sb.str());
  }

#endif 
    
 
  char  infoBuf[OCI_NLS_MAXBUFSZ];
  memset(infoBuf,0,OCI_NLS_MAXBUFSZ);
  ret_code = OCINlsGetInfo(oci_authp,oci_errhp,(text*)infoBuf, (size_t) OCI_NLS_MAXBUFSZ, (ub2)OCI_NLS_CHARACTER_SET);
  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE oci_errhp,ret_code,ORACLECONNECTION_CLASS_NAME);
  __XDBC_HOOK_CHECK(return false);
  
  __mi_svr_cs=OCINlsCharSetNameToId(oci_authp,(text*)infoBuf);

  char  infoBuf2[OCI_NLS_MAXBUFSZ];
  memset(infoBuf2,0,OCI_NLS_MAXBUFSZ);
  ret_code = OCINlsGetInfo(oci_envhp,oci_errhp,(text*)infoBuf2, (size_t) OCI_NLS_MAXBUFSZ, (ub2)OCI_NLS_CHARACTER_SET);
  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE oci_errhp,ret_code,ORACLECONNECTION_CLASS_NAME);
  __XDBC_HOOK_CHECK(return false);
  
  __mi_env_cs=OCINlsCharSetNameToId(oci_envhp,(text*)infoBuf2);

  __mo_attributes[XDBC_URL] = url;
  __mo_attributes[XDBC_USER] = user;

  __mo_attributes[XDBC_DRIVER] = "oracle/oci";
  __mo_attributes[XDBC_VERSION] = "1.0.0";

  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    __sb.append("connect|connected {svr_cs:");
    __sb.append(__mi_svr_cs);
    __sb.append("/");
    __sb.append(infoBuf);
    __sb.append(",env_cs:");
    __sb.append(__mi_env_cs);
    __sb.append("/");
    __sb.append(infoBuf2);
    __sb.append("}");

    __XDBC_FORCED_TRACE(ORACLECONNECTION_CLASS_NAME,__sb.str());
  }

  return true;
}

string OracleConnection::normalizeSQL(const string& sql,int& paramCount)
{
  __XDBC_HOOK_DEFINE();
  string __ret_SQL = OracleSQL::parseSQL(__XDBC_HOOK_INVOKE_MORE sql,paramCount);
  return __ret_SQL;
}

void OracleConnection::close()
{
  __XDBC_TRACE(ORACLECONNECTION_CLASS_NAME,"close()");   

  if(__mb_isClosed)
  {
    return;
  }

  OCISessionEnd (oci_svchp,oci_errhp,oci_authp,(ub4) OCI_DEFAULT);
  OCIServerDetach(oci_srvhp,oci_errhp,(ub4) OCI_DEFAULT);

  if(oci_authp!=0)
  {
    OCIHandleFree((dvoid *) oci_authp, OCI_HTYPE_SESSION);
    oci_authp=0;
  }

  if(oci_svchp!=0)
  {
    OCIHandleFree((dvoid *) oci_svchp, OCI_HTYPE_SVCCTX);
    oci_svchp=0;
  }

  if(oci_srvhp!=0)
  {
    OCIHandleFree((dvoid *) oci_srvhp, OCI_HTYPE_SERVER);
    oci_srvhp=0;
  }

  if(oci_errhp!=0)
  {
    OCIHandleFree((dvoid *) oci_errhp, OCI_HTYPE_ERROR);
    oci_errhp=0;
  }

  if(oci_envhp!=0)
  {
    OCIHandleFree((dvoid *) oci_envhp, OCI_HTYPE_ENV);
    oci_envhp=0;
  }
  
  __mb_isClosed=true;
}

void OracleConnection::destroy()
{
  delete this;
}
  
bool OracleConnection::isClosed()
{
  return __mb_isClosed;
}

int OracleConnection::pingDatabase(int timeout)
{ 
  int retcode = 0;
  __XDBC_HOOK_DEFINE();

  __OPENCODE_TRY
  {   
    Statement* __p_stmt = createStatement(__XDBC_HOOK_INVOKE_ONCE);
    xdbc_smart_ptr<Statement> __sp_stmt(__p_stmt);
    __XDBC_HOOK_CHECK(return -1);
    
    ResultSet*  __p_rs = __p_stmt->executeQuery(__XDBC_HOOK_INVOKE_MORE "SELECT 'x' FROM DUAL");
    xdbc_smart_ptr<ResultSet> __sp_rs(__p_rs);
    __XDBC_HOOK_CHECK(return -1);
  }
  __OPENCODE_CATCH(SQLException& ex)
  {
    retcode = -1;   
  }
  
  return retcode;
}

Statement* OracleConnection::createStatement(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{
  OracleStatement* p_stmt=new OracleStatement(this);
  xdbc_smart_ptr<OracleStatement> __sp_stmt(p_stmt); 
  
  p_stmt->initialize(__XDBC_HOOK_INVOKE_ONCE);
  __XDBC_HOOK_CHECK(return 0);
  
  return __sp_stmt.release();
}

PreparedStatement* OracleConnection::prepareStatement(__XDBC_HOOK_DECLARE_MORE const string& sql,int paramCount)  __XDBC_THROW_DECLARE(SQLException)
{
  OraclePreparedStatement* p_pstmt= new OraclePreparedStatement(this,sql,paramCount);
  xdbc_smart_ptr<OraclePreparedStatement> __sp_pstmt(p_pstmt); 
  
  p_pstmt->initialize(__XDBC_HOOK_INVOKE_ONCE);
  __XDBC_HOOK_CHECK(return 0);

  return __sp_pstmt.release();
}

CallableStatement* OracleConnection::prepareCall(__XDBC_HOOK_DECLARE_MORE const string& sql,int paramCount)  __XDBC_THROW_DECLARE(SQLException)
{
  OracleCallableStatement* p_call= new OracleCallableStatement(this,sql,paramCount);
  xdbc_smart_ptr<OracleCallableStatement> __sp_call(p_call); 
  
  p_call->initialize(__XDBC_HOOK_INVOKE_ONCE);
  __XDBC_HOOK_CHECK(return 0);

  return __sp_call.release();
}

ArrayStatement* OracleConnection::prepareArray(__XDBC_HOOK_DECLARE_MORE const string& sql,int paramCount)  __XDBC_THROW_DECLARE(SQLException)
{
  OracleArrayStatement* p_pstmt= new OracleArrayStatement(this,sql,paramCount);
  xdbc_smart_ptr<OracleArrayStatement> __sp_pstmt(p_pstmt); 
  
  p_pstmt->initialize(__XDBC_HOOK_INVOKE_ONCE);
  __XDBC_HOOK_CHECK(return 0);

  return __sp_pstmt.release();
}

void OracleConnection::setAutoCommit(__XDBC_HOOK_DECLARE_MORE bool autoCommit)  __XDBC_THROW_DECLARE(SQLException)
{
  if(autoCommit==__mb_autoCommit)
  {
    return;
  } 
  
  __mb_autoCommit=autoCommit;

}

bool OracleConnection::getAutoCommit(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    __sb.append("getAutoCommit|");
    __sb.append(__mb_autoCommit);
    
    __XDBC_FORCED_TRACE(ORACLECONNECTION_CLASS_NAME,__sb.str());
  }

  return __mb_autoCommit;
}

void OracleConnection::commit(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{
  __XDBC_TRACE(ORACLECONNECTION_CLASS_NAME,"commit()"); 

  sword ret_code = OCITransCommit(oci_svchp,oci_errhp,(ub4)OCI_DEFAULT);
  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE oci_errhp,ret_code,ORACLECONNECTION_CLASS_NAME);
}

void OracleConnection::rollback(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{
  __XDBC_TRACE(ORACLECONNECTION_CLASS_NAME,"rollback()"); 

  sword ret_code = OCITransRollback(oci_svchp,oci_errhp,(ub4)OCI_DEFAULT);
  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE oci_errhp,ret_code,ORACLECONNECTION_CLASS_NAME);
}

void OracleConnection::setTransactionIsolation(__XDBC_HOOK_DECLARE_MORE int level)  __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    __sb.append("setTransactionIsolation|");
    __sb.append(level);
    
    __XDBC_FORCED_TRACE(ORACLECONNECTION_CLASS_NAME,__sb.str());
  }
 
  Statement* p_stmt = createStatement(__XDBC_HOOK_INVOKE_ONCE);
  xdbc_smart_ptr<Statement> __sp_stmt(p_stmt); 
  __XDBC_HOOK_CHECK(return);
  
  bool canThrow=false;
 
  switch(level)
  {
    case XCONNECTION_TRANSACTION_READ_COMMITTED :
       p_stmt->executeUpdate(__XDBC_HOOK_INVOKE_MORE "ALTER SESSION SET ISOLATION_LEVEL = READ COMMITTED");
       break;

    case XCONNECTION_TRANSACTION_SERIALIZABLE :
       p_stmt->executeUpdate(__XDBC_HOOK_INVOKE_MORE "ALTER SESSION SET ISOLATION_LEVEL = SERIALIZABLE");
       break;

    default:
      canThrow=true;
      break;
  }
  __XDBC_HOOK_CHECK(return);
   
  if(canThrow)
  {
    StringBuffer __sb;
    __sb.append("TransactionIsolation not supported {");
    __sb.append(level);
    __sb.append("}");
    __XDBC_HOOK_RETURN(return,__sb.str());
  }
  
  __mi_transLevel=level;
}

int OracleConnection::getTransactionIsolation(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{
  return __mi_transLevel;
}

Statement* OracleConnection::createStatement(__XDBC_HOOK_DECLARE_MORE int resultSetType, int resultSetConcurrency) __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    __sb.append("createStatement|resultSetType=");
    __sb.append(resultSetConcurrency);
    __sb.append(",resultSetConcurrency=");
    __sb.append(resultSetConcurrency);
    
    __XDBC_FORCED_TRACE(ORACLECONNECTION_CLASS_NAME,__sb.str());
  }

  OracleResultSet::isSupported(__XDBC_HOOK_INVOKE_MORE resultSetType,resultSetConcurrency);
  __XDBC_HOOK_CHECK(return 0);

  OracleStatement* p_stmt=new OracleStatement(this,resultSetType,resultSetConcurrency);
  xdbc_smart_ptr<OracleStatement> __sp_stmt(p_stmt); 
  
  p_stmt->initialize(__XDBC_HOOK_INVOKE_ONCE);
  __XDBC_HOOK_CHECK(return 0);
  
  return __sp_stmt.release();
}

PreparedStatement* OracleConnection::prepareStatement(__XDBC_HOOK_DECLARE_MORE const string& sql, int resultSetType, int resultSetConcurrency,int paramCount) __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    __sb.append("prepareStatement|resultSetType=");
    __sb.append(resultSetConcurrency);
    __sb.append(",resultSetConcurrency=");
    __sb.append(resultSetConcurrency);
    
    __XDBC_FORCED_TRACE(ORACLECONNECTION_CLASS_NAME,__sb.str());
  }

  OracleResultSet::isSupported(__XDBC_HOOK_INVOKE_MORE resultSetType,resultSetConcurrency);
  __XDBC_HOOK_CHECK(return 0);

  OraclePreparedStatement* p_pstmt= new OraclePreparedStatement(this,sql,resultSetType,resultSetConcurrency,paramCount);
  xdbc_smart_ptr<OraclePreparedStatement> __sp_pstmt(p_pstmt); 
  
  p_pstmt->initialize(__XDBC_HOOK_INVOKE_ONCE);
  __XDBC_HOOK_CHECK(return 0);

  return __sp_pstmt.release();
}

CallableStatement* OracleConnection::prepareCall(__XDBC_HOOK_DECLARE_MORE const string& sql, int resultSetType, int resultSetConcurrency,int paramCount)  __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    __sb.append("prepareCall|resultSetType=");
    __sb.append(resultSetConcurrency);
    __sb.append(",resultSetConcurrency=");
    __sb.append(resultSetConcurrency);
    
    __XDBC_FORCED_TRACE(ORACLECONNECTION_CLASS_NAME,__sb.str());
  }

  OracleResultSet::isSupported(__XDBC_HOOK_INVOKE_MORE resultSetType,resultSetConcurrency);
  __XDBC_HOOK_CHECK(return 0);

  OracleCallableStatement* p_call= new OracleCallableStatement(this,sql,resultSetType,resultSetConcurrency,paramCount);
  xdbc_smart_ptr<OracleCallableStatement> __sp_call(p_call); 
  
  p_call->initialize(__XDBC_HOOK_INVOKE_ONCE);
  __XDBC_HOOK_CHECK(return 0);

  return __sp_call.release();
}

ArrayStatement* OracleConnection::prepareArray(__XDBC_HOOK_DECLARE_MORE const string& sql,int resultSetType, int resultSetConcurrency,int paramCount)  __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    __sb.append("prepareArray|resultSetType=");
    __sb.append(resultSetConcurrency);
    __sb.append(",resultSetConcurrency=");
    __sb.append(resultSetConcurrency);
    
    __XDBC_FORCED_TRACE(ORACLECONNECTION_CLASS_NAME,__sb.str());
  }

  OracleResultSet::isSupported(__XDBC_HOOK_INVOKE_MORE resultSetType,resultSetConcurrency);
  __XDBC_HOOK_CHECK(return 0);
  
  OracleArrayStatement* p_pstmt= new OracleArrayStatement(this,sql,resultSetType,resultSetConcurrency,paramCount);
  xdbc_smart_ptr<OracleArrayStatement> __sp_pstmt(p_pstmt); 
  
  p_pstmt->initialize(__XDBC_HOOK_INVOKE_ONCE);
  __XDBC_HOOK_CHECK(return 0);

  return __sp_pstmt.release();
}

__OPENCODE_END_NAMESPACE
