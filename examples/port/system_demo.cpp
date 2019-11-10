#include "port/libsys.h"

#include <iostream>
#include <memory>
#include <vector>

USING_OPENCODE_STL_NS
USING_OPENCODE_NS

//------------------------------------------------------------------
//demo function definition
//------------------------------------------------------------------
void system_demo()
{
  vector<string> names;
  System::propertyNames(names);
  for(int i = 0; i < names.size() ; ++i)
  {
    cout << names[i] << " = " << System::getProperty(names[i]) << endl;
  }
}

//------------------------------------------------------------------
//demo entry point 
//------------------------------------------------------------------
int main(int argc,char** argv)
{
  System::initializeEnvironment(argc,argv);
  
  system_demo();
  
  return 0;
}
