#include "xdbc/ifxSQL.h"

#include <ctype.h>

#include "port/stringTokenizer.h"
#include "port/ioc_sbuf.h"

__OPENCODE_BEGIN_NAMESPACE

//-------------------------------------------------------------------------

const  int IfxSQL::SQL_STMT_UNKNOWN  = 0;
const  int IfxSQL::SQL_STMT_SELECT   = 1;
const  int IfxSQL::SQL_STMT_UPDATE   = 2;
const  int IfxSQL::SQL_STMT_DELETE   = 3;
const  int IfxSQL::SQL_STMT_INSERT   = 4;
const  int IfxSQL::SQL_STMT_CREATE   = 5;
const  int IfxSQL::SQL_STMT_DROP     = 6;
const  int IfxSQL::SQL_STMT_ALTER    = 7;  
const  int IfxSQL::SQL_STMT_BEGIN    = 8;
const  int IfxSQL::SQL_STMT_DECLARE  = 9;


#define IFXSQL_CLASS_NAME  "IfxSQL"

string   IfxSQL::parseInColumns(__XDBC_HOOK_DECLARE_MORE const string& srcSQL) __XDBC_THROW_DECLARE(SQLException)
{
  char caTmp1[50+1];
  char caTmp2[1000+1];
  char caTmp3[1000+1];
  char caTmpColumnName[50+1];
  char caTmpSQL[2000+1];
  char caTmpColumnArray[1000+1];
  char caTmpValueArray[1000+1];
  char caaColumnName[100+1][50+1];
  char caaTmpColumnName[100+1][50+1];
  char caaDateFormatArray[100+1][20+1];
  
  const char* pcSQL=srcSQL.c_str();
  
  char* pcSection;
  char* pcTmp;
  char* pcTmp2;

  int nFoundInterrogation = 0;
  int nFoundToDateMark = 0;
  int nColumn;
  int nToDate;
  int nTmpColumn;
  int i;
  
  strcpy(caTmpSQL,pcSQL);
  
  nColumn = 0;

//------------------------------------------------------------
// 
//------------------------------------------------------------ 

  pcTmp = caTmpSQL;
  
  do
  {
    i = strcspn(pcTmp, "<>");
    if(i && pcTmp[i])
    {
      if(pcTmp[i] == '<') 
      {
        pcTmp[i] = '=';
        
        while(pcTmp[++i] == ' ');
      
        if(pcTmp[i] != '>' )
        {
          pcTmp = pcTmp+i;
        }
        else
        {
          pcTmp[i] = ' ';
          pcTmp = pcTmp+i+1;
        }
      }
      else if( pcTmp[i] == '>')
      {
        pcTmp[i] = '=';
        pcTmp = pcTmp+i+1;
      }
    }
    else
    {
      break;
    }
  }while(true);

//  printf("P1 === [%s]\n",caTmpSQL);

//------------------------------------------------------------
// 
//------------------------------------------------------------   
  sscanf(caTmpSQL,"%s %s %s",caTmp1,caTmp2,caTmp3);
  
  if(StringsKit::equalsIgnoreCase(caTmp1,"SELECT") == 0)
  {
#if defined(OPENCODE_HAS_STRCASESTR)
    if(pcTmp = strcasestr(caTmpSQL,"WHERE"))
#else
    if(pcTmp = strstr(caTmpSQL,"WHERE"))
#endif
    {
      char caTmpSQLWhere[1000+1];
      strcpy(caTmpSQLWhere, pcTmp);
      strcpy(caTmpSQL,caTmpSQLWhere);
    }
  }

//  printf("P2 === [%s]\n",caTmpSQL);

//------------------------------------------------------------
// 
//------------------------------------------------------------    
  bool nFoundColumnName = false;

  pcSection = caTmpSQL;
  
  int   date_column_index = 0;
  bool  __mb_in_to_date = false;
  int   ph2_pos = 0;

#if defined(OPENCODE_HAS_STRCASESTR)
  while(pcSection = strcasestr(pcSection,"TO_DATE"))
#else
  while(pcSection = strstr(pcSection,"TO_DATE"))
#endif
  {
    pcTmp = pcSection;

    ph2_pos = 0;
    __mb_in_to_date = false;

    for( ; *pcTmp != ')'; pcTmp++)
    {  
      if(*pcTmp == '\'') 
      {
        if(!__mb_in_to_date)
        {
          __mb_in_to_date = true;
        }
        else
        {
          __mb_in_to_date = false;

          caaDateFormatArray[date_column_index][ph2_pos] = '\'';
          caaDateFormatArray[date_column_index][ph2_pos+1] = '\0';

          date_column_index++;

        }
      }
    
      if(__mb_in_to_date)
      {
        caaDateFormatArray[date_column_index][ph2_pos] = *pcTmp;
        ph2_pos++;
      }
      *pcTmp = ' ';
    }

    *pcTmp = ' ';
    *pcSection = INFORMIX_TO_DATE_MARK;
    pcSection = pcTmp;
  }
  
  //printf("P3 === [%s]\n",caTmpSQL);
 
//------------------------------------------------------------
// 
//------------------------------------------------------------ 
 
  nColumn = 0;
  nFoundColumnName = 0;
  nFoundToDateMark = 0;
  nFoundInterrogation  = 0;
  i = 0;
  pcTmp = 0;
  pcSection = 0;
   
  if( (StringsKit::equalsIgnoreCase(caTmp1,"UPDATE") == 0) || (StringsKit::equalsIgnoreCase(caTmp1,"DELETE") == 0))
  { 
    pcSection = strtok(caTmpSQL,"=");
    
    nColumn = 0;
    nToDate = 0;
    
    while(pcSection)
    {    
      for(pcTmp = pcSection; *pcTmp == ' '; pcTmp++);
    
      if(*pcTmp == INFORMIX_TO_DATE_MARK)
      {
        nFoundToDateMark = 1;
        if(nFoundColumnName)
        {
          sprintf(caaColumnName[nColumn],"TO_CHAR(%s,%s)",
          caTmpColumnName,caaDateFormatArray[nToDate]);
          nFoundColumnName = 0;
          nFoundToDateMark = 0;
          nColumn++;
          nToDate++;
        }
      }  
    
      if(strchr(pcTmp,'?'))
      {
        nFoundInterrogation = 1;
        if(nFoundColumnName)
        {
          strcpy(caaColumnName[nColumn],caTmpColumnName);
          nFoundInterrogation = 0;
          nColumn++;
        }
      }
      nFoundColumnName = 0;
    
      for(pcTmp = pcSection+strlen(pcSection)-1;;pcTmp--)
      {
        if(*pcTmp != ' ' && *pcTmp != ',')
        {
          if(nFoundColumnName == 0)
          {
            nFoundColumnName = 1; 
          } 
        }
        else
        {
          if(nFoundColumnName == 0 && *pcTmp == ' ')
          {
            *pcTmp = '\0';
          }
          else
          {  
            strcpy(caTmpColumnName,pcTmp+1);
            break;
          }  
        }
      
        if(pcTmp == pcSection)
        {
         break;
        }
      }
      
      pcSection = strtok(0,"=");
    }
  }
  else if(StringsKit::equalsIgnoreCase(caTmp1,"SELECT") == 0)
  {
    pcSection = strtok(caTmpSQL,"=");
  
    nColumn = 0;
    nToDate = 0;
  
    while(pcSection)
    {
      for(pcTmp = pcSection; *pcTmp == ' '; pcTmp++);
    
      if(*pcTmp == INFORMIX_TO_DATE_MARK)
      {
        nFoundToDateMark = 1;
        if(nFoundColumnName)
        {
          sprintf(caaColumnName[nColumn],"TO_CHAR(%s,%s)",
          caTmpColumnName,caaDateFormatArray[nToDate]);
          nFoundColumnName = 0;
          nFoundToDateMark = 0;
          nColumn++;
          nToDate++;
        }
      }
    
      if(strchr(pcTmp,'?'))
      {
        nFoundInterrogation = 1;
        if(nFoundColumnName)
        {
          strcpy(caaColumnName[nColumn],caTmpColumnName);
          nFoundInterrogation = 0;
          nColumn++;
        }
      }
      nFoundColumnName = 0;
    
      for(pcTmp = pcSection+strlen(pcSection)-1;;pcTmp--)
      {
        if(*pcTmp != ' ' && *pcTmp != ',')
        {
          if(nFoundColumnName == 0)
          {
            nFoundColumnName = 1; 
          } 
        }
        else
        {
          if(nFoundColumnName == 0 && *pcTmp == ' ')
          {
            *pcTmp = '\0';
          }
          else
          {
            strcpy(caTmpColumnName,pcTmp+1);
            break;
          }
        }
    
        if(pcTmp == pcSection)
        {
          break;
        }
      }
    
      pcSection = strtok(0,"="); 
    }
  }
  
//------------------------------------------------------------
// 
//------------------------------------------------------------  
  string resultSQL;
  
  if(nColumn >0)
  {
    resultSQL.append(caaColumnName[0]); 
    for(i=1;i<nColumn;i++)
    {
      resultSQL.append(",");
      resultSQL.append(caaColumnName[i]);
    }
  }
  
  return resultSQL;  
}

//--------------------------------------------------------------------------------------

string   IfxSQL::parseTableName(__XDBC_HOOK_DECLARE_MORE const string& srcSQL) __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    __sb.append("parseTableName|ISQL:");
    __sb.append(srcSQL);

    __XDBC_FORCED_TRACE(IFXSQL_CLASS_NAME,__sb.str());
  }

  char caTmp1[50+1];
  char caTmp2[1000+1];
  char caTmp3[1000+1];

  char caTmpSQL[2000+1];

  char* pcTmp;
  char* pcTmp2;

  strcpy(caTmpSQL,srcSQL.c_str());

  sscanf(caTmpSQL,"%s %s %s",caTmp1,caTmp2,caTmp3);

  string table_name;

  if(StringsKit::equalsIgnoreCase(caTmp1,"SELECT") == 0)
  {

#if defined(OPENCODE_HAS_STRCASESTR)
    if(pcTmp = strcasestr(caTmpSQL,"WHERE"))
#else
    if(pcTmp = strstr(caTmpSQL,"WHERE"))
#endif   
    {
      *pcTmp = 0;
    }

#if defined(OPENCODE_HAS_STRCASESTR)
    if(pcTmp = strcasestr(caTmpSQL,"FROM"))
#else
    if(pcTmp = strstr(caTmpSQL,"FROM"))
#endif    
    {
      table_name = pcTmp;
      *pcTmp = 0;
    }
  }
  else
  {
    if(StringsKit::equalsIgnoreCase(caTmp1,"UPDATE") == 0)
    {
      table_name = caTmp2;
    }
    else if(StringsKit::equalsIgnoreCase(caTmp1,"DELETE") == 0)
    {
      if(StringsKit::equalsIgnoreCase(caTmp2,"FROM") == 0)
      {
        table_name = caTmp3;
      }
      else
      {
        table_name = caTmp2;
      }
    }
    else if(StringsKit::equalsIgnoreCase(caTmp1,"INSERT") == 0)
    {
      if(pcTmp = strchr(caTmp3,'('))
      {
        *pcTmp = '\0';
      }
      table_name = caTmp3;
    }
  }
 
  if(!table_name.empty())
  {
    size_t  size_temp = table_name.length();
    char* p_temp = new char[size_temp+1];
    memset(p_temp,0,size_temp+1);

    strncpy(p_temp,table_name.c_str(),size_temp); 
    
    char*  p_tab = p_temp;
    for(;*p_tab == ' ';p_tab++);

    if(StringsKit::equalsIgnoreCase(p_tab,"FROM",4) == 0 )
    {
      p_tab = p_tab+4;
    }

    for(;*p_tab == ' ';p_tab++);
    for(char* p_end = p_temp + size_temp -1;*p_end == ' ';*p_end='\0',p_end--);
 
    table_name = p_tab;
    delete []p_temp;
  }

  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    __sb.append("parseTableName|");
    __sb.append(table_name);
    
    __XDBC_FORCED_TRACE(IFXSQL_CLASS_NAME,__sb.str());
  }

  return table_name;
}

void  IfxSQL::mapColumns(__XDBC_HOOK_DECLARE_MORE const string& column_list,OPENCODE_MAP<string,string>& __map_columns) __XDBC_THROW_DECLARE(SQLException)
{
  StringTokenizer column_tokens(column_list,",");
  while(column_tokens.hasMoreTokens())
  {
    string comp_col = column_tokens.nextToken();
    StringTokenizer col_tokens(comp_col,".");
    
    if(col_tokens.countTokens()<=1)
    {
      __map_columns[comp_col] = string("");
    }
    else
    {
      string tab_name = col_tokens.nextToken();
      string col_name = col_tokens.nextToken();
      __map_columns[col_name] = tab_name;

      //printf("%s - %s\n",col_name.c_str(),tab_name.c_str());
    }
  }
}

void  IfxSQL::mapTables(__XDBC_HOOK_DECLARE_MORE const string& table_name,OPENCODE_MAP<string,string>& map_tables) __XDBC_THROW_DECLARE(SQLException)
{
  StringTokenizer table_tokens(table_name,",");
  while(table_tokens.hasMoreTokens())
  {
    StringTokenizer alias_tokens(table_tokens.nextToken()," ");
    if(alias_tokens.countTokens() == 2 )
    {
      string tab_name = alias_tokens.nextToken();
      string alias_name = alias_tokens.nextToken();
      map_tables[alias_name] = tab_name;

      //printf("%s - %s\n",alias_name.c_str(),tab_name.c_str());
    }

   }//~end while(table_tokens.hasMoreTokens())
}

//--------------------------------------------------------------------------------------

string   IfxSQL::parseSQL(__XDBC_HOOK_DECLARE_MORE const string& ifx_sql,int& param_count) __XDBC_THROW_DECLARE(SQLException)
{
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    __sb.append("parseSQL|ISQL:");
    __sb.append(ifx_sql);
    
    __XDBC_FORCED_TRACE(IFXSQL_CLASS_NAME,__sb.str());
  }
  
  IfxSQL ifxSQL;
  string s = ifxSQL.parse(__XDBC_HOOK_INVOKE_MORE ifx_sql,param_count);
  __XDBC_HOOK_CHECK(return string());
  
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    __sb.append("parseSQL|OSQL:");
    __sb.append(s);
    
    __XDBC_FORCED_TRACE(IFXSQL_CLASS_NAME,__sb.str());  
  }
  return  s;
}

int  IfxSQL::parseSQLKind(const string&ifx_sql)
{
  int stmt_kind=SQL_STMT_SELECT;

  for(StringTokenizer tokens(ifx_sql,"\n \r \t /", true);tokens.hasMoreTokens();)
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
    else if(StringsKit::equalsIgnoreCase("begin",token.c_str())==0 || StringsKit::equalsIgnoreCase("call",token.c_str())==0 || StringsKit::equalsIgnoreCase("execute",token.c_str())==0)
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
    __sb.append("parseStmtType|return stmt_type {");
    __sb.append(stmt_kind);
    __sb.append("}");
    
    __XDBC_FORCED_TRACE(IFXSQL_CLASS_NAME,__sb.str());
  }

  return stmt_kind;
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

string  IfxSQL::parse(__XDBC_HOOK_DECLARE_MORE const string src_sql,int& param_count)  __XDBC_THROW_DECLARE(SQLException)
{
  current_argument = 1;
  i = 0;
  first = true;
  in_string = false;
  this->odbc_sql = src_sql;
  length = this->odbc_sql.length();

  handleODBC(__XDBC_HOOK_INVOKE_ONCE);
  __XDBC_HOOK_CHECK(return string());
  
  if(i < length)
  {
    StringBuffer __sb;
    __sb.append("IfxSQL::parse  at position {");
    __sb.append(i);
    __sb.append("}");
    __XDBC_HOOK_RETURN(return string(),__sb.str()) ;
  }
  
  param_count=current_argument-1; 
  return this->ifx_sql;
}

void    IfxSQL::appendChar(string& stringbuffer,const char& c1)
{
  if(c1 == '?')
  {
    stringbuffer+=nextArgument();
  }
  else
  {
    stringbuffer+=c1;  
  }
}

void    IfxSQL::handleCall(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{
  bool flag = first;
  if(flag)
  {
   ifx_sql.append("EXECUTE PROCEDURE ");
  }

  skipSpace();
  handleODBC(__XDBC_HOOK_INVOKE_ONCE);
  __XDBC_HOOK_CHECK(return);
  skipSpace();
 
 if(flag)
  {
   ifx_sql.append(" ");
  }
}

void    IfxSQL::handleDate(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{
 ifx_sql.append("TO_DATE (");
  skipSpace();
  handleODBC(__XDBC_HOOK_INVOKE_ONCE);
 ifx_sql.append(", 'YYYY-MM-DD')");
}

void    IfxSQL::handleEscape(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{
  ifx_sql.append("ESCAPE ");
  skipSpace();
  handleODBC(__XDBC_HOOK_INVOKE_ONCE);
}

void    IfxSQL::handleFunction(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{
  bool flag = first;
  if(flag)
  {
   ifx_sql.append("BEGIN ");
  }
  appendChar(ifx_sql, '?');
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
  ifx_sql.append(" := ");
  skipSpace();
  handleODBC(__XDBC_HOOK_INVOKE_ONCE);
  __XDBC_HOOK_CHECK(return);

  if(flag)
  {
   ifx_sql.append("; END;");
  }
}

void    IfxSQL::handleODBC(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{
  while(i < length) 
  {
    c = odbc_sql.at(i);

    if(in_string)
    {
     ifx_sql+=c;
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
         ifx_sql+=c;
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
            token_buffer+=c;
          }
          handleToken(__XDBC_HOOK_INVOKE_MORE token_buffer);
          __XDBC_HOOK_CHECK(return);
          c = odbc_sql.at(i);
          if(c != '}')
          {
            StringBuffer __sb;
            __sb.append(i);
            __sb.append(". Expecting \"=\" got \"");
            __sb.append(c);
            __sb.append("\"");

            __XDBC_HOOK_RETURN(return,__sb.str());
          }
          i++;
          break;
        
        case 125: // '}'
          return;
        
        default:
          appendChar(ifx_sql, c);
          i++;
          first = false;
          break;

      }//~end switch

    }//~end if

  }//~end while
}

void    IfxSQL::handleTime(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{
 ifx_sql.append("TO_DATE (");
  skipSpace();
  handleODBC(__XDBC_HOOK_INVOKE_ONCE);
  __XDBC_HOOK_CHECK(return);
 ifx_sql.append(", 'HH24:MI:SS')");
}

void    IfxSQL::handleTimestamp(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{
 ifx_sql.append("TO_DATE (");
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
     ifx_sql.append(""+c);
    }
  }
  
  if(flag)
  {
   ifx_sql.append(""+'\'');
  }
 ifx_sql.append(", 'YYYY-MM-DD HH24:MI:SS')");
}

void    IfxSQL::handleToken(__XDBC_HOOK_DECLARE_MORE const string& s) __XDBC_THROW_DECLARE(SQLException)
{
  if(StringsKit::equalsIgnoreCase("?",s.c_str())==0)
  {
    handleFunction(__XDBC_HOOK_INVOKE_ONCE);
    __XDBC_HOOK_CHECK(return);
  }
  else if(StringsKit::equalsIgnoreCase("call",s.c_str())==0 || StringsKit::equalsIgnoreCase("execute",s.c_str())==0)
  {
    handleCall(__XDBC_HOOK_INVOKE_ONCE);
    __XDBC_HOOK_CHECK(return);
  }
  else if(StringsKit::equalsIgnoreCase("ts",s.c_str())==0)
  {
    handleTimestamp(__XDBC_HOOK_INVOKE_ONCE);
    __XDBC_HOOK_CHECK(return);
  }
  else if(StringsKit::equalsIgnoreCase("t",s.c_str())==0)
  {
    handleTime(__XDBC_HOOK_INVOKE_ONCE);
    __XDBC_HOOK_CHECK(return);
  }
  else if(StringsKit::equalsIgnoreCase("d",s.c_str())==0)
  {
    handleDate(__XDBC_HOOK_INVOKE_ONCE);
    __XDBC_HOOK_CHECK(return);
  }
  else if(StringsKit::equalsIgnoreCase("escape",s.c_str())==0)
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

string  IfxSQL::nextArgument()
{
  string s = "?" ;
//  s=s+current_argument;
  current_argument++;
  return s;
}

void    IfxSQL::skipSpace()
{
  for(; i < length && (c = odbc_sql.at(i)) == ' '; i++);
}

__OPENCODE_END_NAMESPACE
