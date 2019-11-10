/**
 *  oracleArrayArrayResultSet.h
 *
 *  Copyright (C) 2002-2020 zosatapo 
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef   __OPENCODE_XDBC_ORACLE_ARRAYRESULTSET_H__
#define   __OPENCODE_XDBC_ORACLE_ARRAYRESULTSET_H__

#include "xdbc/oradefs.h"
#include "xdbc/xmarco.h"
#include "port/ioc_vector.h"

__OPENCODE_BEGIN_NAMESPACE

class OracleArrayResultSet : public ArrayResultSet
{
public:
  friend class OracleArrayStatement;
  friend class OracleResultSetMetaData;

public:
  virtual ~OracleArrayResultSet();
  OracleArrayResultSet(OracleArrayStatement* pStmt,int resultSetType, int resultSetConcurrency,DataTable* __mp_dataTable,bool isEmpty = false);
 
public:
  __XDBC_INTERFACE_ARRAYRESULTSET(ArrayResultSet)
 
  void initialize(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException); 

private:
  void   checkClosed(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException);
  void   checkException(__XDBC_HOOK_DECLARE_MORE OCIError *errhp,sword status) __XDBC_THROW_DECLARE(SQLException);
  
  int    findDefineIndex(int columnIndex); //return value based on 0

  void  setDefineData(__XDBC_HOOK_DECLARE_MORE DataTable* __mp_dataTable)  __XDBC_THROW_DECLARE(SQLException);
  void  setDefineData(__XDBC_HOOK_DECLARE_MORE DataTable* __mp_dataTable,int column)  __XDBC_THROW_DECLARE(SQLException);

private:
  OracleArrayStatement* __mp_statement; 

  int __mi_resultSetType;
  int __mi_resultSetConcurrency;

//-------------------------------------------
// STATE  Information
//-------------------------------------------
  bool __mb_isEmpty;
  bool __mb_isClosed;
  bool __mb_eof;

  int  __mi_prev_count;

//-------------------------------------------
// COLUMN INFO DATA
//-------------------------------------------
  vector<OracleDefine*> __vector_defines;
  vector<OracleColumn>  __vector_columns; //column metadata
  OPENCODE_MAP<string,int>  __map_columns; 
  int __mi_column_count;
  
  
  DataTable*  __mp_dataTable; 
  int __mi_array_size;
};

__OPENCODE_END_NAMESPACE

#endif
