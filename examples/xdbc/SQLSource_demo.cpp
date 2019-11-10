#include "xdbc/SQLSource.h"
#include "port/ioc_smart.h"
#include "port/ioc_sbuf.h"
#include "port/libsys.h"
#include "port/file.h"

#include <iostream>

USING_OPENCODE_STL_NS
USING_OPENCODE_NS

//------------------------------------------------------------------
//demo function definition
//------------------------------------------------------------------

void SQLSource_demo() 
{
  vector<string> keys;
  SQLSourceImpl::getInstance().keys(keys);
  for(int i = 0; i < keys.size();++i)
  {
    const SQLData& sqldata = SQLSourceImpl::getInstance().getSQLData(keys[i]);
    printf("%03d ==> %s = <%s>\n",(i+1),keys[i].c_str(),sqldata.toString().c_str()); 
  } 
}

//------------------------------------------------------------------
//demo entry point 
//------------------------------------------------------------------

int main(int argc,char** argv)
{
  System::initializeEnvironment(argc,argv);
  
  SQLSource_demo();

  return 0;
}

