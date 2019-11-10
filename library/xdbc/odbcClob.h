/**
 *  odbcClob.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef __OPENCODE_XDBC_ODBCCLOB_H__
#define __OPENCODE_XDBC_ODBCCLOB_H__

#include "xdbc/odbc.h"

__OPENCODE_BEGIN_NAMESPACE


class __OPENCODE_XDBC_API_DECL OdbcClob:public Clob
{

public:
  virtual ~OdbcClob() {}
  OdbcClob(){}

public:
  __XDBC_INTERFACE_CLOB(Clob)

public:
  string  lob_stream;

};

__OPENCODE_END_NAMESPACE

#endif

