#include "xdbc_demo.h"

//------------------------------------------------------------------
//demo function definition
//------------------------------------------------------------------
const char* __g_sel_SQL = ("SELECT * FROM XDBC_PERF WHERE INT_COL = ?");
const char* __g_ins_SQL = ("INSERT INTO XDBC_PERF(INT_COL) VALUES(?)");
const char* __g_upd_SQL = ("UPDATE XDBC_PERF SET INT_COL2 = 1 WHERE INT_COL = ? ");
const char* __g_del_SQL = ("DELETE FROM XDBC_PERF WHERE INT_COL = ?");

int __g_count = 10000;

void xdbc_sel_round(Connection* __p_xdbc_conn) __OPENCODE_THROW_DECLARE(SQLException)
{
  __XDBC_HOOK_DEFINE();

  XLONG __begin = 0;
  XLONG __dif_x   = 0;
  XLONG __total_x = 0;
  int __count = __g_count;

  for(int i = 0; i < __count; ++i)
  {
    __begin = System::currentTimeMicroSeconds(); 
    {
      PreparedStatement*  __p_pstmt = __p_xdbc_conn->prepareStatement(__XDBC_HOOK_INVOKE_MORE __g_sel_SQL);               
      xdbc_smart_ptr<PreparedStatement> __sp_pstmt(__p_pstmt);
      __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return); 
      
      __p_pstmt->setInt(__XDBC_HOOK_INVOKE_MORE 1,i);
      __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);

      ResultSet* __p_xdbc_rs = __p_pstmt->executeQuery(__XDBC_HOOK_INVOKE_ONCE);
      xdbc_smart_ptr<ResultSet> __sp_rs(__p_xdbc_rs);
      __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);

      int __int_col = 0;
      while(__p_xdbc_rs->next(__XDBC_HOOK_INVOKE_ONCE))
      {
      __p_xdbc_rs->getInt(__XDBC_HOOK_INVOKE_MORE 1,__int_col);
      __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);

      __p_xdbc_rs->getInt(__XDBC_HOOK_INVOKE_MORE 2,__int_col);
      __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);        
      }
      __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);      
    }
    __dif_x = System::currentTimeMicroSeconds() - __begin;
    __total_x += __dif_x;
  }
  
  cout << "xdbc_round   = " << __total_x << " us" << endl;
}

void xdbc_sel_prepare(Connection* __p_xdbc_conn) __OPENCODE_THROW_DECLARE(SQLException)
{
  __XDBC_HOOK_DEFINE();
 
  XLONG __begin = 0;
  XLONG __dif_x   = 0;
  XLONG __total_x = 0;
  int __count = __g_count;
 
  for(int i = 0; i < __count; ++i)
  {
    __begin = System::currentTimeMicroSeconds();
    {
      PreparedStatement*  __p_pstmt = __p_xdbc_conn->prepareStatement(__XDBC_HOOK_INVOKE_MORE __g_sel_SQL);
      xdbc_smart_ptr<PreparedStatement> __sp_pstmt(__p_pstmt);
      __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);
    }
    __dif_x = System::currentTimeMicroSeconds() - __begin;
    __total_x += __dif_x;
  }
 
  cout << "xdbc_prepare = " << __total_x << " us" << endl;
}

void xdbc_sel_execute(Connection* __p_xdbc_conn) __OPENCODE_THROW_DECLARE(SQLException)
{
  __XDBC_HOOK_DEFINE();
 
  XLONG __begin = 0;
  XLONG __dif_x   = 0;
  XLONG __total_x = 0;
  int __count = __g_count;
 
  PreparedStatement*  __p_pstmt = __p_xdbc_conn->prepareStatement(__XDBC_HOOK_INVOKE_MORE __g_sel_SQL);
  xdbc_smart_ptr<PreparedStatement> __sp_pstmt(__p_pstmt);
  __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);
  ResultSet* __p_xdbc_rs = 0;

  {
    __p_pstmt->setInt(__XDBC_HOOK_INVOKE_MORE 1,1);
    __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);

    __p_xdbc_rs = __p_pstmt->executeQuery(__XDBC_HOOK_INVOKE_ONCE);
    xdbc_smart_ptr<ResultSet> __sp_rs(__p_xdbc_rs);
    __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);
  }

  for(int i = 0; i < __count; ++i)
  {
    __begin = System::currentTimeMicroSeconds();
    {
      __p_pstmt->setInt(__XDBC_HOOK_INVOKE_MORE 1,i);
      __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);

      __p_xdbc_rs = __p_pstmt->executeQuery(__XDBC_HOOK_INVOKE_ONCE);
      __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);
    }
    __dif_x = System::currentTimeMicroSeconds() - __begin;
    __total_x += __dif_x;
    xdbc_smart_ptr<ResultSet> __sp_rs(__p_xdbc_rs);
  }

  cout << "xdbc_execute = " << __total_x << " us" << endl;
}

void xdbc_sel_fetch(Connection* __p_xdbc_conn) __OPENCODE_THROW_DECLARE(SQLException)
{
  __XDBC_HOOK_DEFINE();
 
  XLONG __begin = 0;
  XLONG __dif_x   = 0;
  XLONG __total_x = 0;
  int __count = __g_count;
 
  PreparedStatement*  __p_pstmt = __p_xdbc_conn->prepareStatement(__XDBC_HOOK_INVOKE_MORE __g_sel_SQL);
  xdbc_smart_ptr<PreparedStatement> __sp_pstmt(__p_pstmt);
  __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);
 
  for(int i = 0; i < __count; ++i)
  {
    __p_pstmt->setInt(__XDBC_HOOK_INVOKE_MORE 1,i);
    __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);

    ResultSet* __p_xdbc_rs = __p_pstmt->executeQuery(__XDBC_HOOK_INVOKE_ONCE);
    xdbc_smart_ptr<ResultSet> __sp_rs(__p_xdbc_rs);
    __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);   
      
    __begin = System::currentTimeMicroSeconds();
    {
      int __int_col = 0;
      while(__p_xdbc_rs->next(__XDBC_HOOK_INVOKE_ONCE))
      {
      __p_xdbc_rs->getInt(__XDBC_HOOK_INVOKE_MORE 1,__int_col);
      __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);

      __p_xdbc_rs->getInt(__XDBC_HOOK_INVOKE_MORE 2,__int_col);
      __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return); 
      }
      __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);
      
    }
    __dif_x = System::currentTimeMicroSeconds() - __begin;
    __total_x += __dif_x;
  }
 
  cout << "xdbc_fetch   = " << __total_x << " us" << endl;
}

void xdbc_sel_demo(Connection* __p_xdbc_conn) __OPENCODE_THROW_DECLARE(SQLException)
{
  __XDBC_HOOK_DEFINE();
 
  XLONG __begin = 0;  
  XLONG __total_x = 0;
  int __count = __g_count;

  PreparedStatement*  __p_pstmt = __p_xdbc_conn->prepareStatement(__XDBC_HOOK_INVOKE_MORE __g_sel_SQL);
  xdbc_smart_ptr<PreparedStatement> __sp_pstmt(__p_pstmt);
  __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);

  ResultSet* __p_xdbc_rs = 0;
  {
    __p_pstmt->setInt(__XDBC_HOOK_INVOKE_MORE 1,1);
    __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);

    __p_xdbc_rs = __p_pstmt->executeQuery(__XDBC_HOOK_INVOKE_ONCE);
    xdbc_smart_ptr<ResultSet> __sp_rs(__p_xdbc_rs);
    __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);
  }  

  __begin = System::currentTimeMicroSeconds();
  int __int_col = 0;

  for(int i = 0; i < __count; ++i)
  {
    __p_xdbc_rs = __p_pstmt->executeQuery(__XDBC_HOOK_INVOKE_ONCE);
    xdbc_smart_ptr<ResultSet> __sp_rs(__p_xdbc_rs);
    __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);

    __p_pstmt->setInt(__XDBC_HOOK_INVOKE_MORE 1,i);
    __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);

    while(__p_xdbc_rs->next(__XDBC_HOOK_INVOKE_ONCE))
    {
      __p_xdbc_rs->getInt(__XDBC_HOOK_INVOKE_MORE 1,__int_col);
      __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);

      __p_xdbc_rs->getInt(__XDBC_HOOK_INVOKE_MORE 2,__int_col);
      __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);
    }
    __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);
  }
  __total_x = System::currentTimeMicroSeconds() - __begin;

  cout << "xdbc_full    = " << __total_x << " us" << endl;
}

void xdbc_ins_demo(Connection* __p_xdbc_conn) __OPENCODE_THROW_DECLARE(SQLException)
{
  __XDBC_HOOK_DEFINE();

  XLONG __begin = 0;
  XLONG __total_x = 0;
  int __count = __g_count;

  PreparedStatement*  __p_pstmt = __p_xdbc_conn->prepareStatement(__XDBC_HOOK_INVOKE_MORE __g_ins_SQL);
  xdbc_smart_ptr<PreparedStatement> __sp_pstmt(__p_pstmt);
  __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);

  __begin = System::currentTimeMicroSeconds();
  int __int_col = 0;
  for(int i = 0; i < __count; ++i)
  {
    __p_pstmt->setInt(__XDBC_HOOK_INVOKE_MORE 1,i);
    __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);

    __p_pstmt->executeUpdate(__XDBC_HOOK_INVOKE_ONCE);
    __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);
  }
  __total_x = System::currentTimeMicroSeconds() - __begin;

  cout << "xdbc_insert  = " << __total_x << " us" << endl;
}

void xdbc_upd_demo(Connection* __p_xdbc_conn) __OPENCODE_THROW_DECLARE(SQLException)
{
  __XDBC_HOOK_DEFINE();

  XLONG __begin = 0;
  XLONG __total_x = 0;
  int __count = __g_count;

  PreparedStatement*  __p_pstmt = __p_xdbc_conn->prepareStatement(__XDBC_HOOK_INVOKE_MORE __g_upd_SQL);
  xdbc_smart_ptr<PreparedStatement> __sp_pstmt(__p_pstmt);
  __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);

  __begin = System::currentTimeMicroSeconds();

  for(int i = 0; i < __count; ++i)
  {
    __p_pstmt->setInt(__XDBC_HOOK_INVOKE_MORE 1,i);
    __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);

    __p_pstmt->executeUpdate(__XDBC_HOOK_INVOKE_ONCE);
    __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);
  }
  __total_x = System::currentTimeMicroSeconds() - __begin;

  cout << "xdbc_update  = " << __total_x << " us" << endl;
}

void xdbc_del_demo(Connection* __p_xdbc_conn) __OPENCODE_THROW_DECLARE(SQLException)
{
  __XDBC_HOOK_DEFINE();

  XLONG __begin = 0;
  XLONG __total_x = 0;
  int __count = __g_count;

  PreparedStatement*  __p_pstmt = __p_xdbc_conn->prepareStatement(__XDBC_HOOK_INVOKE_MORE __g_del_SQL);
  xdbc_smart_ptr<PreparedStatement> __sp_pstmt(__p_pstmt);
  __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);

  __begin = System::currentTimeMicroSeconds();

  for(int i = 0; i < __count; ++i)
  {
    __p_pstmt->setInt(__XDBC_HOOK_INVOKE_MORE 1,i);
    __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);

    __p_pstmt->executeUpdate(__XDBC_HOOK_INVOKE_ONCE);
    __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);
  }
  __total_x = System::currentTimeMicroSeconds() - __begin;

  cout << "xdbc_delete  = " << __total_x << " us" << endl;
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
    Connection* __p_xdbc_conn = getConnection(__XDBC_HOOK_INVOKE_MORE dsname);
    xdbc_smart_ptr<Connection> __sp_conn(__p_xdbc_conn);
    __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return 0);
   
    Statement* __p_stmt = 0;
    __OPENCODE_TRY
    {
    __p_stmt = __p_xdbc_conn->createStatement();  
    xdbc_smart_ptr<Statement> __sp_stmt(__p_stmt);
    __p_stmt->executeUpdate(__XDBC_HOOK_INVOKE_MORE "CREATE TABLE XDBC_PERF(INT_COL INTEGER,INT_COL2 INTEGER,PRIMARY KEY(INT_COL))");  
    __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);
    }
    __OPENCODE_CATCH(SQLException& ex)
    {
      cout << "[" << ex.getSQLState() << "/" << ex.getSQLCode() << "]" << " - " << ex.getMessage() << endl;
    }

    __OPENCODE_TRY
    { 
    xdbc_ins_demo(__p_xdbc_conn);
    xdbc_sel_round(__p_xdbc_conn);
    xdbc_sel_prepare(__p_xdbc_conn);
    xdbc_sel_execute(__p_xdbc_conn);
    xdbc_sel_fetch(__p_xdbc_conn);
    xdbc_sel_demo(__p_xdbc_conn);
    xdbc_upd_demo(__p_xdbc_conn);
    xdbc_del_demo(__p_xdbc_conn);
    }
    __OPENCODE_CATCH(SQLException& ex)
    {
      cout << "[" << ex.getSQLState() << "/" << ex.getSQLCode() << "]" << " - " << ex.getMessage() << endl;
    }

    {
    __p_stmt = __p_xdbc_conn->createStatement();
    xdbc_smart_ptr<Statement> __sp_stmt(__p_stmt);
    __p_stmt->executeUpdate(__XDBC_HOOK_INVOKE_MORE "DROP TABLE XDBC_PERF");
    __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);
    }
  }
  __OPENCODE_CATCH(SQLException& ex)
  {
    cout << "[" << ex.getSQLState() << "/" << ex.getSQLCode() << "]" << " - " << ex.getMessage() << endl;
  }

  return 0;
}
