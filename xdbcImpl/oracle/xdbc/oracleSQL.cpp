#include "xdbc/oracleSQL.h"

#include "port/stringTokenizer.h"
#include "port/ioc_sbuf.h"

__OPENCODE_BEGIN_NAMESPACE

//-------------------------------------------------------------------------

const  int OracleSQL::SQL_STMT_UNKNOWN  = 0;
const  int OracleSQL::SQL_STMT_SELECT   = 1;
const  int OracleSQL::SQL_STMT_UPDATE   = 2;
const  int OracleSQL::SQL_STMT_DELETE   = 3;
const  int OracleSQL::SQL_STMT_INSERT   = 4;
const  int OracleSQL::SQL_STMT_CREATE   = 5;
const  int OracleSQL::SQL_STMT_DROP     = 6;
const  int OracleSQL::SQL_STMT_ALTER    = 7;  
const  int OracleSQL::SQL_STMT_BEGIN    = 8;
const  int OracleSQL::SQL_STMT_DECLARE  = 9;

#define __ORACLESQL_CLASS_NAME  "OracleSQL"

string   OracleSQL::parseSQL(__XDBC_HOOK_DECLARE_MORE const string& oracle_sql,int& param_count) __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb("parseSQL|ISQL|");
    __sb << oracle_sql;

    __XDBC_FORCED_TRACE(__ORACLESQL_CLASS_NAME,__sb.str());
  }

  OracleSQL oracleSQL;
  string s = oracleSQL.parse(__XDBC_HOOK_INVOKE_MORE oracle_sql,param_count);
  __XDBC_HOOK_CHECK(return string());
  
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb("parseSQL|OSQL|");
    __sb << s;

    __XDBC_FORCED_TRACE(__ORACLESQL_CLASS_NAME,__sb.str());
  }

  return  s;
}

int  OracleSQL::parseSQLKind(const string& oracle_sql)
{
  int stmt_kind=SQL_STMT_SELECT;
  StringTokenizer tokens(oracle_sql,"\n \r \t /", true);

  for(; tokens.hasMoreTokens() ; )
  {
    string  token=tokens.nextToken();
  
    if(StringsKit::equalsIgnoreCase("select",token.c_str())==0)
    {
      stmt_kind = SQL_STMT_SELECT;
      break;
    }
    else if(StringsKit::equalsIgnoreCase("update",token.c_str())==0)
    {
      stmt_kind = SQL_STMT_UPDATE;
      break;
    }
    else if(StringsKit::equalsIgnoreCase("delete",token.c_str())==0)
    {
      stmt_kind = SQL_STMT_DELETE;
      break;
    }
    else if(StringsKit::equalsIgnoreCase("insert",token.c_str())==0)
    {
      stmt_kind =  SQL_STMT_INSERT;
      break;
    }
    else if(StringsKit::equalsIgnoreCase("create",token.c_str())==0)
    {
      stmt_kind =  SQL_STMT_CREATE;
      break;
    }
    else if(StringsKit::equalsIgnoreCase("drop",token.c_str())==0)
    {
      stmt_kind = SQL_STMT_DROP;
      break;
    }
    else if(StringsKit::equalsIgnoreCase("alter",token.c_str())==0)
    {
      stmt_kind = SQL_STMT_ALTER;
      break;
    }
    else if(StringsKit::equalsIgnoreCase("begin",token.c_str())==0 || StringsKit::equalsIgnoreCase("call",token.c_str())==0)
    {
      stmt_kind = SQL_STMT_BEGIN;
      break;
    }
    else if(StringsKit::equalsIgnoreCase("declare",token.c_str())==0)
    {
      stmt_kind = SQL_STMT_DECLARE;
      break;
    }
  }

  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    __sb.append("parseSQLKind|return stmt_type {");
    __sb.append(stmt_kind);
    __sb.append("}");

    __XDBC_FORCED_TRACE(__ORACLESQL_CLASS_NAME,__sb.str());
  }

  return stmt_kind;
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

string  OracleSQL::parse(__XDBC_HOOK_DECLARE_MORE const string src_sql,int& param_count)  __XDBC_THROW_DECLARE(SQLException)
{
  current_argument = 1;
  i = 0;
  first = true;
  in_string = false;
  this->odbc_sql = src_sql;
  length = (int)this->odbc_sql.length();

  handleODBC(__XDBC_HOOK_INVOKE_ONCE);
  __XDBC_HOOK_CHECK(return string());
  
  if(i < length)
  {
    StringBuffer __sb;
    __sb.append("OracleSQL::parse  at position {");
    __sb.append(i);
    __sb.append("}");
    __XDBC_HOOK_RETURN(return string(),__sb.str()) ;
  }

  param_count=current_argument-1; 
  
  return this->oracle_sql;
}

void    OracleSQL::appendChar(string& stringbuffer,const char& c1)
{
  if(c1 == '?')
  {
    stringbuffer += nextArgument();
  }
  else if(c1 == ':')
  {
    stringbuffer += nextArgument();
    size_t __end_pos = odbc_sql.find_first_of(' ',i);
    int __skip = 0;
    if(__end_pos == string::npos)
    {
      __skip = odbc_sql.length() - i - 1;
    }  
    else
    {   
      char __char_end = odbc_sql[__end_pos - 1];
      if(__char_end >= '0' && __char_end <= '9')
      {
        __skip = __end_pos - i - 1;
      }
      else
      {
        __skip = __end_pos - i - 2;
      }          
    }  
    
    //printf("appendChar : [%c,%d,%d,%d,%d]\n",c1,i,odbc_sql.length(),__end_pos,__skip);
    
    i += __skip;
  }
  else
  {
    stringbuffer += c1;  
  }
}

void    OracleSQL::handleCall(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{
  //printf("OracleSQL::handleCall\n");

  bool flag = first;
  if(flag)
  {
    oracle_sql.append("BEGIN ");
  }

  skipSpace();
  handleODBC(__XDBC_HOOK_INVOKE_ONCE);
  __XDBC_HOOK_CHECK(return);
  skipSpace();
 
 if(flag)
  {
    oracle_sql.append("; END;");
  }
}

void    OracleSQL::handleDate(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{
  oracle_sql.append("TO_DATE (");
  skipSpace();
  handleODBC(__XDBC_HOOK_INVOKE_ONCE);
  __XDBC_HOOK_CHECK(return);
  oracle_sql.append(", 'YYYY-MM-DD')");
}

void    OracleSQL::handleEscape(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{
  oracle_sql.append("ESCAPE ");
  skipSpace();
  handleODBC(__XDBC_HOOK_INVOKE_ONCE);
}

void    OracleSQL::handleFunction(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{
  bool flag = first;
  if(flag)
  {
    oracle_sql.append("BEGIN ");
  }
  appendChar(oracle_sql, '?');
  skipSpace();
  if(c != '=')
  {
    StringBuffer __sb;
    __sb.append(i);
    __sb.append(". Expecting \"=\" got \"");
    __sb.append(c);
    __sb.append("\"");

    __XDBC_HOOK_RETURN(return,__sb.str());
  }
  i++;
  skipSpace();

  if(odbc_sql.find("call", i)!=i)
  {
    StringBuffer __sb;
    __sb.append(i);
    __sb.append(". Expecting \"call\"");

    __XDBC_HOOK_RETURN(return,__sb.str());
  }

  i += 4;
  oracle_sql.append(" := ");
  skipSpace();
  handleODBC(__XDBC_HOOK_INVOKE_ONCE);
  __XDBC_HOOK_CHECK(return);

  if(flag)
  {
    oracle_sql.append("; END;");
  }
}

void    OracleSQL::handleODBC(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{
  while(i < length) 
  {
    c = odbc_sql.at(i);
    //printf("handleODBC(while) - %c\n",c);

    if(in_string)
    {
      oracle_sql+=c;
      if(c == '\'')
      {
        in_string = false;
      }
      i++;
    } 
    else
    {
      switch(c)
      {
        case 39: // '\''
          oracle_sql+=c;
          in_string = true;
          i++;
          first = false;
          break;
        
        case 123: // '{'
          token_buffer="";
          i++;
          skipSpace();
          for(; i < length && (isalnum(c = odbc_sql.at(i)) || c == '?'); i++)
          {
            //printf("handleODBC(while|for) - %c\n",c);
            token_buffer+=c;
          }
          handleToken(__XDBC_HOOK_INVOKE_MORE token_buffer);
          __XDBC_HOOK_CHECK(return);
          c = odbc_sql.at(i);
          if(c != '}')
          {
            StringBuffer __sb;
            __sb.append(i);
            __sb.append(". Expecting \"}\" got \"");
            __sb.append(c);
            __sb.append("\"");
            __XDBC_HOOK_RETURN(return,__sb.str());
          }
          i++;
          break;
        
        case 125: // '}'
          return;
        
        default:
          appendChar(oracle_sql, c);
          i++;
          first = false;
          break;

      }//~end switch

    }//~end if

  }//~end while
}

void    OracleSQL::handleTime(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{
  oracle_sql.append("TO_DATE (");
  skipSpace();
  handleODBC(__XDBC_HOOK_INVOKE_ONCE);
  __XDBC_HOOK_CHECK(return);
  oracle_sql.append(", 'HH24:MI:SS')");
}

void    OracleSQL::handleTimestamp(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{
  oracle_sql.append("TO_DATE (");
  skipSpace();
  bool flag = false;
  for(; i < length && (c = odbc_sql.at(i)) != '}'; i++)
  {
    if(!flag)
    {
      if(c == '.')
      {
        flag = true;
      }
    }
    else
    {
      oracle_sql.append(""+c);
    }
  }
  
  if(flag)
  {
    oracle_sql.append(""+'\'');
  }
  oracle_sql.append(", 'YYYY-MM-DD HH24:MI:SS')");
}

void    OracleSQL::handleToken(__XDBC_HOOK_DECLARE_MORE const string& s) __XDBC_THROW_DECLARE(SQLException)
{
  //printf("handleToken : %s\n",s.c_str());

  if(StringsKit::equalsIgnoreCase("?",s.c_str()))
  {
    handleFunction(__XDBC_HOOK_INVOKE_ONCE);
    __XDBC_HOOK_CHECK(return);
  }
  else if(StringsKit::equalsIgnoreCase("call",s.c_str()))
  {
    handleCall(__XDBC_HOOK_INVOKE_ONCE);
    __XDBC_HOOK_CHECK(return);
  }
  else if(StringsKit::equalsIgnoreCase("ts",s.c_str()))
  {
    handleTimestamp(__XDBC_HOOK_INVOKE_ONCE);
    __XDBC_HOOK_CHECK(return);
  }
  else if(StringsKit::equalsIgnoreCase("t",s.c_str()))
  {
    handleTime(__XDBC_HOOK_INVOKE_ONCE);
    __XDBC_HOOK_CHECK(return);
  }
  else if(StringsKit::equalsIgnoreCase("d",s.c_str()))
  {
    handleDate(__XDBC_HOOK_INVOKE_ONCE);
    __XDBC_HOOK_CHECK(return);
  }
  else if(StringsKit::equalsIgnoreCase("escape",s.c_str()))
  {
    handleEscape(__XDBC_HOOK_INVOKE_ONCE);
    __XDBC_HOOK_CHECK(return);
  } 
  else
  {
    StringBuffer __sb;
    __sb.append(i);
    __sb.append(": ");
    __sb.append(s);
    __XDBC_HOOK_RETURN(return,__sb.str());
  }
}

string  OracleSQL::nextArgument()
{
  StringBuffer __sb;
  __sb.append(":");
  __sb.append(current_argument);
  current_argument++;
  return __sb.str();
}

void    OracleSQL::skipSpace()
{
  for(; i < length && (c = odbc_sql.at(i)) == ' '; i++);
}

__OPENCODE_END_NAMESPACE
