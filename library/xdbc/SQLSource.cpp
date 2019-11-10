#include "xdbc/SQLSource.h"
#include "xdbc/statement.h"
#include "xdbc/driverManager.h" 

#include "port/utilities.h"
#include "port/logging.h"
#include "port/ioc_sbuf.h"
#include "port/stringTokenizer.h"
#include "port/libnumber.h"
#include "port/properties.h"
#include "port/libsys.h"
#include "port/file.h"
#include "port/booleanComponent.h"

__OPENCODE_BEGIN_NAMESPACE


/////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////

SQLData::SQLData()
{
  paramCount = SQLImplementation::paramCountUnknown;
  pooled = false;
  this->SQLID = SQLDATA_SQLID_NULL;
}

SQLData::SQLData(const string& xSQL)
{
  this->raw_SQL = xSQL;
  this->SQL = xSQL; 
  this->SQLID = SQLDATA_SQLID_NULL;
  
  paramCount = SQLImplementation::paramCountUnknown;
  pooled = false;
}

SQLData::SQLData(const SQLData& rv)
{
  this->raw_SQL = rv.raw_SQL;
  this->SQL = rv.SQL;
  this->SQLID = rv.SQLID;
  
  this->paramCount = rv.paramCount;
  this->pooled = rv.pooled;
  this->dbname = rv.dbname; 
}  

SQLData&  SQLData::operator=(const SQLData& rv)
{
  if(&rv == this)
  {
    return *this;
  }
  
  this->raw_SQL = rv.raw_SQL;
  this->SQL = rv.SQL;
  this->SQLID = rv.SQLID;
     
  this->paramCount = rv.paramCount;
  this->pooled = rv.pooled;
  this->dbname = rv.dbname; 
  
  return *this;  
}

bool SQLData::isNull() const
{
  return this->raw_SQL.empty();
}

string SQLData::toString() const
{
  StringBuffer __sb_params;
    
  if(this->paramCount != SQLImplementation::paramCountUnknown)
  {
    __sb_params << "paramCount=" << this->paramCount;
  }
  
  if(pooled)
  {
    if(!__sb_params.empty())
    {
      __sb_params << ";";
    }
    
    __sb_params << "pooled=true";
  }

  if(SQLID != SQLDATA_SQLID_NULL)
  {
    if(!__sb_params.empty())
    {
      __sb_params << ";";
    }
    
    __sb_params << "SQLID=" << this->SQLID;
  }
  
  if(!dbname.empty())
  {
    if(!__sb_params.empty())
    {
      __sb_params << ";";
    }
    
    __sb_params << "dbname=" << this->dbname ;
  }
      
  StringBuffer __sb;  
  __sb << this->raw_SQL;
  
  if(!__sb_params.empty())
  {
    __sb << "|" <<  __sb_params ;
  }
  
  return __sb.str();
}

/////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_LIFETIME_BEGIN(SQLSource)
IMPLEMENT_LIFETIME_END(SQLSource)

const SQLData& SQLSource::getSQLData(const string& key) const
{
  OPENCODE_MAP<string,SQLData>::const_iterator __pos =  __SQL_map.find(key); 
  if(__pos != __SQL_map.end())
  {
    return __pos->second;
  }
  
  return __SQLData_NULL;
}

const SQLData& SQLSource::getSQLData(int SQLID) const
{
  OPENCODE_MAP<int,SQLData>::const_iterator __pos =  __SQLID_map.find(SQLID); 
  if(__pos != __SQLID_map.end())
  {
    return __pos->second;
  }
  
  return __SQLData_NULL;
}
 
void  SQLSource::keys(vector<string>& sqlkeys) const
{
  OPENCODE_MAP<string,SQLData>::const_iterator __pos =  __SQL_map.begin(); 
  OPENCODE_MAP<string,SQLData>::const_iterator __end =  __SQL_map.end();
  
  while(__pos != __end)
  {
    sqlkeys.push_back(__pos->first); 
    ++__pos;
  }

}

//////////////////////////////////////////////////////////////////////////////////////

SQLSource::~SQLSource()
{
}

SQLSource::SQLSource()
{
  __XDBC_ATTRIBUTES __xdbc_attributes;
  if(! DriverManager::findDatabaseAttributes("SQLS","xdbc.sqls.ini","SQLS_CONFIG","SQLS.CONFIG",__xdbc_attributes))
  {
    LOGGING_WARN("SQLSource::<init>|DBSQLS configuration not found"); 
    return;    
  }
  
  loadSource(__xdbc_attributes);
}

void SQLSource::loadSource(const __XDBC_ATTRIBUTES& props)
{
  __XDBC_ATTRIBUTES::const_iterator __pos = props.begin();
  __XDBC_ATTRIBUTES::const_iterator __end = props.end();
 
  while(__pos != __end)
  {
    parseSQLData(__pos->first,__pos->second);
    ++__pos;
  }
}

void SQLSource::parseSQLData(const string& key,const string& sql)
{
  if(sql.empty())
  {
    LOGGING_WARN("SQLSource::parseSQLData|invalid SQL definition : [" << key << "=" << sql << "]");
    return;   
  }
  
  SQLData __SQLData;
  __SQLData.paramCount =  SQLImplementation::paramCountUnknown;   
  
  vector<string> __SQLs = StringTokenizer::parseTokens(StringsKit::trim(sql),"|");
  __SQLData.raw_SQL = StringsKit::trim(__SQLs[0]);  
  if(__SQLs.size() > 1)
  {
    vector<string> __param_SQLs = StringTokenizer::parseTokens(StringsKit::trim(__SQLs[1]),";");
    int __param_SQLs_size = (int)__param_SQLs.size();
    for(int k = 0; k < __param_SQLs_size; ++k)
    {
      vector<string> __pair_params = StringTokenizer::parseTokens(StringsKit::trim(__param_SQLs[k]),"=");
      if(__pair_params.size() == 2)
      {
        if(StringsKit::equalsIgnoreCase(__pair_params[0],"dbname"))
        {
          __SQLData.dbname = StringsKit::trim(__pair_params[1]);  
        }
        else if(StringsKit::equalsIgnoreCase(__pair_params[0],"paramCount"))
        {
          __OPENCODE_TRY
          {
            __SQLData.paramCount = Number::parseInt(StringsKit::trim(__pair_params[1]));
          }
          __OPENCODE_CATCH(NumberFormatException& ex)
          {
            __SQLData.paramCount = SQLImplementation::paramCountUnknown; 
          }         
        } 
        else if(StringsKit::equalsIgnoreCase(__pair_params[0],"pooled"))
        {
           __SQLData.pooled = BooleanComponent::valueOf(StringsKit::trim(__pair_params[1]));       
        } 
        else if(StringsKit::equalsIgnoreCase(__pair_params[0],"SQLID"))
        {
          __OPENCODE_TRY
          {
            __SQLData.SQLID =  Number::parseInt(StringsKit::trim(__pair_params[1]));
          }
          __OPENCODE_CATCH(NumberFormatException& ex)
          {
            __SQLData.SQLID = SQLDATA_SQLID_NULL; 
          }      
        } 
        else
        {
          LOGGING_WARN("SQLSource::parseSQLData|unknown SQL param definition : [" << __param_SQLs[k] << "]");
        }     
      }  
      else
      {
        LOGGING_WARN("SQLSource::parseSQLData|invalid SQL param definition : [" << __param_SQLs[k] << "]");
      }  
      
    }//~end for(int k = 0; k < __param_SQLs_size; ++k)
      
  }//~end if(__SQLs.size() == 1)
  
  OPENCODE_MAP<string,SQLData>::iterator __pos = __SQL_map.find(key);
  if(__pos != __SQL_map.end())
  {

    if(__pos->second.raw_SQL != __SQLData.raw_SQL)
    {
      StringBuffer __sb;
      __sb << "SQLSource::parseSQLData|duplicated SQLKEY definition [" << key << "] , overwrite : " << File::lineSeparator
           << "[OLD] - " <<  __pos->second.toString() << File::lineSeparator
           << "[NEW] - " <<  __SQLData.toString();
      LOGGING_WARN(__sb.str());
    }
  }
  __SQL_map[key] = __SQLData;
 
  if(__SQLData.SQLID != SQLDATA_SQLID_NULL)
  { 
    OPENCODE_MAP<int,SQLData>::iterator __pos_SQLID = __SQLID_map.find(__SQLData.SQLID);
    if(__pos_SQLID != __SQLID_map.end())
    {
      StringBuffer __sb;
      __sb << "SQLSource::parseSQLData|duplicated SQLID definition [" << __SQLData.SQLID << "] , overwrite : " << File::lineSeparator
           << "[OLD] - " <<  __pos_SQLID->second.toString() << File::lineSeparator
           << "[NEW] - " <<  __SQLData.toString();
      LOGGING_WARN(__sb.str());
    }
  
    __SQLID_map[__SQLData.SQLID] = __SQLData;
  }
} 

__OPENCODE_END_NAMESPACE
