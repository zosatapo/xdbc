/**
 *  odbcResultSetMetaData.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef __OPENCODE_XDBC_ODBCRESULTSETMETADATA_H__
#define __OPENCODE_XDBC_ODBCRESULTSETMETADATA_H__

#include "xdbc/odbc.h"
#include "xdbc/odbcTypes.h"

__OPENCODE_BEGIN_NAMESPACE

class __OPENCODE_XDBC_API_DECL  OdbcResultSetMetaData: public ResultSetMetaData
{
public:
  virtual ~OdbcResultSetMetaData();
  OdbcResultSetMetaData(const vector<ODBC_COLUMN>& vectorColumns,struct ODBC_DRIVER_OPTIONS* odbcOptions);

public:
  __XDBC_INTERFACE_RESULTSETMETADATA(ResultSetMetaData)
  
protected:
  void checkColumnIndex(__XDBC_HOOK_DECLARE_MORE int column)__XDBC_THROW_DECLARE(SQLException);

protected:
  vector<ODBC_COLUMN>         __vector_columns;
  int                         __mi_column_count;
  OdbcTypes*            __odbcTypes;
  struct ODBC_DRIVER_OPTIONS* __odbc_options;
};

__OPENCODE_END_NAMESPACE

#endif

