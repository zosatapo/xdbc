#include "port/systemLibrary.h"
#include "port/libsys.h"

#include <iostream>
#include <memory>

USING_OPENCODE_STL_NS
USING_OPENCODE_NS

//------------------------------------------------------------------
//demo function definition
//------------------------------------------------------------------
void syslib_demo()
{
  __OPENCODE_TRY
  {
    SystemLibrary slib = SystemLibrary::getInstance("/home/reic/instantclient_10_2/libclntsh.so.10.1");
    slib.close();
  }
  __OPENCODE_CATCH(BaseException& ex)
  {
    cout << ex.getMessage() << endl;
  }
}
  
//------------------------------------------------------------------
//demo entry point 
//------------------------------------------------------------------
int main(int argc,char** argv)
{
  System::initializeEnvironment(argc,argv);
  
  syslib_demo();
    
  return 0;
}
