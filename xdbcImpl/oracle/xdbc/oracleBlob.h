/**
 *  oracleBlob.h
 *
 *  Copyright (C) 2002-2020 zosatapo 
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef   __OPENCODE_XDBC_ORACLE_XBLOB_H__
#define   __OPENCODE_XDBC_ORACLE_XBLOB_H__

#include "xdbc/oradefs.h"
#include "xdbc/xmarco.h"

__OPENCODE_BEGIN_NAMESPACE

class OracleBlob:public Blob
{
public:
  friend class OraclePreparedStatement;
  friend class OracleCallableStatement;

public:
  virtual ~OracleBlob();
  OracleBlob(OracleConnection* ora_conn);
  OracleBlob(OracleConnection* ora_conn, OCILobLocator*  pLob);

public:
  virtual void swap(OracleBlob& blob);

public:
  __XDBC_INTERFACE_BLOB(Blob)
  
private:
  OracleConnection* __mp_conn;
  OCILobLocator*    ora_LOB;
};

__OPENCODE_END_NAMESPACE

#endif
