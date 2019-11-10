#include "port/calendar.h"
#include "port/gregorianCalendar.h"
#include "port/buddhistCalendar.h"
#include "port/libsys.h"
#include "port/ioc_sbuf.h"

USING_OPENCODE_STL_NS
__OPENCODE_BEGIN_NAMESPACE

const XLONG CalendarSystem::MILLIS_PER_SECOND = 1 * 1000;
const XLONG CalendarSystem::MILLIS_PER_MINUTE = 60 * MILLIS_PER_SECOND;
const XLONG CalendarSystem::MILLIS_PER_HOUR = 60 * MILLIS_PER_MINUTE;
  
const XLONG CalendarSystem::MILLIS_PER_DAY = 24 * MILLIS_PER_HOUR;
const XLONG CalendarSystem::MILLIS_PER_WEEK = 7 * MILLIS_PER_DAY;

const int CalendarSystem::SUNDAY = 1;
const int CalendarSystem::MONDAY = 2;
const int CalendarSystem::TUESDAY = 3;
const int CalendarSystem::WEDNESDAY = 4;
const int CalendarSystem::THURSDAY = 5;
const int CalendarSystem::FRIDAY = 6;
const int CalendarSystem::SATURDAY = 7;


const int CalendarSystem::JANUARY = 0;
const int CalendarSystem::FEBRUARY = 1;
const int CalendarSystem::MARCH = 2;
const int CalendarSystem::APRIL = 3;
const int CalendarSystem::MAY = 4;
const int CalendarSystem::JUNE = 5;
const int CalendarSystem::JULY = 6;
const int CalendarSystem::AUGUST = 7;
const int CalendarSystem::SEPTEMBER = 8;
const int CalendarSystem::OCTOBER = 9;
const int CalendarSystem::NOVEMBER = 10;
const int CalendarSystem::DECEMBER = 11;
const int CalendarSystem::UNDECIMBER = 12;

const int CalendarSystem::AM = 0;
const int CalendarSystem::PM = 1;  

CalendarDate::CalendarDate()
{
  year = 0;
  month = 0;
  mday = 0;
  dow = 0;
  millis = 0;
}

CalendarDate::CalendarDate(int yearValue, int monthValue, int dateValue) 
{
  this->year = yearValue;
  this->month = monthValue;
  this->mday = dateValue;
  
  dow = 0;
  millis = 0;
}

void CalendarDate::setYear(int yearValue) 
{
  this->year = yearValue;
}

int CalendarDate::getYear() const
{
  return year;
}

void CalendarDate::setMonth(int monthValue) 
{
  this->month = monthValue;
}

int CalendarDate::getMonth() const
{
  return month;
}

void CalendarDate::setDate(int dateValue) 
{
  mday = dateValue;
}

int CalendarDate::getDate() const
{
  return mday;
}

void CalendarDate::setDayOfWeek(int dayOfWeekValue)
{
  dow = dayOfWeekValue;
}

int CalendarDate::getDayOfWeek() const
{
  return dow;
}

void CalendarDate::setTimeOfDay(XLONG timeValue) 
{
  millis = timeValue;
}

XLONG CalendarDate::getTimeOfDay() const
{
  return millis;
}

//---------------------------------------------------------------------------------

const int Calendar::UNSET = CALENDAR_UNSET;
const int Calendar::INTERNALLY_SET = CALENDAR_INTERNALLY_SET;
const int Calendar::MINIMUM_USER_STAMP = 2;

const int Calendar::ERA = CALENDAR_ERA;
const int Calendar::YEAR = CALENDAR_YEAR;

  const int Calendar::MONTH = CALENDAR_MONTH;

  const int Calendar::WEEK_OF_YEAR = CALENDAR_WEEK_OF_YEAR;

  const int Calendar::WEEK_OF_MONTH = CALENDAR_WEEK_OF_MONTH;

  const int Calendar::DATE = CALENDAR_DATE;
 
  const int Calendar::DAY_OF_MONTH = CALENDAR_DAY_OF_MONTH;

  const int Calendar::DAY_OF_YEAR = CALENDAR_DAY_OF_YEAR;

  const int Calendar::DAY_OF_WEEK = CALENDAR_DAY_OF_WEEK;

  const int Calendar::DAY_OF_WEEK_IN_MONTH = CALENDAR_DAY_OF_WEEK_IN_MONTH;

  const int Calendar::AM_PM = CALENDAR_AM_PM;

  const int Calendar::HOUR = CALENDAR_HOUR;

  const int Calendar::HOUR_OF_DAY = CALENDAR_HOUR_OF_DAY;

  const int Calendar::MINUTE = CALENDAR_MINUTE;

  const int Calendar::SECOND = CALENDAR_SECOND;

  const int Calendar::MILLISECOND = CALENDAR_MILLISECOND;

  const int Calendar::ZONE_OFFSET = CALENDAR_ZONE_OFFSET;

  const int Calendar::DST_OFFSET = CALENDAR_DST_OFFSET;
  /**
   * The number of distinct fields recognized by <code>get</code> and <code>set</code>.
   * Field numbers range from <code>0..CALENDAR_FIELD_COUNT-1</code>.
   */
 // const int Calendar::FIELD_COUNT = 17;

  /**
   * Value of the <code>DAY_OF_WEEK</code> field indicating
   * Sunday.
   */
  const int Calendar::SUNDAY = 1;
  /**
   * Value of the <code>DAY_OF_WEEK</code> field indicating
   * Monday.
   */
  const int Calendar::MONDAY = 2;
  /**
   * Value of the <code>DAY_OF_WEEK</code> field indicating
   * Tuesday.
   */
  const int Calendar::TUESDAY = 3;
  /**
   * Value of the <code>DAY_OF_WEEK</code> field indicating
   * Wednesday.
   */
  const int Calendar::WEDNESDAY = 4;
  /**
   * Value of the <code>DAY_OF_WEEK</code> field indicating
   * Thursday.
   */
  const int Calendar::THURSDAY = 5;
  /**
   * Value of the <code>DAY_OF_WEEK</code> field indicating
   * Friday.
   */
  const int Calendar::FRIDAY = 6;
  /**
   * Value of the <code>DAY_OF_WEEK</code> field indicating
   * Saturday.
   */
  const int Calendar::SATURDAY = 7;

  /**
   * Value of the <code>MONTH</code> field indicating the
   * first month of the year.
   */
  const int Calendar::JANUARY = 0;
  /**
   * Value of the <code>MONTH</code> field indicating the
   * second month of the year.
   */
  const int Calendar::FEBRUARY = 1;
  /**
   * Value of the <code>MONTH</code> field indicating the
   * third month of the year.
   */
  const int Calendar::MARCH = 2;
  /**
   * Value of the <code>MONTH</code> field indicating the
   * fourth month of the year.
   */
  const int Calendar::APRIL = 3;
  /**
   * Value of the <code>MONTH</code> field indicating the
   * fifth month of the year.
   */
  const int Calendar::MAY = 4;
 
  const int Calendar::JUNE = 5;

  const int Calendar::JULY = 6;
 
  const int Calendar::AUGUST = 7;
 
  const int Calendar::SEPTEMBER = 8;

  const int Calendar::OCTOBER = 9;

  const int Calendar::NOVEMBER = 10;
 
  const int Calendar::DECEMBER = 11;
 
  const int Calendar::UNDECIMBER = 12;

  const int Calendar::AM = 0;

  const int Calendar::PM = 1;

const char Calendar::FIELD_NAME[CALENDAR_FIELD_COUNT][30] = {
        ",ERA=", ",YEAR=", ",MONTH=", ",WEEK_OF_YEAR=", ",WEEK_OF_MONTH=", ",DAY_OF_MONTH=",
        ",DAY_OF_YEAR=", ",DAY_OF_WEEK=", ",DAY_OF_WEEK_IN_MONTH=", ",AM_PM=", ",HOUR=",
        ",HOUR_OF_DAY=", ",MINUTE=", ",SECOND=", ",MILLISECOND=", ",ZONE_OFFSET=",
        ",DST_OFFSET=" };
 
//---------------------------------------------------------------------------------

Calendar*  Calendar::clone() const
{
  //delegate to doClone
  Calendar* pClone = doClone();
  return pClone;
}

string Calendar::toString() const
{
  StringBuffer buffer("Calendar");
  buffer.append("[time=");

  if(isTimeSet)
  {
    buffer.append(time);
  }
  else
  {
    buffer.append("?");
  }
  buffer.append(",areFieldsSet=");
  buffer.append(areFieldsSet);
  buffer.append(",areAllFieldsSet=");
  buffer.append(areAllFieldsSet);
  buffer.append(",lenient=");
  buffer.append(lenient);
  buffer.append(",zone=");
  buffer.append(zone->toString());
  buffer.append(",firstDayOfWeek=");
  buffer.append(firstDayOfWeek);
  buffer.append(",minimalDaysInFirstWeek=");
  buffer.append(minimalDaysInFirstWeek);

  for (int i=0; i<CALENDAR_FIELD_COUNT; ++i)
  {
     buffer.append(FIELD_NAME[i]);

     if(isSet(i))
     {
       buffer.append(fields[i]);
     }
     else
     {
       buffer.append("?");
     }
  }
  buffer.append(']');

  return buffer.str();
}

Calendar::~Calendar()
{
  if(zone != 0)
  {
    delete zone;
    zone = 0;
  }
}

Calendar::Calendar(const Calendar& rhs)
{
  for(int i=0;i<CALENDAR_FIELD_COUNT;++i)
  {
    this->fields[i]=rhs.fields[i];
    this->stamp[i]=rhs.stamp[i];
  }
  
  this->time = rhs.time;
  this->isTimeSet = rhs.isTimeSet;
  this->areFieldsSet = rhs.areFieldsSet;
  this->areAllFieldsSet = rhs.areAllFieldsSet;
  
  this->lenient = rhs.lenient;

  TimeZone* p_zone = 0;
  if(rhs.zone != 0)
  {
    p_zone  = rhs.zone->clone();
  }
  else
  {
    p_zone = TimeZone::getDefault()->clone();
  }

  this->zone = p_zone;

  this->firstDayOfWeek = rhs.firstDayOfWeek;
  this->minimalDaysInFirstWeek = rhs.minimalDaysInFirstWeek;
  this->nextStamp = rhs.nextStamp;
}

Calendar& Calendar::operator=(const Calendar& rhs)
{
  if(&rhs == this)
  {
    return *this;
  }

  for(int i=0;i<CALENDAR_FIELD_COUNT;++i)
  {
    this->fields[i]=rhs.fields[i];
    this->stamp[i]=rhs.stamp[i];
  }

  this->time = rhs.time;
  this->isTimeSet = rhs.isTimeSet;
  this->areFieldsSet = rhs.areFieldsSet;
  this->areAllFieldsSet = rhs.areAllFieldsSet;

  this->lenient = rhs.lenient;

  TimeZone* p_zone = 0;
  if(rhs.zone != 0)
  {
    p_zone  = rhs.zone->clone();
  }
  else
  {
    p_zone = TimeZone::getDefault()->clone();
  }

  if(this->zone != 0)
  {
    delete this->zone;
    this->zone = 0;
  }
  
  this->zone = p_zone;

  this->firstDayOfWeek = rhs.firstDayOfWeek;
  this->minimalDaysInFirstWeek = rhs.minimalDaysInFirstWeek;
  this->nextStamp = rhs.nextStamp;

  return *this;
}

Calendar::Calendar()
{
  for(int i=0;i<CALENDAR_FIELD_COUNT;++i)
  {
    fields[i] = 0;
    stamp[i] = UNSET;
  }
  
  this->time = 0; 
  this->isTimeSet = false;
  this->areFieldsSet = false;
  this->areAllFieldsSet = false;  
  this->lenient = true;
  
  this->nextStamp = MINIMUM_USER_STAMP;
  
  this->zone = TimeZone::getDefault()->clone();
  
  setWeekCountData(Locale::getDefault()); 
}

Calendar::Calendar(const TimeZone* aTimeZone, const Locale& aLocale)
{
  for(int i=0;i<CALENDAR_FIELD_COUNT;++i)
  {
    fields[i] = 0;
    stamp[i] = UNSET;
  }
  
  this->time = 0;
  this->isTimeSet = false;
  this->areFieldsSet = false;
  this->areAllFieldsSet = false;  
  this->lenient = true;
  
  this->nextStamp = MINIMUM_USER_STAMP;
  
  if(aTimeZone == 0)
  {
    this->zone = TimeZone::getDefault()->clone();
  }
  else
  {
    this->zone = aTimeZone->clone();
  }
  
  setWeekCountData(aLocale);
}

Calendar* Calendar::getInstance()
{
  return createCalendar(TimeZone::getDefault(), Locale::getDefault());
}

Calendar* Calendar::getInstance(const TimeZone* aTimeZone)
{
  return createCalendar(aTimeZone, Locale::getDefault());
}

Calendar* Calendar::getInstance(const Locale& aLocale)
{
  return createCalendar(TimeZone::getDefault(), aLocale);
}

Calendar* Calendar::getInstance(const TimeZone* aTimeZone,const Locale& aLocale)
{
  return createCalendar(aTimeZone, aLocale);
}

Calendar* Calendar::createCalendar(const TimeZone* aTimeZone,const Locale& aLocale)
{
  if (aLocale.getLanguage().compare("th") == 0)
  {
    if (aLocale.getCountry().compare("TH") == 0)
    {
      return new BuddhistCalendar(aTimeZone, aLocale);
    }
  }

  return new GregorianCalendar(aTimeZone, aLocale);
}

XLONG Calendar::getTimeInMillis() const
{
  if (!isTimeSet)
  {
    Calendar* p_cal = (Calendar*)(this);    
    p_cal->updateTime();
  }
  
  return time;
}

void Calendar::setTimeInMillis(XLONG millis)
{
  if(this->time == millis)
  {
    return;
  }

  isTimeSet = true;
  time = millis;
  areFieldsSet = false;

  if (!areFieldsSet)
  {
    computeFields();
    
    areFieldsSet = true;
    areAllFieldsSet = true;
  }
}

int Calendar::get(int field) const
{
  Calendar* p_cal = (Calendar*)(this);    
  p_cal->complete();
  
  return internalGet(field);
}

int Calendar::internalGet(int field) const
{
  return fields[field];
}

void Calendar::internalSet(int field, int value)
{
  fields[field] = value;
}

void Calendar::internalClear(int field)
{
  fields[field] = 0;
  stamp[field] = UNSET;
  
  areFieldsSet = false;
  areAllFieldsSet = false;
}

void Calendar::set(int field, int value)
{
  isTimeSet = false;
  internalSet(field, value);
  
  //fields[field] = value;
  stamp[field] = nextStamp++;
  if (nextStamp == INT_MAX)
  {
    adjustStamp();
  }
  areFieldsSet = false;
}

void Calendar::set(int year, int month, int date)
{
  set(YEAR, year);
  set(MONTH, month);
  set(DATE, date);
}

void Calendar::set(int year, int month, int date, int hour, int minute)
{
  set(YEAR, year);
  set(MONTH, month);
  set(DATE, date);
  set(HOUR_OF_DAY, hour);
  set(MINUTE, minute);
}

void Calendar::set(int year, int month, int date, int hour, int minute, int second)
{
  set(YEAR, year);
  set(MONTH, month);
  set(DATE, date);
  set(HOUR_OF_DAY, hour);
  set(MINUTE, minute);
  set(SECOND, second);
}


void Calendar::clear()
{
  areFieldsSet = false;
  areAllFieldsSet = false;
  isTimeSet = false;
}

void Calendar::clear(int field)
{
  fields[field] = 0;
  stamp[field] = UNSET;
  
  areFieldsSet = false;
  areAllFieldsSet = false;
  isTimeSet = false;
}


bool Calendar::isSet(int field) const
{
  return stamp[field] != UNSET;
}

void Calendar::complete()
{
  if (!isTimeSet)
  {
    updateTime();
  }
  
  if (!areFieldsSet)
  {
    computeFields(); // fills in unset fields
    areFieldsSet = true;
    areAllFieldsSet = true;
  }
}

void Calendar::roll(int field, int amount)
{
  while (amount > 0)
  {
    roll(field, true);
    amount--;
  }
  while (amount < 0)
  {
    roll(field, false);
    amount++;
  }
}

void Calendar::setTimeZone(const TimeZone* aTimeZone) 
{
  if(aTimeZone == 0 ||  aTimeZone == zone)
  {
    return;
  }
  
  if(zone != 0)
  {
    delete zone;
  }
  
  zone = aTimeZone->clone();
  
  /* Recompute the fields from the time using the new zone.  This also
   * works if isTimeSet is false (after a call to set()).  In that case
   * the time will be computed from the fields using the new zone, then
   * the fields will get recomputed from that.  Consider the sequence of
   * calls: cal.setTimeZone(EST); cal.set(HOUR, 1); cal.setTimeZone(PST).
   * Is cal set to 1 o'clock EST or 1 o'clock PST?  Answer: PST.  More
   * generally, a call to setTimeZone() affects calls to set() BEFORE AND
   * AFTER it up to the next call to complete().
   */
  areFieldsSet = false;
}


const TimeZone* Calendar::getTimeZone() const
{
  return zone;
}

void Calendar::setLenient(bool lenientValue)
{
  this->lenient = lenientValue;
}

bool Calendar::isLenient() const
{
  return lenient;
}

void Calendar::setFirstDayOfWeek(int value)
{
  if (firstDayOfWeek == value)
  {
    return;
  }
  
  firstDayOfWeek = value;
  invalidateWeekFields();
}

int Calendar::getFirstDayOfWeek() const
{
  return firstDayOfWeek;
}

void Calendar::setMinimalDaysInFirstWeek(int value) 
{
  if (minimalDaysInFirstWeek == value)
  {
    return;
  }
  minimalDaysInFirstWeek = value;
  invalidateWeekFields();
}

int Calendar::getMinimalDaysInFirstWeek() const
{
  return minimalDaysInFirstWeek;
}

int Calendar::getActualMinimum(int field) const
{
  int fieldValue = getGreatestMinimum(field);
  int endValue = getMinimum(field);

  // if we know that the minimum value is always the same, just return it
  if (fieldValue == endValue)
  {
    return fieldValue;
  }

  // clone the calendar so we don't mess with the real one, and set it to
  // accept anything for the field values
  Calendar* work = this->clone();
  work->setLenient(true);

  // now try each value from getLeastMaximum() to getMaximum() one by one until
  // we get a value that normalizes to another value.  The last value that
  // normalizes to itself is the actual minimum for the current date
  int result = fieldValue;

  do
  {
    work->set(field, fieldValue);
    if (work->get(field) != fieldValue)
    {
      break;
    }
    else
    {
      result = fieldValue;
      fieldValue--;
    }
  }
  while (fieldValue >= endValue);
  
  delete work;
  
  return result;
}

int Calendar::getActualMaximum(int field) const
{
  int fieldValue = getLeastMaximum(field);
  int endValue = getMaximum(field);

  // if we know that the maximum value is always the same, just return it
  if (fieldValue == endValue)
  {
    return fieldValue;
  }

  // clone the calendar so we don't mess with the real one, and set it to
  // accept anything for the field values
  Calendar* work = this->clone();
  work->setLenient(true);

  // if we're counting weeks, set the day of the week to Sunday.  We know the
  // last week of a month or year will contain the first day of the week.
  if (field == WEEK_OF_YEAR || field == WEEK_OF_MONTH)
  {
    work->set(DAY_OF_WEEK, firstDayOfWeek);
  }
  
  // now try each value from getLeastMaximum() to getMaximum() one by one until
  // we get a value that normalizes to another value.  The last value that
  // normalizes to itself is the actual maximum for the current date
  int result = fieldValue;

  do
  {
    work->set(field, fieldValue);
    if (work->get(field) != fieldValue)
    {
      break;
    }
    else
    {
      result = fieldValue;
      fieldValue++;
    }
  }
  while (fieldValue <= endValue);
  
  delete work;
  
  return result;
}


void Calendar::setWeekCountData(const Locale& desiredLocale)
{
  firstDayOfWeek = SUNDAY;
  minimalDaysInFirstWeek = 1;
}

void Calendar::updateTime()
{
  computeTime();
  
  // If we are lenient, we need to recompute the fields to normalize
  // the values.  Also, if we haven't set all the fields yet (i.e.,
  // in a newly-created object), we need to fill in the fields. 
  if (isLenient() || !areAllFieldsSet)
  {
    areFieldsSet = false;
  }
  isTimeSet = true;
}

void Calendar::adjustStamp()
{
  int max = MINIMUM_USER_STAMP;
  int newStamp = MINIMUM_USER_STAMP;

  for (;;)
  {
    int min = INT_MAX;
    for (int i = 0; i < CALENDAR_FIELD_COUNT; i++)
    {
      int v = stamp[i];
      if (v >= newStamp && min > v)
      {
        min = v;
      }
      if (max < v)
      {
        max = v;
      }
    }
    if (max != min && min == INT_MAX)
    {
      break;
    }
    for (int i = 0; i < CALENDAR_FIELD_COUNT; i++)
    {
      if (stamp[i] == min)
      {
        stamp[i] = newStamp;
      }
    }
    newStamp++;
    if (min == max)
    {
      break;
    }
  }
  nextStamp = newStamp;
}

void Calendar::invalidateWeekFields()
{
  if (stamp[WEEK_OF_MONTH] == INTERNALLY_SET)
  {
    stamp[WEEK_OF_MONTH] = UNSET;
    areFieldsSet = false;
  }
  if (stamp[WEEK_OF_YEAR] == INTERNALLY_SET)
  {
    stamp[WEEK_OF_YEAR] = UNSET;
    areFieldsSet = false;
  }
}

__OPENCODE_END_NAMESPACE
