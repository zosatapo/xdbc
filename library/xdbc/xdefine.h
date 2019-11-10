/**
 *  xdefine.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef __OPENCODE_XDBC_CONFIG_H__
#define __OPENCODE_XDBC_CONFIG_H__

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
#ifdef OPENCODE_BUILD_XDBC_DLL
#  define __OPENCODE_XDBC_API_DECL __OPENCODE_EXPORT_DECL
#else
# define __OPENCODE_XDBC_API_DECL  __OPENCODE_IMPORT_DECL
#endif

#include "port/ioc_G_config.h"
#include "port/ioc_types.h" 
#include "port/ioc_class.h"
#include "port/ioc_map.h"

#include "xdbc/xtypes.h"
#include "xdbc/xmarco.h"

__OPENCODE_BEGIN_NAMESPACE

//-------------------------------------------------------------------------
// 1-3  XDBC LIFETIME LONGEVITY
//-------------------------------------------------------------------------
#define __XDBC_LONGEVITY_MAX                   (999996999)
#define __XDBC_LONGEVITY_MIN                   (999996000)

#define __XDBC_LONGEVITY_SQLSOURCE             (__XDBC_LONGEVITY_MAX - 5)
#define __XDBC_LONGEVITY_DRIVERMAP             (__XDBC_LONGEVITY_MAX - 10)
#define __XDBC_LONGEVITY_ODBCDRIVERMAP         (__XDBC_LONGEVITY_MAX - 15)
#define __XDBC_LONGEVITY_ODBCTYPES             (__XDBC_LONGEVITY_MAX - 20)

//---------------------------------------------------------------------
//     XDBC property key definition 
//---------------------------------------------------------------------
#define  XDBC_URL                   "XDBC_URL"
#define  XDBC_USER                  "XDBC_USER" 
#define  XDBC_PASSWORD              "XDBC_PASSWORD"

#define  XDBC_DRIVER                "XDBC_DRIVER"
#define  XDBC_VERSION               "XDBC_VERSION"
#define  XDBC_SQLKEY_KEY            "XDBC_SQLKEY_KEY"
#define  XDBC_SQLVALUE_KEY          "XDBC_SQLVALUE_KEY"

//---------------------------------------------------------------------
//     XDBC type definition 
//---------------------------------------------------------------------
typedef map<string,string>  __XDBC_ATTRIBUTES;

//---------------------------------------------------------------------
//     XDBC class forward declaration
//---------------------------------------------------------------------
class SQLManageable;
class SQLImplementation;

class Connection;
class Statement;
class PreparedStatement;
class CallableStatement;
class ArrayStatement;
class ResultSet;
class ArrayResultSet;
class ResultSetMetaData;

class __OPENCODE_XDBC_API_DECL SQLException : public BaseException
{
public:
  ~SQLException();
  
  SQLException();
  SQLException(const string& message);
  SQLException(const string& message,const string& sqlstate);
  SQLException(const string& message,XLONG sqlcode);
  SQLException(const string& message,const string& sqlstate,XLONG sqlcode);
   
public:
  /**
   * Retrieves the vendor-specific exception code
   * for this <code>SQLException</code> object.
   *
   * @return the vendor's error code
   */
  XLONG     getSQLCode() const;
  
  /**
   * Retrieves the SQLState for this <code>SQLException</code> object.
   *
   * @return the SQLState value
   */
  const string& getSQLState() const;
  
protected:
  string   sqlstate;
};

class __OPENCODE_XDBC_API_DECL SQLManageable
{
public:
  /**
   * Releases this <code>SQLManageable</code> object's database 
   * and XDBC resources immediately instead of waiting for
   * this to happen when it is automatically closed.
   * It is generally good practice to release resources as soon as
   * you are finished with them to avoid tying up database
   * resources.
   * <P>
   * Calling the method <code>close</code> on an <code>SQLManageable</code>
   * object that is already closed has no effect.  
   */
  virtual void close() = 0;

  /** Destroy this <code>SQLManageable</code> object and release resource*/  
  virtual void destroy() = 0;

//=====================================================================
// XDBC Advanced features
//=====================================================================
public:
  /**
   *  return properties for the <code>SQLManageable</code> object
   */
  virtual const __XDBC_ATTRIBUTES&   getAttributes() const;         
  
  virtual string                     getAttribute(const string& key) const;
  virtual void                       removeAttribute(const string& key);
  virtual void                       setAttribute(const string& key,const string& attribute);

protected:
  virtual ~SQLManageable(){}
  SQLManageable(){}
  
  __XDBC_ATTRIBUTES  __mo_attributes;  
};

class __OPENCODE_XDBC_API_DECL SQLImplementation : public  SQLManageable
{
public:
  /**
   * Releases this <code>SQLImplementation</code> object's database 
   * and XDBC resources immediately instead of waiting for
   * this to happen when it is automatically closed.
   * It is generally good practice to release resources as soon as
   * you are finished with them to avoid tying up database
   * resources.
   * <P>
   * Calling the method <code>close</code> on an <code>SQLImplementation</code>
   * object that is already closed has no effect.
   * <P>
   * <B>Note:</B> An <code>SQLImplementation</code> object is automatically closed 
   * when it is garbage collected. When an <code>SQLImplementation</code> object is 
   * closed, its current <code>ResultSet</code> object, if one exists, is 
   * also closed.  
   */
  virtual void close() = 0;

  /**
   * Retrieves the result set type for <code>ResultSet</code> objects
   * generated by this <code>Statement</code> object.
   *
   * @return one of <code>ResultSet.TYPE_FORWARD_ONLY</code>,
   * <code>ResultSet.TYPE_SCROLL_INSENSITIVE</code>, or  
   * <code>ResultSet.TYPE_SCROLL_SENSITIVE</code>
   * @exception SQLException if a database access error occurs
   */
  virtual int getResultSetType(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException) = 0;
  
  /**
   * Retrieves the <code>Connection</code> object
   * that produced this <code>Statement</code> object.
   * @return the connection that produced this statement
   * @exception SQLException if a database access error occurs
   */
  virtual Connection* getConnection(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException) = 0; 

public:
  /**
   * The constant indicating that the param count of  SQL  is unknown.
   */
  const static int paramCountUnknown;

protected:
  virtual ~SQLImplementation(){}
  SQLImplementation(){}
};  
  
__OPENCODE_END_NAMESPACE

#endif
