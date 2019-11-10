/**
 *  ifxResultSet.h
 *
 *  Copyright (C) 2007-2012  zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef   __OPENCODE_XDBC_INFORMIX_RESULTSET_H__
#define   __OPENCODE_XDBC_INFORMIX_RESULTSET_H__

#include "xdbc/ifxdefs.h"
#include "xdbc/xmarco.h"
#include "port/ioc_map.h" 
#include "port/ioc_vector.h"

__OPENCODE_BEGIN_NAMESPACE

class IfxResultSetCollection : public ResultSetCollection
{ 
public:
  virtual ~IfxResultSetCollection() ;
  
  IfxResultSetCollection();
  IfxResultSetCollection(const IfxResultSetCollection& rv);
  IfxResultSetCollection& operator=(const IfxResultSetCollection& rv);

public:
  char* __p_rowbuf;
  int   __rowbuf_size;    
  
  vector<IfxDefine> __vector_defines;
  vector<IfxColumn> __vector_columns;
  OPENCODE_MAP<string,int>   __map_columns;
};

//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------


class IfxResultSet : public ResultSet
{
public:
  friend class IfxStatement;
  friend class IfxResultSetMetaData;

public:
  virtual ~IfxResultSet();
  
  IfxResultSet(IfxConnection* pConn,SQLImplementation* __mp_impl,const string& cursor_name,const string& cursor_id,int resultSetType, int resultSetConcurrency,bool close_stmt,ResultSetCollection* cachedCollection);

public:
  __XDBC_INTERFACE_RESULTSET(ResultSet)

public:  
//-------------------------------------------------
// IfxBase Interfaces
//-------------------------------------------------
  virtual void initialize(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException);

protected:
  bool   checkConversion(__XDBC_HOOK_DECLARE_MORE int columnIndex,int outType,char* outBuffer) __XDBC_THROW_DECLARE(SQLException);

private:
  SQLImplementation* __mp_impl; 
  IfxConnection* __mp_conn;
  
  int __mi_resultSetType;
  int __mi_resultSetConcurrency;
//---------------------------------------
// INFORMIX-ESQLC
//---------------------------------------
  struct sqlda*  __mp_ec_sqlda_columns;   /* sqlda struct pointer to Column */

  string __ms_cursor_name;
  string __ms_cursor_id;
  
//-------------------------------------------
// STATE  Information
//-------------------------------------------
  bool __mb_isEmpty;
  bool __mb_isClosed;
  int  __mi_cur_row;
  int  __mi_row_count;
  
  bool __mb_close_stmt;

//-------------------------------------------
// ROW DATA BUFFER
//-------------------------------------------
  char* __mp_row_buffer;
  int   __mi_buf_size;

  vector<IfxDefine> __vector_defines;

//-------------------------------------------
// COLUMN INFO DATA
//-------------------------------------------
  vector<IfxColumn>  __vector_columns; //column metadata

//-------------------------------------------
// ResultSetCollection 
//-------------------------------------------
  IfxResultSetCollection* __cachedCollection;
  int  __mi_column_count;
  bool __adoptIt;
  
public:
  static bool isSupported(__XDBC_HOOK_DECLARE_MORE int resultSetType, int resultSetConcurrency)  __XDBC_THROW_DECLARE(SQLException);
};

__OPENCODE_END_NAMESPACE

#endif
