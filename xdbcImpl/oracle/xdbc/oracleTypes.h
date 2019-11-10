/**
 *  oracleTypes.h
 *
 *  Copyright (C) 2002-2020 zosatapo 
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef   __OPENCODE_XDBC_ORACLE_TYPES_H__
#define   __OPENCODE_XDBC_ORACLE_TYPES_H__

#include "xdbc/oradefs.h"

#include "port/ioc_map.h" 

__OPENCODE_BEGIN_NAMESPACE

class OracleTypes
{
private:
  OracleTypes();

public:
  static OracleTypes& getInstance();

public:
  int XDBC_2_Oracle(int xdbcType);

  int Oracle_2_XDBC(int oracleType);
  int Oracle_2_XDBC(const OracleColumn& column);
  int Oracle_2_XDBC(int oracleType,int precision,int scale);

  string getTypeName(int dataType,bool isXDBC=true);
  string getTypeName(int oracleType,int precision,int scale);
  string getTypeName(const OracleColumn& column);

public:
  string getXDBCTypeName(int xdbcType);

  string getOracleTypeName(int oracleType,int precision,int scale);
  string getOracleTypeName(int oracleType);
  
private:
  OPENCODE_MAP<int,int> xdbc2OracleTables;
  OPENCODE_MAP<int,int> oracle2XDBCTables;

  OPENCODE_MAP<int,string> xdbc2TypeNames;
  OPENCODE_MAP<int,string> sqlt2TypeNames;

  static OracleTypes m_oracleTypes;
};

__OPENCODE_END_NAMESPACE

#endif
