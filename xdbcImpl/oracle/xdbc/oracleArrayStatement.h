/**
 *  oracleArrayStatement.h
 *
 *  Copyright (C) 2002-2020 zosatapo 
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef   __OPENCODE_XDBC_ORACLE_ARRAYSTATEMENT_H__
#define   __OPENCODE_XDBC_ORACLE_ARRAYSTATEMENT_H__

#include "xdbc/oradefs.h"
#include "xdbc/xmarco.h"
#include "xdbc/oracleStatement.h"
#include "port/ioc_vector.h"

__OPENCODE_BEGIN_NAMESPACE

class OracleArrayStatement: public  ArrayStatement
{
public:
  friend class OracleConnection;
  friend class OracleResultSet;
  friend class OracleArrayResultSet;

public:
  virtual ~OracleArrayStatement();
  
  OracleArrayStatement(OracleConnection* ora_conn,const string& sql,int paramCount);
  OracleArrayStatement(OracleConnection* ora_conn,const string& sql, int resultSetType,int resultSetConcurrency,int paramCount); 

public:
  __XDBC_INTERFACE_ARRAYSTATEMENT(ArrayStatement)
  
  void initialize(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException);
  
protected:
  virtual int   findBindIndex(int parameterIndex);

  virtual void  setBindData(__XDBC_HOOK_DECLARE_MORE DataTable* paramTable) __XDBC_THROW_DECLARE(SQLException);
  virtual void  setBindData(__XDBC_HOOK_DECLARE_MORE DataTable* paramTable,int parameterIndex) __XDBC_THROW_DECLARE(SQLException); 

protected:
  string  str_oracle_sql;
  string  str_orginal_oracle_sql;

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
  vector<OracleBind*> __vector_binds;
  int                __mi_param_count;
  int                __mi_array_size;

  int                __mi_max_size ;

  const static  int  MAX_ROWCOUNT;
};

__OPENCODE_END_NAMESPACE

#endif
