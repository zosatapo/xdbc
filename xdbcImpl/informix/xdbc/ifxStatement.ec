#include "xdbc/ifxStatement.h"
#include "xdbc/ifxConnection.h"
#include "xdbc/ifxResultSet.h"
#include "xdbc/ifxArrayResultSet.h"
#include "xdbc/ifxSQL.h"
#include "xdbc/ifxConversion.h"
#include "xdbc/xsmart.h"
#include "port/ioc_sbuf.h"

#include <algorithm>

USING_OPENCODE_STL_NS
__OPENCODE_BEGIN_NAMESPACE

//-------------------------------------------------------------------------

#define IFSTATEMENT_CLASS_NAME "IfxStatement"

int    IfxStatement::__gi_cursorNumber = 1;
Mutex  IfxStatement::__go_cursorNumber_mutex;

IfxStatement::~IfxStatement()
{
  __XDBC_TRACE(IFSTATEMENT_CLASS_NAME,"destroy()");
}

IfxStatement::IfxStatement(IfxConnection* ora_conn)
{
  __mp_conn=ora_conn;
  
  __mi_resultSetType=ResultSet::TYPE_FORWARD_ONLY;
  __mi_resultSetConcurrency=ResultSet::CONCUR_READ_ONLY;

  __mb_initialized=false;
  __mb_isClosed=false;
}

IfxStatement::IfxStatement(IfxConnection* ora_conn,int resultSetType, int resultSetConcurrency)
{
  __mp_conn=ora_conn;

  __mi_resultSetType=resultSetType;
  __mi_resultSetConcurrency=resultSetConcurrency;

  __mb_initialized=false;
  __mb_isClosed=false;
}


void IfxStatement::initialize(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{
  if(__mb_initialized)
  {  
    return;  
  }
  
  __mb_initialized=true;
}

ResultSet* IfxStatement::executeQuery(__XDBC_HOOK_DECLARE_MORE const string& sql)  __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
     StringBuffer __sb;
     __sb.append("executeQuery|");
     __sb.append(sql);
     
     __XDBC_FORCED_TRACE(IFSTATEMENT_CLASS_NAME,__sb.str()); 
  }

  ResultSetCollection* cachedCollection = 0;
  
  int __mi_stmt_type=IfxSQL::parseSQLKind( sql);
  
  int param_count=0; 
  string ifx_sql=IfxSQL::parseSQL(__XDBC_HOOK_INVOKE_MORE sql,param_count);
  __XDBC_HOOK_CHECK(return 0);
    
  string cursor_name("ifx_curn_");
  string cursor_id("ifx_curi_");
  int cursorID = getCursorCounter();

  StringBuffer __sb;
  __sb.append(cursorID);

  cursor_name = cursor_name + __sb.str();
  cursor_id = cursor_id + __sb.str();

  EXEC SQL BEGIN DECLARE SECTION;
    char*  ec_select_sql;
    char*  ec_cursor_name;
    char*  ec_cursor_id;
  EXEC SQL END DECLARE SECTION;

  ec_select_sql   = (char*)ifx_sql.c_str();
  ec_cursor_name  = (char*)cursor_name.c_str();
  ec_cursor_id    = (char*)cursor_id.c_str();

  EXEC SQL PREPARE :ec_cursor_id FROM :ec_select_sql;
  IfxConversion::checkException(__XDBC_HOOK_INVOKE_MORE SQLCODE,SQLSTATE,IFSTATEMENT_CLASS_NAME);
  __XDBC_HOOK_CHECK(return 0);
  
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

  EXEC SQL OPEN :ec_cursor_name;
  IfxConversion::checkException(__XDBC_HOOK_INVOKE_MORE SQLCODE,SQLSTATE,IFSTATEMENT_CLASS_NAME);
  __XDBC_HOOK_CHECK(return 0);
    
  IfxResultSet* p_rs=new IfxResultSet(__mp_conn,this,cursor_name,cursor_id,__mi_resultSetType,__mi_resultSetConcurrency,true,cachedCollection);
  xdbc_smart_ptr<IfxResultSet> __sp_rs(p_rs);
 
  p_rs->initialize(__XDBC_HOOK_INVOKE_ONCE);
  __XDBC_HOOK_CHECK(return 0);

  return __sp_rs.release();
}

int IfxStatement::executeUpdate(__XDBC_HOOK_DECLARE_MORE const string& sql)  __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
     StringBuffer __sb;
     __sb.append("executeUpdate|");
     __sb.append(sql);
      
     __XDBC_FORCED_TRACE(IFSTATEMENT_CLASS_NAME,__sb.str());
  }

  int param_count=0;

  string ifx_sql=IfxSQL::parseSQL(__XDBC_HOOK_INVOKE_MORE sql,param_count);
  __XDBC_HOOK_CHECK(return 0);
    
  int __mi_stmt_type=IfxSQL::parseSQLKind(sql);

  string cursor_name("ifx_xdbc_cursor_name_");
  string cursor_id("ifx_xdbc_cursor_id_");
  int cursorID = getCursorCounter();

  StringBuffer __sb;
  __sb.append(cursorID);

  cursor_name = cursor_name + __sb.str();
  cursor_id = cursor_id + __sb.str();

  EXEC SQL BEGIN DECLARE SECTION;
    char*  ec_select_sql;
    char*  ec_cursor_name;
    char*  ec_cursor_id;
  EXEC SQL END DECLARE SECTION;

  ec_select_sql   = (char*)ifx_sql.c_str();
  ec_cursor_name  = (char*)cursor_name.c_str();
  ec_cursor_id    = (char*)cursor_id.c_str();

  EXEC SQL PREPARE :ec_cursor_id FROM :ec_select_sql;
  IfxConversion::checkException(__XDBC_HOOK_INVOKE_MORE SQLCODE,SQLSTATE,IFSTATEMENT_CLASS_NAME);
  __XDBC_HOOK_CHECK(return 0);
  
  EXEC SQL EXECUTE :ec_cursor_id;
  int affected_rows = sqlca.sqlerrd[2];
  IfxConversion::checkException(__XDBC_HOOK_INVOKE_MORE SQLCODE,SQLSTATE,IFSTATEMENT_CLASS_NAME);
  __XDBC_HOOK_CHECK(return 0);
  
  EXEC SQL FREE :ec_cursor_id;
  IfxConversion::checkException(__XDBC_HOOK_INVOKE_MORE SQLCODE,SQLSTATE,IFSTATEMENT_CLASS_NAME);
  __XDBC_HOOK_CHECK(return 0);
  
/*
  printf("sqlerrd :-");
  for(int i=0;i<6;++i)
  {
    printf("- [%d] -",sqlca.sqlerrd[i]);
  }
  printf("-\n");
  printf("SQLCODE : [%d] ,SQLSTATE : [%s]\n",SQLCODE,SQLSTATE);
*/

  return  affected_rows;  
}

void IfxStatement::close()
{ 
  __XDBC_TRACE(IFSTATEMENT_CLASS_NAME,"close()");
  if(__mb_isClosed)
  {
    return;
  }
  __mb_isClosed=true;
}

void IfxStatement::destroy()
{ 
  delete this;  
}

int IfxStatement::getResultSetType(__XDBC_HOOK_DECLARE_ONCE)   __XDBC_THROW_DECLARE(SQLException)
{
  return __mi_resultSetType;
}

Connection* IfxStatement::getConnection(__XDBC_HOOK_DECLARE_ONCE)   __XDBC_THROW_DECLARE(SQLException)
{
  return (Connection*)__mp_conn;
}

int  IfxStatement::getCursorCounter()
{
  int i=0;

  Mutex::Guard g(__go_cursorNumber_mutex,System::__M_multithreading);
  {
    if(__gi_cursorNumber == 0x7fffffff)
    {
      __gi_cursorNumber = 0;
    }
    else
    {
      __gi_cursorNumber++;
    }
    i = __gi_cursorNumber;
  }

  return i;
}

__OPENCODE_END_NAMESPACE
