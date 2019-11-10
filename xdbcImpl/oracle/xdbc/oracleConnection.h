/**
 *  oracleConnection.h
 *
 *  Copyright (C) 2002-2020 zosatapo 
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef   __OPENCODE_XDBC_ORACLE_CONNECTION_H__
#define   __OPENCODE_XDBC_ORACLE_CONNECTION_H__

#include "xdbc/oradefs.h"
#include "xdbc/xmarco.h"
#include "port/ioc_vector.h"

__OPENCODE_BEGIN_NAMESPACE

/**
 * A SQL Connect URL string of the form: 
 * //host:[port][/service name]
 *
 * such as:
 * //dlsun242:5521/bjava21
 *
 * As an Oracle Net keyword-value pair. For example: 
 * "(DESCRIPTION=(ADDRESS=(PROTOCOL=tcp) (HOST=dlsun242) (PORT=5521))
 *  (CONNECT_DATA=(SERVICE_NAME=bjava21)))"
 *
 *  ====================================================================
 *
 *  Rules for Statement Caching,Here are some notes to follow:
 *  
 *    Use the function OCIStmtPrepare2() instead of OCIStmtPrepare(). If you are
 *  using OCIStmtPrepare(), you are strongly urged not to use a statement handle
 *  across different service contexts. Doing so will raise an error if the
 *  statement has been obtained by OCIStmtPrepare2(). Migration of a statement
 *  handle to a new service context actually closes the cursor associated with
 *  the old session and therefore no sharing is achieved. Client-side sharing is
 *  also not obtained, because OCI will free all buffers associated with the old
 *  session when the statement handle is migrated.
 *  
 *    You are urged to keep one service context for each session and use statement
 *  handles only for that service context. That will be the preferred and
 *  recommended model and usage.
 *  
 *    A call to OCIStmtPrepare2(), even if the session does not have a statement
 *  cache, will also allocate the statement handle and therefore applications
 *  using only OCIStmtPrepare2() should not call OCIHandleAlloc() for the
 *  statement handle.
 *  
 *    A call to the OCIStmtPrepare2() must be followed with OCIStmtRelease() after
 *  the user is done with the statement handle. If statement caching is used,
 *  this will release the statement to the cache. If statement caching is not
 *  used, the statement will be deallocated. Do not call OCIHandleFree() to free
 *  the memory.
 *  
 *    If the call to OCIStmtPrepare2() is made with the OCI_STMTCACHE_SEARCH_ONLY
 *  mode and a NULL statement was returned (statement was not found), the
 *  subsequent call to OCIStmtRelease() is not required and should not be
 *  performed.
 *  
 *    Do not call OCIStmtRelease() for a statement that was prepared using
 *  OCIStmtPrepare().
 *  The statement cache has a maximum size (number of statements) which can be
 *  modified by an attribute on the service context, OCI_ATTR_STMTCACHESIZE. The
 *  default value is 20.
 *  
 *    You can choose a to tag a statement at the release time so that the next time
 *  you can request a statement of the same tag. The tag will be used to search
 *  the cache. An untagged statement (tag is null) is a special case of a tagged
 *  statement. Two statements are considered different if they only differ in
 *  their tags, or if one is untagged and the other is not.
 */
class OracleConnection : public Connection
{
public:
  friend class OracleStatement;
  friend class OraclePreparedStatement;
  friend class OracleCallableStatement;
  friend class OracleArrayStatement;

  friend class OracleResultSet;
  friend class OracleArrayResultSet;

  friend class OracleClob;
  friend class OracleBlob;

  friend class OracleConversion;

public:
  virtual ~OracleConnection();
  OracleConnection();
  
public:
  __XDBC_INTERFACE_CONNECTION(Connection)
  
private:
//---------------------------------
// OCI
//---------------------------------
  OCIEnv*     oci_envhp;
  OCIError*   oci_errhp;

  OCISession* oci_authp; 
  OCIServer*  oci_srvhp;
  OCISvcCtx*  oci_svchp;    
  
//---------------------------------
// STATE
//---------------------------------
  bool  __mb_initialized;
  bool  __mb_isClosed; 
  int   __mi_transLevel;
  bool  __mb_autoCommit;

//---------------------------------
// CHARSET INFO
//---------------------------------
  int  __mi_svr_cs;
  int  __mi_env_cs;
};

__OPENCODE_END_NAMESPACE

#endif

