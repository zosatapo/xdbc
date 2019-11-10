/**
 *  ifxArrayResultSet.h
 *
 *  Copyright (C) 2007-2012  zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */

#ifndef   __OPENCODE_XDBC_INFORMIX_ARRAYRESULTSET_H__
#define   __OPENCODE_XDBC_INFORMIX_ARRAYRESULTSET_H__

#include "xdbc/ifxdefs.h"
#include "xdbc/xmarco.h"

__OPENCODE_BEGIN_NAMESPACE

class IfxArrayResultSet : public ArrayResultSet
{
public:
  friend class IfxArrayStatement;
  friend class IfxResultSetMetaData;

public:
  virtual ~IfxArrayResultSet();
  IfxArrayResultSet(IfxArrayStatement* pStmt,const string& cursor_name,const string& cursor_id,int resultSetType, int resultSetConcurrency,DataTable* __mp_dataTable,bool close_stmt = true);

public:
  __XDBC_INTERFACE_ARRAYRESULTSET()
  
  virtual void initialize(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException);

private:
  void checkClosed(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException);

  void setDefineData(__XDBC_HOOK_DECLARE_MORE DataTable* __mp_dataTable,int row)  __XDBC_THROW_DECLARE(SQLException);
  void setDefineData(__XDBC_HOOK_DECLARE_MORE DataTable* __mp_dataTable,int row,int column)  __XDBC_THROW_DECLARE(SQLException);

private:
  IfxArrayStatement* __mp_statement; 

  int __mi_resultSetType;
  int __mi_resultSetConcurrency;

//---------------------------------------
// INFORMIX-ESQLC
//---------------------------------------
  struct sqlda*  __mp_ec_sqlda_columns;   /* sqlda struct pointer to Column */

  string __ms_cursor_name;
  string __ms_cursor_id;
  
//-------------------------------------------
// STATE  Information
//-------------------------------------------
  bool __mb_isEmpty;
  bool __mb_isClosed;  
  bool __mb_close_stmt;

  int  __mi_prev_count;

//-------------------------------------------
// COLUMN INFO DATA
//-------------------------------------------
  vector<IfxColumn>  __vector_columns; //column metadata
  OPENCODE_MAP<string,int>  __map_columns; 
  int __mi_column_count;

  DataTable* __mp_dataTable;
  int  __mi_array_size;
};

__OPENCODE_END_NAMESPACE

#endif
