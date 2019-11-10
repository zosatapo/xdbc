/**
 *  resultset.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef   __OPENCODE_XDBC_ARRAYRESULTSET_H__
#define   __OPENCODE_XDBC_ARRAYRESULTSET_H__

#include "xdbc/xdefine.h"

#include "xdbc/clob.h"
#include "xdbc/blob.h"

__OPENCODE_BEGIN_NAMESPACE

class __OPENCODE_XDBC_API_DECL ArrayResultSet : public SQLManageable
{
protected:
  virtual ~ArrayResultSet(){}

public:
  /**
   * Releases this <code>ArrayResultSet</code> object's database and
   * XDBC resources immediately instead of waiting for
   * this to happen when it is automatically closed.
   *
   * <P><B>Note:</B> A <code>ArrayResultSet</code> object
   * is automatically closed by the
   * <code>Statement</code> object that generated it when
   * that <code>Statement</code> object is closed,
   * re-executed, or is used to retrieve the next result from a
   * sequence of multiple results. A <code>ArrayResultSet</code> object
   * is also automatically closed when it is garbage collected.  
   *
   * @exception SQLException if a database access error occurs
   */
  virtual void close() = 0;
  
public:
  /**
   * Moves the cursor down one row from its current position.
   * A <code>ArrayResultSet</code> cursor is initially positioned
   * before the first row; the first call to the method
   * <code>next</code> makes the first row the current row; the
   * second call makes the second row the current row, and so on. 
   *
   * <P>If an input stream is open for the current row, a call
   * to the method <code>next</code> will
   * implicitly close it. A <code>ArrayResultSet</code> object's
   * warning chain is cleared when a new row is read.
   *
   * @return number of rows returned . return <code>0</code> if there are no more rows
   * 
   * @exception SQLException if a database access error occurs
   */
  virtual int next(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException) = 0;
  
  /**
   * Maps the given <code>ArrayResultSet</code> column name to its
   * <code>ArrayResultSet</code> column index.
   *
   * @param columnName the name of the column
   * @return the column index of the given column name
   * @exception SQLException if the <code>ArrayResultSet</code> object
   * does not contain <code>columnName</code> or a database access error occurs
   */
  virtual int findColumn(__XDBC_HOOK_DECLARE_MORE const string& columnName) __XDBC_THROW_DECLARE(SQLException) = 0;
  
  /**
   * Retrieves the  number, types and properties of
   * this <code>ResultSet</code> object's columns.
   *
   * @return the description of this <code>ArrayResultSet</code> object's columns
   * @exception SQLException if a database access error occurs
   */
  virtual ResultSetMetaData* getMetaData(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException) = 0;

};

__OPENCODE_END_NAMESPACE

#endif
