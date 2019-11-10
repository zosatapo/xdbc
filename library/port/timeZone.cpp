#include "port/timeZone.h"
#include "port/zoneInfo.h"
#include "port/libsys.h"
#include "port/calendar.h"
#include "port/logging.h"

#include "port/ioc_time.h" 

USING_OPENCODE_STL_NS
__OPENCODE_BEGIN_NAMESPACE

//----------------------------------------------------------------------------------------
//TimeZoneDoImpl class
//----------------------------------------------------------------------------------------
IMPLEMENT_LIFETIME_BEGIN(TimeZoneDoImpl)
IMPLEMENT_LIFETIME_END(TimeZoneDoImpl)

const int TimeZone::SHORT_NAME = 0;

const int TimeZone::LONG_NAME = 1;

const int TimeZone::GMT_ID_LENGTH = 3;

TimeZoneDoImpl::TimeZoneDoImpl()
{
  defaultTimeZone = 0;
}

TimeZoneDoImpl::~TimeZoneDoImpl()
{
  if(defaultTimeZone != 0)
  {
    delete defaultTimeZone;
    defaultTimeZone = 0;
  }
}

const TimeZone* TimeZoneDoImpl::getDefault()
{
  Mutex::Guard g(tz_mutex,System::__M_multithreading); 

  if(defaultTimeZone == 0 )
  {
    // get the time zone ID from the system properties
    string zoneID = System::getProperty("user.timezone");
 
    if(zoneID.empty())
    {
      string country = System::getProperty("user.country");
      string zihome = System::getProperty("zoneinfo.home");
      if(zihome.empty())
      {
        zihome = System::getProperty("ZONEINFO_HOME",".");
      }
      zoneID = TimeZone::getSystemTimeZoneID(zihome, country);

      // Get the time zone for zoneID. But not fall back to "GMT" here.
      defaultTimeZone = TimeZone::getTimeZone(zoneID, false);
    }
  
    if (defaultTimeZone == 0)
    {
      // If the given zone ID is unknown , try to
      // get the GMT-offset-based time zone ID,
      // a.k.a. custom time zone ID (e.g., "GMT-08:00").
      LOGGING_DEBUG("TimeZoneDoImpl::getDefault|given zone ID \"" << zoneID << "\" is unknown");
      
      string gmtOffsetID = TimeZone::getSystemGMTOffsetID();
      if (!gmtOffsetID.empty())
      {
        zoneID = gmtOffsetID;
      }
    }
  
    if(zoneID.length() == 0)
    {
      zoneID = TimeZone::GMT_ID;
    }
  
    if(defaultTimeZone == 0)
    {
      defaultTimeZone = TimeZone::getTimeZone(zoneID, true);
    }

    System::setProperty("user.timezone", zoneID);
    LOGGING_DEBUG("TimeZoneDoImpl::getDefault|" << defaultTimeZone->toString());
  }
  
  return defaultTimeZone;
}

void TimeZoneDoImpl::setDefault(const TimeZone* newTimeZone)
{
  if(newTimeZone==0 || newTimeZone==defaultTimeZone)
  {
    return;
  }

  Mutex::Guard g(tz_mutex,System::__M_multithreading); 

  if(defaultTimeZone != 0)
  {  
    delete defaultTimeZone;
  }

  defaultTimeZone = newTimeZone->clone();
  LOGGING_DEBUG("TimeZoneDoImpl::setDefault|" << defaultTimeZone->toString());
}

//----------------------------------------------------------------------------------------
//TimeZone class
//----------------------------------------------------------------------------------------
const char TimeZone::GMT_ID[] = "GMT";

TimeZone*  TimeZone::clone() const
{
  //delegate to doClone
  TimeZone* pClone = doClone();
  return pClone;
}

TimeZone::~TimeZone(){}
  
TimeZone::TimeZone(const TimeZone& rhs):ID(rhs.ID)
{
}

TimeZone& TimeZone::operator=(const TimeZone& rhs)
{
  if(&rhs == this)
  {
    return *this;
  }

  this->ID = rhs.ID;

  return *this;
}

TimeZone::TimeZone(const string& ID)
:ID(ID)
{
}


XLONG TimeZone::getOffset(XLONG date) const
{
  if (inDaylightTime(date))
  {
    return getRawOffset() + getDSTSavings();
  }
  
  return getRawOffset();
}


XLONG TimeZone::getOffsets(XLONG date,vector<XLONG>* offsets) const
{
  XLONG rawoffset = getRawOffset();
  XLONG dstoffset = 0;
  
  if (inDaylightTime(date))
  {
    dstoffset = getDSTSavings();
  }
 
  if(offsets != 0)
  { 
    offsets->push_back(rawoffset);
    offsets->push_back(dstoffset);
  }

  return rawoffset + dstoffset;
}

const string& TimeZone::getID() const
{
  return ID;
}

void TimeZone::setID(const string& IDValue)
{
  this->ID = IDValue;
}

string TimeZone::getDisplayName() const
{
  return getDisplayName(false, LONG_NAME, Locale::getDefault());
}

string TimeZone::getDisplayName(const Locale& localeValue) const
{
  return getDisplayName(false, LONG_NAME, localeValue);
}

string TimeZone::getDisplayName(bool daylightValue, int styleValue) const
{
  return getDisplayName(daylightValue, styleValue, Locale::getDefault());
}

string TimeZone::getDisplayName(bool daylightValue, int styleValue,const Locale& localeValue) const
{
  if (styleValue != SHORT_NAME && styleValue != LONG_NAME)
  {
    styleValue = LONG_NAME;
  }

  return getID();
}

XLONG TimeZone::getDSTSavings() const
{
  if (useDaylightTime())
  {
    return 3600000;
  }

  return 0;
}


TimeZone* TimeZone::getTimeZone(const string& aID) 
{
  return getTimeZone(aID, true);
}

TimeZone* TimeZone::getTimeZone(const string& aID, bool fallback)
{
//----------------------------------------------------------------------
// 1. find TimeZone from <zoneinfo.home> files
//----------------------------------------------------------------------
  TimeZone* zi = 0;
  
  if(!aID.empty())
  {
    //printf("1. find TimeZone from <zoneinfo.home> files -- %d,%d,%s\n",aID.size(),aID.empty(),aID.c_str());
    zi = ZoneInfo::getZoneInfo(aID);
  }

//----------------------------------------------------------------------
// 2. find TimeZone from <zoneinfo.home> files using aliasName
//----------------------------------------------------------------------
  if (zi == 0 && !(aID.empty())) 
  {
    //printf("2. find TimeZone from <zoneinfo.home> files using aliasName -- %s\n",aID.c_str());

    //if we can't create an object for the ID, try aliases.
    const OPENCODE_MAP<string,string>& aliasNames = ZoneInfoFile::getInstance().getZoneAliases();
    OPENCODE_MAP<string,string>::const_iterator realID = aliasNames.find(aID);

    if(realID == aliasNames.end())
    {
      //printf("--- realName not  found: %s\n",aID.c_str());
    }
    else
    {
      //printf("--- realName found: %s\n",realID->second.c_str());

      zi = ZoneInfoFile::getInstance().getZoneInfo(realID->second);
      if(zi != 0)
      {
        zi->setID(aID);
      }

    }//~end if(realID != 0)
  }//~end if (zi == 0)

//----------------------------------------------------------------------
// 3. parseCustomTimeZone 
//----------------------------------------------------------------------
  if(zi == 0)
  {
    //printf("3. parseCustomTimeZone -- %s\n",aID.c_str());
    zi = parseCustomTimeZone(aID);
  }

//----------------------------------------------------------------------
// 4. fallback to GMT 
//----------------------------------------------------------------------
  if(zi == 0 && fallback)
  {
    //printf("4. GMT_ID -- \n");
    return new ZoneInfo(GMT_ID,0);
  }

  return  zi;
}

vector<string> TimeZone::getAvailableIDs(XLONG rawOffset)
{
  const vector<string>& IDs = ZoneInfoFile::getInstance().getZoneIDs();
  const vector<XLONG>& rawOffsets = ZoneInfoFile::getInstance().getRawOffsets();
  const vector<int>& indices = ZoneInfoFile::getInstance().getRawOffsetIndices();
  
  vector<string> availableIDs;
  for (size_t index = 0; index < rawOffsets.size(); index++) 
  {
    if (rawOffsets[index] == rawOffset) 
    {  
      for (size_t i = 0; i < indices.size(); i++) 
      {
        if (indices[i] == index) 
        {
          availableIDs.push_back(IDs[i++]);
        }

      }//~end for (size_t i = 0; i < indices.length; i++)

    }

  }//~end for (size_t index = 0; index < rawOffsets.size(); index++)
 
  return availableIDs;
}

vector<string> TimeZone::getAvailableIDs()
{
  return ZoneInfoFile::getInstance().getZoneIDs();
}


const TimeZone* TimeZone::getDefault()
{
  return TimeZoneImpl::getInstance().getDefault();
}

void TimeZone::setDefault(const TimeZone* zone)
{
  TimeZoneImpl::getInstance().setDefault(zone);
}

TimeZone*  TimeZone::parseCustomTimeZone(const string& IDx)
{
  size_t length;

  // Error if the length of id isn't long enough or id doesn't
  // start with "GMT".
  if ((length = IDx.length()) < (GMT_ID_LENGTH + 2) || IDx.find_first_of(GMT_ID) != 0)
  {
    return 0;
  }

  ZoneInfo* zi = 0;

  size_t index = GMT_ID_LENGTH;
  bool negative = false;
  char c = IDx[index++];
  if (c == '-')
  {
    negative = true;
  }
  else if (c != '+')
  {
    return 0;
  }

  int hours = 0;
  int num = 0;
  int countDelim = 0;
  int len = 0;
  while (index < length)
  {
    c = IDx[index++];
    if (c == ':')
    {
      if (countDelim > 0)
      {
        return 0;
      }
      if (len > 2)
      {
        return 0;
      }
      hours = num;
      countDelim++;
      num = 0;
      len = 0;
      continue;
    }
    if (c < '0' || c > '9')
    {
      return 0;
    }
    num = num * 10 + (c - '0');
    len++;
  }
  if (index != length)
  {
    return 0;
  }
  if (countDelim == 0)
  {
    if (len <= 2)
    {
      hours = num;
      num = 0;
    }
    else
    {
      hours = num / 100;
      num %= 100;
    }
  }
  else
  {
    if (len != 2)
    {
      return 0;
    }
  }
  if (hours > 23 || num > 59)
  {
    return 0;
  }

  XLONG gmtOffset = hours * 60 + num;

  if(gmtOffset == 0)
  {
    zi = ZoneInfoFile::getInstance().getZoneInfo(GMT_ID);
    
    if(zi != 0)
    {
      if (negative) 
      {
        zi->setID("GMT-00:00");
      } 
      else 
      {
        zi->setID("GMT+00:00");
      }

    }//~end if(zi != 0)

  }
  else
  {
    zi = ZoneInfoFile::getInstance().getCustomTimeZone(IDx, negative ? -gmtOffset : gmtOffset);

  }//~end if(gmtOffset == 0)

  return zi;
}


#if !defined(PORT_WINDOWS)

string __sysux_getPlatformTimeZoneID()
{
	return string();
}
	
string TimeZone::getSystemTimeZoneID(const string& ziHome, const string& country)
{
	char *tz;
	char *javatz = NULL;
	char *freetz = NULL;
	string __tz_id;
	const char* __sys_tz = System::getSystemProperty("TZ");
	if( (__sys_tz == 0) || (*__sys_tz == '\0'))
	{
		__tz_id = __sysux_getPlatformTimeZoneID();
  }	
  else
  {
  	__tz_id = __sys_tz;
  }	
  
	
	if (!__tz_id.empty()) 
	{	
		if (__tz_id[0] == ':') 
		{
		  __tz_id = __tz_id.substr(1);
		}
		
#if defined(PORT_LINUX)
		/** Ignore "posix/" and "right/" prefix. */  
		if(StringsKit::equalsIgnoreCase(__tz_id.data(),"posix/", 6) || StringsKit::equalsIgnoreCase(__tz_id.data(),"right/", 6))
		{
			 __tz_id = __tz_id.substr(6);
	  }	  
#endif
	}	
	
	return __tz_id;
}

string TimeZone::getSystemGMTOffsetID()
{
  XLONG rawOffset = 0;
  
  time_t now = time(0);
  tm localNow;
  OPENCODE_APIHOOK_LOCALTIME(&now,&localNow);
 
  tm utcNow; 
  gmtime_r(&now,&utcNow);

  rawOffset = (XLONG)::difftime(::mktime(&localNow), ::mktime(&utcNow));

  if(rawOffset == 0)
  {
    return string(GMT_ID);
  }

  char buf[16] = {0};
  char sign;
  XLONG offset;
  if(rawOffset >0)
  {
    sign = '+';
    offset = rawOffset; 
  }
  else
  {
    sign = '-';
    offset = -rawOffset;
  }

  sprintf(buf, (const char *)"GMT%c%02d:%02d", sign, (int)(offset/3600), (int)((offset%3600)/60));  

  return string(buf);
}

#endif

__OPENCODE_END_NAMESPACE  
