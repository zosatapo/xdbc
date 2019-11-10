/**
 *  callableStatement.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
#ifndef   __OPENCODE_XDBC_CALLABLESTATEMENT_H__
#define   __OPENCODE_XDBC_CALLABLESTATEMENT_H__

#include "xdbc/xdefine.h"

#include "xdbc/clob.h"
#include "xdbc/blob.h"

#include "xdbc/preparedStatement.h" 

__OPENCODE_BEGIN_NAMESPACE

/**
 * The interface used to execute SQL stored procedures.  The XDBC API 
 * provides a stored procedure SQL escape syntax that allows stored procedures 
 * to be called in a standard way for all RDBMSs. This escape syntax has one 
 * form that includes a result parameter and one that does not. If used, the result 
 * parameter must be registered as an OUT parameter. The rv parameters
 * can be used for input, output or both. Parameters are referred to 
 * sequentially, by number, with the first parameter being 1.
 * 
 *   {?= call <procedure-name>[<arg1>,<arg2>, ...]}
 *   {call <procedure-name>[<arg1>,<arg2>, ...]}
 * 
 * IN parameter values are set using the <code>set</code> methods inherited from
 * {@link PreparedStatement}.  The type of all OUT parameters must be
 * registered prior to executing the stored procedure; their values
 * are retrieved after execution via the <code>get</code> methods provided here.
 */

class __OPENCODE_XDBC_API_DECL CallableStatement : public  PreparedStatement
{
protected:
  virtual ~CallableStatement(){}

public:
  /**
   * Registers the OUT parameter in ordinal position
   * <code>paramIndex</code> to the XDBC type
   * <code>sqlType</code>.  All OUT parameters must be registered
   * before a stored procedure is executed.
   * <p>
   * The XDBC type specified by <code>sqlType</code> for an OUT
   * parameter determines the XDBC type that must be used
   * in the <code>get</code> method to read the value of that parameter.
   * <p>
   * If the XDBC type expected to be returned to this output parameter
   * is specific to this particular database, <code>sqlType</code>
   * should be <code>XDBC Types.OTHER</code>.  The method
   * {@link #getObject} retrieves the value.
   *
   * @param paramIndex the first parameter is 1, the second is 2,
   *        and so on
   * @param sqlType the XDBC type code defined by <code>XDBC Types</code>.
   *        If the parameter is of XDBC type <code>NUMERIC</code>
   *        or <code>DECIMAL</code>, the version of
   *        <code>registerOutParameter</code> that accepts a scale value
   *        should be used.
   *
   * @exception SQLException if a database access error occurs
   */
  virtual void registerOutParameter(__XDBC_HOOK_DECLARE_MORE int paramIndex, int sqlType) __XDBC_THROW_DECLARE(SQLException) = 0;

  /**
   * Registers the parameter in ordinal position
   * <code>paramIndex</code> to be of XDBC type
   * <code>sqlType</code>.  This method must be called
   * before a stored procedure is executed.
   * <p>
   * The XDBC type specified by <code>sqlType</code> for an OUT
   * parameter determines the XDBC type that must be used
   * in the <code>get</code> method to read the value of that parameter.
   * <p>
   * This version of <code>registerOutParameter</code> should be
   * used when the parameter is of XDBC type <code>NUMERIC</code>
   * or <code>DECIMAL</code>.
   *
   * @param paramIndex the first parameter is 1, the second is 2,
   * and so on
   * @param sqlType the SQL type code defined by <code>XDBC Types</code>.
   * @param scale the desired number of digits to the right of the
   * decimal point.  It must be greater than or equal to zero.
   * @exception SQLException if a database access error occurs
   */
  virtual void registerOutParameter(__XDBC_HOOK_DECLARE_MORE int paramIndex, int sqlType, int scale) __XDBC_THROW_DECLARE(SQLException) = 0;


  /**
   * Retrieves whether the OUT parameter read had the value of
   * SQL <code>NULL</code>.  
   *
   * @return <code>true</code> if the last parameter read was SQL
   * <code>NULL</code>; <code>false</code> otherwise
   * @exception SQLException if a database access error occurs
   */
  virtual bool wasNull(__XDBC_HOOK_DECLARE_MORE int paramIndex)  __XDBC_THROW_DECLARE(SQLException) = 0;

  /**
   * Retrieves the value of the designated SQL <code>CHAR</code>,
   * <code>VARCHAR</code>, or <code>LONGVARCHAR</code> parameter as a
   * <code>String</code> in the XDBC .
   * <p>
   * For the fixed-length type XDBC <code>CHAR</code>,
   * the <code>String</code> object
   * returned has exactly the same value the XDBC
   * <code>CHAR</code> value had in the
   * database, including any padding added by the database.
   *
   * @param paramIndex the first parameter is 1, the second is 2,
   * and so on
   * @return the parameter value. If the value is SQL <code>NULL</code>,
   *         the result is <code>null pointer</code>.
   * @exception SQLException if a database access error occurs
   */
  virtual const char* getString(__XDBC_HOOK_DECLARE_MORE  int paramIndex,int& sizeBytes) __XDBC_THROW_DECLARE(SQLException) = 0;
  
  /**
   * Retrieves the value of the designated SQL <code>TINYINT</code> parameter
   * as a <code>byte</code> in the XDBC .
   *
   * @param paramIndex the first parameter is 1, the second is 2,
   * and so on
   * @return the parameter value.  If the value is SQL <code>NULL</code>, the result
   * is <code>0</code>.
   * @exception SQLException if a database access error occurs
   */
  virtual char getByte(__XDBC_HOOK_DECLARE_MORE  int paramIndex) __XDBC_THROW_DECLARE(SQLException)
  {
    char value = 0;
    getByte(__XDBC_HOOK_INVOKE_MORE paramIndex,value);
    return value;
  }

  /**
   * Retrieves the value of the designated SQL <code>TINYINT</code> parameter
   * as a <code>byte</code> in the XDBC .
   *
   * @param paramIndex the first parameter is 1, the second is 2,
   * and so on
   * @param value the parameter value.  If the value is SQL <code>NULL</code>, the result
   * is <code>0</code>.
   * 
   * @return <code>false</code> if the parameter value read was SQL
   *         <code>NULL</code> and <code>true</code> otherwise
   * @exception SQLException if a database access error occurs
   */    
  virtual bool getByte(__XDBC_HOOK_DECLARE_MORE  int paramIndex,char& value) __XDBC_THROW_DECLARE(SQLException) = 0;

  /**
   * Retrieves the value of the designated SQL <code>SMALLINT</code> parameter
   * as a <code>short</code> in the XDBC .
   *
   * @param paramIndex the first parameter is 1, the second is 2,
   * and so on
   * @return the parameter value.  If the value is SQL <code>NULL</code>, the result
   * is <code>0</code>.
   * @exception SQLException if a database access error occurs
   */
  virtual short getShort(__XDBC_HOOK_DECLARE_MORE  int paramIndex) __XDBC_THROW_DECLARE(SQLException)
  {
    short value = 0;
    getShort(__XDBC_HOOK_INVOKE_MORE paramIndex,value);
    return value;
  }

  /**
   * Retrieves the value of the designated SQL <code>SMALLINT</code> parameter
   * as a <code>short</code> in the XDBC .
   *
   * @param paramIndex the first parameter is 1, the second is 2,
   * and so on
   * @param value the parameter value.  If the value is SQL <code>NULL</code>, the result
   * is <code>0</code>.
   * 
   * @return <code>false</code> if the parameter value read was SQL
   *         <code>NULL</code> and <code>true</code> otherwise
   * @exception SQLException if a database access error occurs
   */    
  virtual bool  getShort(__XDBC_HOOK_DECLARE_MORE  int paramIndex,short& value) __XDBC_THROW_DECLARE(SQLException) = 0;
  

  /**
   * Retrieves the value of the designated SQL <code>INTEGER</code> parameter
   * as an <code>int</code> in the XDBC .
   *
   * @param paramIndex the first parameter is 1, the second is 2,
   * and so on
   * @return the parameter value.  If the value is SQL <code>NULL</code>, the result
   * is <code>0</code>.
   * @exception SQLException if a database access error occurs
   */
  virtual int  getInt(__XDBC_HOOK_DECLARE_MORE  int paramIndex) __XDBC_THROW_DECLARE(SQLException)
  {
    int value = 0;
    getInt(__XDBC_HOOK_INVOKE_MORE paramIndex,value);
    return value;
  }


  /**
   * Retrieves the value of the designated SQL <code>INTEGER</code> parameter
   * as an <code>int</code> in the XDBC .
   *
   * @param paramIndex the first parameter is 1, the second is 2,
   * and so on
   * @param value the parameter value.  If the value is SQL <code>NULL</code>, the result
   * is <code>0</code>.
   * 
   * @return <code>false</code> if the parameter value read was SQL
   *         <code>NULL</code> and <code>true</code> otherwise
   * @exception SQLException if a database access error occurs
   */    
  virtual bool getInt(__XDBC_HOOK_DECLARE_MORE  int paramIndex,int& value) __XDBC_THROW_DECLARE(SQLException) = 0;

  /**
   * Retrieves the value of the designated XDBC <code>BIGINT</code> parameter 
   * as a <code>XLONG</code> in the XDBC.
   *
   * @param paramIndex the first parameter is 1, the second is 2, 
   * and so on
   * @return the parameter value.  If the value is SQL <code>NULL</code>, the result 
   * is <code>0</code>.
   * @exception SQLException if a database access error occurs
   */
  virtual XLONG getLong(__XDBC_HOOK_DECLARE_MORE  int paramIndex) __XDBC_THROW_DECLARE(SQLException)
  {
    XLONG value = 0;
    getLong(__XDBC_HOOK_INVOKE_MORE paramIndex,value);
    return value;
  }

  /**
   * Retrieves the value of the designated XDBC <code>BIGINT</code> parameter 
   * as a <code>XLONG</code> in the XDBC.
   *
   * @param paramIndex the first parameter is 1, the second is 2, 
   * and so on
   * @param value the parameter value.  If the value is SQL <code>NULL</code>, the result 
   * is <code>0</code>.
   * 
   * @return <code>false</code> if the parameter value read was SQL
   *         <code>NULL</code> and <code>true</code> otherwise
   * @exception SQLException if a database access error occurs
   */    
  virtual bool      getLong(__XDBC_HOOK_DECLARE_MORE  int paramIndex,XLONG& value) __XDBC_THROW_DECLARE(SQLException) = 0;

  /**
   * Retrieves the value of the designated SQL <code>FLOAT</code> parameter
   * as a <code>float</code> in the XDBC .
   *
   * @param paramIndex the first parameter is 1, the second is 2,
   *        and so on
   * @return the parameter value.  If the value is SQL <code>NULL</code>, the result
   *         is <code>0</code>.
   * @exception SQLException if a database access error occurs
   */
  virtual float getFloat(__XDBC_HOOK_DECLARE_MORE  int paramIndex) __XDBC_THROW_DECLARE(SQLException)
  {
    float value = 0;
    getFloat(__XDBC_HOOK_INVOKE_MORE paramIndex,value);
    return value;
  }

  /**
   * Retrieves the value of the designated SQL <code>FLOAT</code> parameter
   * as a <code>float</code> in the XDBC .
   *
   * @param paramIndex the first parameter is 1, the second is 2,
   *        and so on
   * @param value  the parameter value.  If the value is SQL <code>NULL</code>, the result
   *         is <code>0</code>.
   * 
   * @return <code>false</code> if the parameter value read was SQL
   *         <code>NULL</code> and <code>true</code> otherwise
   * @exception SQLException if a database access error occurs
   */  
  virtual bool  getFloat(__XDBC_HOOK_DECLARE_MORE  int paramIndex,float& value) __XDBC_THROW_DECLARE(SQLException) = 0;

  /**
   * Retrieves the value of the designated SQL <code>DOUBLE</code> parameter as a <code>double</code>
   * in the XDBC .
   * @param paramIndex the first parameter is 1, the second is 2,
   *        and so on
   * @return the parameter value.  If the value is SQL <code>NULL</code>, the result
   *         is <code>0</code>.
   * @exception SQLException if a database access error occurs
   */
  virtual double getDouble(__XDBC_HOOK_DECLARE_MORE  int paramIndex) __XDBC_THROW_DECLARE(SQLException)
  {
    double value = 0;
    getDouble(__XDBC_HOOK_INVOKE_MORE paramIndex,value);
    return value;
  }

  /**
   * Retrieves the value of the designated SQL <code>DOUBLE</code> parameter as a <code>double</code>
   * in the XDBC .
   * @param paramIndex the first parameter is 1, the second is 2,
   *        and so on
   * @param value the parameter value.  If the value is SQL <code>NULL</code>, the result
   *         is <code>0</code>.
   * 
   * @return <code>false</code> if the parameter value read was SQL
   *         <code>NULL</code> and <code>true</code> otherwise
   * @exception SQLException if a database access error occurs
   */  
  virtual bool   getDouble(__XDBC_HOOK_DECLARE_MORE  int paramIndex,double& value) __XDBC_THROW_DECLARE(SQLException) = 0;

  /**
   * Retrieves the value of the designated SQL <code>DATE</code> parameter as an
   * <code>XDBC_DATE</code> object.
   * @param paramIndex the first parameter is 1, the second is 2,
   *        and so on
   * @return the parameter value.  If the value is SQL <code>NULL</code>, the result
   *         is <code>null</code>.
   * @exception SQLException if a database access error occurs
   */
  virtual XDBC_DATE getDate(__XDBC_HOOK_DECLARE_MORE  int paramIndex) __XDBC_THROW_DECLARE(SQLException)
  {
    XDBC_DATE value;
    getDate(__XDBC_HOOK_INVOKE_MORE paramIndex,value);
    return value;
  }

  /**
   * Retrieves the value of the designated SQL <code>DATE</code> parameter as an
   * <code>XDBC_DATE</code> object.
   * @param paramIndex the first parameter is 1, the second is 2,
   *        and so on
   * @param value  the parameter value.  If the value is SQL <code>NULL</code>, the result
   *         is <code>null</code>.
   * 
   * @return <code>false</code> if the parameter value read was SQL
   *         <code>NULL</code> and <code>true</code> otherwise
   * @exception SQLException if a database access error occurs
   */  
  virtual bool    getDate(__XDBC_HOOK_DECLARE_MORE  int paramIndex,XDBC_DATE& value) __XDBC_THROW_DECLARE(SQLException) = 0;

  /**
   * Retrieves the value of the designated SQL <code>TIME</code> parameter as an
   * <code>XDBC_TIME</code> object.
   *
   * @param paramIndex the first parameter is 1, the second is 2,
   *        and so on
   * @return the parameter value.  If the value is SQL <code>NULL</code>, the result
   *         is <code>null</code>.
   * @exception SQLException if a database access error occurs
   */
  virtual XDBC_TIME getTime(__XDBC_HOOK_DECLARE_MORE  int paramIndex) __XDBC_THROW_DECLARE(SQLException)
  {
    XDBC_TIME value;
    getTime(__XDBC_HOOK_INVOKE_MORE paramIndex,value);
    return value;
  }

  /**
   * Retrieves the value of the designated SQL <code>TIME</code> parameter as an
   * <code>XDBC_TIME</code> object.
   *
   * @param paramIndex the first parameter is 1, the second is 2,
   *        and so on
   * @param value  the parameter value.  If the value is SQL <code>NULL</code>, the result
   *         is <code>null</code>.
   * 
   * @return <code>false</code> if the parameter value read was SQL
   *         <code>NULL</code> and <code>true</code> otherwise
   * @exception SQLException if a database access error occurs
   */    
  virtual bool    getTime(__XDBC_HOOK_DECLARE_MORE  int paramIndex,XDBC_TIME& value) __XDBC_THROW_DECLARE(SQLException) = 0;

  /**
   * Retrieves the value of the designated SQL <code>TIMESTAMP</code> parameter as an
   * <code>XDBC_TIMESTAMP</code> object.
   *
   * @param paramIndex the first parameter is 1, the second is 2,
   *        and so on
   * @return the parameter value.  If the value is SQL <code>NULL</code>, the result
   *         is <code>null</code>.
   * @exception SQLException if a database access error occurs
   */
  virtual XDBC_TIMESTAMP getTimestamp(__XDBC_HOOK_DECLARE_MORE  int paramIndex) __XDBC_THROW_DECLARE(SQLException)
  {
    XDBC_TIMESTAMP value;
    getTimestamp(__XDBC_HOOK_INVOKE_MORE paramIndex,value);
    return value;
  }

  /**
   * Retrieves the value of the designated SQL <code>TIMESTAMP</code> parameter as an
   * <code>XDBC_TIMESTAMP</code> object.
   *
   * @param paramIndex the first parameter is 1, the second is 2,
   *        and so on
   * @param value the parameter value.  If the value is SQL <code>NULL</code>, the result
   *         is <code>null</code>.
   * 
   * @return <code>false</code> if the parameter value read was SQL
   *         <code>NULL</code> and <code>true</code> otherwise
   * @exception SQLException if a database access error occurs
   */    
  virtual bool         getTimestamp(__XDBC_HOOK_DECLARE_MORE  int paramIndex,XDBC_TIMESTAMP& value) __XDBC_THROW_DECLARE(SQLException) = 0;

  /**
   * Retrieves the value of the designated column in the current row
   * of this <code>ResultSet</code> object as
   * an <code>XDBC_INTERVAL</code> object XDBC .
   *
   * @param paramIndex the first parameter is 1, the second is 2,
   *        and so on
   * @return the parameter value.  If the value is SQL <code>NULL</code>, the result
   *         is <code>null</code>.
   * @exception SQLException if a database access error occurs
   */
  virtual XDBC_INTERVAL getInterval(__XDBC_HOOK_DECLARE_MORE  int paramIndex) __XDBC_THROW_DECLARE(SQLException)
  {
    XDBC_INTERVAL value;
    getInterval(__XDBC_HOOK_INVOKE_MORE paramIndex,value);
    return value;
  }

  /**
   * Retrieves the value of the designated column in the current row
   * of this <code>ResultSet</code> object as
   * an <code>XDBC_INTERVAL</code> object XDBC .
   *
   * @param paramIndex the first parameter is 1, the second is 2,
   *        and so on
   * @param value the parameter value.  If the value is SQL <code>NULL</code>, the result
   *         is <code>null</code>.
   * 
   * @return <code>false</code> if the parameter value read was SQL
   *         <code>NULL</code> and <code>true</code> otherwise
   * @exception SQLException if a database access error occurs
   */  
  virtual bool  getInterval(__XDBC_HOOK_DECLARE_MORE  int paramIndex,XDBC_INTERVAL& value) __XDBC_THROW_DECLARE(SQLException) = 0;

//================================================================
//XDBC Advanced Features
//================================================================
public:
  /**
   * Retrieves the value of the designated column in the current row
   * of this <code>ResultSet</code> object as a <code>Blob</code> object
   * in the XDBC.
   *
   * @param paramIndex the first column is 1, the second is 2, ...
   * @return a <code>Blob</code> object representing the SQL 
   *         <code>BLOB</code> value in the specified column
   * @exception SQLException if a database access error occurs
   */
  virtual Blob* getBlob(__XDBC_HOOK_DECLARE_MORE  int paramIndex) __XDBC_THROW_DECLARE(SQLException) = 0;
  
  /**
   * Retrieves the value of the designated column in the current row
   * of this <code>ResultSet</code> object as a <code>Clob</code> object
   * in the XDBC.
   *
   * @param paramIndex the first column is 1, the second is 2, ...
   * @return a <code>Clob</code> object representing the SQL 
   *         <code>CLOB</code> value in the specified column
   * @exception SQLException if a database access error occurs
   */
  virtual Clob* getClob(__XDBC_HOOK_DECLARE_MORE  int paramIndex) __XDBC_THROW_DECLARE(SQLException) = 0;

};

__OPENCODE_END_NAMESPACE

#endif
