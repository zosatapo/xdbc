/**
 *  oradefs.h
 *
 *  Copyright (C) 2002-2020 zosatapo 
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef __OPENCODE_XDBC_ORACLE_CONFIG_H__
#define __OPENCODE_XDBC_ORACLE_CONFIG_H__

#include "xdbc/xdefine.h"
#include "xdbc/xdbc.h"
#include "xdbc/oramarco.h"
#include "oci.h"

#include "port/ioc_sbuf.h"
#include "port/ioc_vector.h"

__OPENCODE_BEGIN_NAMESPACE

//---------------------------------------------------------------

#define OPENCODE_HAS_STMTPREPARE2

//---------------------------------------------------------------
class OracleConnection;

class OracleStatement;
class OraclePreparedStatement;
class OracleCallableStatement;
class OracleArrayStatement;

class OracleResultSet;
class OracleArrayResultSet;

class OracleParameter
{
public:
  OracleParameter();
  OracleParameter(const OracleParameter& rv);
  OracleParameter& operator=(const OracleParameter& rv);

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

class OracleColumn
{
public:
  OracleColumn();
  OracleColumn(const OracleColumn& rv);
  OracleColumn& operator=(const OracleColumn& rv);

public:
  void dump(StringBuffer& __sb);

public:
  int __mi_index;
  
  int __mi_length;  
  int __mi_type;
  int __mi_precision;
  int __mi_scale;

  bool __mb_char_semantics;

  string __ms_name;
  string __ms_schema_name;
  string __ms_type_name;

  string __ms_table_name;
  int    __mi_nullable;
};

class OracleValue
{
public:
  OracleValue();
  OracleValue(const OracleValue& rv);
  OracleValue& operator=(const OracleValue& rv);

public:

  //used for datetime and interval datatypes 
  OCIDateTime*   ora_DATETIME;
  OCIInterval*   ora_INTERVAL;

  //used for LOB (OCI_DTYPE_LOB) or FILE (OCI_DTYPE_FILE) calls
  OCILobLocator* ora_LOB;

  //used for binding or defining ROWID values 
  OCIRowid*      ora_ROWID; 

  //used for SQLT_ODT
  OCIDate        ora_DATE;
    
 /** 
  * The OTS types: NUMBER, NUMERIC, INT, SHORTINT, REAL, DOUBLE PRECISION,
  * FLOAT and DECIMAL are represented by OCINumber.
  * The contents of OCINumber is opaque to clients. 
  *
  * For binding variables of type OCINumber in OCI calls (OCIBindByName(),
  * OCIBindByPos(), and OCIDefineByPos()) use the type code SQLT_VNU.
  */
  OCINumber           ora_NUMBER;
  
  union
  {
    XLONG              ora_BIGINT;
    UXLONG             ora_UBIGINT;
    float              ora_FLOAT; 
    double             ora_DOUBLE;     
  };
};


class OracleBind
{
public:
   OracleBind();
   OracleBind(const OracleBind& rv);
   OracleBind& operator=(const OracleBind& rv);

public:
   void cleanup(const OracleColumn& column);
   void cleanup(const OracleParameter& param);

public:
   int            __mi_index;
   string         string_value;
   OracleValue    ora_value;
   string         __tmp_std_str;
   
   OCIBind*       oci_bndp;

   sb2            __mi_bind_ind;
   ub2            __mi_bind_size;
   ub2            __mi_bind_code;  
   int            __mi_bind_type;
   
   bool           __mb_has_bound;
};


class OracleDefine
{
public:
   OracleDefine();
   OracleDefine(const OracleDefine& rv);
   OracleDefine& operator=(const OracleDefine& rv);

public:
   void cleanup(const OracleColumn& column);
   void cleanup(const OracleParameter& param);

public:
  int            __mi_index;
  OracleValue    ora_value;

  OCIDefine*     oci_defnp;

  int int_buf_offset;
  int __mi_buf_size;

  string __tmp_std_str;
   
  sb2            __mi_define_ind;
  ub2            __mi_define_size;
  ub2            __mi_define_code;
  int            __mi_define_type;

};

__OPENCODE_END_NAMESPACE

#endif
