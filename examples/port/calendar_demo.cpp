#include "port/calendar.h"
#include "port/libsys.h"
#include "port/date.h"

#include <iostream>
#include <memory>

USING_OPENCODE_STL_NS
USING_OPENCODE_NS

//------------------------------------------------------------------
//demo function definition
//------------------------------------------------------------------
void calendar_demoX()
{
  auto_ptr<TimeZone> def(TimeZone::getTimeZone("GMT+8"));
  cout << "------ " << def->toString() << endl;

  Date now;
  auto_ptr<Calendar> cal(Calendar::getInstance(def.get()));
  cal->setTimeInMillis(now.getTime());
  cout << "------ " << cal->toString() << endl;

  cal->add(Calendar::DATE,10);
  cout << "------ " << cal->toString() << endl;
}

void calendar_demo()
{
  auto_ptr<TimeZone> def(TimeZone::getTimeZone("GMT+8"));
  cout << "------ " << def->toString() << endl;

  Date now;
  auto_ptr<Calendar> cal(Calendar::getInstance(def.get()));
  cal->set(2015,10,1);
  cout << "DAY_OF_YEAR -- " << cal->get(Calendar::DAY_OF_YEAR) << endl;
  cout << "------ " << cal->toString() << endl;

  cal->add(Calendar::DATE,10);
  cout << "------ " << cal->toString() << endl;
}
  
//------------------------------------------------------------------
//demo entry point 
//------------------------------------------------------------------
int main(int argc,char** argv)
{
  System::initializeEnvironment(argc,argv);

  calendar_demo();
  
  return 0;
}
