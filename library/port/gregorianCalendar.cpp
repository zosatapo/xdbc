#include "port/gregorianCalendar.h"
#include "port/gregorianCalendar.h"
#include "port/ioc_sbuf.h"
#include "port/libsys.h"

__OPENCODE_BEGIN_NAMESPACE

// 0-based, for day-in-year
const int GregorianSystem::NUM_DAYS[]= { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };  
const int GregorianSystem::LEAP_NUM_DAYS[]= { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335 };

// 0-based, for day-in-month
const int GregorianSystem::MONTH_LENGTH[]= { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }; // 0-based
const int GregorianSystem::LEAP_MONTH_LENGTH[]= { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }; // 0-based

const int GregorianSystem::JAN_1_1_JULIAN_DAY = 1721426; // January 1, year 1 (Gregorian)
const int GregorianSystem::EPOCH_JULIAN_DAY = 2440588; // January 1, 1970 (Gregorian)
const int GregorianSystem::EPOCH_YEAR = 1970;  

const int GregorianSystem::EPOCH_DATE = 719163;

const int GregorianSystem::AD = 1;

const int GregorianSystem::BC = 0;

CalendarDate GregorianSystem::getCalendarDate(XLONG utc) 
{
  XLONG days;
  XLONG millis;

  days = utc / CalendarSystem::MILLIS_PER_DAY;
  millis = (XLONG)(utc % CalendarSystem::MILLIS_PER_DAY);
  if (millis < 0) 
  {
    millis += CalendarSystem::MILLIS_PER_DAY;
    days--;
  }
  days += EPOCH_DATE;
  CalendarDate cdate = getCalendarDateFromFixedDate(days);
  cdate.setTimeOfDay(millis);
  return cdate;
}

XLONG  GregorianSystem::dateToMillis(const CalendarDate& date) 
{
  XLONG gd = getFixedDate(date.getYear(),
  date.getMonth(),
  date.getDate());
  return ((gd - EPOCH_DATE) * CalendarSystem::MILLIS_PER_DAY + date.getTimeOfDay());
}

XLONG GregorianSystem::dateToMillis(int year, int month, int day,XLONG milliseconds) 
{
  XLONG gd = getFixedDate(year, month, day);
  return ((gd - EPOCH_DATE) * CalendarSystem::MILLIS_PER_DAY + milliseconds);
}

bool GregorianSystem::validate(const CalendarDate& date) 
{
  int month = date.getMonth();
  if (month < CalendarSystem::JANUARY || month > CalendarSystem::DECEMBER) 
  {
    return false;
  }
  
  int days = getMonthLength(date.getYear(), month);
  if (date.getDate() <= 0 || date.getDate() > days) 
  {
    return false;
  }
  return true;
}

int GregorianSystem::getMonthLength(int year, int month) 
{
  int days = MONTH_LENGTH[month];
  if (month == CalendarSystem::FEBRUARY && isLeapYear(year)) 
  {
    days++;
  }
  return days;
}

  /**
   * Returns number of days from 0001-01-01. Counting leap correction.
   */
XLONG GregorianSystem::getFixedDate(int year, int month, int day) 
{
  int prevyear = year - 1;
  month++;  // 1-based month numbering
  XLONG days ;

  if (prevyear >= 0) 
  {
    days = (365 * (XLONG)prevyear)
    + (prevyear / 4)
    - (prevyear / 100)
    + (prevyear / 400)
    + ((367 * month - 362) / 12)
    + day;
  } 
  else 
  {
    days = (365 * (XLONG)prevyear)
    + floorDivide(prevyear, 4)
    - floorDivide(prevyear, 100)
    + floorDivide(prevyear, 400)
    + floorDivide((367 * month - 362), 12)
    + day;
  }
  if (month > 2) 
  {
    days -= (isLeapYear(year)) ? 1 : 2;
  }
  return days;
}

CalendarDate GregorianSystem::getCalendarDateFromFixedDate(XLONG fixedDate) 
{
  int year = getYear(fixedDate);
  int pday = (int)(fixedDate - getFixedDate(year, CalendarSystem::JANUARY, 1));
  int corr = 2;
  XLONG mar1 = getFixedDate(year, CalendarSystem::MARCH, 1);
  if (fixedDate < mar1) 
  {
    corr = 0;
  } 
  else if (fixedDate >= mar1 && isLeapYear(year)) 
  {
    corr = 1;
  }
  int month = floorDivide((12 * (pday + corr) + 373), 367) - 1;
  int day = (int)(fixedDate - getFixedDate(year, month, 1) + 1);
  int dow = getDayOfWeekFromFixedDate(fixedDate);

  CalendarDate cdate(year, month, day);
  cdate.setDayOfWeek(dow);
  return cdate;
}

int GregorianSystem::getDayOfWeek(const CalendarDate& date) 
{
  XLONG fixedDate = getFixedDate(date.getYear(),date.getMonth(),date.getDate());
  
  return getDayOfWeekFromFixedDate(fixedDate);
}

int GregorianSystem::getDayOfWeekFromFixedDate(XLONG fixedDate)
{
  if (fixedDate >= 0) 
  {
    return (int)(fixedDate % 7) + SUNDAY;
  }
  
  return (int)mod(fixedDate, (XLONG)7) + SUNDAY;
}

int GregorianSystem::getYear(XLONG fixedDate) 
{
  XLONG d0;
  int  d1, d2, d3;
  int  n400, n100, n4, n1;
  int  year;

  if (fixedDate >= 0) 
  {
    d0 = fixedDate - 1;
    n400 = (int)(d0 / 146097);
    d1 = (int)(d0 % 146097);
    n100 = d1 / 36524;
    d2 = d1 % 36524;
    n4 = d2 / 1461;
    d3 = d2 % 1461;
    n1 = d3 / 365;
  } 
  else 
  {
    d0 = fixedDate - 1;
    n400 = (int)floorDivide((XLONG)d0, (XLONG)146097L);
    d1 = (int)mod((XLONG)d0, (XLONG)146097L);
    n100 = floorDivide((int)d1,(int) 36524);
    d2 = mod(d1, 36524);
    n4 = floorDivide(d2, 1461);
    d3 = mod(d2, 1461);
    n1 = floorDivide(d3, 365);
  }
  year = 400 * n400 + 100 * n100 + 4 * n4 + n1;
  if (!(n100 == 4 || n1 == 4)) 
  {
    ++year;
  }
  return year;
}

bool GregorianSystem::isLeapYear(int year) 
{
  if (year >= 0) 
  {
    return (((year % 4) == 0)&& (((year % 100) != 0) || ((year % 400) == 0)));
  }
  
  return (((mod(year, 4) == 0)&& ((mod(year, 100) != 0) || (mod(year, 400) == 0))));
}


XLONG GregorianSystem::floorDivide(XLONG n, XLONG d) 
{
  return ((n >= 0) ?(n / d) : (((n + 1) / d) - 1));
}

int GregorianSystem::floorDivide(int n, int d) 
{
  return ((n >= 0) ?(n / d) : (((n + 1) / d) - 1));
}

XLONG GregorianSystem::mod(XLONG x, XLONG y) 
{
  return (x - y * floorDivide(x, y));
}

int GregorianSystem::mod(int x, int y) 
{
  return (x - y * floorDivide(x, y));
}

const int GregorianCalendar::BC = 0;

const int GregorianCalendar::AD = 1;

const int GregorianCalendar::MIN_VALUES[] = { 0, 1, 0, 1, 0, 1, 1, 1, -1, 0, 0, 0, 0, 0, 0, -12 * (int)CalendarSystem::MILLIS_PER_HOUR, 0 };

const int GregorianCalendar::LEAST_MAX_VALUES[] ={ 1, 292269054, 11, 52, 4, 28, 365, 7, 4, 1, 11, 23, 59, 59, 999, 12 * (int)CalendarSystem::MILLIS_PER_HOUR, 1 * (int)CalendarSystem::MILLIS_PER_HOUR };

const int GregorianCalendar::MAX_VALUES[] ={ 1, 292278994, 11, 53, 6, 31, 366, 7, 6, 1, 11, 23, 59, 59, 999, 12 * (int)CalendarSystem::MILLIS_PER_HOUR,1 * (int)CalendarSystem::MILLIS_PER_HOUR };


Calendar* GregorianCalendar::doClone() const
{
  return new GregorianCalendar(*this);
}

GregorianCalendar::~GregorianCalendar() 
{
}

GregorianCalendar::GregorianCalendar(const GregorianCalendar& rhs)
:Calendar(rhs)
{
  gregorianCutover = rhs.gregorianCutover;
  normalizedGregorianCutover = rhs.normalizedGregorianCutover;
  gregorianCutoverYear = rhs.gregorianCutoverYear;
}

GregorianCalendar& GregorianCalendar::operator=(const GregorianCalendar& rhs)
{
  if(&rhs == this)
  {
    return *this;
  }

  Calendar::operator=(rhs);

  gregorianCutover = rhs.gregorianCutover;
  normalizedGregorianCutover = rhs.normalizedGregorianCutover;
  gregorianCutoverYear = rhs.gregorianCutoverYear;

  return *this;
}

  
GregorianCalendar::GregorianCalendar()
:Calendar(TimeZone::getDefault(), Locale::getDefault())
{

  gregorianCutover = -12219292800000LL;
  normalizedGregorianCutover = gregorianCutover;
  gregorianCutoverYear = 1582;

  setTimeInMillis(System::currentTimeMillis());
}

GregorianCalendar::GregorianCalendar(const TimeZone* aTimeZone)
:Calendar(aTimeZone, Locale::getDefault())
{
  gregorianCutover = -12219292800000LL;
  normalizedGregorianCutover = gregorianCutover;
  gregorianCutoverYear = 1582;

  setTimeInMillis(System::currentTimeMillis());
}

GregorianCalendar::GregorianCalendar(const Locale& aLocale)
:Calendar(TimeZone::getDefault(), aLocale)
{
  gregorianCutover = -12219292800000LL;
  normalizedGregorianCutover = gregorianCutover;
  gregorianCutoverYear = 1582;

  setTimeInMillis(System::currentTimeMillis());
}


GregorianCalendar::GregorianCalendar(const TimeZone* aTimeZone, const Locale& aLocale)
:Calendar(aTimeZone, aLocale)
{
  gregorianCutover = -12219292800000LL;
  normalizedGregorianCutover = gregorianCutover;
  gregorianCutoverYear = 1582;

  setTimeInMillis(System::currentTimeMillis());
}

GregorianCalendar::GregorianCalendar(int year, int month, int date, int hour, int minute, int second)
:Calendar(TimeZone::getDefault(), Locale::getDefault())
{
  gregorianCutover = -12219292800000LL;
  normalizedGregorianCutover = gregorianCutover;
  gregorianCutoverYear = 1582;

  this->set(YEAR, year);
  this->set(MONTH, month);
  this->set(DATE, date);
  this->set(HOUR_OF_DAY, hour);
  this->set(MINUTE, minute);
  this->set(SECOND, second);
}

void GregorianCalendar::setGregorianChange(XLONG date)
{
  gregorianCutover = date;

  // Precompute two internal variables which we use to do the actual
  // cutover computations.  These are the normalized cutover, which is the
  // midnight at or before the cutover, and the cutover year.  The
  // normalized cutover is in pure date milliseconds; it contains no time
  // of day or timezone component, and it used to compare against rv
  // pure date values.
  XLONG cutoverDay = floorDivide(gregorianCutover, (XLONG)CalendarSystem::MILLIS_PER_DAY);
  normalizedGregorianCutover = cutoverDay * CalendarSystem::MILLIS_PER_DAY;

  // Handle the rare case of numeric overflow.  If the user specifies a
  // change of Date(XLONG_MIN), in order to get a pure Gregorian
  // calendar, then the epoch day is -106751991168, which when multiplied
  // by MILLIS_PER_DAY gives 9223372036794351616 -- the negative value is too
  // large for 64 bits, and overflows into a positive value.  We correct
  // this by using the next day, which for all intents is semantically
  // equivalent.
  if (cutoverDay < 0 && normalizedGregorianCutover > 0)
  {
    normalizedGregorianCutover = (cutoverDay + 1) * CalendarSystem::MILLIS_PER_DAY;
  }

  // Normalize the year so BC values are represented as 0 and negative
  // values.
  GregorianCalendar cal(getTimeZone());
  cal.setTimeInMillis(getTimeInMillis());

  gregorianCutoverYear = cal.get(YEAR);
  if (cal.get(ERA) == BC)
  {
    gregorianCutoverYear = 1 - gregorianCutoverYear;
  }
}

XLONG GregorianCalendar::getGregorianChange() const
{
  return gregorianCutover;
}

bool GregorianCalendar::isLeapYear(int year) const
{
  return year >= gregorianCutoverYear ? ((year % 4 == 0) && ((year % 100 != 0) || (year % 400 == 0))) : // Gregorian
   (year % 4 == 0); // Julian
}

void GregorianCalendar::add(int field, int amount)
{
  if (amount == 0)
  {
    return; // Do nothing!
  }
  complete();

  if (field == YEAR)
  {
    int year = this->internalGet(YEAR);
    if (this->internalGetEra() == AD)
    {
      year += amount;
      if (year > 0)
      {
        this->set(YEAR, year);
      }
      else
      { // year <= 0
        this->set(YEAR, 1 - year);
        // if year == 0, you get 1 BC
        this->set(ERA, BC);
      }
    }
    else
    { // era == BC
      year -= amount;
      if (year > 0)
      {
        this->set(YEAR, year);
      }
      else
      { // year <= 0
        this->set(YEAR, 1 - year);
        // if year == 0, you get 1 AD
        this->set(ERA, AD);
      }
    }
    pinDayOfMonth();
  }
  else if (field == MONTH)
  {
    int month = this->internalGet(MONTH) + amount;
    int year = this->internalGet(YEAR);
    int y_amount;

    if (month >= 0)
    {
      y_amount = month / 12;
    }
    else
    {
      y_amount = (month + 1) / 12 - 1;
    }
    if (y_amount != 0)
    {
      if (this->internalGetEra() == AD)
      {
        year += y_amount;
        if (year > 0)
        {
          this->set(YEAR, year);
        }
        else
        { // year <= 0
          this->set(YEAR, 1 - year);
          // if year == 0, you get 1 BC
          this->set(ERA, BC);
        }
      }
      else
      { // era == BC
        year -= y_amount;
        if (year > 0)
        {
          this->set(YEAR, year);
        }
        else
        { // year <= 0
          this->set(YEAR, 1 - year);
          // if year == 0, you get 1 AD
          this->set(ERA, AD);
        }
      }
    }

    if (month >= 0)
    {
      set(MONTH, (int) (month % 12));
    }
    else
    {
      // month < 0
      month %= 12;
      if (month < 0)
      {
        month += 12;
      }
      set(MONTH, JANUARY + month);
    }
    pinDayOfMonth();
  }
  else if (field == ERA)
  {
    int era = internalGet(ERA) + amount;
    if (era < 0)
    {
      era = 0;
    }
    if (era > 1)
    {
      era = 1;
    }
    set(ERA, era);
  }
  else
  {
    // We handle most fields here.  The algorithm is to add a computed amount
    // of millis to the current millis.  The only wrinkle is with DST -- if
    // the result of the add operation is to move from DST to Standard, or vice
    // versa, we need to adjust by an hour forward or back, respectively.
    // Otherwise you get weird effects in which the hour seems to shift when
    // you add to the DAY_OF_MONTH field, for instance.

    // We only adjust the DST for fields larger than an hour.  For fields
    // smaller than an hour, we cannot adjust for DST without causing problems.
    // for instance, if you add one hour to April 5, 1998, 1:00 AM, in PST,
    // the time becomes "2:00 AM PDT" (an illegal value), but then the adjustment
    // sees the change and compensates by subtracting an hour.  As a result the
    // time doesn't advance at all.

    XLONG delta = amount;
    bool adjustDST = true;

    switch (field)
    {
      case CALENDAR_WEEK_OF_YEAR :
      case CALENDAR_WEEK_OF_MONTH :
      case CALENDAR_DAY_OF_WEEK_IN_MONTH :
        delta *= 7 * 24 * 60 * 60 * 1000; // 7 days
        break;

      case CALENDAR_AM_PM :
        delta *= 12 * 60 * 60 * 1000; // 12 hrs
        break;

      case CALENDAR_DATE : // synonym of DAY_OF_MONTH
      case CALENDAR_DAY_OF_YEAR :
      case CALENDAR_DAY_OF_WEEK :
        delta *= 24 * 60 * 60 * 1000; // 1 day
        break;

      case CALENDAR_HOUR_OF_DAY :
      case CALENDAR_HOUR :
        delta *= 60 * 60 * 1000; // 1 hour
        adjustDST = false;
        break;

      case CALENDAR_MINUTE :
        delta *= 60 * 1000; // 1 minute
        adjustDST = false;
        break;

      case CALENDAR_SECOND :
        delta *= 1000; // 1 second
        adjustDST = false;
        break;

      case CALENDAR_MILLISECOND :
        adjustDST = false;
        break;

      case CALENDAR_ZONE_OFFSET :
      case CALENDAR_DST_OFFSET :
      default :
      {
        System::loggingAndExit("GregorianCalendar::add|These fields cannot be rolled",-1); 
      }
    }

    // Save the current DST state.
    XLONG dst = 0;
    if (adjustDST)
    {
      dst = internalGet(DST_OFFSET);
    }

    setTimeInMillis(time + delta); // Automatically computes fields if necessary

    if (adjustDST)
    {
      // Now do the DST adjustment alluded to above.
      // Only call setTimeInMillis if necessary, because it's an expensive call.
      dst -= internalGet(DST_OFFSET);
      if (dst != 0)
      {
        setTimeInMillis(time + dst);
      }
    }
  }
}

void GregorianCalendar::roll(int field, int amount)
{
  if (amount == 0)
  {
    return; // Nothing to do
  }

  int min = 0, max = 0, gap;
  if (field >= 0 && field < CALENDAR_FIELD_COUNT)
  {
    complete();
    min = getMinimum(field);
    max = getMaximum(field);
  }

  switch (field)
  {
    case CALENDAR_ERA :
    case CALENDAR_YEAR :
    case CALENDAR_AM_PM :
    case CALENDAR_MINUTE :
    case CALENDAR_SECOND :
    case CALENDAR_MILLISECOND :
      // These fields are handled simply, since they have fixed minima
      // and maxima.  The field DAY_OF_MONTH is almost as simple.  Other
      // fields are complicated, since the range within they must roll
      // varies depending on the date.
      break;

    case CALENDAR_HOUR :
    case CALENDAR_HOUR_OF_DAY :
      // Rolling the hour is difficult on the ONSET and CEASE days of
      // daylight savings.  For example, if the change occurs at
      // 2 AM, we have the following progression:
      // ONSET: 12 Std -> 1 Std -> 3 Dst -> 4 Dst
      // CEASE: 12 Dst -> 1 Dst -> 1 Std -> 2 Std
      // To get around this problem we don't use fields; we manipulate
      // the time in millis directly.
      {
        // Assume min == 0 in calculations below
        XLONG start = getTimeInMillis();
        int oldHour = internalGet(field);
        int newHour = (oldHour + amount) % (max + 1);
        if (newHour < 0)
        {
          newHour += max + 1;
        }
        setTimeInMillis(start + CalendarSystem::MILLIS_PER_HOUR * (newHour - oldHour));
        return;
      }
    case CALENDAR_MONTH :
      // Rolling the month involves both pinning the const value to [0, 11]
      // and adjusting the DAY_OF_MONTH if necessary.  We only adjust the
      // DAY_OF_MONTH if, after updating the MONTH field, it is illegal.
      // E.g., <jan31>.roll(MONTH, 1) -> <feb28> or <feb29>.
      {
        int mon = (internalGet(MONTH) + amount) % 12;
        if (mon < 0)
        {
          mon += 12;
        }
        set(MONTH, mon);

        // Keep the day of month in range.  We don't want to spill over
        // into the next month; e.g., we don't want jan31 + 1 mo -> feb31 ->
        // mar3.
        // NOTE: We could optimize this later by checking for dom <= 28
        // first.  Do this if there appears to be a need. [LIU]
        int monthLen = monthLength(mon);
        int dom = internalGet(DAY_OF_MONTH);
        if (dom > monthLen)
        {
          set(DAY_OF_MONTH, monthLen);
        }
        return;
      }

    case CALENDAR_WEEK_OF_YEAR :
      {
        // Unlike WEEK_OF_MONTH, WEEK_OF_YEAR never shifts the day of the
        // week.  Also, rolling the week of the year can have seemingly
        // strange effects simply because the year of the week of year
        // may be different from the calendar year.  For example, the
        // date Dec 28, 1997 is the first day of week 1 of 1998 (if
        // weeks start on Sunday and the minimal days in first week is
        // <= 3).
        int woy = internalGet(WEEK_OF_YEAR);
        // Get the ISO year, which matches the week of year.  This
        // may be one year before or after the calendar year.
        int isoYear = internalGet(YEAR);
        int isoDoy = internalGet(DAY_OF_YEAR);
        if (internalGet(MONTH) == Calendar::JANUARY)
        {
          if (woy >= 52)
          {
            --isoYear;
            isoDoy += yearLength(isoYear);
          }
        }
        else
        {
          if (woy == 1)
          {
            isoDoy -= yearLength(isoYear);
            ++isoYear;
          }
        }
        woy += amount;
        // Do fast checks to avoid unnecessary computation:
        if (woy < 1 || woy > 52)
        {
          // Determine the last week of the ISO year.
          // First, we calculate the relative fractional days of the
          // last week of the year. (This doesn't include days in 
          // the year before or after the calendar year.)
          int lastDoy = yearLength(isoYear);
          int normalizedDayOfWeek = internalGet(DAY_OF_WEEK) - getFirstDayOfWeek();
          if (normalizedDayOfWeek < 0)
          {
            normalizedDayOfWeek += 7;
          }
          int lastRelDow = (lastDoy - isoDoy + normalizedDayOfWeek) % 7;
          if (lastRelDow < 0)
          {
            lastRelDow += 7;
          }

          // Next, calculate the minimal last week of year.
          // Now this value is just the total number of weeks in the
          // year all of which have 7 days a week. Need to check the
          // first and the last week of the year, which would have 
          // days fewer than 7.
          int lastWoy;
          lastDoy -= (lastRelDow + 1);
          lastWoy = lastDoy / 7;

          // If the relative fraction of the first week of the year
          // is more than MinimalDaysInFirstWeek, add 1 to the last
          // week // of the year.
          if ((lastDoy - (lastWoy * 7)) >= getMinimalDaysInFirstWeek())
          {
            lastWoy++;
          }

          // If the relative fraction of the last week of the year 
          // is more than MinimalDaysInFirstWeek, add 1 to the last
          // week of the year.
          if ((6 - lastRelDow) < getMinimalDaysInFirstWeek())
          {
            lastWoy++;
          }

          woy = ((woy + lastWoy - 1) % lastWoy) + 1;
        }
        set(WEEK_OF_YEAR, woy);
        set(YEAR, isoYear);
        return;
      }
    case CALENDAR_WEEK_OF_MONTH :
      {
        // This is tricky, because during the roll we may have to shift
        // to a different day of the week.  For example:

        //    s  m  t  w  r  f  s
        //          1  2  3  4  5
        //    6  7  8  9 10 11 12

        // When rolling from the 6th or 7th back one week, we go to the
        // 1st (assuming that the first partial week counts).  The same
        // thing happens at the end of the month.

        // The rv tricky thing is that we have to figure out whether
        // the first partial week actually counts or not, based on the
        // minimal first days in the week.  And we have to use the
        // correct first day of the week to delineate the week
        // boundaries.

        // Here's our algorithm.  First, we find the real boundaries of
        // the month.  Then we discard the first partial week if it
        // doesn't count in this locale.  Then we fill in the ends with
        // phantom days, so that the first partial week and the last
        // partial week are full weeks.  We then have a nice square
        // block of weeks.  We do the usual rolling within this block,
        // as is done elsewhere in this method.  If we wind up on one of
        // the phantom days that we added, we recognize this and pin to
        // the first or the last day of the month.  Easy, eh?

        // Normalize the DAY_OF_WEEK so that 0 is the first day of the week
        // in this locale.  We have dow in 0..6.
        int dow = internalGet(DAY_OF_WEEK) - getFirstDayOfWeek();
        if (dow < 0)
        {
          dow += 7;
        }

        // Find the day of the week (normalized for locale) for the first
        // of the month.
        int fdm = (dow - internalGet(DAY_OF_MONTH) + 1) % 7;
        if (fdm < 0)
        {
          fdm += 7;
        }

        // Get the first day of the first full week of the month,
        // including phantom days, if any.  Figure out if the first week
        // counts or not; if it counts, then fill in phantom days.  If
        // not, advance to the first real full week (skip the partial week).
        int start;
        if ((7 - fdm) < getMinimalDaysInFirstWeek())
        {
          start = 8 - fdm; // Skip the first partial week
        }
        else
        {
          start = 1 - fdm; // This may be zero or negative
        }

        // Get the day of the week (normalized for locale) for the last
        // day of the month.
        int monthLen = monthLength(internalGet(MONTH));
        int ldm = (monthLen - internalGet(DAY_OF_MONTH) + dow) % 7;
        // We know monthLen >= DAY_OF_MONTH so we skip the += 7 step here.

        // Get the limit day for the blocked-off rectangular month; that
        // is, the day which is one past the last day of the month,
        // after the month has already been filled in with phantom days
        // to fill out the last week.  This day has a normalized DOW of 0.
        int limit = monthLen + 7 - ldm;

        // Now roll between start and (limit - 1).
        gap = limit - start;
        int day_of_month = (internalGet(DAY_OF_MONTH) + amount * 7 - start) % gap;
        if (day_of_month < 0)
        {
          day_of_month += gap;
        }
        day_of_month += start;

        // Finally, pin to the real start and end of the month.
        if (day_of_month < 1)
        {
          day_of_month = 1;
        }
        if (day_of_month > monthLen)
        {
          day_of_month = monthLen;
        }

        // Set the DAY_OF_MONTH.  We rely on the fact that this field
        // takes precedence over everything else (since all rv fields
        // are also set at this point).  If this fact changes (if the
        // disambiguation algorithm changes) then we will have to unset
        // the appropriate fields here so that DAY_OF_MONTH is attended
        // to.
        set(DAY_OF_MONTH, day_of_month);
        return;
      }

    case CALENDAR_DAY_OF_MONTH :
      max = monthLength(internalGet(MONTH));
      break;

    case CALENDAR_DAY_OF_YEAR :
      {
        // Roll the day of year using millis.  Compute the millis for
        // the start of the year, and get the length of the year.
        XLONG delta = amount * CalendarSystem::MILLIS_PER_DAY; // Scale up from days to millis
        XLONG min2 = time - (internalGet(DAY_OF_YEAR) - 1) * CalendarSystem::MILLIS_PER_DAY;
        time = (time + delta - min2) % (yearLength() * CalendarSystem::MILLIS_PER_DAY);
        if (time < 0)
        {
          time += yearLength() * CalendarSystem::MILLIS_PER_DAY;
        }
        XLONG dst = internalGet(DST_OFFSET);
        setTimeInMillis(time + min2);
        dst -= internalGet(DST_OFFSET);
        if (dst != 0)
        {
          setTimeInMillis(time + dst);
        }
        return;
      }

    case CALENDAR_DAY_OF_WEEK :
      {
        // Roll the day of week using millis.  Compute the millis for
        // the start of the week, using the first day of week setting.
        // Restrict the millis to [start, start+7days).
        XLONG delta = amount * CalendarSystem::MILLIS_PER_DAY; // Scale up from days to millis
        // Compute the number of days before the current day in this
        // week.  This will be a value 0..6.
        int leadDays = internalGet(DAY_OF_WEEK) - getFirstDayOfWeek();
        if (leadDays < 0)
        {
          leadDays += 7;
        }
        XLONG min2 = time - leadDays * CalendarSystem::MILLIS_PER_DAY;
        time = (time + delta - min2) % CalendarSystem::MILLIS_PER_WEEK;
        if (time < 0)
        {
          time += CalendarSystem::MILLIS_PER_WEEK;
        }
        XLONG dst = internalGet(DST_OFFSET);
        setTimeInMillis(time + min2);
        dst -= internalGet(DST_OFFSET);
        if (dst != 0)
        {
          setTimeInMillis(time + dst);
        }
        return;
      }

    case CALENDAR_DAY_OF_WEEK_IN_MONTH :
      {
        // Roll the day of week in the month using millis.  Determine
        // the first day of the week in the month, and then the last,
        // and then roll within that range.
        XLONG delta = amount * CalendarSystem::MILLIS_PER_WEEK; // Scale up from weeks to millis
        // Find the number of same days of the week before this one
        // in this month.
        int preWeeks = (internalGet(DAY_OF_MONTH) - 1) / 7;
        // Find the number of same days of the week after this one
        // in this month.
        int postWeeks = (monthLength(internalGet(MONTH)) - internalGet(DAY_OF_MONTH)) / 7;
        // From these compute the min and gap millis for rolling.
        XLONG min2 = time - preWeeks * CalendarSystem::MILLIS_PER_WEEK;
        XLONG gap2 = CalendarSystem::MILLIS_PER_WEEK * (preWeeks + postWeeks + 1); // Must add 1!
        // Roll within this range
        time = (time + delta - min2) % gap2;
        if (time < 0)
        {
          time += gap2;
        }
        XLONG dst = internalGet(DST_OFFSET);
        setTimeInMillis(time + min2);
        dst -= internalGet(DST_OFFSET);
        if (dst != 0)
        {
          setTimeInMillis(time + dst);
        }
        return;
      }

    case CALENDAR_ZONE_OFFSET :
    case CALENDAR_DST_OFFSET :
    default :
    {
      System::loggingAndExit("GregorianCalendar::roll|These fields cannot be rolled",-1); 
    }
  }

  // These are the standard roll instructions.  These work for all
  // simple cases, that is, cases in which the limits are fixed, such
  // as the hour, the month, and the era.
  gap = max - min + 1;
  int value = internalGet(field) + amount;
  value = (value - min) % gap;
  if (value < 0)
  {
    value += gap;
  }
  value += min;

  set(field, value);
}

int GregorianCalendar::getMinimum(int field) const
{
  return MIN_VALUES[field];
}


int GregorianCalendar::getMaximum(int field) const
{
  return MAX_VALUES[field];
}

int GregorianCalendar::getGreatestMinimum(int field) const
{
  return MIN_VALUES[field];
}

int GregorianCalendar::getLeastMaximum(int field) const
{
  return LEAST_MAX_VALUES[field];
}

int GregorianCalendar::getActualMinimum(int field) const
{
  return getMinimum(field);
}

int GregorianCalendar::getActualMaximum(int field) const
{
  /* It is a known limitation that the code here (and in getActualMinimum)
   * won't behave properly at the extreme limits of GregorianCalendar's
   * representable range (except for the code that handles the YEAR
   * field).  That's because the ends of the representable range are at
   * odd spots in the year.  For calendars with the default Gregorian
   * cutover, these limits are Sun Dec 02 16:47:04 GMT 292269055 BC to Sun
   * Aug 17 07:12:55 GMT 292278994 AD, somewhat different for non-GMT
   * zones.  As a result, if the calendar is set to Aug 1 292278994 AD,
   * the actual maximum of DAY_OF_MONTH is 17, not 30.  If the date is Mar
   * 31 in that year, the actual maximum month might be Jul, whereas is
   * the date is Mar 15, the actual maximum might be Aug -- depending on
   * the precise semantics that are desired.  Similar considerations
   * affect all fields.  Nonetheless, this effect is sufficiently arcane
   * that we permit it, rather than complicating the code to handle such
   * intricacies. - liu 8/20/98 */

  switch (field)
  {
    // we have functions that enable us to fast-path number of days in month
    // of year
    case CALENDAR_DAY_OF_MONTH :
    {
      GregorianCalendar* p_cal = (GregorianCalendar*)(this);
      return p_cal->monthLength(p_cal->get(MONTH));
    }

    case CALENDAR_DAY_OF_YEAR :
    {
      GregorianCalendar* p_cal = (GregorianCalendar*)(this);
      return p_cal->yearLength();
    }

      // for week of year, week of month, or day of week in month, we
      // just fall back on the default implementation in Calendar (I'm not sure
      // we could do better by having special calculations here)
    case CALENDAR_WEEK_OF_YEAR :
    case CALENDAR_WEEK_OF_MONTH :
    case CALENDAR_DAY_OF_WEEK_IN_MONTH :
    {
      return Calendar::getActualMaximum(field);
    }

    case CALENDAR_YEAR :
    {
      /* The year computation is no different, in principle, from the
       * others, however, the range of possible maxima is large.  In
       * addition, the way we know we've exceeded the range is different.
       * For these reasons, we use the special case code below to handle
       * this field.
       *
       * The actual maxima for YEAR depend on the type of calendar:
       *
       *     Gregorian = May 17, 292275056 BC - Aug 17, 292278994 AD
       *     Julian    = Dec  2, 292269055 BC - Jan  3, 292272993 AD
       *     Hybrid    = Dec  2, 292269055 BC - Aug 17, 292278994 AD
       *
       * We know we've exceeded the maximum when either the month, date,
       * time, or era changes in response to setting the year.  We don't
       * check for month, date, and time here because the year and era are
       * sufficient to detect an invalid year setting.  NOTE: If code is
       * added to check the month and date in the future for some reason,
       * Feb 29 must be allowed to shift to Mar 1 when setting the year.
       */ 
      GregorianCalendar cal(*this);
      cal.setLenient(true);

      int era = cal.get(ERA);
      XLONG d = cal.getTimeInMillis();

      /* Perform a binary search, with the invariant that lowGood is a
       * valid year, and highBad is an out of range year.
       */
      int lowGood = LEAST_MAX_VALUES[YEAR];
      int highBad = MAX_VALUES[YEAR] + 1;
      while ((lowGood + 1) < highBad)
      {
        int y = (lowGood + highBad) / 2;
        cal.set(YEAR, y);
        if (cal.get(YEAR) == y && cal.get(ERA) == era)
        {
          lowGood = y;
        }
        else
        {
          highBad = y;
          cal.setTimeInMillis(d); // Restore original fields
        }
      }

      return lowGood;
   }

      // and we know none of the rv fields have variable maxima in
      // GregorianCalendar, so we can just return the fixed maximum
    default :
    {
      return getMaximum(field);
    }
  }
}

int GregorianCalendar::getISOYear() const
{
  GregorianCalendar* p_cal = (GregorianCalendar*)(this);
  p_cal->complete();

  int woy = internalGet(WEEK_OF_YEAR);
  // Get the ISO year, which matches the week of year.  This
  // may be one year before or after the calendar year.
  int isoYear = internalGet(YEAR);
  if (internalGet(MONTH) == Calendar::JANUARY)
  {
    if (woy >= 52)
    {
      --isoYear;
    }
  }
  else
  {
    if (woy == 1)
    {
      ++isoYear;
    }
  }
  return isoYear;
}

void GregorianCalendar::computeFields() 
{
  computeFieldsImpl();

  // Careful here: We are manually setting the time stamps[]
  // flags to INTERNALLY_SET, so we must be sure that the
  // computeFieldsImpl method actually does set all the fields.
  for (int i = 0; i < CALENDAR_FIELD_COUNT; ++i)
  {
    stamp[i] = INTERNALLY_SET;
  }
}


void GregorianCalendar::computeTime()
{
  if (!isLenient() && !validateFields())
  {
    System::loggingAndExit("GregorianCalendar::computeTime|!isLenient() && !validateFields()",-1); 
  }

  // This function takes advantage of the fact that unset fields in
  // the time field list have a value of zero.

  // The year defaults to the epoch start.
  int year = (stamp[YEAR] != UNSET) ? internalGet(YEAR) : GregorianSystem::EPOCH_YEAR;

  // The YEAR field must always be used regardless of its SET
  // state because YEAR is a mandatory field to determine the date
  // and the default value (EPOCH_YEAR) may change through the
  // normalization process.
  int fieldMask = 1 << YEAR;

  int era = AD;
  if (stamp[ERA] != UNSET)
  {
    era = internalGet(ERA);
    fieldMask |= 1 << ERA;
    if (era == BC)
    {
      year = 1 - year;
    }
    else if (era != AD)
    {
      // Even in lenient mode we disallow ERA values rv than AD & BC
      System::loggingAndExit("GregorianCalendar::computeTime|Invalid era",-1);  
    }
  }

  vector<int> fieldMaskParam;
  fieldMaskParam.push_back(fieldMask);
 
  // First, use the year to determine whether to use the Gregorian or the
  // Julian calendar. If the year is not the year of the cutover, this
  // computation will be correct. But if the year is the cutover year,
  // this may be incorrect. In that case, assume the Gregorian calendar,
  // make the computation, and then recompute if the resultant millis
  // indicate the wrong calendar has been assumed.

  // A date such as Oct. 10, 1582 does not exist in a Gregorian calendar
  // with the default changeover of Oct. 15, 1582, since in such a
  // calendar Oct. 4 (Julian) is followed by Oct. 15 (Gregorian).  This
  // algorithm will interpret such a date using the Julian calendar,
  // yielding Oct. 20, 1582 (Gregorian).
  bool isGregorian = year >= gregorianCutoverYear;
  XLONG julianDay = computeJulianDay(isGregorian, year, fieldMaskParam);
  XLONG millis = julianDayToMillis(julianDay);

  // The following check handles portions of the cutover year BEFORE the
  // cutover itself happens. The check for the julianDate number is for a
  // rare case; it's a hard-coded number, but it's efficient.  The given
  // Julian day number corresponds to Dec 3, 292269055 BC, which
  // corresponds to millis near XLONG_MIN.  The need for the check
  // arises because for extremely negative Julian day numbers, the millis
  // actually overflow to be positive values. Without the check, the
  // initial date is interpreted with the Gregorian calendar, even when
  // the cutover doesn't warrant it.
  if (isGregorian != (millis >= normalizedGregorianCutover) && julianDay != -106749550580LL)
  { // See above
    fieldMaskParam[0] = fieldMask;
    julianDay = computeJulianDay(!isGregorian, year, fieldMaskParam);
    millis = julianDayToMillis(julianDay);
  }

  fieldMask = fieldMaskParam[0];

  // Do the time portion of the conversion.

  int millisInDay = 0;

  // Find the best set of fields specifying the time of day.  There
  // are only two possibilities here; the HOUR_OF_DAY or the
  // AM_PM and the HOUR.
  int hourOfDayStamp = stamp[HOUR_OF_DAY];
  int hourStamp = stamp[HOUR];
  int bestStamp = (hourStamp > hourOfDayStamp) ? hourStamp : hourOfDayStamp;

  // Hours
  if (bestStamp != UNSET)
  {
    if (bestStamp == hourOfDayStamp)
    {
      // Don't normalize here; let overflow bump into the next period.
      // This is consistent with how we handle rv fields.
      millisInDay += internalGet(HOUR_OF_DAY);
      fieldMask |= 1 << HOUR_OF_DAY;
    }
    else
    {
      // Don't normalize here; let overflow bump into the next period.
      // This is consistent with how we handle rv fields.
      millisInDay += internalGet(HOUR);
      fieldMask |= 1 << HOUR;

      // The default value of AM_PM is 0 which designates AM.
      if (stamp[AM_PM] != UNSET)
      {
        millisInDay += 12 * internalGet(AM_PM);
        fieldMask |= 1 << AM_PM;
      }
    }
  }

  millisInDay *= 60;
  if (stamp[MINUTE] != UNSET)
  {
    millisInDay += internalGet(MINUTE); // now have minutes
    fieldMask |= 1 << MINUTE;
  }
  millisInDay *= 60;
  if (stamp[SECOND] != UNSET)
  {
    millisInDay += internalGet(SECOND); // now have seconds
    fieldMask |= 1 << SECOND;
  }
  millisInDay *= 1000;
  if (stamp[MILLISECOND] != UNSET)
  {
    millisInDay += internalGet(MILLISECOND); // now have millis
    fieldMask |= 1 << MILLISECOND;
  }

  // Now add date and millisInDay together, to make millis contain local wall
  // millis, with no zone or DST adjustments
  millis += millisInDay;

  // Compute the time zone offset and DST offset.  There are two potential
  // ambiguities here.  We'll assume a 2:00 am (wall time) switchover time
  // for discussion purposes here.
  // 1. The transition into DST.  Here, a designated time of 2:00 am - 2:59 am
  //    can be in standard or in DST depending.  However, 2:00 am is an invalid
  //    representation (the representation jumps from 1:59:59 am Std to 3:00:00 am DST).
  //    We assume standard time.
  // 2. The transition out of DST.  Here, a designated time of 1:00 am - 1:59 am
  //    can be in standard or DST.  Both are valid representations (the rep
  //    jumps from 1:59:59 DST to 1:00:00 Std).
  //    Again, we assume standard time.
  // We use the TimeZone object, unless the user has explicitly set the ZONE_OFFSET
  // or DST_OFFSET fields; then we use those fields.
  const TimeZone* zoneValue = getTimeZone();

  XLONG zoneOffset = 0;
  if (stamp[ZONE_OFFSET] >= MINIMUM_USER_STAMP)
  {
    zoneOffset = internalGet(ZONE_OFFSET);
    fieldMask |= 1 << ZONE_OFFSET;
  }
  else
  {
    zoneOffset = zoneValue->getRawOffset();
  }
  if (stamp[DST_OFFSET] >= MINIMUM_USER_STAMP)
  {
    time = millis - (zoneOffset + internalGet(DST_OFFSET));
    fieldMask |= 1 << DST_OFFSET;
  }
  else
  {
    time = millis - zoneValue->getOffsets(millis - (XLONG) zoneOffset, 0);
  }

  // In lenient mode, we need to normalize the fields that have
  // any SET state (i.e., not UNSET) from the time value. First,
  // we calculate all field values and then discard values of
  // the UNSET fields. (4685354)
  if (isLenient())
  {
    computeFieldsImpl();
  }

  for (int i = 0; i < CALENDAR_FIELD_COUNT; i++)
  {
    if (isSet(i))
    {
      int bitMask = 1 << i;
      if ((fieldMask & bitMask) != bitMask)
      {
        internalClear(i);
      }
      else
      {
        stamp[i] = INTERNALLY_SET;
      }
    }
  }
}

void GregorianCalendar::computeFieldsImpl()
{
  const TimeZone* tz = getTimeZone();

  //printf("computeFieldsImpl::time ==>%lld\n",time);
  //printf("computeFieldsImpl::TimeZone ==>%s\n",tz->toString().c_str());

  vector<XLONG> offsets(2);
  XLONG offset = tz->getOffsets(time, &offsets);

  XLONG localMillis = time + offset; // here localMillis is wall

  /* Check for very extreme values -- millis near XLONG_MIN or
   * XLONG_MAX.  For these values, adding the zone offset can push
   * the millis past MAX_VALUE to MIN_VALUE, or vice versa.  This produces
   * the undesirable effect that the time can wrap around at the ends,
   * yielding, for example, a Date(XLONG_MAX) with a big BC year
   * (should be AD).  Handle this by pinning such values to XLONG_MIN
   * or XLONG_MAX. - liu 8/11/98 bug 4149677 */

  if (time > 0 && localMillis < 0 && offset > 0)
  {
    localMillis = XLONG_MAX;
  }
  else if (time < 0 && localMillis > 0 && offset < 0)
  {
    localMillis = XLONG_MIN;
  }

  // Time to fields takes the wall millis (Standard or DST).
  timeToFields(localMillis, false);

  XLONG days = floorDivide(localMillis, (XLONG)CalendarSystem::MILLIS_PER_DAY);
  int millisInDay = (int) (localMillis - (days * CalendarSystem::MILLIS_PER_DAY));
  if (millisInDay < 0)
  {
    millisInDay += (int)CalendarSystem::MILLIS_PER_DAY;
  }

  // Fill in all time-related fields based on millisInDay.  Call internalSet()
  // so as not to perturb flags.
  internalSet(MILLISECOND, millisInDay % 1000);
  millisInDay /= 1000;
  internalSet(SECOND, millisInDay % 60);
  millisInDay /= 60;
  internalSet(MINUTE, millisInDay % 60);
  millisInDay /= 60;
  internalSet(HOUR_OF_DAY, millisInDay);
  internalSet(AM_PM, millisInDay / 12); // Assume AM == 0
  internalSet(HOUR, millisInDay % 12);

  internalSet(ZONE_OFFSET, (int)offsets[0]);
  internalSet(DST_OFFSET, (int)offsets[1]);

}

void GregorianCalendar::timeToFields(XLONG theTime, bool quick)
{
  int rawYear, year, month, date, dayOfWeek, dayOfYear, weekCount, era;
  bool isLeap;

  //printf("GregorianCalendar::timeToFields ==>%lld\n",theTime);

  // Compute the year, month, and day of month from the given millis
  if (theTime >= normalizedGregorianCutover)
  {
    // The Gregorian epoch day is zero for Monday January 1, year 1.
    XLONG gregorianEpochDay = millisToJulianDay(theTime) - GregorianSystem::JAN_1_1_JULIAN_DAY;

    // Here we convert from the day number to the multiple radix
    // representation.  We use 400-year, 100-year, and 4-year cycles.
    // For example, the 4-year cycle has 4 years + 1 leap day; giving
    // 1461 == 365*4 + 1 days.
    int n400, n100, n4, n1;
    if (gregorianEpochDay > 0)
    {
      n400 = (int) (gregorianEpochDay / 146097);
      dayOfYear = (int) (gregorianEpochDay % 146097);
      n100 = dayOfYear / 36524;
      dayOfYear %= 36524;
      n4 = dayOfYear / 1461;
      dayOfYear %= 1461;
      n1 = dayOfYear / 365;
      dayOfYear %= 365; // zero-based day of year
    }
    else
    {
      vector<int> rem;

      n400 = floorDivide(gregorianEpochDay, 146097, rem); // 400-year cycle length
      n100 = floorDivide(rem[0], 36524, rem); // 100-year cycle length
      n4 = floorDivide(rem[0], 1461, rem); // 4-year cycle length
      n1 = floorDivide(rem[0], 365, rem);
      dayOfYear = rem[0]; // zero-based day of year
    }
    rawYear = 400 * n400 + 100 * n100 + 4 * n4 + n1;
    if (n100 == 4 || n1 == 4)
    {
      dayOfYear = 365; // Dec 31 at end of 4- or 400-yr cycle
    }
    else
    {
      ++rawYear;
    }

    isLeap = ((rawYear %4) == 0) && (rawYear % 100 != 0 || rawYear % 400 == 0);

    // Gregorian day zero is a Monday
    dayOfWeek = (int) ((gregorianEpochDay + 1) % 7);
  }
  else
  {
    // The Julian epoch day (not the same as Julian Day)
    // is zero on Saturday December 30, 0 (Gregorian).
    XLONG julianEpochDay = millisToJulianDay(theTime) - (GregorianSystem::JAN_1_1_JULIAN_DAY - 2);
    rawYear = (int) floorDivide(4 * julianEpochDay + 1464, (XLONG)1461);

    // Compute the Julian calendar day number for January 1, rawYear
    XLONG january1 = 365 * (rawYear - 1) + floorDivide(rawYear - 1, 4);
    dayOfYear = (int) (julianEpochDay - january1); // 0-based

    // Julian leap years occurred historically every 4 years starting
    // with 8 AD.  Before 8 AD the spacing is irregular; every 3 years
    // from 45 BC to 9 BC, and then none until 8 AD.  However, we don't
    // implement this historical detail; instead, we implement the
    // computationally cleaner proleptic calendar, which assumes
    // consistent 4-year cycles throughout time.
    isLeap = ((rawYear & 0x3) == 0); // equiv. to (rawYear%4 == 0)

    // Julian calendar day zero is a Saturday
    dayOfWeek = (int) ((julianEpochDay - 1) % 7);
  }

  // Common Julian/Gregorian calculation
  int correction = 0;
  int march1 = isLeap ? 60 : 59; // zero-based DOY for March 1
  if (dayOfYear >= march1)
  {
    correction = isLeap ? 1 : 2;
  }
  month = (12 * (dayOfYear + correction) + 6) / 367; // zero-based month

  // one-based DOM
  date = dayOfYear - (isLeap ? GregorianSystem::LEAP_NUM_DAYS[month] : GregorianSystem::NUM_DAYS[month]) + 1;

  // Normalize day of week
  dayOfWeek += (dayOfWeek < 0) ? (SUNDAY + 7) : SUNDAY;

  era = AD;
  year = rawYear;
  if (year < 1)
  {
    era = BC;
    year = 1 - year;
  }

  internalSet(ERA, era);
  internalSet(YEAR, year);
  internalSet(MONTH, month + JANUARY); // 0-based
  internalSet(DATE, date);
  internalSet(DAY_OF_WEEK, dayOfWeek);
  internalSet(DAY_OF_YEAR, ++dayOfYear); // Convert from 0-based to 1-based
  if (quick)
  {
    return;
  }

  // WEEK_OF_YEAR start
  // Compute the week of the year.  Valid week numbers run from 1 to 52
  // or 53, depending on the year, the first day of the week, and the
  // minimal days in the first week.  Days at the start of the year may
  // fall into the last week of the previous year; days at the end of
  // the year may fall into the first week of the next year.
  int relDow = (dayOfWeek + 7 - getFirstDayOfWeek()) % 7; // 0..6
  int relDowJan1 = (dayOfWeek - dayOfYear + 701 - getFirstDayOfWeek()) % 7; // 0..6
  int woy = (dayOfYear - 1 + relDowJan1) / 7; // 0..53
  if ((7 - relDowJan1) >= getMinimalDaysInFirstWeek())
  {
    ++woy;
  }

  // XXX: The calculation of dayOfYear does not take into account 
  // Gregorian cut over date. The next if statement depends on that 
  // assumption.
  if (dayOfYear > 359)
  { // Fast check which eliminates most cases
    // Check to see if we are in the last week; if so, we need
    // to handle the case in which we are the first week of the
    // next year.
    int lastDoy = yearLength();
    int lastRelDow = (relDow + lastDoy - dayOfYear) % 7;
    if (lastRelDow < 0)
    {
      lastRelDow += 7;
    }
    if (((6 - lastRelDow) >= getMinimalDaysInFirstWeek()) && ((dayOfYear + 7 - relDow) > lastDoy))
    {
      woy = 1;
    }
  }
  else if (woy == 0)
  {
    // We are the last week of the previous year.
    int prevDoy = dayOfYear + yearLength(rawYear - 1);
    woy = weekNumber(prevDoy, dayOfWeek);
  }
  internalSet(WEEK_OF_YEAR, woy);
  // WEEK_OF_YEAR end

  internalSet(WEEK_OF_MONTH, weekNumber(date, dayOfWeek));
  internalSet(DAY_OF_WEEK_IN_MONTH, (date - 1) / 7 + 1);
}

XLONG GregorianCalendar::computeJulianDay(bool isGregorian, int year,vector<int>& fieldMaskParam)
{
  int month = 0, date = 0, y;
  XLONG millis = 0;

  // bit masks to remember which fields have been used to
  // determine the date
  int fieldMask = fieldMaskParam[0];

  // Find the most recent group of fields specifying the day within
  // the year.  These may be any of the following combinations:
  //   MONTH + DAY_OF_MONTH
  //   MONTH + WEEK_OF_MONTH + DAY_OF_WEEK
  //   MONTH + DAY_OF_WEEK_IN_MONTH + DAY_OF_WEEK
  //   DAY_OF_YEAR
  //   WEEK_OF_YEAR + DAY_OF_WEEK
  // We look for the most recent of the fields in each group to determine
  // the age of the group.  For groups involving a week-related field such
  // as WEEK_OF_MONTH, DAY_OF_WEEK_IN_MONTH, or WEEK_OF_YEAR, both the
  // week-related field and the DAY_OF_WEEK must be set for the group as a
  // whole to be considered.  (See bug 4153860 - liu 7/24/98.)
  int dowStamp = stamp[DAY_OF_WEEK];
  int monthStamp = stamp[MONTH];
  int domStamp = stamp[DAY_OF_MONTH];
  int womStamp = aggregateStamp(stamp[WEEK_OF_MONTH], dowStamp);
  int dowimStamp = aggregateStamp(stamp[DAY_OF_WEEK_IN_MONTH], dowStamp);
  int doyStamp = stamp[DAY_OF_YEAR];
  int woyStamp = aggregateStamp(stamp[WEEK_OF_YEAR], dowStamp);

  int bestStamp = domStamp;
  if (womStamp > bestStamp)
  {
    bestStamp = womStamp;
  }
  if (dowimStamp > bestStamp)
  {
    bestStamp = dowimStamp;
  }
  if (doyStamp > bestStamp)
  {
    bestStamp = doyStamp;
  }
  if (woyStamp > bestStamp)
  {
    bestStamp = woyStamp;
  }

  /* No complete combination exists.  Look for WEEK_OF_MONTH,
   * DAY_OF_WEEK_IN_MONTH, or WEEK_OF_YEAR alone.  Treat DAY_OF_WEEK alone
   * as DAY_OF_WEEK_IN_MONTH.
   */
  if (bestStamp == UNSET)
  {
    womStamp = stamp[WEEK_OF_MONTH];
    dowimStamp = __M_max(stamp[DAY_OF_WEEK_IN_MONTH], dowStamp);
    woyStamp = stamp[WEEK_OF_YEAR];
    bestStamp = __M_max(__M_max(womStamp, dowimStamp), woyStamp);

    /* Treat MONTH alone or no fields at all as DAY_OF_MONTH.  This may
     * result in bestStamp = domStamp = UNSET if no fields are set,
     * which indicates DAY_OF_MONTH.
     */
    if (bestStamp == UNSET)
    {
      bestStamp = domStamp = monthStamp;
    }
  }

  bool useMonth = false;

  if (bestStamp == domStamp
    || (bestStamp == womStamp && stamp[WEEK_OF_MONTH] >= stamp[WEEK_OF_YEAR])
    || (bestStamp == dowimStamp && stamp[DAY_OF_WEEK_IN_MONTH] >= stamp[WEEK_OF_YEAR]))
  {
    useMonth = true;

    // We have the month specified. Make it 0-based for the algorithm.
    month = (monthStamp != UNSET) ? internalGet(MONTH) - JANUARY : 0;

    // If the month is out of range, adjust it into range
    if (month < 0 || month > 11)
    {
                        vector<int> rem(1);
      year += floorDivide(month, 12, rem);
      month = rem[0];
    }

    // Set the MONTH field mask because it's been determined
    // to use the MONTH field.
    fieldMask |= 1 << MONTH;
  }

  bool isLeap = year % 4 == 0;
  y = year - 1;
  XLONG julianDay = 365L * y + floorDivide(y, 4) + (GregorianSystem::JAN_1_1_JULIAN_DAY - 3);

  if (isGregorian)
  {
    isLeap = isLeap && ((year % 100 != 0) || (year % 400 == 0));
    // Add 2 because Gregorian calendar starts 2 days after Julian calendar
    julianDay += floorDivide(y, 400) - floorDivide(y, 100) + 2;
  }

  // At this point julianDay is the 0-based day BEFORE the first day of
  // January 1, year 1 of the given calendar.  If julianDay == 0, it
  // specifies (Jan. 1, 1) - 1, in whatever calendar we are using (Julian
  // or Gregorian).

  if (useMonth)
  {

    julianDay += isLeap ? GregorianSystem::LEAP_NUM_DAYS[month] : GregorianSystem::NUM_DAYS[month];

    if (bestStamp == domStamp)
    {
      if (stamp[DAY_OF_MONTH] != UNSET)
      {
        date = internalGet(DAY_OF_MONTH);
        fieldMask |= 1 << DAY_OF_MONTH;
      }
      else
      {
        date = 1;
      }
    }
    else
    { // assert(bestStamp == womStamp || bestStamp == dowimStamp)
      // Compute from day of week plus week number or from the day of
      // week plus the day of week in month.  The computations are
      // almost identical.

      // Find the day of the week for the first of this month.  This
      // is zero-based, with 0 being the locale-specific first day of
      // the week.  Add 1 to get the 1st day of month.  Subtract
      // getFirstDayOfWeek() to make 0-based.
      int fdm = julianDayToDayOfWeek(julianDay + 1) - getFirstDayOfWeek();
      if (fdm < 0)
      {
        fdm += 7;
      }

      // Find the start of the first week.  This will be a date from
      // 0..6.  It represents the locale-specific first day of the
      // week of the first day of the month, ignoring minimal days in
      // first week.
      int normalizedDayOfWeek = 0;
      if (dowStamp != UNSET)
      {
        normalizedDayOfWeek = internalGet(DAY_OF_WEEK) - getFirstDayOfWeek();
        if (normalizedDayOfWeek < 0)
        {
          normalizedDayOfWeek += 7;
        }
        fieldMask |= 1 << DAY_OF_WEEK;
      }
      date = 1 - fdm + normalizedDayOfWeek;

      if (bestStamp == womStamp)
      {
        // Adjust for minimal days in first week.
        if ((7 - fdm) < getMinimalDaysInFirstWeek())
        {
          date += 7;
        }

        // Now adjust for the week number.
        date += 7 * (internalGet(WEEK_OF_MONTH) - 1);
        fieldMask |= 1 << WEEK_OF_MONTH;
      }
      else
      { // assert(bestStamp == dowimStamp)
        // Adjust into the month, if needed.
        if (date < 1)
        {
          date += 7;
        }

        // We are basing this on the day-of-week-in-month.  The only
        // trickiness occurs if the day-of-week-in-month is
        // negative.
        int dim;
        if (stamp[DAY_OF_WEEK_IN_MONTH] != UNSET)
        {
          dim = internalGet(DAY_OF_WEEK_IN_MONTH);
          fieldMask |= 1 << DAY_OF_WEEK_IN_MONTH;
        }
        else
        {
          dim = 1;
        }
        if (dim >= 0)
        {
          date += 7 * (dim - 1);
        }
        else
        {
          // Move date to the last of this day-of-week in this
          // month, then back up as needed.  If dim==-1, we don't
          // back up at all.  If dim==-2, we back up once, etc.
          // Don't back up past the first of the given day-of-week
          // in this month.  Note that we handle -2, -3,
          // etc. correctly, even though values < -1 are
          // technically disallowed.
          date += ((monthLength(month, year) - date) / 7 + dim + 1) * 7;
        }
      }
    }

    julianDay += date;
  }
  else
  {
    // assert(bestStamp == doyStamp || bestStamp == woyStamp ||
    // bestStamp == UNSET).  In the last case we should use January 1.

    // No month, start with January 0 (day before Jan 1), then adjust.

    if (bestStamp == doyStamp)
    {
      julianDay += internalGet(DAY_OF_YEAR);
      fieldMask |= 1 << DAY_OF_YEAR;
    }
    else
    { // assert(bestStamp == woyStamp)
      // Compute from day of week plus week of year

      // Find the day of the week for the first of this year.  This
      // is zero-based, with 0 being the locale-specific first day of
      // the week.  Add 1 to get the 1st day of month.  Subtract
      // getFirstDayOfWeek() to make 0-based.
      int fdy = julianDayToDayOfWeek(julianDay + 1) - getFirstDayOfWeek();
      if (fdy < 0)
      {
        fdy += 7;
      }

      // Find the start of the first week.  This may be a valid date
      // from -5..7. It represents the locale-specific first day of
      // the week of the first day of the year.
      int normalizedDayOfWeek = 0;
      if (dowStamp != UNSET)
      {
        normalizedDayOfWeek = internalGet(DAY_OF_WEEK) - getFirstDayOfWeek();
        if (normalizedDayOfWeek < 0)
        {
          normalizedDayOfWeek += 7;
        }
        fieldMask |= 1 << DAY_OF_WEEK;
      }
      date = 1 - fdy + normalizedDayOfWeek;

      // Adjust for minimal days in first week.
      if ((7 - fdy) < getMinimalDaysInFirstWeek())
      {
        date += 7;
      }

      // Now adjust for the week number.
      date += 7 * (internalGet(WEEK_OF_YEAR) - 1);
      fieldMask |= 1 << WEEK_OF_YEAR;

      julianDay += date;
    }
  }

  fieldMaskParam[0] = fieldMask;
  return julianDay;
}

XLONG  GregorianCalendar::millisToJulianDay(XLONG millis)
{
  return GregorianSystem::EPOCH_JULIAN_DAY + floorDivide(millis, (XLONG)CalendarSystem::MILLIS_PER_DAY);
}

XLONG GregorianCalendar::julianDayToMillis(XLONG julian)
{
  return (julian - GregorianSystem::EPOCH_JULIAN_DAY) * CalendarSystem::MILLIS_PER_DAY;
}

int GregorianCalendar::julianDayToDayOfWeek(XLONG julian)
{
  // If julian is negative, then julian%7 will be negative, so we adjust
  // accordingly.  We add 1 because Julian day 0 is Monday.
  int dayOfWeek = (int) ((julian + 1) % 7);
  return dayOfWeek + ((dayOfWeek < 0) ? (7 + SUNDAY) : SUNDAY);
}

XLONG GregorianCalendar::floorDivide(XLONG numerator, XLONG denominator)
{
  // We do this computation in order to handle
  // a numerator of XLONG_MIN correctly
  return (numerator >= 0) ? numerator / denominator : ((numerator + 1) / denominator) - 1;
}

int GregorianCalendar::floorDivide(int numerator, int denominator)
{
  // We do this computation in order to handle
  // a numerator of Integer.MIN_VALUE correctly
  return (numerator >= 0) ? numerator / denominator : ((numerator + 1) / denominator) - 1;
}

int GregorianCalendar::floorDivide(int numerator, int denominator, vector<int>& remainder)
{
  if (numerator >= 0)
  {
    remainder[0] = numerator % denominator;
    return numerator / denominator;
  }
  int quotient = ((numerator + 1) / denominator) - 1;

  if(remainder.empty())
  {
    remainder.resize(1);
  }
  remainder[0] = ( numerator - (quotient * denominator));

  return quotient;
}

int GregorianCalendar::floorDivide(XLONG numerator, int denominator, vector<int>& remainder)
{
  if (numerator >= 0)
  {
    remainder[0] = (int) (numerator % denominator);
    return (int) (numerator / denominator);
  }
  int quotient = (int) (((numerator + 1) / denominator) - 1);

  if(remainder.empty())
  {
    remainder.resize(1);
  }

  remainder[0] = ((int) (numerator - (quotient * denominator)));

  return quotient;
}

int GregorianCalendar::aggregateStamp(int stamp_a, int stamp_b)
{
  return (stamp_a != UNSET && stamp_b != UNSET) ? __M_max(stamp_a, stamp_b) : UNSET;
}

int GregorianCalendar::weekNumber(int dayOfPeriod, int dayOfWeek)
{
  // Determine the day of the week of the first day of the period
  // in question (either a year or a month).  Zero represents the
  // first day of the week on this calendar.
  int periodStartDayOfWeek = (dayOfWeek - getFirstDayOfWeek() - dayOfPeriod + 1) % 7;
  if (periodStartDayOfWeek < 0)
  {
    periodStartDayOfWeek += 7;
  }

  // Compute the week number.  Initially, ignore the first week, which
  // may be fractional (or may not be).  We add periodStartDayOfWeek in
  // order to fill out the first week, if it is fractional.
  int weekNo = (dayOfPeriod + periodStartDayOfWeek - 1) / 7;

  // If the first week is long enough, then count it.  If
  // the minimal days in the first week is one, or if the period start
  // is zero, we always increment weekNo.
  if ((7 - periodStartDayOfWeek) >= getMinimalDaysInFirstWeek())
  {
    ++weekNo;
  }

  return weekNo;
}

int GregorianCalendar::monthLength(int month, int year)
{
  return isLeapYear(year) ? GregorianSystem::LEAP_MONTH_LENGTH[month] : GregorianSystem::MONTH_LENGTH[month];
}

int GregorianCalendar::monthLength(int month)
{
  int year = internalGet(YEAR);
  if (internalGetEra() == BC)
  {
    year = 1 - year;
  }
  return monthLength(month, year);
}

int GregorianCalendar::prevMonthLength(int month)
{
  return (month > 1) ? monthLength(month - 1) : 31;
}

int GregorianCalendar::yearLength(int year)
{
  return isLeapYear(year) ? 366 : 365;
}

int GregorianCalendar::yearLength()
{
  return isLeapYear(internalGet(YEAR)) ? 366 : 365;
}

void GregorianCalendar::pinDayOfMonth()
{
  int monthLen = monthLength(internalGet(MONTH));
  int dom = internalGet(DAY_OF_MONTH);
  if (dom > monthLen)
  {
    set(DAY_OF_MONTH, monthLen);
  }
}

bool GregorianCalendar::validateFields()
{
  for (int field = 0; field < CALENDAR_FIELD_COUNT; field++)
  {
    // Ignore DATE and DAY_OF_YEAR which are handled below
    if (field != DATE && field != DAY_OF_YEAR && isSet(field) && !boundsCheck(internalGet(field), field))
    {
      return false;
    }
  }

  // Values differ in Least-Maximum and Maximum should be handled
  // specially.
  if (stamp[DATE] >= MINIMUM_USER_STAMP)
  {
    int date = internalGet(DATE);
    if (date < getMinimum(DATE) || date > monthLength(internalGet(MONTH)))
    {
      return false;
    }
  }

  if (stamp[DAY_OF_YEAR] >= MINIMUM_USER_STAMP)
  {
    int days = internalGet(DAY_OF_YEAR);
    if (days < 1 || days > yearLength())
    {
      return false;
    }
  }

  // Handle DAY_OF_WEEK_IN_MONTH, which must not have the value zero.
  // We've checked against minimum and maximum above already.
  if (isSet(DAY_OF_WEEK_IN_MONTH) && 0 == internalGet(DAY_OF_WEEK_IN_MONTH))
  {
    return false;
  }

  return true;
}

bool GregorianCalendar::boundsCheck(int value, int field)
{
  return value >= getMinimum(field) && value <= getMaximum(field);
}

XLONG GregorianCalendar::getEpochDay()
{
  complete();
  // Divide by 1000 (convert to seconds) in order to prevent overflow when
  // dealing with Date(XLONG_MIN) and Date(XLONG_MAX).
  XLONG wallSec = time / 1000 + (internalGet(ZONE_OFFSET) + internalGet(DST_OFFSET)) / 1000;
  return floorDivide(wallSec, (XLONG)CalendarSystem::MILLIS_PER_DAY / 1000);
}


int GregorianCalendar::internalGetEra()
{
  return isSet(ERA) ? internalGet(ERA) : AD;
}

__OPENCODE_END_NAMESPACE

