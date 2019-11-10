#include "port/date.h"
#include "port/time.h"
#include "port/timestamp.h"
#include "port/libsys.h"

#include <iostream>
#include <memory>

USING_OPENCODE_STL_NS
USING_OPENCODE_NS

//------------------------------------------------------------------
//demo function definition
//------------------------------------------------------------------
void date_demo()
{
  Date date;
  XLONG times = date.getTime();
  cout << "------ " << date.toString() << " --||" << MemoryKit::encode16((const char*)&times,sizeof(times)) << endl;
}

void time_demo()
{
  Time time;
  cout << "------ " << time.toString() << endl;
}  

void timestamp_demo()
{
  Timestamp ts;
  cout << "------ " << ts.toString() << endl;
} 

//------------------------------------------------------------------
//demo entry point 
//------------------------------------------------------------------
int main(int argc,char** argv)
{
  System::initializeEnvironment(argc,argv);
  
  date_demo();
  time_demo();
  timestamp_demo();
  
  return 0;
}
