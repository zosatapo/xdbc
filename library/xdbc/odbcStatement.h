/**
 *  odbcStatement.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef __OPENCODE_XDBC_ODBCSTATEMENT_H__
#define __OPENCODE_XDBC_ODBCSTATEMENT_H__

#include "xdbc/odbc.h"

__OPENCODE_BEGIN_NAMESPACE

class __OPENCODE_XDBC_API_DECL OdbcStatement: public  Statement
{
public:
  friend class OdbcConnection;
  friend class OdbcResultSet;

public:
  virtual ~OdbcStatement();
  
  OdbcStatement(OdbcConnection* pConnection);
  OdbcStatement(OdbcConnection* pConnection,int resultSetType, int resultSetConcurrency);

  void initialize(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException);

public:
  __XDBC_INTERFACE_STATEMENT(Statement)

public:
//---------------------------------------
// ODBC
//---------------------------------------
  SQLHSTMT odbc_hstmt;
  
  OdbcConnection*                __mp_conn;
  ODBCSQLOPTIONS_SUPPORTSSTMTATTR    __options_SUPPORTSSTMTATTR;
      
protected:
  int      __mi_resultSetType;
  int      __mi_resultSetConcurrency;

  bool     __mb_initialized;
  bool     __mb_isClosed;
};

__OPENCODE_END_NAMESPACE

#endif

