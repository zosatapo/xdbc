#include "port/zoneInfo.h"
#include "port/gregorianCalendar.h"
#include "port/file.h"
#include "port/ioc_sbuf.h"
#include "port/libsys.h"
#include "port/libnumber.h"
#include "port/ioc_types.h"

#include <algorithm>
#include <vector>

USING_OPENCODE_STL_NS
__OPENCODE_BEGIN_NAMESPACE

//---------------------------------------------------------------------------
/** 
 * The magic number for the ZoneInfo data file format. 
 */ 
const  char ZoneInfoFileImpl::OPENCODEZI_LABEL[] = {(char)'j', (char)'a', (char)'v', (char)'a', (char)'z', (char)'i', (char)'\000'}; 
const  int  ZoneInfoFileImpl::OPENCODEZI_LABEL_LENGTH = 7; 

/** 
 * The ZoneInfoMappings file name. 
 */ 
const  char ZoneInfoFileImpl::OPENCODEZM_FILE_NAME[] = "ZoneInfoMappings"; 
 
/** 
 * The magic number for the ZoneInfoMappings file format. 
 */ 
const  char ZoneInfoFileImpl::OPENCODEZM_LABEL[] = {(char)'j', (char)'a', (char)'v', (char)'a', (char)'z', (char)'m', (char)'\000'}; 
const  int  ZoneInfoFileImpl::OPENCODEZM_LABEL_LENGTH = 7; 
 
 const  char ZoneInfoFileImpl::OPENCODEZI_VERSION = 1; 
   
  /** 
   * Raw offset data item tag. 
  */ 
   const  char ZoneInfoFileImpl::TAG_RawOffset = 1; 
   
  /** 
   * Known last Daylight Saving Time save value data item tag. 
  */ 
   const  char ZoneInfoFileImpl::TAG_LastDSTSaving = 2; 
   
  /** 
   * Checksum data item tag. 
  */ 
   const  char ZoneInfoFileImpl::TAG_CRC32 = 3; 
   
  /** 
   * Transition data item tag. 
  */ 
   const  char ZoneInfoFileImpl::TAG_Transition = 4; 
   
  /** 
   * Offset table data item tag. 
  */ 
   const  char ZoneInfoFileImpl::TAG_Offset = 5; 
   
  /** 
   * SimpleTimeZone parameters data item tag. 
  */ 
   const  char ZoneInfoFileImpl::TAG_SimpleTimeZone = 6; 
   
  /** 
   * Raw GMT offset will change in the future. 
  */ 
   const  char ZoneInfoFileImpl::TAG_GMTOffsetWillChange = 7; 
   
  /** 
   * The ZoneInfoMappings file format version number. 
  */ 
  const  char ZoneInfoFileImpl::OPENCODEZM_VERSION = 1; 
   
  /** 
   * Time zone IDs data item tag. 
  */ 
  const  char ZoneInfoFileImpl::TAG_ZoneIDs = 64; 
   
  /** 
   * Raw GMT offsets table data item tag. 
  */ 
  const  char ZoneInfoFileImpl::TAG_RawOffsets = 65; 
   
  /** 
   * Indices to the raw GMT offset table data item tag. 
  */ 
  const  char ZoneInfoFileImpl::TAG_RawOffsetIndices = 66; 
   
  /** 
   * Time zone aliases table data item tag. 
  */ 
  const  char ZoneInfoFileImpl::TAG_ZoneAliases = 67; 
   
  /** 
   * Olson's zone information version tag. 
  */ 
  const  char ZoneInfoFileImpl::TAG_TZDataVersion = 68; 
//---------------------------------------------------------------------------

IMPLEMENT_LIFETIME_BEGIN(ZoneInfoFileImpl)
IMPLEMENT_LIFETIME_END(ZoneInfoFileImpl)

ZoneInfoFileImpl::ZoneInfoFileImpl()
{
  string buf;
  getZoneInfoMappings(buf);
  
  loadRawOffsets(buf);
  loadZoneIDs(buf);
  loadZoneAliases(buf);
  loadRawOffsetIndices(buf);  
}

string ZoneInfoFileImpl::getFileName(const string& ID) 
{ 
  if (File::separatorChar == '/') 
  { 
      return ID; 
  } 
  //return ID.replace('/', File::separatorChar); 

  return ID;
} 
   

ZoneInfo* ZoneInfoFileImpl::getCustomTimeZone(const string& originalId, XLONG gmtOffset) 
{ 
  char sign; 
  XLONG offset; 
  
  if (gmtOffset >= 0) 
  { 
    sign = '+'; 
    offset = gmtOffset; 
  } 
  else 
  { 
    sign = '-'; 
    offset = -gmtOffset; 
  } 
  
  XLONG hh = offset / 60; 
  XLONG mm = offset % 60; 
  
  StringBuffer sb("GMT"); 
  sb.append(sign); 
  
  if (hh < 10) 
  { 
      sb.append('0'); 
  } 
  sb.append(hh).append(':'); 
  if (mm < 10) 
  { 
      sb.append('0'); 
  } 
  sb.append(mm); 
  string id = sb.toString(); 
 
  //printf("getCustomTimeZone == %lld,%lld\n",gmtOffset,(gmtOffset * 60000)); 
  ZoneInfo* zi = new ZoneInfo(id, gmtOffset * 60000); 
  
  return zi;
} 
   
ZoneInfo* ZoneInfoFileImpl::getZoneInfo(const string& ID) 
{ 
  ZoneInfo* zi = createZoneInfo(ID); 
  return  zi;    
} 
   
const vector<string>& ZoneInfoFileImpl::getZoneIDs() 
{ 
  return zoneIDs; 
} 
   
const OPENCODE_MAP<string,string>& ZoneInfoFileImpl::getZoneAliases() 
{ 
  return zoneAliases; 
} 
   
const vector<int>& ZoneInfoFileImpl::getRawOffsetIndices() 
{ 
  return rawOffsetIndices; 
} 
 
const vector<XLONG>& ZoneInfoFileImpl::getRawOffsets() 
{         
  return rawOffsets;
} 

bool ZoneInfoFileImpl::loadRawOffsets(const string& buf)
{
  size_t index = OPENCODEZM_LABEL_LENGTH + 1; 
  size_t filesize = buf.size(); 
 
  while (index < filesize) 
  { 
    char tag = buf[index++]; 
    
    int len = ((buf[index++] & 255) << 8) + (buf[index++] & 255); 
    
    switch (tag) 
    {
      case TAG_RawOffsets: 
      { 
        int n = len / 4; 
        rawOffsets.resize(n);
         
        for (int i = 0; i < n; i++) 
        { 
          int val = buf[index++] & 255; 
          val = (val << 8) + (buf[index++] & 255); 
          val = (val << 8) + (buf[index++] & 255); 
          val = (val << 8) + (buf[index++] & 255); 
          rawOffsets[i] = val; 
        } 
      } 
      return true; 
      
      default: 
      {
        index += len; 
      }
      break;     
    } 
  } 
  
  return true; 
}

bool ZoneInfoFileImpl::loadZoneIDs(const string& buf)
{
  size_t index = OPENCODEZM_LABEL_LENGTH + 1; 
  size_t filesize = buf.size(); 
 
  while (index < filesize) 
  { 
    char tag = buf[index++]; 
    
    int len = ((buf[index++] & 255) << 8) + (buf[index++] & 255); 
    
    switch (tag) 
    {
      case TAG_ZoneIDs: 
      { 
        int n = (buf[index++] << 8) + (buf[index++] & 255); 
        zoneIDs.resize(n);
        for (int i = 0; i < n; i++) 
        { 
          char m = buf[index++]; 
          zoneIDs[i] = string(buf, index, m); 
          index += m; 
        } 
      } 
      return true; 
      
      default: 
      {
        index += len; 
      }
      break;     
    } 
  } 
  
  return true;    
}

bool ZoneInfoFileImpl::loadZoneAliases(const string& buf)
{
  size_t index = OPENCODEZM_LABEL_LENGTH + 1; 
  size_t filesize = buf.size(); 

  while (index < filesize) 
  { 
    char tag = buf[index++]; 
    
    int len = ((buf[index++] & 255) << 8) + (buf[index++] & 255); 

    switch (tag) 
    {
      case TAG_ZoneAliases: 
      { 
        int n = (buf[index++] << 8) + (buf[index++] & 255); 
        
        for (int i = 0; i < n; i++) 
        { 
          char m = buf[index++]; 
          string name(buf, index, m); 
          index += m; 
          m = buf[index++]; 
          string realName(buf, index, m); 
          index += m; 
          zoneAliases[name]= realName;
        }//~end for (int i = 0; i < n; i++)

      }          
      return true; 
      
      default: 
      {
        index += len; 
      }
      break;     

    }//~end switch (tag) 

  }//~end while (index < filesize) 
  
  return true;    
}

bool ZoneInfoFileImpl::loadRawOffsetIndices(const string& buf)
{
  size_t index = OPENCODEZM_LABEL_LENGTH + 1; 
  size_t filesize = buf.size(); 
 
  while (index < filesize) 
  { 
    char tag = buf[index++]; 
    
    int len = ((buf[index++] & 255) << 8) + (buf[index++] & 255); 
    
    switch (tag) 
    {
      case TAG_RawOffsetIndices: 
      { 
        rawOffsetIndices.resize(len); 
        for (int i = 0; i < len; i++) 
        { 
          rawOffsetIndices[i] = buf[index++]; 
        } 
      }  
      return true; 
      
      default: 
      {
        index += len; 
      }
      break;     
    } 
  } 
  
  return true;
}

ZoneInfo* ZoneInfoFileImpl::createZoneInfo(const string& ID) 
{ 
  string buf;
  readZoneInfoFileImpl(getFileName(ID),buf); 

  if (buf.empty()) 
  { 
    return 0; 
  } 
  
  int index; 
  for (index = 0; index < OPENCODEZI_LABEL_LENGTH; index++) 
  { 
    if (buf[index] != OPENCODEZI_LABEL[index]) 
    { 
      LOGGING_WARN("ZoneInfoFileImpl::createZoneInfo|wrong magic number: {" << ID <<"}"); 
      return 0; 
    } 
  } 
  
  if (buf[index++] > OPENCODEZI_VERSION) 
  { 
    LOGGING_WARN("ZoneInfoFileImpl::createZoneInfo|incompatible version ("<< buf[index - 1] <<"): {" << ID <<"}");
    return 0; 
  } 
  
  XLONG filesize = buf.size(); 
  XLONG  rawOffset = 0; 
  XLONG  dstSavings = 0; 
  XLONG checksum = 0; 
  bool willGMTOffsetChange = false; 
  
  vector<XLONG> transitions;
  vector<XLONG> offsets;
  vector<XLONG> simpleTimeZoneParams;
    
  while (index < filesize) 
  { 
    char tag = buf[index++]; 
    short len = ((buf[index++] & 255) << 8) + (buf[index++] & 255); 
    if (filesize < index + len) 
    { 
      break; 
    } 
    
    switch (tag) 
    {   
      case TAG_CRC32:
      {
        int val = buf[index++] & 0xff;
        val = (val << 8) + (buf[index++] & 0xff);
        val = (val << 8) + (buf[index++] & 0xff);
        val = (val << 8) + (buf[index++] & 0xff);
        checksum = val;
      }
      break;

      case TAG_LastDSTSaving: 
      { 
        short val = (short)(buf[index++] & 255); 
        val = (short)((val << 8) + (buf[index++] & 255)); 
        dstSavings = val * 1000; 
      } 
      break; 
      
      case TAG_RawOffset: 
      { 
        int val = buf[index++] & 255; 
        val = (val << 8) + (buf[index++] & 255); 
        val = (val << 8) + (buf[index++] & 255); 
        val = (val << 8) + (buf[index++] & 255); 
        rawOffset = val; 
      } 
      break; 
      
      case TAG_Transition: 
      { 
        int n = len / 8; 
        transitions.resize(n); 
        for (int i = 0; i < n; i++) 
        { 
          XINT64 val = buf[index++] & 255; 
          val = (val << 8) + (buf[index++] & 255); 
          val = (val << 8) + (buf[index++] & 255); 
          val = (val << 8) + (buf[index++] & 255); 
          val = (val << 8) + (buf[index++] & 255); 
          val = (val << 8) + (buf[index++] & 255); 
          val = (val << 8) + (buf[index++] & 255); 
          val = (val << 8) + (buf[index++] & 255); 
          transitions[i] = val; 
        } 
      } 
      break; 
      
      case TAG_Offset: 
      { 
        int n = len / 4; 
        offsets.resize(n);
        for (int i = 0; i < n; i++) 
        { 
          int val = buf[index++] & 255; 
          val = (val << 8) + (buf[index++] & 255); 
          val = (val << 8) + (buf[index++] & 255); 
          val = (val << 8) + (buf[index++] & 255); 
          offsets[i] = val; 
        } 
      } 
      break; 
      
      case TAG_SimpleTimeZone: 
      { 
        if (len != 32 && len != 40) 
        { 
          LOGGING_WARN("ZoneInfoFileImpl::createZoneInfo|wrong SimpleTimeZone parameter size"); 
          return 0; 
        } 
        
        int n = len / 4; 
        simpleTimeZoneParams.resize(n); 
        for (int i = 0; i < n; i++) 
        { 
          int val = buf[index++] & 255; 
          val = (val << 8) + (buf[index++] & 255); 
          val = (val << 8) + (buf[index++] & 255); 
          val = (val << 8) + (buf[index++] & 255); 
          simpleTimeZoneParams[i] = val; 
        } 
      } 
      break; 
      
      case TAG_GMTOffsetWillChange: 
      { 
        if (len != 1) 
        { 
          LOGGING_WARN("ZoneInfoFileImpl::createZoneInfo|wrong char length for TAG_GMTOffsetWillChange"); 
        } 
        willGMTOffsetChange = buf[index++] == 1; 
      } 
      break; 
      
      default: 
      {
        LOGGING_WARN("ZoneInfoFileImpl::createZoneInfo|unknown tag <"<< tag <<">. ignored."); 
        index += len; 
      }
      break; 
    
    } //~end switch (tag) 
  
  } //~end while (index < filesize) 
  
  if (index != filesize) 
  { 
    LOGGING_WARN("ZoneInfoFileImpl::createZoneInfo|wrong file size: " << ID); 
    return 0; 
  } 
  
  return new ZoneInfo(ID, rawOffset, dstSavings, checksum, transitions, offsets, simpleTimeZoneParams, willGMTOffsetChange); 
} 


/** 
 * Reads the specified file under <zoneinfo.home> into a buffer. 
 * @return the buffer, or null if any I/O error occurred. 
*/ 
bool ZoneInfoFileImpl::readZoneInfoFileImpl(const string& fileName,string& buf) 
{ 
  if(fileName.empty())
  {
    return false;
  }

  string homeDir = System::getProperty("zoneinfo.home");
  if(homeDir.empty())
  {
    homeDir = System::getProperty("ZONEINFO_HOME",".");
  }

  string fname = homeDir + File::separator + fileName; 

  FILE* fp = fopen(fname.c_str(),"r");
  if(fp == 0)
  {
    return false;
  }

  fseek(fp,0,SEEK_END);
  long filesize = ftell(fp);
  buf.resize(filesize);

  fseek(fp,0,SEEK_SET);
  fread((char*)buf.data(),filesize,1,fp);
  fclose(fp);
  
  return true; 
} 

bool ZoneInfoFileImpl::getZoneInfoMappings(string& buf) 
{ 
  return readZoneInfoFileImpl(OPENCODEZM_FILE_NAME,buf); 
} 

const XLONG ZoneInfo::OFFSET_MASK = 0x0fL;
const XLONG ZoneInfo::DST_MASK = 0xf0L;
const XLONG ZoneInfo::DST_NSHIFT = 4;

const XLONG ZoneInfo::ABBR_MASK = 0xf00L;
const XLONG ZoneInfo::TRANSITION_NSHIFT = 12;
  
TimeZone* ZoneInfo::doClone() const
{
  return new ZoneInfo(*this);
}

string ZoneInfo::toString() const
{
  StringBuffer sb("ZoneInfo");

  sb << "[id=\"" << getID() << "\""
     << ",offset=" << getLastRawOffset()
     << ",dstSavings=" << dstSavings
     << ",useDaylight=" << useDaylightTime()
     << ",transitions=" << ((transitions.size() != 0) ? transitions.size() : 0)
     << ",lastRule=" ;


  SimpleTimeZone* stz = getLastRuleInstance();
  if(stz != 0)
  {
    sb<< stz->toString();
    delete stz;
  }
  else
  {
    sb << "?"; 
  }

  sb << "]";

  return sb.toString();
}

ZoneInfo* ZoneInfo::getZoneInfo(const string& aID)
{
  return ZoneInfoFile::getInstance().getZoneInfo(aID);
}

ZoneInfo::~ZoneInfo() 
{
  if(lastRule != 0)
  {
    delete lastRule;
  }
}

ZoneInfo::ZoneInfo(const ZoneInfo& rv)
:TimeZone(rv)
{
  this->rawOffset = rv.rawOffset;
  this->rawOffsetDiff = rv.rawOffsetDiff;
  this->dstSavings = rv.dstSavings;
  this->checksum = rv.checksum;
  this->transitions = rv.transitions;
  this->offsets = rv.offsets;
  this->simpleTimeZoneParams = rv.simpleTimeZoneParams;
  this->willGMTOffsetChange = rv.willGMTOffsetChange;

  if(rv.lastRule != 0)
  {
    this->lastRule = (SimpleTimeZone*)(rv.lastRule->clone());
  }
  else
  {
    this->lastRule = 0;  
  }
}

ZoneInfo& ZoneInfo::operator=(const ZoneInfo& rv)
{
  if(&rv == this)
  {
    return *this;
  }
  
  TimeZone::operator=(rv);

  this->rawOffset = rv.rawOffset;
  this->rawOffsetDiff = rv.rawOffsetDiff;
  this->dstSavings = rv.dstSavings;
  this->checksum = rv.checksum;
  this->transitions = rv.transitions;
  this->offsets = rv.offsets;
  this->simpleTimeZoneParams = rv.simpleTimeZoneParams;
  this->willGMTOffsetChange = rv.willGMTOffsetChange;

  if(this->lastRule != 0)
  {
    delete this->lastRule;
    this->lastRule = 0;
  }

  if(rv.lastRule != 0)
  {
    this->lastRule = (SimpleTimeZone*)(rv.lastRule->clone());
  }

  return *this;
}

ZoneInfo::ZoneInfo(const string& ID, XLONG rawOffsetValue) 
:TimeZone(ID)
{
  this->rawOffset = rawOffsetValue;
  this->rawOffsetDiff = 0; 
  this->dstSavings = 0;
  this->checksum = 0;
  
  this->willGMTOffsetChange = false;

  this->lastRule = 0;
}

ZoneInfo::ZoneInfo(const string& IDValue,
   XLONG rawOffsetValue,
   XLONG dstSavingsValue,
   XLONG checksumValue,
   const vector<XLONG>& transitionsValue,
   const vector<XLONG>& offsetsValue,
   const vector<XLONG>& simpleTimeZoneParamsValue,
   bool willGMTOffsetChangeValue) 
:TimeZone(IDValue)   
{
  this->rawOffset = rawOffsetValue;
  this->rawOffsetDiff = 0;
  this->dstSavings = dstSavingsValue;
  this->checksum = checksumValue;
  this->transitions = transitionsValue;
  this->offsets = offsetsValue;
  this->simpleTimeZoneParams = simpleTimeZoneParamsValue;
  this->willGMTOffsetChange = willGMTOffsetChangeValue;

  this->lastRule = 0;
}


XLONG ZoneInfo::getOffset(XLONG date) const
{
  return getOffsets(date, 0, false);
}

XLONG ZoneInfo::getOffsets(XLONG utc, vector<XLONG>* offsetsValue)  const 
{
  return getOffsets(utc, offsetsValue, false);
}

XLONG ZoneInfo::getOffsetsByWall(XLONG wall, vector<XLONG>* offsetsValue)   const
{
  return getOffsets(wall, offsetsValue, true);
}


XLONG ZoneInfo::getOffset(XLONG era, XLONG year, XLONG month, XLONG day,XLONG dayOfWeek, XLONG milliseconds)  const 
{
  if (milliseconds < 0 || milliseconds >= GregorianSystem::MILLIS_PER_DAY) 
  {
    System::loggingAndExit("ZoneInfo::getOffset|milliseconds < 0 OR milliseconds >= GregorianSystem::MILLIS_PER_DAY",-1);
  }

  if (era == GregorianSystem::BC) 
  { // BC
    year = 1 - year;
  } 
  else if (era != GregorianSystem::AD) 
  {
    System::loggingAndExit("ZoneInfo::getOffset|era != GregorianSystem::AD",-1);
  }
  
  CalendarDate date((int)year, (int)month, (int)day);
  if (!GregorianSystem::validate(date)) 
  {
    System::loggingAndExit("ZoneInfo::getOffset|!GregorianSystem::validate(date)",-1);
  }
  
  // bug-for-bug compatible argument checking
  if (dayOfWeek < GregorianSystem::SUNDAY || dayOfWeek > GregorianSystem::SATURDAY) 
  {
    System::loggingAndExit("ZoneInfo::getOffset|dayOfWeek < GregorianSystem::SUNDAY OR dayOfWeek > GregorianSystem::SATURDAY",-1);
  }

  if (transitions.empty()) 
  {
    return getLastRawOffset();
  }

  XLONG dateInMillis = GregorianSystem::dateToMillis(date) + milliseconds;
  dateInMillis -= (XLONG) rawOffset; // make it UTC
  return getOffsets(dateInMillis, 0, false);
}

void ZoneInfo::setRawOffset(XLONG offsetMillis)
{
  rawOffsetDiff = offsetMillis - rawOffset;
  if (lastRule != 0) 
  {
    lastRule->setRawOffset(offsetMillis);
  }
}

/**
 * Returns the GMT offset of the current date. This GMT offset
 * value is not modified during Daylight Saving Time.
 *
 * @return the GMT offset value in milliseconds to add to UTC time
 * to get local standard time
 */
XLONG ZoneInfo::getRawOffset()  const
{
  if (!willGMTOffsetChange) 
  {
    return rawOffset + rawOffsetDiff;
  }

  vector<XLONG> offsetsValue(2);  
  getOffsets(System::currentTimeMillis(), &offsetsValue, false);
  
  return offsetsValue[0];
}

XLONG ZoneInfo::getLastRawOffset() const
{
  //printf("ZoneInfo::getLastRawOffset == %lld,%lld\n",rawOffset,rawOffsetDiff);
  return rawOffset + rawOffsetDiff;
}

bool ZoneInfo::useDaylightTime()  const
{
  return (!simpleTimeZoneParams.empty());
}

bool ZoneInfo::inDaylightTime(XLONG date)  const
{
  if (transitions.empty()) 
  {
    return false;
  }

  XLONG utc = date - rawOffsetDiff;

  int index = getTransitionIndex(utc, false);
  
  // before transitions in the transition table
  if (index < 0) 
  {
    return false;
  }

  // the time is in the table range.
  if (index < transitions.size()) 
  {
    return (transitions[index] & DST_MASK) != 0;
  }

  // beyond the transition table
  const SimpleTimeZone* tz = getLastRule();
  if (tz != 0) 
  {
    return tz->inDaylightTime(date);
  }

  return false;
}

XLONG ZoneInfo::getDSTSavings() const 
{
  return dstSavings;
}

const SimpleTimeZone*  ZoneInfo::getLastRule() const 
{
  if (lastRule == 0) 
  {
    lastRule = getLastRuleInstance();
  }

  return lastRule;
}

SimpleTimeZone* ZoneInfo::getLastRuleInstance() const
{
  if (simpleTimeZoneParams.empty()) 
  {
    return 0;
  }
  
  if (simpleTimeZoneParams.size() == 10) 
  {
    return new SimpleTimeZone(getLastRawOffset(),getID(),
            simpleTimeZoneParams[0],
            simpleTimeZoneParams[1],
            simpleTimeZoneParams[2],
            simpleTimeZoneParams[3],
            simpleTimeZoneParams[4],
            simpleTimeZoneParams[5],
            simpleTimeZoneParams[6],
            simpleTimeZoneParams[7],
            simpleTimeZoneParams[8],
            simpleTimeZoneParams[9],
            dstSavings);
  }
  
  return new SimpleTimeZone(getLastRawOffset(), getID(),
        simpleTimeZoneParams[0],
        simpleTimeZoneParams[1],
        simpleTimeZoneParams[2],
        simpleTimeZoneParams[3],
        simpleTimeZoneParams[4],
        simpleTimeZoneParams[5],
        simpleTimeZoneParams[6],
        simpleTimeZoneParams[7],
        dstSavings);
}

bool ZoneInfo::hasSameRules(const TimeZone* rv) const
{
  if (rv == this) 
  {
    return true;
  }
  
  if (rv == 0) 
  {
    return false;
  }
 
  const ZoneInfo* zi = (const ZoneInfo*)(rv);

  if (zi == 0) 
  {
    if (getRawOffset() != rv->getRawOffset()) 
    {
      return false;
    }
    
    // if both have the same raw offset and neither observes
    // DST, they have the same rule.
    if ((transitions.empty())&& (!useDaylightTime())&& (!rv->useDaylightTime())) 
    {
      return true;
    }
    
    return false;
  }
  
  if (getLastRawOffset() != zi->getLastRawOffset()) 
  {
    return false;
  }
  
  return (checksum == zi->checksum);
}

XLONG ZoneInfo::getOffsets(XLONG date, vector<XLONG>* offsetsValue, bool isWall) const 
{
  // if dst is never observed, there is no transition.
  if (transitions.empty()) 
  {
    XLONG offset = getLastRawOffset();
    if (offsetsValue != 0) 
    {
      (*offsetsValue)[0] = offset;
      (*offsetsValue)[1] = 0;
    }
    return offset;
  }

  date -= rawOffsetDiff;
  int index = getTransitionIndex(date, isWall);

  // prior to the transition table, returns the raw offset.
  // should support LMT.
  if (index < 0) 
  {
    XLONG offset = getLastRawOffset();
    if (offsetsValue != 0) 
    {
      (*offsetsValue)[0] = offset;
      (*offsetsValue)[1] = 0;
    }
    return offset;
  }

  if (index < transitions.size()) 
  {
    XLONG val = transitions[index];
    XLONG offset = this->offsets[(int)(val & OFFSET_MASK)] + rawOffsetDiff;
    
    if (offsetsValue != 0) 
    {
      XLONG dst = ( Number::ushiftRight(val ,(int)DST_NSHIFT) & 0xfL);
      
      XLONG save = (dst == 0) ? 0 : this->offsets[dst];
      (*offsetsValue)[0] = offset - save;
      (*offsetsValue)[1] = save;
    }
    return offset;
  }

  // beyond the transitions, delegate to SimpleTimeZone if there
  // is a rule; otherwise, return rawOffset.
  const SimpleTimeZone* tz = getLastRule();
  if (tz != 0) 
  {
    XLONG rawoffset = tz->getRawOffset();
    
    XLONG msec = date;
    
    if (isWall) 
    {
      msec -= rawOffset;
    }
    XLONG dstoffset = tz->inDaylightTime(msec) ? tz->getDSTSavings() : 0;
    if (offsetsValue != 0) 
    {
      (*offsetsValue)[0] = rawoffset;
      (*offsetsValue)[1] = dstoffset;
    }
    return rawoffset + dstoffset;
  }
  
  XLONG offset = getLastRawOffset();
  if (offsetsValue != 0) 
  {
    (*offsetsValue)[0] = offset;
    (*offsetsValue)[1] = 0;
  }
  return offset;
}

int ZoneInfo::getTransitionIndex(XLONG date, bool isWall) const 
{
  int low = 0;
  int high = (int)(transitions.size() - 1);
  
  while (low <= high)
  {
    int mid = (low + high) / 2;
    XLONG val = transitions[mid];
    XLONG midVal = val >> TRANSITION_NSHIFT;
    if (isWall) 
    {
      midVal += offsets[(int)(val & OFFSET_MASK)]; // wall time
    }
  
    if (midVal < date) 
    {
      low = mid + 1;
    } 
    else if (midVal > date) 
    {
      high = mid - 1;
    } else 
    {
      return mid;
    }
  }

  // if beyond the transitions, returns that index.
  if (low >= transitions.size()) 
  {
    return low;
  }
  return low - 1;
}

__OPENCODE_END_NAMESPACE
