/**
 *  dataTable.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef   __OPENCODE_XDBC_DATATABLE_H__
#define   __OPENCODE_XDBC_DATATABLE_H__

#include "xdbc/xdefine.h"

__OPENCODE_BEGIN_NAMESPACE

//----------------------------------------------------------------------------------------------
// DataTableModel class Group
//----------------------------------------------------------------------------------------------

class __OPENCODE_XDBC_API_DECL DataTableModel
{
public:
  virtual ~DataTableModel();
  DataTableModel();

public:
//----------------------------------------------------------------------------
// DataTableMode Schema Access
//----------------------------------------------------------------------------
  /**
   * Returns the number of rows in the model. A
   * <code>DataTable</code> uses this method to determine how many rows it
   * should handle.  This method should be quick, as it
   * is called frequently during rendering.
   *
   * @return the number of rows in the model
   */
  virtual int getRowCount() const = 0;  

  /**
   * Returns the number of columns in the model. A
   * <code>DataTable</code> uses this method to determine how many columns it
   * should handle.
   *
   * @return the number of columns in the model
   */
  virtual int getColumnCount() const = 0;

  /**
   * Retrieves the designated column's SQL type.
   *
   * @param column the first column is 1, the second is 2, ...
   * @return SQL type from Types
   */
  virtual int getColumnType(int column) const = 0;

  /**
   * Retrieves the designated column's SQL type.
   *
   * @param column the first column is 1, the second is 2, ...
   * @return SQL type from  database-dependently SQL type   
   */
  virtual int getColumnRawType(int column) const = 0;

  /**
   * Indicates the designated column's data width in bytes.
   *
   * @param column the first column is 1, the second is 2, ...
   * @return the number of bytes allowed as the data width
   *          of the designated column
   */
  virtual int getColumnSize(int column) const = 0;

  /**
   * Indicates the designated column's normal data offset in bytes.
   *
   * @param column the first column is 1, the second is 2, ...
   * @return the  number of bytes as the data offset 
   *          of the designated column
   */
  virtual XLONG getColumnOffset(int column) const = 0;

  /**
   * Indicates the designated column's data skip parameter in bytes.
   *
   * @param column the first column is 1, the second is 2, ...
   * @return the  number of bytes allowed as the data 
   *          skip paramter  of the designated column
   */
  virtual int getColumnSkip(int column) const = 0;

  /**
   * Indicates the designated column's indicator width in bytes.
   *
   * @param column the first column is 1, the second is 2, ...
   * @return the maximum  number of bytes allowed as the indicator width
   *          of the designated column
   */
  virtual int getColumnIndicatorSize(int column) const = 0; 

  /**
   * Indicates the designated column's normal indicator offset in bytes.
   *
   * @param column the first column is 1, the second is 2, ...
   * @return the  number of bytes as the indicator offset
   *          of the designated column
   */
  virtual XLONG getColumnIndicatorOffset(int column) const = 0;

  /**
   * Indicates the designated column's indicator skip parameter in bytes.
   *
   * @param column the first column is 1, the second is 2, ...
   * @return the  number of bytes allowed as the indicator 
   *          skip paramter  of the designated column
   */
  virtual int getColumnIndicatorSkip(int column) const = 0;

  /**
   * Indicates the designated column's actual length width in bytes.
   *
   * @param column the first column is 1, the second is 2, ...
   * @return the maximum  number of bytes allowed as the actual length width
   *          of the designated column
   */
  virtual int getColumnLengthSize(int column) const = 0;

  /**
   * Indicates the designated column's normal length offset in bytes.
   *
   * @param column the first column is 1, the second is 2, ...
   * @return the  number of bytes as the length offset
   *          of the designated column
   */
  virtual XLONG getColumnLengthOffset(int column) const = 0;

  /**
   * Indicates the designated column's actual length skip parameter in bytes.
   *
   * @param column the first column is 1, the second is 2, ...
   * @return the  number of bytes allowed as the actual length 
   *          skip paramter  of the designated column
   */
  virtual int getColumnLengthSkip(int column) const = 0;  

  /**
   * Indicates the designated column's return code width in bytes.
   *
   * @param column the first column is 1, the second is 2, ...
   * @return the maximum  number of bytes allowed as the return code width
   *          of the designated column
   */
 virtual  int getColumnCodeSize(int column) const = 0;

  /**
   * Indicates the designated column's normal return code offset in bytes.
   *
   * @param column the first column is 1, the second is 2, ...
   * @return the  number of bytes as the return code offset
   *          of the designated column
   */
  virtual XLONG getColumnCodeOffset(int column) const = 0;

  /**
   * Indicates the designated column's return code skip parameter in bytes.
   *
   * @param column the first column is 1, the second is 2, ...
   * @return the  number of bytes allowed as the return code 
   *          skip paramter  of the designated column
   */
  virtual int getColumnCodeSkip(int column) const = 0;

//----------------------------------------------------------------------------
// DataTableMode feature configuration 
//----------------------------------------------------------------------------

 /**
  * Tests whether the database-dependently column's SQL type is provided
  *
  * @param column the first column is 1, the second is 2, ...
  * @return  <code>true</code> if and only if database-dependently column's
  *          SQL type is provided ; <code>false</code> otherwise
  *
  */
  virtual bool useColumnRawType(int column) const = 0;

 /**
  * Tests whether the column's indicator is provided
  *
  * @param column the first column is 1, the second is 2, ...
  * @return  <code>true</code> if and only if column's indicator 
  *         is provided ; <code>false</code> otherwise
  *
  */
  virtual bool useColumnIndicator(int column) const = 0; 

 /**
  * Tests whether the column's actual length is provided
  *
  * @param column the first column is 1, the second is 2, ...
  * @return  <code>true</code> if and only if column's actual length
  *         is provided ; <code>false</code> otherwise
  *
  */
  virtual bool useColumnLength(int column) const = 0;

 /**
  * Tests whether the column's return code is provided
  *
  * @param column the first column is 1, the second is 2, ...
  * @return  <code>true</code> if and only if column's return code
  *         is provided ; <code>false</code> otherwise
  *
  */
  virtual bool useColumnCode(int column) const = 0;

//----------------------------------------------------------------------------
// DataTableMode Data Access
//----------------------------------------------------------------------------

  virtual const char*  getDataBuffer() const = 0;

  virtual const char*  getIndicatorBuffer() const = 0 ;
  virtual const char*  getLengthBuffer() const = 0;
  virtual const char*  getCodeBuffer() const = 0;  
}; 


class __OPENCODE_XDBC_API_DECL GenericTableModel : public DataTableModel
{
public:
  virtual ~GenericTableModel() ;
  GenericTableModel();

public:
  virtual bool useColumnIndicator(int column) const;
  virtual bool useColumnLength(int column) const;
  virtual bool useColumnCode(int column) const ; 

  virtual int getColumnIndicatorSize(int column) const; 
  virtual XLONG getColumnIndicatorOffset(int column) const ;   
  virtual int getColumnIndicatorSkip(int column) const;

  virtual int getColumnLengthSize(int column) const;
  virtual XLONG getColumnLengthOffset(int column) const; 
  virtual int getColumnLengthSkip(int column) const;

  virtual int getColumnCodeSize(int column) const;
  virtual XLONG getColumnCodeOffset(int column) const;
  virtual int getColumnCodeSkip(int column) const;

  virtual const char*  getIndicatorBuffer() const; 
  virtual const char*  getLengthBuffer() const;   
  virtual const char*  getCodeBuffer() const;
};

class __OPENCODE_XDBC_API_DECL QueryTableModel : public DataTableModel
{
public:
  virtual ~QueryTableModel() ;
  QueryTableModel();

public:
  virtual bool useColumnIndicator(int column) const;
  virtual bool useColumnLength(int column) const;
  virtual bool useColumnCode(int column) const;

  virtual int getColumnIndicatorSize(int column) const;
  virtual int getColumnLengthSize(int column) const;
  virtual int getColumnCodeSize(int column) const;
};

//----------------------------------------------------------------------------------------------
// DataTable clas 
//----------------------------------------------------------------------------------------------

class __OPENCODE_XDBC_API_DECL DataTable
{
public:
  ~DataTable() ;

  DataTable(const DataTableModel * aDataModel);

public:
//----------------------------------------------------------------------------
// DataTableMode  Access
//----------------------------------------------------------------------------
  const DataTableModel* getModel() const;

  void setModel(const DataTableModel* aDataModel);

public:
//----------------------------------------------------------------------------
// DataTableMode Schema Access
//----------------------------------------------------------------------------
  int getRowCount() const; 
  int getColumnCount() const; 
  int getColumnType(int column) const;

  XLONG getColumnOffset(int column) const;
  int getColumnSize(int column) const;
  int getColumnSkip(int column) const;

protected:
  const DataTableModel* dataModel;
};

__OPENCODE_END_NAMESPACE

#endif
