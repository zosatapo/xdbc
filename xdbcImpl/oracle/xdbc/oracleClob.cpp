#include "xdbc/oracleClob.h"
#include "xdbc/oracleConnection.h"
#include "xdbc/oracleConversion.h"

#include "port/ioc_sbuf.h"

__OPENCODE_BEGIN_NAMESPACE

#define ORACLECLOB_CLASS_NAME "OracleClob"

OracleClob::OracleClob(OracleConnection* ora_conn):Clob()
{
  __mp_conn=ora_conn;
  ora_LOB=0;

  __XDBC_HOOK_DEFINE();
  sword ret_code = 0;
  ret_code = OCIDescriptorAlloc((dvoid *)__mp_conn->oci_envhp, (dvoid **)&ora_LOB ,(ub4) OCI_DTYPE_LOB, (size_t) 0, (dvoid **) 0);
  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLECLOB_CLASS_NAME);
  __XDBC_HOOK_CHECK(return);
  
  ub4 lobEmpty = 0;
  ret_code = OCIAttrSet( (OCILobLocator*)ora_LOB, OCI_DTYPE_LOB, (ub4*)&lobEmpty, sizeof(ub4), OCI_ATTR_LOBEMPTY, __mp_conn->oci_errhp);
  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLECLOB_CLASS_NAME);
}

OracleClob::OracleClob(OracleConnection* ora_conn, OCILobLocator*  pLob):Clob()
{
  __mp_conn=ora_conn;
  ora_LOB=0;

  __XDBC_HOOK_DEFINE();  
  sword ret_code = 0;
  ret_code = OCIDescriptorAlloc((dvoid *)__mp_conn->oci_envhp, (dvoid **)&ora_LOB ,(ub4) OCI_DTYPE_LOB, (size_t) 0, (dvoid **) 0);
  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLECLOB_CLASS_NAME);
  __XDBC_HOOK_CHECK(return);
  
  ub4 lobEmpty=0;
  ret_code = OCIAttrSet( (OCILobLocator*)ora_LOB, OCI_DTYPE_LOB, (ub4*)&lobEmpty, sizeof(ub4),OCI_ATTR_LOBEMPTY, __mp_conn->oci_errhp);
  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLECLOB_CLASS_NAME);
  __XDBC_HOOK_CHECK(return);
  
  ret_code = OCILobAssign(__mp_conn->oci_envhp,__mp_conn->oci_errhp,(OCILobLocator*)pLob,(OCILobLocator**)&ora_LOB);
  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLECLOB_CLASS_NAME);
}

OracleClob::~OracleClob()
{
  if(ora_LOB!=0)
  {
    OCIDescriptorFree((dvoid *) ora_LOB, OCI_DTYPE_LOB); 
  }
}

int OracleClob::length(__XDBC_HOOK_DECLARE_ONCE) const  __XDBC_THROW_DECLARE(SQLException)
{
  ub4 lenp=0;
  sword ret_code = OCILobGetLength(__mp_conn->oci_svchp,__mp_conn->oci_errhp,ora_LOB,&lenp);
  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLECLOB_CLASS_NAME);
  __XDBC_HOOK_CHECK(return 0);
  
  return lenp;
}

int OracleClob::write(__XDBC_HOOK_DECLARE_MORE int pos, const string& str)  __XDBC_THROW_DECLARE(SQLException)
{
  const char* p_char=str.c_str();
  return write(__XDBC_HOOK_INVOKE_MORE pos,p_char,0,strlen(p_char));
}

int OracleClob::write(__XDBC_HOOK_DECLARE_MORE int pos, const char* str, int offset, int len)  __XDBC_THROW_DECLARE(SQLException)
{
  const char* p_start = str + offset;
  ub4 charWrite = len;
  
  sword ret_code = OCILobWrite(__mp_conn->oci_svchp,__mp_conn->oci_errhp,
                              ora_LOB, &charWrite, pos,
                              (void*)p_start, len,OCI_ONE_PIECE,
                              0, 0,
                              __mp_conn->__mi_env_cs, SQLCS_IMPLICIT);

  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLECLOB_CLASS_NAME);
 
  return charWrite;
}

int OracleClob::read(__XDBC_HOOK_DECLARE_MORE int pos, char* str,int offset,int length) const  __XDBC_THROW_DECLARE(SQLException)
{
  ub4 charRead=length;
  sword ret_code = OCILobRead(__mp_conn->oci_svchp,__mp_conn->oci_errhp,
                                            ora_LOB, &charRead, pos,
                                            str+offset, length,
                                            0, 0,
                                            __mp_conn->__mi_env_cs, SQLCS_IMPLICIT);  
  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLECLOB_CLASS_NAME);
  
  return charRead;
}

void OracleClob::truncate(__XDBC_HOOK_DECLARE_MORE int len)  __XDBC_THROW_DECLARE(SQLException)
{
  sword ret_code = OCILobTrim(__mp_conn->oci_svchp,__mp_conn->oci_errhp,ora_LOB,len); 
  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLECLOB_CLASS_NAME);
}

void OracleClob::swap(OracleClob& clob)
{
  OracleConnection* p_tmp_conn=__mp_conn;
  OCILobLocator*    p_tmp_lob=ora_LOB;

  __mp_conn=clob.__mp_conn;
  ora_LOB=clob.ora_LOB;

  clob.__mp_conn=p_tmp_conn;
  clob.ora_LOB=p_tmp_lob;
}

__OPENCODE_END_NAMESPACE
