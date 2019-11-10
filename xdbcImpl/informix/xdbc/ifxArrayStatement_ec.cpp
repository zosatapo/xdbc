#include <sqlhdr.h>
#include <sqliapi.h>
#line 1 "ifxArrayStatement.ec"
#include "xdbc/ifxArrayStatement.h"
#include "xdbc/ifxConnection.h"
#include "xdbc/ifxResultSet.h"
#include "xdbc/ifxArrayResultSet.h"
#include "xdbc/ifxConversion.h"

#include "xdbc/ifxClob.h"
#include "xdbc/ifxBlob.h"

#include "xdbc/ifxSQL.h"
#include "xdbc/xsmart.h"
#include "port/ioc_sbuf.h"
#include "port/utilities.h"
#include "port/file.h"
#include "port/ioc_map.h"

__OPENCODE_BEGIN_NAMESPACE

//-------------------------------------------------------------------------

#define IFXARRAYSTATEMENT_CLASS_NAME "IfxArrayStatement"
const int IfxArrayStatement::MAX_ROWCOUNT = 30000;

//-------------------------------------------------------------------------

IfxArrayStatement::~IfxArrayStatement()
{
  __XDBC_TRACE(IFXARRAYSTATEMENT_CLASS_NAME,"destroy()");
}

IfxArrayStatement::IfxArrayStatement(IfxConnection* ora_conn,const string& sql,int paramCount)
{
  __mp_conn=ora_conn;

  __mi_resultSetType=ResultSet::TYPE_FORWARD_ONLY;
  __mi_resultSetConcurrency=ResultSet::CONCUR_READ_ONLY;

  str_orginal_ifx_sql=sql;
  str_ifx_sql=sql;


  __mi_var_count=paramCount;
  __mp_ec_sqlda_vars = 0;

  __mi_array_size = 1;
  __mi_max_size = MAX_ROWCOUNT;
}

IfxArrayStatement::IfxArrayStatement(IfxConnection* ora_conn,const string& sql, int resultSetType, int resultSetConcurrency,int paramCount)
{
  __mp_conn=ora_conn;

  __mi_resultSetType=resultSetType;
  __mi_resultSetConcurrency=resultSetConcurrency;

   str_orginal_ifx_sql=sql;
   str_ifx_sql=sql;


   __mi_var_count=paramCount;
   __mp_ec_sqlda_vars = 0;

  __mi_array_size = 1;
  __mi_max_size = MAX_ROWCOUNT;
}

void IfxArrayStatement::initialize(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{
  if(__mb_initialized)
  {
    return;
  }

  initialize(__XDBC_HOOK_INVOKE_ONCE);
  __XDBC_HOOK_CHECK(return);

  str_ifx_sql=IfxSQL::parseSQL(__XDBC_HOOK_INVOKE_MORE str_orginal_ifx_sql,__mi_var_count);
  __XDBC_HOOK_CHECK(return);

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

/*
 *   EXEC SQL BEGIN DECLARE SECTION;
 */
#line 104 "ifxArrayStatement.ec"
#line 105 "ifxArrayStatement.ec"
#line 105 "ifxArrayStatement.ec"
  char *ec_select_sql;
#line 106 "ifxArrayStatement.ec"
  char *ec_cursor_id;
/*
 *   EXEC SQL END DECLARE SECTION;
 */
#line 107 "ifxArrayStatement.ec"


  ec_select_sql   = (char*)str_ifx_sql.c_str();
  ec_cursor_id    = (char*)cursor_id.c_str();

/*
 *   EXEC SQL PREPARE :ec_cursor_id FROM :ec_select_sql;
 */
#line 112 "ifxArrayStatement.ec"
  {
#line 112 "ifxArrayStatement.ec"
#line 112 "ifxArrayStatement.ec"
#line 112 "ifxArrayStatement.ec"
  sqli_prep(ESQLINTVERSION, (char *) ec_cursor_id, ec_select_sql,(ifx_literal_t *) 0, (ifx_namelist_t *) 0, -1, 0,0 ); 
#line 112 "ifxArrayStatement.ec"
  }
  IfxConversion::checkException(__XDBC_HOOK_INVOKE_MORE SQLCODE,SQLSTATE,IFXARRAYSTATEMENT_CLASS_NAME);
  __XDBC_HOOK_CHECK(return);

  parseInColumns(__XDBC_HOOK_INVOKE_ONCE);
}

int  IfxArrayStatement::getMaxArraySize() const
{
  return __mi_max_size;
}

void  IfxArrayStatement::setMaxArraySize(int size)
{
  __mi_max_size = size;
}


int IfxArrayStatement::executeUpdate(__XDBC_HOOK_DECLARE_MORE DataTable* paramTable,vector<ArrayException>* errors)  __XDBC_THROW_DECLARE(SQLException)
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
     __sb<<"executeUpdate|rows[s/r/m] = ["<<__mi_array_size
       <<"/" <<rowcount<<"/"<<__mi_max_size <<"] , cols = ["<<colcount<<"]";

     __XDBC_FORCED_TRACE(IFXARRAYSTATEMENT_CLASS_NAME,__sb.str());
  }

  string cursor_name("ifx_xdbc_cursor_name_");
  string cursor_id("ifx_xdbc_cursor_id_");
  int cursorID = IfxStatement::getCursorCounter();

  StringBuffer __sb;
  __sb.append(cursorID);

  cursor_name = cursor_name + __sb.str();
  cursor_id = cursor_id + __sb.str();

/*
 *   EXEC SQL BEGIN DECLARE SECTION;
 */
#line 164 "ifxArrayStatement.ec"
#line 165 "ifxArrayStatement.ec"
#line 165 "ifxArrayStatement.ec"
  char *ec_cursor_name;
#line 166 "ifxArrayStatement.ec"
  char *ec_cursor_id;
/*
 *   EXEC SQL END DECLARE SECTION;
 */
#line 167 "ifxArrayStatement.ec"


  ec_cursor_name  = (char*)__ms_cursor_name.c_str();
  ec_cursor_id    = (char*)__ms_cursor_id.c_str();

  int affected_row = 0;

  if(paramTable == 0)
  {
    if(__mi_var_count == 0)
    {
/*
 *        EXEC SQL EXECUTE :ec_cursor_id;
 */
#line 178 "ifxArrayStatement.ec"
  {
#line 178 "ifxArrayStatement.ec"
#line 178 "ifxArrayStatement.ec"
  sqli_exec(ESQLINTVERSION, sqli_curs_locate(ESQLINTVERSION, (char *) ec_cursor_id, 257), (ifx_sqlda_t *) 0, (char *) 0, (struct value *) 0, (ifx_sqlda_t *) 0, (char *) 0, (struct value *) 0, 0);
#line 178 "ifxArrayStatement.ec"
  }
    }
    else
    {
/*
 *        EXEC SQL EXECUTE :ec_cursor_id  USING DESCRIPTOR __mp_ec_sqlda_vars;
 */
#line 182 "ifxArrayStatement.ec"
  {
#line 182 "ifxArrayStatement.ec"
#line 182 "ifxArrayStatement.ec"
  sqli_exec(ESQLINTVERSION, sqli_curs_locate(ESQLINTVERSION, (char *) ec_cursor_id, 257), __mp_ec_sqlda_vars, (char *) 0, (struct value *) 0, (ifx_sqlda_t *) 0, (char *) 0, (struct value *) 0, 0);
#line 182 "ifxArrayStatement.ec"
  }
    }

    affected_row =  sqlca.sqlerrd[2];
    IfxConversion::checkException(__XDBC_HOOK_INVOKE_MORE SQLCODE,SQLSTATE,IFXARRAYSTATEMENT_CLASS_NAME);
    __XDBC_HOOK_CHECK(return 0);
  }
  else
  {
    for(int i=1;i<=rowcount;++i)
    {
      setBindData(__XDBC_HOOK_INVOKE_MORE paramTable,i);
      __XDBC_HOOK_CHECK(return);
    }

    if(__mi_var_count == 0)
    {
/*
 *        EXEC SQL EXECUTE :ec_cursor_id;
 */
#line 199 "ifxArrayStatement.ec"
  {
#line 199 "ifxArrayStatement.ec"
#line 199 "ifxArrayStatement.ec"
  sqli_exec(ESQLINTVERSION, sqli_curs_locate(ESQLINTVERSION, (char *) ec_cursor_id, 257), (ifx_sqlda_t *) 0, (char *) 0, (struct value *) 0, (ifx_sqlda_t *) 0, (char *) 0, (struct value *) 0, 0);
#line 199 "ifxArrayStatement.ec"
  }
    }
    else
    {
/*
 *        EXEC SQL EXECUTE :ec_cursor_id  USING DESCRIPTOR __mp_ec_sqlda_vars;
 */
#line 203 "ifxArrayStatement.ec"
  {
#line 203 "ifxArrayStatement.ec"
#line 203 "ifxArrayStatement.ec"
  sqli_exec(ESQLINTVERSION, sqli_curs_locate(ESQLINTVERSION, (char *) ec_cursor_id, 257), __mp_ec_sqlda_vars, (char *) 0, (struct value *) 0, (ifx_sqlda_t *) 0, (char *) 0, (struct value *) 0, 0);
#line 203 "ifxArrayStatement.ec"
  }
    }

    affected_row +=  sqlca.sqlerrd[2];
    IfxConversion::checkException(__XDBC_HOOK_INVOKE_MORE SQLCODE,SQLSTATE,IFXARRAYSTATEMENT_CLASS_NAME);
    __XDBC_HOOK_CHECK(return 0);
  }

  return  affected_row;
}

ArrayResultSet*  IfxArrayStatement::executeQuery(__XDBC_HOOK_DECLARE_MORE DataTable* __mp_dataTable,DataTable* paramTable)  __XDBC_THROW_DECLARE(SQLException)
{
  int rowcount = 0;
  int colcount = 0;
  int paramrowcount = 0;

  if(paramTable != 0)
  {
    rowcount = paramTable->getRowCount();
    colcount = paramTable->getColumnCount();
    paramrowcount = __M_min(1,rowcount);

    if(paramrowcount > 0)
    {
      setBindData(__XDBC_HOOK_INVOKE_MORE paramTable,1);
      __XDBC_HOOK_CHECK(return);
    }
  }

  if(DriverManager::isTracingOn)
  {
     StringBuffer __sb;
     __sb<<"executeQuery|rows[s/r] = [" <<paramrowcount<<"/"<< rowcount <<"]";

     __XDBC_FORCED_TRACE(IFXARRAYSTATEMENT_CLASS_NAME,__sb.str());
  }

/*
 *   EXEC SQL BEGIN DECLARE SECTION;
 */
#line 241 "ifxArrayStatement.ec"
#line 242 "ifxArrayStatement.ec"
#line 242 "ifxArrayStatement.ec"
  char *ec_cursor_name;
#line 243 "ifxArrayStatement.ec"
  char *ec_cursor_id;
/*
 *   EXEC SQL END DECLARE SECTION;
 */
#line 244 "ifxArrayStatement.ec"


  ec_cursor_name  = (char*)__ms_cursor_name.c_str();
  ec_cursor_id    = (char*)__ms_cursor_id.c_str();

  if(__mi_resultSetType == ResultSet::TYPE_SCROLL_INSENSITIVE)
  {
/*
 *     EXEC SQL DECLARE :ec_cursor_name SCROLL CURSOR FOR :ec_cursor_id ;
 */
#line 251 "ifxArrayStatement.ec"
  {
#line 251 "ifxArrayStatement.ec"
#line 251 "ifxArrayStatement.ec"
  sqli_curs_decl_dynm(ESQLINTVERSION, sqli_curs_locate(ESQLINTVERSION, (char *) ec_cursor_name, 0), (char *) ec_cursor_name, sqli_curs_locate(ESQLINTVERSION, (char *) ec_cursor_id, 1), 32, 0);
#line 251 "ifxArrayStatement.ec"
  }
  }
  else if (__mi_resultSetType == ResultSet::TYPE_SCROLL_SENSITIVE)
  {
/*
 *     EXEC SQL DECLARE :ec_cursor_name SCROLL CURSOR FOR :ec_cursor_id ;
 */
#line 255 "ifxArrayStatement.ec"
  {
#line 255 "ifxArrayStatement.ec"
#line 255 "ifxArrayStatement.ec"
  sqli_curs_decl_dynm(ESQLINTVERSION, sqli_curs_locate(ESQLINTVERSION, (char *) ec_cursor_name, 0), (char *) ec_cursor_name, sqli_curs_locate(ESQLINTVERSION, (char *) ec_cursor_id, 1), 32, 0);
#line 255 "ifxArrayStatement.ec"
  }
  }
  else
  {
/*
 *     EXEC SQL DECLARE :ec_cursor_name CURSOR FOR :ec_cursor_id ;
 */
#line 259 "ifxArrayStatement.ec"
  {
#line 259 "ifxArrayStatement.ec"
#line 259 "ifxArrayStatement.ec"
  sqli_curs_decl_dynm(ESQLINTVERSION, sqli_curs_locate(ESQLINTVERSION, (char *) ec_cursor_name, 0), (char *) ec_cursor_name, sqli_curs_locate(ESQLINTVERSION, (char *) ec_cursor_id, 1), 0, 0);
#line 259 "ifxArrayStatement.ec"
  }
  }

  if(__mi_var_count == 0)
  {
/*
 *      EXEC SQL OPEN :ec_cursor_name;
 */
#line 264 "ifxArrayStatement.ec"
  {
#line 264 "ifxArrayStatement.ec"
#line 264 "ifxArrayStatement.ec"
  sqli_curs_open(ESQLINTVERSION, sqli_curs_locate(ESQLINTVERSION, (char *) ec_cursor_name, 256), (ifx_sqlda_t *) 0, (char *) 0, (struct value *) 0, 0, 0);
#line 264 "ifxArrayStatement.ec"
  }
  }
  else
  {
/*
 *      EXEC SQL OPEN :ec_cursor_name  USING DESCRIPTOR __mp_ec_sqlda_vars;
 */
#line 268 "ifxArrayStatement.ec"
  {
#line 268 "ifxArrayStatement.ec"
#line 268 "ifxArrayStatement.ec"
  sqli_curs_open(ESQLINTVERSION, sqli_curs_locate(ESQLINTVERSION, (char *) ec_cursor_name, 256), __mp_ec_sqlda_vars, (char *) 0, (struct value *) 0, 1, 0);
#line 268 "ifxArrayStatement.ec"
  }
  }

  IfxConversion::checkException(__XDBC_HOOK_INVOKE_MORE SQLCODE,SQLSTATE,IFXARRAYSTATEMENT_CLASS_NAME);
  __XDBC_HOOK_CHECK(return 0);

  IfxArrayResultSet* p_rs=new IfxArrayResultSet(this,__ms_cursor_name,__ms_cursor_id,__mi_resultSetType,__mi_resultSetConcurrency,__mp_dataTable,false);
  xdbc_smart_ptr<IfxArrayResultSet> __sp_rs(p_rs);

  p_rs->initialize(__XDBC_HOOK_INVOKE_ONCE);
  __XDBC_HOOK_CHECK(return 0);

  return p_rs;
}

//---------------------------------------------------------------------
// Statement Interfaces
//---------------------------------------------------------------------
void  IfxArrayStatement::close()
{
  __XDBC_TRACE(IFXARRAYSTATEMENT_CLASS_NAME,"close()");

  if(__mb_isClosed)
  {
    return;
  }

  if(__mp_ec_sqlda_vars != 0)
  {
    delete __mp_ec_sqlda_vars;
    __mp_ec_sqlda_vars = 0;
  }

  if(__mi_var_count > 0)
  {
/*
 *     EXEC SQL BEGIN DECLARE SECTION;
 */
#line 303 "ifxArrayStatement.ec"
#line 304 "ifxArrayStatement.ec"
#line 304 "ifxArrayStatement.ec"
  char *ec_cursor_id;
/*
 *     EXEC SQL END DECLARE SECTION;
 */
#line 305 "ifxArrayStatement.ec"


    ec_cursor_id    = (char*) str_var_cursor_id.c_str();

/*
 *     EXEC SQL FREE :ec_cursor_id;
 */
#line 309 "ifxArrayStatement.ec"
  {
#line 309 "ifxArrayStatement.ec"
#line 309 "ifxArrayStatement.ec"
  sqli_curs_free(ESQLINTVERSION, sqli_curs_locate(ESQLINTVERSION, (char *) ec_cursor_id, 258));
#line 309 "ifxArrayStatement.ec"
  }
    IfxConversion::checkException(__XDBC_HOOK_INVOKE_MORE SQLCODE,SQLSTATE,IFXARRAYSTATEMENT_CLASS_NAME);
  }
}

void  IfxArrayStatement::destroy()
{
  delete this;
}

int IfxArrayStatement::getResultSetType(__XDBC_HOOK_DECLARE_ONCE)   __XDBC_THROW_DECLARE(SQLException)
{
  return getResultSetType(__XDBC_HOOK_INVOKE_ONCE);
}

Connection* IfxArrayStatement::getConnection(__XDBC_HOOK_DECLARE_ONCE)   __XDBC_THROW_DECLARE(SQLException)
{
  return getConnection(__XDBC_HOOK_INVOKE_ONCE);
}

void  IfxArrayStatement::parseInColumns(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException)
{
  if(__mi_var_count == 0)
  {
    return;
  }

  //-------------------------------------------------------------------------
  // 1. fetch  Parameter Information
  //-------------------------------------------------------------------------
  int numcols = 0;

  if(__mi_stmt_type == IfxSQL::SQL_STMT_BEGIN)
  {
    return;
  }
  else if(__mi_stmt_type == IfxSQL::SQL_STMT_INSERT)
  {
    parseInColumns_insert(__XDBC_HOOK_INVOKE_ONCE);
    __XDBC_HOOK_CHECK(return);

    if(__mp_ec_sqlda_vars != 0)
    {
      numcols = __mp_ec_sqlda_vars->sqld;
      IfxConversion::parseResultSetMetaData(__mp_ec_sqlda_vars,__vector_vars);
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
      IfxConversion::parseResultSetMetaData(__mp_ec_sqlda_vars,__vector_vars);
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
      IfxConversion::parseResultSetMetaData(__mp_ec_sqlda_vars,__vector_vars);
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
    StringBuffer __sb;
    __sb << "parseInColumns|" << File::lineSeparator;

    size_t size = __vector_vars.size();
    for(size_t i=0;i<size;++i)
    {
      __vector_vars[i].dump(__sb);
    }

    __XDBC_FORCED_TRACE(IFXARRAYSTATEMENT_CLASS_NAME,__sb.str());
  }

}

void  IfxArrayStatement::parseInColumns_common(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException)
{
/*
 *   EXEC SQL BEGIN DECLARE SECTION;
 */
#line 455 "ifxArrayStatement.ec"
#line 456 "ifxArrayStatement.ec"
#line 456 "ifxArrayStatement.ec"
  char *ec_select_sql;
#line 457 "ifxArrayStatement.ec"
  char *ec_cursor_id;
/*
 *   EXEC SQL END DECLARE SECTION;
 */
#line 458 "ifxArrayStatement.ec"


  ec_select_sql   = (char*)str_ifx_sql.c_str();
  ec_cursor_id    = (char*) str_var_cursor_id.c_str();

/*
 *   EXEC SQL PREPARE :ec_cursor_id FROM :ec_select_sql;
 */
#line 463 "ifxArrayStatement.ec"
  {
#line 463 "ifxArrayStatement.ec"
#line 463 "ifxArrayStatement.ec"
#line 463 "ifxArrayStatement.ec"
  sqli_prep(ESQLINTVERSION, (char *) ec_cursor_id, ec_select_sql,(ifx_literal_t *) 0, (ifx_namelist_t *) 0, -1, 0,0 ); 
#line 463 "ifxArrayStatement.ec"
  }
  IfxConversion::checkException(__XDBC_HOOK_INVOKE_MORE SQLCODE,SQLSTATE,IFXARRAYSTATEMENT_CLASS_NAME);
  __XDBC_HOOK_CHECK(return);

/*
 *   EXEC SQL DESCRIBE INPUT :ec_cursor_id into __mp_ec_sqlda_vars;
 */
#line 467 "ifxArrayStatement.ec"
  {
#line 467 "ifxArrayStatement.ec"
#line 467 "ifxArrayStatement.ec"
  sqli_describe_input_stmt(ESQLINTVERSION, sqli_curs_locate(ESQLINTVERSION, (char *) ec_cursor_id, 257), &__mp_ec_sqlda_vars, (char *) 0);
#line 467 "ifxArrayStatement.ec"
  }
  int __SQLCODE = SQLCODE;
  string __SQLSTATE = 0;

/*
 *   EXEC SQL FREE  :ec_cursor_id;
 */
#line 471 "ifxArrayStatement.ec"
  {
#line 471 "ifxArrayStatement.ec"
#line 471 "ifxArrayStatement.ec"
  sqli_curs_free(ESQLINTVERSION, sqli_curs_locate(ESQLINTVERSION, (char *) ec_cursor_id, 258));
#line 471 "ifxArrayStatement.ec"
  }
  IfxConversion::checkException(__XDBC_HOOK_INVOKE_MORE __SQLCODE,__SQLSTATE,IFXARRAYSTATEMENT_CLASS_NAME);
}


void  IfxArrayStatement::parseInColumns_insert(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException)
{
/*
 *   EXEC SQL BEGIN DECLARE SECTION;
 */
#line 478 "ifxArrayStatement.ec"
#line 479 "ifxArrayStatement.ec"
#line 479 "ifxArrayStatement.ec"
  char *ec_select_sql;
#line 480 "ifxArrayStatement.ec"
  char *ec_cursor_id;
/*
 *   EXEC SQL END DECLARE SECTION;
 */
#line 481 "ifxArrayStatement.ec"


  ec_select_sql   = (char*)str_ifx_sql.c_str();
  ec_cursor_id    = (char*) str_var_cursor_id.c_str();

/*
 *   EXEC SQL PREPARE :ec_cursor_id FROM :ec_select_sql;
 */
#line 486 "ifxArrayStatement.ec"
  {
#line 486 "ifxArrayStatement.ec"
#line 486 "ifxArrayStatement.ec"
#line 486 "ifxArrayStatement.ec"
  sqli_prep(ESQLINTVERSION, (char *) ec_cursor_id, ec_select_sql,(ifx_literal_t *) 0, (ifx_namelist_t *) 0, -1, 0,0 ); 
#line 486 "ifxArrayStatement.ec"
  }
  IfxConversion::checkException(__XDBC_HOOK_INVOKE_MORE SQLCODE,SQLSTATE,IFXARRAYSTATEMENT_CLASS_NAME);
  __XDBC_HOOK_CHECK(return);

/*
 *   EXEC SQL DESCRIBE :ec_cursor_id into __mp_ec_sqlda_vars;
 */
#line 490 "ifxArrayStatement.ec"
  {
#line 490 "ifxArrayStatement.ec"
#line 490 "ifxArrayStatement.ec"
  sqli_describe_stmt(ESQLINTVERSION, sqli_curs_locate(ESQLINTVERSION, (char *) ec_cursor_id, 257), &__mp_ec_sqlda_vars, (char *) 0);
#line 490 "ifxArrayStatement.ec"
  }
  int __SQLCODE = SQLCODE;
  string __SQLSTATE = 0;

/*
 *   EXEC SQL FREE  :ec_cursor_id;
 */
#line 494 "ifxArrayStatement.ec"
  {
#line 494 "ifxArrayStatement.ec"
#line 494 "ifxArrayStatement.ec"
  sqli_curs_free(ESQLINTVERSION, sqli_curs_locate(ESQLINTVERSION, (char *) ec_cursor_id, 258));
#line 494 "ifxArrayStatement.ec"
  }
  IfxConversion::checkException(__XDBC_HOOK_INVOKE_MORE __SQLCODE,__SQLSTATE,IFXARRAYSTATEMENT_CLASS_NAME);
}

void  IfxArrayStatement::parseInColumns_guess(__XDBC_HOOK_DECLARE_MORE const string& table_name,const string& column_list) __XDBC_THROW_DECLARE(SQLException)
{
  if(column_list.empty())
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
    __XDBC_FORCED_TRACE(IFXARRAYSTATEMENT_CLASS_NAME,__sb.str());
  }


/*
 *   EXEC SQL BEGIN DECLARE SECTION;
 */
#line 519 "ifxArrayStatement.ec"
#line 520 "ifxArrayStatement.ec"
#line 520 "ifxArrayStatement.ec"
  char *ec_select_sql;
#line 521 "ifxArrayStatement.ec"
  char *ec_cursor_id;
/*
 *   EXEC SQL END DECLARE SECTION;
 */
#line 522 "ifxArrayStatement.ec"


  ec_select_sql   = (char*) inColumnsSQL.c_str();
  ec_cursor_id    = (char*) str_var_cursor_id.c_str();

/*
 *   EXEC SQL PREPARE :ec_cursor_id FROM :ec_select_sql;
 */
#line 527 "ifxArrayStatement.ec"
  {
#line 527 "ifxArrayStatement.ec"
#line 527 "ifxArrayStatement.ec"
#line 527 "ifxArrayStatement.ec"
  sqli_prep(ESQLINTVERSION, (char *) ec_cursor_id, ec_select_sql,(ifx_literal_t *) 0, (ifx_namelist_t *) 0, -1, 0,0 ); 
#line 527 "ifxArrayStatement.ec"
  }
  IfxConversion::checkException(__XDBC_HOOK_INVOKE_MORE SQLCODE,SQLSTATE,IFXARRAYSTATEMENT_CLASS_NAME);
  __XDBC_HOOK_CHECK(return);

/*
 *   EXEC SQL DESCRIBE :ec_cursor_id into __mp_ec_sqlda_vars;
 */
#line 531 "ifxArrayStatement.ec"
  {
#line 531 "ifxArrayStatement.ec"
#line 531 "ifxArrayStatement.ec"
  sqli_describe_stmt(ESQLINTVERSION, sqli_curs_locate(ESQLINTVERSION, (char *) ec_cursor_id, 257), &__mp_ec_sqlda_vars, (char *) 0);
#line 531 "ifxArrayStatement.ec"
  }
  int __SQLCODE = SQLCODE;
  string __SQLSTATE = 0;

/*
 *   EXEC SQL FREE  :ec_cursor_id;
 */
#line 535 "ifxArrayStatement.ec"
  {
#line 535 "ifxArrayStatement.ec"
#line 535 "ifxArrayStatement.ec"
  sqli_curs_free(ESQLINTVERSION, sqli_curs_locate(ESQLINTVERSION, (char *) ec_cursor_id, 258));
#line 535 "ifxArrayStatement.ec"
  }
  IfxConversion::checkException(__XDBC_HOOK_INVOKE_MORE __SQLCODE,__SQLSTATE,IFXARRAYSTATEMENT_CLASS_NAME);
}

void  IfxArrayStatement::setBindData(__XDBC_HOOK_DECLARE_MORE DataTable* paramTable,int row) __XDBC_THROW_DECLARE(SQLException)
{
  int colcount = paramTable->getColumnCount();
  for(int i=1; i<=colcount;++i)
  {
    setBindData(__XDBC_HOOK_INVOKE_MORE paramTable,row,i);
    __XDBC_HOOK_CHECK(return);
  }
}

void  IfxArrayStatement::setBindData(__XDBC_HOOK_DECLARE_MORE DataTable* paramTable,int row, int parameterIndex) __XDBC_THROW_DECLARE(SQLException)
{
  const char* colData = paramTable->getModel()->getDataBuffer() + paramTable->getColumnOffset(parameterIndex);
  int colSize = paramTable->getColumnSize(parameterIndex);
  int colSkip = paramTable->getColumnSkip(parameterIndex);
  int colType = 0;
  if(paramTable->getModel()->useColumnRawType(parameterIndex))
  {
    colType =  paramTable->getModel()->getColumnRawType(parameterIndex);
  }
  else
  {
    colType = IfxConversion::XDBC_TO_DATABASE(paramTable->getColumnType(parameterIndex));
  }

  colData += (row - 1) * colSkip;

  const char* indData = 0;
  int indSize = 0;
  int indSkip = 0;

  if(paramTable->getModel()->useColumnIndicator(parameterIndex))
  {
    indData=paramTable->getModel()->getIndicatorBuffer()+paramTable->getModel()->getColumnIndicatorOffset(parameterIndex);
    indSize = paramTable->getModel()->getColumnIndicatorSize(parameterIndex);
    indSkip = paramTable->getModel()->getColumnIndicatorSkip(parameterIndex);
  }

  colData += (row - 1) * colSkip;
  indData += (row - 1) * indSkip;

  struct sqlvar_struct* p_ec_column = &(__mp_ec_sqlda_vars->sqlvar[parameterIndex-1]);

  p_ec_column->sqlind = (int2*)indData;
  p_ec_column->sqlilen = indSize;
  p_ec_column->sqlitype = SQLSMINT;

  p_ec_column->sqldata = (char*)colData;
  p_ec_column->sqllen = colSize;
  p_ec_column->sqltype = colType;

}

__OPENCODE_END_NAMESPACE

#line 592 "ifxArrayStatement.ec"
