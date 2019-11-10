/**
 *  oraclePreparedStatement.h
 *
 *  Copyright (C) 2002-2020 zosatapo 
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef   __OPENCODE_XDBC_ORACLE_PREPAREDSTATEMENT_H__
#define   __OPENCODE_XDBC_ORACLE_PREPAREDSTATEMENT_H__

#include "xdbc/oradefs.h"
#include "xdbc/xmarco.h"
#include "xdbc/oracleStatement.h"

__OPENCODE_BEGIN_NAMESPACE

class OracleResultSetCollection;

class OraclePreparedStatement:public  PreparedStatement
{
public:
  friend class OracleConnection;
  friend class OracleCallableStatement;
  friend class OracleResultSet;

public:
  virtual ~OraclePreparedStatement();
  
  OraclePreparedStatement(OracleConnection* ora_conn,const string& sql,int paramCount);
  OraclePreparedStatement(OracleConnection* ora_conn,const string& sql, int resultSetType, int resultSetConcurrency,int paramCount);

public:
  __XDBC_INTERFACE_PREPAREDSTATEMENT(PreparedStatement)
  
  void initialize(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException);
  
protected:
  string  str_oracle_sql;
  string  str_orginal_oracle_sql;

protected:
//---------------------------------------
// OCI
//--------------------------------------- 
  OracleConnection* __mp_conn;
  OCIStmt* oci_stmthp;
  
//-------------------------------------------
// STATE  Information
//-------------------------------------------
  int      __mi_resultSetType;
  int      __mi_resultSetConcurrency;

  bool     __mb_initialized;
  bool     __mb_isClosed;

//------------------------------------------------------
// 
//------------------------------------------------------  
  vector<OracleParameter*> __vector_params; 

  vector<OracleBind*> __vector_binds;
  int                __mi_param_count;

//---------------------------------------
// ResultSet cache
//---------------------------------------
  OracleResultSet* __p_rs_opened;
  OracleResultSetCollection* __cachedCollection;
  bool __adoptIt;
  bool __isDefined;
};

__OPENCODE_END_NAMESPACE

#endif
