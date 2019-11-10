#include "xdbc/ifxPreparedStatement.h"
#include "xdbc/ifxConnection.h"
#include "xdbc/ifxResultSet.h"
#include "xdbc/ifxConversion.h"
#include "xdbc/xsmart.h"
#include "xdbc/ifxClob.h"
#include "xdbc/ifxBlob.h"
#include "xdbc/ifxSQL.h"

#include "port/ioc_sbuf.h"
#include "port/file.h"
#include "port/ioc_map.h"

__OPENCODE_BEGIN_NAMESPACE

//-------------------------------------------------------------------------

#define IFXPREPAREDSTATEMENT_CLASS_NAME "IfxPreparedStatement"


IfxPreparedStatement::~IfxPreparedStatement()
{
  __XDBC_TRACE(IFXPREPAREDSTATEMENT_CLASS_NAME,"destroy()"); 
}

IfxPreparedStatement::IfxPreparedStatement(IfxConnection* ora_conn,const string& sql,int paramCount)
{
  __mp_conn=ora_conn;
  
  __mi_resultSetType=ResultSet::TYPE_FORWARD_ONLY;
  __mi_resultSetConcurrency=ResultSet::CONCUR_READ_ONLY;

  str_orginal_ifx_sql=sql;
  str_ifx_sql=sql;  

  __mi_var_count= paramCount;
  __mp_ec_sqlda_vars = 0;

  __mp_var_buffer = 0;
  __mi_var_buf_size = 0;
}

IfxPreparedStatement::IfxPreparedStatement(IfxConnection* ora_conn,const string& sql, int resultSetType, int resultSetConcurrency,int paramCount)
{
  __mp_conn=ora_conn;

  __mi_resultSetType=resultSetType;
  __mi_resultSetConcurrency=resultSetConcurrency;
  
  str_orginal_ifx_sql=sql;
  str_ifx_sql=sql;  

   __mi_var_count=paramCount;
   __mp_ec_sqlda_vars = 0;

   __mp_var_buffer = 0;
   __mi_var_buf_size = 0;
}

void IfxPreparedStatement::initialize(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{ 
  if(__mb_initialized)
  {  
    return;  
  }

  initialize(__XDBC_HOOK_INVOKE_ONCE);
  __XDBC_HOOK_CHECK(return);
  
  if(__mi_var_count== SQLImplementation::paramCountUnknown)
  {
    str_ifx_sql=IfxSQL::parseSQL(__XDBC_HOOK_INVOKE_MORE str_orginal_ifx_sql,__mi_var_count);
    __XDBC_HOOK_CHECK(return);
  }
  
  __mi_stmt_type=IfxSQL::parseSQLKind(str_ifx_sql);

  string cursor_name("ifx_xdbc_cursor_name_");
  string cursor_id("ifx_xdbc_cursor_id_");

  string var_cursor_name("ifx_xdbc_var_cursor_name_");
  string var_cursor_id("ifx_xdbc_var_cursor_id_");

  int cursorID = IfxStatement::getCursorCounter();

  StringBuffer __sb;
  __sb.append(cursorID);

  cursor_name = cursor_name + __sb.str();
  cursor_id = cursor_id + __sb.str();

  __ms_cursor_name = cursor_name;
  __ms_cursor_id   = cursor_id;

  str_var_cursor_name = var_cursor_name;
  str_var_cursor_id   = var_cursor_id;

  EXEC SQL BEGIN DECLARE SECTION;
    char*  ec_select_sql;
    char*  ec_cursor_id;
  EXEC SQL END DECLARE SECTION;

  ec_select_sql   = (char*)str_ifx_sql.c_str();
  ec_cursor_id    = (char*)cursor_id.c_str();
  
  EXEC SQL PREPARE :ec_cursor_id FROM :ec_select_sql;
  IfxConversion::checkException(__XDBC_HOOK_INVOKE_MORE SQLCODE,SQLSTATE,IFXPREPAREDSTATEMENT_CLASS_NAME);
  __XDBC_HOOK_CHECK(return);
  
  parseInColumns(__XDBC_HOOK_INVOKE_ONCE);
}

ResultSet* IfxPreparedStatement::executeQuery(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{
  __XDBC_TRACE(IFXPREPAREDSTATEMENT_CLASS_NAME,"executeQuery()");
  
  ResultSetCollection* cachedCollection = 0;
  
  EXEC SQL BEGIN DECLARE SECTION;
    char*  ec_cursor_name;
    char*  ec_cursor_id;
  EXEC SQL END DECLARE SECTION;

  ec_cursor_name  = (char*)__ms_cursor_name.c_str();
  ec_cursor_id    = (char*)__ms_cursor_id.c_str();

  if(__mi_resultSetType == ResultSet::TYPE_SCROLL_INSENSITIVE)
  {
    EXEC SQL DECLARE :ec_cursor_name SCROLL CURSOR FOR :ec_cursor_id ;
  }
  else if (__mi_resultSetType == ResultSet::TYPE_SCROLL_SENSITIVE)
  {
    EXEC SQL DECLARE :ec_cursor_name SCROLL CURSOR FOR :ec_cursor_id ;
  }
  else
  {
    EXEC SQL DECLARE :ec_cursor_name CURSOR FOR :ec_cursor_id ;
  }

  if(__mi_var_count== 0)
  {
     EXEC SQL OPEN :ec_cursor_name;
  }
  else
  {
     EXEC SQL OPEN :ec_cursor_name  USING DESCRIPTOR __mp_ec_sqlda_vars;
  }

  IfxConversion::checkException(__XDBC_HOOK_INVOKE_MORE SQLCODE,SQLSTATE,IFXPREPAREDSTATEMENT_CLASS_NAME);
  __XDBC_HOOK_CHECK(return 0);
   
  IfxResultSet* p_rs=new IfxResultSet(__mp_conn,this,__ms_cursor_name,__ms_cursor_id,__mi_resultSetType,__mi_resultSetConcurrency,false,cachedCollection);
  xdbc_smart_ptr<IfxResultSet> __sp_rs(p_rs);
  
  p_rs->initialize(__XDBC_HOOK_INVOKE_ONCE);
  __XDBC_HOOK_CHECK(return 0);
  
  return __sp_rs.release();
}

int IfxPreparedStatement::executeUpdate(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{
  __XDBC_TRACE(IFXPREPAREDSTATEMENT_CLASS_NAME,"executeUpdate()");

  string cursor_name("ifx_xdbc_cursor_name_");
  string cursor_id("ifx_xdbc_cursor_id_");
  int cursorID = IfxStatement::getCursorCounter();

  StringBuffer __sb;
  __sb.append(cursorID);

  cursor_name = cursor_name + __sb.str();
  cursor_id = cursor_id + __sb.str();

  EXEC SQL BEGIN DECLARE SECTION;
    char*  ec_cursor_name;
    char*  ec_cursor_id;
  EXEC SQL END DECLARE SECTION;

  ec_cursor_name  = (char*)__ms_cursor_name.c_str();
  ec_cursor_id    = (char*)__ms_cursor_id.c_str();

  if(__mi_var_count== 0)
  {
     EXEC SQL EXECUTE :ec_cursor_id;
  }
  else
  {
     EXEC SQL EXECUTE :ec_cursor_id  USING DESCRIPTOR __mp_ec_sqlda_vars;
  }
  int affected_row =  sqlca.sqlerrd[2];
  IfxConversion::checkException(__XDBC_HOOK_INVOKE_MORE SQLCODE,SQLSTATE,IFXPREPAREDSTATEMENT_CLASS_NAME);
  
/*
  printf("sqlerrd :-");
  for(int i=0;i<6;++i)
  {
    printf("- [%d] -",sqlca.sqlerrd[i]);
  }
  printf("-\n");
  printf("SQLCODE : [%d] ,SQLSTATE : [%s]\n",SQLCODE,SQLSTATE);
*/


  return  affected_row; 
}

void IfxPreparedStatement::setByte(__XDBC_HOOK_DECLARE_MORE int paramIndex,const char& x,bool is_null)  __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    __sb << "setByte|index=" << paramIndex << ",value=" << x << ",null=" << is_null;
    __XDBC_FORCED_TRACE(IFXPREPAREDSTATEMENT_CLASS_NAME,__sb.str()); 
  }

  __XDBC_INFORMIXPREPARED_CHECKPARAM(paramIndex); 
  __vector_binds[paramIndex].ifx_value.ec_SMINT = x;

  __XDBC_INFORMIXPREPARED_CHECKBOUND_BEGIN_2(paramIndex);
  
  __XDBC_INFORMIXPREPARED_CHECKBOUND_END_2(paramIndex);
}

void IfxPreparedStatement::setShort(__XDBC_HOOK_DECLARE_MORE int paramIndex,const short& x,bool is_null)  __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    __sb << "setShort|index=" << paramIndex << ",value=" << x << ",null=" << is_null;
    
    __XDBC_FORCED_TRACE(IFXPREPAREDSTATEMENT_CLASS_NAME,__sb.str()); 
  }

  __XDBC_INFORMIXPREPARED_CHECKPARAM(paramIndex); 
  __vector_binds[paramIndex].ifx_value.ec_SMINT = x;
  
  __XDBC_INFORMIXPREPARED_CHECKBOUND_BEGIN_2(paramIndex);
  
  __XDBC_INFORMIXPREPARED_CHECKBOUND_END_2(paramIndex);  
}

void IfxPreparedStatement::setInt(__XDBC_HOOK_DECLARE_MORE int paramIndex,const int& x,bool is_null)  __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    __sb << "setInt|index=" << paramIndex << ",value=" << x << ",null=" << is_null;
    
    __XDBC_FORCED_TRACE(IFXPREPAREDSTATEMENT_CLASS_NAME,__sb.str()); 
  }

  __XDBC_INFORMIXPREPARED_CHECKPARAM(paramIndex); 
  __vector_binds[paramIndex].ifx_value.ec_INT = x;

  __XDBC_INFORMIXPREPARED_CHECKBOUND_BEGIN_2(paramIndex);
  
  __XDBC_INFORMIXPREPARED_CHECKBOUND_END_2(paramIndex);  
}

void IfxPreparedStatement::setLong(__XDBC_HOOK_DECLARE_MORE int paramIndex,const XLONG& x,bool is_null)  __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    __sb << "setLong|index=" << paramIndex << ",value=" << x << ",null=" << is_null;
    
    __XDBC_FORCED_TRACE(IFXPREPAREDSTATEMENT_CLASS_NAME,__sb.str()); 
  }

  __XDBC_INFORMIXPREPARED_CHECKPARAM(paramIndex); 
  IfxConversion::longToint8(x,&(__vector_binds[paramIndex].ifx_value.ec_int8));  

  __XDBC_INFORMIXPREPARED_CHECKBOUND_BEGIN_2(paramIndex);
  
  __XDBC_INFORMIXPREPARED_CHECKBOUND_END_2(paramIndex);
}

void IfxPreparedStatement::setFloat(__XDBC_HOOK_DECLARE_MORE int paramIndex,const float& x,bool is_null)  __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    __sb << "setFloat|index=" << paramIndex << ",value=" << x << ",null=" << is_null;
    __XDBC_FORCED_TRACE(IFXPREPAREDSTATEMENT_CLASS_NAME,__sb.str()); 
  }

  __XDBC_INFORMIXPREPARED_CHECKPARAM(paramIndex); 
  __vector_binds[paramIndex].ifx_value.ec_DOUBLE = x;

  __XDBC_INFORMIXPREPARED_CHECKBOUND_BEGIN_2(paramIndex);
  
  __XDBC_INFORMIXPREPARED_CHECKBOUND_END_2(paramIndex);
}

void IfxPreparedStatement::setDouble(__XDBC_HOOK_DECLARE_MORE int paramIndex,const double& x,bool is_null)  __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    __sb << "setDouble|index=" << paramIndex << ",value=" << x << ",null=" << is_null;
    
    __XDBC_FORCED_TRACE(IFXPREPAREDSTATEMENT_CLASS_NAME,__sb.str()); 
  }

  __XDBC_INFORMIXPREPARED_CHECKPARAM(paramIndex);   
  deccvdbl(x,&(__vector_binds[paramIndex].ifx_value.ec_decimal));

  __XDBC_INFORMIXPREPARED_CHECKBOUND_BEGIN_2(paramIndex);
  
  __XDBC_INFORMIXPREPARED_CHECKBOUND_END_2(paramIndex);
}

void IfxPreparedStatement::setDate(__XDBC_HOOK_DECLARE_MORE int paramIndex,const XDBC_DATE* x,bool is_null) __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    if(x != 0)
    {
      __sb << "setDate|index=" << paramIndex << ",value=" << XDBCUtilities::toString(*x)<< ",null=" << is_null;
    }
    else
    {
      __sb << "setDate|index=" << paramIndex << ",value=<NULL>,null=" << is_null;
    }
    
    __XDBC_FORCED_TRACE(IFXPREPAREDSTATEMENT_CLASS_NAME,__sb.str()); 
  }

  __XDBC_INFORMIXPREPARED_CHECKPARAM(paramIndex);   

  short mdy_array[3];
  mdy_array[0] = x->month;
  mdy_array[1] = x->day;
  mdy_array[2] = x->year;
  
  rmdyjul(mdy_array,&(__vector_binds[paramIndex].ifx_value.ec_INT));
  
  __XDBC_INFORMIXPREPARED_CHECKBOUND_BEGIN_2(paramIndex);
  
  __XDBC_INFORMIXPREPARED_CHECKBOUND_END_2(paramIndex);
}

void IfxPreparedStatement::setTime(__XDBC_HOOK_DECLARE_MORE int paramIndex,const XDBC_TIME* x,bool is_null)  __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    if(x != 0)
    {
      __sb << "setTime|index=" << paramIndex << ",value=" << XDBCUtilities::toString(*x) << ",null=" << is_null;
    }
    else
    {
      __sb << "setTime|index=" << paramIndex << ",value=<NULL>,null=" << is_null;
    }
    
    __XDBC_FORCED_TRACE(IFXPREPAREDSTATEMENT_CLASS_NAME,__sb.str()); 
  }

  __XDBC_INFORMIXPREPARED_CHECKPARAM(paramIndex);   
  IfxConversion::makeDateTime(*x,&(__vector_binds[paramIndex].ifx_value.ec_dtime));
  
  __XDBC_INFORMIXPREPARED_CHECKBOUND_BEGIN_2(paramIndex);
  
  __XDBC_INFORMIXPREPARED_CHECKBOUND_END_2(paramIndex); 
}

void IfxPreparedStatement::setTimestamp(__XDBC_HOOK_DECLARE_MORE int paramIndex,const XDBC_TIMESTAMP* x,bool is_null)  __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    if(x != 0)
    {
      __sb << "setTimestamp|index=" << paramIndex << ",value=" << XDBCUtilities::toString(*x) << ",null=" << is_null;
    }
    else
    {
      __sb << "setTimestamp|index=" << paramIndex << ",value=<NULL>,null=" << is_null;
    }
    
    __XDBC_FORCED_TRACE(IFXPREPAREDSTATEMENT_CLASS_NAME,__sb.str());     
  }

  __XDBC_INFORMIXPREPARED_CHECKPARAM(paramIndex);   
  IfxConversion::makeDateTime(*x,&(__vector_binds[paramIndex].ifx_value.ec_dtime));
  
  __XDBC_INFORMIXPREPARED_CHECKBOUND_BEGIN_2(paramIndex);
  
  __XDBC_INFORMIXPREPARED_CHECKBOUND_END_2(paramIndex); 
}

void IfxPreparedStatement::setInterval(__XDBC_HOOK_DECLARE_MORE int paramIndex,  const XDBC_INTERVAL* x,bool is_null)  __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    if(x != 0)
    {
      __sb << "setInterval|index=" << paramIndex << ",value=" << XDBCUtilities::toString(*x) << ",null=" << is_null;
    }
    else
    {
      __sb << "setInterval|index=" << paramIndex << ",value=<NULL>,null=" << is_null;
    }
    
    __XDBC_FORCED_TRACE(IFXPREPAREDSTATEMENT_CLASS_NAME,__sb.str());       
  }

  __XDBC_INFORMIXPREPARED_CHECKPARAM(paramIndex);   
  IfxConversion::makeInterval(*x,&(__vector_binds[paramIndex].ifx_value.ec_interval));
  
  __XDBC_INFORMIXPREPARED_CHECKBOUND_BEGIN_2(paramIndex);
  
  __XDBC_INFORMIXPREPARED_CHECKBOUND_END_2(paramIndex); 
}

void IfxPreparedStatement::setString(__XDBC_HOOK_DECLARE_MORE int paramIndex,const string* x,bool is_null)  __XDBC_THROW_DECLARE(SQLException)
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
    
    __XDBC_FORCED_TRACE(IFXPREPAREDSTATEMENT_CLASS_NAME,__sb.str()); 
  }

  __XDBC_INFORMIXPREPARED_CHECKPARAM(paramIndex);   
  char* start = __mp_var_buffer+__vector_binds[paramIndex].int_buf_offset;
  int count= __vector_binds[paramIndex].__mi_buf_size;
  memcpy(start,x->c_str(),__M_min((int)x->length(),count));
  
  __XDBC_INFORMIXPREPARED_CHECKBOUND_BEGIN_2(paramIndex);
    
  __XDBC_INFORMIXPREPARED_CHECKBOUND_END_2(paramIndex); 
}

void IfxPreparedStatement::setBlob (__XDBC_HOOK_DECLARE_MORE int paramIndex, const char* x,int count,bool is_null)  __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    if(x != 0)
    {
      __sb << "setBlob|index=" << paramIndex << ",value=<BLOB>[" << count << "],null=" << is_null;
    }
    else
    {
      __sb << "setBlob|index=" << paramIndex << ",value=<NULL>,null=" << is_null;
    }
    
    __XDBC_FORCED_TRACE(IFXPREPAREDSTATEMENT_CLASS_NAME,__sb.str());       
  }

  __XDBC_INFORMIXPREPARED_CHECKPARAM(paramIndex);   
  if(x != 0)
  {
    __vector_binds[paramIndex].string_value.assign(x,count); 
  }
  
  __XDBC_INFORMIXPREPARED_CHECKBOUND_BEGIN_2(paramIndex);
  
  __vector_binds[paramIndex].ifx_value.ec_locator.loc_loctype = LOCMEMORY;  /* set loctype for in memory */ 
  __vector_binds[paramIndex].ifx_value.ec_locator.loc_oflags  = 0;    /* clear loc_oflags */
  __vector_binds[paramIndex].ifx_value.ec_locator.loc_bufsize = __vector_binds[paramIndex].string_value.length();    /* set buffer */ 
  __vector_binds[paramIndex].ifx_value.ec_locator.loc_buffer  = (char*)__vector_binds[paramIndex].string_value.data();
  
  __XDBC_INFORMIXPREPARED_CHECKBOUND_END_2(paramIndex); 
}

void IfxPreparedStatement::setClob (__XDBC_HOOK_DECLARE_MORE int paramIndex, const char* x,int count,bool is_null)  __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    if(x != 0)
    {
      __sb << "setClob|index=" << paramIndex << ",value=<CLOB>[" << count << "],null=" << is_null;
    }
    else
    {
      __sb << "setClob|index=" << paramIndex << ",value=<NULL>, null=" << is_null;
    }
    
    __XDBC_FORCED_TRACE(IFXPREPAREDSTATEMENT_CLASS_NAME,__sb.str()); 
  }

  __XDBC_INFORMIXPREPARED_CHECKPARAM(paramIndex);   
  if(x != 0)
  {
    __vector_binds[paramIndex].string_value.assign(x,count); 
  }

  __XDBC_INFORMIXPREPARED_CHECKBOUND_BEGIN_2(paramIndex);
  
  __vector_binds[paramIndex].ifx_value.ec_locator.loc_loctype = LOCMEMORY;  /* set loctype for in memory */ 
  __vector_binds[paramIndex].ifx_value.ec_locator.loc_oflags  = 0;    /* clear loc_oflags */
  __vector_binds[paramIndex].ifx_value.ec_locator.loc_bufsize = __vector_binds[paramIndex].string_value.length();    /* set buffer */ 
  __vector_binds[paramIndex].ifx_value.ec_locator.loc_buffer  = (char*)__vector_binds[paramIndex].string_value.data();
  
  __XDBC_INFORMIXPREPARED_CHECKBOUND_END_2(paramIndex);

}

void IfxPreparedStatement::setBlob (__XDBC_HOOK_DECLARE_MORE int paramIndex,const Blob* x,bool is_null)  __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    if(x != 0)
    {
      __sb << "setBlob|index=" << paramIndex << ",value =<BLOB>[" << x->length(__XDBC_HOOK_INVOKE_ONCE)<< "],null=" << is_null;
    }
    else
    {
      __sb << "setBlob|index=" << paramIndex << ",value=<NULL>null=" << is_null;
    }
    
    __XDBC_FORCED_TRACE(IFXPREPAREDSTATEMENT_CLASS_NAME,__sb.str()); 
  }

  __XDBC_INFORMIXPREPARED_CHECKPARAM(paramIndex);   
  __vector_binds[paramIndex].string_value.resize(x->length(__XDBC_HOOK_INVOKE_ONCE));
  x->read(__XDBC_HOOK_INVOKE_MORE 1,(char*)__vector_binds[paramIndex].string_value.data(),0,x->length(__XDBC_HOOK_INVOKE_ONCE));
  __XDBC_HOOK_CHECK(return);
  
  __XDBC_INFORMIXPREPARED_CHECKBOUND_BEGIN_2(paramIndex);
    
  __vector_binds[paramIndex].ifx_value.ec_locator.loc_loctype = LOCMEMORY;  /* set loctype for in memory */ 
  __vector_binds[paramIndex].ifx_value.ec_locator.loc_oflags  = 0;    /* clear loc_oflags */
  __vector_binds[paramIndex].ifx_value.ec_locator.loc_bufsize = __vector_binds[paramIndex].string_value.length();    /* set buffer */ 
  __vector_binds[paramIndex].ifx_value.ec_locator.loc_buffer  = (char*)__vector_binds[paramIndex].string_value.data();  

  __XDBC_INFORMIXPREPARED_CHECKBOUND_END_2(paramIndex); 
}

void IfxPreparedStatement::setClob (__XDBC_HOOK_DECLARE_MORE int paramIndex,const Clob* x,bool is_null)  __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    if(x != 0)
    {
      __sb << "setClob|index=" << paramIndex << ",value=<CLOB>[" << x->length(__XDBC_HOOK_INVOKE_ONCE)<< "],null=" << is_null;
    }
    else
    {
      __sb << "setClob|index=" << paramIndex << ",value =<null>,null=" << is_null;
    }
    
    __XDBC_FORCED_TRACE(IFXPREPAREDSTATEMENT_CLASS_NAME,__sb.str()); 
  }

  __XDBC_INFORMIXPREPARED_CHECKPARAM(paramIndex);   
  __vector_binds[paramIndex].string_value.resize(x->length(__XDBC_HOOK_INVOKE_ONCE));
  x->read(__XDBC_HOOK_INVOKE_MORE 1,(char*)__vector_binds[paramIndex].string_value.data(),0,x->length(__XDBC_HOOK_INVOKE_ONCE));
  __XDBC_HOOK_CHECK(return);
  
  __XDBC_INFORMIXPREPARED_CHECKBOUND_BEGIN_2(paramIndex);
    
  __vector_binds[paramIndex].ifx_value.ec_locator.loc_loctype = LOCMEMORY;  /* set loctype for in memory */ 
  __vector_binds[paramIndex].ifx_value.ec_locator.loc_oflags  = 0;    /* clear loc_oflags */
  __vector_binds[paramIndex].ifx_value.ec_locator.loc_bufsize = __vector_binds[paramIndex].string_value.length();    /* set buffer */ 
  __vector_binds[paramIndex].ifx_value.ec_locator.loc_buffer  = (char*)__vector_binds[paramIndex].string_value.data();  

  __XDBC_INFORMIXPREPARED_CHECKBOUND_END_2(paramIndex); 

}

//---------------------------------------------------------------------
// Statement Interfaces
//---------------------------------------------------------------------
void  IfxPreparedStatement::close()
{
  __XDBC_TRACE(IFXPREPAREDSTATEMENT_CLASS_NAME,"close()");

  if(__mb_isClosed)
  {
    return;
  }

  if(__mp_ec_sqlda_vars != 0)
  {
    delete __mp_ec_sqlda_vars;
    __mp_ec_sqlda_vars = 0;
  } 

  if(__mp_var_buffer!=0)
  {
    delete []__mp_var_buffer;
    __mp_var_buffer=0;
    __mi_var_buf_size = 0;
  }
 
  for(int i=0;i<__mi_var_count;++i)
  {
    __vector_binds[i].cleanup(__vector_vars[i]);
  }

  EXEC SQL BEGIN DECLARE SECTION;
    char*  ec_cursor_id;
  EXEC SQL END DECLARE SECTION;
  ec_cursor_id    = (char*) __ms_cursor_id.c_str();
  EXEC SQL FREE :ec_cursor_id;
}

void  IfxPreparedStatement::destroy()
{
  delete this;
}

int IfxPreparedStatement::getResultSetType(__XDBC_HOOK_DECLARE_ONCE)   __XDBC_THROW_DECLARE(SQLException)
{
  return getResultSetType(__XDBC_HOOK_INVOKE_ONCE);
}

Connection* IfxPreparedStatement::getConnection(__XDBC_HOOK_DECLARE_ONCE)   __XDBC_THROW_DECLARE(SQLException)
{
  return getConnection(__XDBC_HOOK_INVOKE_ONCE);
}

void  IfxPreparedStatement::parseInColumns(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException)
{
  if(__mi_var_count== 0)
  {
    return;
  } 

  //-------------------------------------------------------------------------
  // 1. fetch  Parameter Information
  //-------------------------------------------------------------------------
  int numcols = 0;

  if(__mi_stmt_type == IfxSQL::SQL_STMT_BEGIN)
  {

    if(!parseInColumns_begin(__XDBC_HOOK_INVOKE_ONCE))
    {
      return;
    }
    __XDBC_HOOK_CHECK(return);
    
    if(__mp_ec_sqlda_vars != 0)
    {
      numcols = __mp_ec_sqlda_vars->sqld;
      IfxConversion::parseResultSetMetaData( __mp_ec_sqlda_vars,__vector_vars);
    }

  }
  else if(__mi_stmt_type == IfxSQL::SQL_STMT_INSERT)
  {
    parseInColumns_insert(__XDBC_HOOK_INVOKE_ONCE);
    __XDBC_HOOK_CHECK(return);
    
    if(__mp_ec_sqlda_vars != 0)
    {
      numcols = __mp_ec_sqlda_vars->sqld;
      IfxConversion::parseResultSetMetaData( __mp_ec_sqlda_vars,__vector_vars);
    }
    
    string table_name  = IfxSQL::parseTableName(__XDBC_HOOK_INVOKE_MORE str_ifx_sql);
    __XDBC_HOOK_CHECK(return);
    
    size_t size = __vector_vars.size();
    for(size_t i=0;i<size;++i)
    {
      __vector_vars[i].__ms_table_name = table_name;
    }

  }
  else if(    __mi_stmt_type == IfxSQL::SQL_STMT_DELETE 
          || __mi_stmt_type == IfxSQL::SQL_STMT_UPDATE
          || __mi_stmt_type == IfxSQL::SQL_STMT_SELECT)
  {
    parseInColumns_common(__XDBC_HOOK_INVOKE_ONCE);
    __XDBC_HOOK_CHECK(return);
    
    if(__mp_ec_sqlda_vars != 0)
    {
      numcols = __mp_ec_sqlda_vars->sqld;
      IfxConversion::parseResultSetMetaData( __mp_ec_sqlda_vars,__vector_vars);
    }

  }
  else
  {
    string table_name  = IfxSQL::parseTableName(__XDBC_HOOK_INVOKE_MORE str_ifx_sql);
    __XDBC_HOOK_CHECK(return);
    
    string column_list = IfxSQL::parseInColumns(__XDBC_HOOK_INVOKE_MORE str_ifx_sql);
    __XDBC_HOOK_CHECK(return);
    
    parseInColumns_guess(__XDBC_HOOK_INVOKE_MORE table_name,column_list);
    __XDBC_HOOK_CHECK(return);
    
    if(__mp_ec_sqlda_vars != 0)
    {
      numcols = __mp_ec_sqlda_vars->sqld;
      IfxConversion::parseResultSetMetaData( __mp_ec_sqlda_vars,__vector_vars);
    }
    
    OPENCODE_MAP<string,string> map_tables;
    OPENCODE_MAP<string,string> __map_columns;
    
    IfxSQL::mapTables(__XDBC_HOOK_INVOKE_MORE table_name,map_tables);
    __XDBC_HOOK_CHECK(return);
    
    IfxSQL::mapColumns(__XDBC_HOOK_INVOKE_MORE column_list,__map_columns);
    __XDBC_HOOK_CHECK(return);
    
    if(__map_columns.empty())
    {
      size_t size = __vector_vars.size();
      for(size_t i=0;i<size;++i)
      {
        __vector_vars[i].__ms_table_name = table_name;
      }
    }
    else
    {
      size_t size = __vector_vars.size();
      for(size_t i=0;i<size;++i)
      {
        __vector_vars[i].__ms_table_name = table_name;
        string col_name = __vector_vars[i].__ms_name;

        OPENCODE_MAP<string,string>::iterator pos = __map_columns.find(col_name);
        if(pos != __map_columns.end())
        {
          string alias_name = pos->second;
          OPENCODE_MAP<string,string>::iterator pos2 = map_tables.find(alias_name);
          if(pos2 != map_tables.end())
          {
             __vector_vars[i].__ms_table_name = pos2->second; 
          }
        }
      }
    }

  }//~end if(__mi_stmt_type == IfxSQL::SQL_STMT_BEGIN) 

  if(DriverManager::isTracingOn)
  {
    int size = (int)__vector_vars.size();
    StringBuffer __sb;
    __sb << "parseInColumns|" << File::lineSeparator;
    for(int i=0;i<size;++i)
    {
      __vector_vars[i].dump(__sb);
    }
    
    __XDBC_FORCED_TRACE(IFXPREPAREDSTATEMENT_CLASS_NAME,__sb.str());
  }

  //-------------------------------------------------------------------------
  // 2. allocate & bind row data buffer 
  //-------------------------------------------------------------------------
  IfxConversion::prepareVariableData(__XDBC_HOOK_INVOKE_MORE __mp_conn,__mp_ec_sqlda_vars,__vector_vars,__vector_binds,__mp_var_buffer,__mi_var_buf_size);
}

void  IfxPreparedStatement::parseInColumns_common(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException)
{
  EXEC SQL BEGIN DECLARE SECTION;
    char*  ec_select_sql;
    char*  ec_cursor_id;
  EXEC SQL END DECLARE SECTION;

  ec_select_sql   = (char*)str_ifx_sql.c_str();
  ec_cursor_id    = (char*) str_var_cursor_id.c_str();

  EXEC SQL PREPARE :ec_cursor_id FROM :ec_select_sql;
  IfxConversion::checkException(__XDBC_HOOK_INVOKE_MORE SQLCODE,SQLSTATE,IFXPREPAREDSTATEMENT_CLASS_NAME);
  __XDBC_HOOK_CHECK(return);
  
  EXEC SQL DESCRIBE INPUT :ec_cursor_id into __mp_ec_sqlda_vars;
  int __SQLCODE = SQLCODE;
  string __SQLSTATE = SQLSTATE;
  
  EXEC SQL FREE  :ec_cursor_id;
  IfxConversion::checkException(__XDBC_HOOK_INVOKE_MORE __SQLCODE,__SQLSTATE,IFXPREPAREDSTATEMENT_CLASS_NAME);
}


bool  IfxPreparedStatement::parseInColumns_begin(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException)
{
  return false;  
}

void  IfxPreparedStatement::parseInColumns_insert(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException)
{
  EXEC SQL BEGIN DECLARE SECTION;
    char*  ec_select_sql;
    char*  ec_cursor_id;
  EXEC SQL END DECLARE SECTION;

  ec_select_sql   = (char*)str_ifx_sql.c_str();
  ec_cursor_id    = (char*) str_var_cursor_id.c_str();

  EXEC SQL PREPARE :ec_cursor_id FROM :ec_select_sql;  
  IfxConversion::checkException(__XDBC_HOOK_INVOKE_MORE SQLCODE,SQLSTATE,IFXPREPAREDSTATEMENT_CLASS_NAME);
  __XDBC_HOOK_CHECK(return);
  
  EXEC SQL DESCRIBE :ec_cursor_id into __mp_ec_sqlda_vars;
  int __SQLCODE = SQLCODE;
  string __SQLSTATE = SQLSTATE;
  
  EXEC SQL FREE  :ec_cursor_id;
  IfxConversion::checkException(__XDBC_HOOK_INVOKE_MORE __SQLCODE,__SQLSTATE,IFXPREPAREDSTATEMENT_CLASS_NAME);
}

void  IfxPreparedStatement::parseInColumns_guess(__XDBC_HOOK_DECLARE_MORE const string& table_name,const string& column_list) __XDBC_THROW_DECLARE(SQLException)
{
  if(column_list.length() == 0)
  {
    return;
  }

  string inColumnsSQL;
  inColumnsSQL.append("SELECT ");
  inColumnsSQL.append(column_list);
  inColumnsSQL.append(" FROM ");
  inColumnsSQL.append(table_name);

  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    __sb << "parseInColumns|" << inColumnsSQL;
    
    __XDBC_FORCED_TRACE(IFXPREPAREDSTATEMENT_CLASS_NAME,__sb.str());
  }

  EXEC SQL BEGIN DECLARE SECTION;
    char*  ec_select_sql;
    char*  ec_cursor_id;
  EXEC SQL END DECLARE SECTION;

  ec_select_sql   = (char*) inColumnsSQL.c_str();
  ec_cursor_id    = (char*) str_var_cursor_id.c_str();

  EXEC SQL PREPARE :ec_cursor_id FROM :ec_select_sql;
  IfxConversion::checkException(__XDBC_HOOK_INVOKE_MORE SQLCODE,SQLSTATE,IFXPREPAREDSTATEMENT_CLASS_NAME);
  __XDBC_HOOK_CHECK(return);
  
  EXEC SQL DESCRIBE :ec_cursor_id into __mp_ec_sqlda_vars;
  int __SQLCODE = SQLCODE;
  string __SQLSTATE = SQLSTATE;
  
  EXEC SQL FREE  :ec_cursor_id;
  IfxConversion::checkException(__XDBC_HOOK_INVOKE_MORE __SQLCODE,__SQLSTATE,IFXPREPAREDSTATEMENT_CLASS_NAME);
}

bool IfxPreparedStatement::checkConversion(__XDBC_HOOK_DECLARE_MORE int paramIndex,int valueType,const char* value,int length) __XDBC_THROW_DECLARE(SQLException)
{
  int varType = __vector_vars[paramIndex].__mi_type;
 
  if((varType == valueType))
  {
    return false;
  }

  //cout<<"Conversion ["<<paramIndex<<"]. valueType:"<<valueType<<",varType:"<<varType<<endl;

  char* buf_start=__mp_var_buffer+__vector_binds[paramIndex].int_buf_offset;
  int buf_size=__vector_binds[paramIndex].__mi_buf_size;
 
  switch(valueType)
  {
    case SQLINT:
    {
      int* p_int =(int*)value;

      if(varType == SQLSMINT)
      {
        short short_temp = (short)(*p_int);
        memcpy(buf_start,(char*)&short_temp,__M_min((int)sizeof(short_temp),buf_size)); 
        return true;
      }

      if(varType == SQLINT8 || varType == SQLSERIAL8)
      {
        int4 int4_temp = (int4)(*p_int);
        ifx_int8cvlong(int4_temp,&(__vector_binds[paramIndex].ifx_value.ec_int8));    
   
        return true;
      }

      if(varType == SQLSMFLOAT)
      {
        float float_temp = (float)(*p_int);
        memcpy(buf_start,(char*)&float_temp,__M_min((int)sizeof(float_temp),buf_size));
        return true;
      }

      if(varType == SQLFLOAT)
      {
        double double_temp = (double)(*p_int);
        memcpy(buf_start,(char*)&double_temp,__M_min((int)sizeof(double_temp),buf_size));
        return true;
      }

      if(varType == SQLDECIMAL || varType == SQLMONEY)
      {
        double double_temp = (double)(*p_int);
        deccvdbl(double_temp,&(__vector_binds[paramIndex].ifx_value.ec_decimal));
        return true;
      }

    }
    break;

//-----------------------------------------------
   case SQLINT8:
   case SQLSERIAL8:
   {
      XLONG* p_long = (XLONG*)value;

      if(varType == SQLSMINT)
      {
        short short_temp = (short)(*p_long);
        memcpy(buf_start,(char*)&short_temp,__M_min((int)sizeof(short_temp),buf_size));
        return true;
      }

      if(varType == SQLINT)
      {
        int int_temp = (int)(*p_long);
        memcpy(buf_start,(char*)&int_temp,__M_min((int)sizeof(int_temp),buf_size));
        return true;
      }

      if(varType == SQLINT8 || varType == SQLSERIAL8)
      {
        int4 int4_temp = (int4)(*p_long);
        ifx_int8cvlong(int4_temp,&(__vector_binds[paramIndex].ifx_value.ec_int8));

        return true;
      }

      if(varType == SQLSMFLOAT)
      {
        float float_temp = (float)(*p_long);
        memcpy(buf_start,(char*)&float_temp,__M_min((int)sizeof(float_temp),buf_size));
        return true;
      }

      if(varType == SQLFLOAT)
      {
        double double_temp = (double)(*p_long);
        memcpy(buf_start,(char*)&double_temp,__M_min((int)sizeof(double_temp),buf_size));
        return true;
      }

      if(varType == SQLDECIMAL || varType == SQLMONEY)
      {
        double double_temp = (double)(*p_long);
        deccvdbl(double_temp,&(__vector_binds[paramIndex].ifx_value.ec_decimal));
        return true;
      }

   }
   break;    

//-----------------------------------------------

    case SQLFLOAT:
    case SQLDECIMAL:
    {
      double* p_double =(double*)value;

      if(varType == SQLSMINT)
      {
        short short_temp = (short)(*p_double);
        memcpy(buf_start,(char*)&short_temp,__M_min((int)sizeof(short_temp),buf_size));
        return true;
      }

      if(varType == SQLINT)
      {
        int int_temp = (int)(*p_double);
        memcpy(buf_start,(char*)&int_temp,__M_min((int)sizeof(int_temp),buf_size));
        return true;
      }

      if(varType == SQLINT8 || varType == SQLSERIAL8)
      {
        int4 int4_temp = (int4)(*p_double);
        ifx_int8cvlong(int4_temp,&(__vector_binds[paramIndex].ifx_value.ec_int8));

        return true;
      }

      if(varType == SQLSMFLOAT)
      {
        float float_temp = (float)(*p_double);
        memcpy(buf_start,(char*)&float_temp,__M_min((int)sizeof(float_temp),buf_size));
        return true;
      }

      if(varType == SQLFLOAT)
      {
        double double_temp = (double)(*p_double);
        memcpy(buf_start,(char*)&double_temp,__M_min((int)sizeof(double_temp),buf_size));
        return true;
      }

      if(varType == SQLDECIMAL || varType == SQLMONEY)
      {
        double double_temp = (double)(*p_double);
        deccvdbl(double_temp,&(__vector_binds[paramIndex].ifx_value.ec_decimal));
        return true;
      }
    }
    break;

//-----------------------------------------------

    case SQLVCHAR:
    {
      const char* x=value;

      if( (varType == SQLTEXT) || (varType == SQLBYTES))
      {
        if(x==0) 
        {
          __vector_binds[paramIndex].string_value.resize(0);
          __vector_binds[paramIndex].ifx_value.ec_locator.loc_indicator = -1;
          __vector_binds[paramIndex].__mi_bind_ind = -1;
        }
        else 
        {
          __vector_binds[paramIndex].string_value.assign(x,length);
          __vector_binds[paramIndex].ifx_value.ec_locator.loc_indicator = 1;
          __vector_binds[paramIndex].__mi_bind_ind = 1;
        }

        __vector_binds[paramIndex].ifx_value.ec_locator.loc_loctype = LOCMEMORY; 
        __vector_binds[paramIndex].ifx_value.ec_locator.loc_type = varType;
        __vector_binds[paramIndex].ifx_value.ec_locator.loc_oflags  = 0;
        __vector_binds[paramIndex].ifx_value.ec_locator.loc_mflags  = 0;
        __vector_binds[paramIndex].ifx_value.ec_locator.loc_bufsize = __vector_binds[paramIndex].string_value.length(); 
        __vector_binds[paramIndex].ifx_value.ec_locator.loc_buffer  = (char*) __vector_binds[paramIndex].string_value.data();
        __vector_binds[paramIndex].ifx_value.ec_locator.loc_size = __vector_binds[paramIndex].string_value.length();

        return true;
      }
      else if(varType == SQLUDTFIXED)
      {
        //printf("-----------varType == SQLUDTFIXED--------------\n");

        ifx_int8_t offset;
        int error = 0;

        ifx_int8cvint(0, &offset);

        ifx_lo_truncate(__vector_binds[paramIndex].ifx_value.int_ec_lofd,&offset);     
        IfxConversion::checkException(__XDBC_HOOK_INVOKE_MORE SQLCODE,SQLSTATE,IFXPREPAREDSTATEMENT_CLASS_NAME);
        __XDBC_HOOK_CHECK(return false);

        ifx_lo_writewithseek(__vector_binds[paramIndex].ifx_value.int_ec_lofd, (char*)value,length,&offset, LO_SEEK_SET, &error);    
        IfxConversion::checkException(__XDBC_HOOK_INVOKE_MORE SQLCODE,SQLSTATE,IFXPREPAREDSTATEMENT_CLASS_NAME);
        __XDBC_HOOK_CHECK(return false);
          
        return true;
      }
    }

    break;

//-----------------------------------------------

    case SQLDATE:
    {
    }
    break;

//-----------------------------------------------

    case SQLDTIME:
    {
    }
    break;

//-----------------------------------------------

    default:
      break;
  }

  StringBuffer __sb;
  __sb.append("[IfxPreparedStatement] Conversion not supported . sourceSQLType:");
  __sb.append(valueType);
  __sb.append(",targetSQLType:");
  __sb.append(varType);
 
  __XDBC_HOOK_RETURN(return false,__sb.str());
}

__OPENCODE_END_NAMESPACE
