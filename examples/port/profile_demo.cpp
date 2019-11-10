#include "port/profile.h"
#include "port/libsys.h"
#include "port/ioc_sbuf.h" 

#include <iostream>
#include <memory>

USING_OPENCODE_STL_NS
USING_OPENCODE_NS

//------------------------------------------------------------------
//demo function definition
//------------------------------------------------------------------
void profile_demo()
{
  string __apphome = System::getProperty("application.home"); 
  string __conf = __apphome + File::separator +"schema.ini";
    
  Profile profile(__conf);
  
  vector<string> sections;
  profile.sections(sections);

  for(int i = 0;i < sections.size();++i) 
  {
    cout << "[" << sections[i] << "]" << endl;
    StringBuffer __sb;
    profile.getProperties(sections[i]).dump(__sb);
    cout << __sb.str() << endl;
 
  }
}
  
//------------------------------------------------------------------
//demo entry point 
//------------------------------------------------------------------
int main(int argc,char** argv)
{
  System::initializeEnvironment(argc,argv);
 
  profile_demo();
  
  return 0;
}
