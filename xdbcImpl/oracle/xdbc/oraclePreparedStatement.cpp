#include "xdbc/oraclePreparedStatement.h"
#include "xdbc/oracleConnection.h"
#include "xdbc/oracleResultSet.h"
#include "xdbc/oracleConversion.h"

#include "xdbc/oracleClob.h"
#include "xdbc/oracleBlob.h"

#include "xdbc/oracleSQL.h"
#include "xdbc/oracleConversion.h"

#include "port/ioc_sbuf.h"

__OPENCODE_BEGIN_NAMESPACE

#define ORACLEPREPAREDSTATEMENT_CLASS_NAME "OraclePreparedStatement"

OraclePreparedStatement::~OraclePreparedStatement()
{
  __XDBC_TRACE(ORACLEPREPAREDSTATEMENT_CLASS_NAME,"destroy()");
}

OraclePreparedStatement::OraclePreparedStatement(OracleConnection* ora_conn,const string& sql,int paramCount)
{
  __mp_conn=ora_conn;
  
  __mi_resultSetType=ResultSet::TYPE_FORWARD_ONLY;
  __mi_resultSetConcurrency=ResultSet::CONCUR_READ_ONLY;
  
  oci_stmthp=0;
  __mb_initialized=false;
  __mb_isClosed=false;
  
  str_orginal_oracle_sql = sql;
  str_oracle_sql = sql;
  
  __mi_param_count= paramCount;  

//--------------------------
   
  __cachedCollection = 0;
  __adoptIt = false;
  __isDefined = false;  
  __p_rs_opened = 0;
}

OraclePreparedStatement::OraclePreparedStatement(OracleConnection* ora_conn,const string& sql, int resultSetType, int resultSetConcurrency,int paramCount)
{
  __mp_conn=ora_conn;

  __mi_resultSetType=resultSetType;
  __mi_resultSetConcurrency=resultSetConcurrency;
  
  oci_stmthp=0;
  __mb_initialized=false;
  __mb_isClosed=false;
  
  str_orginal_oracle_sql = sql;
  str_oracle_sql = sql;

  __mi_param_count= paramCount;
  
//--------------------------
   
  __cachedCollection = 0;
  __adoptIt = false;
  __isDefined = false;  
  __p_rs_opened = 0;
}

void OraclePreparedStatement::initialize(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{ 
  if(__mb_initialized)
  {
    return;
  }
  
  __mb_initialized=true;
    
#if !defined(OPENCODE_HAS_STMTPREPARE2)
  sword ret_code = OCIHandleAlloc( (dvoid *) __mp_conn->oci_envhp, (dvoid **) &oci_stmthp,OCI_HTYPE_STMT, (size_t) 0, (dvoid **) 0);
  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLEPREPAREDSTATEMENT_CLASS_NAME);
  __XDBC_HOOK_CHECK(return);
#endif

#if !defined(OPENCODE_HAS_STMTPREPARE2)
  if(__mi_param_count== SQLImplementation::paramCountUnknown)
  {
    str_oracle_sql = OracleSQL::parseSQL(str_orginal_oracle_sql,__mi_param_count);
  }
  
  sword ret_code = OCIStmtPrepare(oci_stmthp, __mp_conn->oci_errhp, 
                                 (text*)str_oracle_sql.c_str(), (ub4) str_oracle_sql.length(), 
                                 (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);                                   

  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLEPREPAREDSTATEMENT_CLASS_NAME);
  __XDBC_HOOK_CHECK(return);
#else 
  sword ret_code = OCIStmtPrepare2(__mp_conn->oci_svchp,&oci_stmthp, __mp_conn->oci_errhp, 
                                 (text*)0, (ub4)0,
                                 (text*)str_orginal_oracle_sql.c_str(), (ub4) str_orginal_oracle_sql.length(),
                                 (ub4) OCI_NTV_SYNTAX, (ub4) OCI_PREP2_CACHE_SEARCHONLY); 

  if(ret_code != OCI_SUCCESS)
  {
    if(__mi_param_count== SQLImplementation::paramCountUnknown)
    {
      str_oracle_sql = OracleSQL::parseSQL(__XDBC_HOOK_INVOKE_MORE str_orginal_oracle_sql,__mi_param_count);
      __XDBC_HOOK_CHECK(return);
    }
    
    ret_code = OCIStmtPrepare2(__mp_conn->oci_svchp,&oci_stmthp, __mp_conn->oci_errhp, 
                               (text*)str_oracle_sql.c_str(), (ub4) str_oracle_sql.length(),
                               (text*)0, (ub4) 0,
                               (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT); 

    OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLEPREPAREDSTATEMENT_CLASS_NAME);
    __XDBC_HOOK_CHECK(return);
    __XDBC_TRACE(ORACLEPREPAREDSTATEMENT_CLASS_NAME,"<init>|create statement");
  }
  else
  {
    OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLEPREPAREDSTATEMENT_CLASS_NAME);
    __XDBC_HOOK_CHECK(return);
  }

#endif

  if(__mi_param_count== SQLImplementation::paramCountUnknown)
  {
    ub4 __bind_count= 0;
    ret_code = OCIAttrGet((dvoid *)oci_stmthp, OCI_HTYPE_STMT,
                  (dvoid *)&__bind_count, (ub4 *)0, OCI_ATTR_BIND_COUNT,
                   __mp_conn->oci_errhp);
    OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLEPREPAREDSTATEMENT_CLASS_NAME);                
    __XDBC_HOOK_CHECK(return);  
    __mi_param_count= __bind_count;
  }
      
  if(__mi_param_count > 0)
  {
    __vector_binds.resize(__mi_param_count + 1);
    __vector_params.resize(__mi_param_count + 1);
    
    for( int i  =  1; i <= __mi_param_count ;++i)
    {
      OracleBind* __p_bind  =  new OracleBind();
      __p_bind->__mi_index  =  i;   
      __vector_binds[i]  =  __p_bind;
      
      OracleParameter* __p_param  =  new OracleParameter();
      __p_param->__mi_index  =  i; 
      __vector_params[i]  =  __p_param;
    }
  }
}

ResultSet* OraclePreparedStatement::executeQuery(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
     StringBuffer __sb;
     __sb.append("executeQuery|");
     __sb.append(str_oracle_sql);
     
     __XDBC_FORCED_TRACE(ORACLEPREPAREDSTATEMENT_CLASS_NAME,__sb.str()); 
  }

  sword ret_code = 0;
  if(__mi_resultSetType  ==  ResultSet::TYPE_FORWARD_ONLY)
  {
    ret_code = OCIStmtExecute(__mp_conn->oci_svchp, oci_stmthp, __mp_conn->oci_errhp,
                           (ub4) 0, (ub4) 0,
                           (CONST OCISnapshot *) 0, (OCISnapshot *) 0,
                           OCI_DEFAULT);
  }
  else
  {
    ret_code = OCIStmtExecute(__mp_conn->oci_svchp, oci_stmthp, __mp_conn->oci_errhp,
                           (ub4) 0, (ub4) 0,
                           (CONST OCISnapshot *) 0, (OCISnapshot *) 0,
                            OCI_STMT_SCROLLABLE_READONLY);

  }

  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLEPREPAREDSTATEMENT_CLASS_NAME);
  __XDBC_HOOK_CHECK(return 0);
  
  if(!__isDefined)
  { 
    __cachedCollection = new OracleResultSetCollection(); 
    __adoptIt = true;
    __isDefined = true; 
    
    OracleConversion::parseResultSetMetaData(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,oci_stmthp,__cachedCollection->__vector_columns); 
    __XDBC_HOOK_CHECK(return 0);
    
    OracleResultSet::prepareColumnData(__XDBC_HOOK_INVOKE_MORE __mp_conn,oci_stmthp,__cachedCollection); 
    __XDBC_HOOK_CHECK(return 0);
      
    OracleResultSet::defineColumnData(__XDBC_HOOK_INVOKE_MORE __mp_conn,oci_stmthp,__cachedCollection); 
    __XDBC_HOOK_CHECK(return 0);  
  }
  
  if(__p_rs_opened == 0)
  {
    __p_rs_opened  = new OracleResultSet(__mp_conn,this,oci_stmthp,__mi_resultSetType,__mi_resultSetConcurrency,__cachedCollection,false,false);           
  }
  else
  {
    __XDBC_INTERFACE_CLOSEANDDESTROY(__p_rs_opened);
    
    OracleResultSet::resetResultSet(__p_rs_opened,__mp_conn,this,oci_stmthp,
                               __mi_resultSetType,__mi_resultSetConcurrency,
                               __cachedCollection,false,
                               false);    
  }
  
  
   
  return __p_rs_opened;  
}

int OraclePreparedStatement::executeUpdate(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
     StringBuffer __sb;
     __sb.append("executeUpdate|");
     __sb.append(str_oracle_sql);
     
     __XDBC_FORCED_TRACE(ORACLEPREPAREDSTATEMENT_CLASS_NAME,__sb.str()); 
  }

  sword ret_code = 0;

  if(__mp_conn->__mb_autoCommit)
  {
     ret_code = OCIStmtExecute(__mp_conn->oci_svchp, oci_stmthp, __mp_conn->oci_errhp,
                            (ub4) 1, (ub4) 0, (CONST OCISnapshot *) 0, (OCISnapshot *) 0, 
                            OCI_DEFAULT|OCI_COMMIT_ON_SUCCESS);
  }
  else
  {

     ret_code = OCIStmtExecute(__mp_conn->oci_svchp, oci_stmthp, __mp_conn->oci_errhp,
                            (ub4) 1, (ub4) 0, (CONST OCISnapshot *) 0, (OCISnapshot *) 0,
                            OCI_DEFAULT);
  }
  
  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLEPREPAREDSTATEMENT_CLASS_NAME);
  __XDBC_HOOK_CHECK(return 0);
  
  ub4 rowsFetched = 0;
  ub4 size = sizeof(rowsFetched);
  OCIAttrGet ( (dvoid *) oci_stmthp,(ub4) OCI_HTYPE_STMT, (dvoid*)&rowsFetched, (ub4*)&size, (ub4) OCI_ATTR_ROW_COUNT, __mp_conn->oci_errhp);

  return rowsFetched;
}

void OraclePreparedStatement::setByte(__XDBC_HOOK_DECLARE_MORE int paramIndex,const char& x,bool is_null)  __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    __sb << "setByte|index=" << paramIndex << ",value=" << x << ",null=" << is_null;
    __XDBC_FORCED_TRACE(ORACLEPREPAREDSTATEMENT_CLASS_NAME,__sb.str()); 
  }

  __XDBC_ORACLEPREPARED_CHECKPARAM(paramIndex);

  OracleBind* __oracle_bind = __vector_binds[paramIndex];
  OracleParameter* __oracle_param = __vector_params[paramIndex]; 
  
   __oracle_param->__mi_type  =  SQLT_VNU;
  
  
  sword ret_code = OCINumberFromInt( __mp_conn->oci_errhp,
                                  &x,sizeof(x),OCI_NUMBER_SIGNED,
                                  & __oracle_bind->ora_value.ora_NUMBER);

  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLEPREPAREDSTATEMENT_CLASS_NAME);
  __XDBC_HOOK_CHECK(return);
  
   __XDBC_ORACLEPREPARED_CHECKBOUND_BEGIN_2(paramIndex);

   sword ret_code = OCIBindByPos(oci_stmthp, &(__oracle_bind->oci_bndp), __mp_conn->oci_errhp,
                          paramIndex,
                          (dvoid *) &(__oracle_bind->ora_value.ora_NUMBER),
                          (sword)sizeof(OCINumber),SQLT_VNU,
                          (dvoid *)&(__oracle_bind->__mi_bind_ind) ,
                          (ub2 *) 0, (ub2 *) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLEPREPAREDSTATEMENT_CLASS_NAME);

  __XDBC_ORACLEPREPARED_CHECKBOUND_END_2(paramIndex);
}

void OraclePreparedStatement::setShort(__XDBC_HOOK_DECLARE_MORE int paramIndex,const short& x,bool is_null)  __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    __sb << "setShort|index=" << paramIndex << ",value=" << x << ",null=" << is_null;
    
    __XDBC_FORCED_TRACE(ORACLEPREPAREDSTATEMENT_CLASS_NAME,__sb.str()); 
  }

  __XDBC_ORACLEPREPARED_CHECKPARAM(paramIndex);

  OracleBind* __oracle_bind = __vector_binds[paramIndex];
  OracleParameter* __oracle_param = __vector_params[paramIndex]; 
    
   __oracle_param->__mi_type  =  SQLT_VNU;
  
  sword ret_code = OCINumberFromInt( __mp_conn->oci_errhp,
                                  &x,sizeof(x),OCI_NUMBER_SIGNED,
                                  & __oracle_bind->ora_value.ora_NUMBER);

  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLEPREPAREDSTATEMENT_CLASS_NAME);
  __XDBC_HOOK_CHECK(return);  

   __XDBC_ORACLEPREPARED_CHECKBOUND_BEGIN_2(paramIndex);

   sword ret_code = OCIBindByPos(oci_stmthp, &(__oracle_bind->oci_bndp), __mp_conn->oci_errhp,
                          paramIndex,
                          (dvoid *) &(__oracle_bind->ora_value.ora_NUMBER),
                          (sword)sizeof(OCINumber),SQLT_VNU,
                          (dvoid *)&(__oracle_bind->__mi_bind_ind) ,
                          (ub2 *) 0, (ub2 *) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLEPREPAREDSTATEMENT_CLASS_NAME);

  __XDBC_ORACLEPREPARED_CHECKBOUND_END_2(paramIndex);
}

void OraclePreparedStatement::setInt(__XDBC_HOOK_DECLARE_MORE int paramIndex,const int& x,bool is_null)  __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    __sb << "setInt|index=" << paramIndex << ",value=" << x << ",null=" << is_null;
    
    __XDBC_FORCED_TRACE(ORACLEPREPAREDSTATEMENT_CLASS_NAME,__sb.str()); 
  }
  
  __XDBC_ORACLEPREPARED_CHECKPARAM(paramIndex);

  OracleBind* __oracle_bind = __vector_binds[paramIndex];
  OracleParameter* __oracle_param = __vector_params[paramIndex]; 

   __oracle_param->__mi_type  =  SQLT_VNU;

  sword ret_code = OCINumberFromInt( __mp_conn->oci_errhp,
                                  &x,sizeof(x),OCI_NUMBER_SIGNED,
                                  & __oracle_bind->ora_value.ora_NUMBER);

  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLEPREPAREDSTATEMENT_CLASS_NAME);
  __XDBC_HOOK_CHECK(return);
    
  __XDBC_ORACLEPREPARED_CHECKBOUND_BEGIN_2(paramIndex);

  sword ret_code = OCIBindByPos(oci_stmthp, &(__oracle_bind->oci_bndp), __mp_conn->oci_errhp,
                          paramIndex,
                          (dvoid *) &(__oracle_bind->ora_value.ora_NUMBER),
                          (sword)sizeof(OCINumber),SQLT_VNU,
                          (dvoid *)&(__oracle_bind->__mi_bind_ind) ,
                          (ub2 *) 0, (ub2 *) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLEPREPAREDSTATEMENT_CLASS_NAME);

  __XDBC_ORACLEPREPARED_CHECKBOUND_END_2(paramIndex);
}

void OraclePreparedStatement::setLong(__XDBC_HOOK_DECLARE_MORE int paramIndex,const XLONG& x,bool is_null)  __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    __sb << "setLong|index=" << paramIndex << ",value=" << x << ",null=" << is_null;
    
    __XDBC_FORCED_TRACE(ORACLEPREPAREDSTATEMENT_CLASS_NAME,__sb.str()); 
  }

  __XDBC_ORACLEPREPARED_CHECKPARAM(paramIndex);

  OracleBind* __oracle_bind = __vector_binds[paramIndex];
  OracleParameter* __oracle_param = __vector_params[paramIndex]; 
    
   __oracle_param->__mi_type  =  SQLT_VNU;
  
  sword ret_code = OCINumberFromInt( __mp_conn->oci_errhp,
          &x,sizeof(x),OCI_NUMBER_SIGNED,
          & __oracle_bind->ora_value.ora_NUMBER); 

  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLEPREPAREDSTATEMENT_CLASS_NAME);
  __XDBC_HOOK_CHECK(return);
    
  __XDBC_ORACLEPREPARED_CHECKBOUND_BEGIN_2(paramIndex);

  sword ret_code = OCIBindByPos(oci_stmthp, &(__oracle_bind->oci_bndp), __mp_conn->oci_errhp,
                          paramIndex,
                          (dvoid *) &(__oracle_bind->ora_value.ora_NUMBER),
                          (sword)sizeof(OCINumber),SQLT_VNU,
                          (dvoid *)&(__oracle_bind->__mi_bind_ind) ,
                          (ub2 *) 0, (ub2 *) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLEPREPAREDSTATEMENT_CLASS_NAME);

  __XDBC_ORACLEPREPARED_CHECKBOUND_END_2(paramIndex);
}

void OraclePreparedStatement::setFloat(__XDBC_HOOK_DECLARE_MORE int paramIndex,const float& x,bool is_null)  __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    __sb << "setFloat|index=" << paramIndex << ",value=" << x << ",null=" << is_null;
    __XDBC_FORCED_TRACE(ORACLEPREPAREDSTATEMENT_CLASS_NAME,__sb.str()); 
  }

  __XDBC_ORACLEPREPARED_CHECKPARAM(paramIndex);

  OracleBind* __oracle_bind = __vector_binds[paramIndex];
  OracleParameter* __oracle_param = __vector_params[paramIndex];
     
   __oracle_param->__mi_type  =  SQLT_VNU;
  
  sword ret_code = OCINumberFromReal( __mp_conn->oci_errhp,
                                  &x,sizeof(x),
                                  & __oracle_bind->ora_value.ora_NUMBER);

  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLEPREPAREDSTATEMENT_CLASS_NAME);
  __XDBC_HOOK_CHECK(return);
    
   __XDBC_ORACLEPREPARED_CHECKBOUND_BEGIN_2(paramIndex);

   sword ret_code = OCIBindByPos(oci_stmthp, &(__oracle_bind->oci_bndp), __mp_conn->oci_errhp,
                          paramIndex,
                          (dvoid *) &(__oracle_bind->ora_value.ora_NUMBER),
                          (sword)sizeof(OCINumber),SQLT_VNU,
                          (dvoid *)&(__oracle_bind->__mi_bind_ind) ,
                          (ub2 *) 0, (ub2 *) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLEPREPAREDSTATEMENT_CLASS_NAME);

  __XDBC_ORACLEPREPARED_CHECKBOUND_END_2(paramIndex);
}

void OraclePreparedStatement::setDouble(__XDBC_HOOK_DECLARE_MORE int paramIndex,const double& x,bool is_null)  __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    __sb << "setDouble|index=" << paramIndex << ",value=" << x << ",null=" << is_null;
    
    __XDBC_FORCED_TRACE(ORACLEPREPAREDSTATEMENT_CLASS_NAME,__sb.str()); 
  }

  __XDBC_ORACLEPREPARED_CHECKPARAM(paramIndex);

  OracleBind* __oracle_bind = __vector_binds[paramIndex];
  OracleParameter* __oracle_param = __vector_params[paramIndex]; 
    
   __oracle_param->__mi_type  =  SQLT_VNU;

  sword ret_code = OCINumberFromReal( __mp_conn->oci_errhp,
                                  &x,sizeof(x),
                                  & __oracle_bind->ora_value.ora_NUMBER);

  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLEPREPAREDSTATEMENT_CLASS_NAME);
  __XDBC_HOOK_CHECK(return);  

   __XDBC_ORACLEPREPARED_CHECKBOUND_BEGIN_2(paramIndex);

   sword ret_code = OCIBindByPos(oci_stmthp, &(__oracle_bind->oci_bndp), __mp_conn->oci_errhp,
                          paramIndex,
                          (dvoid *) &(__oracle_bind->ora_value.ora_NUMBER),
                          (sword)sizeof(OCINumber),SQLT_VNU,
                          (dvoid *)&(__oracle_bind->__mi_bind_ind) ,
                          (ub2 *) 0, (ub2 *) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLEPREPAREDSTATEMENT_CLASS_NAME);

  __XDBC_ORACLEPREPARED_CHECKBOUND_END_2(paramIndex);
}

void OraclePreparedStatement::setString(__XDBC_HOOK_DECLARE_MORE int paramIndex,const string* x,bool is_null)  __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    if(x != 0)
    {
      __sb << "setString|index=" << paramIndex << ",value=" << (*x) << ",null=" << is_null;
    }
    else
    {
      __sb << "setString|index=" << paramIndex << ",value=<NULL>,null=" << is_null;
    }
        
    __XDBC_FORCED_TRACE(ORACLEPREPAREDSTATEMENT_CLASS_NAME,__sb.str()); 
  }

  __XDBC_ORACLEPREPARED_CHECKPARAM(paramIndex);

  OracleBind* __oracle_bind = __vector_binds[paramIndex];
  OracleParameter* __oracle_param = __vector_params[paramIndex]; 
    
   __oracle_param->__mi_type  =  SQLT_CHR; 
   
  if(x != 0)
  { 
    __oracle_bind->string_value = *x; 
  }

  __XDBC_ORACLEPREPARED_CHECKBOUND_BEGIN_1(paramIndex);
  
  sword ret_code = OCIBindByPos(oci_stmthp, &(__oracle_bind->oci_bndp), __mp_conn->oci_errhp,
                            paramIndex,
                            (dvoid *) __oracle_bind->string_value.c_str(),
                            //(sword)strlen(__oracle_bind->string_value.c_str()),
                            (sword)__oracle_bind->string_value.size(),
                             __oracle_param->__mi_type,
                            (dvoid *)&(__oracle_bind->__mi_bind_ind) ,
                            (ub2 *) 0, (ub2 *) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLEPREPAREDSTATEMENT_CLASS_NAME);

  __XDBC_ORACLEPREPARED_CHECKBOUND_END_1(paramIndex);
}

void OraclePreparedStatement::setString(__XDBC_HOOK_DECLARE_MORE int paramIndex,const char* x,int count,bool is_null)  __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    if(x != 0)
    {
      __sb << "setString|index=" << paramIndex << ",value=" << x << ",count=" << count << ",null=" << is_null;
    }
    else
    {
      __sb << "setString|index=" << paramIndex << ",value=<NULL>,count=" << count << ",null=" << is_null;
    }
    
    
    __XDBC_FORCED_TRACE(ORACLEPREPAREDSTATEMENT_CLASS_NAME,__sb.str()); 
  }

  __XDBC_ORACLEPREPARED_CHECKPARAM(paramIndex);

  OracleBind* __oracle_bind = __vector_binds[paramIndex];
  OracleParameter* __oracle_param = __vector_params[paramIndex]; 
    
   __oracle_param->__mi_type  =  SQLT_CHR;
   
  if(x != 0)
  { 
    __oracle_bind->string_value.assign(x,count);
  }

  __XDBC_ORACLEPREPARED_CHECKBOUND_BEGIN_1(paramIndex);

  sword ret_code = OCIBindByPos(oci_stmthp, &(__oracle_bind->oci_bndp), __mp_conn->oci_errhp,
                            paramIndex,
                            (dvoid *) __oracle_bind->string_value.c_str(),
                            (sword)__oracle_bind->string_value.size(),
                              __oracle_param->__mi_type,
                            (dvoid *)&(__oracle_bind->__mi_bind_ind) ,
                            (ub2 *) 0, (ub2 *) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLEPREPAREDSTATEMENT_CLASS_NAME);
  
  __XDBC_ORACLEPREPARED_CHECKBOUND_END_1(paramIndex);
}

void OraclePreparedStatement::setDate(__XDBC_HOOK_DECLARE_MORE int paramIndex,const XDBC_DATE* x,bool is_null) __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    if(x !=  0)
    {
      __sb << "setDate|index  =  " << paramIndex << ", value  =  " << XDBCUtilities::toString(*x) << ", null  =  " << is_null;
    }
    else
    {
      __sb << "setDate|index  =  " << paramIndex << ", value  =  <null>, null  =  " << is_null;
    }
    
    __XDBC_FORCED_TRACE(ORACLEPREPAREDSTATEMENT_CLASS_NAME,__sb.str()); 
  }

  __XDBC_ORACLEPREPARED_CHECKPARAM(paramIndex);

  OracleBind* __oracle_bind = __vector_binds[paramIndex];
  OracleParameter* __oracle_param = __vector_params[paramIndex]; 
    
   __oracle_param->__mi_type  =  SQLT_ODT; 
  
  if(x != 0) 
  {
    OracleConversion::makeDate(*x,&(__oracle_bind->ora_value.ora_DATE)); 
  }


  __XDBC_ORACLEPREPARED_CHECKBOUND_BEGIN_2(paramIndex);

  sword ret_code = OCIBindByPos(oci_stmthp,  &(__oracle_bind->oci_bndp), __mp_conn->oci_errhp,
                            paramIndex,
                            (dvoid *)&(__oracle_bind->ora_value.ora_DATE),
                            (sword)sizeof(OCIDate),
                             __oracle_param->__mi_type,
                            (dvoid *)&(__oracle_bind->__mi_bind_ind) ,
                            (ub2 *) 0, (ub2 *) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLEPREPAREDSTATEMENT_CLASS_NAME);
  
  __XDBC_ORACLEPREPARED_CHECKBOUND_END_2(__paramIndex);
}

void OraclePreparedStatement::setTime(__XDBC_HOOK_DECLARE_MORE int paramIndex,const XDBC_TIME* x,bool is_null)  __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    if(x !=  0)
    {
      __sb << "setTime|index  =  " << paramIndex << ", value  =  " << XDBCUtilities::toString(*x) << ", null  =  " << is_null;
    }
    else
    {
      __sb << "setTime|index  =  " << paramIndex << ", value  =  <null>, null  =  " << is_null;
    }
    
    __XDBC_FORCED_TRACE(ORACLEPREPAREDSTATEMENT_CLASS_NAME,__sb.str()); 
  }

  __XDBC_ORACLEPREPARED_CHECKPARAM(paramIndex);

  OracleBind* __oracle_bind = __vector_binds[paramIndex];
  OracleParameter* __oracle_param = __vector_params[paramIndex]; 
    
   __oracle_param->__mi_type  =  SQLT_ODT;
  
  if(x != 0)
  { 
    OracleConversion::makeDate(*x,&(__oracle_bind->ora_value.ora_DATE)); 
  }


  __XDBC_ORACLEPREPARED_CHECKBOUND_BEGIN_2(paramIndex);

  sword ret_code = OCIBindByPos(oci_stmthp,  &(__oracle_bind->oci_bndp), __mp_conn->oci_errhp,
                            paramIndex,
                            (dvoid *)&(__oracle_bind->ora_value.ora_DATE),
                            (sword)sizeof(OCIDate) ,
                             __oracle_param->__mi_type,
                            (dvoid *)&(__oracle_bind->__mi_bind_ind) ,
                            (ub2 *) 0, (ub2 *) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLEPREPAREDSTATEMENT_CLASS_NAME);
  
  __XDBC_ORACLEPREPARED_CHECKBOUND_END_2(__paramIndex);
}

void OraclePreparedStatement::setTimestamp(__XDBC_HOOK_DECLARE_MORE int paramIndex,const XDBC_TIMESTAMP* x,bool is_null)  __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    if(x !=  0)
    {
      __sb << "setTimestamp|index  =  " << paramIndex << ", value  =  " << XDBCUtilities::toString(*x) << ", null  =  " << is_null;
    }
    else
    {
      __sb << "setTimestamp|index  =  " << paramIndex << ", value  =  <null>, null  =  " << is_null;
    }
    
    __XDBC_FORCED_TRACE(ORACLEPREPAREDSTATEMENT_CLASS_NAME,__sb.str());     
  }

  __XDBC_ORACLEPREPARED_CHECKPARAM(paramIndex);

  OracleBind* __oracle_bind = __vector_binds[paramIndex];
  OracleParameter* __oracle_param = __vector_params[paramIndex]; 
    
   __oracle_param->__mi_type  =  SQLT_TIMESTAMP;
  
  if( (__oracle_bind->ora_value.ora_DATETIME  ==  0) && (x !=  0))
  {
    /* Allocate the descriptor (storage) for the datatype */
    sword ret_code = OCIDescriptorAlloc(__mp_conn->oci_envhp,(dvoid **)&(__oracle_bind->ora_value.ora_DATETIME), OracleConversion::SQLT_TO_DTYPE(__vector_params[paramIndex]->__mi_type), 0, (dvoid **)0);
    OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLEPREPAREDSTATEMENT_CLASS_NAME);
    __XDBC_HOOK_CHECK(return);
  }

  if(x != 0)
  {
    OCIDateTime* ora_DATETIME = __oracle_bind->ora_value.ora_DATETIME;

    sword ret_code = OCIDateTimeConstruct((dvoid*)__mp_conn->oci_envhp,__mp_conn->oci_errhp,ora_DATETIME,
                             (sb2)x->year,(ub1)x->month, (ub1)x->day,
                             (ub1)x->hour, (ub1)x->minute,(ub1)x->second,(ub4)x->fraction,
                             (text*)0, 0);

    OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLEPREPAREDSTATEMENT_CLASS_NAME);
    __XDBC_HOOK_CHECK(return);
  }


  __XDBC_ORACLEPREPARED_CHECKBOUND_BEGIN_2(paramIndex);

  sword ret_code = OCIBindByPos(oci_stmthp,  &(__oracle_bind->oci_bndp), __mp_conn->oci_errhp,
                            paramIndex,
                            (dvoid *)&(__oracle_bind->ora_value.ora_DATETIME),
                            (sword)-1 ,
                             __oracle_param->__mi_type,
                            (dvoid *)&(__oracle_bind->__mi_bind_ind) ,
                            (ub2 *) 0, (ub2 *) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLEPREPAREDSTATEMENT_CLASS_NAME);
  
  __XDBC_ORACLEPREPARED_CHECKBOUND_END_2(paramIndex);
}

void OraclePreparedStatement::setInterval(__XDBC_HOOK_DECLARE_MORE int paramIndex,  const XDBC_INTERVAL* x,bool is_null)  __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    if(x !=  0)
    {
      __sb << "setInterval|index  =  " << paramIndex << ", value  =  " << XDBCUtilities::toString(*x) << ", null  =  " << is_null;
    }
    else
    {
      __sb << "setInterval|index  =  " << paramIndex << ", value  =  <null>, null  =  " << is_null;
    }
    
    __XDBC_FORCED_TRACE(ORACLEPREPAREDSTATEMENT_CLASS_NAME,__sb.str());       
  }

  __XDBC_ORACLEPREPARED_CHECKPARAM(paramIndex);

  ub2 param_type  =  SQLT_INTERVAL_DS;

  if(x !=  0)
  {
    param_type  =  OracleConversion::parseIntervalType(*x);
  }

  OracleBind* __oracle_bind = __vector_binds[paramIndex];
  OracleParameter* __oracle_param = __vector_params[paramIndex]; 
  
   __oracle_param->__mi_type  =  param_type;
  
  if((__oracle_bind->ora_value.ora_INTERVAL == 0 ) && (& x !=  0))
  {
    /* Allocate the descriptor (storage) for the datatype */
    sword ret_code = OCIDescriptorAlloc(__mp_conn->oci_envhp,(dvoid **)&(__oracle_bind->ora_value.ora_INTERVAL), OracleConversion::SQLT_TO_DTYPE(param_type), 0, (dvoid **)0);
    OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLEPREPAREDSTATEMENT_CLASS_NAME);
    __XDBC_HOOK_CHECK(return);
  }

  if(x  !=  0) 
  {
    OracleConversion::makeInterval(__XDBC_HOOK_INVOKE_MORE __mp_conn,*x,__oracle_bind->ora_value.ora_INTERVAL);
    __XDBC_HOOK_CHECK(return);
  }

  __XDBC_ORACLEPREPARED_CHECKBOUND_BEGIN_2(paramIndex);

  sword ret_code = OCIBindByPos(oci_stmthp,  &(__oracle_bind->oci_bndp), __mp_conn->oci_errhp,
                            paramIndex,
                            (dvoid *)&(__oracle_bind->ora_value.ora_INTERVAL),
                            (sword)-1 ,param_type,
                            (dvoid *)&(__oracle_bind->__mi_bind_ind) ,
                            (ub2 *) 0, (ub2 *) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLEPREPAREDSTATEMENT_CLASS_NAME);
  
  __XDBC_ORACLEPREPARED_CHECKBOUND_END_2(__paramIndex);
}

void OraclePreparedStatement::setBlob (__XDBC_HOOK_DECLARE_MORE int paramIndex, const char* x,int count,bool is_null)  __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    if(x !=  0)
    {
      __sb << "setBlob|index  =  " << paramIndex << ", value  =  <BLOB>[" << count << "], null  =  " << is_null;
    }
    else
    {
      __sb << "setBlob|index  =  " << paramIndex << ", value  =  <null>, null  =  " << is_null;
    }
    
    __XDBC_FORCED_TRACE(ORACLEPREPAREDSTATEMENT_CLASS_NAME,__sb.str());       
  }

  __XDBC_ORACLEPREPARED_CHECKPARAM(paramIndex);

  OracleBind* __oracle_bind = __vector_binds[paramIndex];
  OracleParameter* __oracle_param = __vector_params[paramIndex]; 
    
   __oracle_param->__mi_type  =  SQLT_LBI;
  
  if(x == 0) 
  { 
    __oracle_bind->string_value.resize(0);
  }
  else 
  {
    __oracle_bind->string_value.assign(x,count); 
  }

  __XDBC_ORACLEPREPARED_CHECKBOUND_BEGIN_1(paramIndex);
  

  sword ret_code = OCIBindByPos(oci_stmthp, &(__oracle_bind->oci_bndp), __mp_conn->oci_errhp,
                            paramIndex,
                            (dvoid *)__oracle_bind->string_value.data(),
                            (sword)__oracle_bind->string_value.size(),
                             __oracle_param->__mi_type,
                            (dvoid *)&(__oracle_bind->__mi_bind_ind) ,
                            (ub2 *) 0, (ub2 *) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLEPREPAREDSTATEMENT_CLASS_NAME);
  
  __XDBC_ORACLEPREPARED_CHECKBOUND_END_1(paramIndex);
}

void OraclePreparedStatement::setBlob (__XDBC_HOOK_DECLARE_MORE int paramIndex,const Blob* x,bool is_null)  __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    if(x !=  0)
    {
     __sb << "setBlob|index  =  " << paramIndex << ", value  =  <BLOB>[" << x->length(__XDBC_HOOK_INVOKE_ONCE)<< "], null  =  " << is_null;
    }
    else
    {
      __sb << "setBlob|index  =  " << paramIndex << ", value  =  <null>, null  =  " << is_null;
    }
    
    __XDBC_FORCED_TRACE(ORACLEPREPAREDSTATEMENT_CLASS_NAME,__sb.str()); 
  }

  __XDBC_ORACLEPREPARED_CHECKPARAM(paramIndex);

  OracleBind* __oracle_bind = __vector_binds[paramIndex];
  OracleParameter* __oracle_param = __vector_params[paramIndex]; 
    
   __oracle_param->__mi_type  =  SQLT_BLOB; 
  
  if(__oracle_bind->ora_value.ora_LOB == 0)
  {
    sword ret_code = OCIDescriptorAlloc((dvoid *)__mp_conn->oci_envhp, (dvoid **)&(__oracle_bind->ora_value.ora_LOB) ,(ub4) OCI_DTYPE_LOB, (size_t) 0, (dvoid **) 0);
    OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLEPREPAREDSTATEMENT_CLASS_NAME);
    __XDBC_HOOK_CHECK(return);
        
    ub4 lobEmpty = 0;
    ret_code = OCIAttrSet( (OCILobLocator*)__oracle_bind->ora_value.ora_LOB, OCI_DTYPE_LOB, (ub4*)&lobEmpty, sizeof(ub4),OCI_ATTR_LOBEMPTY, __mp_conn->oci_errhp);
    OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLEPREPAREDSTATEMENT_CLASS_NAME);
    __XDBC_HOOK_CHECK(return);      
  }

  if(x == 0) 
  { 
    __oracle_bind->string_value.resize(0);
  }
  else
  {
    OracleBlob* p_blob = (OracleBlob*)x;
    sword ret_code = OCILobAssign(__mp_conn->oci_envhp,__mp_conn->oci_errhp,(OCILobLocator*) p_blob->ora_LOB,(OCILobLocator**)&(__oracle_bind->ora_value.ora_LOB));
    OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLEPREPAREDSTATEMENT_CLASS_NAME);
    __XDBC_HOOK_CHECK(return);
  }


  __XDBC_ORACLEPREPARED_CHECKBOUND_BEGIN_2(paramIndex);

  sword ret_code = OCIBindByPos(oci_stmthp,&(__oracle_bind->oci_bndp), __mp_conn->oci_errhp,
                            paramIndex,
                            (dvoid *)&(__oracle_bind->ora_value.ora_LOB),
                            (sword)-1,
                             __oracle_param->__mi_type,
                            (dvoid *)&(__oracle_bind->__mi_bind_ind) ,
                            (ub2 *) 0, (ub2 *) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLEPREPAREDSTATEMENT_CLASS_NAME);

  __XDBC_ORACLEPREPARED_CHECKBOUND_END_2(paramIndex);
}

void OraclePreparedStatement::setClob (__XDBC_HOOK_DECLARE_MORE int paramIndex, const char* x,int count,bool is_null)  __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    if(x !=  0)
    {
      __sb << "setClob|index  =  " << paramIndex << ", value  =  <CLOB>[" << count << "], null  =  " << is_null;
    }
    else
    {
      __sb << "setClob|index  =  " << paramIndex << ", value  =  <null>, null  =  " << is_null;
    }
    
    __XDBC_FORCED_TRACE(ORACLEPREPAREDSTATEMENT_CLASS_NAME,__sb.str()); 
  }

  __XDBC_ORACLEPREPARED_CHECKPARAM(paramIndex);

  OracleBind* __oracle_bind = __vector_binds[paramIndex];
  OracleParameter* __oracle_param = __vector_params[paramIndex]; 
    
   __oracle_param->__mi_type  =  SQLT_LNG;
  
  if(x == 0) 
  { 
    __oracle_bind->string_value.resize(0);
  }
  else 
  {
    __oracle_bind->string_value.assign(x,count); 
  }

  __XDBC_ORACLEPREPARED_CHECKBOUND_BEGIN_1(paramIndex);

  sword ret_code = OCIBindByPos(oci_stmthp,  &(__oracle_bind->oci_bndp), __mp_conn->oci_errhp,
                            paramIndex,
                            (dvoid *)__oracle_bind->string_value.c_str(),
                            (sword)strlen(__oracle_bind->string_value.c_str()) ,
                             __oracle_param->__mi_type,
                            (dvoid *)&(__oracle_bind->__mi_bind_ind) ,
                            (ub2 *) 0, (ub2 *) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLEPREPAREDSTATEMENT_CLASS_NAME);

  __XDBC_ORACLEPREPARED_CHECKBOUND_END_1(paramIndex);
}

void OraclePreparedStatement::setClob (__XDBC_HOOK_DECLARE_MORE int paramIndex,const Clob* x,bool is_null)  __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    if(x !=  0)
    {
      __sb << "setClob|index  =  " << paramIndex << ", value  =  <CLOB>[" << x->length(__XDBC_HOOK_INVOKE_ONCE)<< "], null  =  " << is_null;
    }
    else
    {
      __sb << "setClob|index  =  " << paramIndex << ", value  =  <null>, null  =  " << is_null;
    }
    
    __XDBC_FORCED_TRACE(ORACLEPREPAREDSTATEMENT_CLASS_NAME,__sb.str()); 
  }

  __XDBC_ORACLEPREPARED_CHECKPARAM(paramIndex);

  OracleBind* __oracle_bind = __vector_binds[paramIndex];
  OracleParameter* __oracle_param = __vector_params[paramIndex]; 
    
   __oracle_param->__mi_type  =  SQLT_CLOB;
  
  if(__oracle_bind->ora_value.ora_LOB == 0 )
  {
    sword ret_code = OCIDescriptorAlloc((dvoid *)__mp_conn->oci_envhp, (dvoid **)&(__oracle_bind->ora_value.ora_LOB) ,(ub4) OCI_DTYPE_LOB, (size_t) 0, (dvoid **) 0);

    ub4 lobEmpty = 0;
    ret_code = OCIAttrSet( (OCILobLocator*)__oracle_bind->ora_value.ora_LOB, OCI_DTYPE_LOB, (ub4*)&lobEmpty, sizeof(ub4),OCI_ATTR_LOBEMPTY, __mp_conn->oci_errhp);
    OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLEPREPAREDSTATEMENT_CLASS_NAME);
    __XDBC_HOOK_CHECK(return);
  }

  if(x != 0)
  {
    OracleClob* p_clob = (OracleClob*)x;
    sword ret_code = OCILobAssign(__mp_conn->oci_envhp,__mp_conn->oci_errhp,(OCILobLocator*) p_clob->ora_LOB,(OCILobLocator**)&(__oracle_bind->ora_value.ora_LOB));
    OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLEPREPAREDSTATEMENT_CLASS_NAME);
    __XDBC_HOOK_CHECK(return);
  }

  __XDBC_ORACLEPREPARED_CHECKBOUND_BEGIN_2(paramIndex);

  sword ret_code = OCIBindByPos(oci_stmthp, &(__oracle_bind->oci_bndp), __mp_conn->oci_errhp,
                            paramIndex,
                            (dvoid *)&(__oracle_bind->ora_value.ora_LOB),
                            (sword)-1 ,
                             __oracle_param->__mi_type,
                            (dvoid *)&(__oracle_bind->__mi_bind_ind) ,
                            (ub2 *) 0, (ub2 *) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLEPREPAREDSTATEMENT_CLASS_NAME);

  __XDBC_ORACLEPREPARED_CHECKBOUND_END_2(paramIndex);
}

//---------------------------------------------------------------------
// Statement Interfaces
//---------------------------------------------------------------------
void  OraclePreparedStatement::close()
{
  __XDBC_TRACE(ORACLEPREPAREDSTATEMENT_CLASS_NAME,"close()");
  if(__mb_isClosed)
  {
    return;
  }

  __mb_isClosed=true;

  for(int i = 1;i<= __mi_param_count;++i)
  {
    if(__vector_binds[i] != 0)
    {
    //  __vector_binds[i]->cleanup(*__vector_params[i]);
      delete __vector_binds[i];
    }
  }
  __vector_binds.clear();

  for(int i = 1;i<= __mi_param_count;++i)
  {
    if(__vector_params[i] != 0)
    {
      delete __vector_params[i];
    }
  }
  __vector_params.clear();
  
#if !defined(OPENCODE_HAS_STMTPREPARE2)
  if(oci_stmthp!=0)
  {
    OCIHandleFree((dvoid*)oci_stmthp,OCI_HTYPE_STMT);
    oci_stmthp=0;
  }
#else
  OCIStmtRelease(oci_stmthp, __mp_conn->oci_errhp,
                (text*)str_orginal_oracle_sql.c_str(), (ub4) str_orginal_oracle_sql.length(),     
                OCI_DEFAULT); 
#endif
}

void  OraclePreparedStatement::destroy()
{
  if(__adoptIt && (__cachedCollection != 0))
  {
    __XDBC_HOOK_DEFINE();
    OracleResultSet::releaseColumnData(__XDBC_HOOK_INVOKE_MORE __mp_conn,oci_stmthp,__cachedCollection); 
    delete __cachedCollection;
  }
  
  if(__p_rs_opened != 0)
  {
    __p_rs_opened->__mb_selfDelete = true;
    
   __XDBC_INTERFACE_CLOSEANDDESTROY(__p_rs_opened);
  }
      
  delete this;
}

int OraclePreparedStatement::getResultSetType(__XDBC_HOOK_DECLARE_ONCE)   __XDBC_THROW_DECLARE(SQLException)
{
  return __mi_resultSetType;
}

Connection* OraclePreparedStatement::getConnection(__XDBC_HOOK_DECLARE_ONCE)   __XDBC_THROW_DECLARE(SQLException)
{
  return (Connection*)__mp_conn;
}

__OPENCODE_END_NAMESPACE
