/**
 *  ifxConnection.h
 *
 *  Copyright (C) 2007-2012  zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef   __OPENCODE_XDBC_INFORMIX_CONNECTION_H__
#define   __OPENCODE_XDBC_INFORMIX_CONNECTION_H__

#include "xdbc/ifxdefs.h"
#include "xdbc/xmarco.h"
#include "port/mutex.h"
#include "port/ioc_vector.h"

__OPENCODE_BEGIN_NAMESPACE

/**
 * A SQL Connect URL string of the form: 
 * //server:[port][/database]
 *
 * such as:
 * //cbsdev01/mon
 */
class IfxConnection : public Connection
{
public:
  friend class IfxStatement;
  friend class IfxPreparedStatement;
  friend class IfxCallableStatement;
  friend class IfxArrayStatement;

  friend class IfxResultSet;
  friend class IfxArrayResultSet;

  friend class IfxClob;
  friend class IfxBlob;

  friend class IfxConversion;

public:
  virtual ~IfxConnection();
  IfxConnection();

public:
  __XDBC_INTERFACE_CONNECTION(Connection)

//--------------------------------------------------
// Transaction Helper Functions
//--------------------------------------------------
  virtual void  beginTrans(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException);
  virtual void  breakTrans(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException);
  
protected:
//---------------------------------
// INFORMIX-ESQLC
//---------------------------------
  
//---------------------------------
// STATE
//---------------------------------
  bool    __mb_initialized;
  bool    __mb_isClosed; 

  int     __mi_transLevel;
  bool    __mb_autoCommit;
  bool    __mb_explicit_trans;

  string  __ms_connName;

//-------------------------------------------
  static  int       __gi_connNumber;
  static  Mutex     __go_connNumber_mutex;
  static  string    getConnName();  
};

__OPENCODE_END_NAMESPACE

#endif

