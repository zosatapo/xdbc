/**
 *  ifxStatement.h
 *
 *  Copyright (C) 2007-2012  zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef   __OPENCODE_XDBC_INFORMIX_STATEMENT_H__
#define   __OPENCODE_XDBC_INFORMIX_STATEMENT_H__

#include "xdbc/ifxdefs.h"
#include "xdbc/xmarco.h"
#include "port/mutex.h"
#include "port/ioc_vector.h"

__OPENCODE_BEGIN_NAMESPACE

class IfxStatement : public  Statement
{
public:
  virtual ~IfxStatement();
  
  IfxStatement(IfxConnection* ora_conn);
  IfxStatement(IfxConnection* ora_conn,int resultSetType, int resultSetConcurrency);

public: 
  __XDBC_INTERFACE_STATEMENT(Statement)
  
  void initialize(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException);

  static  int       getCursorCounter();

protected:
//---------------------------------------
//---------------------------------------
  IfxConnection* __mp_conn;

  int      __mi_resultSetType;
  int      __mi_resultSetConcurrency;
  int      __mi_stmt_type;

  bool     __mb_initialized;
  bool     __mb_isClosed;

//-------------------------------------------
  static  int       __gi_cursorNumber;
  static  Mutex     __go_cursorNumber_mutex;
};
__OPENCODE_END_NAMESPACE

#endif
