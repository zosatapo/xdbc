#include "port/librandom.h"
#include "port/libsys.h"

#include <iostream>
#include <memory>

USING_OPENCODE_STL_NS
USING_OPENCODE_NS

//------------------------------------------------------------------
//demo function definition
//------------------------------------------------------------------
void random_demo()
{
  Random __random;
  cout << "__random.nextInt() : " <<  __random.nextInt() << endl;
}
  
//------------------------------------------------------------------
//demo entry point 
//------------------------------------------------------------------
int main(int argc,char** argv)
{
  System::initializeEnvironment(argc,argv);
  
  random_demo();
    
  return 0;
}
