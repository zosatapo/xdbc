#include "xdbc/oracleArrayStatement.h"
#include "xdbc/oracleConnection.h"
#include "xdbc/oracleResultSet.h"
#include "xdbc/oracleArrayResultSet.h"
#include "xdbc/oracleConversion.h"
#include "xdbc/dataTable.h"

#include "xdbc/oracleClob.h"
#include "xdbc/oracleBlob.h"

#include "xdbc/oracleSQL.h"
#include "xdbc/oracleConversion.h"
#include "xdbc/xsmart.h"

#include "port/ioc_sbuf.h"
#include "port/utilities.h"


__OPENCODE_BEGIN_NAMESPACE


#define ORACLEARRAYSTATEMENT_CLASS_NAME "OracleArrayStatement"

const int  OracleArrayStatement::MAX_ROWCOUNT = 30000;

OracleArrayStatement::~OracleArrayStatement()
{
  __XDBC_TRACE(ORACLEARRAYSTATEMENT_CLASS_NAME,"destroy()"); 
}

OracleArrayStatement::OracleArrayStatement(OracleConnection* ora_conn,const string& sql,int paramCount)
{ 
  __mp_conn=ora_conn;
  
  __mi_resultSetType=ResultSet::TYPE_FORWARD_ONLY;
  __mi_resultSetConcurrency=ResultSet::CONCUR_READ_ONLY;
  
  oci_stmthp=0;
  __mb_initialized=false;
  __mb_isClosed=false;
    
  str_orginal_oracle_sql = sql;
  str_oracle_sql = sql;

  __mi_param_count = paramCount;
  
  __mi_array_size = 1;
  __mi_max_size = MAX_ROWCOUNT; 
}

OracleArrayStatement::OracleArrayStatement(OracleConnection* ora_conn,const string& sql, int resultSetType, int resultSetConcurrency,int paramCount)
{
  __mp_conn=ora_conn;

  __mi_resultSetType=resultSetType;
  __mi_resultSetConcurrency=resultSetConcurrency;
  
  oci_stmthp=0;
  __mb_initialized=false;
  __mb_isClosed=false;
    
  str_orginal_oracle_sql = sql;
  str_oracle_sql = sql;
  
  __mi_param_count = paramCount;

  __mi_array_size = 1;
  __mi_max_size = MAX_ROWCOUNT;
}

void OracleArrayStatement::initialize(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{ 
  if(__mb_initialized)
  {
    return;
  }  
  
  __mb_initialized=true;
  sword ret_code = OCIHandleAlloc( (dvoid *) __mp_conn->oci_envhp, (dvoid **) &oci_stmthp,OCI_HTYPE_STMT, (size_t) 0, (dvoid **) 0);
  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLEARRAYSTATEMENT_CLASS_NAME);
  __XDBC_HOOK_CHECK(return);
  
  if(__mi_param_count == SQLImplementation::paramCountUnknown)
  {
    str_oracle_sql=OracleSQL::parseSQL(__XDBC_HOOK_INVOKE_MORE str_oracle_sql,__mi_param_count);
    __XDBC_HOOK_CHECK(return);
  }
  
  const char* char_sql=str_oracle_sql.c_str();
  ret_code = OCIStmtPrepare(oci_stmthp, __mp_conn->oci_errhp, (text*)char_sql, (ub4) strlen(char_sql), (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLEARRAYSTATEMENT_CLASS_NAME);
}

int OracleArrayStatement::executeUpdate(__XDBC_HOOK_DECLARE_MORE DataTable* paramTable,vector<ArrayException>* errors)  __XDBC_THROW_DECLARE(SQLException)
{
  __mi_array_size = 1;

  int rowcount = 0;
  int colcount = 0;

  if(paramTable != 0)
  {
    rowcount = paramTable->getRowCount();
    colcount = paramTable->getColumnCount();

    __mi_array_size = __M_min(__mi_max_size,rowcount);
  }
  
  if(DriverManager::isTracingOn)
  {
     StringBuffer __sb;
     __sb<<"executeUpdate|datarows[s/r/m] = ["<<__mi_array_size
       <<"/" <<rowcount<<"/"<<__mi_max_size <<"] , cols = ["<<colcount<<"]";

     __XDBC_FORCED_TRACE(ORACLEARRAYSTATEMENT_CLASS_NAME,__sb.str());
  }

  if(paramTable != 0)
  {
    setBindData(__XDBC_HOOK_INVOKE_MORE paramTable);
    __XDBC_HOOK_CHECK(return);
  }

  sword ret_code = 0;

  if(__mp_conn->__mb_autoCommit)
  {
     ret_code=OCIStmtExecute(__mp_conn->oci_svchp, oci_stmthp, __mp_conn->oci_errhp,
                            (ub4) __mi_array_size, (ub4) 0, (CONST OCISnapshot *) 0, (OCISnapshot *) 0,
                            OCI_DEFAULT|OCI_BATCH_ERRORS|OCI_COMMIT_ON_SUCCESS);
  }
  else
  {

     ret_code=OCIStmtExecute(__mp_conn->oci_svchp, oci_stmthp, __mp_conn->oci_errhp,
                            (ub4) __mi_array_size, (ub4) 0, (CONST OCISnapshot *) 0, (OCISnapshot *) 0,
                            OCI_DEFAULT|OCI_BATCH_ERRORS);
  }
  
  __OPENCODE_TRY
  { 
    OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLEARRAYSTATEMENT_CLASS_NAME);
    if(!__XDBC_HOOK_INVOKE_CHECKPASSED)
    {
      if(errors != 0)
      {
        ub4 num_errs = 0;
        /* get the number of errors */
        ret_code=OCIAttrGet(oci_stmthp, OCI_HTYPE_STMT, &num_errs, 0, OCI_ATTR_NUM_DML_ERRORS, __mp_conn->oci_errhp);
        OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLEARRAYSTATEMENT_CLASS_NAME);
        __XDBC_HOOK_CHECK(return 0);
         
        if(num_errs !=0)
        {
          for(int i=0;i<num_errs;++i)
          {
            OCIError *errhp2 = 0;
            ub4 row_offset = 0;
            text errbuf[1024];
            sb4 errcode = 0;
    
            OCIHandleAlloc((dvoid *)__mp_conn->oci_envhp, (dvoid **)&errhp2, (ub4) OCI_HTYPE_ERROR, 0, (dvoid **) 0);
            OCIParamGet(__mp_conn->oci_errhp, OCI_HTYPE_ERROR,  __mp_conn->oci_errhp, (dvoid**)&errhp2, i);
            OCIAttrGet (errhp2, OCI_HTYPE_ERROR, &row_offset, 0, OCI_ATTR_DML_ROW_OFFSET, __mp_conn->oci_errhp);
            OCIErrorGet((dvoid *)errhp2, (ub4) 1, (text *) NULL, &errcode,errbuf,(ub4) sizeof(errbuf),OCI_HTYPE_ERROR);
            errors->push_back(ArrayException(row_offset,SQLException(string()+"[OracleArrayStatement] - "+(const char*)errbuf,errcode)));
    
            OCIHandleFree((dvoid *) errhp2, OCI_HTYPE_ERROR);
    
          }//~end for(int i=0;i<num_errs;++i)
    
        }//~end if(num_errs !=0)
    
      }//~end if(errors !=0)    
    }
  }
  __OPENCODE_CATCH(SQLException& ex)
  {
    if(errors != 0)
    {
      ub4 num_errs = 0;
      /* get the number of errors */
      ret_code=OCIAttrGet(oci_stmthp, OCI_HTYPE_STMT, &num_errs, 0, OCI_ATTR_NUM_DML_ERRORS, __mp_conn->oci_errhp);
      OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLEARRAYSTATEMENT_CLASS_NAME);
      __XDBC_HOOK_CHECK(return 0);
      
      if(num_errs !=0)
      {
        for(int i=0;i<num_errs;++i)
        {
          OCIError *errhp2 = 0;
          ub4 row_offset = 0;
          text errbuf[1024];
          sb4 errcode = 0;
  
          OCIHandleAlloc((dvoid *)__mp_conn->oci_envhp, (dvoid **)&errhp2, (ub4) OCI_HTYPE_ERROR, 0, (dvoid **) 0);
          OCIParamGet(__mp_conn->oci_errhp, OCI_HTYPE_ERROR,  __mp_conn->oci_errhp, (dvoid**)&errhp2, i);
          OCIAttrGet (errhp2, OCI_HTYPE_ERROR, &row_offset, 0, OCI_ATTR_DML_ROW_OFFSET, __mp_conn->oci_errhp);
          OCIErrorGet((dvoid *)errhp2, (ub4) 1, (text *) NULL, &errcode,errbuf,(ub4) sizeof(errbuf),OCI_HTYPE_ERROR);
          errors->push_back(ArrayException(row_offset,SQLException(string()+"[OracleArrayStatement] - "+(const char*)errbuf,errcode)));

          OCIHandleFree((dvoid *) errhp2, OCI_HTYPE_ERROR);

        }//~end for(int i=0;i<num_errs;++i)

      }//~end if(num_errs !=0)

    }//~end if(errors !=0)

    __OPENCODE_RETHROW;
  }

  ub4 rowsFetched = 0;
  ub4 size = sizeof(rowsFetched);
  OCIAttrGet( (dvoid *) oci_stmthp,(ub4) OCI_HTYPE_STMT, (dvoid*)&rowsFetched, (ub4*)&size, (ub4) OCI_ATTR_ROW_COUNT, __mp_conn->oci_errhp);

  return rowsFetched;
}

ArrayResultSet*  OracleArrayStatement::executeQuery(__XDBC_HOOK_DECLARE_MORE DataTable* __mp_dataTable,DataTable* paramTable)  __XDBC_THROW_DECLARE(SQLException)
{
  int rowcount = 0;
  int colcount = 0;
  int paramrowcount = 0;

  if(paramTable != 0)
  {
    rowcount = paramTable->getRowCount();
    colcount = paramTable->getColumnCount();
    paramrowcount = __M_min(1,rowcount);
  }

  if(DriverManager::isTracingOn)
  {
     StringBuffer __sb;
     __sb<<"executeQuery|paramrows[s/r] = [" <<paramrowcount<<"/"<< rowcount <<"]";

     __XDBC_FORCED_TRACE(ORACLEARRAYSTATEMENT_CLASS_NAME,__sb.str());
  }

  if(paramTable != 0)
  {
    setBindData(__XDBC_HOOK_INVOKE_MORE paramTable);
    __XDBC_HOOK_CHECK(return);
  }

  sword ret_code=0;

  if(__mi_resultSetType == ResultSet::TYPE_FORWARD_ONLY)
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

  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLEARRAYSTATEMENT_CLASS_NAME);
  __XDBC_HOOK_CHECK(return 0);
  
  OracleArrayResultSet* p_rs=0;

  if(ret_code == OCI_NO_DATA)
  {
    p_rs = new OracleArrayResultSet(this,__mi_resultSetType,__mi_resultSetConcurrency,__mp_dataTable,true);
  }
  else
  {
    p_rs = new OracleArrayResultSet(this,__mi_resultSetType,__mi_resultSetConcurrency,__mp_dataTable,false);
  }

  xdbc_smart_ptr<OracleArrayResultSet> __sp_rs(p_rs);
  
  p_rs->initialize(__XDBC_HOOK_INVOKE_ONCE);
  __XDBC_HOOK_CHECK(return);
  
  return __sp_rs.release();
}

//---------------------------------------------------------------------
// Statement Interfaces
//---------------------------------------------------------------------
void  OracleArrayStatement::close()
{
  __XDBC_TRACE(ORACLEARRAYSTATEMENT_CLASS_NAME,"close()");

  if(__mb_isClosed)
  {
    return;
  }
  __mb_isClosed=true;
 
  int size = (int)(__vector_binds.size());

  for(int i=0;i<size;++i)
  {
    delete __vector_binds[i];   
  }
  __vector_binds.clear();

  if(oci_stmthp!=0)
  {
    OCIHandleFree((dvoid*)oci_stmthp,OCI_HTYPE_STMT);
    oci_stmthp=0;
  } 
}

void  OracleArrayStatement::destroy()
{
  delete this;
}
  
int OracleArrayStatement::getResultSetType(__XDBC_HOOK_DECLARE_ONCE)   __XDBC_THROW_DECLARE(SQLException)
{
  return __mi_resultSetType;
}

Connection* OracleArrayStatement::getConnection(__XDBC_HOOK_DECLARE_ONCE)   __XDBC_THROW_DECLARE(SQLException)
{
  return (Connection*)__mp_conn;
}

int  OracleArrayStatement::getMaxArraySize() const
{
  return __mi_max_size;
}

void  OracleArrayStatement::setMaxArraySize(int size)
{
  __mi_max_size = size;
}

int OracleArrayStatement::findBindIndex(int parameterIndex)
{
  int size= (int)(__vector_binds.size());

  for(int i=0;i<size;++i)
  {
    if(__vector_binds[i]->__mi_index==parameterIndex)
    {
      return i;
    } 
  }
  
  return -1;
}


void OracleArrayStatement::setBindData(__XDBC_HOOK_DECLARE_MORE DataTable* paramTable)  __XDBC_THROW_DECLARE(SQLException)
{
  int colcount = paramTable->getColumnCount();

  for(int i=1; i<=colcount;++i)
  {
    setBindData(__XDBC_HOOK_INVOKE_MORE paramTable,i);
    __XDBC_HOOK_CHECK(return);
  }
}

void  OracleArrayStatement::setBindData(__XDBC_HOOK_DECLARE_MORE DataTable* paramTable,int parameterIndex)  __XDBC_THROW_DECLARE(SQLException)
{
  int bind_index=findBindIndex(parameterIndex);
  if(bind_index==-1)
  {
    OracleBind* oracle_bind = new OracleBind();
    oracle_bind->__mi_index=parameterIndex;
    __vector_binds.push_back(oracle_bind);
    bind_index=findBindIndex(parameterIndex);
  }

  const char* colData = paramTable->getModel()->getDataBuffer() + paramTable->getColumnOffset(parameterIndex);
  int colSize =  paramTable->getColumnSize(parameterIndex);
  int colSkip =  paramTable->getColumnSkip(parameterIndex); 
  int colType =0;

  if(paramTable->getModel()->useColumnRawType(parameterIndex))
  {
    colType =  paramTable->getModel()->getColumnRawType(parameterIndex);
  }
  else
  {
    colType = OracleConversion::XDBC_TO_SQLT(paramTable->getColumnType(parameterIndex));
  }

  const char* indData = 0;
  int indSkip = 0;

  if(paramTable->getModel()->useColumnIndicator(parameterIndex))
  {
    indData = paramTable->getModel()->getIndicatorBuffer()+paramTable->getModel()->getColumnIndicatorOffset(parameterIndex);
    indSkip = paramTable->getModel()->getColumnIndicatorSkip(parameterIndex);
  }

  const char* lenData = 0;
  int lenSkip = 0;

  if(paramTable->getModel()->useColumnLength(parameterIndex))
  {
    lenData = paramTable->getModel()->getLengthBuffer()+paramTable->getModel()->getColumnLengthOffset(parameterIndex);
    lenSkip = paramTable->getModel()->getColumnLengthSkip(parameterIndex);
  }

  const char* codeData = 0;
  int codeSkip = 0;

  if(paramTable->getModel()->useColumnCode(parameterIndex))
  {
    codeData = paramTable->getModel()->getCodeBuffer()+paramTable->getModel()->getColumnCodeOffset(parameterIndex);
    codeSkip = paramTable->getModel()->getColumnCodeSkip(parameterIndex);
  }

  sword ret_code = 0;
  ret_code = OCIBindByPos(oci_stmthp, &(__vector_binds[bind_index]->oci_bndp), __mp_conn->oci_errhp,
                          parameterIndex,
                          (dvoid *)colData ,
                          (sword) colSize,colType,
                          (dvoid *)indData , (ub2 *)lenData, (ub2 *) codeData,
                          (ub4) 0, (ub4 *) 0, OCI_DEFAULT);
  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLEARRAYSTATEMENT_CLASS_NAME);
  __XDBC_HOOK_CHECK(return);
    
  ret_code = OCIBindArrayOfStruct(__vector_binds[bind_index]->oci_bndp, __mp_conn->oci_errhp,
                                  colSkip,indSkip,lenSkip,codeSkip);
  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE __mp_conn->oci_errhp,ret_code,ORACLEARRAYSTATEMENT_CLASS_NAME);
}

__OPENCODE_END_NAMESPACE
