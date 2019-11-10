/**
 *  oracleStatement.h
 *
 *  Copyright (C) 2002-2020 zosatapo 
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef   __OPENCODE_XDBC_ORACLE_STATEMENT_H__
#define   __OPENCODE_XDBC_ORACLE_STATEMENT_H__

#include "xdbc/oradefs.h"
#include "xdbc/xmarco.h"
#include "port/ioc_vector.h"

__OPENCODE_BEGIN_NAMESPACE

class OracleStatement : public  Statement
{
public:
  friend class OracleConnection;
  friend class OracleResultSet;
  friend class OracleArrayResultSet;

public:
  virtual ~OracleStatement();
  
  OracleStatement(OracleConnection* ora_conn);
  OracleStatement(OracleConnection* ora_conn,int resultSetType, int resultSetConcurrency);

public:
  __XDBC_INTERFACE_STATEMENT(Statement)
 
  void initialize(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException);
  
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
};
__OPENCODE_END_NAMESPACE

#endif
