/**
 *  resultsetMetaData.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef   __OPENCODE_XDBC_RESULTSETMETADATA_H__
#define   __OPENCODE_XDBC_RESULTSETMETADATA_H__

#include "xdbc/xdefine.h"

__OPENCODE_BEGIN_NAMESPACE

class __OPENCODE_XDBC_API_DECL ResultSetMetaData
{
public:
  virtual ~ResultSetMetaData(){}

public:
  /**
   * Returns the number of columns in this <code>ResultSet</code> object.
   *
   * @return the number of columns
   * @exception SQLException if a database access error occurs
   */
  virtual int getColumnCount(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException)=0;
  
  /**
   * Indicates the nullability of values in the designated column.    
   *
   * @param columnIndex the first column is 1, the second is 2, ...
   * @return the nullability status of the given column; one of <code>columnNoNulls</code>,
   *          <code>columnNullable</code> or <code>columnNullableUnknown</code>
   * @exception SQLException if a database access error occurs
   */
  virtual int isNullable(__XDBC_HOOK_DECLARE_MORE int columnIndex) __XDBC_THROW_DECLARE(SQLException)=0;

  /**
   * Indicates the char semantics of values in the designated column.
   *
   * @param columnIndex the first column is 1, the second is 2, ...
   * @return the nullability status of the given column; one of <code>columnNoCharSemantics</code>,
   *          <code>columnCharSemantics</code> or <code>columnCharSemanticsUnknown</code>
   * @exception SQLException if a database access error occurs
   */
  virtual int isCharSemantics(__XDBC_HOOK_DECLARE_MORE int columnIndex) __XDBC_THROW_DECLARE(SQLException)=0;
  
  /**
   * Indicates the designated column's normal maximum width in bytes.
   *
   * @param columnIndex the first column is 1, the second is 2, ...
   * @return the normal maximum number of bytes allowed as the width
   *          of the designated column
   * @exception SQLException if a database access error occurs
   */
  virtual int getColumnDisplaySize(__XDBC_HOOK_DECLARE_MORE int columnIndex) __XDBC_THROW_DECLARE(SQLException)=0;
  
  /**
   * Get the designated column's name.
   *
   * @param columnIndex the first column is 1, the second is 2, ...
   * @return column name
   * @exception SQLException if a database access error occurs
   */
  virtual string getColumnName(__XDBC_HOOK_DECLARE_MORE int columnIndex) __XDBC_THROW_DECLARE(SQLException)=0;
  
  /**
   * Get the designated column's table's schema.
   *
   * @param columnIndex the first column is 1, the second is 2, ...
   * @return schema name or "" if not applicable
   * @exception SQLException if a database access error occurs
   */
  virtual string getSchemaName(__XDBC_HOOK_DECLARE_MORE int columnIndex) __XDBC_THROW_DECLARE(SQLException)=0;
  
  /**
   * Get the designated column's number of decimal digits.
   *
   * @param columnIndex the first column is 1, the second is 2, ...
   * @return precision
   * @exception SQLException if a database access error occurs
   */
  virtual int getPrecision(__XDBC_HOOK_DECLARE_MORE int columnIndex) __XDBC_THROW_DECLARE(SQLException)=0;
  
  /**
   * Gets the designated column's number of digits to right of the decimal point.
   *
   * @param columnIndex the first column is 1, the second is 2, ...
   * @return scale
   * @exception SQLException if a database access error occurs
   */
  virtual int getScale(__XDBC_HOOK_DECLARE_MORE int columnIndex) __XDBC_THROW_DECLARE(SQLException)=0;  
  
  /**
   * Gets the designated column's table name. 
   *
   * @param columnIndex the first column is 1, the second is 2, ...
   * @return table name or "" if not applicable
   * @exception SQLException if a database access error occurs
   */
  virtual string getTableName(__XDBC_HOOK_DECLARE_MORE int columnIndex) __XDBC_THROW_DECLARE(SQLException)=0;
  
  /**
   * Retrieves the designated column's SQL type.
   *
   * @param columnIndex the first column is 1, the second is 2, ...
   * @return SQL type from Types
   * @exception SQLException if a database access error occurs
   */
  virtual int getColumnType(__XDBC_HOOK_DECLARE_MORE int columnIndex) __XDBC_THROW_DECLARE(SQLException)=0;

  /**
   * Retrieves the designated raw database column's SQL type.
   *
   * @param columnIndex the first column is 1, the second is 2, ...
   * @return SQL type from database 
   * @exception SQLException if a database access error occurs
   */
 virtual int getColumnRawType(__XDBC_HOOK_DECLARE_MORE int columnIndex) __XDBC_THROW_DECLARE(SQLException)=0;
  
  /**
   * Retrieves the designated column's database-specific type name.
   *
   * @param columnIndex the first column is 1, the second is 2, ...
   * @return type name used by the database. If the column type is
   * a user-defined type, then a fully-qualified type name is returned.
   * @exception SQLException if a database access error occurs
   */
  virtual string getColumnTypeName(__XDBC_HOOK_DECLARE_MORE int columnIndex) __XDBC_THROW_DECLARE(SQLException)=0; 

public:
  /**
   * The constant indicating that a
   * column does not allow <code>NULL</code> values.
   */
  static const int columnNoNulls;

  /**
   * The constant indicating that a
   * column allows <code>NULL</code> values.
   */
  static const int columnNullable;

  /**
   * The constant indicating that the
   * nullability of a column's values is unknown.
   */
  static const int columnNullableUnknown;

  /**
   * The constant indicating that a
   * column is NOT char semantics.
   */
  static const int columnNoCharSemantics;

  /**
   * The constant indicating that a
   * column is char semantics.
   */
  static const int columnCharSemantics;

  /**
   * The constant indicating that the
   * char semantics of a column's values is unknown.
   */
  static const int columnCharSemanticsUnknown;  
};
__OPENCODE_END_NAMESPACE

#endif
