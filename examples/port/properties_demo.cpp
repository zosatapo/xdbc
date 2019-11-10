#include "port/properties.h"
#include "port/libsys.h"
#include "port/ioc_sbuf.h" 

#include <iostream>
#include <memory>

USING_OPENCODE_STL_NS
USING_OPENCODE_NS

//------------------------------------------------------------------
//demo function definition
//------------------------------------------------------------------
void properties_demo()
{
  string __apphome = System::getProperty("application.home"); 
  string __conf = __apphome + File::separator +"schema.properties";
    
  Properties props(__conf);

  StringBuffer __sb;
  props.dump(__sb);
  
  cout << __sb.str() << endl;
}
  
//------------------------------------------------------------------
//demo entry point 
//------------------------------------------------------------------
int main(int argc,char** argv)
{
  System::initializeEnvironment(argc,argv);
  
  properties_demo();
    
  return 0;
}
