#include "xdbc_demo.h"
#include "port/file.h"

USING_OPENCODE_STL_NS
USING_OPENCODE_NS

//------------------------------------------------------------------
//demo function definition
//------------------------------------------------------------------
void xdbc_stmt_sel_demo(Connection* __p_conn) __OPENCODE_THROW_DECLARE(SQLException)
{
  __XDBC_HOOK_DEFINE();
  
  Statement*  __p_stmt = __p_conn->createStatement(__XDBC_HOOK_INVOKE_ONCE);
  xdbc_smart_ptr<Statement> __sp_stmt(__p_stmt);
  __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);
    
  ResultSet* __p_rs = __p_stmt->executeQuery(__XDBC_HOOK_INVOKE_MORE "SELECT  DAT_COL,TS_COL,INT_COL,INT8_COL,FLT_COL,DBL_COL,NC_COL,VC_COL,cbl_col,blb_clob,txt_col FROM XDBC_DEMO");
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

void xdbc_pstmt_upd_demo(Connection* __p_conn) __OPENCODE_THROW_DECLARE(SQLException)
{
  __XDBC_HOOK_DEFINE();
  
  PreparedStatement* __p_pstmt=__p_conn->prepareStatement(__XDBC_HOOK_INVOKE_MORE "insert into XDBC_DEMO(vc_col,int_col) values(?,?)");
  xdbc_smart_ptr<PreparedStatement> __sp_stmt(__p_pstmt);
  __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);

  string str = "ÖÐÎÄ²âÊÔ";
  __p_pstmt->setString(__XDBC_HOOK_INVOKE_MORE 1,str);
  __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);

  __p_pstmt->setInt(__XDBC_HOOK_INVOKE_MORE 2,10);
  __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);

  __p_pstmt->executeUpdate(__XDBC_HOOK_INVOKE_ONCE);
  __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);
}

void xdbc_main()
{
  string __dsn_name("ORACLE2"); 

  __OPENCODE_TRY
  {
    Connection* __p_conn = getConnection( __dsn_name);
    xdbc_smart_ptr<Connection> __sp_conn(__p_conn);
    
    //xdbc_stmt_upd_demo(__p_conn);
    xdbc_pstmt_upd_demo(__p_conn);
    xdbc_stmt_sel_demo(__p_conn);
  }
  __OPENCODE_CATCH(SQLException& ex)
  {
    cout << "[" << ex.getSQLState() << "/" << ex.getSQLCode() << "]" << " - " << ex.getMessage() << endl;
  }
}

//------------------------------------------------------------------
//demo entry point 
//------------------------------------------------------------------
int main(int argc,char** argv)
{
  System::initializeEnvironment(argc,argv);
  System::setMultiThreading(false);

  Logging::setInternalDebugging(true);

  File __curDir(System::getProperty("application.home"));
  File __examplesDir(__curDir.getParentFile().getParentFile(),"examples");

  System::setProperty("XDBC.CONFIG",__examplesDir.getAbsolutePath() + string("\\xdbc.ini"));
  System::setProperty("XDBC_LIBRARY_PATH",__curDir.getAbsolutePath());
  System::setProperty("ODBC.CONFIG",__examplesDir.getAbsolutePath() + string("\\odbc.ini"));
  System::setProperty("xdbc.isTracingOn","TRUE");
  xdbc_main();

  LifeTimeGlobals::__lifetime_destroy();
  System::waitAnyKey();
  return 0;
}