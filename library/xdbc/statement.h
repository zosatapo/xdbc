/**
 *  statement.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef   __OPENCODE_XDBC_STATEMENT_H__
#define   __OPENCODE_XDBC_STATEMENT_H__

#include "xdbc/xdefine.h"

#include "xdbc/connection.h"
#include "xdbc/resultset.h"

#include "port/ioc_map.h" 

__OPENCODE_BEGIN_NAMESPACE

class __OPENCODE_XDBC_API_DECL Statement : public  SQLImplementation
{ 
protected:
  virtual ~Statement(){}
  
public:    
  /**
   * Executes the given SQL statement, which returns a single 
   * <code>ResultSet</code> object.
   *
   * @param sql an SQL statement to be sent to the database, typically a 
   *        static SQL <code>SELECT</code> statement
   * @return a <code>ResultSet</code> object that contains the data produced 
   *         by the given query; never <code>null</code> 
   * @exception SQLException if a database access error occurs or the given
   *            SQL statement produces anything rv than a single
   *            <code>ResultSet</code> object
   */
  virtual ResultSet* executeQuery(__XDBC_HOOK_DECLARE_MORE const string& sql) __XDBC_THROW_DECLARE(SQLException) = 0;

  /**
   * Executes the given SQL statement, which may be an <code>INSERT</code>, 
   * <code>UPDATE</code>, or <code>DELETE</code> statement or an
   * SQL statement that returns nothing, such as an SQL DDL statement.
   *
   * @param sql an SQL <code>INSERT</code>, <code>UPDATE</code> or
   * <code>DELETE</code> statement or an SQL statement that returns nothing
   * @return either the row count for <code>INSERT</code>, <code>UPDATE</code>
   * or <code>DELETE</code> statements, or <code>0</code> for SQL statements 
   * that return nothing
   * @exception SQLException if a database access error occurs or the given
   *            SQL statement produces a <code>ResultSet</code> object
   */
  virtual int executeUpdate(__XDBC_HOOK_DECLARE_MORE const string& sql) __XDBC_THROW_DECLARE(SQLException) = 0;
};
__OPENCODE_END_NAMESPACE

#endif
