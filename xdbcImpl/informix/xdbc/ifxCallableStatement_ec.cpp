#include <sqlhdr.h>
#include <sqliapi.h>
#line 1 "ifxCallableStatement.ec"
#include "xdbc/ifxCallableStatement.h"
#include "xdbc/ifxClob.h"
#include "xdbc/ifxBlob.h"
#include "xdbc/xsmart.h"
#include "xdbc/ifxConnection.h"
#include "xdbc/ifxConversion.h"
#include "xdbc/ifxSQL.h"

#include "port/ioc_sbuf.h"
#include "port/file.h"

__OPENCODE_BEGIN_NAMESPACE

//-------------------------------------------------------------------------

#define IFXCALLABLESTATEMENT_CLASS_NAME "IfxCallableStatement"

//-------------------------------------------------------------------------
IfxCallableStatement::~IfxCallableStatement()
{
  __XDBC_TRACE(IFXCALLABLESTATEMENT_CLASS_NAME,"destroy()");
}

IfxCallableStatement::IfxCallableStatement(IfxConnection* ora_conn,const string& sql,int paramCount)
{
  __sp_pstmt.reset(new IfxPreparedStatement(ora_conn,sql,paramCount));
  __mp_value_buffer = 0;
  __mi_value_buf_size = 0;

  __mp_ec_sqlda_values = 0;
  __mi_value_count = 0;

  __mb_is_proc = true;
}

IfxCallableStatement::IfxCallableStatement(IfxConnection* ora_conn,const string& sql, int resultSetType, int resultSetConcurrency,int paramCount)
{
  __sp_pstmt.reset(new IfxPreparedStatement(ora_conn,sql,resultSetType,resultSetConcurrency,paramCount));

  __mp_value_buffer = 0;
  __mi_value_buf_size = 0;

  __mp_ec_sqlda_values = 0;
  __mi_value_count = 0;

  __mb_is_proc = true;
}

//-------------------------------------------------
// Ifx Interfaces
//-------------------------------------------------
void IfxCallableStatement::initialize(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{
//------------------------------------------------------------
// 1. call base function
//------------------------------------------------------------
  __sp_pstmt->initialize(__XDBC_HOOK_INVOKE_ONCE);

//------------------------------------------------------------
// 2. parse procedure name
//------------------------------------------------------------
//  parseProcName(__XDBC_HOOK_INVOKE_ONCE);

//------------------------------------------------------------
// 3. retrive return value info
//------------------------------------------------------------
/*
 *   EXEC SQL BEGIN DECLARE SECTION;
 */
#line 67 "ifxCallableStatement.ec"
#line 68 "ifxCallableStatement.ec"
#line 68 "ifxCallableStatement.ec"
  char *ec_cursor_name;
#line 69 "ifxCallableStatement.ec"
  char *ec_cursor_id;
/*
 *   EXEC SQL END DECLARE SECTION;
 */
#line 70 "ifxCallableStatement.ec"


  ec_cursor_name  = (char*)__sp_pstmt->__ms_cursor_name.c_str();
  ec_cursor_id    = (char*)__sp_pstmt->__ms_cursor_id.c_str();

/*
 *   EXEC SQL DESCRIBE :ec_cursor_id into __mp_ec_sqlda_values;
 */
#line 75 "ifxCallableStatement.ec"
  {
#line 75 "ifxCallableStatement.ec"
#line 75 "ifxCallableStatement.ec"
  sqli_describe_stmt(ESQLINTVERSION, sqli_curs_locate(ESQLINTVERSION, (char *) ec_cursor_id, 257), &__mp_ec_sqlda_values, (char *) 0);
#line 75 "ifxCallableStatement.ec"
  }
  if(SQLCODE != SQ_EXECPROC)
  {
    __mb_is_proc = false;
  }
  IfxConversion::checkException(__XDBC_HOOK_INVOKE_MORE SQLCODE,SQLSTATE,IFXCALLABLESTATEMENT_CLASS_NAME);
  __XDBC_HOOK_CHECK(return);

  if(__mb_is_proc)
  {
    int numcols = 0;
    if(__mp_ec_sqlda_values != 0)
    {
      numcols = __mp_ec_sqlda_values->sqld;
      IfxConversion::parseResultSetMetaData(__mp_ec_sqlda_values,__vector_values);
    }

    __mi_value_count = numcols;

    if(DriverManager::isTracingOn)
    {
      StringBuffer __sb;
      __sb << "<init>|" << File::lineSeparator;
      int value_size = (int)__vector_values.size();
      for(int i=0;i<value_size;++i)
      {
        __vector_values[i].dump(__sb);
      }

      __XDBC_FORCED_TRACE(IFXCALLABLESTATEMENT_CLASS_NAME,__sb.str());
    }

//------------------------------------------------------------
// 4. allocate & define  value data buffer
//------------------------------------------------------------
    IfxConversion::prepareColumnData(__XDBC_HOOK_INVOKE_MORE __mp_ec_sqlda_values,__vector_values,__vector_defines,__mp_value_buffer,__mi_value_buf_size,0,true);

  }//~end if(__mb_is_proc)
}

void IfxCallableStatement::registerOutParameter(__XDBC_HOOK_DECLARE_MORE int paramIndex, int sqlType)  __XDBC_THROW_DECLARE(SQLException)
{
  __XDBC_TRACE(IFXCALLABLESTATEMENT_CLASS_NAME,"operation 'registerOutParameter'  not supported");

  __XDBC_INFORMIXCALL_CHECKPARAM(paramIndex);
}

void IfxCallableStatement::registerOutParameter(__XDBC_HOOK_DECLARE_MORE int paramIndex, int sqlType, int scale)  __XDBC_THROW_DECLARE(SQLException)
{
  registerOutParameter(__XDBC_HOOK_INVOKE_MORE paramIndex,sqlType);
}

bool IfxCallableStatement::wasNull(__XDBC_HOOK_DECLARE_MORE int paramIndex)   __XDBC_THROW_DECLARE(SQLException)
{
  checkStoredProcedure(__XDBC_HOOK_INVOKE_ONCE);
  __XDBC_HOOK_CHECK(return false);

  return __vector_defines[paramIndex].__mi_define_ind == -1;
}

bool IfxCallableStatement::getByte(__XDBC_HOOK_DECLARE_MORE int paramIndex,char& paramval)  __XDBC_THROW_DECLARE(SQLException)
{
  __XDBC_INFORMIXCALL_CHECKNULL_BOOLEAN(paramIndex);

  if((__vector_values[paramIndex].__mi_type & SQLTYPE) == SQLSMINT)
  {
    paramval = __vector_defines[paramIndex].ifx_value.ec_SMINT;
    return true;
  }

  short __paramval = 0;
  checkConversion(__XDBC_HOOK_INVOKE_MORE paramIndex,SQLSMINT,(char*)&__paramval);
  __XDBC_HOOK_CHECK(return false);

  paramval = __paramval;
  return true;
}

bool IfxCallableStatement::getShort(__XDBC_HOOK_DECLARE_MORE int paramIndex,short& paramval)  __XDBC_THROW_DECLARE(SQLException)
{
  __XDBC_INFORMIXCALL_CHECKNULL_BOOLEAN(paramIndex);

  if((__vector_values[paramIndex].__mi_type & SQLTYPE) == SQLSMINT)
  {
    paramval = __vector_defines[paramIndex].ifx_value.ec_SMINT;
    return true;
  }

  return checkConversion(__XDBC_HOOK_INVOKE_MORE paramIndex,SQLSMINT,(char*)&paramval);
}

bool IfxCallableStatement::getInt(__XDBC_HOOK_DECLARE_MORE int paramIndex,int& paramval)  __XDBC_THROW_DECLARE(SQLException)
{
  __XDBC_INFORMIXCALL_CHECKNULL_BOOLEAN(paramIndex);

  if((__vector_values[paramIndex].__mi_type & SQLTYPE) == SQLINT)
  {
    paramval = __vector_defines[paramIndex].ifx_value.ec_INT;
    return true;
  }

  int __paramval = 0;
  checkConversion(__XDBC_HOOK_INVOKE_MORE paramIndex,SQLINT,(char*)&__paramval);
  __XDBC_HOOK_CHECK(return false);

  paramval = __paramval;
  return true;
}

bool IfxCallableStatement::getLong(__XDBC_HOOK_DECLARE_MORE int paramIndex,XLONG& paramval)  __XDBC_THROW_DECLARE(SQLException)
{
  __XDBC_INFORMIXCALL_CHECKNULL_BOOLEAN(paramIndex);

  if((__vector_values[paramIndex].__mi_type & SQLTYPE) == SQLINT8)
  {
    IfxConversion::int8ToLong(&(__vector_defines[paramIndex].ifx_value.ec_int8),paramval);
    return true;
  }

  return checkConversion(__XDBC_HOOK_INVOKE_MORE paramIndex,SQLINT8,(char*)&paramval);
}

bool IfxCallableStatement::getFloat(__XDBC_HOOK_DECLARE_MORE int paramIndex,float& paramval)  __XDBC_THROW_DECLARE(SQLException)
{
  __XDBC_INFORMIXCALL_CHECKNULL_BOOLEAN(paramIndex);

  if((__vector_values[paramIndex].__mi_type & SQLTYPE) == SQLFLOAT)
  {
    paramval = (float)__vector_defines[paramIndex].ifx_value.ec_DOUBLE;
    return true;
  }

  double __paramval = 0;
  checkConversion(__XDBC_HOOK_INVOKE_MORE paramIndex,SQLFLOAT,(char*)&__paramval);
  __XDBC_HOOK_CHECK(return false);

  paramval = __paramval;
  return true;
}

bool IfxCallableStatement::getDouble(__XDBC_HOOK_DECLARE_MORE int paramIndex,double& paramval)  __XDBC_THROW_DECLARE(SQLException)
{
  __XDBC_INFORMIXCALL_CHECKNULL_BOOLEAN(paramIndex);

  if((__vector_values[paramIndex].__mi_type & SQLTYPE) == SQLDECIMAL)
  {
    dectodbl(&(__vector_defines[paramIndex].ifx_value.ec_decimal), &paramval);
    return true;
  }

  return checkConversion(__XDBC_HOOK_INVOKE_MORE paramIndex,SQLDECIMAL,(char*)&paramval);
}

bool IfxCallableStatement::getDate(__XDBC_HOOK_DECLARE_MORE int paramIndex,XDBC_DATE& paramval)  __XDBC_THROW_DECLARE(SQLException)
{
  __XDBC_INFORMIXCALL_CHECKNULL_BOOLEAN(paramIndex);

  if((__vector_values[paramIndex].__mi_type & SQLTYPE) == SQLDATE)
  {
    short mdy_array[3];
    rjulmdy(__vector_defines[paramIndex].ifx_value.ec_INT, mdy_array);

    paramval.year  = mdy_array[2];
    paramval.month = mdy_array[0];
    paramval.day   = mdy_array[1];

    return true;
  }

  return checkConversion(__XDBC_HOOK_INVOKE_MORE paramIndex,SQLDATE,(char*)&paramval);
}

bool IfxCallableStatement::getTime(__XDBC_HOOK_DECLARE_MORE int paramIndex,XDBC_TIME& paramval)  __XDBC_THROW_DECLARE(SQLException)
{
  __XDBC_INFORMIXCALL_CHECKNULL_BOOLEAN(paramIndex);

  XDBC_TIMESTAMP __sql_timestamp;
  if(getTimestamp(__XDBC_HOOK_INVOKE_MORE paramIndex,__sql_timestamp))
  {
    paramval.hour = __sql_timestamp.hour;
    paramval.minute = __sql_timestamp.minute;
    paramval.second = __sql_timestamp.second;

    return true;
  }

  return false;
}

bool IfxCallableStatement::getTimestamp(__XDBC_HOOK_DECLARE_MORE int paramIndex,XDBC_TIMESTAMP& paramval)  __XDBC_THROW_DECLARE(SQLException)
{
  __XDBC_INFORMIXCALL_CHECKNULL_BOOLEAN(paramIndex);

  if((__vector_values[paramIndex].__mi_type & SQLTYPE) == SQLDTIME)
  {
    return IfxConversion::parseDateTime(&(__vector_defines[paramIndex].ifx_value.ec_dtime),paramval);
  }

  return checkConversion(__XDBC_HOOK_INVOKE_MORE paramIndex,SQLDTIME,(char*)&paramval);
}

bool IfxCallableStatement::getInterval(__XDBC_HOOK_DECLARE_MORE int paramIndex,XDBC_INTERVAL& paramval)  __XDBC_THROW_DECLARE(SQLException)
{
  __XDBC_INFORMIXCALL_CHECKNULL_BOOLEAN(paramIndex);

  if((__vector_values[paramIndex].__mi_type & SQLTYPE) == SQLSMINT)
  {
    return IfxConversion::parseInterval(&(__vector_defines[paramIndex].ifx_value.ec_interval),paramval);
  }

  return checkConversion(__XDBC_HOOK_INVOKE_MORE paramIndex,SQLINTERVAL,(char*)&paramval);
}

const char* IfxCallableStatement::getString(__XDBC_HOOK_DECLARE_MORE int paramIndex,int& sizeBytes)  __XDBC_THROW_DECLARE(SQLException)
{
  sizeBytes = 0;
  __XDBC_INFORMIXCALL_CHECKNULL_POINTER(paramIndex);

  if((__vector_values[paramIndex].__mi_type & SQLTYPE) == SQLVCHAR)
  {
    sizeBytes = (int)__sp_pstmt->__vector_binds[paramIndex].string_value.size();
    return __sp_pstmt->__vector_binds[paramIndex].string_value.data();
  }

  string& __tmp_str = __sp_pstmt->__vector_binds[paramIndex].__tmp_std_str;
  checkConversion(__XDBC_HOOK_INVOKE_MORE paramIndex,SQLVCHAR,(char*)&__tmp_str);
  __XDBC_HOOK_CHECK(return 0);

  sizeBytes = (int)__tmp_str.size();
  return __tmp_str.data();
}

Blob* IfxCallableStatement::getBlob(__XDBC_HOOK_DECLARE_MORE int paramIndex)  __XDBC_THROW_DECLARE(SQLException)
{
  __XDBC_INFORMIXCALL_CHECKNULL_POINTER(paramIndex);

  if((__vector_values[paramIndex].__mi_type & SQLTYPE) == SQLUDTFIXED)
  {
    struct ifx_lo_ts* p_ec_lo_ts = &(__vector_defines[paramIndex].ifx_value.ec_lo_ts);
    return new IfxBlob(__sp_pstmt->__mp_conn,p_ec_lo_ts);
  }

  IfxBlob blob(__sp_pstmt->__mp_conn);
  checkConversion(__XDBC_HOOK_INVOKE_MORE paramIndex,SQLUDTFIXED,(char*)&blob);
  __XDBC_HOOK_CHECK(return 0);

  IfxBlob* p_blob=new IfxBlob(__sp_pstmt->__mp_conn);
  p_blob->swap(blob);
  return p_blob;
}

Clob* IfxCallableStatement::getClob(__XDBC_HOOK_DECLARE_MORE int paramIndex)  __XDBC_THROW_DECLARE(SQLException)
{
  __XDBC_INFORMIXCALL_CHECKNULL_POINTER(paramIndex);

  if((__vector_values[paramIndex].__mi_type & SQLTYPE) == SQLUDTFIXED)
  {
    struct ifx_lo_ts* p_ec_lo_ts = &(__vector_defines[paramIndex].ifx_value.ec_lo_ts);
    return new IfxClob(__sp_pstmt->__mp_conn,p_ec_lo_ts);
  }

  IfxClob Clob(__sp_pstmt->__mp_conn);
  checkConversion(__XDBC_HOOK_INVOKE_MORE paramIndex,SQLUDTFIXED,(char*)&Clob);
  __XDBC_HOOK_CHECK(return 0);

  IfxClob* p_clob=new IfxClob(__sp_pstmt->__mp_conn);
  p_clob->swap(Clob);
  return p_clob;
}

//----------------------------------------------------------------------------------------
// PreparedStatement Interfaces
//----------------------------------------------------------------------------------------
ResultSet*  IfxCallableStatement::executeQuery(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{
  if( (__mb_is_proc) || (__sp_pstmt->__mi_stmt_type == IfxSQL::SQL_STMT_BEGIN))
  {
//----------------------------------------------------------------
    if(__vector_values.empty())
    {
      __XDBC_TRACE(IFXCALLABLESTATEMENT_CLASS_NAME,"executeQuery() - ");
    }

    if(__vector_values.empty())
    {
      StringBuffer __sb;
      __sb.append("[");
      __sb.append(IFXCALLABLESTATEMENT_CLASS_NAME);
      __sb.append("] Prepared  statement  has no return values - ");
      __sb.append(__sp_pstmt->str_orginal_ifx_sql);

      if(DriverManager::isTracingOn)
      {
        StringBuffer __sb;
        __sb << "executeQuery|" << "Prepared  statement  has no return values - " <<  __sp_pstmt->str_orginal_ifx_sql;
        __XDBC_FORCED_TRACE(IFXCALLABLESTATEMENT_CLASS_NAME,__sb.str());
      }

      __XDBC_HOOK_RETURN(return 0,__sb.str());
    }
//----------------------------------------------------------------
  }

  return __sp_pstmt->executeQuery(__XDBC_HOOK_INVOKE_ONCE);
}

int IfxCallableStatement::executeUpdate(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{
  if( (__mb_is_proc) || (__sp_pstmt->__mi_stmt_type == IfxSQL::SQL_STMT_BEGIN))
  {
//----------------------------------------------------------------
    if(!__vector_values.empty())
    {
      __XDBC_TRACE(IFXCALLABLESTATEMENT_CLASS_NAME,"executeUpdate()");
    }

/*
 *     EXEC SQL BEGIN DECLARE SECTION;
 */
#line 391 "ifxCallableStatement.ec"
#line 392 "ifxCallableStatement.ec"
#line 392 "ifxCallableStatement.ec"
  char *ec_cursor_name;
#line 393 "ifxCallableStatement.ec"
  char *ec_cursor_id;
/*
 *     EXEC SQL END DECLARE SECTION;
 */
#line 394 "ifxCallableStatement.ec"


    ec_cursor_name  = (char*)__sp_pstmt->__ms_cursor_name.c_str();
    ec_cursor_id    = (char*)__sp_pstmt->__ms_cursor_id.c_str();

    int affected_row = 0;

    if(!__vector_values.empty())
    {
/*
 *       EXEC SQL DECLARE :ec_cursor_name SCROLL CURSOR FOR :ec_cursor_id ;
 */
#line 403 "ifxCallableStatement.ec"
  {
#line 403 "ifxCallableStatement.ec"
#line 403 "ifxCallableStatement.ec"
  sqli_curs_decl_dynm(ESQLINTVERSION, sqli_curs_locate(ESQLINTVERSION, (char *) ec_cursor_name, 0), (char *) ec_cursor_name, sqli_curs_locate(ESQLINTVERSION, (char *) ec_cursor_id, 1), 32, 0);
#line 403 "ifxCallableStatement.ec"
  }

      if(__sp_pstmt->__mi_var_count == 0)
      {
/*
 *         EXEC SQL OPEN :ec_cursor_name;
 */
#line 407 "ifxCallableStatement.ec"
  {
#line 407 "ifxCallableStatement.ec"
#line 407 "ifxCallableStatement.ec"
  sqli_curs_open(ESQLINTVERSION, sqli_curs_locate(ESQLINTVERSION, (char *) ec_cursor_name, 256), (ifx_sqlda_t *) 0, (char *) 0, (struct value *) 0, 0, 0);
#line 407 "ifxCallableStatement.ec"
  }
      }
      else
      {
        struct sqlda* __xp_ec_sqlda_vars = __sp_pstmt->__mp_ec_sqlda_vars;
/*
 *         EXEC SQL OPEN :ec_cursor_name  USING DESCRIPTOR __xp_ec_sqlda_vars;
 */
#line 412 "ifxCallableStatement.ec"
  {
#line 412 "ifxCallableStatement.ec"
#line 412 "ifxCallableStatement.ec"
  sqli_curs_open(ESQLINTVERSION, sqli_curs_locate(ESQLINTVERSION, (char *) ec_cursor_name, 256), __xp_ec_sqlda_vars, (char *) 0, (struct value *) 0, 1, 0);
#line 412 "ifxCallableStatement.ec"
  }
      }

/*
 *       EXEC SQL OPEN :ec_cursor_name;
 */
#line 415 "ifxCallableStatement.ec"
  {
#line 415 "ifxCallableStatement.ec"
#line 415 "ifxCallableStatement.ec"
  sqli_curs_open(ESQLINTVERSION, sqli_curs_locate(ESQLINTVERSION, (char *) ec_cursor_name, 256), (ifx_sqlda_t *) 0, (char *) 0, (struct value *) 0, 0, 0);
#line 415 "ifxCallableStatement.ec"
  }
      IfxConversion::checkException(__XDBC_HOOK_INVOKE_MORE SQLCODE,SQLSTATE,IFXCALLABLESTATEMENT_CLASS_NAME);
      __XDBC_HOOK_CHECK(return 0);

/*
 *       EXEC SQL FETCH :ec_cursor_name USING DESCRIPTOR __mp_ec_sqlda_values;
 */
#line 419 "ifxCallableStatement.ec"
  {
#line 419 "ifxCallableStatement.ec"
  static _FetchSpec _FS0 = { 0, 1, 0 };
#line 419 "ifxCallableStatement.ec"
  sqli_curs_fetch(ESQLINTVERSION, sqli_curs_locate(ESQLINTVERSION, (char *) ec_cursor_name, 256), (ifx_sqlda_t *) 0, __mp_ec_sqlda_values, (char *) 0, &_FS0);
#line 419 "ifxCallableStatement.ec"
  }
      IfxConversion::checkException(__XDBC_HOOK_INVOKE_MORE SQLCODE,SQLSTATE,IFXCALLABLESTATEMENT_CLASS_NAME);
      __XDBC_HOOK_CHECK(return 0);

/*
 *       EXEC SQL CLOSE :ec_cursor_name;
 */
#line 423 "ifxCallableStatement.ec"
  {
#line 423 "ifxCallableStatement.ec"
#line 423 "ifxCallableStatement.ec"
  sqli_curs_close(ESQLINTVERSION, sqli_curs_locate(ESQLINTVERSION, (char *) ec_cursor_name, 256));
#line 423 "ifxCallableStatement.ec"
  }
/*
 *       EXEC SQL FREE :ec_cursor_name;
 */
#line 424 "ifxCallableStatement.ec"
  {
#line 424 "ifxCallableStatement.ec"
#line 424 "ifxCallableStatement.ec"
  sqli_curs_free(ESQLINTVERSION, sqli_curs_locate(ESQLINTVERSION, (char *) ec_cursor_name, 258));
#line 424 "ifxCallableStatement.ec"
  }

      affected_row =  sqlca.sqlerrd[2];
      return affected_row;
    }

//----------------------------------------------------------------
  }

  return __sp_pstmt->executeUpdate(__XDBC_HOOK_INVOKE_ONCE);
}

void IfxCallableStatement::setByte(__XDBC_HOOK_DECLARE_MORE int paramIndex, const char& x,bool is_null)  __XDBC_THROW_DECLARE(SQLException)
{
  if( (__mb_is_proc) || (__sp_pstmt->__mi_stmt_type == IfxSQL::SQL_STMT_BEGIN))
  {
    __XDBC_INFORMIXCALL_CHECKPARAM(paramIndex);

    __sp_pstmt->__vector_binds[paramIndex].__mi_buf_size = sizeof(mint);

    struct sqlvar_struct* p_ec_var = &(__sp_pstmt->__mp_ec_sqlda_vars->sqlvar[paramIndex]);
    p_ec_var->sqltype = SQLSMINT;
    p_ec_var->sqllen  = __sp_pstmt->__vector_binds[paramIndex].__mi_buf_size;
  }

  __sp_pstmt->setByte(__XDBC_HOOK_INVOKE_MORE paramIndex,x,is_null);
}

void IfxCallableStatement::setShort(__XDBC_HOOK_DECLARE_MORE int paramIndex, const short& x,bool is_null)  __XDBC_THROW_DECLARE(SQLException)
{
  if( (__mb_is_proc) || (__sp_pstmt->__mi_stmt_type == IfxSQL::SQL_STMT_BEGIN))
  {
    __XDBC_INFORMIXCALL_CHECKPARAM(paramIndex);

    __sp_pstmt->__vector_binds[paramIndex].__mi_buf_size = sizeof(mint);

    struct sqlvar_struct* p_ec_var = &(__sp_pstmt->__mp_ec_sqlda_vars->sqlvar[paramIndex]);
    p_ec_var->sqltype = SQLSMINT;
    p_ec_var->sqllen  = __sp_pstmt->__vector_binds[paramIndex].__mi_buf_size;
  }

  __sp_pstmt->setShort(__XDBC_HOOK_INVOKE_MORE paramIndex,x,is_null);
}

void IfxCallableStatement::setInt(__XDBC_HOOK_DECLARE_MORE int paramIndex, const int& x,bool is_null)  __XDBC_THROW_DECLARE(SQLException)
{
  if( (__mb_is_proc) || (__sp_pstmt->__mi_stmt_type == IfxSQL::SQL_STMT_BEGIN))
  {
    __XDBC_INFORMIXCALL_CHECKPARAM(paramIndex);

    __sp_pstmt->__vector_binds[paramIndex].__mi_buf_size = sizeof(int4);

    struct sqlvar_struct* p_ec_var = &(__sp_pstmt->__mp_ec_sqlda_vars->sqlvar[paramIndex]);
    p_ec_var->sqltype = SQLINT;
    p_ec_var->sqllen  = __sp_pstmt->__vector_binds[paramIndex].__mi_buf_size;
  }

  __sp_pstmt->setInt(__XDBC_HOOK_INVOKE_MORE paramIndex,x,is_null);
}

void IfxCallableStatement::setLong(__XDBC_HOOK_DECLARE_MORE int paramIndex, const XLONG& x,bool is_null)  __XDBC_THROW_DECLARE(SQLException)
{
  if( (__mb_is_proc) || (__sp_pstmt->__mi_stmt_type == IfxSQL::SQL_STMT_BEGIN))
  {
    __XDBC_INFORMIXCALL_CHECKPARAM(paramIndex);

    __sp_pstmt->__vector_binds[paramIndex].__mi_buf_size = sizeof(struct ifx_int8);

    struct sqlvar_struct* p_ec_var = &(__sp_pstmt->__mp_ec_sqlda_vars->sqlvar[paramIndex]);
    p_ec_var->sqltype = SQLINT8;
    p_ec_var->sqllen  = __sp_pstmt->__vector_binds[paramIndex].__mi_buf_size;

    p_ec_var->sqldata = (char*)&(__sp_pstmt->__vector_binds[paramIndex].ifx_value.ec_int8);
  }

  __sp_pstmt->setLong(__XDBC_HOOK_INVOKE_MORE paramIndex,x,is_null);
}

void IfxCallableStatement::setFloat(__XDBC_HOOK_DECLARE_MORE int paramIndex, const float& x,bool is_null)  __XDBC_THROW_DECLARE(SQLException)
{
  if( (__mb_is_proc) || (__sp_pstmt->__mi_stmt_type == IfxSQL::SQL_STMT_BEGIN))
  {
    __XDBC_INFORMIXCALL_CHECKPARAM(paramIndex);

    __sp_pstmt->__vector_binds[paramIndex].__mi_buf_size = sizeof(double);

    struct sqlvar_struct* p_ec_var = &(__sp_pstmt->__mp_ec_sqlda_vars->sqlvar[paramIndex]);
    p_ec_var->sqltype = SQLFLOAT;
    p_ec_var->sqllen  = __sp_pstmt->__vector_binds[paramIndex].__mi_buf_size;
  }

  __sp_pstmt->setFloat(__XDBC_HOOK_INVOKE_MORE paramIndex,x,is_null);
}

void IfxCallableStatement::setDouble(__XDBC_HOOK_DECLARE_MORE int paramIndex, const double& x,bool is_null)  __XDBC_THROW_DECLARE(SQLException)
{
  if( (__mb_is_proc) || (__sp_pstmt->__mi_stmt_type == IfxSQL::SQL_STMT_BEGIN))
  {
    __XDBC_INFORMIXCALL_CHECKPARAM(paramIndex);

    __sp_pstmt->__vector_binds[paramIndex].__mi_buf_size = sizeof(struct decimal);

    struct sqlvar_struct* p_ec_var = &(__sp_pstmt->__mp_ec_sqlda_vars->sqlvar[paramIndex]);
    p_ec_var->sqltype = SQLDECIMAL;
    p_ec_var->sqllen  = __sp_pstmt->__vector_binds[paramIndex].__mi_buf_size;
    p_ec_var->sqldata = (char*)&(__sp_pstmt->__vector_binds[paramIndex].ifx_value.ec_decimal);
  }

  __sp_pstmt->setDouble(__XDBC_HOOK_INVOKE_MORE paramIndex,x,is_null);
}
void IfxCallableStatement::setDate(__XDBC_HOOK_DECLARE_MORE int paramIndex,const XDBC_DATE* x,bool is_null) __XDBC_THROW_DECLARE(SQLException)
{
  if( (__mb_is_proc) || (__sp_pstmt->__mi_stmt_type == IfxSQL::SQL_STMT_BEGIN))
  {
    StringBuffer __sb;
    __sb<< "[" << IFXCALLABLESTATEMENT_CLASS_NAME << "] - " << "operation 'setDate' not supported";
    __XDBC_HOOK_RETURN(return,__sb.str());
  }

  __sp_pstmt->setDate(__XDBC_HOOK_INVOKE_MORE paramIndex,x,is_null);
}

void IfxCallableStatement::setTime(__XDBC_HOOK_DECLARE_MORE int paramIndex,const  XDBC_TIME* x,bool is_null)  __XDBC_THROW_DECLARE(SQLException)
{
  if( (__mb_is_proc) || (__sp_pstmt->__mi_stmt_type == IfxSQL::SQL_STMT_BEGIN))
  {
    StringBuffer __sb;
    __sb<< "[" << IFXCALLABLESTATEMENT_CLASS_NAME << "] - " << "operation 'setTime' not supported";
    __XDBC_HOOK_RETURN(return,__sb.str());
  }

  __sp_pstmt->setTime(__XDBC_HOOK_INVOKE_MORE paramIndex,x,is_null);
}

void IfxCallableStatement::setTimestamp(__XDBC_HOOK_DECLARE_MORE int paramIndex,const XDBC_TIMESTAMP* x,bool is_null)  __XDBC_THROW_DECLARE(SQLException)
{
  if( (__mb_is_proc) || (__sp_pstmt->__mi_stmt_type == IfxSQL::SQL_STMT_BEGIN))
  {
    StringBuffer __sb;
    __sb<< "[" << IFXCALLABLESTATEMENT_CLASS_NAME << "] - " << "operation 'setTimestamp' not supported";
    __XDBC_HOOK_RETURN(return,__sb.str());
  }

  __sp_pstmt->setTimestamp(__XDBC_HOOK_INVOKE_MORE paramIndex,x,is_null);
}

void  IfxCallableStatement::setInterval(__XDBC_HOOK_DECLARE_MORE int paramIndex,  const XDBC_INTERVAL* x,bool is_null)  __XDBC_THROW_DECLARE(SQLException)
{
  if( (__mb_is_proc) || (__sp_pstmt->__mi_stmt_type == IfxSQL::SQL_STMT_BEGIN))
  {
    StringBuffer __sb;
    __sb<< "[" << IFXCALLABLESTATEMENT_CLASS_NAME << "] - " << "operation 'setInterval' not supported";
    __XDBC_HOOK_RETURN(return,__sb.str());
  }

  __sp_pstmt->setInterval(__XDBC_HOOK_INVOKE_MORE paramIndex,x,is_null);
}

void IfxCallableStatement::setString(__XDBC_HOOK_DECLARE_MORE int paramIndex,const string* x,bool is_null)  __XDBC_THROW_DECLARE(SQLException)
{
  __sp_pstmt->setString(__XDBC_HOOK_INVOKE_MORE paramIndex,x,is_null);
}

void IfxCallableStatement::setString(__XDBC_HOOK_DECLARE_MORE int paramIndex,const char* x,int count,bool is_null)  __XDBC_THROW_DECLARE(SQLException)
{
  __sp_pstmt->setString(__XDBC_HOOK_INVOKE_MORE paramIndex,x,count,is_null);
}

void IfxCallableStatement::setBlob (__XDBC_HOOK_DECLARE_MORE int paramIndex, const char* x,int count,bool is_null)  __XDBC_THROW_DECLARE(SQLException)
{
  if( (__mb_is_proc) || (__sp_pstmt->__mi_stmt_type == IfxSQL::SQL_STMT_BEGIN))
  {
    StringBuffer __sb;
    __sb<< "[" << IFXCALLABLESTATEMENT_CLASS_NAME << "] - " << "operation 'setBlob' not supported";
    __XDBC_HOOK_RETURN(return,__sb.str());
  }

  __sp_pstmt->setBlob(__XDBC_HOOK_INVOKE_MORE paramIndex,x,count,is_null);
}

void IfxCallableStatement::setBlob (__XDBC_HOOK_DECLARE_MORE int paramIndex,const Blob* x,bool is_null)  __XDBC_THROW_DECLARE(SQLException)
{
  if( (__mb_is_proc) || (__sp_pstmt->__mi_stmt_type == IfxSQL::SQL_STMT_BEGIN))
  {
    StringBuffer __sb;
    __sb<< "[" << IFXCALLABLESTATEMENT_CLASS_NAME << "] - " << "operation 'setBlob' not supported";
    __XDBC_HOOK_RETURN(return,__sb.str());
  }

  __sp_pstmt->setBlob(__XDBC_HOOK_INVOKE_MORE paramIndex,x,is_null);
}

void IfxCallableStatement::setClob (__XDBC_HOOK_DECLARE_MORE int paramIndex, const char* x,int count,bool is_null)  __XDBC_THROW_DECLARE(SQLException)
{
  if( (__mb_is_proc) || (__sp_pstmt->__mi_stmt_type == IfxSQL::SQL_STMT_BEGIN))
  {
    StringBuffer __sb;
    __sb<< "[" << IFXCALLABLESTATEMENT_CLASS_NAME << "] - " << "operation 'setClob' not supported";
    __XDBC_HOOK_RETURN(return,__sb.str());
  }

  __sp_pstmt->setClob(__XDBC_HOOK_INVOKE_MORE paramIndex,x,count,is_null);
}

void IfxCallableStatement::setClob (__XDBC_HOOK_DECLARE_MORE int paramIndex,const Clob* x,bool is_null)  __XDBC_THROW_DECLARE(SQLException)
{
  if( (__mb_is_proc) || (__sp_pstmt->__mi_stmt_type == IfxSQL::SQL_STMT_BEGIN))
  {
    StringBuffer __sb;
    __sb<< "[" << IFXCALLABLESTATEMENT_CLASS_NAME << "] - " << "operation 'setClob' not supported";
    __XDBC_HOOK_RETURN(return,__sb.str());
  }

  __sp_pstmt->setClob(__XDBC_HOOK_INVOKE_MORE paramIndex,x,is_null);
}

void  IfxCallableStatement::close()
{
  __XDBC_TRACE(IFXCALLABLESTATEMENT_CLASS_NAME,"close()");

  if(__mp_ec_sqlda_values != 0)
  {
    delete __mp_ec_sqlda_values;
    __mp_ec_sqlda_values = 0;
  }

  if(__mp_value_buffer!=0)
  {
    delete []__mp_value_buffer;
    __mp_value_buffer=0;
    __mi_value_buf_size = 0;
  }

  if( (__mb_is_proc) || (__sp_pstmt->__mi_stmt_type == IfxSQL::SQL_STMT_BEGIN))
  {
    if(!__vector_values.empty())
    {
    }
  }

  if(__sp_pstmt->__mp_ec_sqlda_vars != 0)
  {
    delete[] __sp_pstmt->__mp_ec_sqlda_vars->sqlvar;
    delete __sp_pstmt->__mp_ec_sqlda_vars;
    __sp_pstmt->__mp_ec_sqlda_vars = 0;
  }

  __sp_pstmt->close();
}

void  IfxCallableStatement::destroy()
{
  delete this;
}

int IfxCallableStatement::getResultSetType(__XDBC_HOOK_DECLARE_ONCE)   __XDBC_THROW_DECLARE(SQLException)
{
  return __sp_pstmt->getResultSetType(__XDBC_HOOK_INVOKE_ONCE);
}

Connection* IfxCallableStatement::getConnection(__XDBC_HOOK_DECLARE_ONCE)   __XDBC_THROW_DECLARE(SQLException)
{
  return __sp_pstmt->getConnection(__XDBC_HOOK_INVOKE_ONCE);
}

void  IfxCallableStatement::parseProcName(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{
  int flag=__sp_pstmt->str_orginal_ifx_sql.find("call");
  if(flag==string::npos)
  {
    flag=__sp_pstmt->str_orginal_ifx_sql.find("execute procedure");
    if(flag==string::npos)
    {
      __XDBC_HOOK_RETURN(return,"procedure not found");
    }
  }

  int end=__sp_pstmt->str_orginal_ifx_sql.find("(",flag);
  if(end==string::npos)
  {
    __XDBC_HOOK_RETURN(return,"procedure not found");
  }

  int start=flag+strlen("call");
  while(__sp_pstmt->str_orginal_ifx_sql.at(start) == ' ')
  {
    start++;
  }

  while(__sp_pstmt->str_orginal_ifx_sql.at(end) == ' ')
  {
    end--;
  }

  __ms_proc_name=__sp_pstmt->str_orginal_ifx_sql.substr(start,end-start);
}

bool IfxCallableStatement::checkConversion(__XDBC_HOOK_DECLARE_MORE int paramIndex,int outType,char* outBuffer) __XDBC_THROW_DECLARE(SQLException)
{
  int inType=__vector_values[paramIndex].__mi_type & SQLTYPE;

  //printf("--column_name : [%s] ,outType : [%d] ,inType : [%d] \n",__vector_values[paramIndex].__ms_name.c_str(),outType,inType);

  return IfxConversion::checkConversion(__XDBC_HOOK_INVOKE_MORE "IfxCallableStatement",
                                        __mp_value_buffer,__vector_defines[paramIndex],inType,
                                        outType,outBuffer,__vector_values[paramIndex].__ms_name);
}

void  IfxCallableStatement::checkStoredProcedure(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{
  if( (! __mb_is_proc) && (__sp_pstmt->__mi_stmt_type != IfxSQL::SQL_STMT_BEGIN))
  {
    StringBuffer __sb;
    __sb.append("[");
    __sb.append(IFXCALLABLESTATEMENT_CLASS_NAME);
    __sb.append("] Prepared statement is not EXECUTE PROCEDURE - ");
    __sb.append(__sp_pstmt->str_orginal_ifx_sql);
    __XDBC_HOOK_RETURN(return,__sb.str());
  }
}

bool IfxCallableStatement::parseInColumns_begin(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException)
{
  if(__sp_pstmt->__mi_var_count > 0)
  {
    __sp_pstmt->__mp_ec_sqlda_vars = new struct sqlda();
    memset(__sp_pstmt->__mp_ec_sqlda_vars,0,sizeof(struct sqlda));

   __sp_pstmt->__mp_ec_sqlda_vars->sqld = __sp_pstmt->__mi_var_count;
   __sp_pstmt->__mp_ec_sqlda_vars->desc_occ = sizeof(struct sqlda);

   __sp_pstmt->__mp_ec_sqlda_vars->sqlvar = new struct sqlvar_struct[__sp_pstmt->__mi_var_count];
  }

  for(int i=0;i<__sp_pstmt->__mi_var_count;++i)
  {
    struct sqlvar_struct* p_ec_var = &(__sp_pstmt->__mp_ec_sqlda_vars->sqlvar[i]);
    memset(p_ec_var,0,sizeof(struct sqlvar_struct));

    p_ec_var->sqltype = SQLVCHAR;
    p_ec_var->sqllen = SQLVCHAR_BYTE_MAX;
  }

  IfxConversion::parseResultSetMetaData(__sp_pstmt->__mp_ec_sqlda_vars,__sp_pstmt->__vector_vars);

  IfxConversion::prepareVariableData(__XDBC_HOOK_INVOKE_MORE __sp_pstmt->__mp_conn,__sp_pstmt->__mp_ec_sqlda_vars,__sp_pstmt->__vector_vars,__sp_pstmt->__vector_binds,__sp_pstmt->__mp_var_buffer,__sp_pstmt->__mi_var_buf_size);

  return false;
}

__OPENCODE_END_NAMESPACE

#line 774 "ifxCallableStatement.ec"
