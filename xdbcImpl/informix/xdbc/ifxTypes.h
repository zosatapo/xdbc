/**
 *  ifxTypes.h
 *
 *  Copyright (C) 2007-2012  zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef   __OPENCODE_XDBC_INFORMIX_XTYPES_H__
#define   __OPENCODE_XDBC_INFORMIX_XTYPES_H__

#include "xdbc/ifxdefs.h"

#include "port/ioc_map.h" 

__OPENCODE_BEGIN_NAMESPACE

class IfxTypes
{
private:
  IfxTypes();

public:
  static IfxTypes& getInstance();

public:
  int XDBC_2_Ifx(int xdbcType);

  int Ifx_2_XDBC(int ifxType);
  int Ifx_2_XDBC(const IfxColumn& column);

  string getTypeName(int dataType,bool isXDBC=true);
  string getTypeName(const IfxColumn& column);

private:
  static IfxTypes m_ifxTypes;

  OPENCODE_MAP<int,int> xdbc2IfDataTables;
  OPENCODE_MAP<int,int> ifx2XDBCTables;     
};

__OPENCODE_END_NAMESPACE

#endif
