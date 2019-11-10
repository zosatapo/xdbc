/**
 *  ifxBlob.h
 *
 *  Copyright (C) 2007-2012  zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef   __OPENCODE_XDBC_INFORMIX_XBLOB_H__
#define   __OPENCODE_XDBC_INFORMIX_XBLOB_H__

#include "xdbc/ifxdefs.h"
#include "xdbc/xmarco.h"

__OPENCODE_BEGIN_NAMESPACE

class IfxBlob:public Blob
{
public:
  friend class IfxPreparedStatement;
  friend class IfxCallableStatement;

public:
  virtual ~IfxBlob();
  
  IfxBlob(IfxConnection* ora_conn);
  IfxBlob(IfxConnection* ora_conn,struct ifx_lo_ts* p_ec_lo_ts);
  IfxBlob(IfxConnection* ora_conn,struct tag_loc_t* p_ec_locator);

public:
  __XDBC_INTERFACE_BLOB(Blob)

public:
//-------------------------------------------------
//
//-------------------------------------------------
  virtual void swap(IfxBlob& blob);

private:
  IfxConnection* __mp_conn;
  string  lob_stream;
};

__OPENCODE_END_NAMESPACE

#endif
