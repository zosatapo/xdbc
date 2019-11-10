#include "port/file.h"
#include "port/libsys.h"

#include <iostream>
#include <memory>

USING_OPENCODE_STL_NS
USING_OPENCODE_NS

//------------------------------------------------------------------
//demo function definition
//------------------------------------------------------------------
void file_demo()
{
  File f(".");
  printf("[File] path=%s\n",f.getCanonicalPath().c_str());

  vector<string> names;
  f.list(names);
  
  for(int i=0;i<names.size();++i)
  {
    cout << "[File Name] "  << names[i] << endl;
  }
}

//------------------------------------------------------------------
//demo entry point 
//------------------------------------------------------------------
int main(int argc,char** argv)
{
  System::initializeEnvironment(argc,argv);
  
  file_demo();
    
  return 0;
}
