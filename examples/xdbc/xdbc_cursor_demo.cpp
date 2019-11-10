#include "xdbc_demo.h"
#include "port/ioc_smart.h"

//------------------------------------------------------------------
//demo function definition
//------------------------------------------------------------------

void xdbc_cursor_demo(Connection* __p_conn) __OPENCODE_THROW_DECLARE(SQLException)
{
  __XDBC_HOOK_DEFINE();
  
  Statement* __p_stmt=__p_conn->createStatement(__XDBC_HOOK_INVOKE_MORE  ResultSet::TYPE_SCROLL_INSENSITIVE,ResultSet::CONCUR_READ_ONLY);
  xdbc_smart_ptr<Statement> __sp_stmt(__p_stmt);
  __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);
  
  ResultSet* __p_rs=__p_stmt->executeQuery(__XDBC_HOOK_INVOKE_MORE  "select * from XDBC_DEMO");
  xdbc_smart_ptr<ResultSet> __sp_rs(__p_rs);
  __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);
  
  smart_ptr<ResultSetMetaData> __sp_meta(__p_rs->getMetaData(__XDBC_HOOK_INVOKE_ONCE));
  __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);
  
  if(__p_rs->absolute(__XDBC_HOOK_INVOKE_MORE 2))
  {
    dumpRow(__XDBC_HOOK_INVOKE_MORE __p_rs,__sp_meta.get());
  }
  __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);
  
  if(__p_rs->previous(__XDBC_HOOK_INVOKE_ONCE))
  {
    dumpRow(__XDBC_HOOK_INVOKE_MORE __p_rs,__sp_meta.get());
  }
  __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);
   
  cout<<"-------------------------------"<<endl; 
  __p_rs->last(__XDBC_HOOK_INVOKE_ONCE);
  __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);
  
  int __last_row = __p_rs->getRow(__XDBC_HOOK_INVOKE_ONCE);
  __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);
  cout<<"--row count--["<< __last_row <<"]"<<endl;
}

//------------------------------------------------------------------
//demo entry point 
//------------------------------------------------------------------
int main(int argc,char** argv)
{
  System::initializeEnvironment(argc,argv);
  string dsname;
  
  __XDBC_HOOK_DEFINE();
     
  __OPENCODE_TRY
  {
    Connection* __p_conn = getConnection(__XDBC_HOOK_INVOKE_MORE dsname);
    xdbc_smart_ptr<Connection> __sp_conn(__p_conn);
    __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return 0);
 
    xdbc_cursor_demo(__p_conn);
  }
  __OPENCODE_CATCH(SQLException& ex)
  {
    cout << "[" << ex.getSQLState() << "/" << ex.getSQLCode() << "]" << " - " << ex.getMessage() << endl;
  }
      
  return 0;
}
