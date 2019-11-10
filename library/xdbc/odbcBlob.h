/**
 *  odbcBlob.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef __OPENCODE_XDBC_ODBCBLOB_H__
#define __OPENCODE_XDBC_ODBCBLOB_H__

#include "xdbc/odbc.h"

__OPENCODE_BEGIN_NAMESPACE

class  __OPENCODE_XDBC_API_DECL OdbcBlob:public Blob
{

public:
  virtual ~OdbcBlob() {}
  OdbcBlob(){}

public:
  __XDBC_INTERFACE_BLOB(Blob)

public:
  string lob_stream;
};

__OPENCODE_END_NAMESPACE

#endif

