#include "xdbc/XDBCUtilities.h"
#include "port/stringTokenizer.h" 

__OPENCODE_BEGIN_NAMESPACE

string  XDBCUtilities::parseURL(__XDBC_ATTRIBUTES& props,const string& url,bool doChange)
{
  string newURL = url;
  size_t pos = url.find_first_of(';');
 
  if(pos != string::npos && (pos+1 < url.length()))
  {
    if(doChange)
    { 
      newURL = url.substr(0,pos);
    }


    string infoURL = url.substr(pos+1);
    StringTokenizer tokens(infoURL,";");
    
    while(tokens.hasMoreTokens())
    {
      string token = tokens.nextToken();
      pos = token.find_first_of('=');

      if(pos != string::npos && (pos+1 < token.length()))
      {
        props[token.substr(0,pos)] = token.substr(pos+1);
      }
    }
  }
  
  return newURL;
}

void XDBCUtilities::setDate(const XDBC_DATE& date,Date& rv)
{
  rv.setYear(date.year);
  rv.setMonth(date.month - 1 ); 
  rv.setDate(date.day);
}

void XDBCUtilities::setTime(const XDBC_TIME& time,Time& rv)
{ 
  rv.setHours(time.hour);
  rv.setMinutes(time.minute); 
  rv.setSeconds(time.second);
}

void XDBCUtilities::setTimestamp(const XDBC_TIMESTAMP& timestamp,Timestamp& rv)
{ 
  rv.setYear(timestamp.year);
  rv.setMonth(timestamp.month - 1 );  
  rv.setDate(timestamp.day);

  rv.setHours(timestamp.hour);
  rv.setMinutes(timestamp.minute);  
  rv.setSeconds(timestamp.second);
  
  rv.setNanos(timestamp.fraction);
}  

void XDBCUtilities::setSQLDate(const Date& date,XDBC_DATE& rv)
{
  rv.year  = date.getYear();
  rv.month = date.getMonth() + 1;
  rv.day   = date.getDate();
}

void XDBCUtilities::setSQLTime(const Time& time,XDBC_TIME& rv)
{  
  rv.hour   = time.getHours();
  rv.minute = time.getMinutes();
  rv.second = time.getSeconds();
}

void XDBCUtilities::setSQLTimestamp(const Timestamp& timestamp,XDBC_TIMESTAMP& rv)
{ 
  rv.year  = timestamp.getYear();
  rv.month = timestamp.getMonth() + 1;
  rv.day   = timestamp.getDate(); 
  
  rv.hour   = timestamp.getHours();
  rv.minute = timestamp.getMinutes();
  rv.second = timestamp.getSeconds();
  
  rv.fraction = timestamp.getNanos(); 
}
  
string XDBCUtilities::toString(const XDBC_DATE& __date)
{
  //YYYY-MM-DD(10)
  char result[40] = {0};
  sprintf(result,"%04d-%02d-%02d",__date.year,__date.month,__date.day);

  return string(result);  
}

string XDBCUtilities::toString(const XDBC_TIME& __time)
{
  //hh:mm:ss(8)
  char result[40] = {0};
  sprintf(result,"%02d:%02d:%02d",__time.hour,__time.minute,__time.second);

  return string(result);
}

string XDBCUtilities::toString(const XDBC_TIMESTAMP& __timestamp)
{
  //YYYY-MM-DD hh:mm:ss.xxxyyyzzz(29)
  char result[40] = {0};
  sprintf(result,"%04d-%02d-%02d %02d:%02d:%02d.%09d",__timestamp.year,__timestamp.month,__timestamp.day,__timestamp.hour,__timestamp.minute,__timestamp.second,__timestamp.fraction);

  return string(result);
}

string XDBCUtilities::toString(const XDBC_INTERVAL& __interval)
{
  char result[40] = {0};
  int  __interval_type = __interval.interval_type;
  char __interval_sign = '+';
  if(__interval.interval_sign == XDBC_TRUE)
  {
    __interval_sign = '-';
  }
  
  switch(__interval_type)
  {
    case XDBC_IS_YEAR:
    case XDBC_IS_MONTH:
    case XDBC_IS_YEAR_TO_MONTH:  
    {
      sprintf(result,"%c %d-%d",__interval_sign,
                                __interval.intval.year_month.year,
                                __interval.intval.year_month.month);
    }  
    break;
    
    default:
    {
      sprintf(result,"%c %d %d:%d:%d.%09d",__interval_sign,
                                           __interval.intval.day_second.day,
                                           __interval.intval.day_second.hour,
                                           __interval.intval.day_second.minute,
                                           __interval.intval.day_second.second,
                                           __interval.intval.day_second.fraction);
    }  
    break;
  }  
    
  return string(result);
}    

string XDBCUtilities::toString(const XDBC_NUMERIC& numberic)
{
  return XDBC_NUMERIC::stringValue(&numberic);
}

__OPENCODE_END_NAMESPACE

