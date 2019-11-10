/**
 *  oracleCallableStatement.h
 *
 *  Copyright (C) 2002-2020 zosatapo 
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef   __OPENCODE_XDBC_ORACLE_CALLABLESTATEMENT_H__
#define   __OPENCODE_XDBC_ORACLE_CALLABLESTATEMENT_H__

#include "xdbc/oradefs.h"
#include "xdbc/xmarco.h"
#include "xdbc/oraclePreparedStatement.h"

__OPENCODE_BEGIN_NAMESPACE

class OracleCallableStatement :public  CallableStatement
{
public:
  friend class OracleConnection;
  friend class OracleResultSet;

public:
  virtual ~OracleCallableStatement();
  
  OracleCallableStatement(OracleConnection* ora_conn,const string& sql,int paramCount);
  OracleCallableStatement(OracleConnection* ora_conn,const string& sql, int resultSetType, int resultSetConcurrency,int paramCount);

public:
  __XDBC_INTERFACE_CALLABLESTATEMENT(CallableStatement)
  
  void initialize(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException);

protected:
  bool   checkConversion(__XDBC_HOOK_DECLARE_MORE int columnIndex,int outType,char* outBuffer) __XDBC_THROW_DECLARE(SQLException); 
  void   parseProcName(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException);
 
protected:
//-------------------------------------------
// Parameter INFO DATA
//-------------------------------------------
  xdbc_smart_ptr<OraclePreparedStatement>  __sp_pstmt;
  vector<OracleParameter*>  __vector_call_params; 
  int __mi_call_param_count;

//-------------------------------------------
// procecure/function name
//-------------------------------------------
  string __ms_proc_name;
  bool __mb_stored_proc;
};

__OPENCODE_END_NAMESPACE

#endif
