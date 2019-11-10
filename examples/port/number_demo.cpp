#include "port/libnumber.h"
#include "port/libsys.h"

#include <iostream>
#include <memory>

USING_OPENCODE_STL_NS
USING_OPENCODE_NS

//------------------------------------------------------------------
//demo function definition
//------------------------------------------------------------------
void number_demo()
{
  int int_var  = 123;
  XLONG long_var = 20;
  
  float float_var = 30.3;
  double double_var = 40.4;

  cout << "toString() : " << Number::toString(double_var) << endl;
  cout << "toString() : " << Number::toString(int_var) << endl;

  cout << "parseLong() : " << Number::parseLong("-FF",16) << endl;
  cout << "parseInt() : " << Number::parseInt("123",10) << endl;
}
//------------------------------------------------------------------
//demo entry point 
//------------------------------------------------------------------
int main(int argc,char** argv)
{
  System::initializeEnvironment(argc,argv);
  
  number_demo();
    
  return 0;
}
