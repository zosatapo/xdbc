/**
 *  odbcArrayStatement.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */

#ifndef   __OPENCODE_XDBC_ODBC_ARRAYSTATEMENT_H__
#define   __OPENCODE_XDBC_ODBC_ARRAYSTATEMENT_H__

#include "xdbc/odbc.h"

__OPENCODE_BEGIN_NAMESPACE

class __OPENCODE_XDBC_API_DECL OdbcArrayStatement: public  ArrayStatement
{
public:
  virtual ~OdbcArrayStatement();

  OdbcArrayStatement(OdbcConnection* pConnection,const string& sql,int paramCount);
  OdbcArrayStatement(OdbcConnection* pConnection,const string& sql, int resultSetType,int resultSetConcurrency,int paramCount);

  void initialize(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException);

public:
  __XDBC_INTERFACE_ARRAYSTATEMENT(ArrayStatement)

protected:
  void  setBindData(__XDBC_HOOK_DECLARE_MORE DataTable* paramTable) __XDBC_THROW_DECLARE(SQLException);
  void  setBindDataImpl(__XDBC_HOOK_DECLARE_MORE DataTable* paramTable,int parameterIndex) __XDBC_THROW_DECLARE(SQLException);
  
public:
//---------------------------------------
// ODBC
//---------------------------------------
  SQLHSTMT odbc_hstmt;

  OdbcConnection*               __mp_conn;
  ODBCSQLAPI_SQLEXECUTE        __api_SQLEXECUTE;
  ODBCSQLOPTIONS_SUPPORTSSTMTATTR  __options_SUPPORTSSTMTATTR;

protected:
//------------------------------------------------------
//
//------------------------------------------------------
  vector<ODBC_PARAM*> __vector_params;
  int  __mi_param_count;
  string  __ms_odbc_sql;

  int      __mi_resultSetType;
  int      __mi_resultSetConcurrency;

  bool     __mb_initialized;
  bool     __mb_isClosed;

  int      __mi_array_size;
  int      __mi_array_max ;
};

__OPENCODE_END_NAMESPACE

#endif

