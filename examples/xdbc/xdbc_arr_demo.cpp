#include "xdbc_demo.h"
#include <iostream>
using namespace std;

//------------------------------------------------------------------
//util class definition 
//------------------------------------------------------------------

#define __ARRAY_ROWS_MAX  3 
#define __ARRAY_COLS_MAX  100 
int   int_col[__ARRAY_ROWS_MAX] = {1,2,3};
char  nc_col[__ARRAY_ROWS_MAX][__ARRAY_COLS_MAX] = {"nc-2-1","nc-2-2","nc-2-3"};
char  vc_col[__ARRAY_ROWS_MAX][__ARRAY_COLS_MAX] = {"vc-3-1","vc-3-2","vc-3-3"};


class ArrayTestX : public GenericTableModel
{
public:
  ~ArrayTestX() {}

  ArrayTestX()
  {
     __dm_offsets.push_back((const char*)&(int_col[0]) - getDataBuffer());
     //__dm_rawtypes.push_back(SQLT_INT);
     __dm_types.push_back(XDBC_TYPE_INTEGER);
     __dm_sizes.push_back(sizeof(int));
  
     __dm_offsets.push_back((const char*)&(nc_col[0][0]) - getDataBuffer());
     //__dm_rawtypes.push_back(SQLT_AFC);
     __dm_types.push_back(XDBC_TYPE_CHAR);
     __dm_sizes.push_back(__ARRAY_COLS_MAX);

     __dm_offsets.push_back((const char*)&(vc_col[0][0]) - getDataBuffer());
     //__dm_rawtypes.push_back(SQLT_CHR);
     __dm_types.push_back(XDBC_TYPE_VARCHAR); 
     __dm_sizes.push_back(__ARRAY_COLS_MAX);
  }
   
  virtual int getRowCount() const
  {
    return __ARRAY_ROWS_MAX;
  }  
  
  virtual int  getColumnCount() const
  {
    return 3;
  }
  
  virtual int  getColumnType(int column) const
  {
    cout << "getColumnType[" << column << "] = " << __dm_types[column-1] << endl;
    return __dm_types[column - 1];
    return 0;
  }
  
  bool  useColumnRawType(int column) const
  {
    return false;
  }
  
  virtual int  getColumnRawType(int column) const
  {
    cout << "getColumnRawType[" << column << "] = " << __dm_rawtypes[column-1] << endl;
    return __dm_rawtypes[column - 1];
  }
  
  virtual int  getColumnSize(int column) const
  {
    cout << "getColumnSize[" << column << "] = " << __dm_sizes[column-1] << endl;
    return __dm_sizes[column - 1];
  }
  
  virtual XLONG  getColumnOffset(int column) const
  {
    cout << "getColumnOffset[" << column << "] = " << __dm_offsets[column-1] << endl;
    return __dm_offsets[column - 1];
  }
  
  virtual int  getColumnSkip(int column) const
  {
    cout << "getColumnSkip[" << column << "] = " << __dm_sizes[column-1] << endl;
    return __dm_sizes[column - 1];
  }

public:
  virtual const char*  getDataBuffer() const
  {
    return (const char*)&(int_col[0]);  
  }

private:
  vector<int> __dm_rawtypes;
  vector<int> __dm_types;
  vector<int> __dm_sizes;
  vector<XLONG> __dm_offsets;
};

//------------------------------------------------------------------
//demo function definition
//------------------------------------------------------------------
void xdbc_arr_sel_demo(Connection* __p_conn) __OPENCODE_THROW_DECLARE(SQLException)
{
  ArrayTestX ts;
  DataTable xt(&ts);

  __XDBC_HOOK_DEFINE();
  
  __OPENCODE_TRY
  {
    ArrayStatement* __p_pstmt=__p_conn->prepareArray(__XDBC_HOOK_INVOKE_MORE "SELECT int_col,nc_col,vc_col from XDBC_DEMO");
    xdbc_smart_ptr<ArrayStatement> __sp_pstmt(__p_pstmt);
    __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);
    
    xdbc_smart_ptr<ArrayResultSet> __sp_rs(__p_pstmt->executeQuery(__XDBC_HOOK_INVOKE_MORE &xt,0));
    __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);

    for(;;)
    {
      int __rowcount = __sp_rs->next(__XDBC_HOOK_INVOKE_ONCE);
      __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);
      printf("__rowcount = %d\n",__rowcount);
      if(__rowcount <= 0)
      {
        break;
      }
    }
    {
      int __rowcount = __sp_rs->next(__XDBC_HOOK_INVOKE_ONCE);
      __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);
    }
  }
  __OPENCODE_CATCH(SQLException& ex)
  {
    __OPENCODE_RETHROW;
  }
}


void xdbc_arr_upd_demo(Connection* __p_conn) __OPENCODE_THROW_DECLARE(SQLException)
{
  vector<ArrayException> errors;
  ArrayTestX ts;

  DataTable xt(&ts);

  __XDBC_HOOK_DEFINE();

  __OPENCODE_TRY
  {
    ArrayStatement* __p_pstmt=__p_conn->prepareArray(__XDBC_HOOK_INVOKE_MORE "insert into XDBC_DEMO(int_col,nc_col,vc_col) values(?,?,?)");
    xdbc_smart_ptr<ArrayStatement> __sp_pstmt(__p_pstmt);
    __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);

    __p_pstmt->executeUpdate(__XDBC_HOOK_INVOKE_MORE &xt,&errors);
    __XDBC_HOOK_CHECK(__XDBC_HOOK_PRINTF();return);
  }
  __OPENCODE_CATCH(SQLException& ex)
  {
    int size = errors.size();
    for(int i=0;i<size;++i)
    {
      cout << errors[i].getRowOffset() << "-" << errors[i].getException().getMessage() << endl;
    }
    __OPENCODE_RETHROW;
  }
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
    
//    xdbc_arr_upd_demo(__p_conn);
    xdbc_arr_sel_demo(__p_conn);
  }
  __OPENCODE_CATCH(SQLException& ex)
  {
    cout << "[" << ex.getSQLState() << "/" << ex.getSQLCode() << "]" << " - " << ex.getMessage() << endl;
  }
      
  return 0;
}
