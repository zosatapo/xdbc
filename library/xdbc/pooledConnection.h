/**
 *  pooledConnection.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */

#ifndef   __OPENCODE_XDBC_POOLEDCONNECTION_H__
#define   __OPENCODE_XDBC_POOLEDCONNECTION_H__

#include "xdbc/xdefine.h"
#include "xdbc/connection.h"
#include "xdbc/xmarco.h"
#include "port/mutex.h"

#include "port/ioc_map.h" 

__OPENCODE_BEGIN_NAMESPACE

class PooledPreparedStatement;
class BasicDataSource;

class __OPENCODE_XDBC_API_DECL PooledConnection : public Connection
{
public:
  friend class BasicDataSource;
  
public:
  virtual ~PooledConnection(); 
  PooledConnection(BasicDataSource* pDataSource,Connection* pConnection);

public:
  __XDBC_INTERFACE_CONNECTION(Connection)

public:
  //======================================================================
  //   XDBC Connection - PreparedStatement Cache
  //======================================================================
  virtual PreparedStatement*  findPooledPreparedStatement(const string& key);
  virtual PreparedStatement*  findPooledPreparedStatement(int sqlid);
  virtual bool  insertPooledPreparedStatement(const string& key,int sqlid,PooledPreparedStatement* pstmt);

protected:
  virtual void pooledClose();
  virtual void pooledDestroy(); 
   
private:
  Connection* __p_connection; 
  BasicDataSource* __p_dataSource;
  bool __bol_isClosed;
  bool __bol_oldAutoCommit;
  
  OPENCODE_MAP<string,PooledPreparedStatement*>  __pstmt_cache;
  OPENCODE_MAP<int,PooledPreparedStatement*>     __pstmt_cache_fast;
};

__OPENCODE_END_NAMESPACE

#endif
