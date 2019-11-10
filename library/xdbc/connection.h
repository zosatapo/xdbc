/**
 *  connection.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef   __OPENCODE_XDBC_CONNECTION_H__
#define   __OPENCODE_XDBC_CONNECTION_H__

#include "xdbc/xdefine.h"
#include "port/ioc_map.h"
  
__OPENCODE_BEGIN_NAMESPACE

/**
 * A constant indicating that transactions are not supported. 
 */
#define  XCONNECTION_TRANSACTION_NONE                0

/**
 * A constant indicating that
 * dirty reads, non-repeatable reads and phantom reads can occur.
 * This level allows a row changed by one transaction to be read
 * by another transaction before any changes in that row have been
 * committed (a "dirty read").  If any of the changes are rolled back, 
 * the second transaction will have retrieved an invalid row.
 */
#define  XCONNECTION_TRANSACTION_READ_UNCOMMITTED     1

/**
 * A constant indicating that
 * dirty reads are prevented; non-repeatable reads and phantom
 * reads can occur.  This level only prohibits a transaction
 * from reading a row with uncommitted changes in it.
 */
#define  XCONNECTION_TRANSACTION_READ_COMMITTED        2

/**
 * A constant indicating that
 * dirty reads and non-repeatable reads are prevented; phantom
 * reads can occur.  This level prohibits a transaction from
 * reading a row with uncommitted changes in it, and it also
 * prohibits the situation where one transaction reads a row,
 * a second transaction alters the row, and the first transaction
 * rereads the row, getting different values the second time
 * (a "non-repeatable read").
 */
#define  XCONNECTION_TRANSACTION_REPEATABLE_READ       4

/**
 * A constant indicating that
 * dirty reads, non-repeatable reads and phantom reads are prevented.
 * This level includes the prohibitions in
 * <code>TRANSACTION_REPEATABLE_READ</code> and further prohibits the 
 * situation where one transaction reads all rows that satisfy
 * a <code>WHERE</code> condition, a second transaction inserts a row that
 * satisfies that <code>WHERE</code> condition, and the first transaction
 * rereads for the same condition, retrieving the additional
 * "phantom" row in the second read.
 */
#define  XCONNECTION_TRANSACTION_SERIALIZABLE            8


class __OPENCODE_XDBC_API_DECL Connection : public  SQLManageable
{
protected:
  virtual ~Connection(){}
  
public:
  /**
   * Releases this <code>Connection</code> object's database and XDBC resources
   * immediately instead of waiting for them to be automatically released.
   * <P>
   * Calling the method <code>close</code> on a <code>Connection</code>
   * object that is already closed is a no-op.
   *
   */
  virtual void close() = 0;
  
  /** Destroy this <code>Connection</code> object */  
  virtual void destroy() = 0;
  
public:
  /**
   * Attempts to make a database connection to the given URL.
   * The driver should return "null" if it realizes it is the wrong kind
   * of driver to connect to the given URL.  This will be common, as when
   * the XDBC driver manager is asked to connect to a given URL it passes
   * the URL to each loaded driver in turn.
   *
   * <P>The driver should throw an <code>SQLException</code> if it is the right 
   * driver to connect to the given URL but has trouble connecting to
   * the database.
   *
   * <P>The <code>map<string,string></code> argument can be used to pass
   * arbitrary string tag/value pairs as connection arguments.
   * Normally at least "user" and "password" properties should be
   * included in the <code>map<string,string></code> object.
   *
   * @param url the URL of the database to which to connect
   * @param info a list of arbitrary string tag/value pairs as
   * connection arguments. Normally at least a "user" and
   * "password" property should be included.
   * @return  <code>true</code> if get a connection to the URL ,false otherwise
   * @exception SQLException if a database access error occurs
   */
  virtual bool connect(__XDBC_HOOK_DECLARE_MORE const string& url) __XDBC_THROW_DECLARE(SQLException) = 0;
  virtual bool connect(__XDBC_HOOK_DECLARE_MORE const string& url, const string& user, const string& password) __XDBC_THROW_DECLARE(SQLException) = 0;
  virtual bool connect(__XDBC_HOOK_DECLARE_MORE const string& url, __XDBC_ATTRIBUTES& info) __XDBC_THROW_DECLARE(SQLException) = 0;

  /**
   * Creates a <code>Statement</code> object for sending
   * SQL statements to the database.
   * SQL statements without parameters are normally
   * executed using <code>Statement</code> objects. If the same SQL statement 
   * is executed many times, it may be more efficient to use a 
   * <code>PreparedStatement</code> object.
   * <P>
   * Result sets created using the returned <code>Statement</code>
   * object will by default be type <code>TYPE_FORWARD_ONLY</code>
   * and have a concurrency level of <code>CONCUR_READ_ONLY</code>.
   *
   * @return a new default <code>Statement</code> object 
   * @exception SQLException if a database access error occurs
   */
  virtual Statement* createStatement(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException) = 0;
  
  /**
   * Creates a <code>PreparedStatement</code> object for sending
   * parameterized SQL statements to the database.
   * <P>
   * A SQL statement with or without IN parameters can be
   * pre-compiled and stored in a <code>PreparedStatement</code> object. This
   * object can then be used to efficiently execute this statement
   * multiple times.
   *
   * <P><B>Note:</B> This method is optimized for handling
   * parametric SQL statements that benefit from precompilation. If
   * the driver supports precompilation,
   * the method <code>prepareStatement</code> will send
   * the statement to the database for precompilation. Some drivers
   * may not support precompilation. In this case, the statement may
   * not be sent to the database until the <code>PreparedStatement</code> 
   * object is executed.  This has no direct effect on users; however, it does
   * affect which methods throw certain <code>SQLException</code> objects.
   * <P>
   * Result sets created using the returned <code>PreparedStatement</code>
   * object will by default be type <code>TYPE_FORWARD_ONLY</code>
   * and have a concurrency level of <code>CONCUR_READ_ONLY</code>.
   *
   * @param sql an SQL statement that may contain one or more '?' IN
   * parameter placeholders
   * @return a new default <code>PreparedStatement</code> object containing the
   * pre-compiled SQL statement 
   * @exception SQLException if a database access error occurs
   */
  virtual PreparedStatement* prepareStatement(__XDBC_HOOK_DECLARE_MORE const string& sql) __XDBC_THROW_DECLARE(SQLException)
  {
    return prepareStatement(__XDBC_HOOK_INVOKE_MORE sql,SQLImplementation::paramCountUnknown);
  }   
  virtual PreparedStatement* prepareStatement(__XDBC_HOOK_DECLARE_MORE const string& sql,int paramCount) __XDBC_THROW_DECLARE(SQLException) = 0;
  
  /**
   * Creates a <code>CallableStatement</code> object for calling
   * database stored procedures.
   * The <code>CallableStatement</code> object provides
   * methods for setting up its IN and OUT parameters, and
   * methods for executing the call to a stored procedure.
   *
   * <P><B>Note:</B> This method is optimized for handling stored
   * procedure call statements. Some drivers may send the call
   * statement to the database when the method <code>prepareCall</code>
   * is done; others
   * may wait until the <code>CallableStatement</code> object
   * is executed. This has no
   * direct effect on users; however, it does affect which method
   * throws certain SQLExceptions.
   * <P>
   * Result sets created using the returned <code>CallableStatement</code>
   * object will by default be type <code>TYPE_FORWARD_ONLY</code>
   * and have a concurrency level of <code>CONCUR_READ_ONLY</code>.
   *
   * @param sql an SQL statement that may contain one or more '?'
   * parameter placeholders. Typically this  statement is a XDBC
   * function call escape string.
   * @return a new default <code>CallableStatement</code> object containing the
   * pre-compiled SQL statement 
   * @exception SQLException if a database access error occurs
   */
  virtual CallableStatement* prepareCall(__XDBC_HOOK_DECLARE_MORE const string& sql) __XDBC_THROW_DECLARE(SQLException)
  {
    return prepareCall(__XDBC_HOOK_INVOKE_MORE sql,SQLImplementation::paramCountUnknown);
  } 
  virtual CallableStatement* prepareCall(__XDBC_HOOK_DECLARE_MORE const string& sql,int paramCount) __XDBC_THROW_DECLARE(SQLException) = 0;
  
  /**
   * Creates a <code>ArrayStatement</code> object for sending
   * parameterized SQL statements to the database.
   * <P>
   * A SQL statement with or without IN parameters can be
   * pre-compiled and stored in a <code>ArrayStatement</code> object. This
   * object can then be used to efficiently execute this statement
   * multiple times.
   *
   * <P><B>Note:</B> This method is optimized for handling
   * parametric SQL statements that benefit from precompilation. If
   * the driver supports precompilation,
   * the method <code>prepareArray</code> will send
   * the statement to the database for precompilation. Some drivers
   * may not support precompilation. In this case, the statement may
   * not be sent to the database until the <code>ArrayStatement</code>
   * object is executed.  This has no direct effect on users; however, it does
   * affect which methods throw certain <code>SQLException</code> objects.
   * <P>
   * Result sets created using the returned <code>PreparedStatement</code>
   * object will by default be type <code>TYPE_FORWARD_ONLY</code>
   * and have a concurrency level of <code>CONCUR_READ_ONLY</code>.
   *
   * @param sql an SQL statement that may contain one or more '?' IN
   * parameter placeholders
   * @return a new default <code>ArrayStatement</code> object containing the
   * pre-compiled SQL statement
   * @exception SQLException if a database access error occurs
   */
  virtual ArrayStatement* prepareArray(__XDBC_HOOK_DECLARE_MORE const string& sql) __XDBC_THROW_DECLARE(SQLException)
  {
    return prepareArray(__XDBC_HOOK_INVOKE_MORE sql,SQLImplementation::paramCountUnknown);
  }  
  virtual ArrayStatement* prepareArray(__XDBC_HOOK_DECLARE_MORE const string& sql,int paramCount) __XDBC_THROW_DECLARE(SQLException) = 0;
            
  /**
   * Sets this connection's auto-commit mode to the given state.
   * If a connection is in auto-commit mode, then all its SQL
   * statements will be executed and committed as individual
   * transactions.  Otherwise, its SQL statements are grouped into
   * transactions that are terminated by a call to either
   * the method <code>commit</code> or the method <code>rollback</code>.
   * By default, new connections are in auto-commit
   * mode.
   * <P>
   * The commit occurs when the statement completes or the next
   * execute occurs, whichever comes first. In the case of
   * statements returning a <code>ResultSet</code> object, 
   * the statement completes when the last row of the 
   * <code>ResultSet</code> object has been retrieved or the
   * <code>ResultSet</code> object has been closed. In advanced cases, a single
   * statement may return multiple results as well as output
   * parameter values. In these cases, the commit occurs when all results and
   * output parameter values have been retrieved.
   * <P>
   * <B>NOTE:</B>  If this method is called during a transaction, the
   * transaction is committed.
   *
   * @param autoCommit <code>true</code> to enable auto-commit mode; 
   *         <code>false</code> to disable it
   * @exception SQLException if a database access error occurs
   */
  virtual void setAutoCommit(__XDBC_HOOK_DECLARE_MORE bool autoCommit) __XDBC_THROW_DECLARE(SQLException) = 0;

  /**
   * Retrieves the current auto-commit mode for this <code>Connection</code>
   * object.
   *
   * @return the current state of this <code>Connection</code> object's 
   *         auto-commit mode
   * @exception SQLException if a database access error occurs
   */
  virtual bool getAutoCommit(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException) = 0;

  /**
   * Makes all changes made since the previous
   * commit/rollback permanent and releases any database locks
   * currently held by this <code>Connection</code> object. 
   * This method should be
   * used only when auto-commit mode has been disabled.
   *
   * @exception SQLException if a database access error occurs or this
   *            <code>Connection</code> object is in auto-commit mode
   */
  virtual void commit(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException) = 0;

  /**
   * Undoes all changes made in the current transaction
   * and releases any database locks currently held
   * by this <code>Connection</code> object. This method should be 
   * used only when auto-commit mode has been disabled.
   *
   * @exception SQLException if a database access error occurs or this
   *            <code>Connection</code> object is in auto-commit mode
   */
  virtual void rollback(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException) = 0;
  
  /**
   * Retrieves whether this <code>Connection</code> object has been
   * closed.  A connection is closed if the method <code>close</code>
   * has been called on it or if certain fatal errors have occurred.
   * This method is guaranteed to return <code>true</code> only when
   * it is called after the method <code>Connection::close</code> has
   * been called.
   *
   * @return <code>true</code> if this <code>Connection</code> object 
   *         is closed; <code>false</code> if it is still open
   */
  virtual bool isClosed() = 0;
       
  /**
   * Attempts to change the transaction isolation level for this
   * <code>Connection</code> object to the one given.
   * The constants defined in the interface <code>Connection</code>
   * are the possible transaction isolation levels.
   * <P>
   * <B>Note:</B> If this method is called during a transaction, the result
   * is implementation-defined.
   *
   * @param level one of the following <code>Connection</code> constants:
   *        <code>Connection::TRANSACTION_READ_UNCOMMITTED</code>,
   *        <code>Connection::TRANSACTION_READ_COMMITTED</code>,
   *        <code>Connection::TRANSACTION_REPEATABLE_READ</code>, or
   *        <code>Connection::TRANSACTION_SERIALIZABLE</code>.
   *        (Note that <code>Connection::TRANSACTION_NONE</code> cannot be used 
   *        because it specifies that transactions are not supported.)
   * @exception SQLException if a database access error occurs
   *            or the given parameter is not one of the <code>Connection</code>
   *            constants
   */
  virtual void setTransactionIsolation(__XDBC_HOOK_DECLARE_MORE int level) __XDBC_THROW_DECLARE(SQLException) = 0;
  
  /**
   * Retrieves this <code>Connection</code> object's current 
   * transaction isolation level.
   *
   * @return the current transaction isolation level, which will be one
   *         of the following constants:
   *        <code>Connection::TRANSACTION_READ_UNCOMMITTED</code>, 
   *        <code>Connection::TRANSACTION_READ_COMMITTED</code>,
   *        <code>Connection::TRANSACTION_REPEATABLE_READ</code>, 
   *        <code>Connection::TRANSACTION_SERIALIZABLE</code>, or
   *        <code>Connection::TRANSACTION_NONE</code>.
   * @exception SQLException if a database access error occurs
   */
  virtual int getTransactionIsolation(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException) = 0;

  /**
   * Creates a <code>Statement</code> object that will generate
   * <code>ResultSet</code> objects with the given type and concurrency.
   * This method is the same as the <code>createStatement</code> method
   * above, but it allows the default result set
   * type and concurrency to be overridden.
   *
   * @param resultSetType a result set type; one of 
   *        <code>ResultSet::TYPE_FORWARD_ONLY</code>,
   *        <code>ResultSet::TYPE_SCROLL_INSENSITIVE</code>, or
   *        <code>ResultSet::TYPE_SCROLL_SENSITIVE</code>
   * @param resultSetConcurrency a concurrency type; one of
   *        <code>ResultSet::CONCUR_READ_ONLY</code> or
   *        <code>ResultSet::CONCUR_UPDATABLE</code>
   * @return a new <code>Statement</code> object that will generate
   *         <code>ResultSet</code> objects with the given type and
   *         concurrency
   * @exception SQLException if a database access error occurs
   *         or the given parameters are not <code>ResultSet</code> 
   *         constants indicating type and concurrency
   */
  virtual Statement* createStatement(__XDBC_HOOK_DECLARE_MORE int resultSetType, int resultSetConcurrency)__XDBC_THROW_DECLARE(SQLException) = 0;
  
  /**
   *
   * Creates a <code>PreparedStatement</code> object that will generate
   * <code>ResultSet</code> objects with the given type and concurrency.
   * This method is the same as the <code>prepareStatement</code> method
   * above, but it allows the default result set
   * type and concurrency to be overridden.
   *
   * @param sql a <code>String</code> object that is the SQL statement to
   *            be sent to the database; may contain one or more ? IN
   *            parameters
   * @param resultSetType a result set type; one of 
   *         <code>ResultSet::TYPE_FORWARD_ONLY</code>, 
   *         <code>ResultSet::TYPE_SCROLL_INSENSITIVE</code>, or
   *         <code>ResultSet::TYPE_SCROLL_SENSITIVE</code>
   * @param resultSetConcurrency a concurrency type; one of
   *         <code>ResultSet::CONCUR_READ_ONLY</code> or
   *         <code>ResultSet::CONCUR_UPDATABLE</code>
   * @return a new PreparedStatement object containing the
   * pre-compiled SQL statement that will produce <code>ResultSet</code>
   * objects with the given type and concurrency
   * @exception SQLException if a database access error occurs
   *         or the given parameters are not <code>ResultSet</code> 
   *         constants indicating type and concurrency
   */
  virtual PreparedStatement* prepareStatement(__XDBC_HOOK_DECLARE_MORE const string& sql, int resultSetType, int resultSetConcurrency)__XDBC_THROW_DECLARE(SQLException)
  {
    return prepareStatement(__XDBC_HOOK_INVOKE_MORE sql,resultSetType,resultSetConcurrency,SQLImplementation::paramCountUnknown);
  }  
  virtual PreparedStatement* prepareStatement(__XDBC_HOOK_DECLARE_MORE const string& sql, int resultSetType, int resultSetConcurrency,int paramCount)__XDBC_THROW_DECLARE(SQLException) = 0;

  /**
   * Creates a <code>CallableStatement</code> object that will generate
   * <code>ResultSet</code> objects with the given type and concurrency.
   * This method is the same as the <code>prepareCall</code> method
   * above, but it allows the default result set
   * type and concurrency to be overridden.
   *
   * @param sql a <code>String</code> object that is the SQL statement to
   *            be sent to the database; may contain on or more ? parameters
   * @param resultSetType a result set type; one of 
   *         <code>ResultSet::TYPE_FORWARD_ONLY</code>, 
   *         <code>ResultSet::TYPE_SCROLL_INSENSITIVE</code>, or
   *         <code>ResultSet::TYPE_SCROLL_SENSITIVE</code>
   * @param resultSetConcurrency a concurrency type; one of
   *         <code>ResultSet::CONCUR_READ_ONLY</code> or
   *         <code>ResultSet::CONCUR_UPDATABLE</code>
   * @return a new <code>CallableStatement</code> object containing the
   * pre-compiled SQL statement that will produce <code>ResultSet</code>
   * objects with the given type and concurrency
   * @exception SQLException if a database access error occurs
   *         or the given parameters are not <code>ResultSet</code> 
   *         constants indicating type and concurrency
   */
 virtual  CallableStatement* prepareCall(__XDBC_HOOK_DECLARE_MORE const string& sql, int resultSetType, int resultSetConcurrency) __XDBC_THROW_DECLARE(SQLException)
 {
   return prepareCall(__XDBC_HOOK_INVOKE_MORE sql,resultSetType,resultSetConcurrency,SQLImplementation::paramCountUnknown);
 }
  
 virtual  CallableStatement* prepareCall(__XDBC_HOOK_DECLARE_MORE const string& sql, int resultSetType, int resultSetConcurrency,int paramCount) __XDBC_THROW_DECLARE(SQLException) = 0;
 
  /**
   *
   * Creates a <code>ArrayStatement</code> object that will generate
   * <code>ResultSet</code> objects with the given type and concurrency.
   * This method is the same as the <code>prepareArray</code> method
   * above, but it allows the default result set
   * type and concurrency to be overridden.
   *
   * @param sql a <code>String</code> object that is the SQL statement to
   *            be sent to the database; may contain one or more ? IN
   *            parameters
   * @param resultSetType a result set type; one of
   *         <code>ResultSet::TYPE_FORWARD_ONLY</code>,
   *         <code>ResultSet::TYPE_SCROLL_INSENSITIVE</code>, or
   *         <code>ResultSet::TYPE_SCROLL_SENSITIVE</code>
   * @param resultSetConcurrency a concurrency type; one of
   *         <code>ResultSet::CONCUR_READ_ONLY</code> or
   *         <code>ResultSet::CONCUR_UPDATABLE</code>
   * @return a new ArrayStatement object containing the
   * pre-compiled SQL statement that will produce <code>ResultSet</code>
   * objects with the given type and concurrency
   * @exception SQLException if a database access error occurs
   *         or the given parameters are not <code>ResultSet</code>
   *         constants indicating type and concurrency
   */
  virtual ArrayStatement* prepareArray(__XDBC_HOOK_DECLARE_MORE const string& sql, int resultSetType, int resultSetConcurrency)__XDBC_THROW_DECLARE(SQLException)
  {
    return prepareArray(__XDBC_HOOK_INVOKE_MORE sql,resultSetType,resultSetConcurrency,SQLImplementation::paramCountUnknown);
  }
  
  virtual ArrayStatement* prepareArray(__XDBC_HOOK_DECLARE_MORE const string& sql, int resultSetType, int resultSetConcurrency,int paramCount)__XDBC_THROW_DECLARE(SQLException) = 0;


//-----------------------------------------------------
//   XDBC Connection Advanced features:
//----------------------------------------------------- 
public:
  /**
   *  normalize SQL to database-independent SQL
   *
   *  @param  sql  SQL to be normalized
   *  @param  paramCount save SQL's param count
   */
  virtual string normalizeSQL(const string& sql,int& paramCount) = 0;

  /**
   * Test Retrieves whether this <code>Connection</code> object has been closed
   * 
   * @param timeout  ping timeout
   * @return <code>0</code> if this <code>Connection</code> object 
   *         is alive;otherwise if a database access error occurs
   */
  virtual int pingDatabase(int timeout)   =   0;
  
public:
//---------------------------------------------------------------------
//     XDBC Connection Constants Definitions
//---------------------------------------------------------------------
  /**
   * A constant indicating that transactions are not supported. 
   */
  const static int TRANSACTION_NONE ;
  
  /**
   * A constant indicating that
   * dirty reads, non-repeatable reads and phantom reads can occur.
   * This level allows a row changed by one transaction to be read
   * by another transaction before any changes in that row have been
   * committed (a "dirty read").  If any of the changes are rolled back, 
   * the second transaction will have retrieved an invalid row.
   */
  const static int TRANSACTION_READ_UNCOMMITTED;
  
  /**
   * A constant indicating that
   * dirty reads are prevented; non-repeatable reads and phantom
   * reads can occur.  This level only prohibits a transaction
   * from reading a row with uncommitted changes in it.
   */
  const static int TRANSACTION_READ_COMMITTED;
  
  /**
   * A constant indicating that
   * dirty reads and non-repeatable reads are prevented; phantom
   * reads can occur.  This level prohibits a transaction from
   * reading a row with uncommitted changes in it, and it also
   * prohibits the situation where one transaction reads a row,
   * a second transaction alters the row, and the first transaction
   * rereads the row, getting different values the second time
   * (a "non-repeatable read").
   */
  const static int TRANSACTION_REPEATABLE_READ;
  
  /**
   * A constant indicating that
   * dirty reads, non-repeatable reads and phantom reads are prevented.
   * This level includes the prohibitions in
   * <code>TRANSACTION_REPEATABLE_READ</code> and further prohibits the 
   * situation where one transaction reads all rows that satisfy
   * a <code>WHERE</code> condition, a second transaction inserts a row that
   * satisfies that <code>WHERE</code> condition, and the first transaction
   * rereads for the same condition, retrieving the additional
   * "phantom" row in the second read.
   */
  const static int TRANSACTION_SERIALIZABLE;
};

__OPENCODE_END_NAMESPACE

#endif

