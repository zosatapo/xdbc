#include "xdbc/oracleCallableStatement.h"
#include "xdbc/oracleClob.h"
#include "xdbc/oracleBlob.h"

#include "xdbc/oracleConnection.h"
#include "xdbc/oracleConversion.h"
#include "xdbc/oracleTypes.h"
#include "xdbc/xsmart.h"

#include "port/ioc_sbuf.h"
#include "port/file.h"

__OPENCODE_BEGIN_NAMESPACE

#define ORACLECALLABLESTATEMENT_CLASS_NAME "OracleCallableStatement"

OracleCallableStatement::~OracleCallableStatement()
{
  __XDBC_TRACE(ORACLECALLABLESTATEMENT_CLASS_NAME,"destroy()");
}

OracleCallableStatement::OracleCallableStatement(OracleConnection* ora_conn,const string& sql,int paramCount)
{
  __sp_pstmt.reset(new OraclePreparedStatement(ora_conn,sql,paramCount));
  __mi_call_param_count = 0;
}

OracleCallableStatement::OracleCallableStatement(OracleConnection* ora_conn,const string& sql, int resultSetType, int resultSetConcurrency,int paramCount)
{
  __sp_pstmt.reset(new OraclePreparedStatement(ora_conn,sql,resultSetType,resultSetConcurrency,paramCount));
  __mi_call_param_count = 0;
}

//-------------------------------------------------
// Oracle Interfaces
//-------------------------------------------------
void OracleCallableStatement::initialize(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{
//------------------------------------------------------------
// 0. call base function 
//------------------------------------------------------------
  __sp_pstmt->initialize(__XDBC_HOOK_INVOKE_ONCE);
  __XDBC_HOOK_CHECK(return);
  
//------------------------------------------------------------
// 1. parse procedure name
//------------------------------------------------------------  
  parseProcName(__XDBC_HOOK_INVOKE_ONCE);
  __XDBC_HOOK_CHECK(return);
  
//------------------------------------------------------------
// 2. retrive param info 
//------------------------------------------------------------
  text* objptr = (text*)__ms_proc_name.c_str(); /* the name of a procedure to be described */
  ub4 objp_len = (ub4)strlen((char *)objptr);

  ub2 numargs = 0;
  OCIParam *parmh = (OCIParam *) 0;         /* parameter handle */
  OCIParam *arglst = (OCIParam *) 0;          /* list of args */
  OCIParam *arg = (OCIParam *) 0;             /* param handle */
  OCIDescribe *dschp = (OCIDescribe *)0;      /* describe handle */

  OCIHandleAlloc((dvoid *)__sp_pstmt->__mp_conn->oci_envhp, (dvoid **)&dschp, (ub4)OCI_HTYPE_DESCRIBE, (size_t)0, (dvoid **)0);

  __mb_stored_proc = true;
  sword ret_code = 0;
 
  __OPENCODE_TRY
  {
    /* get the describe handle for the procedure */
    ret_code = OCIDescribeAny(__sp_pstmt->__mp_conn->oci_svchp,__sp_pstmt->__mp_conn->oci_errhp,
            (dvoid *)objptr,objp_len,OCI_OTYPE_NAME, 0, OCI_PTYPE_PROC, dschp);
    OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __sp_pstmt->__mp_conn->oci_errhp,ret_code,ORACLECALLABLESTATEMENT_CLASS_NAME);  
    if(!__XDBC_HOOK_INVOKE_CHECKPASSED)
    {
      /* get the describe handle for the function */
      ret_code = OCIDescribeAny(__sp_pstmt->__mp_conn->oci_svchp,__sp_pstmt->__mp_conn->oci_errhp,
              (dvoid *)objptr,objp_len,OCI_OTYPE_NAME, 0, OCI_PTYPE_FUNC, dschp);
      OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __sp_pstmt->__mp_conn->oci_errhp,ret_code,ORACLECALLABLESTATEMENT_CLASS_NAME);
      __XDBC_HOOK_CHECK(return);
      
      __mb_stored_proc = false;
    }      
  }
  __OPENCODE_CATCH(SQLException& ex)
  {
    /* get the describe handle for the function */
    ret_code = OCIDescribeAny(__sp_pstmt->__mp_conn->oci_svchp,__sp_pstmt->__mp_conn->oci_errhp,
            (dvoid *)objptr,objp_len,OCI_OTYPE_NAME, 0, OCI_PTYPE_FUNC, dschp);
    OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __sp_pstmt->__mp_conn->oci_errhp,ret_code,ORACLECALLABLESTATEMENT_CLASS_NAME);
    __XDBC_HOOK_CHECK(return);

    __mb_stored_proc = false;
  } 
 
  if(DriverManager::isTracingOn)
  {
     StringBuffer __sb;
     __sb.append("parseProcName|name=");
     __sb.append(__ms_proc_name);
     __sb.append(",isStoredProc=");
     __sb.append(__mb_stored_proc);

     __XDBC_FORCED_TRACE(ORACLECALLABLESTATEMENT_CLASS_NAME,__sb.str());
  }
 
  /* get the parameter handle */
  ret_code = OCIAttrGet((dvoid *)dschp, OCI_HTYPE_DESCRIBE, (dvoid *)&parmh,
             (ub4 *)0, OCI_ATTR_PARAM, __sp_pstmt->__mp_conn->oci_errhp);
  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __sp_pstmt->__mp_conn->oci_errhp,ret_code,ORACLECALLABLESTATEMENT_CLASS_NAME);
  __XDBC_HOOK_CHECK(return);
  
  /* Get the number of params and the arg list */
  ret_code = OCIAttrGet((dvoid *)parmh, OCI_DTYPE_PARAM, (dvoid *)&arglst, 
             (ub4 *)0, OCI_ATTR_LIST_ARGUMENTS, __sp_pstmt->__mp_conn->oci_errhp);
  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __sp_pstmt->__mp_conn->oci_errhp,ret_code,ORACLECALLABLESTATEMENT_CLASS_NAME);
  __XDBC_HOOK_CHECK(return);
  
  ret_code = OCIAttrGet((dvoid *)arglst, OCI_DTYPE_PARAM, (dvoid *)&numargs,
             (ub4 *)0, OCI_ATTR_NUM_PARAMS, __sp_pstmt->__mp_conn->oci_errhp);
  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __sp_pstmt->__mp_conn->oci_errhp,ret_code,ORACLECALLABLESTATEMENT_CLASS_NAME);
  __XDBC_HOOK_CHECK(return);
  
  text*             namep;
  ub4               sizep;
  ub2               collen;
  ub2               coltyp;
  OCITypeParamMode  iomode;
  ub1               precision;
  sb1               scale;

  //-------------------------------------------------------------
  //
  //-------------------------------------------------------------
  __mi_call_param_count = numargs;
  __vector_call_params.resize(numargs + 1);

  StringBuffer __sb;
  __sb << "<init>|" << File::lineSeparator;
  
  /* For a procedure, we begin with i = 1; for a function, we begin with i = 0. */
  for (int i = 1;  i<= numargs; i++)
  {
    if(__mb_stored_proc)
    { 
      /* get procedure parameter for param i */
      ret_code = OCIParamGet ((dvoid *)arglst, OCI_DTYPE_PARAM, __sp_pstmt->__mp_conn->oci_errhp, (dvoid **)&arg, (ub4)i);
      OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __sp_pstmt->__mp_conn->oci_errhp,ret_code,ORACLECALLABLESTATEMENT_CLASS_NAME);
      __XDBC_HOOK_CHECK(return);
    }
    else
    {
      /* get function parameter for param i-1 */
      ret_code = OCIParamGet ((dvoid *)arglst, OCI_DTYPE_PARAM, __sp_pstmt->__mp_conn->oci_errhp, (dvoid **)&arg, (ub4)(i-1));
      OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __sp_pstmt->__mp_conn->oci_errhp,ret_code,ORACLECALLABLESTATEMENT_CLASS_NAME);
      __XDBC_HOOK_CHECK(return);
    }

    ret_code = OCIAttrGet ((dvoid*) arg, (ub4) OCI_DTYPE_PARAM,
              (dvoid*) &iomode, (ub4 *) 0,
              (ub4) OCI_ATTR_IOMODE, (OCIError *)__sp_pstmt->__mp_conn->oci_errhp);
    OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __sp_pstmt->__mp_conn->oci_errhp,ret_code,ORACLECALLABLESTATEMENT_CLASS_NAME);
    __XDBC_HOOK_CHECK(return);
    
/*
    if(iomode != OCI_TYPEPARAM_OUT  && !__mb_stored_proc)
    {
      continue;
    }
*/

    __vector_call_params[i] = new OracleParameter();
    __vector_call_params[i]->__mi_index = i;
    __vector_call_params[i]->__mi_iomode=iomode;

    /* column length */
    sizep=sizeof(collen);
    ret_code = OCIAttrGet((dvoid*) arg, (ub4) OCI_DTYPE_PARAM,
              (dvoid*) &collen, (ub4 *) &sizep,
              (ub4) OCI_ATTR_DATA_SIZE, (OCIError *)__sp_pstmt->__mp_conn->oci_errhp);

    OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __sp_pstmt->__mp_conn->oci_errhp,ret_code,ORACLECALLABLESTATEMENT_CLASS_NAME);
    __XDBC_HOOK_CHECK(return);
    
    __vector_call_params[i]->__mi_length=collen;
    

    ret_code = OCIAttrGet((dvoid*) arg, (ub4) OCI_DTYPE_PARAM,
              (dvoid*) &namep, (ub4 *) &sizep,
              (ub4) OCI_ATTR_NAME, (OCIError *)__sp_pstmt->__mp_conn->oci_errhp);
    OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __sp_pstmt->__mp_conn->oci_errhp,ret_code,ORACLECALLABLESTATEMENT_CLASS_NAME);
    __XDBC_HOOK_CHECK(return);
    
    char* ptemp= 0 ; 

    if(sizep > 0)
    {
      ptemp=new char[sizep+1];
      strncpy ((char *)ptemp, (char *)namep, (size_t) sizep);
      ptemp[sizep] = '\0';
      __vector_call_params[i]->__ms_name.append((char*)ptemp);
      delete []ptemp;
    }

    ret_code = OCIAttrGet((dvoid*) arg, (ub4) OCI_DTYPE_PARAM,
             (dvoid*) &namep, (ub4 *) &sizep,
             (ub4) OCI_ATTR_SCHEMA_NAME, (OCIError *)__sp_pstmt->__mp_conn->oci_errhp);
    OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __sp_pstmt->__mp_conn->oci_errhp,ret_code,ORACLECALLABLESTATEMENT_CLASS_NAME);
    __XDBC_HOOK_CHECK(return);
    
    if(sizep > 0)
    {
      ptemp = new char[sizep+1];
      strncpy ((char *)ptemp, (char *)namep, (size_t) sizep);
      ptemp[sizep] = '\0';
      __vector_call_params[i]->__ms_schema_name.append((char*)ptemp);
      delete []ptemp;
    }

    ret_code = OCIAttrGet((dvoid*) arg, (ub4) OCI_DTYPE_PARAM,
               (dvoid*) &namep, (ub4 *) &sizep,
               (ub4) OCI_ATTR_TYPE_NAME, (OCIError *)__sp_pstmt->__mp_conn->oci_errhp);
    OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __sp_pstmt->__mp_conn->oci_errhp,ret_code,ORACLECALLABLESTATEMENT_CLASS_NAME);
    __XDBC_HOOK_CHECK(return);
    
    if(sizep > 0)
    {
      ptemp=new char[sizep+1];
      strncpy ((char *)ptemp, (char *)namep, (size_t) sizep);
      ptemp[sizep] = '\0';
      __vector_call_params[i]->__ms_type_name.append((char*)ptemp);
      delete []ptemp;
    }

    ret_code = OCIAttrGet((dvoid*) arg, (ub4) OCI_DTYPE_PARAM,
               (dvoid*) &coltyp, (ub4 *) 0,
               (ub4) OCI_ATTR_DATA_TYPE, (OCIError *)__sp_pstmt->__mp_conn->oci_errhp);
    OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __sp_pstmt->__mp_conn->oci_errhp,ret_code,ORACLECALLABLESTATEMENT_CLASS_NAME);
    __XDBC_HOOK_CHECK(return);
    
    __vector_call_params[i]->__mi_type = coltyp;

    ret_code = OCIAttrGet ((dvoid*) arg, (ub4) OCI_DTYPE_PARAM,
              (dvoid*) &precision, (ub4 *) 0,
              (ub4) OCI_ATTR_PRECISION, (OCIError *)__sp_pstmt->__mp_conn->oci_errhp);
    OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __sp_pstmt->__mp_conn->oci_errhp,ret_code,ORACLECALLABLESTATEMENT_CLASS_NAME);
    __XDBC_HOOK_CHECK(return);
    
    __vector_call_params[i]->__mi_precision=precision;

    ret_code = OCIAttrGet ((dvoid*) arg, (ub4) OCI_DTYPE_PARAM,
              (dvoid*) &scale, (ub4 *) 0,
              (ub4) OCI_ATTR_SCALE, (OCIError *)__sp_pstmt->__mp_conn->oci_errhp);
    OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __sp_pstmt->__mp_conn->oci_errhp,ret_code,ORACLECALLABLESTATEMENT_CLASS_NAME);
    __XDBC_HOOK_CHECK(return);
    
    __vector_call_params[i]->__mi_scale=scale;

    if(iomode == OCI_TYPEPARAM_OUT )
    {    
      if(__vector_call_params[i]->__mi_type == SQLT_NUM)
      {
         __vector_call_params[i]->__mi_type = SQLT_VNU;
      }


      if(__vector_call_params[i]->__mi_length == 0)
      {
//=======================================================
        if(__vector_call_params[i]->__mi_type == SQLT_CHR)
        {
          __vector_call_params[i]->__mi_length=SQLT_CHR_BYTE_MAX;
        }
        else if(__vector_call_params[i]->__mi_type == SQLT_AVC)
        {
          __vector_call_params[i]->__mi_length=SQLT_AVC_BYTE_MAX;
        }
        else if(__vector_call_params[i]->__mi_type == SQLT_AFC)
        {
          __vector_call_params[i]->__mi_length=SQLT_AFC_BYTE_MAX;
        }
        else if(__vector_call_params[i]->__mi_type == SQLT_VCS)
        {
          __vector_call_params[i]->__mi_length=SQLT_VCS_BYTE_MAX + SHORT_PREFIX_LENGTH;
        }
        else if(__vector_call_params[i]->__mi_type == SQLT_VBI)
        {
          __vector_call_params[i]->__mi_length=SQLT_VBI_BYTE_MAX + SHORT_PREFIX_LENGTH;
        }
        else if(__vector_call_params[i]->__mi_type == SQLT_LVC)
        {
          __vector_call_params[i]->__mi_length=SQLT_LVC_BYTE_MAX + INTEGER_PREFIX_LENGTH;
        }
        else if(__vector_call_params[i]->__mi_type == SQLT_LVB)
        {
          __vector_call_params[i]->__mi_length=SQLT_LVB_BYTE_MAX + INTEGER_PREFIX_LENGTH;
        }
//=======================================================
       }  
     }

    __vector_call_params[i]->__ms_type_name = OracleTypes::getInstance().getTypeName( __vector_call_params[i]->__mi_type, __vector_call_params[i]->__mi_precision, __vector_call_params[i]->__mi_scale); 

    if(DriverManager::isTracingOn)
    {
      __vector_call_params[i]->dump(__sb);
    }

  }


  __XDBC_TRACE(ORACLECALLABLESTATEMENT_CLASS_NAME,__sb.str());

    
  if (dschp != 0)
  {
    OCIHandleFree((dvoid *) dschp, OCI_HTYPE_DESCRIBE);
  }
}

void OracleCallableStatement::registerOutParameter(__XDBC_HOOK_DECLARE_MORE int paramIndex, int sqlType)  __XDBC_THROW_DECLARE(SQLException)
{
  __XDBC_ORACLECALL_CHECKPARAM(paramIndex);

  if(__vector_call_params[paramIndex ]->__mi_iomode != OCI_TYPEPARAM_OUT)
  {
    StringBuffer __sb;
    __sb.append("[OracleCallableStatement] *ONLY* can registerOutParameter OUT parameter : ");
    __sb.append(paramIndex);
    __XDBC_HOOK_RETURN(return,__sb.str());
  }

  int sqlt_type=OracleConversion::XDBC_TO_SQLT(sqlType);

  if(DriverManager::isTracingOn)
  {
     StringBuffer __sb;
     __sb.append("registerOutParameter|paramIndex=");
     __sb.append(paramIndex);
     __sb.append(",XDBC=");
     __sb.append(sqlType);
     __sb.append(",SQLT=");
     __sb.append(sqlt_type);

     __XDBC_FORCED_TRACE(ORACLECALLABLESTATEMENT_CLASS_NAME,__sb.str());
  }

  __sp_pstmt->__vector_binds[paramIndex]->__mi_bind_type = sqlt_type;
  sword ret_code = 0;

  switch(sqlt_type)
  {
     case SQLT_INT:
     {
        ret_code=OCIBindByPos(__sp_pstmt->oci_stmthp, &(__sp_pstmt->__vector_binds[paramIndex]->oci_bndp), 
                             __sp_pstmt->__mp_conn->oci_errhp, paramIndex,
                             (dvoid *)& __sp_pstmt->__vector_binds[paramIndex]->ora_value.ora_BIGINT,
                             (sword) sizeof(__sp_pstmt->__vector_binds[paramIndex]->ora_value.ora_BIGINT),SQLT_INT,
                             (dvoid *) &(__sp_pstmt->__vector_binds[paramIndex]->__mi_bind_ind), 
                             (ub2 *) &(__sp_pstmt->__vector_binds[paramIndex]->__mi_bind_size),
                             (ub2 *) &(__sp_pstmt->__vector_binds[paramIndex]->__mi_bind_code), 
                             (ub4) 0, (ub4 *) 0, OCI_DEFAULT);
     }
     break;

     case SQLT_FLT:
     {
        ret_code=OCIBindByPos(__sp_pstmt->oci_stmthp, &(__sp_pstmt->__vector_binds[paramIndex]->oci_bndp),
                             __sp_pstmt->__mp_conn->oci_errhp, paramIndex,
                             (dvoid *)& __sp_pstmt->__vector_binds[paramIndex]->ora_value.ora_DOUBLE,
                             (sword) sizeof(__sp_pstmt->__vector_binds[paramIndex]->ora_value.ora_DOUBLE),SQLT_FLT,
                             (dvoid *) &(__sp_pstmt->__vector_binds[paramIndex]->__mi_bind_ind),
                             (ub2 *) &(__sp_pstmt->__vector_binds[paramIndex]->__mi_bind_size),
                             (ub2 *) &(__sp_pstmt->__vector_binds[paramIndex]->__mi_bind_code),
                             (ub4) 0, (ub4 *) 0, OCI_DEFAULT);
     }
     break;

     case SQLT_VNU:
     {
        ret_code=OCIBindByPos(__sp_pstmt->oci_stmthp, &(__sp_pstmt->__vector_binds[paramIndex]->oci_bndp), 
                             __sp_pstmt->__mp_conn->oci_errhp, paramIndex,
                             (dvoid *)& __sp_pstmt->__vector_binds[paramIndex]->ora_value.ora_NUMBER,
                             (sword) sizeof(__sp_pstmt->__vector_binds[paramIndex]->ora_value.ora_NUMBER),SQLT_VNU,
                             (dvoid *) &(__sp_pstmt->__vector_binds[paramIndex]->__mi_bind_ind), 
                             (ub2 *) &(__sp_pstmt->__vector_binds[paramIndex]->__mi_bind_size),
                             (ub2 *) &(__sp_pstmt->__vector_binds[paramIndex]->__mi_bind_code), 
                             (ub4) 0, (ub4 *) 0, OCI_DEFAULT);
     }
     break;
     
     case SQLT_DAT:
     {
        ret_code=OCIBindByPos(__sp_pstmt->oci_stmthp, &(__sp_pstmt->__vector_binds[paramIndex]->oci_bndp),
                             __sp_pstmt->__mp_conn->oci_errhp, paramIndex,
                             (dvoid *)&(__sp_pstmt->__vector_binds[paramIndex]->ora_value.ora_DATE) ,
                             (sword)sizeof(OCIDate) ,SQLT_ODT,
                             (dvoid *) &(__sp_pstmt->__vector_binds[paramIndex]->__mi_bind_ind),
                             (ub2 *) &(__sp_pstmt->__vector_binds[paramIndex]->__mi_bind_size),
                             (ub2 *) &(__sp_pstmt->__vector_binds[paramIndex]->__mi_bind_code),
                             (ub4) 0, (ub4 *) 0, OCI_DEFAULT);
     }
     break;

     case SQLT_TIMESTAMP:
     {
        if(__sp_pstmt->__vector_binds[paramIndex]->ora_value.ora_DATETIME == 0)
        {
          /* Allocate the descriptor (storage) for the datatype */
          ret_code=OCIDescriptorAlloc(__sp_pstmt->__mp_conn->oci_envhp,(dvoid **)&(__sp_pstmt->__vector_binds[paramIndex]->ora_value.ora_DATETIME),
                                   OracleConversion::SQLT_TO_DTYPE(SQLT_TIMESTAMP), 0, (dvoid **)0);
     
          OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __sp_pstmt->__mp_conn->oci_errhp,ret_code,ORACLECALLABLESTATEMENT_CLASS_NAME);
          __XDBC_HOOK_CHECK(return);  
        }
    
        OCIDateTime* ora_DATETIME=__sp_pstmt->__vector_binds[paramIndex]->ora_value.ora_DATETIME;
    
        ret_code=OCIBindByPos(__sp_pstmt->oci_stmthp, &(__sp_pstmt->__vector_binds[paramIndex]->oci_bndp),
                              __sp_pstmt->__mp_conn->oci_errhp, paramIndex,
                             (dvoid *)&ora_DATETIME , (sword)-1 ,SQLT_TIMESTAMP,
                             (dvoid *) &(__sp_pstmt->__vector_binds[paramIndex]->__mi_bind_ind),
                             (ub2 *) &(__sp_pstmt->__vector_binds[paramIndex]->__mi_bind_size),
                             (ub2 *) &(__sp_pstmt->__vector_binds[paramIndex]->__mi_bind_code),
                             (ub4) 0, (ub4 *) 0, OCI_DEFAULT);      
     }
     break;

     case SQLT_CLOB:
     case SQLT_BLOB:  
     {
        if(__sp_pstmt->__vector_binds[paramIndex]->ora_value.ora_LOB == 0)
        {
          ret_code = OCIDescriptorAlloc((dvoid *)__sp_pstmt->__mp_conn->oci_envhp, 
                                        (dvoid **)&(__sp_pstmt->__vector_binds[paramIndex]->ora_value.ora_LOB) ,
                                        (ub4) OCI_DTYPE_LOB, 
                                        (size_t) 0, 
                                        (dvoid **) 0);
          
          OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __sp_pstmt->__mp_conn->oci_errhp,ret_code,ORACLECALLABLESTATEMENT_CLASS_NAME);
          __XDBC_HOOK_CHECK(return);
          
          ub4 lobEmpty=0;
          ret_code = OCIAttrSet( (OCILobLocator*)__sp_pstmt->__vector_binds[paramIndex]->ora_value.ora_LOB,
                                 OCI_DTYPE_LOB, 
                                 (ub4*)&lobEmpty, 
                                 sizeof(ub4),
                                 OCI_ATTR_LOBEMPTY, 
                                 __sp_pstmt->__mp_conn->oci_errhp);
        
          OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __sp_pstmt->__mp_conn->oci_errhp,ret_code,ORACLECALLABLESTATEMENT_CLASS_NAME);
          __XDBC_HOOK_CHECK(return);  
        }
  
        ret_code=OCIBindByPos(__sp_pstmt->oci_stmthp, &(__sp_pstmt->__vector_binds[paramIndex]->oci_bndp), 
                              __sp_pstmt->__mp_conn->oci_errhp, paramIndex,
                             (dvoid *)&(__sp_pstmt->__vector_binds[paramIndex]->ora_value.ora_LOB) , (sword)-1 ,sqlt_type,
                             (dvoid *) &(__sp_pstmt->__vector_binds[paramIndex]->__mi_bind_ind),
                             (ub2 *) &(__sp_pstmt->__vector_binds[paramIndex]->__mi_bind_size),
                             (ub2 *) &(__sp_pstmt->__vector_binds[paramIndex]->__mi_bind_code),
                             (ub4) 0, (ub4 *) 0, OCI_DEFAULT);  
     }
     break;                    

     default: 
     {
        __sp_pstmt->__vector_binds[paramIndex]->string_value.resize(__vector_call_params[paramIndex]->__mi_length);  
     
        ret_code=OCIBindByPos(__sp_pstmt->oci_stmthp, &(__sp_pstmt->__vector_binds[paramIndex]->oci_bndp), 
                              __sp_pstmt->__mp_conn->oci_errhp, paramIndex,
                             (dvoid *) (__sp_pstmt->__vector_binds[paramIndex]->string_value.data()),
                             (sword) __sp_pstmt->__vector_binds[paramIndex]->string_value.length(),SQLT_STR,
                             (dvoid *) &(__sp_pstmt->__vector_binds[paramIndex]->__mi_bind_ind),
                             (ub2 *) &(__sp_pstmt->__vector_binds[paramIndex]->__mi_bind_size),
                             (ub2 *) &(__sp_pstmt->__vector_binds[paramIndex]->__mi_bind_code),
                             (ub4) 0, (ub4 *) 0, OCI_DEFAULT);       
     }
     break;
  }
  
  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __sp_pstmt->__mp_conn->oci_errhp,ret_code,ORACLECALLABLESTATEMENT_CLASS_NAME);
}

void OracleCallableStatement::registerOutParameter(__XDBC_HOOK_DECLARE_MORE int paramIndex, int sqlType, int scale)  __XDBC_THROW_DECLARE(SQLException)
{
  registerOutParameter(__XDBC_HOOK_INVOKE_MORE paramIndex,sqlType);
}

bool OracleCallableStatement::wasNull(__XDBC_HOOK_DECLARE_MORE int paramIndex)   __XDBC_THROW_DECLARE(SQLException)
{
  return __sp_pstmt->__vector_binds[paramIndex ]->__mi_bind_ind == -1;  
}

bool OracleCallableStatement::getByte(__XDBC_HOOK_DECLARE_MORE int paramIndex,char& paramval)  __XDBC_THROW_DECLARE(SQLException)
{
  XLONG __val_int = 0;
  bool __ret_code = getLong(__XDBC_HOOK_INVOKE_MORE paramIndex,__val_int);
  __XDBC_HOOK_CHECK(return false);
  
  paramval = (char)__val_int; 
  return __ret_code;
}

bool OracleCallableStatement::getShort(__XDBC_HOOK_DECLARE_MORE int paramIndex,short& paramval)  __XDBC_THROW_DECLARE(SQLException)
{
  XLONG __val_int = 0;
  bool __ret_code = getLong(__XDBC_HOOK_INVOKE_MORE paramIndex,__val_int);
  __XDBC_HOOK_CHECK(return false);
    
  paramval = (short)__val_int; 
  return __ret_code;
}

bool OracleCallableStatement::getInt(__XDBC_HOOK_DECLARE_MORE int paramIndex,int& paramval)  __XDBC_THROW_DECLARE(SQLException)
{
  XLONG __val_int = 0;
  bool __ret_code = getLong(__XDBC_HOOK_INVOKE_MORE paramIndex,__val_int);
  __XDBC_HOOK_CHECK(return false);
  
  paramval = (int)__val_int; 
  return __ret_code;
}


bool OracleCallableStatement::getLong(__XDBC_HOOK_DECLARE_MORE int paramIndex,XLONG& paramval)  __XDBC_THROW_DECLARE(SQLException)
{
  __XDBC_ORACLECALL_CHECKNULL_BOOLEAN(paramIndex);

  int mi_bind_type = __sp_pstmt->__vector_binds[paramIndex]->__mi_bind_type;
 
  switch(mi_bind_type)
  {
    case SQLT_INT:
    {
       paramval = __sp_pstmt->__vector_binds[paramIndex]->ora_value.ora_BIGINT; 
       return true;
    }
    break;

    case SQLT_VNU:
    {
       OCINumber* p_oci_number = & __sp_pstmt->__vector_binds[paramIndex]->ora_value.ora_NUMBER;
       sword ret_code = OCINumberToInt(__sp_pstmt->__mp_conn->oci_errhp,p_oci_number,sizeof(XLONG),OCI_NUMBER_SIGNED,(dvoid*)&paramval);
       OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __sp_pstmt->__mp_conn->oci_errhp,ret_code,ORACLECALLABLESTATEMENT_CLASS_NAME);
       __XDBC_HOOK_CHECK(return false);
       return true;
    }
    break;
  }
  
  return checkConversion(__XDBC_HOOK_INVOKE_MORE paramIndex,SQLT_INT,(char*)&paramval);
}

bool OracleCallableStatement::getFloat(__XDBC_HOOK_DECLARE_MORE int paramIndex,float& paramval)  __XDBC_THROW_DECLARE(SQLException)
{
  double __val_double = 0;
  bool __ret_code = getDouble(__XDBC_HOOK_INVOKE_MORE paramIndex,__val_double);
  __XDBC_HOOK_CHECK(return __val_double);
  
  paramval = __val_double; 
  return __ret_code;
}

bool OracleCallableStatement::getDouble(__XDBC_HOOK_DECLARE_MORE int paramIndex,double& paramval)  __XDBC_THROW_DECLARE(SQLException)
{
  __XDBC_ORACLECALL_CHECKNULL_BOOLEAN(paramIndex);

  int mi_bind_type = __sp_pstmt->__vector_binds[paramIndex]->__mi_bind_type;

  switch(mi_bind_type)
  {
    case SQLT_FLT:
    {
       paramval = __sp_pstmt->__vector_binds[paramIndex]->ora_value.ora_DOUBLE;
       return true;
    }
    break;

    case SQLT_VNU:
    {
      OCINumber* p_oci_number = & __sp_pstmt->__vector_binds[paramIndex]->ora_value.ora_NUMBER;
      sword ret_code = OCINumberToReal(__sp_pstmt->__mp_conn->oci_errhp,p_oci_number,sizeof(double),(dvoid*)&paramval);
      OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __sp_pstmt->__mp_conn->oci_errhp,ret_code,ORACLECALLABLESTATEMENT_CLASS_NAME);
      __XDBC_HOOK_CHECK(return false);
      return true;
    }
    break;
  }

  return checkConversion(__XDBC_HOOK_INVOKE_MORE paramIndex,SQLT_FLT,(char*)&paramval);
}

bool OracleCallableStatement::getDate(__XDBC_HOOK_DECLARE_MORE int paramIndex,XDBC_DATE& paramval)  __XDBC_THROW_DECLARE(SQLException)
{
  __XDBC_ORACLECALL_CHECKNULL_BOOLEAN(paramIndex);

  if(__sp_pstmt->__vector_binds[paramIndex]->__mi_bind_type == SQLT_DAT)
  {
    return OracleConversion::parseDate(__XDBC_HOOK_INVOKE_MORE __sp_pstmt->__mp_conn,&__sp_pstmt->__vector_binds[paramIndex]->ora_value.ora_DATE,paramval);
  }
  
  return checkConversion(__XDBC_HOOK_INVOKE_MORE paramIndex,SQLT_DAT,(char*)&paramval);
}

bool OracleCallableStatement::getTime(__XDBC_HOOK_DECLARE_MORE int paramIndex,XDBC_TIME& paramval)  __XDBC_THROW_DECLARE(SQLException)
{
  __XDBC_ORACLECALL_CHECKNULL_BOOLEAN(paramIndex);

  if(__sp_pstmt->__vector_binds[paramIndex]->__mi_bind_type == SQLT_DAT)
  {
    return OracleConversion::parseTime(__XDBC_HOOK_INVOKE_MORE __sp_pstmt->__mp_conn,&__sp_pstmt->__vector_binds[paramIndex]->ora_value.ora_DATE,paramval);
  }
  
  return checkConversion(__XDBC_HOOK_INVOKE_MORE paramIndex,SQLT_DAT,(char*)&paramval);
}

bool OracleCallableStatement::getTimestamp(__XDBC_HOOK_DECLARE_MORE int paramIndex,XDBC_TIMESTAMP& paramval)  __XDBC_THROW_DECLARE(SQLException)
{
  __XDBC_ORACLECALL_CHECKNULL_BOOLEAN(paramIndex);

  if(__sp_pstmt->__vector_binds[paramIndex]->__mi_bind_type == SQLT_TIMESTAMP)
  {
    int paramType=__vector_call_params[paramIndex]->__mi_type;
    OCIDateTime* ora_DATETIME = __sp_pstmt->__vector_binds[paramIndex]->ora_value.ora_DATETIME;
    return OracleConversion::parseTimestamp(__XDBC_HOOK_INVOKE_MORE __sp_pstmt->__mp_conn,ora_DATETIME,paramType,paramval);    
  }
  
  return checkConversion(__XDBC_HOOK_INVOKE_MORE paramIndex,SQLT_TIMESTAMP,(char*)&paramval);
}

bool OracleCallableStatement::getInterval(__XDBC_HOOK_DECLARE_MORE int paramIndex,XDBC_INTERVAL& paramval)  __XDBC_THROW_DECLARE(SQLException)
{
  __XDBC_ORACLECALL_CHECKNULL_BOOLEAN(paramIndex);
    
  int outType=__vector_call_params[paramIndex]->__mi_type;

  if(__sp_pstmt->__vector_binds[paramIndex]->__mi_bind_type == outType)
  {
    OCIInterval* ora_INTERVAL = __sp_pstmt->__vector_binds[paramIndex]->ora_value.ora_INTERVAL;
    return OracleConversion::parseInterval(__XDBC_HOOK_INVOKE_MORE __sp_pstmt->__mp_conn,ora_INTERVAL,outType,paramval);
  }
  
  return checkConversion(__XDBC_HOOK_INVOKE_MORE paramIndex,outType,(char*)&paramval);
}

const char* OracleCallableStatement::getString(__XDBC_HOOK_DECLARE_MORE int paramIndex,int& sizeBytes)  __XDBC_THROW_DECLARE(SQLException)
{
  sizeBytes = 0;
  
  __XDBC_ORACLECALL_CHECKNULL_POINTER(paramIndex);

  int bind_type = __sp_pstmt->__vector_binds[paramIndex]->__mi_bind_type;    

  switch(bind_type)
  {
    case SQLT_STR:
    case SQLT_CHR:
    {
       sizeBytes = (int)(__sp_pstmt->__vector_binds[paramIndex]->__mi_bind_size);
       return __sp_pstmt->__vector_binds[paramIndex]->string_value.data();
    }
    break;
   
  }//~switch(bind_type)

  
  string& __tmp_str = __sp_pstmt->__vector_binds[paramIndex]->__tmp_std_str;  
  checkConversion(__XDBC_HOOK_INVOKE_MORE paramIndex,SQLT_STR,(char*)&__tmp_str);
  __XDBC_HOOK_CHECK(return 0);
  
  sizeBytes = (int)__tmp_str.size();
  return __tmp_str.data();
}

Blob* OracleCallableStatement::getBlob(__XDBC_HOOK_DECLARE_MORE int paramIndex)  __XDBC_THROW_DECLARE(SQLException)
{
  __XDBC_ORACLECALL_CHECKNULL_POINTER(paramIndex);
 
  OCILobLocator* ora_LOB=__sp_pstmt->__vector_binds[paramIndex]->ora_value.ora_LOB;

  if(__sp_pstmt->__vector_binds[paramIndex]->__mi_bind_type == SQLT_BLOB)
  {
    return new OracleBlob(__sp_pstmt->__mp_conn,ora_LOB);
  }
      
  OracleBlob blob(__sp_pstmt->__mp_conn,ora_LOB);
  checkConversion(__XDBC_HOOK_INVOKE_MORE paramIndex,SQLT_BLOB,(char*)&blob);
  __XDBC_HOOK_CHECK(return 0);
  
  OracleBlob* p_blob = new OracleBlob(__sp_pstmt->__mp_conn,ora_LOB);
  p_blob->swap(blob);

  return p_blob;
}

Clob* OracleCallableStatement::getClob(__XDBC_HOOK_DECLARE_MORE int paramIndex)  __XDBC_THROW_DECLARE(SQLException)
{
  __XDBC_ORACLECALL_CHECKNULL_POINTER(paramIndex);
 
  OCILobLocator* ora_LOB=__sp_pstmt->__vector_binds[paramIndex]->ora_value.ora_LOB;

  if(__sp_pstmt->__vector_binds[paramIndex]->__mi_bind_type == SQLT_BLOB)
  {
    return new OracleClob(__sp_pstmt->__mp_conn,ora_LOB);
  }
      
  OracleClob clob(__sp_pstmt->__mp_conn,ora_LOB);
  checkConversion(__XDBC_HOOK_INVOKE_MORE paramIndex,SQLT_BLOB,(char*)&clob);
  __XDBC_HOOK_CHECK(return 0);
  
  OracleClob* p_clob = new OracleClob(__sp_pstmt->__mp_conn,ora_LOB);
  p_clob->swap(clob);

  return p_clob;
}

//----------------------------------------------------------------------------------------
// PreparedStatement Interfaces
//----------------------------------------------------------------------------------------
ResultSet*  OracleCallableStatement::executeQuery(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{
  return __sp_pstmt->executeQuery(__XDBC_HOOK_INVOKE_ONCE);
}

int OracleCallableStatement::executeUpdate(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{
  return __sp_pstmt->executeUpdate(__XDBC_HOOK_INVOKE_ONCE);
}

void OracleCallableStatement::setByte(__XDBC_HOOK_DECLARE_MORE int paramIndex, const char& x,bool is_null)  __XDBC_THROW_DECLARE(SQLException)
{
  __sp_pstmt->setByte(__XDBC_HOOK_INVOKE_MORE paramIndex,x,is_null);
}

void OracleCallableStatement::setShort(__XDBC_HOOK_DECLARE_MORE int paramIndex, const short& x,bool is_null)  __XDBC_THROW_DECLARE(SQLException)
{
  __sp_pstmt->setShort(__XDBC_HOOK_INVOKE_MORE paramIndex,x,is_null);
}

void OracleCallableStatement::setInt(__XDBC_HOOK_DECLARE_MORE int paramIndex, const int& x,bool is_null)  __XDBC_THROW_DECLARE(SQLException)
{
  __sp_pstmt->setInt(__XDBC_HOOK_INVOKE_MORE paramIndex,x,is_null);
}

void OracleCallableStatement::setLong(__XDBC_HOOK_DECLARE_MORE int paramIndex, const XLONG& x,bool is_null)  __XDBC_THROW_DECLARE(SQLException)
{
  __sp_pstmt->setLong(__XDBC_HOOK_INVOKE_MORE paramIndex,x,is_null);
}

void OracleCallableStatement::setFloat(__XDBC_HOOK_DECLARE_MORE int paramIndex, const float& x,bool is_null)  __XDBC_THROW_DECLARE(SQLException)
{
  __sp_pstmt->setFloat(__XDBC_HOOK_INVOKE_MORE paramIndex,x,is_null);
}

void OracleCallableStatement::setDouble(__XDBC_HOOK_DECLARE_MORE int paramIndex, const double& x,bool is_null)  __XDBC_THROW_DECLARE(SQLException)
{
  __sp_pstmt->setDouble(__XDBC_HOOK_INVOKE_MORE paramIndex,x,is_null);
}

void OracleCallableStatement::setDate(__XDBC_HOOK_DECLARE_MORE int paramIndex,const XDBC_DATE* x,bool is_null) __XDBC_THROW_DECLARE(SQLException)
{
  __sp_pstmt->setDate(__XDBC_HOOK_INVOKE_MORE paramIndex,x,is_null);
}

void OracleCallableStatement::setTime(__XDBC_HOOK_DECLARE_MORE int paramIndex,const  XDBC_TIME* x,bool is_null)  __XDBC_THROW_DECLARE(SQLException)
{
  __sp_pstmt->setTime(__XDBC_HOOK_INVOKE_MORE paramIndex,x,is_null);
}

void OracleCallableStatement::setTimestamp(__XDBC_HOOK_DECLARE_MORE int paramIndex,const XDBC_TIMESTAMP* x,bool is_null)  __XDBC_THROW_DECLARE(SQLException)
{
  __sp_pstmt->setTimestamp(__XDBC_HOOK_INVOKE_MORE paramIndex,x,is_null);
}

void  OracleCallableStatement::setInterval(__XDBC_HOOK_DECLARE_MORE int paramIndex,  const XDBC_INTERVAL* x,bool is_null)  __XDBC_THROW_DECLARE(SQLException)
{
  __sp_pstmt->setInterval(__XDBC_HOOK_INVOKE_MORE paramIndex,x,is_null);
}

void OracleCallableStatement::setString(__XDBC_HOOK_DECLARE_MORE int paramIndex, const string* x,bool is_null)  __XDBC_THROW_DECLARE(SQLException)
{
  __sp_pstmt->setString(__XDBC_HOOK_INVOKE_MORE paramIndex,x,is_null);
}

void OracleCallableStatement::setString(__XDBC_HOOK_DECLARE_MORE int paramIndex,  const char* x,int count,bool is_null)  __XDBC_THROW_DECLARE(SQLException)
{
  __sp_pstmt->setString(__XDBC_HOOK_INVOKE_MORE paramIndex,x,count,is_null);
}

void OracleCallableStatement::setBlob (__XDBC_HOOK_DECLARE_MORE int paramIndex, const char* x,int count,bool is_null)  __XDBC_THROW_DECLARE(SQLException)
{
  __sp_pstmt->setBlob(__XDBC_HOOK_INVOKE_MORE paramIndex,x,count,is_null);
}

void OracleCallableStatement::setBlob (__XDBC_HOOK_DECLARE_MORE int paramIndex,const Blob* x,bool is_null)  __XDBC_THROW_DECLARE(SQLException)
{
  __sp_pstmt->setBlob(__XDBC_HOOK_INVOKE_MORE paramIndex,x,is_null);
}

void OracleCallableStatement::setClob (__XDBC_HOOK_DECLARE_MORE int paramIndex, const char* x,int count,bool is_null)  __XDBC_THROW_DECLARE(SQLException)
{
  __sp_pstmt->setClob(__XDBC_HOOK_INVOKE_MORE paramIndex,x,count,is_null);
}

void OracleCallableStatement::setClob (__XDBC_HOOK_DECLARE_MORE int paramIndex,const Clob* x,bool is_null)  __XDBC_THROW_DECLARE(SQLException)
{
  __sp_pstmt->setClob(__XDBC_HOOK_INVOKE_MORE paramIndex,x,is_null);
}

void  OracleCallableStatement::close()
{
  __XDBC_TRACE(ORACLECALLABLESTATEMENT_CLASS_NAME,"close()");

  for(int i = 1;i <= __mi_call_param_count;++i)
  {
    delete __vector_call_params[i];
  }
  __vector_call_params.clear();  
  __sp_pstmt->close();
}

void  OracleCallableStatement::destroy()
{
  delete this;
}

int OracleCallableStatement::getResultSetType(__XDBC_HOOK_DECLARE_ONCE)   __XDBC_THROW_DECLARE(SQLException)
{
  return __sp_pstmt->getResultSetType(__XDBC_HOOK_INVOKE_ONCE);
}

Connection* OracleCallableStatement::getConnection(__XDBC_HOOK_DECLARE_ONCE)   __XDBC_THROW_DECLARE(SQLException)
{
  return __sp_pstmt->getConnection(__XDBC_HOOK_INVOKE_ONCE);
}

void  OracleCallableStatement::parseProcName(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{
  size_t  flag=__sp_pstmt->str_orginal_oracle_sql.find("call");
  if(flag==string::npos)
  {
    __XDBC_HOOK_RETURN(return,"procedure not found");
  }

  size_t end=__sp_pstmt->str_orginal_oracle_sql.find("(",flag);
  if(end==string::npos)
  {
    __XDBC_HOOK_RETURN(return,"procedure not found");
  }

  size_t start=flag+strlen("call");
  while(__sp_pstmt->str_orginal_oracle_sql.at(start) == ' ')
  {
    start++;
  }

  while(__sp_pstmt->str_orginal_oracle_sql.at(end) == ' ')
  {
    end--;
  }

  __ms_proc_name=__sp_pstmt->str_orginal_oracle_sql.substr(start,end-start);
}

bool OracleCallableStatement::checkConversion(__XDBC_HOOK_DECLARE_MORE int paramIndex,int outType,char* outBuffer) __XDBC_THROW_DECLARE(SQLException)
{
  if(__vector_call_params[paramIndex]->__mi_iomode != OCI_TYPEPARAM_OUT)
  {
    __XDBC_HOOK_RETURN(return false,"can only retrive return data from OUT parameter");
  }
  
  int inType = __sp_pstmt->__vector_binds[paramIndex]->__mi_bind_type;
 
  if(inType==outType)
  {
    return false;
  }
    
  switch(outType)
  {
    case SQLT_INT:
    {
      if(   inType == SQLT_UIN
         || inType == SQLT_FLT
         || inType == SQLT_BFLOAT
         || inType == SQLT_BDOUBLE
         || inType == SQLT_IBFLOAT
         || inType == SQLT_IBDOUBLE
         || inType == SQLT_NUM
         || inType == SQLT_VNU
        )
      {
        return false;
      }
    }
    break;
    
    case SQLT_FLT:
    {
      if(   inType == SQLT_UIN
         || inType == SQLT_INT
         || inType == SQLT_BFLOAT
         || inType == SQLT_BDOUBLE
         || inType == SQLT_IBFLOAT
         || inType == SQLT_IBDOUBLE
         || inType == SQLT_NUM
         || inType == SQLT_VNU
        )
      {
        return false;
      }
    }
    break;
        
    case SQLT_STR:
    {
      if( inType == SQLT_CHR || inType == SQLT_AFC || inType == SQLT_AVC )
      {
        return false;
      }     
      else if( inType == SQLT_VCS || inType == SQLT_VBI)
      {
        return false;
      }
      else if( inType == SQLT_LVC || inType == SQLT_LVB)
      {
        return false;
      }
 
    }
    break;
    
    case SQLT_DAT:
    {
      
    }
    break;
    
    case SQLT_TIMESTAMP:
    {
    }
    break;
    
    case SQLT_CLOB:
    {
    }
    break;
    
    case SQLT_BLOB:
    {
    }
    break;
    
    default:
    {
    }
    break;
  }
  
  StringBuffer __sb;
  __sb.append("[OracleCallableStatement] Conversion not supported . sourceSQLType:");
  __sb.append(__vector_call_params[paramIndex]->__ms_type_name);
  __sb.append("(");
  __sb.append(inType);
  __sb.append("),targetSQLType:");
  __sb.append(OracleTypes::getInstance().getTypeName(outType,false));
  __sb.append("(");
  __sb.append(outType);
  __sb.append(")");
  
  __XDBC_HOOK_RETURN(return false,__sb.str());
}

__OPENCODE_END_NAMESPACE
