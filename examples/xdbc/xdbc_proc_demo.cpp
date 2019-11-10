#include "xdbc_demo.h"

//------------------------------------------------------------------
//demo function definition
//------------------------------------------------------------------

void xdbc_proc_demo(Connection* __p_conn) __OPENCODE_THROW_DECLARE(SQLException)
{
  __XDBC_HOOK_DEFINE();

  CallableStatement* __p_pstmt=__p_conn->prepareCall(__XDBC_HOOK_INVOKE_MORE "call proc_demo (?,?)");
  xdbc_smart_ptr<CallableStatement> __sp_stmt(__p_pstmt);
  __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);

  __p_pstmt->setString(__XDBC_HOOK_INVOKE_MORE 1,"1000");
  __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);
  __p_pstmt->registerOutParameter(__XDBC_HOOK_INVOKE_MORE 2,Types::VARCHAR);
  __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);
  
  __p_pstmt->executeUpdate(__XDBC_HOOK_INVOKE_ONCE);
  __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);

  int __sizeBytes = 0;
  const char*  __p_str_ret=__p_pstmt->getString(__XDBC_HOOK_INVOKE_MORE 2,__sizeBytes);
  __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);

  cout << "__sizeBytes : " << __sizeBytes << endl;
  if(__p_str_ret != 0)
  {
    cout<<"ret0---:<not null>||" << __p_str_ret <<"||"<<endl;
  }
  else
  {
    cout<<"ret0---:<null>"<<endl;
  }
}

void xdbc_func_demo(Connection* __p_conn) __OPENCODE_THROW_DECLARE(SQLException)
{
  __XDBC_HOOK_DEFINE();
  
  CallableStatement* __p_pstmt=__p_conn->prepareCall(__XDBC_HOOK_INVOKE_MORE " ?= call func_demo(?,?)");
  xdbc_smart_ptr<CallableStatement> __sp_stmt(__p_pstmt);
  __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);
  
  __p_pstmt->setInt(__XDBC_HOOK_INVOKE_MORE 2,10);
  __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);

  __p_pstmt->registerOutParameter(__XDBC_HOOK_INVOKE_MORE 1,Types::INTEGER);
  __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);  
  __p_pstmt->registerOutParameter(__XDBC_HOOK_INVOKE_MORE 3,Types::INTEGER);
  __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);  

  __p_pstmt->executeUpdate(__XDBC_HOOK_INVOKE_ONCE);
  __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);

  int __out_param = __p_pstmt->getInt(__XDBC_HOOK_INVOKE_MORE 1);
  __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);
  cout<<"---:"<< __out_param << endl;

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
 
    xdbc_proc_demo(__p_conn);
//    xdbc_func_demo(__p_conn);
  }
  __OPENCODE_CATCH(SQLException& ex)
  {
    cout << "[" << ex.getSQLState() << "/" << ex.getSQLCode() << "]" << " - " << ex.getMessage() << endl;
  }
      
  return 0;
}
