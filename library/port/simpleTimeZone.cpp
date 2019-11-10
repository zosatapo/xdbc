#include "port/simpleTimeZone.h"
#include "port/gregorianCalendar.h"
#include "port/ioc_sbuf.h"
#include "port/libsys.h"

__OPENCODE_BEGIN_NAMESPACE

const XLONG SimpleTimeZone::DOM_MODE = 1; // Exact day of month, "Mar 1"
const XLONG SimpleTimeZone::DOW_IN_MONTH_MODE = 2; // Day of week in month, "lastSun"
const XLONG SimpleTimeZone::DOW_GE_DOM_MODE = 3; // Day of week after day of month, "Sun>=15"
const XLONG SimpleTimeZone::DOW_LE_DOM_MODE = 4; // Day of week before day of month, "Sun<=21"

const XLONG SimpleTimeZone::WALL_TIME = 0; // Zero for backward compatibility

const XLONG SimpleTimeZone::STANDARD_TIME = 1;

const XLONG SimpleTimeZone::UTC_TIME = 2;  

TimeZone* SimpleTimeZone::doClone() const
{
  return new SimpleTimeZone(*this);
}

string  SimpleTimeZone::toString() const
{
  StringBuffer sb("SimpleTimeZone");

  sb << "[id="  <<  getID()
     << ",offset="  <<  rawOffset
     << ",dstSavings="  <<  dstSavings
     << ",useDaylight="  <<  useDaylight
     << ",startYear="  <<  startYear
     << ",startMode="  <<  startMode
     << ",startMonth="  <<  startMonth
     << ",startDay="  <<  startDay
     << ",startDayOfWeek="  <<  startDayOfWeek
     << ",startTime="  <<  startTime
     << ",startTimeMode="  <<  startTimeMode
     << ",endMode="  <<  endMode
     << ",endMonth="  <<  endMonth
     << ",endDay="  <<  endDay
     << ",endDayOfWeek="  <<  endDayOfWeek
     << ",endTime="  <<  endTime
     << ",endTimeMode="  <<  endTimeMode  <<  ']';

  return sb.toString();
}

SimpleTimeZone::~SimpleTimeZone()
{
}

SimpleTimeZone::SimpleTimeZone(const SimpleTimeZone& rhs)
:TimeZone(rhs)
{
  startMonth = rhs.startMonth;
  startDay = rhs.startDay;
  startDayOfWeek = rhs.startDayOfWeek;
  startTime = rhs.startTime;
  startTimeMode = rhs.startTimeMode;
  endMonth = rhs.endMonth;

  endDay = rhs.endDay;

  endDayOfWeek = rhs.endDayOfWeek;
  endTime = rhs.endTime;
  endTimeMode = rhs.endTimeMode;
  startYear = rhs.startYear;

  rawOffset = rhs.rawOffset;

  useDaylight = rhs.useDaylight;
  startMode = rhs.startMode;

  endMode = rhs.endMode;

  dstSavings = rhs.dstSavings;
}

SimpleTimeZone& SimpleTimeZone::operator=(const SimpleTimeZone& rhs)
{
  if(&rhs == this)
  {
    return *this;
  }

  TimeZone::operator=(rhs);

  startMonth = rhs.startMonth;
  startDay = rhs.startDay;
  startDayOfWeek = rhs.startDayOfWeek;
  startTime = rhs.startTime;
  startTimeMode = rhs.startTimeMode;
  endMonth = rhs.endMonth;

  endDay = rhs.endDay;

  endDayOfWeek = rhs.endDayOfWeek;
  endTime = rhs.endTime;
  endTimeMode = rhs.endTimeMode;
  startYear = rhs.startYear;

  rawOffset = rhs.rawOffset;

  useDaylight = rhs.useDaylight;
  startMode = rhs.startMode;

  endMode = rhs.endMode;

  dstSavings = rhs.dstSavings;

  return *this;
}

SimpleTimeZone::SimpleTimeZone(XLONG rawOffsetValue, const string& IDValue)
:TimeZone(IDValue)
{
  this->rawOffset = rawOffsetValue;

  dstSavings = CalendarSystem::MILLIS_PER_HOUR; // In case user sets rules later

  startYear = 0;

  startMonth = 0;
  startDay = 0;
  startDayOfWeek = 0;
  startTime = 0;
  startTimeMode = 0;
  startMode = 0;

  endMonth = 0;
  endDay = 0;
  endDayOfWeek = 0;
  endTime = 0;
  endTimeMode = 0;
  endMode = 0;

  useDaylight = false;
}

SimpleTimeZone::SimpleTimeZone(
  XLONG rawOffsetValue,
  const string& IDValue,
  XLONG startMonthValue,
  XLONG startDayValue,
  XLONG startDayOfWeekValue,
  XLONG startTimeValue,
  XLONG endMonthValue,
  XLONG endDayValue,
  XLONG endDayOfWeekValue,
  XLONG endTimeValue,
  XLONG dstSavingsValue)
:TimeZone(IDValue)
{
  XLONG xstartTimeMode = WALL_TIME;
  XLONG xendTimeMode = WALL_TIME;

  if (endMonthValue == CalendarSystem::JANUARY
    && endDayValue == 1
    && endDayOfWeekValue == 0
    && endTimeValue == 0
    && xendTimeMode == WALL_TIME
    && dstSavingsValue > 0)
  {
    endMonthValue = CalendarSystem::DECEMBER;
    endDayValue = 31;
    endTimeValue = ((24 * 60 * 60 * 1000) - dstSavings);
    xendTimeMode = STANDARD_TIME;
  }

  this->rawOffset = rawOffsetValue;
  this->startMonth = startMonthValue;
  this->startDay = startDayValue;
  this->startDayOfWeek = startDayOfWeekValue;
  this->startTime = startTimeValue;
  this->startTimeMode = xstartTimeMode;
  this->endMonth = endMonthValue;
  this->endDay = endDayValue;
  this->endDayOfWeek = endDayOfWeekValue;
  this->endTime = endTimeValue;
  this->endTimeMode = xendTimeMode;
  this->dstSavings = dstSavingsValue;

  // this->useDaylight is set by decodeRules
  decodeRules();
}

SimpleTimeZone::SimpleTimeZone(
  XLONG rawOffsetValue,
  const string& IDValue,
  XLONG startMonthValue,
  XLONG startDayValue,
  XLONG startDayOfWeekValue,
  XLONG startTimeValue,
  XLONG startTimeModeValue,
  XLONG endMonthValue,
  XLONG endDayValue,
  XLONG endDayOfWeekValue,
  XLONG endTimeValue,
  XLONG endTimeModeValue,
  XLONG dstSavingsValue)
:TimeZone(ID)
{
  if (endMonthValue == CalendarSystem::JANUARY
    && endDayValue == 1
    && endDayOfWeekValue == 0
    && endTimeValue == 0
    && endTimeModeValue == WALL_TIME
    && dstSavingsValue > 0)
  {
    endMonthValue = CalendarSystem::DECEMBER;
    endDayValue = 31;
    endTimeValue = ((24 * 60 * 60 * 1000) - dstSavings);
    endTimeModeValue = STANDARD_TIME;
  }

  this->rawOffset = rawOffsetValue;
  this->startMonth = startMonthValue;
  this->startDay = startDayValue;
  this->startDayOfWeek = startDayOfWeekValue;
  this->startTime = startTimeValue;
  this->startTimeMode = startTimeModeValue;
  this->endMonth = endMonthValue;
  this->endDay = endDayValue;
  this->endDayOfWeek = endDayOfWeekValue;
  this->endTime = endTimeValue;
  this->endTimeMode = endTimeModeValue;
  this->dstSavings = dstSavingsValue;

  // this->useDaylight is set by decodeRules
  decodeRules();
}

void SimpleTimeZone::setStartYear(XLONG yearValue)
{
  startYear = yearValue;
}

void  SimpleTimeZone::setStartRule(XLONG startMonthValue, XLONG startDayValue, XLONG startDayOfWeekValue, XLONG startTimeValue)
{
  this->startMonth = startMonthValue;
  this->startDay = startDayValue;
  this->startDayOfWeek = startDayOfWeekValue;
  this->startTime = startTimeValue;
  this->startTimeMode = WALL_TIME;
  decodeStartRule();
}

void SimpleTimeZone::setStartRule(XLONG startMonthValue, XLONG startDayValue, XLONG startTimeValue)
{
  setStartRule(startMonthValue, startDayValue, 0, startTimeValue);
}

void SimpleTimeZone::setStartRule(XLONG startMonthValue, XLONG startDayValue, XLONG startDayOfWeekValue, XLONG startTimeValue, bool after)
{
  // TODO: this method doesn't check the initial values of dayOfMonth or dsyOfWeek.
  if (after)
  {
    setStartRule(startMonthValue, startDayValue, -startDayOfWeekValue, startTimeValue);
  }
  else
  {
    setStartRule(startMonthValue, -startDayValue, -startDayOfWeekValue, startTimeValue);
  }
}

void SimpleTimeZone::setEndRule(XLONG endMonthValue, XLONG endDayValue, XLONG endDayOfWeekValue, XLONG endTimeValue)
{
  this->endMonth = endMonthValue;
  this->endDay = endDayValue;
  this->endDayOfWeek = endDayOfWeekValue;
  this->endTime = endTimeValue;
  this->endTimeMode = WALL_TIME;
  // useDaylight = true; // Set by decodeRules
  decodeEndRule();
}


void SimpleTimeZone::setEndRule(XLONG endMonthValue, XLONG endDayValue, XLONG endTimeValue)
{
  setEndRule(endMonthValue, endDayValue, 0, endTimeValue);
}

void SimpleTimeZone::setEndRule(XLONG endMonthValue, XLONG endDayValue, XLONG endDayOfWeekValue, XLONG endTimeValue, bool after)
{
  if (after)
  {
    setEndRule(endMonthValue, endDayValue, -endDayOfWeekValue, endTimeValue);
  }
  else
  {
    setEndRule(endMonthValue, -endDayValue, -endDayOfWeek, endTimeValue);
  }
}

XLONG SimpleTimeZone::getOffset(XLONG date) const
{
  return getOffsets(date, 0);
}

XLONG SimpleTimeZone::getOffsets(XLONG date, vector<XLONG>* offsetsValue)const
{
  XLONG offsetValue;

  if (!useDaylight)
  {
    offsetValue = rawOffset;
  }

  // get standard local time
  CalendarDate cdate = GregorianSystem::getCalendarDate(date + rawOffset);

  int year = cdate.getYear();
  // if it's BC, assume no DST.
  if (year <= 0)
  {
    offsetValue = rawOffset;
  }
  int month = cdate.getMonth();
  int monthLength = GregorianSystem::MONTH_LENGTH[month];
  int prevMonthLength = GregorianSystem::MONTH_LENGTH[(month + CalendarSystem::DECEMBER) % 12];
  if (GregorianSystem::isLeapYear(year))
  {
    if (month == CalendarSystem::FEBRUARY)
    {
      ++monthLength;
    }
    else if (month == CalendarSystem::MARCH)
    {
      ++prevMonthLength;
    }
  }

  //printf("--offsets : %p , rawOffset = %lld,(offset - rawOffset) = %lld\n",offsets,rawOffset,(offset - rawOffset));
  offsetValue = getOffset( GregorianSystem::AD, year, month, cdate.getDate(), cdate.getDayOfWeek(),
                      cdate.getTimeOfDay(), monthLength, prevMonthLength);

  if(offsetsValue != 0)
  {
    if(offsetsValue->size() <2)
    {
      offsetsValue->resize(2);
    }

    (*offsetsValue)[0]=(rawOffset);
    (*offsetsValue)[1]=(offsetValue - rawOffset);
  }

  return offsetValue;
}

XLONG SimpleTimeZone::getOffset(XLONG era, XLONG year, XLONG month, XLONG day, XLONG dayOfWeek, XLONG millis) const
{
  // Check the month before indexing into staticMonthLength. This
  // duplicates the test that occurs in the 7-argument getOffset(),
  // however, this is unavoidable. We don't mind because this method, in
  // fact, should not be called; internal code should always call the
  // 7-argument getOffset(), and outside code should use Calendar::get(int
  // field) with fields ZONE_OFFSET and DST_OFFSET. We can't get rid of
  // this method because it's public API. - liu 8/10/98
  if (month < CalendarSystem::JANUARY || month > CalendarSystem::DECEMBER)
  {
    StringBuffer sb("SimpleTimeZone::getOffset|Illegal month ");
    System::loggingAndExit(sb.append(month).str(),-1);  
  }

  XLONG monthLength, prevMonthLength;
  if ((era == GregorianSystem::AD) && GregorianSystem::isLeapYear((int)year))
  {
    monthLength = GregorianSystem::LEAP_MONTH_LENGTH[month];
    prevMonthLength = (month > 1) ? GregorianSystem::LEAP_MONTH_LENGTH[month - 1] : 31;
  }
  else
  {
    monthLength = GregorianSystem::MONTH_LENGTH[month];
    prevMonthLength = (month > 1) ? GregorianSystem::MONTH_LENGTH[month - 1] : 31;
  }


  /* This parameter checking code is better for debugging, but
   * overkill for normal operation.  Only one of these two blocks
   * should actually get compiled into the class file.  */
  if (era != GregorianSystem::AD && era != GregorianSystem::BC)
  {
    StringBuffer sb("SimpleTimeZone::getOffset|Illegal era ");
    System::loggingAndExit(sb.append(era).str(),-1);  
  }
  if (month < CalendarSystem::JANUARY || month > CalendarSystem::DECEMBER)
  {
    StringBuffer sb("SimpleTimeZone::getOffset|Illegal era ");
    System::loggingAndExit(sb.append(month).str(),-1);  
  }
  if (day < 1 || day > monthLength)
  {
    StringBuffer sb("SimpleTimeZone::getOffset|Illegal day ");
    System::loggingAndExit(sb.append(day).str(),-1);  
  }
  if (dayOfWeek < CalendarSystem::SUNDAY || dayOfWeek > CalendarSystem::SATURDAY)
  {
    StringBuffer sb("SimpleTimeZone::getOffset|Illegal day of week ");
    System::loggingAndExit(sb.append(dayOfWeek).str(),-1);  
  }
  if (millis < 0 || millis >= CalendarSystem::MILLIS_PER_DAY)
  {
    StringBuffer sb("SimpleTimeZone::getOffset|Illegal millis ");
    System::loggingAndExit(sb.append(millis).str(),-1);  
  }

  return getOffset(era, year, month, day, dayOfWeek, millis, monthLength, prevMonthLength);
}

XLONG SimpleTimeZone::getOffset(
    XLONG era,
    XLONG year,
    XLONG month,
    XLONG day,
    XLONG dayOfWeek,
    XLONG millis,
    XLONG monthLength,
    XLONG prevMonthLength) const
{
  XLONG result = rawOffset;

  // Bail out if we are before the onset of daylight saving time
  if (!useDaylight || year < startYear || era != GregorianSystem::AD)
  {
    return result;
  }

  // Check for southern hemisphere.  We assume that the start and end
  // month are different.
  bool southern = (startMonth > endMonth);

  // Compare the date to the starting and ending rules.+1 = date>rule, -1
  // = date<rule, 0 = date==rule.
  int startCompare =
    compareToRule(
      month,
      monthLength,
      prevMonthLength,
      day,
      dayOfWeek,
      millis,
      startTimeMode == UTC_TIME ? -rawOffset : 0,
      startMode,
      startMonth,
      startDayOfWeek,
      startDay,
      startTime);
  int endCompare = 0;

  /* We don't always have to compute endCompare.  For many instances,
   * startCompare is enough to determine if we are in DST or not.  In the
   * northern hemisphere, if we are before the start rule, we can't have
   * DST.  In the southern hemisphere, if we are after the start rule, we
   * must have DST.  This is reflected in the way the next if statement
   * (not the one immediately following) short circuits. */
  if (southern != (startCompare >= 0))
  {
    /* For the ending rule comparison, we add the dstSavings to the millis
     * passed in to convert them from standard to wall time.  We then must
     * normalize the millis to the range 0..millisPerDay-1. */
    endCompare =
      compareToRule(
        month,
        monthLength,
        prevMonthLength,
        day,
        dayOfWeek,
        millis,
        endTimeMode == WALL_TIME ? dstSavings : (endTimeMode == UTC_TIME ? -rawOffset : 0),
        endMode,
        endMonth,
        endDayOfWeek,
        endDay,
        endTime);
  }

  // Check for both the northern and southern hemisphere cases.  We
  // assume that in the northern hemisphere, the start rule is before the
  // end rule within the calendar year, and vice versa for the southern
  // hemisphere.
  if ((!southern && (startCompare >= 0 && endCompare < 0)) || (southern && (startCompare >= 0 || endCompare < 0)))
  {
    result += dstSavings;
  }

  return result;
}

int SimpleTimeZone::compareToRule(
    XLONG month,
    XLONG monthLen,
    XLONG prevMonthLen,
    XLONG dayOfMonth,
    XLONG dayOfWeek,
    XLONG millis,
    XLONG millisDelta,
    XLONG ruleMode,
    XLONG ruleMonth,
    XLONG ruleDayOfWeek,
    XLONG ruleDay,
    XLONG ruleMillis)
{
  // Make adjustments for startTimeMode and endTimeMode
  millis += millisDelta;
  while (millis >= CalendarSystem::MILLIS_PER_DAY)
  {
    millis -= CalendarSystem::MILLIS_PER_DAY;
    ++dayOfMonth;
    dayOfWeek = 1 + (dayOfWeek % 7); // dayOfWeek is one-based
    if (dayOfMonth > monthLen)
    {
      dayOfMonth = 1;
      /* When incrementing the month, it is desirable to overflow
       * from DECEMBER to DECEMBER+1, since we use the result to
       * compare against a real month. Wraparound of the value
       * leads to bug 4173604. */
      ++month;
    }
  }
  while (millis < 0)
  {
    millis += CalendarSystem::MILLIS_PER_DAY;
    --dayOfMonth;
    dayOfWeek = 1 + ((dayOfWeek + 5) % 7); // dayOfWeek is one-based
    if (dayOfMonth < 1)
    {
      dayOfMonth = prevMonthLen;
      --month;
    }
  }

  if (month < ruleMonth)
  {
    return -1;
  }
  if (month > ruleMonth)
  {
    return 1;
  }

  XLONG ruleDayOfMonth = 0;
  switch (ruleMode)
  {
    case DOM_MODE :
      ruleDayOfMonth = ruleDay;
      break;

    case DOW_IN_MONTH_MODE :
      // In this case ruleDay is the day-of-week-in-month
      if (ruleDay > 0)
      {
        ruleDayOfMonth = 1 + (ruleDay - 1) * 7 + (7 + ruleDayOfWeek - (dayOfWeek - dayOfMonth + 1)) % 7;
      }
      else
      {
        // Assume ruleDay < 0 here
        ruleDayOfMonth = monthLen + (ruleDay + 1) * 7 - (7 + (dayOfWeek + monthLen - dayOfMonth) - ruleDayOfWeek) % 7;
      }
      break;

    case DOW_GE_DOM_MODE :
      ruleDayOfMonth = ruleDay + (49 + ruleDayOfWeek - ruleDay - dayOfWeek + dayOfMonth) % 7;
      break;

    case DOW_LE_DOM_MODE :
      ruleDayOfMonth = ruleDay - (49 - ruleDayOfWeek + ruleDay + dayOfWeek - dayOfMonth) % 7;
      // Note at this point ruleDayOfMonth may be <1, although it will
      // be >=1 for well-formed rules.
      break;
  }

  if (dayOfMonth < ruleDayOfMonth)
  {
    return -1;
  }
  if (dayOfMonth > ruleDayOfMonth)
  {
    return 1;
  }

  if (millis < ruleMillis)
  {
    return -1;
  }
  if (millis > ruleMillis)
  {
    return 1;
  }
  return 0;
}

XLONG SimpleTimeZone::getRawOffset() const
{
  // The given date will be taken into account while
  // we have the historical time zone data in place.
  return rawOffset;
}

void SimpleTimeZone::setRawOffset(XLONG offsetMillis)
{
  this->rawOffset = offsetMillis;
}

void SimpleTimeZone::setDSTSavings(XLONG millisSavedDuringDST)
{
  dstSavings = millisSavedDuringDST;
}

XLONG SimpleTimeZone::getDSTSavings() const
{
  if (useDaylight)
  {
    return dstSavings;
  }
  return 0;
}

bool SimpleTimeZone::useDaylightTime() const
{
  return useDaylight;
}

bool SimpleTimeZone::inDaylightTime(XLONG date) const
{
  return (getOffset(date) != rawOffset);
}

//----------------------------------------------------------------------
// Rule representation
//
// We represent the following flavors of rules:
//       5        the fifth of the month
//       lastSun  the last Sunday in the month
//       lastMon  the last Monday in the month
//       Sun>=8   first Sunday on or after the eighth
//       Sun<=25  last Sunday on or before the 25th
// This is further complicated by the fact that we need to remain
// backward compatible with the 1.1 FCS.  Finally, we need to minimize
// API changes.  In order to satisfy these requirements, we support
// three representation systems, and we translate between them.
//
// INTERNAL REPRESENTATION
// This is the format SimpleTimeZone objects take after construction or
// streaming in is complete.  Rules are represented directly, using an
// unencoded format.  We will discuss the start rule only below; the end
// rule is analogous.
//   startMode      Takes on enumerated values DAY_OF_MONTH,
//                  DOW_IN_MONTH, DOW_AFTER_DOM, or DOW_BEFORE_DOM.
//   startDay       The day of the month, or for DOW_IN_MONTH mode, a
//                  value indicating which DOW, such as +1 for first,
//                  +2 for second, -1 for last, etc.
//   startDayOfWeek The day of the week.  Ignored for DAY_OF_MONTH.
//
// ENCODED REPRESENTATION
// This is the format accepted by the constructor and by setStartRule()
// and setEndRule().  It uses various combinations of positive, negative,
// and zero values to encode the different rules.  This representation
// allows us to specify all the different rule flavors without altering
// the API.
//   MODE              startMonth    startDay    startDayOfWeek
//   DOW_IN_MONTH_MODE >=0           !=0         >0
//   DOM_MODE          >=0           >0          ==0
//   DOW_GE_DOM_MODE   >=0           >0          <0
//   DOW_LE_DOM_MODE   >=0           <0          <0
//   (no DST)          don't care    ==0         don't care
//
// STREAMED REPRESENTATION
// We must retain binary compatibility with the 1.1 FCS.  The 1.1 code only
// handles DOW_IN_MONTH_MODE and non-DST mode, the latter indicated by the
// flag useDaylight.  When we stream an object out, we translate into an
// approximate DOW_IN_MONTH_MODE representation so the object can be parsed
// and used by 1.1 code.  Following that, we write out the full
// representation separately so that contemporary code can recognize and
// parse it.  The full representation is written in a "packed" format,
// consisting of a version number, a length, and an array of bytes.  Future
// versions of this class may specify different versions.  If they wish to
// include additional data, they should do so by storing them after the
// packed representation below.
//----------------------------------------------------------------------

/**
 * Given a set of encoded rules in startDay and startDayOfMonth, decode
 * them and set the startMode appropriately.  Do the same for endDay and
 * endDayOfMonth.  Upon entry, the day of week variables may be zero or
 * negative, in order to indicate special modes.  The day of month
 * variables may also be negative.  Upon exit, the mode variables will be
 * set, and the day of week and day of month variables will be positive.
 * This method also recognizes a startDay or endDay of zero as indicating
 * no DST.
 */
void SimpleTimeZone::decodeRules()
{
  decodeStartRule();
  decodeEndRule();
}

/**
 * Decode the start rule and validate the parameters.  The parameters are
 * expected to be in encoded form, which represents the various rule modes
 * by negating or zeroing certain values.  Representation formats are:
 * <p>
 * <pre>
 *            DOW_IN_MONTH  DOM    DOW>=DOM  DOW<=DOM  no DST
 *            ------------  -----  --------  --------  ----------
 * month       0..11        same    same      same     don't care
 * day        -5..5         1..31   1..31    -1..-31   0
 * dayOfWeek   1..7         0      -1..-7    -1..-7    don't care
 * time        0..ONEDAY    same    same      same     don't care
 * </pre>
 * The range for month does not include UNDECIMBER since this class is
 * really specific to GregorianCalendar, which does not use that month.
 * The range for time includes ONEDAY (vs. ending at ONEDAY-1) because the
 * end rule is an exclusive limit point.  That is, the range of times that
 * are in DST include those >= the start and < the end.  For this reason,
 * it should be possible to specify an end of ONEDAY in order to include the
 * entire day.  Although this is equivalent to time 0 of the following day,
 * it's not always possible to specify that, for example, on December 31.
 * While arguably the start range should still be 0..ONEDAY-1, we keep
 * the start and end ranges the same for consistency.
 */
void SimpleTimeZone::decodeStartRule()
{
  useDaylight = (startDay != 0) && (endDay != 0);
  if (startDay != 0)
  {
    if (startMonth < CalendarSystem::JANUARY || startMonth > CalendarSystem::DECEMBER)
    {
      StringBuffer sb("SimpleTimeZone::decodeStartRule|Illegal start month ");
      System::loggingAndExit(sb.append(startMonth).str(),-1);  
    }
    if (startTime < 0 || startTime >= CalendarSystem::MILLIS_PER_DAY)
    {
      StringBuffer sb("SimpleTimeZone::decodeStartRule|Illegal start time ");
      System::loggingAndExit(sb.append(startTime).str(),-1);  
    }
    if (startDayOfWeek == 0)
    {
      startMode = DOM_MODE;
    }
    else
    {
      if (startDayOfWeek > 0)
      {
        startMode = DOW_IN_MONTH_MODE;
      }
      else
      {
        startDayOfWeek = -startDayOfWeek;
        if (startDay > 0)
        {
          startMode = DOW_GE_DOM_MODE;
        }
        else
        {
          startDay = -startDay;
          startMode = DOW_LE_DOM_MODE;
        }
      }
      if (startDayOfWeek > CalendarSystem::SATURDAY)
      {
        StringBuffer sb("SimpleTimeZone::decodeStartRule|Illegal start day of week ");
        System::loggingAndExit(sb.append(startDayOfWeek).str(),-1);       
      }
    }
    if (startMode == DOW_IN_MONTH_MODE)
    {
      if (startDay < -5 || startDay > 5)
      {
        StringBuffer sb("SimpleTimeZone::decodeStartRule|Illegal start day of week in month ");
        System::loggingAndExit(sb.append(startDay).str(),-1); 
      }
    }
    else if (startDay < 1 || startDay > GregorianSystem::MONTH_LENGTH[startMonth])
    {
       StringBuffer sb("SimpleTimeZone::decodeStartRule|Illegal start day ");
       System::loggingAndExit(sb.append(startDay).str(),-1);
    }
  }
}

void SimpleTimeZone::decodeEndRule()
{
  useDaylight = (startDay != 0) && (endDay != 0);
  if (endDay != 0)
  {
    if (endMonth < CalendarSystem::JANUARY || endMonth > CalendarSystem::DECEMBER)
    {
       StringBuffer sb("SimpleTimeZone::decodeEndRule|Illegal end month ");
       System::loggingAndExit(sb.append(endMonth).str(),-1);
    }
    if (endTime < 0 || endTime >= CalendarSystem::MILLIS_PER_DAY)
    {
       StringBuffer sb("SimpleTimeZone::decodeEndRule|Illegal end time ");
       System::loggingAndExit(sb.append(endTime).str(),-1);
    }
    if (endDayOfWeek == 0)
    {
      endMode = DOM_MODE;
    }
    else
    {
      if (endDayOfWeek > 0)
      {
        endMode = DOW_IN_MONTH_MODE;
      }
      else
      {
        endDayOfWeek = -endDayOfWeek;
        if (endDay > 0)
        {
          endMode = DOW_GE_DOM_MODE;
        }
        else
        {
          endDay = -endDay;
          endMode = DOW_LE_DOM_MODE;
        }
      }
      if (endDayOfWeek > CalendarSystem::SATURDAY)
      {
         StringBuffer sb("SimpleTimeZone::decodeEndRule|Illegal end day of week ");
         System::loggingAndExit(sb.append(endDayOfWeek).str(),-1);
      }
    }
    if (endMode == DOW_IN_MONTH_MODE)
    {
      if (endDay < -5 || endDay > 5)
      {
         StringBuffer sb("SimpleTimeZone::decodeEndRule|Illegal end day of week in month ");
         System::loggingAndExit(sb.append(endDay).str(),-1);
      }
    }
    else if (endDay < 1 || endDay > GregorianSystem::MONTH_LENGTH[endMonth])
    {
       StringBuffer sb("SimpleTimeZone::decodeEndRule|Illegal end day ");
       System::loggingAndExit(sb.append(endDay).str(),-1);
    }
  }
}

__OPENCODE_END_NAMESPACE
