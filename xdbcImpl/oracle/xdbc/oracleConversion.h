/**
 *  oracleConversion.h
 *
 *  Copyright (C) 2002-2020 zosatapo 
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef __OPENCODE_XDBC_ORACLE_CONVERSION_H__
#define __OPENCODE_XDBC_ORACLE_CONVERSION_H__

#include "xdbc/oradefs.h"

__OPENCODE_BEGIN_NAMESPACE

class OracleConversion
{
private:
  OracleConversion();

public:
  static void   makeDate(const XDBC_TIMESTAMP& dat,char* p_date);
  static void   makeDate(const XDBC_TIMESTAMP& dat,OCIDate* p_date);

  static void   makeDate(const XDBC_DATE& dat,char* p_date);
  static void   makeDate(const XDBC_DATE& dat,OCIDate* p_date);

  static void   makeDate(const XDBC_TIME& dat,char* p_date);
  static void   makeDate(const XDBC_TIME& dat,OCIDate* p_date);

  static void   makeInterval(__XDBC_HOOK_DECLARE_MORE OracleConnection* p_conn,const XDBC_INTERVAL& interval,OCIInterval* ora_INTERVAL)  __XDBC_THROW_DECLARE(SQLException);

  static bool   parseDate(const char* p_date,XDBC_DATE& dat);
  static bool   parseTime(const char* p_time,XDBC_TIME& time);
  static bool   parseTimestamp(const char* p_timestamp,XDBC_TIMESTAMP& timestamp);
   
  static bool   parseDate(__XDBC_HOOK_DECLARE_MORE OracleConnection* p_conn,OCIDate* p_date,XDBC_DATE& dat)  __XDBC_THROW_DECLARE(SQLException);
  static bool   parseTime(__XDBC_HOOK_DECLARE_MORE OracleConnection* p_conn,OCIDate* p_date,XDBC_TIME& time)  __XDBC_THROW_DECLARE(SQLException);
  static bool   parseTimestamp(__XDBC_HOOK_DECLARE_MORE OracleConnection* p_conn,OCIDate* p_date,XDBC_TIMESTAMP& timestamp)  __XDBC_THROW_DECLARE(SQLException);
 
  static bool   parseDate(__XDBC_HOOK_DECLARE_MORE OracleConnection* p_conn,OCIDateTime* ora_DATETIME,int oracleType,XDBC_DATE& dat)  __XDBC_THROW_DECLARE(SQLException);
  static bool   parseTime(__XDBC_HOOK_DECLARE_MORE OracleConnection* p_conn,OCIDateTime* ora_DATETIME,int oracleType,XDBC_TIME& time)  __XDBC_THROW_DECLARE(SQLException);
  static bool   parseTimestamp(__XDBC_HOOK_DECLARE_MORE OracleConnection* p_conn,OCIDateTime* ora_DATETIME,int oracleType,XDBC_TIMESTAMP& timestamp)  __XDBC_THROW_DECLARE(SQLException);

  static bool   parseInterval(__XDBC_HOOK_DECLARE_MORE OracleConnection* p_conn,OCIInterval* ora_INTERVAL,int oracleType,XDBC_INTERVAL& interval) __XDBC_THROW_DECLARE(SQLException);

public:
  //get Descriptor type code from SQLT 
  static ub4         SQLT_TO_DTYPE(ub2 sqlt);
  static int         SQLT_TO_XDBC(ub2 sqlt);
  static int         SQLT_TO_XDBC(const OracleColumn& column);

  static ub2         XDBC_TO_SQLT(int xtype);

  static ub2         parseIntervalType(const XDBC_INTERVAL& interval);
  
public:
  static void checkException(__XDBC_HOOK_DECLARE_MORE OCIError *errhp,sword status,const char* className = 0) __XDBC_THROW_DECLARE(SQLException);

public:
  static void parseResultSetMetaData(__XDBC_HOOK_DECLARE_MORE OCIError* oci_errhp,OCIStmt* oci_stmthp,vector<OracleColumn>& __vector_columns) __XDBC_THROW_DECLARE(SQLException);
};

__OPENCODE_END_NAMESPACE

#endif
