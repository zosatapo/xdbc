/**
 *  oracleResultSetMetaData.h
 *
 *  Copyright (C) 2002-2020 zosatapo 
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef   __OPENCODE_XDBC_ORACLE_RESULTSETMETADATA_H__
#define   __OPENCODE_XDBC_ORACLE_RESULTSETMETADATA_H__

#include "xdbc/oradefs.h"
#include "xdbc/xmarco.h"

__OPENCODE_BEGIN_NAMESPACE

class  OracleResultSetMetaData: public ResultSetMetaData
{
public:
  virtual ~OracleResultSetMetaData();
  OracleResultSetMetaData(const vector<OracleColumn>& __vector_columns);
  
public:
  __XDBC_INTERFACE_RESULTSETMETADATA(ResultSetMetaData)
  
protected:
  void checkColumnIndex(__XDBC_HOOK_DECLARE_MORE int column) __XDBC_THROW_DECLARE(SQLException);

protected:
  vector<OracleColumn> __vector_columns;
  int __mi_column_count;
};

__OPENCODE_END_NAMESPACE

#endif
