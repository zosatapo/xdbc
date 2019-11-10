/**
 *  odbcTypes.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef __OPENCODE_XDBC_ODBCTYPES_H__
#define __OPENCODE_XDBC_ODBCTYPES_H__

#include "xdbc/odbc.h"
#include "port/lifetime.h"
#include "port/ioc_map.h" 

__OPENCODE_BEGIN_NAMESPACE

class __OPENCODE_XDBC_API_DECL OdbcTypes
{
  DECLARE_LIFETIME_BEGIN(OdbcTypes)
  DECLARE_LIFETIME_END(OdbcTypes)
  
public:
  OdbcTypes();

  int xdbc_cnvt_odbc(struct ODBC_DRIVER_OPTIONS* odbcOptions,int xdbcType);
  
  int odbc_cnvt_xdbc(struct ODBC_DRIVER_OPTIONS* odbcOptions,int odbcType);
  int odbc_cnvt_xdbc(struct ODBC_DRIVER_OPTIONS* odbcOptions,const ODBC_COLUMN& column);
  int odbc_cnvt_xdbc(struct ODBC_DRIVER_OPTIONS* odbcOptions,int odbcType,int precision,int scale);

  string getTypeName(struct ODBC_DRIVER_OPTIONS* odbcOptions,int odbcType);
  string getTypeName(struct ODBC_DRIVER_OPTIONS* odbcOptions,const ODBC_COLUMN& column);
  string getTypeName(struct ODBC_DRIVER_OPTIONS* odbcOptions,int odbcType,int precision,int scale);  
  
private:
  OPENCODE_MAP<int,int> xdbc2ODBCTables;
  OPENCODE_MAP<int,int> odbc2XDBCTables;

  OPENCODE_MAP<int,string> xdbc2TypeNames;
  OPENCODE_MAP<int,string> odbc2TypeNames;
};

typedef SingletonHolder<OdbcTypes,CreateUsingNew<OdbcTypes>,SingletonWithLongevity<OdbcTypes>,MultiThread<OdbcTypes> > OdbcTypesImpl;
__OPENCODE_INLINE unsigned int getLongevity(OdbcTypes* impl){return __XDBC_LONGEVITY_ODBCTYPES;}

__OPENCODE_END_NAMESPACE

#endif

