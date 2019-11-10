/**
 *  ifxSQL.h
 *
 *  Copyright (C) 2007-2012  zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef __OPENCODE_XDBC_INFORMIX_SQL_H__
#define __OPENCODE_XDBC_INFORMIX_SQL_H__

#include "xdbc/ifxdefs.h"

#include "port/ioc_map.h" 

__OPENCODE_BEGIN_NAMESPACE

#define INFORMIX_TO_DATE_MARK '~'

class IfxSQL
{
//---------------------------------------------------------------------------
//   static block
//---------------------------------------------------------------------------
public:
  static string   parseSQL(__XDBC_HOOK_DECLARE_MORE const string& ifx_sql,int& param_count) __XDBC_THROW_DECLARE(SQLException);
  static int      parseSQLKind(const string& ifx_sql);

  static string   parseTableName(__XDBC_HOOK_DECLARE_MORE const string& srcSQL)  __XDBC_THROW_DECLARE(SQLException);
  static string   parseInColumns(__XDBC_HOOK_DECLARE_MORE const string& srcSQL) __XDBC_THROW_DECLARE(SQLException);

  static void     mapColumns(__XDBC_HOOK_DECLARE_MORE const string& column_list,OPENCODE_MAP<string,string>& __map_columns) __XDBC_THROW_DECLARE(SQLException);
  static void     mapTables(__XDBC_HOOK_DECLARE_MORE const string& table_name,OPENCODE_MAP<string,string>& map_tables) __XDBC_THROW_DECLARE(SQLException);

public:
  string  parse(__XDBC_HOOK_DECLARE_MORE const string ifx_sql,int& param_count) __XDBC_THROW_DECLARE(SQLException);

protected:
  void    appendChar(string& stringbuffer,const char& c1);
  void    handleCall(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException);
  void    handleDate(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException);
  void    handleEscape(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException);
  void    handleFunction(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException);
  void    handleODBC(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException);
  void    handleTime(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException);
  void    handleTimestamp(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException);
  void    handleToken(__XDBC_HOOK_DECLARE_MORE const string& ifx_sql) __XDBC_THROW_DECLARE(SQLException);
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
  string  ifx_sql;
  string  token_buffer;

public:
  const static  int SQL_STMT_UNKNOWN;
  const static  int SQL_STMT_SELECT ;
  const static  int SQL_STMT_UPDATE;
  const static  int SQL_STMT_DELETE;
  const static  int SQL_STMT_INSERT ;
  const static  int SQL_STMT_CREATE;
  const static  int SQL_STMT_DROP;
  const static  int SQL_STMT_ALTER;  
  const static  int SQL_STMT_BEGIN;
  const static  int SQL_STMT_DECLARE;
};

__OPENCODE_END_NAMESPACE

#endif
