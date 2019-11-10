/**
 *  oracleSQL.h
 *
 *  Copyright (C) 2002-2020 zosatapo 
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef __OPENCODE_XDBC_ORACLE_SQL_H__
#define __OPENCODE_XDBC_ORACLE_SQL_H__

#include "xdbc/oradefs.h"

__OPENCODE_BEGIN_NAMESPACE

class OracleSQL
{
public:
  string  parse(__XDBC_HOOK_DECLARE_MORE const string oracle_sql,int& param_count) __XDBC_THROW_DECLARE(SQLException);

protected:
  void    appendChar(string& stringbuffer,const char& c1);
  void    handleCall(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException);
  void    handleDate(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException);
  void    handleEscape(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException);
  void    handleFunction(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException);
  void    handleODBC(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException);
  void    handleTime(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException);
  void    handleTimestamp(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException);
  void    handleToken(__XDBC_HOOK_DECLARE_MORE const string& oracle_sql) __XDBC_THROW_DECLARE(SQLException);
  string  nextArgument();
  void    skipSpace();

protected:
  int     current_argument;
  int     i;
  int     length;
  char    c;
  bool    first;
  bool    in_string;
  string  odbc_sql;
  string  oracle_sql;
  string  token_buffer;

//---------------------------------------------------------------------------
//   static block
//---------------------------------------------------------------------------
public:
  static string   parseSQL(__XDBC_HOOK_DECLARE_MORE const string& oracle_sql,int& param_count) __XDBC_THROW_DECLARE(SQLException);
  
  static int      parseSQLKind(const string& oracle_sql);

public:
  const static  int SQL_STMT_UNKNOWN;
  const static  int SQL_STMT_SELECT;
  const static  int SQL_STMT_UPDATE;
  const static  int SQL_STMT_DELETE;
  const static  int SQL_STMT_INSERT;
  const static  int SQL_STMT_CREATE;
  const static  int SQL_STMT_DROP;
  const static  int SQL_STMT_ALTER;  
  const static  int SQL_STMT_BEGIN ;
  const static  int SQL_STMT_DECLARE;
};

__OPENCODE_END_NAMESPACE

#endif
