/**
 *  ifxConversion.h
 *
 *  Copyright (C) 2007-2012  zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef __OPENCODE_XDBC_INFORMIX_CONVERSION_H__
#define __OPENCODE_XDBC_INFORMIX_CONVERSION_H__

#include "xdbc/ifxdefs.h"
#include "xdbc/xdbc.h"
#include "xdbc/ifxResultSet.h" 
#include "port/ioc_vector.h"

__OPENCODE_BEGIN_NAMESPACE

class IfxConversion
{
private:
  IfxConversion();

public:
  static int DATABASE_TO_XDBC(int sqlt);
  static int DATABASE_TO_XDBC(const IfxColumn& column);

  static int XDBC_TO_DATABASE(int xtype);

public:
  static void parseResultSetMetaData(struct sqlda*  __mp_ec_sqlda_columns,vector<IfxColumn>& __vector_columns);

  static void prepareColumnData(__XDBC_HOOK_DECLARE_MORE struct sqlda*  __mp_ec_sqlda_columns,
                                const vector<IfxColumn>& __vector_columns,
                                vector<IfxDefine>& __vector_defines,
                                char*& p_data_buf,int& __mi_buf_size,
                                IfxResultSetCollection* __cachedCollection,
                                bool __adoptIt)  __XDBC_THROW_DECLARE(SQLException);

  static void prepareVariableData(__XDBC_HOOK_DECLARE_MORE IfxConnection* __mp_conn,struct sqlda*  __mp_ec_sqlda_vars,const vector<IfxColumn>& __vector_vars,vector<IfxBind>& __vector_binds,char*& p_data_buf,int& __mi_buf_size) __XDBC_THROW_DECLARE(SQLException);

  static bool checkConversion(__XDBC_HOOK_DECLARE_MORE const string& className,char* rowBuffer,IfxDefine& ifxDefine,int inType,int outType,char* outBuffer,const string& columnName) __XDBC_THROW_DECLARE(SQLException);

public:
  static bool  parseDateTime(struct dtime* p_dtime,XDBC_TIMESTAMP& ts);
  static void  makeDateTime(const XDBC_TIMESTAMP& date,struct dtime* p_dtime);
  static void  makeDateTime(const XDBC_DATE& date,struct dtime* p_dtime);
  static void  makeDateTime(const XDBC_TIME& date,struct dtime* p_dtime);

  static bool  parseInterval(struct intrvl* p_interval,XDBC_INTERVAL& interval);
  static void  makeInterval(const XDBC_INTERVAL& interval,struct intrvl* p_interval);

public:
  static void  int8ToLong(struct ifx_int8* p_ec_int8,XLONG& intLong);
  static void  int8ToString(struct ifx_int8* p_ec_int8,string& strLong);
  static void  longToint8(const XLONG& intLong,struct ifx_int8* p_ec_int8);

  static void  decimalToLong(struct decimal* p_ec_decimal,XLONG& intLong);
  static void  decimalToString(struct decimal* p_ec_decimal,string& strLong);
  static void  longToDecimal(const XLONG& intLong,struct decimal* p_ec_decimal);

  static string toString(struct dtime* p_dtime);
  static string toString(struct intrvl* p_interval);

public:
  static void checkException(__XDBC_HOOK_DECLARE_MORE int sql_code,const string& sql_state,const string& className) __XDBC_THROW_DECLARE(SQLException);

protected:
  static void formatDateTimeInterval(const vector<int>& fields,int qual,char* buf); 
  static void fixDateTimeInterval(vector<int>& fields,int qual,const vector<int>& fvalues);
};

__OPENCODE_END_NAMESPACE

#endif
