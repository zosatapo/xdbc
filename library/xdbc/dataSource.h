/**
 *  dataSource.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */

#ifndef   __OPENCODE_XDBC_DATASOURCE_H__
#define   __OPENCODE_XDBC_DATASOURCE_H__

#include "xdbc/xdefine.h"

#include "xdbc/connection.h"

__OPENCODE_BEGIN_NAMESPACE

#define DATASOURCE_DEFAULT_NAME  "DATABASE_CONF"

/** 
 * <p>A factory for connections to the physical data source that this
 * <code>DataSource</code> object represents.  An alternative to the
 * <code>DriverManager</code> facility, a <code>DataSource</code> object
 * is the preferred means of getting a connection. 
 * <P>
 * The <code>DataSource</code> interface is implemented by a driver vendor.
 * There are three types of implementations:
 * <OL>
 *   <LI>Basic implementation -- produces a standard <code>Connection</code> 
 *       object
 *   <LI>Connection pooling implementation -- produces a <code>Connection</code>
 *       object that will automatically participate in connection pooling.  This
 *       implementation works with a middle-tier connection pooling manager.
 *   <LI>Distributed transaction implementation -- produces a
 *       <code>Connection</code> object that may be used for distributed
 *       transactions and almost always participates in connection pooling. 
 *       This implementation works with a middle-tier 
 *       transaction manager and almost always with a connection 
 *       pooling manager.
 * </OL>
 *
 */

class __OPENCODE_XDBC_API_DECL DataSource 
{
public:
  virtual ~DataSource() {}

public:  
  /**
   * <p>Attempts to establish a connection with the data source that
   * this <code>DataSource</code> object represents.
   *
   * @return  a connection to the data source
   * @exception SQLException if a database access error occurs
   */
  virtual Connection* getConnection(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException) = 0;        

public:
  virtual void start(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException) = 0;    
  virtual void stop(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException) = 0;    
};

__OPENCODE_END_NAMESPACE

#endif

