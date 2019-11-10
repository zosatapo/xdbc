/**
 *  odbcResultSet.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef __OPENCODE_XDBC_ODBCRESULTSET_H__
#define __OPENCODE_XDBC_ODBCRESULTSET_H__

#include "xdbc/odbc.h"

__OPENCODE_BEGIN_NAMESPACE

//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------

class __OPENCODE_XDBC_API_DECL OdbcResultSet : public ResultSet
{
public:
  friend class OdbcStatement;
  friend class OdbcPreparedStatement;

public:
  virtual ~OdbcResultSet();
 
  OdbcResultSet(OdbcConnection* pConnection,SQLImplementation* pXimpl,SQLHSTMT odbcHstmt,
              int resultSetType, int resultSetConcurrency,
              OdbcResultSetCollection* cachedCollection,bool adoptIt,
              bool selfDelete);
 
private:
  bool   checkConversion(__XDBC_HOOK_DECLARE_MORE int columnIndex,ODBC_DEFINE* odbc_define,int outType,char* outBuffer)__XDBC_THROW_DECLARE(SQLException);

public:
  __XDBC_INTERFACE_RESULTSET(ResultSet)

public:
//---------------------------------------
// ODBC
//---------------------------------------  
  SQLHSTMT odbc_hstmt;
  
  OdbcConnection*              __mp_conn;  
  bool                         __mb_fintAvailable;
  ODBCSQLAPI_SQLFETCH         __api_SQLFETCH;
  ODBCSQLAPI_SQLFETCHSCROLL   __api_SQLFETCHSCROLL;
private:
//-------------------------------------------
// STATE  Information
//------------------------------------------- 
  SQLImplementation* __p_ximpl;
  
  bool __mb_selfDelete;
  bool __mb_isClosed;
  int  __mi_cur_row;

  int __mi_resultSetType;
  int __mi_resultSetConcurrency;
  int __mi_prefetch;
  
//-------------------------------------------
// ResultSetCollection 
//-------------------------------------------
  OdbcResultSetCollection* __cachedCollection;
  int  __mi_column_count;
  bool __adoptIt;

public:
  static bool isSupported(__XDBC_HOOK_DECLARE_MORE int resultSetType, int resultSetConcurrency) __XDBC_THROW_DECLARE(SQLException);
  static void prepareColumnData(__XDBC_HOOK_DECLARE_MORE OdbcConnection* pConnection,const string& odbcSQL,SQLHSTMT odbcHstmt,OdbcResultSetCollection* cachedCollection) __XDBC_THROW_DECLARE(SQLException);
  static void defineColumnData(__XDBC_HOOK_DECLARE_MORE OdbcConnection* pConnection,SQLHSTMT odbcHstmt,OdbcResultSetCollection* cachedCollection) __XDBC_THROW_DECLARE(SQLException);
  static void resetResultSet(OdbcResultSet* pResultSet,OdbcConnection* pConnection,SQLImplementation* pXimpl,SQLHSTMT odbcHstmt,
                              int resultSetType, int resultSetConcurrency,
                              OdbcResultSetCollection* cachedCollection,bool adoptIt,
                              bool selfDelete);
};

__OPENCODE_END_NAMESPACE

#endif

