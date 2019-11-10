/**
 *  ifxClob.h
 *
 *  Copyright (C) 2007-2012  zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef   __OPENCODE_XDBC_INFORMIX_XCLOB_H__
#define   __OPENCODE_XDBC_INFORMIX_XCLOB_H__

#include "xdbc/ifxdefs.h"
#include "xdbc/xmarco.h"

__OPENCODE_BEGIN_NAMESPACE

class IfxClob:public Clob 
{
public:
  friend class IfxPreparedStatement;
  friend class IfxCallableStatement;

public:
  virtual ~IfxClob();
  
  IfxClob(IfxConnection* ora_conn);
  IfxClob(IfxConnection* ora_conn,const string& lob_str);
  IfxClob(IfxConnection* ora_conn,struct ifx_lo_ts* p_ec_lo_ts);
  IfxClob(IfxConnection* ora_conn,struct tag_loc_t* p_ec_locator);
  
public:
  __XDBC_INTERFACE_CLOB(Clob)

public:
//-------------------------------------------------
//
//-------------------------------------------------
  virtual void swap(IfxClob& clob);

private:
  IfxConnection* __mp_conn;
  string  lob_stream;
};

__OPENCODE_END_NAMESPACE

#endif
