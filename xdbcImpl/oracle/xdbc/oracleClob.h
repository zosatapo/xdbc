/**
 *  oracleClob.h
 *
 *  Copyright (C) 2002-2020 zosatapo 
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef   __OPENCODE_XDBC_ORACLE_XCLOB_H__
#define   __OPENCODE_XDBC_ORACLE_XCLOB_H__

#include "xdbc/oradefs.h"
#include "xdbc/xmarco.h"

__OPENCODE_BEGIN_NAMESPACE

class OracleClob : public Clob 
{
public:
  friend class OraclePreparedStatement;
  friend class OracleCallableStatement;

public:
  virtual ~OracleClob();
  
  OracleClob(OracleConnection* ora_conn);
  OracleClob(OracleConnection* ora_conn,OCILobLocator* pLob);

public:
  virtual void swap(OracleClob& clob);

public:    
  __XDBC_INTERFACE_CLOB(Clob)


private:
  OracleConnection* __mp_conn;
  OCILobLocator*    ora_LOB;
};

__OPENCODE_END_NAMESPACE

#endif
