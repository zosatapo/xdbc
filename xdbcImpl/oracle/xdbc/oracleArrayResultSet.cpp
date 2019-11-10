#include "xdbc/oracleArrayResultSet.h"
#include "xdbc/oracleConnection.h"
#include "xdbc/oracleArrayStatement.h"
#include "xdbc/oracleResultSetMetaData.h"

#include "xdbc/oracleClob.h"
#include "xdbc/oracleBlob.h"
#include "xdbc/oracleConversion.h"
#include "xdbc/oracleTypes.h"

#include "port/utilities.h"
#include "port/ioc_sbuf.h"

__OPENCODE_BEGIN_NAMESPACE


#define ORACLEARRAYRESULTSET_CLASS_NAME "OracleArrayResultSet"


OracleArrayResultSet::~OracleArrayResultSet() 
{ 
  __XDBC_TRACE(ORACLEARRAYRESULTSET_CLASS_NAME,"destroy()");
}

OracleArrayResultSet::OracleArrayResultSet(OracleArrayStatement* pStmt,int resultSetType, int resultSetConcurrency,DataTable* __mp_dataTable,bool isEmpty)
{
  __mp_statement=pStmt;

  __mi_resultSetType=resultSetType;
  __mi_resultSetConcurrency=resultSetConcurrency;

  this->__mp_dataTable = __mp_dataTable;

  __mb_isEmpty=false;
  __mb_isClosed=false;
  __mb_eof =  false;
  __mi_column_count = 0;
}

void  OracleArrayResultSet::initialize(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{
  if(__mb_isEmpty)
  {
    return;
  }

//-------------------------------------------------------------------------
// 1. fetch  ResultSetMetaData
//-------------------------------------------------------------------------
  OracleConversion::parseResultSetMetaData(__XDBC_HOOK_INVOKE_MORE __mp_statement->__mp_conn->oci_errhp,__mp_statement->oci_stmthp,__vector_columns);
  __XDBC_HOOK_CHECK(return);
  
  __mi_column_count = (int)(__vector_columns.size() - 1);
//-------------------------------------------------------------------------
// 2. define DataTable Data 
//-------------------------------------------------------------------------
  __mi_array_size = 1;

  int rowcount = 0;
  int colcount = 0;

  int __mi_max_size = __mp_statement->getMaxArraySize();

  if(__mp_dataTable != 0)
  {
    rowcount = __mp_dataTable->getRowCount();
    colcount = __mp_dataTable->getColumnCount();

    __mi_array_size = __M_min(__mi_max_size,rowcount);
  }

  if(DriverManager::isTracingOn)
  {
     StringBuffer __sb;
     __sb<<"executeQuery|datarows[s/r/m] = ["<<__mi_array_size
       <<"/" <<rowcount<<"/"<<__mi_max_size <<"] , cols = ["<<colcount<<"]";

     __XDBC_FORCED_TRACE(ORACLEARRAYRESULTSET_CLASS_NAME,__sb.str());
  }

  if(__mp_dataTable != 0)
  {
    setDefineData(__XDBC_HOOK_INVOKE_MORE __mp_dataTable);
    __XDBC_HOOK_CHECK(return);
  }
}

int OracleArrayResultSet::next(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{
  if(__mb_isEmpty || __mb_eof)
  {
    return 0;
  }

  checkClosed(__XDBC_HOOK_INVOKE_ONCE);
  __XDBC_HOOK_CHECK(return 0);

  sword  ret_code=OCIStmtFetch2(__mp_statement->oci_stmthp,__mp_statement->__mp_conn->oci_errhp,
                               (ub4)__mi_array_size,
                               OCI_FETCH_NEXT,
                               (ub4)0,
                               OCI_DEFAULT);

  if(ret_code != OCI_NO_DATA)
  {
    checkException(__XDBC_HOOK_INVOKE_MORE __mp_statement->__mp_conn->oci_errhp,ret_code);
    __XDBC_HOOK_CHECK(return 0);
  }

  ub4 rowsFetched = 0;
  ub4 size = sizeof(rowsFetched);
  OCIAttrGet ( (dvoid *) __mp_statement->oci_stmthp,(ub4) OCI_HTYPE_STMT, (dvoid*)&rowsFetched, (ub4*)&size, (ub4) OCI_ATTR_ROWS_FETCHED, __mp_statement->__mp_conn->oci_errhp);
  
  if(rowsFetched < __mi_array_size)
  {
    __mb_eof = true;
  } 

  return rowsFetched;
}

void OracleArrayResultSet::close()
{
  __XDBC_TRACE(ORACLEARRAYRESULTSET_CLASS_NAME,"close()");

  if(__mb_isClosed)
  {
    return;
  }

  size_t defsize = __vector_defines.size();
  for(size_t i = 0;i < defsize;++i)
  {
    delete __vector_defines[i];
  }
  __vector_defines.clear();

  __mb_isClosed=true; 
}

void OracleArrayResultSet::destroy()
{
  delete this;
}
  
ResultSetMetaData* OracleArrayResultSet::getMetaData(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{
  return new  OracleResultSetMetaData(__vector_columns);  
}

int OracleArrayResultSet::findColumn(__XDBC_HOOK_DECLARE_MORE const string& columnName)  __XDBC_THROW_DECLARE(SQLException)
{
  if(__map_columns.empty())
  {
    for(int i = 1;i <= __mi_column_count; ++i)
    {
      __map_columns[StringsKit::toUpperCase(__vector_columns[i].__ms_name)] = i;
    }    
  }  
  
  OPENCODE_MAP<string,int>::iterator __pos = __map_columns.find(StringsKit::toUpperCase(columnName));
  if(__pos != __map_columns.end())
  {
    return __pos->second;
  } 

  StringBuffer __sb;
  __sb.append("[OracleArrayResultSet] Column '");
  __sb.append(columnName);
  __sb.append("' not found in SELECT-LIST");
  
  __XDBC_HOOK_RETURN(return -1,__sb.str());
}

void   OracleArrayResultSet::checkException(__XDBC_HOOK_DECLARE_MORE OCIError *errhp,sword status) __XDBC_THROW_DECLARE(SQLException)
{
  if((status == OCI_ERROR) &&(__mp_dataTable->getModel()->getIndicatorBuffer() == 0))
  {
    sb4 errcode = 0;
    OCIErrorGet((dvoid *)errhp, (ub4) 1, (text *) NULL, &errcode,0,(ub4)0,OCI_HTYPE_ERROR);
    if(errcode == 1405)
    {
      __XDBC_TRACE(ORACLEARRAYRESULTSET_CLASS_NAME,"In an OCI program, a FETCH operation returned a NULL column value. The column buffer in the program remained unchanged and the cursor return code was +2");
      return;
    }
  }

  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE errhp,status,ORACLEARRAYRESULTSET_CLASS_NAME);
}

void   OracleArrayResultSet::checkClosed(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException)
{
  if(__mb_isClosed)
  {
    __XDBC_HOOK_RETURN(return,"[OracleArrayResultSet] array resultset has always been closed");
  }
}

int    OracleArrayResultSet::findDefineIndex(int column)
{
  int size = (int)(__vector_defines.size());
  for(int i=0;i<size;++i)
  {
    if(__vector_defines[i]->__mi_index==column)
    {
       return i;
    }
  }
 
  return -1;
}

void  OracleArrayResultSet::setDefineData(__XDBC_HOOK_DECLARE_MORE DataTable* __mp_dataTable)  __XDBC_THROW_DECLARE(SQLException)
{
  int colcount = __mp_dataTable->getColumnCount();

  for(int i=1; i<=colcount;++i)
  {
    setDefineData(__XDBC_HOOK_INVOKE_MORE __mp_dataTable,i);
    __XDBC_HOOK_CHECK(return);
  }
}

void  OracleArrayResultSet::setDefineData(__XDBC_HOOK_DECLARE_MORE DataTable* __mp_dataTable,int column)  __XDBC_THROW_DECLARE(SQLException)
{
  int define_index=findDefineIndex(column);
  if(define_index==-1)
  {
    OracleDefine* oracle_define = new OracleDefine();
    oracle_define->__mi_index=column;
    __vector_defines.push_back(oracle_define);
    define_index=findDefineIndex(column);
  }

  const char* colData = __mp_dataTable->getModel()->getDataBuffer() + __mp_dataTable->getColumnOffset(column);
  int colSize =  __mp_dataTable->getColumnSize(column);
  int colSkip =  __mp_dataTable->getColumnSkip(column); 
  int colType =0;

  if(__mp_dataTable->getModel()->useColumnRawType(column))
  {
    colType =  __mp_dataTable->getModel()->getColumnRawType(column);
  }
  else
  {
    colType = OracleConversion::XDBC_TO_SQLT(__mp_dataTable->getColumnType(column));
  }

  const char* indData = 0;
  int indSkip = 0;

  if(__mp_dataTable->getModel()->useColumnIndicator(column))
  {
    indData=__mp_dataTable->getModel()->getIndicatorBuffer()+__mp_dataTable->getModel()->getColumnIndicatorOffset(column);
    indSkip = __mp_dataTable->getModel()->getColumnIndicatorSkip(column);
  }

  const char* lenData = 0;
  int lenSkip = 0;

  if(__mp_dataTable->getModel()->useColumnLength(column))
  {
    lenData=__mp_dataTable->getModel()->getLengthBuffer()+__mp_dataTable->getModel()->getColumnLengthOffset(column);
    lenSkip = __mp_dataTable->getModel()->getColumnLengthSkip(column);
  }

  const char* codeData = 0;
  int codeSkip = 0;

  if(__mp_dataTable->getModel()->useColumnCode(column))
  {
    codeData=__mp_dataTable->getModel()->getCodeBuffer()+__mp_dataTable->getModel()->getColumnCodeOffset(column);
    codeSkip = __mp_dataTable->getModel()->getColumnCodeSkip(column);
  }

  sword ret_code = 0;
  ret_code = OCIDefineByPos( __mp_statement->oci_stmthp, &(__vector_defines[define_index]->oci_defnp), 
                             __mp_statement->__mp_conn->oci_errhp,
                             column, (dvoid *)colData , (sword) colSize,colType,
                             (dvoid *)indData , (ub2 *)lenData, (ub2 *) codeData, OCI_DEFAULT);

  checkException(__XDBC_HOOK_INVOKE_MORE  __mp_statement->__mp_conn->oci_errhp,ret_code);
  __XDBC_HOOK_CHECK(return);
  
  ret_code = OCIDefineArrayOfStruct(__vector_defines[define_index]->oci_defnp,  __mp_statement->__mp_conn->oci_errhp,
          colSkip,indSkip,lenSkip,codeSkip);
  checkException(__XDBC_HOOK_INVOKE_MORE  __mp_statement->__mp_conn->oci_errhp,ret_code);
}

__OPENCODE_END_NAMESPACE
