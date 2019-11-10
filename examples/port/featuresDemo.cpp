#include "port/libsys.h"

#include <iostream>
#include <memory>
#include <vector>

USING_OPENCODE_STL_NS
USING_OPENCODE_NS

//------------------------------------------------------------------
//demo function definition
//------------------------------------------------------------------

void loopDemo()
{
  XLONG __begin = 0;
  XLONG __diff  = 0;
  int __count = 10000;

  vector<int> __vec_eq(10000);
  vector<int>::iterator __pos = __vec_eq.begin();
  vector<int>::iterator __end = __vec_eq.end();

  __begin = System::currentTimeMicroSeconds();
  for(int i = 0; i < __count;++i)
  {
    for(;;)
    {
      if(__pos == __end){break;}
      ++__pos;
    }
  }
  __diff = System::currentTimeMicroSeconds() - __begin;
  cout << "for(;;) - " << __diff << " us" << endl;

  __begin = System::currentTimeMicroSeconds();
  for(int i = 0; i < __count;++i)
  {
    while(__pos != __end)
    {
      ++__pos;
    }
  }

  __diff = System::currentTimeMicroSeconds() - __begin;
  cout << "while(__pos != __end) - " << __diff << " us" << endl; 
}

void wordSizeDemo()
{
  cout << "sizeof(int)   = " << sizeof(int)   << endl; 
  cout << "sizeof(long)  = " << sizeof(long)  << endl; 
  cout << "sizeof(XLONG) = " << sizeof(XLONG) << endl;
#ifdef OPENCODE_LP64
  cout << "MARCO OPENCODE_LP64  defined" << endl;
#else
  cout << "MARCO OPENCODE_LP64  undefined" << endl;
#endif  
}
  
//------------------------------------------------------------------
//demo entry point 
//------------------------------------------------------------------
int main(int argc,char** argv)
{
  System::initializeEnvironment(argc,argv);
  
  //loopDemo();
  wordSizeDemo();
  
  return 0;
}
