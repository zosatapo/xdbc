#include "xdbc/oracleBlob.h"
#include "xdbc/oracleConnection.h"
#include "xdbc/oracleConversion.h"

#include "port/ioc_sbuf.h"

__OPENCODE_BEGIN_NAMESPACE

//-------------------------------------------------------------------------

#define ORACLEBLOB_CLASS_NAME "OracleBlob"

OracleBlob::OracleBlob(OracleConnection* ora_conn):Blob()
{
  __mp_conn = ora_conn;
  ora_LOB = 0;

  __XDBC_HOOK_DEFINE();
  sword ret_code = OCIDescriptorAlloc((dvoid *)__mp_conn->oci_envhp, (dvoid **)&ora_LOB ,(ub4) OCI_DTYPE_LOB, (size_t) 0, (dvoid **) 0);
  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLEBLOB_CLASS_NAME);
  __XDBC_HOOK_CHECK(return);
  
  ub4 lobEmpty=0;
  ret_code = OCIAttrSet( (OCILobLocator*)ora_LOB, OCI_DTYPE_LOB, (ub4*)&lobEmpty, sizeof(ub4),OCI_ATTR_LOBEMPTY, __mp_conn->oci_errhp);
  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLEBLOB_CLASS_NAME);
}

OracleBlob::OracleBlob(OracleConnection* ora_conn, OCILobLocator*  pLob):Blob()
{
  __mp_conn = ora_conn;
  ora_LOB = 0;

  __XDBC_HOOK_DEFINE();
  sword ret_code = 0;  
  ret_code = OCIDescriptorAlloc((dvoid *)__mp_conn->oci_envhp, (dvoid **)&ora_LOB ,(ub4) OCI_DTYPE_LOB, (size_t) 0, (dvoid **) 0);
  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLEBLOB_CLASS_NAME);
  __XDBC_HOOK_CHECK(return);
  
  ub4 lobEmpty=0;
  ret_code = OCIAttrSet( (OCILobLocator*)ora_LOB, OCI_DTYPE_LOB, (ub4*)&lobEmpty, sizeof(ub4),OCI_ATTR_LOBEMPTY, __mp_conn->oci_errhp);
  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLEBLOB_CLASS_NAME);
  __XDBC_HOOK_CHECK(return);
  
  ret_code = OCILobAssign(__mp_conn->oci_envhp,__mp_conn->oci_errhp,(OCILobLocator*)pLob,(OCILobLocator**)&ora_LOB);
  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLEBLOB_CLASS_NAME);
}

OracleBlob::~OracleBlob()
{
  if(ora_LOB!=0)
  {
    OCIDescriptorFree((dvoid *) ora_LOB, OCI_DTYPE_LOB);
  }
}

int OracleBlob::length(__XDBC_HOOK_DECLARE_ONCE) const  __XDBC_THROW_DECLARE(SQLException)
{
  ub4 lenp = 0;
  sword ret_code = OCILobGetLength(__mp_conn->oci_svchp,__mp_conn->oci_errhp,ora_LOB,&lenp);
  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLEBLOB_CLASS_NAME);
  
  return lenp;
}


int OracleBlob::write(__XDBC_HOOK_DECLARE_MORE int pos, const char* str, int offset, int len)  __XDBC_THROW_DECLARE(SQLException)
{
  const char* p_start=str+offset;

  ub4 charWrite=len;
  sword ret_code = OCILobWrite(__mp_conn->oci_svchp,__mp_conn->oci_errhp,
                              ora_LOB, &charWrite, pos,
                              (void*)p_start, len,OCI_ONE_PIECE,
                              0, 0,
                              __mp_conn->__mi_env_cs, SQLCS_IMPLICIT);
  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLEBLOB_CLASS_NAME);
  
  return charWrite;
}

int OracleBlob::read(__XDBC_HOOK_DECLARE_MORE  int pos, char* str,int offset,int length) const  __XDBC_THROW_DECLARE(SQLException)
{
  ub4 byteRead = length;
  sword ret_code = OCILobRead(__mp_conn->oci_svchp,__mp_conn->oci_errhp,
                              ora_LOB, &byteRead, pos,
                              str+offset, length,
                              0, 0,
                              __mp_conn->__mi_env_cs, SQLCS_IMPLICIT);  
  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLEBLOB_CLASS_NAME);
  
  return byteRead;
}

void OracleBlob::truncate(__XDBC_HOOK_DECLARE_MORE int len)  __XDBC_THROW_DECLARE(SQLException)
{
  sword ret_code = OCILobTrim(__mp_conn->oci_svchp,__mp_conn->oci_errhp,ora_LOB,len); 
  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLEBLOB_CLASS_NAME);
}

void OracleBlob::swap(OracleBlob& blob)
{
  OracleConnection* p_tmp_conn=__mp_conn;
  OCILobLocator*    p_tmp_lob=ora_LOB;

  __mp_conn=blob.__mp_conn;
  ora_LOB=blob.ora_LOB;

  blob.__mp_conn=p_tmp_conn;
  blob.ora_LOB=p_tmp_lob;
}

__OPENCODE_END_NAMESPACE
