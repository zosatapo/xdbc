#include "port/timestamp.h"
#include "port/libsys.h"

__OPENCODE_BEGIN_NAMESPACE

Timestamp::~Timestamp() 
{
}

Timestamp::Timestamp(const Timestamp& rv)
{
  __date = rv.__date;
  nanos = rv.nanos;
}

Timestamp& Timestamp::operator=(const Timestamp& rv)
{
  if(&rv == this)
  {
    return *this;
  }

  __date = rv.__date;
  nanos = rv.nanos;

  return *this;
}

  
Timestamp::Timestamp()
{
  XLONG seconds = 0;
  XLONG nanoseconds = 0;
  System::currentTime(seconds,nanoseconds);
  
  XLONG millis = seconds*1000 + (nanoseconds/(1000*1000));
  __date.setTime(millis);
  nanos = (int)nanoseconds;
}

Timestamp::Timestamp(int year, int month, int date,int hour, int minute, int second, int nano) 
:__date(year, month, date, hour, minute, second)
{
  nanos = nano;
  
  if (nano > 999999999) 
  {
    this->nanos = 999999999;
  }

  if ( nano < 0) 
  {
    this->nanos = 0;
  }   
}

Timestamp::Timestamp(XLONG time) 
{  
  this->nanos = (int)((time%1000) * 1000000);
  if (this->nanos < 0) 
  {
    this->nanos = 1000000000 + nanos;
    __date.setTime(((time/1000)-1)*1000);
  }
  else
  {
    __date.setTime((time/1000)*1000);
  }
}


void Timestamp::setTime(XLONG time) 
{
  this->nanos = (int)((time%1000) * 1000000);
  if (this->nanos < 0) 
  {
    this->nanos = 1000000000 + nanos;
    __date.setTime(((time/1000)-1)*1000);
  }
  else
  {
    __date.setTime((time/1000)*1000);
  }
}


XLONG Timestamp::getTime() const
{
  return (__date.getTime() + (this->nanos / 1000000));
}

int Timestamp::getYear() const
{
  return __date.getYear();
}


void Timestamp::setYear(int year)
{
  __date.setYear(year);
}


int Timestamp::getMonth() const
{
  return __date.getMonth();
}


void Timestamp::setMonth(int month)
{
  __date.setMonth(month);
}

int Timestamp::getDate() const
{
  return __date.getDate();
}


void Timestamp::setDate(int date)
{
  __date.setDate(date);
}

int Timestamp::getDay() const
{
  return __date.getDay();
}

int Timestamp::getHours() const
{
  return __date.getHours();
}


void Timestamp::setHours(int hours)
{
  __date.setHours(hours);
}

int Timestamp::getMinutes() const
{
  return __date.getMinutes();
}

void Timestamp::setMinutes(int minutes)
{
  __date.setMinutes(minutes);
}

int Timestamp::getSeconds() const
{
  return __date.getSeconds();
}

void Timestamp::setSeconds(int seconds)
{
  __date.setSeconds(seconds);
}

int Timestamp::getNanos() const
{
  return this->nanos;
}

void Timestamp::setNanos(int nano) 
{
  nanos = nano;
  
  if (nano > 999999999) 
  {
    this->nanos = 999999999;
  }

  if ( nano < 0) 
  {
    this->nanos = 0;
  }
}

string Timestamp::toString() const
{
  //YYYY-MM-DD hh:mm:ss.xxxyyyzzz(29)
  char result[40] = {0};
  sprintf(result,"%04d-%02d-%02d %02d:%02d:%02d.%09d",getYear(),getMonth()+1,getDate(),getHours(),getMinutes(),getSeconds(),getNanos());

  return string(result);
}

void Timestamp::setTimeZone(const TimeZone* aTimeZone)
{
  __date.setTimeZone(aTimeZone);
}

const TimeZone* Timestamp::getTimeZone() const
{
  return __date.getTimeZone();
}


int Timestamp::getTimezoneOffset() const
{
  return __date.getTimezoneOffset();
}

__OPENCODE_END_NAMESPACE

