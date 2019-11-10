/**
 *  ifxResultSetMetaData.h
 *
 *  Copyright (C) 2007-2012  zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef   __OPENCODE_XDBC_INFORMIX_RESULTSETMETADATA_H__
#define   __OPENCODE_XDBC_INFORMIX_RESULTSETMETADATA_H__

#include "xdbc/ifxdefs.h"
#include "xdbc/xmarco.h"

__OPENCODE_BEGIN_NAMESPACE

class  IfxResultSetMetaData: public ResultSetMetaData
{
public: 
  virtual ~IfxResultSetMetaData();
  IfxResultSetMetaData(const vector<IfxColumn>& columns);
  
public:
  __XDBC_INTERFACE_RESULTSETMETADATA(ResultSetMetaData)
  
protected:
  void checkColumnIndex(__XDBC_HOOK_DECLARE_MORE int column) __XDBC_THROW_DECLARE(SQLException);

protected:
  vector<IfxColumn> __vector_columns;
  int __mi_column_count;

};

__OPENCODE_END_NAMESPACE

#endif
