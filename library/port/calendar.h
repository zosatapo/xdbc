/** 
 *  calendar.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef __OPENCODE_CALENDAR_H__
#define __OPENCODE_CALENDAR_H__

#include "port/libdefs.h"
#include "port/timeZone.h"
#include "port/locale.h"
#include "port/ioc_vector.h"

__OPENCODE_BEGIN_NAMESPACE

class __OPENCODE_CORE_API_DECL CalendarSystem
{
public:
  // Useful millisecond constants.   
  static const XLONG MILLIS_PER_SECOND;
  static const XLONG MILLIS_PER_MINUTE;
  static const XLONG MILLIS_PER_HOUR;
  
  static const XLONG MILLIS_PER_DAY;
  static const XLONG MILLIS_PER_WEEK;

  static const int SUNDAY;
  static const int MONDAY;
  static const int TUESDAY;
  static const int WEDNESDAY;
  static const int THURSDAY;
  static const int FRIDAY;
  static const int SATURDAY;


  static const int JANUARY;
  static const int FEBRUARY;
  static const int MARCH;
  static const int APRIL;
  static const int MAY;
  static const int JUNE;
  static const int JULY;
  static const int AUGUST;
  static const int SEPTEMBER;
  static const int OCTOBER;
  static const int NOVEMBER;
  static const int DECEMBER;
  static const int UNDECIMBER;

  static const int AM;
  static const int PM;  
  
protected:
   CalendarSystem(){}
  
public:
  virtual ~CalendarSystem() {}

};

/**
 * <code>CalendarDate</code> holds the date fields, such as year and
 * month, to represent a particular date and time.
 * <code>CalendarDate</code> is independent from any calendar
 * systems. Unlike base.Calendar,
 * <code>CalendarDate</code> has methods only to set or get a field and
 * any calendar calculation operations must be performed with a
 * {@link CalendarSystem} implementation.
 *
 */
class __OPENCODE_CORE_API_DECL CalendarDate 
{
public:
  CalendarDate();

  CalendarDate(int year, int month, int date); 

  void setYear(int year); 

  int getYear() const;

  void setMonth(int month); 

  int getMonth() const;

  void setDate(int date); 

  int getDate() const;

  void setDayOfWeek(int dayOfWeek);

  int getDayOfWeek() const;

  void setTimeOfDay(XLONG time); 

  XLONG getTimeOfDay() const;

private:
  int year;
  int month;
  int mday;
  int dow;
  
  XLONG millis;    // milliseconds within the day
};

#define CALENDAR_FIELD_COUNT     17
#define CALENDAR_UNSET           0
#define CALENDAR_INTERNALLY_SET  1

#define CALENDAR_WEEK_OF_YEAR    3
#define CALENDAR_WEEK_OF_MONTH   4
#define CALENDAR_DAY_OF_WEEK_IN_MONTH  8
#define CALENDAR_AM_PM           9
#define CALENDAR_DATE            5
#define CALENDAR_DAY_OF_YEAR     6
#define CALENDAR_DAY_OF_WEEK     7
#define CALENDAR_HOUR_OF_DAY     11
#define CALENDAR_HOUR            10
#define CALENDAR_MINUTE          12
#define CALENDAR_SECOND          13
#define CALENDAR_MILLISECOND     14
#define CALENDAR_ZONE_OFFSET     15
#define CALENDAR_DST_OFFSET      16

#define CALENDAR_ERA             0
#define CALENDAR_YEAR            1
#define CALENDAR_MONTH           2
#define CALENDAR_DAY_OF_MONTH    5

/**
 * <code>Calendar</code> is an abstract base class for converting between
 * a <code>Date</code> object and a set of integer fields such as
 * <code>YEAR</code>, <code>MONTH</code>, <code>DAY</code>, <code>HOUR</code>,
 * and so on. (A <code>Date</code> object represents a specific instant in
 * time with millisecond precision. See
 * {@link Date}
 * for information about the <code>Date</code> class.)
 *
 * <p>
 * Subclasses of <code>Calendar</code> interpret a <code>Date</code>
 * according to the rules of a specific calendar system. The platform
 * provides one concrete subclass of <code>Calendar</code>:
 * <code>GregorianCalendar</code>. Future subclasses could represent
 * the various types of lunar calendars in use in many parts of the world.
 *
 * <p>
 * Like rv locale-sensitive classes, <code>Calendar</code> provides a
 * class method, <code>getInstance</code>, for getting a generally useful
 * object of this type. <code>Calendar</code>'s <code>getInstance</code> method
 * returns a <code>Calendar</code> object whose
 * time fields have been initialized with the current date and time:
 * <blockquote>
 * <pre>
 * Calendar* rightNow = Calendar::getInstance();
 * </pre>
 * </blockquote>
 *
 * <p>A <code>Calendar</code> object can produce all the time field values
 * needed to implement the date-time formatting for a particular language and
 * calendar style (for example, Japanese-Gregorian, Japanese-Traditional).
 * <code>Calendar</code> defines the range of values returned by certain fields,
 * as well as their meaning.  For example, the first month of the year has value
 * <code>MONTH</code> == <code>JANUARY</code> for all calendars.  Other values
 * are defined by the concrete subclass, such as <code>ERA</code> and
 * <code>YEAR</code>.  See individual field documentation and subclass
 * documentation for details.
 *
 * <p>When a <code>Calendar</code> is <em>lenient</em>, it accepts a wider range
 * of field values than it produces.  For example, a lenient
 * <code>GregorianCalendar</code> interprets <code>MONTH</code> ==
 * <code>JANUARY</code>, <code>DAY_OF_MONTH</code> == 32 as February 1.  A
 * non-lenient <code>GregorianCalendar</code> throws an exception when given
 * out-of-range field settings.  When calendars recompute field values for
 * return by <code>get()</code>, they normalize them.  For example, a
 * <code>GregorianCalendar</code> always produces <code>DAY_OF_MONTH</code>
 * values between 1 and the length of the month.
 *
 * <p><code>Calendar</code> defines a locale-specific seven day week using two
 * parameters: the first day of the week and the minimal days in first week
 * (from 1 to 7).  These numbers are taken from the locale resource data when a
 * <code>Calendar</code> is constructed.  They may also be specified explicitly
 * through the API.
 *
 * <p>When setting or getting the <code>WEEK_OF_MONTH</code> or
 * <code>WEEK_OF_YEAR</code> fields, <code>Calendar</code> must determine the
 * first week of the month or year as a reference point.  The first week of a
 * month or year is defined as the earliest seven day period beginning on
 * <code>getFirstDayOfWeek()</code> and containing at least
 * <code>getMinimalDaysInFirstWeek()</code> days of that month or year.  Weeks
 * numbered ..., -1, 0 precede the first week; weeks numbered 2, 3,... follow
 * it.  Note that the normalized numbering returned by <code>get()</code> may be
 * different.  For example, a specific <code>Calendar</code> subclass may
 * designate the week before week 1 of a year as week <em>n</em> of the previous
 * year.
 *
 * <p> When computing a <code>Date</code> from time fields, two special
 * circumstances may arise: there may be insufficient information to compute the
 * <code>Date</code> (such as only year and month but no day in the month), or
 * there may be inconsistent information (such as "Tuesday, July 15, 1996" --
 * July 15, 1996 is actually a Monday).
 *
 * <p>
 * <strong>Insufficient information.</strong> The calendar will use default
 * information to specify the missing fields. This may vary by calendar; for
 * the Gregorian calendar, the default for a field is the same as that of the
 * start of the epoch: i.e., YEAR = 1970, MONTH = JANUARY, DATE = 1, etc.
 *
 * <p>
 * <strong>Inconsistent information.</strong> If fields conflict, the calendar
 * will give preference to fields set more recently. For example, when
 * determining the day, the calendar will look for one of the following
 * combinations of fields.  The most recent combination, as determined by the
 * most recently set single field, will be used.
 *
 * <blockquote>
 * <pre>
 * MONTH + DAY_OF_MONTH
 * MONTH + WEEK_OF_MONTH + DAY_OF_WEEK
 * MONTH + DAY_OF_WEEK_IN_MONTH + DAY_OF_WEEK
 * DAY_OF_YEAR
 * DAY_OF_WEEK + WEEK_OF_YEAR
 * </pre>
 * </blockquote>
 *
 * For the time of day:
 *
 * <blockquote>
 * <pre>
 * HOUR_OF_DAY
 * AM_PM + HOUR
 * </pre>
 * </blockquote>
 *
 * <p>
 * <strong>Note:</strong> for some non-Gregorian calendars, different
 * fields may be necessary for complete disambiguation. For example, a full
 * specification of the historical Arabic astronomical calendar requires year,
 * month, day-of-month <em>and</em> day-of-week in some cases.
 *
 * <p>
 * <strong>Note:</strong> There are certain possible ambiguities in
 * interpretation of certain singular times, which are resolved in the
 * following ways:
 * <ol>
 *     <li> 23:59 is the last minute of the day and 00:00 is the first minute of the
 *          next day. Thus, 23:59 on Dec 31, 1999 &lt; 00:00 on Jan 1, 2000 &lt; 00:01 on
 *          Jan 1, 2000.
 *
 *     <li> Although historically not precise, midnight also belongs to "am",
 *          and noon belongs to "pm", so on the same day,
 *          12:00 am (midnight) &lt; 12:01 am, and 12:00 pm (noon) &lt; 12:01 pm
 * </ol>
 *
 * <p><strong>Field manipulation methods</strong></p>
 * 
 * <p><code>Calendar</code> fields can be changed using three methods:
 * <code>set()</code>, <code>add()</code>, and <code>roll()</code>.</p>
 * 
 * <p><strong><code>set(f, value)</code></strong> changes field
 * <code>f</code> to <code>value</code>.  In addition, it sets an
 * internal member variable to indicate that field <code>f</code> has
 * been changed. Although field <code>f</code> is changed immediately,
 * the calendar's milliseconds is not recomputed until the next call to
 * <code>get()</code>, <code>getTime()</code>, or
 * <code>getTimeInMillis()</code> is made. Thus, multiple calls to
 * <code>set()</code> do not trigger multiple, unnecessary
 * computations. As a result of changing a field using
 * <code>set()</code>, rv fields may also change, depending on the
 * field, the field value, and the calendar system. In addition,
 * <code>get(f)</code> will not necessarily return <code>value</code>
 * after the fields have been recomputed. The specifics are determined by
 * the concrete calendar class.</p>
 * 
 * <p><em>Example</em>: Consider a <code>GregorianCalendar</code>
 * originally set to August 31, 1999. Calling <code>set(Calendar::MONTH,
 * Calendar::SEPTEMBER)</code> sets the calendar to September 31,
 * 1999. This is a temporary internal representation that resolves to
 * October 1, 1999 if <code>getTime()</code>is then called. However, a
 * call to <code>set(Calendar::DAY_OF_MONTH, 30)</code> before the call to
 * <code>getTime()</code> sets the calendar to September 30, 1999, since
 * no recomputation occurs after <code>set()</code> itself.</p>
 * 
 * <p><strong><code>add(f, delta)</code></strong> adds <code>delta</code>
 * to field <code>f</code>.  This is equivalent to calling <code>set(f,
 * get(f) + delta)</code> with two adjustments:</p>
 * 
 * <blockquote>
 *   <p><strong>Add rule 1</strong>. The value of field <code>f</code>
 *   after the call minus the value of field <code>f</code> before the
 *   call is <code>delta</code>, modulo any overflow that has occurred in
 *   field <code>f</code>. Overflow occurs when a field value exceeds its
 *   range and, as a result, the next larger field is incremented or
 *   decremented and the field value is adjusted back into its range.</p>
 * 
 *   <p><strong>Add rule 2</strong>. If a smaller field is expected to be
 *   invariant, but &nbsp; it is impossible for it to be equal to its
 *   prior value because of changes in its minimum or maximum after field
 *   <code>f</code> is changed, then its value is adjusted to be as close
 *   as possible to its expected value. A smaller field represents a
 *   smaller unit of time. <code>HOUR</code> is a smaller field than
 *   <code>DAY_OF_MONTH</code>. No adjustment is made to smaller fields
 *   that are not expected to be invariant. The calendar system
 *   determines what fields are expected to be invariant.</p>
 * </blockquote>
 * 
 * <p>In addition, unlike <code>set()</code>, <code>add()</code> forces
 * an immediate recomputation of the calendar's milliseconds and all
 * fields.</p>
 * 
 * <p><em>Example</em>: Consider a <code>GregorianCalendar</code>
 * originally set to August 31, 1999. Calling <code>add(Calendar::MONTH,
 * 13)</code> sets the calendar to September 30, 2000. <strong>Add rule
 * 1</strong> sets the <code>MONTH</code> field to September, since
 * adding 13 months to August gives September of the next year. Since
 * <code>DAY_OF_MONTH</code> cannot be 31 in September in a
 * <code>GregorianCalendar</code>, <strong>add rule 2</strong> sets the
 * <code>DAY_OF_MONTH</code> to 30, the closest possible value. Although
 * it is a smaller field, <code>DAY_OF_WEEK</code> is not adjusted by
 * rule 2, since it is expected to change when the month changes in a
 * <code>GregorianCalendar</code>.</p>
 * 
 * <p><strong><code>roll(f, delta)</code></strong> adds
 * <code>delta</code> to field <code>f</code> without changing larger
 * fields. This is equivalent to calling <code>add(f, delta)</code> with
 * the following adjustment:</p>
 * 
 * <blockquote>
 *   <p><strong>Roll rule</strong>. Larger fields are unchanged after the
 *   call. A larger field represents a larger unit of
 *   time. <code>DAY_OF_MONTH</code> is a larger field than
 *   <code>HOUR</code>.</p>
 * </blockquote>
 * 
 * <p><em>Example</em>: See {@link base.GregorianCalendar#roll(int, int)}.
 * 
 * <p><strong>Usage model</strong>. To motivate the behavior of
 * <code>add()</code> and <code>roll()</code>, consider a user interface
 * component with increment and decrement buttons for the month, day, and
 * year, and an underlying <code>GregorianCalendar</code>. If the
 * interface reads January 31, 1999 and the user presses the month
 * increment button, what should it read? If the underlying
 * implementation uses <code>set()</code>, it might read March 3, 1999. A
 * better result would be February 28, 1999. Furthermore, if the user
 * presses the month increment button again, it should read March 31,
 * 1999, not March 28, 1999. By saving the original date and using either
 * <code>add()</code> or <code>roll()</code>, depending on whether larger
 * fields should be affected, the user interface can behave as most users
 * will intuitively expect.</p>
 *
 *
 * Data flow in Calendar
 * ---------------------
 *
 * The current time is represented in two ways by Calendar: as UTC
 * milliseconds from the epoch start (1 January 1970 0:00 UTC), and as local
 * fields such as MONTH, HOUR, AM_PM, etc.  It is possible to compute the
 * millis from the fields, and vice versa.  The data needed to do this
 * conversion is encapsulated by a TimeZone object owned by the Calendar::
 * The data provided by the TimeZone object may also be overridden if the
 * user sets the ZONE_OFFSET and/or DST_OFFSET fields directly. The class
 * keeps track of what information was most recently set by the caller, and
 * uses that to compute any rv information as needed.
 *
 * If the user sets the fields using set(), the data flow is as follows.
 * This is implemented by the Calendar subclass's computeTime() method.
 * During this process, certain fields may be ignored.  The disambiguation
 * algorithm for resolving which fields to pay attention to is described
 * above.
 *
 *   local fields (YEAR, MONTH, DATE, HOUR, MINUTE, etc.)
 *           |
 *           | Using Calendar-specific algorithm
 *           V
 *   local standard millis
 *           |
 *           | Using TimeZone or user-set ZONE_OFFSET / DST_OFFSET
 *           V
 *   UTC millis (in time data member)

 * If the user sets the UTC millis using setTime(), the data flow is as
 * follows.  This is implemented by the Calendar subclass's computeFields()
 * method.
 *
 *   UTC millis (in time data member)
 *           |
 *           | Using TimeZone getOffset()
 *           V
 *   local standard millis
 *           |
 *           | Using Calendar-specific algorithm
 *           V
 *   local fields (YEAR, MONTH, DATE, HOUR, MINUTE, etc.)
 *
 * In general, a round trip from fields, through local and UTC millis, and
 * back out to fields is made when necessary.  This is implemented by the
 * complete() method.  Resolving a partial set of fields into a UTC millis
 * value allows all remaining fields to be generated from that value.  If
 * the Calendar is lenient, the fields are also renormalized to standard
 * ranges when they are regenerated.
 */

class __OPENCODE_CORE_API_DECL Calendar
{
public:
  Calendar* clone() const;

  /**
   * Return a string representation of this calendar. This method 
   * is intended to be used only for debugging purposes, and the 
   * format of the returned string may vary between implementations. 
   * The returned string may be empty but may not be <code>null</code>.
   * 
   * @return  a string representation of this calendar.
   */
  virtual string toString() const;

private:
  virtual Calendar* doClone() const = 0;

public:
  virtual ~Calendar();

protected:
  Calendar(const Calendar& rhs);
  Calendar& operator=(const Calendar& rhs);

protected :
  /**
   * Constructs a Calendar with the default time zone
   * and locale.
   */
  Calendar();

  /**
   * Constructs a calendar with the specified time zone and locale.
   * @param aTimeZone the time zone to use
   * @param aLocale the locale for the week data
   */
  Calendar(const TimeZone* aTimeZone, const Locale& aLocale);
  
public :
  /**
   * Gets a calendar using the default time zone and locale. The
   * <code>Calendar</code> returned is based on the current time
   * in the default time zone with the default locale.
   *
   * @return a Calendar::
   */
  static Calendar* getInstance();

  /**
   * Gets a calendar using the specified time zone and default locale.
   * The <code>Calendar</code> returned is based on the current time
   * in the given time zone with the default locale. 
   *
   * @param aTimeZone the time zone to use
   * @return a Calendar::
   */
  static Calendar* getInstance(const TimeZone* aTimeZone);

  /**
   * Gets a calendar using the default time zone and specified locale.
   * The <code>Calendar</code> returned is based on the current time
   * in the default time zone with the given locale.
   *
   * @param aLocale the locale for the week data
   * @return a Calendar::
   */
  static Calendar* getInstance(const Locale& aLocale);

  /**
   * Gets a calendar with the specified time zone and locale.
   * The <code>Calendar</code> returned is based on the current time
   * in the given time zone with the given locale.
   *
   * @param aTimeZone the time zone to use
   * @param aLocale the locale for the week data
   * @return a Calendar::
   */
  static Calendar* getInstance(const TimeZone* aTimeZone,const Locale& aLocale);

protected :
  /**
   * Converts the current field values in <code>fields[]</code>
   * to the millisecond time value
   * <code>time</code>.
   */
  virtual void computeTime()  = 0;

  /**
   * Converts 
   * the current millisecond time value
   * <code>time</code>
   * to field values in <code>fields[]</code>.
   * This allows you to sync up the time field values with
   * a new time that is set for the calendar.  The time is <em>not</em>
   * recomputed first; to recompute the time, then the fields, call the
   * <code>complete</code> method.
   */
  virtual void computeFields() = 0;

public :
  /**
   * Gets this Calendar's current time as a long.
   * @return the current time as UTC milliseconds from the epoch.
   */
  XLONG getTimeInMillis() const;

  /**
   * Sets this Calendar's current time from the given long value.
   * @param millis the new time in UTC milliseconds from the epoch.
   */
  void setTimeInMillis(XLONG millis);

  /**
   * Gets the value for a given time field.
   * @param field the given time field.
   * @return the value for the given time field.
   */
  int get(int field) const;

public :
  /**
   * Sets the time field with the given value.
   * @param field the given time field.
   * @param value the value to be set for the given time field.
   */
  virtual void set(int field, int value);

  /**
   * Sets the values for the fields year, month, and date.
   * Previous values of rv fields are retained.  If this is not desired,
   * call <code>clear</code> first.
   * @param year the value used to set the YEAR time field.
   * @param month the value used to set the MONTH time field.
   * Month value is 0-based. e.g., 0 for January.
   * @param date the value used to set the DATE time field.
   */
  virtual void set(int year, int month, int date);
  
  /**
   * Sets the values for the fields year, month, date, hour, and minute.
   * Previous values of rv fields are retained.  If this is not desired,
   * call <code>clear</code> first.
   * @param year the value used to set the YEAR time field.
   * @param month the value used to set the MONTH time field.
   * Month value is 0-based. e.g., 0 for January.
   * @param date the value used to set the DATE time field.
   * @param hour the value used to set the HOUR_OF_DAY time field.
   * @param minute the value used to set the MINUTE time field.
   */
  virtual void set(int year, int month, int date, int hour, int minute);

  /**
   * Sets the values for the fields year, month, date, hour, minute, and second.
   * Previous values of rv fields are retained.  If this is not desired,
   * call <code>clear</code> first.
   * @param year the value used to set the YEAR time field.
   * @param month the value used to set the MONTH time field.
   * Month value is 0-based. e.g., 0 for January.
   * @param date the value used to set the DATE time field.
   * @param hour the value used to set the HOUR_OF_DAY time field.
   * @param minute the value used to set the MINUTE time field.
   * @param second the value used to set the SECOND time field.
   */
  virtual void set(int year, int month, int date, int hour, int minute, int second);

  /**
   * Clears the values of all the time fields.
   */
  void clear();

  /**
   * Clears the value in the given time field.
   * @param field the time field to be cleared.
   */
  void clear(int field);

  /**
   * Determines if the given time field has a value set.
   * @return true if the given time field has a value set; false otherwise.
   */
  bool isSet(int field) const;
  
public :
  /**
   * Date Arithmetic function.
   * Adds the specified (signed) amount of time to the given time field,
   * based on the calendar's rules. For example, to subtract 5 days from
   * the current time of the calendar, you can achieve it by calling:
   * <p>add(Calendar::DATE, -5).
   * @param field the time field.
   * @param amount the amount of date or time to be added to the field.
   */
  virtual void add(int field, int amount) = 0;

  /**
   * Time Field Rolling function.
   * Add to field a signed amount without changing larger fields.
   * A negative roll amount means to roll down.
   * [NOTE:  This default implementation on Calendar just repeatedly calls the
   * version of roll() that takes a boolean and rolls by one unit.  This may not
   * always do the right thing.  For example, if the DAY_OF_MONTH field is 31,
   * rolling through February will leave it set to 28.  The GregorianCalendar
   * version of this function takes care of this problem.  Other subclasses
   * should also provide overrides of this function that do the right thing.
   * @param field the time field.
   * @param amount the signed amount to add to <code>field</code>.
   */
  virtual void roll(int field, int amount);

  /**
   * Sets the time zone with the given time zone value.
   * @param aTimeZone the given time zone.
   */
  void setTimeZone(const TimeZone* aTimeZone);

  /**
   * Gets the time zone.
   * @return the time zone object associated with this calendar.
   */
  const TimeZone* getTimeZone() const;

  /**
   * Specify whether or not date/time interpretation is to be lenient.  With
   * lenient interpretation, a date such as "February 942, 1996" will be
   * treated as being equivalent to the 941st day after February 1, 1996.
   * With strict interpretation, such dates will cause an exception to be
   * thrown.
   *
   */
  void setLenient(bool lenient);

  /**
   * Tell whether date/time interpretation is to be lenient.
   */
  bool isLenient() const;

  /**
   * Sets what the first day of the week is; e.g., Sunday in US,
   * Monday in France.
   * @param value the given first day of the week.
   */
  void setFirstDayOfWeek(int value);

  /**
   * Gets what the first day of the week is; e.g., Sunday in US,
   * Monday in France.
   * @return the first day of the week.
   */
  int getFirstDayOfWeek() const;

  /**
   * Sets what the minimal days required in the first week of the year are;
   * For example, if the first week is defined as one that contains the first
   * day of the first month of a year, call the method with value 1. If it
   * must be a full week, use value 7.
   * @param value the given minimal days required in the first week
   * of the year.
   */
  void setMinimalDaysInFirstWeek(int value);

  /**
   * Gets what the minimal days required in the first week of the year are;
   * e.g., if the first week is defined as one that contains the first day
   * of the first month of a year, getMinimalDaysInFirstWeek returns 1. If
   * the minimal days required must be a full week, getMinimalDaysInFirstWeek
   * returns 7.
   * @return the minimal days required in the first week of the year.
   */
  int getMinimalDaysInFirstWeek() const;

public :
  /**
   * Gets the minimum value for the given time field.
   * e.g., for Gregorian DAY_OF_MONTH, 1.
   * @param field the given time field.
   * @return the minimum value for the given time field.
   */
  virtual int getMinimum(int field) const = 0;

  /**
   * Gets the maximum value for the given time field.
   * e.g. for Gregorian DAY_OF_MONTH, 31.
   * @param field the given time field.
   * @return the maximum value for the given time field.
   */
  virtual int getMaximum(int field) const = 0;

  /**
   * Gets the highest minimum value for the given field if varies.
   * Otherwise same as getMinimum(). For Gregorian, no difference.
   * @param field the given time field.
   * @return the highest minimum value for the given time field.
   */
  virtual int getGreatestMinimum(int field) const = 0;

  /**
   * Gets the lowest maximum value for the given field if varies.
   * Otherwise same as getMaximum(). e.g., for Gregorian DAY_OF_MONTH, 28.
   * @param field the given time field.
   * @return the lowest maximum value for the given time field.
   */
  virtual int getLeastMaximum(int field) const = 0;

  /**
   * Return the minimum value that this field could have, given the current date.
   * For the Gregorian calendar, this is the same as getMinimum() and getGreatestMinimum().
   *
   * The version of this function on Calendar uses an iterative algorithm to determine the
   * actual minimum value for the field.  There is almost always a more efficient way to
   * accomplish this (in most cases, you can simply return getMinimum()).  GregorianCalendar
   * overrides this function with a more efficient implementation.
   *
   * @param field the field to determine the minimum of
   * @return the minimum of the given field for the current date of this Calendar
   */
  int getActualMinimum(int field) const ;

  /**
   * Return the maximum value that this field could have, given the current date.
   * For example, with the date "Feb 3, 1997" and the DAY_OF_MONTH field, the actual
   * maximum would be 28; for "Feb 3, 1996" it s 29.  Similarly for a Hebrew calendar,
   * for some years the actual maximum for MONTH is 12, and for others 13.
   *
   * The version of this function on Calendar uses an iterative algorithm to determine the
   * actual maximum value for the field.  There is almost always a more efficient way to
   * accomplish this (in most cases, you can simply return getMaximum()).  GregorianCalendar
   * overrides this function with a more efficient implementation.
   *
   * @param field the field to determine the maximum of
   * @return the maximum of the given field for the current date of this Calendar
   */
  int getActualMaximum(int field) const ;

protected :
  /**
   * Fills in any unset fields in the time field list.
   */
  void complete() ;
  
  /**
   * Gets the value for a given time field. This is an internal
   * fast time field value getter for the subclasses.
   * @param field the given time field.
   * @return the value for the given time field.
   */
  int internalGet(int field) const;

  /**
   * Sets the value for the given time field.  This is an internal
   * fast setter for subclasses.  It does not affect the areFieldsSet, isTimeSet,
   * or areAllFieldsSet flags.
   */
  void internalSet(int field, int value);

  /**
   * Clears the value of the given calendar field and resets the
   * field status flags only. The difference from clear(int) is that
   * this method doesn't reset isTimeSet.
   *
   * @param field the given calendar field.
   */
  void internalClear(int field);
  
private :

  static Calendar* createCalendar(const TimeZone* aTimeZone,const Locale& aLocale);
  
  /**
   * Both firstDayOfWeek and minimalDaysInFirstWeek are locale-dependent.
   * They are used to figure out the week count for a specific date for
   * a given locale. These must be set when a Calendar is constructed.
   * @param desiredLocale the given locale.
   */
  void setWeekCountData(const Locale& desiredLocale);

  /**
   * Recompute the time and update the status fields isTimeSet
   * and areFieldsSet.  Callers should check isTimeSet and only
   * call this method if isTimeSet is false.
   */
  void updateTime();

  /**
   * Adjusts the stamp[] values before nextStamp overflow. nextStamp
   * is set to the next stamp value upon the return.
   */
  void adjustStamp();

  /**
   * Invalidates the WEEK_OF_MONTH and WEEK_OF_YEAR fields if they
   * have been calculated internally.
   */
  void invalidateWeekFields();

public :
  /**
   * Field number for <code>get</code> and <code>set</code> indicating the
   * era, e.g., AD or BC in the Julian calendar. This is a calendar-specific
   * value; see subclass documentation.
   */
  const static int ERA;

  /**
   * Field number for <code>get</code> and <code>set</code> indicating the
   * year. This is a calendar-specific value; see subclass documentation.
   */
  const static int YEAR;

  /**
   * Field number for <code>get</code> and <code>set</code> indicating the
   * month. This is a calendar-specific value. The first month of the year is
   * <code>JANUARY</code> which is 0; the last depends on the number of months in a year.
   */
  const static int MONTH;
  /**
   * Field number for <code>get</code> and <code>set</code> indicating the
   * week number within the current year.  The first week of the year, as
   * defined by <code>getFirstDayOfWeek()</code> and
   * <code>getMinimalDaysInFirstWeek()</code>, has value 1.  Subclasses define
   * the value of <code>WEEK_OF_YEAR</code> for days before the first week of
   * the year.
   */
  const static int WEEK_OF_YEAR;
  /**
   * Field number for <code>get</code> and <code>set</code> indicating the
   * week number within the current month.  The first week of the month, as
   * defined by <code>getFirstDayOfWeek()</code> and
   * <code>getMinimalDaysInFirstWeek()</code>, has value 1.  Subclasses define
   * the value of <code>WEEK_OF_MONTH</code> for days before the first week of
   * the month.
   */
  const static int WEEK_OF_MONTH;
  /**
   * Field number for <code>get</code> and <code>set</code> indicating the
   * day of the month. This is a synonym for <code>DAY_OF_MONTH</code>.
   * The first day of the month has value 1.
   */
  const static int DATE;
  /**
   * Field number for <code>get</code> and <code>set</code> indicating the
   * day of the month. This is a synonym for <code>DATE</code>.
   * The first day of the month has value 1.
   */
  const static int DAY_OF_MONTH;
  /**
   * Field number for <code>get</code> and <code>set</code> indicating the day
   * number within the current year.  The first day of the year has value 1.
   */
  const static int DAY_OF_YEAR;
  /**
   * Field number for <code>get</code> and <code>set</code> indicating the day
   * of the week.  This field takes values <code>SUNDAY</code>,
   * <code>MONDAY</code>, <code>TUESDAY</code>, <code>WEDNESDAY</code>,
   * <code>THURSDAY</code>, <code>FRIDAY</code>, and <code>SATURDAY</code>.
   */
  const static int DAY_OF_WEEK;
  /**
   * Field number for <code>get</code> and <code>set</code> indicating the
   * ordinal number of the day of the week within the current month. Together
   * with the <code>DAY_OF_WEEK</code> field, this uniquely specifies a day
   * within a month.  Unlike <code>WEEK_OF_MONTH</code> and
   * <code>WEEK_OF_YEAR</code>, this field's value does <em>not</em> depend on
   * <code>getFirstDayOfWeek()</code> or
   * <code>getMinimalDaysInFirstWeek()</code>.  <code>DAY_OF_MONTH 1</code>
   * through <code>7</code> always correspond to <code>DAY_OF_WEEK_IN_MONTH
   * 1</code>; <code>8</code> through <code>14</code> correspond to
   * <code>DAY_OF_WEEK_IN_MONTH 2</code>, and so on.
   * <code>DAY_OF_WEEK_IN_MONTH 0</code> indicates the week before
   * <code>DAY_OF_WEEK_IN_MONTH 1</code>.  Negative values count back from the
   * end of the month, so the last Sunday of a month is specified as
   * <code>DAY_OF_WEEK = SUNDAY, DAY_OF_WEEK_IN_MONTH = -1</code>.  Because
   * negative values count backward they will usually be aligned differently
   * within the month than positive values.  For example, if a month has 31
   * days, <code>DAY_OF_WEEK_IN_MONTH -1</code> will overlap
   * <code>DAY_OF_WEEK_IN_MONTH 5</code> and the end of <code>4</code>.
   */
  const static int DAY_OF_WEEK_IN_MONTH;
  /**
   * Field number for <code>get</code> and <code>set</code> indicating
   * whether the <code>HOUR</code> is before or after noon.
   * E.g., at 10:04:15.250 PM the <code>AM_PM</code> is <code>PM</code>.
   */
  const static int AM_PM;
  /**
   * Field number for <code>get</code> and <code>set</code> indicating the
   * hour of the morning or afternoon. <code>HOUR</code> is used for the 12-hour
   * clock.
   * E.g., at 10:04:15.250 PM the <code>HOUR</code> is 10.
   */
  const static int HOUR;
  /**
   * Field number for <code>get</code> and <code>set</code> indicating the
   * hour of the day. <code>HOUR_OF_DAY</code> is used for the 24-hour clock.
   * E.g., at 10:04:15.250 PM the <code>HOUR_OF_DAY</code> is 22.
   */
  const static int HOUR_OF_DAY;
  /**
   * Field number for <code>get</code> and <code>set</code> indicating the
   * minute within the hour.
   * E.g., at 10:04:15.250 PM the <code>MINUTE</code> is 4.
   */
  const static int MINUTE;
  /**
   * Field number for <code>get</code> and <code>set</code> indicating the
   * second within the minute.
   * E.g., at 10:04:15.250 PM the <code>SECOND</code> is 15.
   */
  const static int SECOND;
  /**
   * Field number for <code>get</code> and <code>set</code> indicating the
   * millisecond within the second.
   * E.g., at 10:04:15.250 PM the <code>MILLISECOND</code> is 250.
   */
  const static int MILLISECOND;
  /**
   * Field number for <code>get</code> and <code>set</code>
   * indicating the raw offset from GMT in milliseconds.
   * <p>
   * This field reflects the correct GMT offset value of the time
   * zone of this <code>Calendar</code> if the
   * <code>TimeZone</code> implementation subclass supports
   * historical GMT offset changes.
   */
  const static int ZONE_OFFSET;
  /**
   * Field number for <code>get</code> and <code>set</code> indicating the
   * daylight savings offset in milliseconds.
   * <p>
   * This field reflects the correct daylight saving offset value of
   * the time zone of this <code>Calendar</code> if the
   * <code>TimeZone</code> implementation subclass supports
   * historical Daylight Saving Time schedule changes.
   */
  const static int DST_OFFSET;
  /**
   * The number of distinct fields recognized by <code>get</code> and <code>set</code>.
   * Field numbers range from <code>0..CALENDAR_FIELD_COUNT-1</code>.
   */
//  const static int CALENDAR_FIELD_COUNT;

  /**
   * Value of the <code>DAY_OF_WEEK</code> field indicating
   * Sunday.
   */
  const static int SUNDAY;
  /**
   * Value of the <code>DAY_OF_WEEK</code> field indicating
   * Monday.
   */
  const static int MONDAY;
  /**
   * Value of the <code>DAY_OF_WEEK</code> field indicating
   * Tuesday.
   */
  const static int TUESDAY;
  /**
   * Value of the <code>DAY_OF_WEEK</code> field indicating
   * Wednesday.
   */
  const static int WEDNESDAY;
  /**
   * Value of the <code>DAY_OF_WEEK</code> field indicating
   * Thursday.
   */
  const static int THURSDAY;
  /**
   * Value of the <code>DAY_OF_WEEK</code> field indicating
   * Friday.
   */
  const static int FRIDAY;
  /**
   * Value of the <code>DAY_OF_WEEK</code> field indicating
   * Saturday.
   */
  const static int SATURDAY;

  /**
   * Value of the <code>MONTH</code> field indicating the
   * first month of the year.
   */
  const static int JANUARY;
  /**
   * Value of the <code>MONTH</code> field indicating the
   * second month of the year.
   */
  const static int FEBRUARY;
  /**
   * Value of the <code>MONTH</code> field indicating the
   * third month of the year.
   */
  const static int MARCH;
  /**
   * Value of the <code>MONTH</code> field indicating the
   * fourth month of the year.
   */
  const static int APRIL;
  /**
   * Value of the <code>MONTH</code> field indicating the
   * fifth month of the year.
   */
  const static int MAY;
  /**
   * Value of the <code>MONTH</code> field indicating the
   * sixth month of the year.
   */
  const static int JUNE;
  /**
   * Value of the <code>MONTH</code> field indicating the
   * seventh month of the year.
   */
  const static int JULY;
  /**
   * Value of the <code>MONTH</code> field indicating the
   * eighth month of the year.
   */
  const static int AUGUST;
  /**
   * Value of the <code>MONTH</code> field indicating the
   * ninth month of the year.
   */
  const static int SEPTEMBER;
  /**
   * Value of the <code>MONTH</code> field indicating the
   * tenth month of the year.
   */
  const static int OCTOBER;
  /**
   * Value of the <code>MONTH</code> field indicating the
   * eleventh month of the year.
   */
  const static int NOVEMBER;
  /**
   * Value of the <code>MONTH</code> field indicating the
   * twelfth month of the year.
   */
  const static int DECEMBER;
  /**
   * Value of the <code>MONTH</code> field indicating the
   * thirteenth month of the year. Although <code>GregorianCalendar</code>
   * does not use this value, lunar calendars do.
   */
  const static int UNDECIMBER;

  /**
   * Value of the <code>AM_PM</code> field indicating the
   * period of the day from midnight to just before noon.
   */
  const static int AM;
  /**
   * Value of the <code>AM_PM</code> field indicating the
   * period of the day from noon to just before midnight.
   */
  const static int PM;
  
protected :

  // Internal notes:
  // Calendar contains two kinds of time representations: current "time" in
  // milliseconds, and a set of time "fields" representing the current time.
  // The two representations are usually in sync, but can get out of sync
  // as follows.
  // 1. Initially, no fields are set, and the time is invalid.
  // 2. If the time is set, all fields are computed and in sync.
  // 3. If a single field is set, the time is invalid.
  // Recomputation of the time and fields happens when the object needs
  // to return a result to the user, or use a result for a computation.

  /**
   * The field values for the currently set time for this calendar.
   * This is an array of <code>CALENDAR_FIELD_COUNT</code> integers, with index values
   * <code>ERA</code> through <code>DST_OFFSET</code>.
   */
   int fields[CALENDAR_FIELD_COUNT]; // NOTE: Make transient when possible

  /**
   * Pseudo-time-stamps which specify when each field was set. There
   * are two special values, UNSET and INTERNALLY_SET. Values from
   * MINIMUM_USER_SET to Integer.MAX_VALUE are legal user set values.
   */
   int stamp[CALENDAR_FIELD_COUNT];

  /**
   * The currently set time for this calendar, expressed in milliseconds after
   * January 1, 1970, 0:00:00 GMT.
   * 
   */
   XLONG time;

  /**
   * True if then the value of <code>time</code> is valid.
   * The time is made invalid by a change to an item of <code>field[]</code>.
   * 
   */
   bool isTimeSet; // NOTE: Make transient when possible

  /**
   * True if <code>fields[]</code> are in sync with the currently set time.
   * If false, then the next attempt to get the value of a field will
   * force a recomputation of all fields from the current value of
   * <code>time</code>.
   * 
   */
   bool areFieldsSet; // NOTE: Make transient when possible

  /**
   * True if all fields have been set.
   * 
   */
   bool areAllFieldsSet;

  /**
   * True if this calendar allows out-of-range field values during computation
   * of <code>time</code> from <code>fields[]</code>.
   * 
   */
  bool lenient;

  /**
   * The <code>TimeZone</code> used by this calendar. </code>Calendar</code>
   * uses the time zone data to translate between locale and GMT time.
   * 
   */
  TimeZone* zone;

  /**
   * The first day of the week, with possible values <code>SUNDAY</code>,
   * <code>MONDAY</code>, etc.  This is a locale-dependent value.
   * 
   */
  int firstDayOfWeek;

  /**
   * The number of days required for the first week in a month or year,
   * with possible values from 1 to 7.  This is a locale-dependent value.
   * 
   */
  int minimalDaysInFirstWeek;

  // Special values of stamp[]

  static const int UNSET ;//= 0;
  static const int INTERNALLY_SET ;//= 1;
  static const int MINIMUM_USER_STAMP ;//= 2;

  /**
   * The next available value for <code>stamp[]</code>, an internal array.
   * This actually should not be written out to the stream, and will probably
   * be removed from the stream in the near future.  In the meantime,
   * a value of <code>MINIMUM_USER_STAMP</code> should be used.
   * 
   */
  int nextStamp;

protected:
  const static  char FIELD_NAME[CALENDAR_FIELD_COUNT][30];

};

__OPENCODE_END_NAMESPACE

#endif
