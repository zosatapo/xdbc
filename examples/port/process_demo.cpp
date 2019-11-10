#include "port/libproc.h"
#include "port/libsys.h"

#include <iostream>
#include <memory>

USING_OPENCODE_STL_NS
USING_OPENCODE_NS

//------------------------------------------------------------------
//demo function definition
//------------------------------------------------------------------
void process_demo()
{
}
  
//------------------------------------------------------------------
//demo entry point 
//------------------------------------------------------------------
int main(int argc,char** argv)
{
  System::initializeEnvironment(argc,argv);
  
  process_demo();
    
  return 0;
}
