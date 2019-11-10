/**
 *  odbcCallableStatement.h
 *
 *  Copyright (C) 2002-2020 zosatapo 
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef   __OPENCODE_XDBC_ODBC_CALLABLESTATEMENT_H__
#define   __OPENCODE_XDBC_ODBC_CALLABLESTATEMENT_H__

#include "xdbc/odbc.h"
#include "xdbc/odbcPreparedStatement.h"

__OPENCODE_BEGIN_NAMESPACE

class __OPENCODE_XDBC_API_DECL OdbcCallableStatement : public  CallableStatement
{
public:
  friend class OdbcConnection;
  friend class OdbcResultSet;

public:
  virtual ~OdbcCallableStatement();
  
  OdbcCallableStatement(OdbcConnection* pConnection,const string& sql,int paramCount);
  OdbcCallableStatement(OdbcConnection* pConnection,const string& sql, int resultSetType, int resultSetConcurrency,int paramCount);

  void initialize(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException);

public:
  __XDBC_INTERFACE_CALLABLESTATEMENT(CallableStatement)
  
protected: 
  void   parseProcName(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException);
  void   parseProcVariables(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException);
  
  void   parseProcVariablesImpl(__XDBC_HOOK_DECLARE_MORE SQLHSTMT odbc_hstmt_for_proc)  __XDBC_THROW_DECLARE(SQLException);
 
  bool   checkConversion(__XDBC_HOOK_DECLARE_MORE int columnIndex,ODBC_PARAM* odbc_param,int outType,char* outBuffer) __XDBC_THROW_DECLARE(SQLException); 
  
public:
//---------------------------------------
// ODBC
//---------------------------------------
  SQLHSTMT odbc_hstmt;
  
  OdbcConnection*               __mp_conn;
  bool                          __mb_fintAvailable;
  ODBCSQLAPI_SQLEXECUTE        __api_SQLEXECUTE;
  ODBCSQLOPTIONS_SUPPORTSSTMTATTR  __options_SUPPORTSSTMTATTR;
   
protected:
//------------------------------------------------------
// 
//------------------------------------------------------
  vector<ODBC_PARAM*> __vector_params; 
  int      __mi_param_count;

  string   __ms_odbc_sql;
  string   __ms_proc_name;
  string   __ms_schema_name;
  bool     __mb_stored_proc;

  int      __mi_resultSetType;
  int      __mi_resultSetConcurrency;
     
  char*    __p_parambuf;
  int      __parambuf_size;

  bool     __mb_initialized;
  bool     __mb_isClosed;
};

__OPENCODE_END_NAMESPACE

#endif
