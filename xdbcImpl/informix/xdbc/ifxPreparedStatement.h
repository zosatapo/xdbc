/**
 *  ifxPreparedStatement.h
 *
 *  Copyright (C) 2007-2012  zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef   __OPENCODE_XDBC_INFORMIX_PREPAREDSTATEMENT_H__
#define   __OPENCODE_XDBC_INFORMIX_PREPAREDSTATEMENT_H__

#include "xdbc/ifxdefs.h"
#include "xdbc/xmarco.h"
#include "xdbc/ifxStatement.h"

__OPENCODE_BEGIN_NAMESPACE

class IfxPreparedStatement:public  PreparedStatement
{
public:
  virtual ~IfxPreparedStatement();
  
  IfxPreparedStatement(IfxConnection* ora_conn,const string& sql,int paramCount);
  IfxPreparedStatement(IfxConnection* ora_conn,const string& sql, int resultSetType, int resultSetConcurrency,int paramCount);

public:
  __XDBC_INTERFACE_PREPAREDSTATEMENT(PreparedStatement)
  
  virtual void initialize(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException);
  
public:  
  virtual bool  checkConversion(__XDBC_HOOK_DECLARE_MORE int varIndex,int valueType,const char* value,int length) __XDBC_THROW_DECLARE(SQLException);
  virtual void  parseInColumns(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException);
  virtual void  parseInColumns_common(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException);
  virtual bool  parseInColumns_begin(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException);
  virtual void  parseInColumns_insert(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException); 
  virtual void  parseInColumns_guess(__XDBC_HOOK_DECLARE_MORE const string& table_name,const string& column_list) __XDBC_THROW_DECLARE(SQLException);

public:
  string  str_ifx_sql;
  string  str_orginal_ifx_sql;
//---------------------------------------
//---------------------------------------
  IfxConnection* __mp_conn;

  int      __mi_resultSetType;
  int      __mi_resultSetConcurrency;
  int      __mi_stmt_type;

  bool     __mb_initialized;
  bool     __mb_isClosed;
  
//---------------------------------------
// INFORMIX-ESQLC
//---------------------------------------
  struct sqlda*  __mp_ec_sqlda_vars;   /* sqlda struct pointer to Parameter */

  string __ms_cursor_name;
  string __ms_cursor_id;

  string str_var_cursor_name;
  string str_var_cursor_id;

//-------------------------------------------
// PARAMETER DATA BUFFER
//-------------------------------------------
  char* __mp_var_buffer;
  int   __mi_var_buf_size;

//-------------------------------------------
// PARAMETER INFO DATA
//-------------------------------------------
  vector<IfxColumn>  __vector_vars; //PARAMETER metadata

  vector<IfxBind>    __vector_binds;
  int                __mi_var_count;
};

__OPENCODE_END_NAMESPACE

#endif
