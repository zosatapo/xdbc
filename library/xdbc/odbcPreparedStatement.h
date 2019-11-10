/**
 *  odbcPreparedStatement.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef __OPENCODE_XDBC_ODBCPREPAREDSTATEMENT_H__
#define __OPENCODE_XDBC_ODBCPREPAREDSTATEMENT_H__

#include "xdbc/odbc.h"
#include "xdbc/odbcResultSet.h"

__OPENCODE_BEGIN_NAMESPACE

class __OPENCODE_XDBC_API_DECL OdbcPreparedStatement : public  PreparedStatement
{
public:
  friend class OdbcConnection;
  friend class OdbcResultSet;

public:
  virtual ~OdbcPreparedStatement();
  
  OdbcPreparedStatement(OdbcConnection* pConnection,const string& sql,int paramCount);
  OdbcPreparedStatement(OdbcConnection* pConnection,const string& sql, int resultSetType, int resultSetConcurrency,int paramCount);

  void initialize(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException);

private:
  bool  checkConversion(__XDBC_HOOK_DECLARE_MORE int paramIndex,ODBC_PARAM* odbc_param,int funType,const char* funValue,int funValueSize = -1)__XDBC_THROW_DECLARE(SQLException);

public:
  __XDBC_INTERFACE_PREPAREDSTATEMENT(PreparedStatement)
 
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
  int  __mi_param_count;
  string  __ms_odbc_sql;
  
  char* __p_parambuf;
  int   __parambuf_size;

  int      __mi_resultSetType;
  int      __mi_resultSetConcurrency;

  bool     __mb_initialized;
  bool     __mb_isClosed;
  
//---------------------------------------
// ResultSet cache
//---------------------------------------
  OdbcResultSet* __p_rs_opened;
  OdbcResultSetCollection* __cachedCollection;
  bool __adoptIt;
  bool __isDefined;     
};

__OPENCODE_END_NAMESPACE

#endif

