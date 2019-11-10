#include "port/utilities.h"
#include "port/libsys.h"

#include <iostream>
#include <memory>

USING_OPENCODE_STL_NS
USING_OPENCODE_NS

//------------------------------------------------------------------
//demo function definition
//------------------------------------------------------------------
void memUtilities_demo()
{
  char p[20]={"0123456789ABCDEF"};
  p[0] = 10;

  cout << "== MemoryKit::numDump ==" <<endl;
  cout << MemoryKit::numDump(p,strlen(p)) << endl;

  string data;
  cout << "== MemoryKit::encode16 ==" <<endl;
  MemoryKit::encode16(data,p,strlen(p));
  cout << MemoryKit::numDump(data.data(),data.size()) << endl;

  string data2;
  cout << "== MemoryKit::decode16 ==" <<endl;
  MemoryKit::decode16(data2,data.data(),data.size());
  cout << MemoryKit::numDump(data2.data(),data2.size()) << endl;
}

//------------------------------------------------------------------
//demo entry point 
//------------------------------------------------------------------
int main(int argc,char** argv)
{
  System::initializeEnvironment(argc,argv);
  
  memUtilities_demo();
    
  return 0;
}
