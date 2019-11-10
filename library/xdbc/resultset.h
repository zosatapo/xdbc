/**
 *  resultset.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef   __OPENCODE_XDBC_RESULTSET_H__
#define   __OPENCODE_XDBC_RESULTSET_H__

#include "xdbc/xdefine.h"
#include "xdbc/clob.h"
#include "xdbc/blob.h"

__OPENCODE_BEGIN_NAMESPACE

class __OPENCODE_XDBC_API_DECL ResultSetCollection
{
public:
  virtual ~ResultSetCollection() {}
};


/**
 * The constant indicating the type for a <code>ResultSet</code> object
 * whose cursor may move only forward.
 */
#define  RESULTSET_TYPE_FORWARD_ONLY       1003

/**
 * The constant indicating the type for a <code>ResultSet</code> object
 * that is scrollable but generally not sensitive to changes made by others.
 */
#define  RESULTSET_TYPE_SCROLL_INSENSITIVE  1004

/**
 * The constant indicating the type for a <code>ResultSet</code> object
 * that is scrollable and generally sensitive to changes made by others.
 */
#define  RESULTSET_TYPE_SCROLL_SENSITIVE    1005
  
/**
 * The constant indicating the concurrency mode for a
 * <code>ResultSet</code> object that may NOT be updated.
 */
#define  RESULTSET_CONCUR_READ_ONLY         1007

/**
 * The constant indicating the concurrency mode for a
 * <code>ResultSet</code> object that may be updated.
 */
#define  RESULTSET_CONCUR_UPDATABLE         1008

  
class __OPENCODE_XDBC_API_DECL ResultSet : public  SQLManageable
{
protected:
  virtual ~ResultSet(){}

public:
  /**
   * Releases this <code>ResultSet</code> object's database and
   * XDBC resources immediately instead of waiting for
   * this to happen when it is automatically closed.
   *
   */
  virtual void close() = 0;

  /** Destroy this <code>ResultSet</code> object */  
  virtual void destroy() = 0;

public:
  /**
   * Moves the cursor down one row from its current position.
   * A <code>ResultSet</code> cursor is initially positioned
   * before the first row; the first call to the method
   * <code>next</code> makes the first row the current row; the
   * second call makes the second row the current row, and so on. 
   *
   * <P>If an input stream is open for the current row, a call
   * to the method <code>next</code> will
   * implicitly close it. A <code>ResultSet</code> object's
   * warning chain is cleared when a new row is read.
   *
   * @return <code>true</code> if the new current row is valid; 
   * <code>false</code> if there are no more rows 
   * @exception SQLException if a database access error occurs
   */
  virtual bool next(__XDBC_HOOK_DECLARE_ONCE ) __XDBC_THROW_DECLARE(SQLException) = 0;
  
  /**
   * Reports whether
   * the  column  had a value of SQL <code>NULL</code>.
   * @return <code>true</code> if the columnIndex column value read was SQL
   *         <code>NULL</code> and <code>false</code> otherwise
   * @exception SQLException if a database access error occurs
   */
  virtual bool wasNull(__XDBC_HOOK_DECLARE_MORE int columnIndex) __XDBC_THROW_DECLARE(SQLException) = 0;
  
  //======================================================================
  // Methods for accessing results 
  //======================================================================
    
  /**
   * Retrieves the value of the designated column in the current row
   * of this <code>ResultSet</code> object as
   * a <code>byte</code>  in the XDBC .
   *
   * @param columnIndex the first column is 1, the second is 2, ...
   * @return the column value; if the value is SQL <code>NULL</code>, the
   * value returned is <code>0</code>
   * @exception SQLException if a database access error occurs
   */
  virtual char getByte(__XDBC_HOOK_DECLARE_MORE int columnIndex) __XDBC_THROW_DECLARE(SQLException)
  {
    char colval = 0;
    getByte(__XDBC_HOOK_INVOKE_MORE columnIndex,colval);
    return colval;
  }

  /**
   * Retrieves the value of the designated column in the current row
   * of this <code>ResultSet</code> object as
   * a <code>byte</code>  in the XDBC .
   *
   * @param columnIndex the first column is 1, the second is 2, ...
   * @param colval      the column value; if the value is SQL <code>NULL</code>, the
   * value returned is <code>0</code>
   *
   * @return <code>false</code> if the columnIndex column value read was SQL
   *         <code>NULL</code> and <code>true</code> otherwise
   * @exception SQLException if a database access error occurs
   */    
  virtual bool getByte(__XDBC_HOOK_DECLARE_MORE int columnIndex,char& colval) __XDBC_THROW_DECLARE(SQLException) = 0;
  
  /**
   * Retrieves the value of the designated column in the current row
   * of this <code>ResultSet</code> object as
   * a <code>short</code>  in the XDBC .
   *
   * @param columnIndex the first column is 1, the second is 2, ...
   * @return the column value; if the value is SQL <code>NULL</code>, the
   * value returned is <code>0</code>
   * @exception SQLException if a database access error occurs
   */
  virtual short getShort(__XDBC_HOOK_DECLARE_MORE int columnIndex) __XDBC_THROW_DECLARE(SQLException)
  {
    short colval = 0;
    getShort(__XDBC_HOOK_INVOKE_MORE columnIndex,colval);
    return colval;
  }

  /**
   * Retrieves the value of the designated column in the current row
   * of this <code>ResultSet</code> object as
   * a <code>short</code>  in the XDBC .
   *
   * @param columnIndex the first column is 1, the second is 2, ...
   * @param colval      the column value; if the value is SQL <code>NULL</code>, the
   * value returned is <code>0</code>
   *
   * @return <code>false</code> if the columnIndex column value read was SQL
   *         <code>NULL</code> and <code>true</code> otherwise
   * @exception SQLException if a database access error occurs
   */    
  virtual bool  getShort(__XDBC_HOOK_DECLARE_MORE int columnIndex,short& colval) __XDBC_THROW_DECLARE(SQLException) = 0;
  
  /**
   * Retrieves the value of the designated column in the current row
   * of this <code>ResultSet</code> object as
   * an <code>int</code>  in the XDBC .
   *
   * @param columnIndex the first column is 1, the second is 2, ...
   * @return the column value; if the value is SQL <code>NULL</code>, the
   * value returned is <code>0</code>
   * @exception SQLException if a database access error occurs
   */
  virtual int  getInt(__XDBC_HOOK_DECLARE_MORE int columnIndex) __XDBC_THROW_DECLARE(SQLException)
  {
    int colval = 0;
    getInt(__XDBC_HOOK_INVOKE_MORE columnIndex,colval);
    return colval;
  }

  /**
   * Retrieves the value of the designated column in the current row
   * of this <code>ResultSet</code> object as
   * an <code>int</code>  in the XDBC .
   *
   * @param columnIndex the first column is 1, the second is 2, ...
   * @param colval      the column value; if the value is SQL <code>NULL</code>, the
   * value returned is <code>0</code>
   *
   * @return <code>false</code> if the columnIndex column value read was SQL
   *         <code>NULL</code> and <code>true</code> otherwise
   * @exception SQLException if a database access error occurs
   */    
  virtual bool getInt(__XDBC_HOOK_DECLARE_MORE int columnIndex,int& colval) __XDBC_THROW_DECLARE(SQLException) = 0;
  
  /**
   * Retrieves the value of the designated column in the current row
   * of this <code>ResultSet</code> object as
   * a <code>XLONG</code> in the XDBC.
   *
   * @param columnIndex the first column is 1, the second is 2, ...
   * @return the column value; if the value is SQL <code>NULL</code>, the
   * value returned is <code>0</code>
   * @exception SQLException if a database access error occurs
   */
  virtual XLONG getLong(__XDBC_HOOK_DECLARE_MORE int columnIndex) __XDBC_THROW_DECLARE(SQLException)
  {
    XLONG colval = 0;
    getLong(__XDBC_HOOK_INVOKE_MORE columnIndex,colval);
    return colval;
  }

  /**
   * Retrieves the value of the designated column in the current row
   * of this <code>ResultSet</code> object as
   * a <code>XLONG</code> in the XDBC.
   *
   * @param columnIndex the first column is 1, the second is 2, ...
   * @param colval      the column value; if the value is SQL <code>NULL</code>, the
   * value returned is <code>0</code>
   *
   * @return <code>false</code> if the columnIndex column value read was SQL
   *         <code>NULL</code> and <code>true</code> otherwise
   * @exception SQLException if a database access error occurs
   */    
  virtual bool      getLong(__XDBC_HOOK_DECLARE_MORE int columnIndex,XLONG& colval) __XDBC_THROW_DECLARE(SQLException) = 0;
  
  /**
   * Retrieves the value of the designated column in the current row
   * of this <code>ResultSet</code> object as
   * a <code>float</code>  in the XDBC .
   *
   * @param columnIndex the first column is 1, the second is 2, ...
   * @return the column value; if the value is SQL <code>NULL</code>, the
   * value returned is <code>0</code>
   * @exception SQLException if a database access error occurs
   */
  virtual float getFloat(__XDBC_HOOK_DECLARE_MORE int columnIndex) __XDBC_THROW_DECLARE(SQLException)
  {
    float colval = 0;
    getFloat(__XDBC_HOOK_INVOKE_MORE columnIndex,colval);
    return colval;
  }

  /**
   * Retrieves the value of the designated column in the current row
   * of this <code>ResultSet</code> object as
   * a <code>float</code>  in the XDBC .
   *
   * @param columnIndex the first column is 1, the second is 2, ...
   * @param colval      the column value; if the value is SQL <code>NULL</code>, the
   * value returned is <code>0</code>
   *
   * @return <code>false</code> if the columnIndex column value read was SQL
   *         <code>NULL</code> and <code>true</code> otherwise
   * @exception SQLException if a database access error occurs
   */  
  virtual bool  getFloat(__XDBC_HOOK_DECLARE_MORE int columnIndex,float& colval) __XDBC_THROW_DECLARE(SQLException) = 0;
  
  /**
   * Retrieves the value of the designated column in the current row
   * of this <code>ResultSet</code> object as
   * a <code>double</code> XDBC .
   *
   * @param columnIndex the first column is 1, the second is 2, ...
   * @return the column value; if the value is SQL <code>NULL</code>, the
   * value returned is <code>0</code>
   * @exception SQLException if a database access error occurs
   */
  virtual double getDouble(__XDBC_HOOK_DECLARE_MORE int columnIndex) __XDBC_THROW_DECLARE(SQLException)
  {
    double colval = 0;
    getDouble(__XDBC_HOOK_INVOKE_MORE columnIndex,colval);
    return colval;
  }

  /**
   * Retrieves the value of the designated column in the current row
   * of this <code>ResultSet</code> object as
   * a <code>double</code>  in the XDBC .
   *
   * @param columnIndex the first column is 1, the second is 2, ...
   * @param colval      the column value; if the value is SQL <code>NULL</code>, the
   * value returned is <code>0</code>
   *
   * @return <code>false</code> if the columnIndex column value read was SQL
   *         <code>NULL</code> and <code>true</code> otherwise
   * @exception SQLException if a database access error occurs
   */  
  virtual bool   getDouble(__XDBC_HOOK_DECLARE_MORE int columnIndex,double& colval) __XDBC_THROW_DECLARE(SQLException) = 0;
  
  /**
   * Retrieves the value of the designated column in the current row
   * of this <code>ResultSet</code> object as
   * an <code>XDBC_DATE</code> object XDBC .
   *
   * @param columnIndex the first column is 1, the second is 2, ...
   * @return the column value; if the value is SQL <code>NULL</code>, the
   * value returned is <code>null</code>
   * @exception SQLException if a database access error occurs
   */
  virtual XDBC_DATE getDate(__XDBC_HOOK_DECLARE_MORE int columnIndex) __XDBC_THROW_DECLARE(SQLException)
  {
    XDBC_DATE colval;
    getDate(__XDBC_HOOK_INVOKE_MORE columnIndex,colval);
    return colval;
  }

  /**
   * Retrieves the value of the designated column in the current row
   * of this <code>ResultSet</code> object as
   * an <code>XDBC_DATE</code> object  in the XDBC .
   *
   * @param columnIndex the first column is 1, the second is 2, ...
   * @param colval      the column value; if the value is SQL <code>NULL</code>, the
   * value returned is <code>0</code>
   *
   * @return <code>false</code> if the columnIndex column value read was SQL
   *         <code>NULL</code> and <code>true</code> otherwise
   * @exception SQLException if a database access error occurs
   */  
  virtual bool    getDate(__XDBC_HOOK_DECLARE_MORE int columnIndex,XDBC_DATE& colval) __XDBC_THROW_DECLARE(SQLException) = 0;
  
  /**
   * Retrieves the value of the designated column in the current row
   * of this <code>ResultSet</code> object as
   * an <code>XDBC_TIME</code> object  in the XDBC .
   *
   * @param columnIndex the first column is 1, the second is 2, ...
   * @return the column value; if the value is SQL <code>NULL</code>, the
   * value returned is <code>null</code>
   * @exception SQLException if a database access error occurs
   */
  virtual XDBC_TIME getTime(__XDBC_HOOK_DECLARE_MORE int columnIndex) __XDBC_THROW_DECLARE(SQLException)
  {
    XDBC_TIME colval;
    getTime(__XDBC_HOOK_INVOKE_MORE columnIndex,colval);
    return colval;
  }

  /**
   * Retrieves the value of the designated column in the current row
   * of this <code>ResultSet</code> object as
   * an <code>XDBC_TIME</code> object  in the XDBC .
   *
   * @param columnIndex the first column is 1, the second is 2, ...
   * @param colval      the column value; if the value is SQL <code>NULL</code>, the
   * value returned is <code>0</code>
   *
   * @return <code>false</code> if the columnIndex column value read was SQL
   *         <code>NULL</code> and <code>true</code> otherwise
   * @exception SQLException if a database access error occurs
   */    
  virtual bool    getTime(__XDBC_HOOK_DECLARE_MORE int columnIndex,XDBC_TIME& colval) __XDBC_THROW_DECLARE(SQLException) = 0;
  
  /**
   * Retrieves the value of the designated column in the current row
   * of this <code>ResultSet</code> object as
   * an <code>XDBC_TIMESTAMP</code> object in the XDBC .
   *
   * @param columnIndex the first column is 1, the second is 2, ...
   * @return the column value; if the value is SQL <code>NULL</code>, the
   * value returned is <code>null</code>
   * @exception SQLException if a database access error occurs
   */
  virtual XDBC_TIMESTAMP getTimestamp(__XDBC_HOOK_DECLARE_MORE int columnIndex) __XDBC_THROW_DECLARE(SQLException)
  {
    XDBC_TIMESTAMP colval;
    getTimestamp(__XDBC_HOOK_INVOKE_MORE columnIndex,colval);
    return colval;
  }

  /**
   * Retrieves the value of the designated column in the current row
   * of this <code>ResultSet</code> object as
   * an <code>XDBC_TIMESTAMP</code> object  in the XDBC .
   *
   * @param columnIndex the first column is 1, the second is 2, ...
   * @param colval      the column value; if the value is SQL <code>NULL</code>, the
   * value returned is <code>0</code>
   *
   * @return <code>false</code> if the columnIndex column value read was SQL
   *         <code>NULL</code> and <code>true</code> otherwise
   * @exception SQLException if a database access error occurs
   */    
  virtual bool         getTimestamp(__XDBC_HOOK_DECLARE_MORE int columnIndex,XDBC_TIMESTAMP& colval) __XDBC_THROW_DECLARE(SQLException) = 0;
  
  /**
   * Retrieves the value of the designated column in the current row
   * of this <code>ResultSet</code> object as
   * an <code>XDBC_INTERVAL</code> object  in the XDBC .
   *
   * @param columnIndex the first column is 1, the second is 2, ...
   * @return the column value; if the value is SQL <code>NULL</code>, the
   * value returned is <code>null</code>
   * @exception SQLException if a database access error occurs
   */
  virtual XDBC_INTERVAL getInterval(__XDBC_HOOK_DECLARE_MORE int columnIndex) __XDBC_THROW_DECLARE(SQLException)
  {
    XDBC_INTERVAL colval;
    getInterval(__XDBC_HOOK_INVOKE_MORE columnIndex,colval);
    return colval;
  }

  /**
   * Retrieves the value of the designated column in the current row
   * of this <code>ResultSet</code> object as
   * an <code>XDBC_INTERVAL</code> object  in the XDBC .
   *
   * @param columnIndex the first column is 1, the second is 2, ...
   * @param colval      the column value; if the value is SQL <code>NULL</code>, the
   * value returned is <code>0</code>
   *
   * @return <code>false</code> if the columnIndex column value read was SQL
   *         <code>NULL</code> and <code>true</code> otherwise
   * @exception SQLException if a database access error occurs
   */  
  virtual bool  getInterval(__XDBC_HOOK_DECLARE_MORE int columnIndex,XDBC_INTERVAL& colval) __XDBC_THROW_DECLARE(SQLException) = 0;


  /**
   * Retrieves the value of the designated column in the current row
   * of this <code>ResultSet</code> object as
   * a <code>String</code>  in the XDBC .
   *
   * @param columnIndex the first column is 1, the second is 2, ...
   * @param sizeBytes   buffer size in bytes of the column value
   *
   * @return the column value; if the value is SQL <code>NULL</code>, the
   * value returned is <code>null pointer</code> and <code>buffer pointer</code> otherwise
   * @exception SQLException if a database access error occurs
   */
  virtual const char* getString(__XDBC_HOOK_DECLARE_MORE int columnIndex,int& sizeBytes) __XDBC_THROW_DECLARE(SQLException)  = 0;
    
  /**
   * Maps the given <code>ResultSet</code> column name to its
   * <code>ResultSet</code> column index.
   *
   * @param columnName the name of the column
   * @return the column index of the given column name
   * @exception SQLException if the <code>ResultSet</code> object
   * does not contain <code>columnName</code> or a database access error occurs
   */
  virtual int findColumn(__XDBC_HOOK_DECLARE_MORE const string& columnName) __XDBC_THROW_DECLARE(SQLException) = 0;
  
  /**
   * Retrieves the value of the designated column in the current row
   * of this <code>ResultSet</code> object as
   * a <code>byte</code>  in the XDBC .
   *
   * @param columnName the SQL name of the column
   * @return the column value; if the value is SQL <code>NULL</code>, the
   * value returned is <code>0</code>
   * @exception SQLException if a database access error occurs
   */
  virtual char getByte(__XDBC_HOOK_DECLARE_MORE const string& columnName) __XDBC_THROW_DECLARE(SQLException)
  {
    char colval = 0;
    getByte(__XDBC_HOOK_INVOKE_MORE findColumn(__XDBC_HOOK_INVOKE_MORE columnName),colval);
    return colval;     
  }

  /**
   * Retrieves the value of the designated column in the current row
   * of this <code>ResultSet</code> object as
   * a <code>byte</code>  in the XDBC .
   *
   * @param columnName the SQL name of the column
   * @param colval      the column value; if the value is SQL <code>NULL</code>, the
   * value returned is <code>0</code>
   *
   * @return <code>false</code> if the columnName column value read was SQL
   *         <code>NULL</code> and <code>true</code> otherwise
   * @exception SQLException if a database access error occurs
   */  
  virtual bool getByte(__XDBC_HOOK_DECLARE_MORE const string& columnName,char& colval) __XDBC_THROW_DECLARE(SQLException)
  {
    return getByte(__XDBC_HOOK_INVOKE_MORE findColumn(__XDBC_HOOK_INVOKE_MORE columnName),colval);  
  }
  
  /**
   * Retrieves the value of the designated column in the current row
   * of this <code>ResultSet</code> object as
   * a <code>short</code>  in the XDBC .
   *
   * @param columnName the SQL name of the column
   * @return the column value; if the value is SQL <code>NULL</code>, the
   * value returned is <code>0</code>
   * @exception SQLException if a database access error occurs
   */
  virtual short getShort(__XDBC_HOOK_DECLARE_MORE const string& columnName) __XDBC_THROW_DECLARE(SQLException)
  {
    short colval = 0;
    getShort(__XDBC_HOOK_INVOKE_MORE findColumn(__XDBC_HOOK_INVOKE_MORE columnName),colval);
    return colval; 
  }

  /**
   * Retrieves the value of the designated column in the current row
   * of this <code>ResultSet</code> object as
   * a <code>short</code>  in the XDBC .
   *
   * @param columnName the SQL name of the column
   * @param colval      the column value; if the value is SQL <code>NULL</code>, the
   * value returned is <code>0</code>
   *
   * @return <code>false</code> if the columnName column value read was SQL
   *         <code>NULL</code> and <code>true</code> otherwise
   * @exception SQLException if a database access error occurs
   */     
  virtual bool  getShort(__XDBC_HOOK_DECLARE_MORE const string& columnName,short& colval) __XDBC_THROW_DECLARE(SQLException)
  {
    return getShort(__XDBC_HOOK_INVOKE_MORE findColumn(__XDBC_HOOK_INVOKE_MORE columnName),colval);  
  }
  
  /**
   * Retrieves the value of the designated column in the current row
   * of this <code>ResultSet</code> object as
   * an <code>int</code>  in the XDBC .
   *
   * @param columnName the SQL name of the column
   * @return the column value; if the value is SQL <code>NULL</code>, the
   * value returned is <code>0</code>
   * @exception SQLException if a database access error occurs
   */
  virtual int  getInt(__XDBC_HOOK_DECLARE_MORE const string& columnName) __XDBC_THROW_DECLARE(SQLException)
  {
    int colval = 0;
    getInt(__XDBC_HOOK_INVOKE_MORE findColumn(__XDBC_HOOK_INVOKE_MORE columnName),colval);
    return colval;       
  }

  /**
   * Retrieves the value of the designated column in the current row
   * of this <code>ResultSet</code> object as
   * an <code>int</code>  in the XDBC .
   *
   * @param columnName the SQL name of the column
   * @param colval      the column value; if the value is SQL <code>NULL</code>, the
   * value returned is <code>0</code>
   *
   * @return <code>false</code> if the columnName column value read was SQL
   *         <code>NULL</code> and <code>true</code> otherwise
   * @exception SQLException if a database access error occurs
   */     
  virtual bool getInt(__XDBC_HOOK_DECLARE_MORE const string& columnName,int& colval) __XDBC_THROW_DECLARE(SQLException)
  {
    return getInt(__XDBC_HOOK_INVOKE_MORE findColumn(__XDBC_HOOK_INVOKE_MORE columnName),colval);  
  }
  
  /**
   * Retrieves the value of the designated column in the current row
   * of this <code>ResultSet</code> object as
   * a <code>XLONG</code> in the XDBC.
   *
   * @param columnName the SQL name of the column
   * @return the column value; if the value is SQL <code>NULL</code>, the
   * value returned is <code>0</code>
   * @exception SQLException if a database access error occurs
   */
  virtual XLONG getLong(__XDBC_HOOK_DECLARE_MORE const string& columnName) __XDBC_THROW_DECLARE(SQLException)
  {
    XLONG colval = 0;
    getLong(__XDBC_HOOK_INVOKE_MORE findColumn(__XDBC_HOOK_INVOKE_MORE columnName),colval);
    return colval;     
  }
 
  /**
   * Retrieves the value of the designated column in the current row
   * of this <code>ResultSet</code> object as
   * a <code>XLONG</code> in the XDBC.
   *
   * @param columnName the SQL name of the column
   * @param colval      the column value; if the value is SQL <code>NULL</code>, the
   * value returned is <code>0</code>
   *
   * @return <code>false</code> if the columnName column value read was SQL
   *         <code>NULL</code> and <code>true</code> otherwise
   * @exception SQLException if a database access error occurs
   */ 
  virtual bool      getLong(__XDBC_HOOK_DECLARE_MORE const string& columnName,XLONG& colval) __XDBC_THROW_DECLARE(SQLException)
  {
    return getLong(__XDBC_HOOK_INVOKE_MORE findColumn(__XDBC_HOOK_INVOKE_MORE columnName),colval);  
  }
  
  /**
   * Retrieves the value of the designated column in the current row
   * of this <code>ResultSet</code> object as
   * a <code>float</code>  in the XDBC .
   *
   * @param columnName the SQL name of the column
   * @return the column value; if the value is SQL <code>NULL</code>, the
   * value returned is <code>0</code>
   * @exception SQLException if a database access error occurs
   */
  virtual float getFloat(__XDBC_HOOK_DECLARE_MORE const string& columnName) __XDBC_THROW_DECLARE(SQLException)
  {
    float colval = 0;
    getFloat(__XDBC_HOOK_INVOKE_MORE findColumn(__XDBC_HOOK_INVOKE_MORE columnName),colval);
    return colval;  
  }

  /**
   * Retrieves the value of the designated column in the current row
   * of this <code>ResultSet</code> object as
   * a <code>float</code>  in the XDBC .
   *
   * @param columnName the SQL name of the column
   * @param colval      the column value; if the value is SQL <code>NULL</code>, the
   * value returned is <code>0</code>
   *
   * @return <code>false</code> if the columnName column value read was SQL
   *         <code>NULL</code> and <code>true</code> otherwise
   * @exception SQLException if a database access error occurs
   */    
  virtual bool  getFloat(__XDBC_HOOK_DECLARE_MORE const string& columnName,float& colval) __XDBC_THROW_DECLARE(SQLException)
  {
    return getFloat(__XDBC_HOOK_INVOKE_MORE findColumn(__XDBC_HOOK_INVOKE_MORE columnName),colval);  
  }
  
  /**
   * Retrieves the value of the designated column in the current row
   * of this <code>ResultSet</code> object as
   * a <code>double</code> XDBC .
   *
   * @param columnName the SQL name of the column
   * @return the column value; if the value is SQL <code>NULL</code>, the
   * value returned is <code>0</code>
   * @exception SQLException if a database access error occurs
   */
  virtual double getDouble(__XDBC_HOOK_DECLARE_MORE const string& columnName) __XDBC_THROW_DECLARE(SQLException)
  {
    double colval = 0;
    getDouble(__XDBC_HOOK_INVOKE_MORE findColumn(__XDBC_HOOK_INVOKE_MORE columnName),colval);
    return colval;     
  }

  /**
   * Retrieves the value of the designated column in the current row
   * of this <code>ResultSet</code> object as
   * a <code>double</code>  in the XDBC .
   *
   * @param columnName the SQL name of the column
   * @param colval      the column value; if the value is SQL <code>NULL</code>, the
   * value returned is <code>0</code>
   *
   * @return <code>false</code> if the columnName column value read was SQL
   *         <code>NULL</code> and <code>true</code> otherwise
   * @exception SQLException if a database access error occurs
   */    
  virtual bool   getDouble(__XDBC_HOOK_DECLARE_MORE const string& columnName,double& colval) __XDBC_THROW_DECLARE(SQLException)
  {
    return getDouble(__XDBC_HOOK_INVOKE_MORE findColumn(__XDBC_HOOK_INVOKE_MORE columnName),colval);  
  }
  
  /**
   * Retrieves the value of the designated column in the current row
   * of this <code>ResultSet</code> object as
   * a <code>XDBC_DATE</code> object XDBC .
   *
   * @param columnName the SQL name of the column
   * @return the column value; if the value is SQL <code>NULL</code>, the
   * value returned is <code>null</code>
   * @exception SQLException if a database access error occurs
   */
  virtual XDBC_DATE getDate(__XDBC_HOOK_DECLARE_MORE const string& columnName) __XDBC_THROW_DECLARE(SQLException)
  {
    XDBC_DATE colval;
    getDate(__XDBC_HOOK_INVOKE_MORE findColumn(__XDBC_HOOK_INVOKE_MORE columnName),colval);
    return colval;      
  }

  /**
   * Retrieves the value of the designated column in the current row
   * of this <code>ResultSet</code> object as
   * an <code>XDBC_DATE</code> object  in the XDBC .
   *
   * @param columnName the SQL name of the column
   * @param colval      the column value; if the value is SQL <code>NULL</code>, the
   * value returned is <code>0</code>
   *
   * @return <code>false</code> if the columnName column value read was SQL
   *         <code>NULL</code> and <code>true</code> otherwise
   * @exception SQLException if a database access error occurs
   */  
  virtual bool    getDate(__XDBC_HOOK_DECLARE_MORE const string& columnName,XDBC_DATE& colval) __XDBC_THROW_DECLARE(SQLException)
  {
    return getDate(__XDBC_HOOK_INVOKE_MORE findColumn(__XDBC_HOOK_INVOKE_MORE columnName),colval);    
  }
  
  /**
   * Retrieves the value of the designated column in the current row
   * of this <code>ResultSet</code> object as
   * an <code>XDBC_TIME</code> object  in the XDBC .
   *
   * @param columnName the SQL name of the column
   * @return the column value; if the value is SQL <code>NULL</code>, the
   * value returned is <code>null</code>
   * @exception SQLException if a database access error occurs
   */
  virtual XDBC_TIME getTime(__XDBC_HOOK_DECLARE_MORE const string& columnName) __XDBC_THROW_DECLARE(SQLException)
  {
    XDBC_TIME colval;
    getTime(__XDBC_HOOK_INVOKE_MORE findColumn(__XDBC_HOOK_INVOKE_MORE columnName),colval);
    return colval;    
  }

  /**
   * Retrieves the value of the designated column in the current row
   * of this <code>ResultSet</code> object as
   * an <code>XDBC_TIME</code> object  in the XDBC .
   *
   * @param columnName the SQL name of the column
   * @param colval      the column value; if the value is SQL <code>NULL</code>, the
   * value returned is <code>0</code>
   *
   * @return <code>false</code> if the columnName column value read was SQL
   *         <code>NULL</code> and <code>true</code> otherwise
   * @exception SQLException if a database access error occurs
   */    
  virtual bool    getTime(__XDBC_HOOK_DECLARE_MORE const string& columnName,XDBC_TIME& colval) __XDBC_THROW_DECLARE(SQLException)
  {
    return getTime(__XDBC_HOOK_INVOKE_MORE findColumn(__XDBC_HOOK_INVOKE_MORE columnName),colval);
  }
  
  /**
   * Retrieves the value of the designated column in the current row
   * of this <code>ResultSet</code> object as
   * an <code>XDBC_TIMESTAMP</code> object in the XDBC .
   *
   * @param columnName the SQL name of the column
   * @return the column value; if the value is SQL <code>NULL</code>, the
   * value returned is <code>null</code>
   * @exception SQLException if a database access error occurs
   */
  virtual XDBC_TIMESTAMP getTimestamp(__XDBC_HOOK_DECLARE_MORE const string& columnName) __XDBC_THROW_DECLARE(SQLException)
  {
    XDBC_TIMESTAMP colval;
    getTimestamp(__XDBC_HOOK_INVOKE_MORE findColumn(__XDBC_HOOK_INVOKE_MORE columnName),colval);
    return colval;    
  }

  /**
   * Retrieves the value of the designated column in the current row
   * of this <code>ResultSet</code> object as
   * an <code>XDBC_TIMESTAMP</code> object  in the XDBC .
   *
   * @param columnName the SQL name of the column
   * @param colval      the column value; if the value is SQL <code>NULL</code>, the
   * value returned is <code>0</code>
   *
   * @return <code>false</code> if the columnName column value read was SQL
   *         <code>NULL</code> and <code>true</code> otherwise
   * @exception SQLException if a database access error occurs
   */    
  virtual bool         getTimestamp(__XDBC_HOOK_DECLARE_MORE const string& columnName,XDBC_TIMESTAMP& colval) __XDBC_THROW_DECLARE(SQLException)
  {
    return getTimestamp(__XDBC_HOOK_INVOKE_MORE findColumn(__XDBC_HOOK_INVOKE_MORE columnName),colval);
  }
  
  /**
   * Retrieves the value of the designated column in the current row
   * of this <code>ResultSet</code> object as
   * an <code>XDBC_INTERVAL</code> object  in the XDBC .
   *
   * @param columnName the SQL name of the column
   * @return the column value; if the value is SQL <code>NULL</code>, the
   * value returned is <code>null</code>
   * @exception SQLException if a database access error occurs
   */
  virtual XDBC_INTERVAL getInterval(__XDBC_HOOK_DECLARE_MORE const string& columnName) __XDBC_THROW_DECLARE(SQLException)
  {
    XDBC_INTERVAL colval;
    getInterval(__XDBC_HOOK_INVOKE_MORE findColumn(__XDBC_HOOK_INVOKE_MORE columnName),colval);
    return colval;
  }

  /**
   * Retrieves the value of the designated column in the current row
   * of this <code>ResultSet</code> object as
   * an <code>XDBC_INTERVAL</code> object  in the XDBC .
   *
   * @param columnName the SQL name of the column
   * @param colval      the column value; if the value is SQL <code>NULL</code>, the
   * value returned is <code>0</code>
   *
   * @return <code>false</code> if the columnName column value read was SQL
   *         <code>NULL</code> and <code>true</code> otherwise
   * @exception SQLException if a database access error occurs
   */     
  virtual bool     getInterval(__XDBC_HOOK_DECLARE_MORE const string& columnName,XDBC_INTERVAL& colval) __XDBC_THROW_DECLARE(SQLException) 
  {
    return getInterval(__XDBC_HOOK_INVOKE_MORE findColumn(__XDBC_HOOK_INVOKE_MORE columnName),colval);
  }
 
  /**
   * Retrieves the value of the designated column in the current row
   * of this <code>ResultSet</code> object as
   * a <code>String</code>  in the XDBC .
   *
   * @param columnName the SQL name of the column
   * @param sizeBytes   buffer size in bytes of the column value
   *
   * @return the column value; if the value is SQL <code>NULL</code>, the
   * value returned is <code>null pointer</code> and <code>buffer pointer</code> otherwise
   * @exception SQLException if a database access error occurs
   */
  virtual  const char* getString(__XDBC_HOOK_DECLARE_MORE const string& columnName,int& sizeBytes) __XDBC_THROW_DECLARE(SQLException)
  {
    return getString(__XDBC_HOOK_INVOKE_MORE findColumn(__XDBC_HOOK_INVOKE_MORE columnName),sizeBytes);
  }

//=====================================================================
// XDBC Advanced features
//=====================================================================

public:
  /**
   * Retrieves the value of the designated column in the current row
   * of this <code>ResultSet</code> object as a <code>Blob</code> object
   * in the XDBC.
   *
   * @param i the first column is 1, the second is 2, ...
   * @return a <code>Blob</code> object representing the SQL
   *         <code>BLOB</code> value in the specified column
   * @exception SQLException if a database access error occurs
   */
  virtual Blob* getBlob(__XDBC_HOOK_DECLARE_MORE int columnIndex) __XDBC_THROW_DECLARE(SQLException) = 0;

  /**
   * Retrieves the value of the designated column in the current row
   * of this <code>ResultSet</code> object as a <code>Clob</code> object
   * in the XDBC.
   *
   * @param i the first column is 1, the second is 2, ...
   * @return a <code>Clob</code> object representing the SQL
   *         <code>CLOB</code> value in the specified column
   * @exception SQLException if a database access error occurs
   */
  virtual Clob* getClob(__XDBC_HOOK_DECLARE_MORE int columnIndex) __XDBC_THROW_DECLARE(SQLException) = 0;
    
  /**
   * Retrieves the value of the designated column in the current row
   * of this <code>ResultSet</code> object as a <code>Blob</code> object
   * in the XDBC.
   *
   * @param columnName the name of the column from which to retrieve the value
   * @return a <code>Blob</code> object representing the SQL <code>BLOB</code> 
   *         value in the specified column
   * @exception SQLException if a database access error occurs
   */
  virtual Blob* getBlob(__XDBC_HOOK_DECLARE_MORE const string& columnName) __XDBC_THROW_DECLARE(SQLException)
  {
    return getBlob(__XDBC_HOOK_INVOKE_MORE findColumn(__XDBC_HOOK_INVOKE_MORE columnName));
  }
  
  /**
   * Retrieves the value of the designated column in the current row
   * of this <code>ResultSet</code> object as a <code>Clob</code> object
   * in the XDBC.
   *
   * @param columnName the name of the column from which to retrieve the value
   * @return a <code>Clob</code> object representing the SQL <code>CLOB</code>
   * value in the specified column
   * @exception SQLException if a database access error occurs
   */
  virtual Clob* getClob(__XDBC_HOOK_DECLARE_MORE const string& columnName) __XDBC_THROW_DECLARE(SQLException)
  {
   return getClob(__XDBC_HOOK_INVOKE_MORE findColumn(__XDBC_HOOK_INVOKE_MORE columnName));
  }
  
  /**
   * Retrieves the type of this <code>ResultSet</code> object.  
   * The type is determined by the <code>Statement</code> object
   * that created the result set.
   *
   * @return <code>ResultSet::TYPE_FORWARD_ONLY</code>,
   *         <code>ResultSet::TYPE_SCROLL_INSENSITIVE</code>,
   *         or <code>ResultSet::TYPE_SCROLL_SENSITIVE</code>
   * @exception SQLException if a database access error occurs
   */
  virtual int getType(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException) = 0;
  
  /**
   * Retrieves the concurrency mode of this <code>ResultSet</code> object.
   * The concurrency used is determined by the 
   * <code>Statement</code> object that created the result set.
   *
   * @return the concurrency type, either
   *         <code>ResultSet::CONCUR_READ_ONLY</code>
   *         or <code>ResultSet::CONCUR_UPDATABLE</code>
   * @exception SQLException if a database access error occurs
   */
  virtual int getConcurrency(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException) = 0;

  /**
   * Retrieves the  number, types and properties of
   * this <code>ResultSet</code> object's columns.
   *
   * @return the description of this <code>ResultSet</code> object's columns
   * @exception SQLException if a database access error occurs
   */
  virtual ResultSetMetaData* getMetaData(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException) = 0;

  /**
   * Retrieves the <code>SQLImplementation</code> object
   * that produced this <code>ResultSet</code> object.
   * @return the statement that produced this resultset
   * @exception SQLException if a database access error occurs
   */
  virtual SQLImplementation* getImplementation(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException) = 0;

//---------------------------------------------------------------------
// Traversal/Positioning
//---------------------------------------------------------------------
public:

  /**
   * Retrieves the current row number.  The first row is number 1, the
   * second number 2, and so on.
   *
   * @return the current row number; <code>0</code> if there is no current row
   * @exception SQLException if a database access error occurs
   */
  virtual int getRow(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException) = 0;
  
  /**
   * Retrieves whether the cursor is before the first row in 
   * this <code>ResultSet</code> object.
   *
   * @return <code>true</code> if the cursor is before the first row;
   * <code>false</code> if the cursor is at any rv position or the
   * result set contains no rows
   * @exception SQLException if a database access error occurs
   */
  virtual bool isBeforeFirst(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException) = 0;
    
  /**
   * Retrieves whether the cursor is after the last row in 
   * this <code>ResultSet</code> object.
   *
   * @return <code>true</code> if the cursor is after the last row;
   * <code>false</code> if the cursor is at any rv position or the
   * result set contains no rows
   * @exception SQLException if a database access error occurs
   */
  virtual bool isAfterLast(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException) = 0;
  
  /**
   * Retrieves whether the cursor is on the first row of
   * this <code>ResultSet</code> object.
   *
   * @return <code>true</code> if the cursor is on the first row;
   * <code>false</code> otherwise   
   * @exception SQLException if a database access error occurs
   */
  virtual bool isFirst(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException) = 0;
  
  /**
   * Retrieves whether the cursor is on the last row of 
   * this <code>ResultSet</code> object.
   * Note: Calling the method <code>isLast</code> may be expensive
   * because the XDBC driver
   * might need to fetch ahead one row in order to determine 
   * whether the current row is the last row in the result set.
   *
   * @return <code>true</code> if the cursor is on the last row;
   * <code>false</code> otherwise   
   * @exception SQLException if a database access error occurs
   */
  virtual bool isLast(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException) = 0;
  
  /**
   * Moves the cursor to the first row in
   * this <code>ResultSet</code> object.
   *
   * @return <code>true</code> if the cursor is on a valid row;
   * <code>false</code> if there are no rows in the result set
   * @exception SQLException if a database access error
   * occurs or the result set type is <code>TYPE_FORWARD_ONLY</code>
   */
  virtual bool first(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException) = 0;
  
  /**
   * Moves the cursor to the last row in
   * this <code>ResultSet</code> object.
   *
   * @return <code>true</code> if the cursor is on a valid row;
   * <code>false</code> if there are no rows in the result set
   * @exception SQLException if a database access error
   * occurs or the result set type is <code>TYPE_FORWARD_ONLY</code>
   */
  virtual bool last(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException) = 0;
  
  /**
   * Moves the cursor to the given row number in
   * this <code>ResultSet</code> object.
   *
   * <p>If the row number is positive, the cursor moves to 
   * the given row number with respect to the
   * beginning of the result set.  The first row is row 1, the second
   * is row 2, and so on. 
   *
   * <p>If the given row number is negative, the cursor moves to
   * an absolute row position with respect to
   * the end of the result set.  For example, calling the method
   * <code>absolute(-1)</code> positions the 
   * cursor on the last row; calling the method <code>absolute(-2)</code>
   * moves the cursor to the next-to-last row, and so on.
   *
   * <p>An attempt to position the cursor beyond the first/last row in
   * the result set leaves the cursor before the first row or after 
   * the last row.
   *
   * <p><B>Note:</B> Calling <code>absolute(1)</code> is the same
   * as calling <code>first()</code>. Calling <code>absolute(-1)</code> 
   * is the same as calling <code>last()</code>.
   *
   * @param row the number of the row to which the cursor should move.
   *        A positive number indicates the row number counting from the
   *        beginning of the result set; a negative number indicates the
   *        row number counting from the end of the result set
   * @return <code>true</code> if the cursor is on the result set;
   * <code>false</code> otherwise
   * @exception SQLException if a database access error
   * occurs, or the result set type is <code>TYPE_FORWARD_ONLY</code>
   */
  virtual bool absolute(__XDBC_HOOK_DECLARE_MORE int row ) __XDBC_THROW_DECLARE(SQLException) = 0;
  
  /**
   * Moves the cursor a relative number of rows, either positive or negative.
   * Attempting to move beyond the first/last row in the
   * result set positions the cursor before/after the
   * the first/last row. Calling <code>relative(0)</code> is valid, but does
   * not change the cursor position.
   *
   * <p>Note: Calling the method <code>relative(1)</code>
   * is identical to calling the method <code>next()</code> and 
   * calling the method <code>relative(-1)</code> is identical
   * to calling the method <code>previous()</code>.
   *
   * @param rows an <code>int</code> specifying the number of rows to
   *        move from the current row; a positive number moves the cursor
   *        forward; a negative number moves the cursor backward
   * @return <code>true</code> if the cursor is on a row;
   *         <code>false</code> otherwise
   * @exception SQLException if a database access error occurs, 
   *            there is no current row, or the result set type is 
   *            <code>TYPE_FORWARD_ONLY</code>
   */
  virtual bool relative(__XDBC_HOOK_DECLARE_MORE int rows ) __XDBC_THROW_DECLARE(SQLException) = 0;
  
  /**
   * Moves the cursor to the previous row in this
   * <code>ResultSet</code> object.
   *
   * @return <code>true</code> if the cursor is on a valid row; 
   * <code>false</code> if it is off the result set
   * @exception SQLException if a database access error
   * occurs or the result set type is <code>TYPE_FORWARD_ONLY</code>
   */
  virtual bool previous(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException) = 0;

  /**
   * Gives the XDBC driver a hint as to the number of rows that should 
   * be fetched from the database when more rows are needed for this 
   * <code>ResultSet</code> object.
   * If the fetch size specified is zero, the XDBC driver 
   * ignores the value and is free to make its own best guess as to what
   * the fetch size should be.  The default value is set by the 
   * <code>Statement</code> object
   * that created the result set.  The fetch size may be changed at any time.
   *
   * @param rows the number of rows to fetch
   * @exception SQLException if a database access error occurs or the
   * condition <code>0 <= rows <= Statement.getMaxRows()</code> is not satisfied
   */
  virtual void setFetchSize(__XDBC_HOOK_DECLARE_MORE int rows) __XDBC_THROW_DECLARE(SQLException) = 0;
  
  /**
   * Retrieves the fetch size for this 
   * <code>ResultSet</code> object.
   *
   * @return the current fetch size for this <code>ResultSet</code> object
   * @exception SQLException if a database access error occurs
   */
 virtual  int getFetchSize(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException) = 0;

public:
  /**
   * The constant indicating the type for a <code>ResultSet</code> object
   * whose cursor may move only forward.
   */
  const static int TYPE_FORWARD_ONLY;
  
  /**
   * The constant indicating the type for a <code>ResultSet</code> object
   * that is scrollable but generally not sensitive to changes made by others.
   */
  const static int TYPE_SCROLL_INSENSITIVE;
  
  /**
   * The constant indicating the type for a <code>ResultSet</code> object
   * that is scrollable and generally sensitive to changes made by others.
   */
  const static int TYPE_SCROLL_SENSITIVE;
    
  /**
   * The constant indicating the concurrency mode for a
   * <code>ResultSet</code> object that may NOT be updated.
   */
  const static int CONCUR_READ_ONLY;
  
  /**
   * The constant indicating the concurrency mode for a
   * <code>ResultSet</code> object that may be updated.
   */
  const static int CONCUR_UPDATABLE;
};
__OPENCODE_END_NAMESPACE

#endif
