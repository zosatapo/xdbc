#include "xdbc_demo.h" 
#include "xdbc/SQLSource.h" 
#include "xdbc/basicDataSource.h"
#include "xdbc/pooledPreparedStatement.h"
#include "xdbc/pooledConnection.h"

#include "port/ioc_smart.h"
#include "port/ioc_sbuf.h"
#include "port/libsys.h"
#include "port/profile.h" 
#include "xdbc/xsmart.h"
#include "xdbc/xdbc.h" 
#include "port/file.h"

#include <iostream>

USING_OPENCODE_STL_NS
USING_OPENCODE_NS

//------------------------------------------------------------------
//demo function definition
//------------------------------------------------------------------

void DataSource_demo(const string& dbname)
{
  string __dbname = dbname;
  if(__dbname.empty())
  {
    __dbname = System::getProperty("xdbc.dbname");   
  }

  if(__dbname.empty())
  {
    cout << ("no dbname provided") << endl;   
    return;
  }
  
  __XDBC_ATTRIBUTES __xdbc_attributes;
  if(! DriverManager::findDatabaseAttributes(__dbname,"xdbc.ini","XDBC_CONFIG","XDBC.CONFIG",__xdbc_attributes))
  {
    cout << "no xdbc.ini provided for db_name '" << __dbname << "'" << endl;   
    return;    
  }    


  BasicDataSource*  __p_datasource = BasicDataSource::createDataSource(__xdbc_attributes);
  smart_ptr<BasicDataSource> __sp_datasource(__p_datasource); 
  
  __XDBC_HOOK_DEFINE();
  
  __OPENCODE_TRY
  {  
    __sp_datasource->start(__XDBC_HOOK_INVOKE_ONCE);
    __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);

    { 
      Connection* __p_xconn = __sp_datasource->getConnection(__XDBC_HOOK_INVOKE_ONCE);      
      xdbc_smart_ptr<Connection> __sp_xconn(__p_xconn);
      __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);
      
      const SQLData& sqlData = SQLSourceImpl::getInstance().getSQLData("xdbc.select.xdbc_demo");
      PreparedStatement* __p_pstmt = __p_xconn->prepareStatement(__XDBC_HOOK_INVOKE_MORE sqlData.SQL,sqlData.paramCount);
      xdbc_smart_ptr<PreparedStatement> __sp_pstmt(__p_pstmt);
      __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);
      
      xdbc_smart_ptr<ResultSet> __sp_rs(__p_pstmt->executeQuery(__XDBC_HOOK_INVOKE_ONCE));
      __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);
    }

    __sp_datasource->stop(__XDBC_HOOK_INVOKE_ONCE);
    __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);
    
  }
  __OPENCODE_CATCH(SQLException& ex)
  {
    cout << "DataSource_demo : " << ex.getMessage() << endl;
  }
}

void pooled_XXX_demo(const string& dbname)
{
  string __dbname = dbname;
  if(__dbname.empty())
  {
    __dbname = System::getProperty("xdbc.dbname");   
  }

  if(__dbname.empty())
  {
    cout << ("no dbname provided") << endl;   
    return;
  }

  __XDBC_HOOK_DEFINE();
          
  Connection* __p_conn = getConnection(__XDBC_HOOK_INVOKE_MORE __dbname);
  xdbc_smart_ptr<Connection> __sp_conn(__p_conn); 
  __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);
  
  PooledConnection*  __p_pooled_conn = new PooledConnection(0,__sp_conn.release()); 
  xdbc_smart_ptr<Connection> __sp_pooled_conn(__p_pooled_conn);    
  __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);
  
  SQLData __sqlData("select * from XDBC_DEMO where int_col = ?");
  string __SQL_key("xdbc.select.xdbc_demo");
  
  PreparedStatement* __p_pstmt = __p_conn->prepareStatement(__XDBC_HOOK_INVOKE_MORE __sqlData.SQL);
  __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);
  __p_pstmt->setAttribute(XDBC_SQLVALUE_KEY,__sqlData.SQL);        
  
  PooledPreparedStatement* __p_pooled_pstmt = new PooledPreparedStatement(__p_pstmt);
  __p_pooled_conn->insertPooledPreparedStatement(__SQL_key,__sqlData.SQLID,__p_pooled_pstmt);

}
  
//------------------------------------------------------------------
//demo entry point 
//------------------------------------------------------------------
int main(int argc,char** argv)
{
  System::initializeEnvironment(argc,argv);

  string dbname  = System::getProperty("xdbc.dbname");
  DataSource_demo(dbname);
  //pooled_XXX_demo(dbname);
  
  return 0;
}

