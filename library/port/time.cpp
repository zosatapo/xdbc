#include "port/time.h"

__OPENCODE_BEGIN_NAMESPACE

Time::~Time()
{

}

Time::Time(const Time& rv)
{
  __date = rv.__date;
}

Time& Time::operator=(const Time& rv)
{
  if(&rv == this)
  {
    return *this;
  }

  __date = rv.__date;

  return *this;
}


Time::Time()
:__date(1970, 0, 1)
{
}

Time::Time(int hour, int minute, int second) 
:__date(1970, 0, 1, hour, minute, second)
{
}
   
Time::Time(XLONG time) 
:__date(time)
{
}

void Time::setHours(int hours)
{
  __date.setHours(hours);
}

int Time::getHours() const
{
  return __date.getHours();
}

int Time::getMinutes() const
{
  return __date.getMinutes();
}

void Time::setMinutes(int minutes)
{
  __date.setMinutes(minutes);
}

int Time::getSeconds() const
{
  return __date.getSeconds();
}

void Time::setSeconds(int seconds)
{
  __date.setSeconds(seconds);
}

void Time::setTime(XLONG time) 
{
  __date.setTime(time);
}

XLONG Time::getTime() const
{
  return __date.getTime();
}

string Time::toString() const
{
  //hh:mm:ss(8)
  char result[30] = {0};
  sprintf(result,"%02d:%02d:%02d",getHours(),getMinutes(),getSeconds());
  return string(result);
}

__OPENCODE_END_NAMESPACE
