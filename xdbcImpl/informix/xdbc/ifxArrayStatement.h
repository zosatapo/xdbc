/**
 *  ifxArrayStatement.h
 *
 *  Copyright (C) 2007-2012  zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef   __OPENCODE_XDBC_INFORMIX_ARRAYSTATEMENT_H__
#define   __OPENCODE_XDBC_INFORMIX_ARRAYSTATEMENT_H__

#include "xdbc/ifxdefs.h"
#include "xdbc/xmarco.h"
#include "xdbc/ifxStatement.h" 

__OPENCODE_BEGIN_NAMESPACE

class IfxArrayStatement:public  ArrayStatement
{
public:
  friend class IfxConnection;
  friend class IfxResultSet;
  friend class IfxArrayResultSet;

public:
  virtual ~IfxArrayStatement();
  
  IfxArrayStatement(IfxConnection* ora_conn,const string& sql,int paramCount);
  IfxArrayStatement(IfxConnection* ora_conn,const string& sql, int resultSetType, int resultSetConcurrency,int paramCount);

public:
  __XDBC_INTERFACE_ARRAYSTATEMENT(ArrayStatement)

  virtual void initialize(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException);

protected:
  void  setBindData(__XDBC_HOOK_DECLARE_MORE DataTable* paramTable,int row) __XDBC_THROW_DECLARE(SQLException);
  void  setBindData(__XDBC_HOOK_DECLARE_MORE DataTable* paramTable,int row,int parameterIndex) __XDBC_THROW_DECLARE(SQLException);
  
  void  parseInColumns(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException);
  void  parseInColumns_common(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException);
  void  parseInColumns_insert(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException); 
  void  parseInColumns_guess(__XDBC_HOOK_DECLARE_MORE const string& table_name,const string& column_list) __XDBC_THROW_DECLARE(SQLException);

protected:
//---------------------------------------
//---------------------------------------
  IfxConnection* __mp_conn;

  int      __mi_resultSetType;
  int      __mi_resultSetConcurrency;
  int      __mi_stmt_type;

  bool     __mb_initialized;
  bool     __mb_isClosed;

  string   str_ifx_sql;
  string   str_orginal_ifx_sql;
   
//---------------------------------------
// INFORMIX-ESQLC
//---------------------------------------
  struct sqlda*  __mp_ec_sqlda_vars;   /* sqlda struct pointer to Parameter */

  string __ms_cursor_name;
  string __ms_cursor_id;

  string str_var_cursor_name;
  string str_var_cursor_id;

//-------------------------------------------
// PARAMETER INFO DATA
//-------------------------------------------
  vector<IfxColumn>  __vector_vars; //PARAMETER metadata
  int                __mi_var_count;

  int  __mi_array_size;
  int  __mi_max_size;

  const static  int MAX_ROWCOUNT; 
};

__OPENCODE_END_NAMESPACE

#endif
