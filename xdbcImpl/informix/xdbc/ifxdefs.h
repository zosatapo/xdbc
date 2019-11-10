/**
 *  ifxdefs.h
 *
 *  Copyright (C) 2007-2012  zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef __OPENCODE_XDBC_INFORMIX_CONFIG_H__
#define __OPENCODE_XDBC_INFORMIX_CONFIG_H__

#include <locator.h>

#include <decimal.h>
#include <datetime.h>
#include <int8.h>

#include <sqlca.h>
#include <sqlhdr.h>
#include <sqlstype.h>
#include <sqltypes.h>

#include "xdbc/xdefine.h"
#include "xdbc/xdbc.h"
#include "xdbc/ifxmarco.h"

#include "port/ioc_sbuf.h"
#include "port/ioc_vector.h"

__OPENCODE_BEGIN_NAMESPACE

//---------------------------------------------------------------
//
//---------------------------------------------------------------
class IfxConnection;

class IfxStatement;
class IfxPreparedStatement;
class IfxCallableStatement;
class IfxArrayStatement; 

class IfxResultSet;
class IfxArrayResultSet;

//----------------------------------------------------------
class IfxParameter
{
public:
  IfxParameter();
  IfxParameter(const IfxParameter& rv);
  IfxParameter& operator=(const IfxParameter& rv);

public:
  void dump(StringBuffer& __sb);

public:
  int __mi_index;

  int __mi_length;
  int __mi_type;
  int __mi_precision;
  int __mi_scale;
  int __mi_iomode; 

  string __ms_name;
  string __ms_schema_name;
  string __ms_type_name;
};

//----------------------------------------------------------
//----------------------------------------------------------
class IfxColumn
{
public:
  IfxColumn();
  IfxColumn(const IfxColumn& rv);
  IfxColumn& operator=(const IfxColumn& rv);

public:
  void dump(StringBuffer& __sb);

public:
  int __mi_index;
  
  int __mi_length;  
  int __mi_type;
  int __mi_xtype;
  int __mi_precision;
  int __mi_scale;

  bool __mb_char_semantics;

  string __ms_name;
  string __ms_schema_name;
  string __ms_type_name;

  string __ms_table_name;
  int   __mi_nullable;
};

//-----------------------------------------------------------
//-----------------------------------------------------------
class IfxValue
{
public:
  IfxValue();
  IfxValue(const IfxValue& rv);
  IfxValue& operator=(const IfxValue& rv);

//-----------------------------------------------------------
  //SQLDECIMAL,SQLMONEY
  struct decimal  ec_decimal;

  //SQLUDTFIXED(BOLB,CLOB)
  struct ifx_lo_ts  ec_lo_ts;
  struct ifx_lo_create_spec_s* p_ec_create_spec;
  int    int_ec_lofd;

  //SQLTEXT,SQLBYTES
  struct tag_loc_t ec_locator;

  //SQLDTIME
  struct dtime  ec_dtime;

  //SQLINTERVAL
  struct intrvl ec_interval;

  //SQLSERIAL8,SQLINT8
  struct ifx_int8  ec_int8;

//-----------------------------------------------------------
  union
  {
    mint      ec_SMINT;  
    int4      ec_INT;
    XLONG ec_SERIAL;    
    float     ec_FLOAT;
    double    ec_DOUBLE;
  };
};

//-----------------------------------------------------------
//-----------------------------------------------------------
class IfxBind
{
public:
   IfxBind();
   IfxBind(const IfxBind& rv);
   IfxBind& operator=(const IfxBind& rv);

public:
   void cleanup(const IfxColumn& column);
   void cleanup(const IfxParameter& param);

public:
   int      int_parameter_index;

   string    string_value;
   IfxValue  ifx_value;
   string    __tmp_std_str;
   
//-----------------------------------------------------------
  int int_buf_offset;
  int __mi_buf_size;

//-----------------------------------------------------------

//----------------------------------------
   int2           __mi_bind_ind;
   int            __mi_bind_size;
   int            __mi_bind_code;  

//----------------------------------------
   int            __mi_bind_type;
   bool           __mb_has_bound;
};

//-------------------------------------------------------------
//-------------------------------------------------------------
class IfxDefine
{
public:
   IfxDefine();
   IfxDefine(const IfxDefine& rv);
   IfxDefine& operator=(const IfxDefine& rv);

public:
   void cleanup(const IfxColumn& column);
   void cleanup(const IfxParameter& param);

public:
  int            __mi_index;

//-----------------------------------------------------------
  IfxValue  ifx_value;
  
//-----------------------------------------------------------
  int int_buf_offset;
  int __mi_buf_size;

//-----------------------------------------------------------  
  string __tmp_std_str;
  
//-----------------------------------------------------------
  int            __mi_define_size;
  int2           __mi_define_ind;
  int            __mi_define_code;
};
__OPENCODE_END_NAMESPACE

#endif
