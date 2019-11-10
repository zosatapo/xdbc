/**
 *  odbcConnection.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef __OPENCODE_XDBC_ODBCCONNECTION_H__
#define __OPENCODE_XDBC_ODBCCONNECTION_H__

#include "xdbc/odbc.h"

__OPENCODE_BEGIN_NAMESPACE

class __OPENCODE_XDBC_API_DECL OdbcConnection : public Connection
{
  friend class OdbcClob;
  friend class OdbcBlob;
  friend class OdbcStatement;
  friend class OdbcPreparedStatement;
  friend class OdbcResultSet;

public:
  virtual ~OdbcConnection();
  OdbcConnection();

public:
  __XDBC_INTERFACE_CONNECTION(Connection)


public:
//---------------------------------
// ODBC
//--------------------------------- 
  SQLHDBC odbc_hdbc ;
  SQLHENV odbc_henv ;  
  
  struct ODBC_LIBRARY_APIS*       __odbc_apis; 
  struct ODBC_DRIVER_OPTIONS*     __odbc_options;
  ODBCSQLOPTIONS_SUPPORTSCONNECTATTR  __options_SUPPORTSCONNECTATTR;
  ODBCSQLOPTIONS_SUPPORTSENVATTR      __options_SUPPORTSENVATTR;  
 
private:
//---------------------------------
// STATE
//---------------------------------
  bool  __mb_closed; 
  int   __mi_transLevel;
  bool  __mb_autoCommit;
  bool  __mb_proc_supported;
}; 

__OPENCODE_END_NAMESPACE

#endif

