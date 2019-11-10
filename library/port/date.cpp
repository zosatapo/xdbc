#include "port/date.h"
#include "port/libsys.h"
#include "port/stringTokenizer.h"
#include "port/libnumber.h" 
#include "port/ioc_smart.h" 

__OPENCODE_BEGIN_NAMESPACE

Date::~Date() 
{
  if(p_cal != 0)
  {
    delete p_cal;
    p_cal = 0;
  }
}

Date::Date(const Date& rhs)
{
  Calendar* p_calx = 0;

  if(rhs.p_cal != 0)
  {
    p_calx = rhs.p_cal->clone();
  }
  else
  {
    p_calx = Calendar::getInstance();
    p_calx->setTimeInMillis(System::currentTimeMillis());
  }

  this->p_cal = p_calx;
}

Date& Date::operator=(const Date& rhs)
{
  if(&rhs == this)
  {
    return *this;
  }

  Calendar* p_calx = 0;
  if(rhs.p_cal != 0)
  {
    p_calx = rhs.p_cal->clone();
  }
  else
  {
    p_calx = Calendar::getInstance();
  }

  if(this->p_cal != 0)
  {
    delete this->p_cal;
  }

  this->p_cal = p_calx;

  return *this;
}

Date::Date()
{
  XLONG millis = System::currentTimeMillis();

  p_cal = Calendar::getInstance();
  p_cal->setTimeInMillis(millis);
}

Date::Date(XLONG date)
{
  p_cal = Calendar::getInstance();
  p_cal->setTimeInMillis(date);
}


Date::Date(int year, int month, int date, int hrs, int min, int sec)
{
  p_cal = Calendar::getInstance();
  p_cal->set(year, month, date,hrs, min, sec);
}

XLONG Date::UTC(int year, int month, int date,int hrs, int min, int sec)
{
  smart_ptr<TimeZone> __sp_timeZone(TimeZone::getTimeZone("GMT"));
  Calendar* p_utcCal = Calendar::getInstance(__sp_timeZone.get());
  
  p_utcCal->set(year, month, date, hrs, min, sec);
  XLONG millis = p_utcCal->getTimeInMillis();
  delete p_utcCal;
  
  return millis;  
}

int Date::getYear() const
{
  return p_cal->get(Calendar::YEAR);
}


void Date::setYear(int year)
{
  p_cal->set(Calendar::YEAR, year);
}


int Date::getMonth() const
{
  return p_cal->get(Calendar::MONTH);
}


void Date::setMonth(int month)
{
  p_cal->set(Calendar::MONTH, month);
}

int Date::getDate() const
{
  return p_cal->get(Calendar::DATE);
}


void Date::setDate(int date)
{
  p_cal->set(Calendar::DATE, date);
}

int Date::getDay() const
{
  return p_cal->get(Calendar::DAY_OF_WEEK) - Calendar::SUNDAY;
}

int Date::getHours() const
{
  return p_cal->get(Calendar::HOUR_OF_DAY);
}


void Date::setHours(int hours)
{
  p_cal->set(Calendar::HOUR_OF_DAY, hours);
}

int Date::getMinutes() const
{
  return p_cal->get(Calendar::MINUTE);
}

void Date::setMinutes(int minutes)
{
  p_cal->set(Calendar::MINUTE, minutes);
}

int Date::getSeconds() const
{
  return p_cal->get(Calendar::SECOND);
}

void Date::setSeconds(int seconds)
{
  p_cal->set(Calendar::SECOND, seconds);
}

XLONG Date::getTime() const
{
  return p_cal->getTimeInMillis();
}

void Date::setTime(XLONG time)
{
  p_cal->setTimeInMillis(time);
}

string  Date::toString() const
{
  //YYYY-MM-DD hh:mm:ss(19)
  char result[30] = {0};
  sprintf(result,"%04d-%02d-%02d %02d:%02d:%02d",getYear(),getMonth()+1,getDate(),getHours(),getMinutes(),getSeconds());
  return string(result);
}

void Date::setTimeZone(const TimeZone* aTimeZone)
{
  p_cal->setTimeZone(aTimeZone);
}

const TimeZone* Date::getTimeZone() const
{
  return p_cal->getTimeZone();
}

int Date::getTimezoneOffset() const
{
  int offset = p_cal->get(Calendar::ZONE_OFFSET) + p_cal->get(Calendar::DST_OFFSET);
  return - (offset / 1000 / 60); // convert to minutes
}


vector<int> Date::parseFields(const string& s,int year,int month,int date)
{
  bool hasDate = false;
  bool hasTime = false;
  bool hasFraction = false;

  const char* buf = s.c_str();

  if(strchr(buf,'-') != 0)
  {
    hasDate = true;
  }

  if( strchr(buf,':') != 0 || strchr(buf,'.') != 0)
  {
    hasTime = true;
  }

  if(hasTime &&  strchr(buf,'.') != 0)
  {
    hasFraction = true; 
  }

  int int_year = year;
  int int_month = month;
  int int_date  = date;

  StringTokenizer tokens_t(buf," ");
  string str_date;
  string str_time;

  int count_t = tokens_t.countTokens();
  if(count_t == 2)
  {
    tokens_t.hasMoreTokens();
    str_date = tokens_t.nextToken();
    hasDate = true;

    tokens_t.hasMoreTokens();
    str_time = tokens_t.nextToken();
    hasTime = true;
  }
  else if(count_t == 1)
  {
    if(hasDate && !hasTime)
    {
      tokens_t.hasMoreTokens();
      str_date = tokens_t.nextToken();
    }
    else
    {
      tokens_t.hasMoreTokens();
      str_time = tokens_t.nextToken();
      hasTime = true;
      hasDate = false;
    }
  }
  else
  {
    System::loggingAndExit("Date::parseFields|bad DATE/INTERVAL  string - (" + s + ")",-1); 
  }

//-------------------------------------------------------------------------------------------
// parse DATE : form RIGHT (DD) to LEFT (YYYY)
//-------------------------------------------------------------------------------------------

  if(hasDate)
  {
    size_t size = str_date.length();
    for(size_t i = 0;i < size;++i)
    {
      if(str_date[i] =='-')
      {
        str_date[i] = '/';
      }      
    }

    for(size_t i = 0;i < size - 1;++i)
    {
      if(str_date[i] =='/' && str_date[i+1] == '/')
      {
        str_date[i] = '-';
      }      
    }

    StringTokenizer tokens(str_date,"/");
    int count = tokens.countTokens();
    if(count == 1)
    {
      tokens.hasMoreTokens();
      int_date = Number::parseInt(tokens.nextToken());
    }
    else if(count == 2)
    {
      tokens.hasMoreTokens();
      int_month = Number::parseInt(tokens.nextToken());

      tokens.hasMoreTokens();
      int_date = Number::parseInt(tokens.nextToken());
    }
    else if(count == 3)
    {
      tokens.hasMoreTokens();
      int_year = Number::parseInt(tokens.nextToken());

      tokens.hasMoreTokens();
      int_month = Number::parseInt(tokens.nextToken());

      tokens.hasMoreTokens();
      int_date = Number::parseInt(tokens.nextToken());
    }
    else
    {
      System::loggingAndExit("Date::parseFields|bad DATE/INTERVAL  string - (" + s + ")",-1); 
    }

  }//~end if(hasDate)

//-------------------------------------------------------------------------------------------
// parse DATE : (1) (!hasFraction) -- form LEFT (HOUR) to RIGHT (FRACTION)
//              (2) (hasFraction)  -- form RIGHT (FRACTION) to LEFT (HOUR)
//-------------------------------------------------------------------------------------------
  int int_hour = 0;
  int int_minute = 0;
  int int_second = 0;

  int int_nanos = 0;
  int int_nanos_len = 0;

  if(hasTime)
  {
    StringTokenizer tokens(str_time,":");
    int count = tokens.countTokens();

    if(!hasFraction)
    {
//==============================================================================================
      if(count == 1)
      {
        tokens.hasMoreTokens();
        int_hour = Number::parseInt(tokens.nextToken());
      }
      else if(count == 2)
      {
        tokens.hasMoreTokens();
        int_hour = Number::parseInt(tokens.nextToken());
  
        tokens.hasMoreTokens();
        int_minute = Number::parseInt(tokens.nextToken());
      }
      else if(count == 3)
      {
        tokens.hasMoreTokens();
        int_hour = Number::parseInt(tokens.nextToken());
  
        tokens.hasMoreTokens();
        int_minute = Number::parseInt(tokens.nextToken());
  
        tokens.hasMoreTokens();
        StringTokenizer tokens_f(tokens.nextToken().c_str(),".");
        int count_f = tokens_f.countTokens();
        if(count_f == 1)
        {
          tokens_f.hasMoreTokens();
          int_second = Number::parseInt(tokens_f.nextToken());
        }
        else
        {
          tokens_f.hasMoreTokens();
          int_second = Number::parseInt(tokens_f.nextToken());
  
          tokens_f.hasMoreTokens();
          string str_nanos = tokens_f.nextToken();
          int_nanos_len = (int)str_nanos.length();
          int_nanos = Number::parseInt(str_nanos);
        }
      }
      else
      {
        System::loggingAndExit("Date::parseFields|bad DATE/INTERVAL  string - (" + s + ")",-1); 
      }
//==============================================================================================
    }
    else
    {
//==============================================================================================
      if(count == 1)
      {
        tokens.hasMoreTokens();
        StringTokenizer tokens_f(tokens.nextToken().c_str(),".");
        int count_f = tokens_f.countTokens();
        if(count_f == 1)
        {
          tokens_f.hasMoreTokens();
          int_second = Number::parseInt(tokens_f.nextToken());
        }
        else
        {
          tokens_f.hasMoreTokens();
          int_second = Number::parseInt(tokens_f.nextToken());
  
          tokens_f.hasMoreTokens();
          string str_nanos = tokens_f.nextToken();
          int_nanos_len = (int)str_nanos.length();
          int_nanos = Number::parseInt(str_nanos);
        }
      }
      else if(count == 2)
      {
        tokens.hasMoreTokens();
        int_minute = Number::parseInt(tokens.nextToken());
  
        tokens.hasMoreTokens();
        StringTokenizer tokens_f(tokens.nextToken().c_str(),".");
        int count_f = tokens_f.countTokens();
        if(count_f == 1)
        {
          tokens_f.hasMoreTokens();
          int_second = Number::parseInt(tokens_f.nextToken());
        }
        else
        {
          tokens_f.hasMoreTokens();
          int_second = Number::parseInt(tokens_f.nextToken());
  
          tokens_f.hasMoreTokens();
          string str_nanos = tokens_f.nextToken();
          int_nanos_len = (int)str_nanos.length();
          int_nanos = Number::parseInt(str_nanos);
        }
      }
      else if(count == 3)
      {
        tokens.hasMoreTokens();
        int_hour = Number::parseInt(tokens.nextToken());
  
        tokens.hasMoreTokens();
        int_minute = Number::parseInt(tokens.nextToken());
  
        tokens.hasMoreTokens();
        StringTokenizer tokens_f(tokens.nextToken().c_str(),".");
        int count_f = tokens_f.countTokens();
        if(count_f == 1)
        {
          tokens_f.hasMoreTokens();
          int_second = Number::parseInt(tokens_f.nextToken());
        }
        else
        {
          tokens_f.hasMoreTokens();
          int_second = Number::parseInt(tokens_f.nextToken());
  
          tokens_f.hasMoreTokens();
          string str_nanos = tokens_f.nextToken();
          int_nanos_len = (int)str_nanos.length();
          int_nanos = Number::parseInt(str_nanos);
        }
      }
      else
      {
        System::loggingAndExit("Date::parseFields|bad DATE/INTERVAL  string - (" + s + ")",-1); 
      }
//==============================================================================================
    }

  }//~end if(hasTime) 

  //adjust nano filed to 9 digits
  if(int_nanos_len > 0)
  {
    int times = 9 - int_nanos_len;
    while(times >= 1)
    {
      int_nanos = (int_nanos << 3) +(int_nanos <<1);
      --times;
    }

  }//~end if(int_nanos_len > 0)

  vector<int> values;
  values.push_back(int_year);
  values.push_back(int_month);
  values.push_back(int_date);

  values.push_back(int_hour);
  values.push_back(int_minute);
  values.push_back(int_second);
  values.push_back(int_nanos);

  return values;
}

__OPENCODE_END_NAMESPACE
