#include "xdbc_demo.h"
#include "port/booleanComponent.h"

//------------------------------------------------------------------
//demo function definition
//------------------------------------------------------------------
void xdbc_stmt_sel_demo(Connection* __p_conn) __OPENCODE_THROW_DECLARE(SQLException)
{
  __XDBC_HOOK_DEFINE();
  
  Statement*  __p_stmt = __p_conn->createStatement(__XDBC_HOOK_INVOKE_ONCE);
  xdbc_smart_ptr<Statement> __sp_stmt(__p_stmt);
  __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);
    
  ResultSet* __p_rs = __p_stmt->executeQuery(__XDBC_HOOK_INVOKE_MORE "SELECT DAT_COL,TS_COL,INT_COL,INT8_COL,FLT_COL,DBL_COL,NC_COL,VC_COL,cbl_col,blb_clob,txt_col FROM XDBC_DEMO");

//    ResultSet* __p_rs = __p_stmt->executeQuery(__XDBC_HOOK_INVOKE_MORE "SELECT VC_COL,INT_COL FROM XDBC_DEMO");
 
 
  xdbc_smart_ptr<ResultSet> __sp_rs(__p_rs);
  __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);
    
  dumpMetaData(__XDBC_HOOK_INVOKE_MORE __p_rs);
  dumpResultSet(__XDBC_HOOK_INVOKE_MORE __p_rs);
}

void xdbc_stmt_upd_demo(Connection* __p_conn) __OPENCODE_THROW_DECLARE(SQLException)
{
  __XDBC_HOOK_DEFINE();
  
  Statement* __p_stmt=__p_conn->createStatement(__XDBC_HOOK_INVOKE_ONCE);
  xdbc_smart_ptr<Statement> __sp_stmt(__p_stmt);
  __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);
    
  __p_stmt->executeUpdate(__XDBC_HOOK_INVOKE_MORE "insert into XDBC_DEMO(vc_col) values('2006-01-03 00:00:00')");
  __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);
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
 
    xdbc_stmt_sel_demo(__p_conn);
  //  xdbc_stmt_upd_demo(__p_conn);
  }
  __OPENCODE_CATCH(SQLException& ex)
  {
    cout << "[" << ex.getSQLState() << "/" << ex.getSQLCode() << "]" << " - " << ex.getMessage() << endl;
  }
      
  return 0;
}
