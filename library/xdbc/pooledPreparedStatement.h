/**
 *  pooledPreparedStatement.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef   __OPENCODE_XDBC_POOLEDPREPAREDSTATEMENT_H__
#define   __OPENCODE_XDBC_POOLEDPREPAREDSTATEMENT_H__

#include "xdbc/xdefine.h"
#include "xdbc/xmarco.h"
#include "xdbc/preparedStatement.h"
#include "xdbc/pooledConnection.h"

__OPENCODE_BEGIN_NAMESPACE

class __OPENCODE_XDBC_API_DECL PooledPreparedStatement : public  PreparedStatement 
{  
public:
  friend class PooledConnection;
  friend class BasicDataSource;
  
public:
  virtual ~PooledPreparedStatement();
  PooledPreparedStatement(PreparedStatement* pPreparedStatement);

public:
  __XDBC_INTERFACE_PREPAREDSTATEMENT(PreparedStatement)

protected:
  virtual void pooledClose();
  virtual void pooledDestroy();   
  
private:
  PreparedStatement* __p_pstmt;
  int __pooled_ref_count;
};

__OPENCODE_END_NAMESPACE

#endif
