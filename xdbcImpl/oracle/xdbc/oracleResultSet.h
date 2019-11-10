/**
 *  oracleResultSet.h
 *
 *  Copyright (C) 2002-2020 zosatapo 
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef   __OPENCODE_XDBC_ORACLE_RESULTSET_H__
#define   __OPENCODE_XDBC_ORACLE_RESULTSET_H__

#include "xdbc/oradefs.h"
#include "xdbc/xmarco.h"
#include "port/ioc_map.h" 

#include <vector>
using OPENCODE_STL_NS::vector;
  
__OPENCODE_BEGIN_NAMESPACE

class OracleResultSetCollection : public ResultSetCollection
{ 
public:
  virtual ~OracleResultSetCollection() ;
  
  OracleResultSetCollection();
  OracleResultSetCollection(const OracleResultSetCollection& rv);
  OracleResultSetCollection& operator=(const OracleResultSetCollection& rv);

public:
  char* __p_rowbuf;
  int   __rowbuf_size;    
  
  vector<OracleDefine> __vector_defines;
  vector<OracleColumn> __vector_columns;
  OPENCODE_MAP<string,int>      __map_columns;
};

//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------

class OracleResultSet : public ResultSet
{
public:
  friend class OracleStatement;
  friend class OraclePreparedStatement;
  friend class OracleResultSetMetaData;

  virtual ~OracleResultSet();

  OracleResultSet(OracleConnection* ora_conn,SQLImplementation* pXstmt,OCIStmt* ociStmthp,
              int resultSetType, int resultSetConcurrency,
              OracleResultSetCollection* cachedCollection,bool adoptIt,
              bool selfDelete);
              
public:
  __XDBC_INTERFACE_RESULTSET(ResultSet)

public:
  static bool isSupported(__XDBC_HOOK_DECLARE_MORE int resultSetType, int resultSetConcurrency)  __XDBC_THROW_DECLARE(SQLException);
  static void prepareColumnData(__XDBC_HOOK_DECLARE_MORE OracleConnection* ora_conn,OCIStmt* ociStmthp,OracleResultSetCollection* cachedCollection)  __XDBC_THROW_DECLARE(SQLException);
  static void defineColumnData(__XDBC_HOOK_DECLARE_MORE OracleConnection* ora_conn,OCIStmt* ociStmthp,OracleResultSetCollection* cachedCollection)  __XDBC_THROW_DECLARE(SQLException);
  static void resetResultSet(OracleResultSet* pResultSet,OracleConnection* ora_conn,SQLImplementation* pXstmt,OCIStmt* ociStmthp ,
                              int resultSetType, int resultSetConcurrency,
                              OracleResultSetCollection* cachedCollection,bool adoptIt,
                              bool selfDelete);
  

  static void releaseColumnData(__XDBC_HOOK_DECLARE_MORE OracleConnection* ora_conn,OCIStmt* ociStmthp,OracleResultSetCollection* cachedCollection)  __XDBC_THROW_DECLARE(SQLException);

private: 
  bool   checkConversion(__XDBC_HOOK_DECLARE_MORE int columnIndex,int outType,char* outBuffer) __XDBC_THROW_DECLARE(SQLException);

private:
//---------------------------------------
// OCI
//--------------------------------------- 
  OracleConnection* __mp_conn;
  SQLImplementation* __p_xstmt;
  OCIStmt* oci_stmthp;
  
//-------------------------------------------
// STATE  Information
//-------------------------------------------
  bool __mb_selfDelete;
  bool __mb_isClosed;
  int  __mi_cur_row;

  int __mi_resultSetType;
  int __mi_resultSetConcurrency;
  int __mi_prefetch;
//-------------------------------------------
// ResultSetCollection 
//-------------------------------------------
  OracleResultSetCollection* __cachedCollection;
  int  __mi_column_count;
  bool __adoptIt;
};

__OPENCODE_END_NAMESPACE

#endif
