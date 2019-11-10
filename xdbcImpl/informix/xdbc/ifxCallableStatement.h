/**
 *  ifxCallableStatement.h
 *
 *  Copyright (C) 2007-2012  zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef   __OPENCODE_XDBC_INFORMIX_CALLABLESTATEMENT_H__
#define   __OPENCODE_XDBC_INFORMIX_CALLABLESTATEMENT_H__

#include "xdbc/ifxdefs.h"
#include "xdbc/xmarco.h"
#include "xdbc/ifxPreparedStatement.h"

__OPENCODE_BEGIN_NAMESPACE

class IfxCallableStatement :public  CallableStatement
{
public:
  friend class IfxConnection;
  friend class IfxResultSet;

public:
  virtual ~IfxCallableStatement();
  
  IfxCallableStatement(IfxConnection* ora_conn,const string& sql,int paramCount);
  IfxCallableStatement(IfxConnection* ora_conn,const string& sql, int resultSetType, int resultSetConcurrency,int paramCount);

public: 
  __XDBC_INTERFACE_CALLABLESTATEMENT(CallableStatement)
  
  virtual void initialize(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException);

protected:
   bool  checkConversion(__XDBC_HOOK_DECLARE_MORE int columnIndex,int outType,char* outBuffer) __XDBC_THROW_DECLARE(SQLException);
   void  checkStoredProcedure(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException); 
   bool  parseInColumns_begin(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException);
   void  parseProcName(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException);
 
protected:
//---------------------------------------
// INFORMIX-ESQLC
//---------------------------------------
  struct sqlda*  __mp_ec_sqlda_values;   /* sqlda struct pointer to Column */
  xdbc_smart_ptr<IfxPreparedStatement> __sp_pstmt;

//-------------------------------------------
// Return INFO DATA
//-------------------------------------------
  vector<IfxColumn>  __vector_values; 

//-------------------------------------------
// VALUE DATA BUFFER
//-------------------------------------------
  char* __mp_value_buffer;
  int   __mi_value_buf_size;

  vector<IfxDefine> __vector_defines;

  int  __mi_value_count;

//-------------------------------------------
// procecure name
//-------------------------------------------
  string __ms_proc_name;
  bool __mb_is_proc;

};

__OPENCODE_END_NAMESPACE

#endif
