#include "xdbc_demo.h"

//------------------------------------------------------------------
//demo function definition
//------------------------------------------------------------------
void xdbc_pstmt_sel_demo(Connection* __p_conn) __OPENCODE_THROW_DECLARE(SQLException)
{
  __XDBC_HOOK_DEFINE();
  
//  PreparedStatement* __p_pstmt=__p_conn->prepareStatement(__XDBC_HOOK_INVOKE_MORE "select * from XDBC_DEMO where int_col=?");

  PreparedStatement* __p_pstmt=__p_conn->prepareStatement(__XDBC_HOOK_INVOKE_MORE "SELECT DAT_COL,TS_COL,INT_COL,INT8_COL,FLT_COL,DBL_COL,NC_COL,VC_COL,cbl_col,blb_clob,txt_col from XDBC_DEMO where int_col=?");

  xdbc_smart_ptr<PreparedStatement> __sp_stmt(__p_pstmt);
  __p_pstmt->setInt(__XDBC_HOOK_INVOKE_MORE 1,1);
  __XDBC_HOOK_CHECK(return);
  
  ResultSet* __p_rs=__p_pstmt->executeQuery(__XDBC_HOOK_INVOKE_ONCE);
  xdbc_smart_ptr<ResultSet> __s__p_rs(__p_rs);
  __XDBC_HOOK_CHECK(return);
    
  dumpResultSet(__XDBC_HOOK_INVOKE_MORE __p_rs);
}

void xdbc_pstmt_upd_demo(Connection* __p_conn) __OPENCODE_THROW_DECLARE(SQLException)
{
  __XDBC_HOOK_DEFINE();
  
  PreparedStatement* __p_pstmt=__p_conn->prepareStatement(__XDBC_HOOK_INVOKE_MORE "insert into XDBC_DEMO(vc_col,int_col) values(?,?)");
  xdbc_smart_ptr<PreparedStatement> __sp_stmt(__p_pstmt);
  __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);
    
  string str = "2006-01-03 00:00:00";
  __p_pstmt->setString(__XDBC_HOOK_INVOKE_MORE 1,str);
  __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);
  
  __p_pstmt->setInt(__XDBC_HOOK_INVOKE_MORE 2,10);
  __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);

  __p_pstmt->executeUpdate(__XDBC_HOOK_INVOKE_ONCE);
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
 
    xdbc_pstmt_sel_demo(__p_conn);
//    xdbc_pstmt_upd_demo(__p_conn);
  }
  __OPENCODE_CATCH(SQLException& ex)
  {
    cout << "[" << ex.getSQLState() << "/" << ex.getSQLCode() << "]" << " - " << ex.getMessage() << endl;
  }
      
  return 0;
}
