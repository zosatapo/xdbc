/** 
 *  gregorianCalendar.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef __OPENCODE_GREGORIANCALENDAR_H__
#define __OPENCODE_GREGORIANCALENDAR_H__

#include "port/libdefs.h"
#include "port/calendar.h"

__OPENCODE_BEGIN_NAMESPACE

/**
 * Gregorian calendar calculations. The algorithms are from "Calendrical
 * Calculation" by Nachum Dershowitz and Edward M. Reingold (ISBN:0-521-56474-3).
 */

class __OPENCODE_CORE_API_DECL GregorianSystem : public CalendarSystem
{ 
public:
  virtual ~GregorianSystem() {}
  
  GregorianSystem(){}

public:
  /**
  * Calculates Gregorian calendar fields from the given UTC value
  * which is the difference from Gregorian January 1, 1970 00:00:00
  * GMT.
  * @param utc the UTC value in milliseconds
  * @return the CaledarDate object that contains the calculated Gregorian calendar field values.
  */
  static CalendarDate getCalendarDate(XLONG utc) ;
  
  /**
   * Calculates milliseconds of given time from EPOCH, 1970-01-01 00:00AM.
   */
  static XLONG dateToMillis(const CalendarDate& date);

  /**
   * Calculates milliseconds of given time from EPOCH, 1970-01-01 00:00AM.
   */
  static XLONG dateToMillis(int year, int month, int day,XLONG milliseconds);

  static bool validate(const CalendarDate& date);

  /**
   * @param month 0-based
   */
  static int getMonthLength(int year, int month);

  /**
   * Returns number of days from 0001-01-01. Counting leap correction.
   */
  static  XLONG getFixedDate(int year, int month, int day) ;

  /**
  * Calculates year/month/day from given date. The date is from 0001-01-01.
  */
  static CalendarDate getCalendarDateFromFixedDate(XLONG fixedDate);

  /**
   * Returns day of week of given day
   */
  static int getDayOfWeek(const CalendarDate& date);

  static  int getDayOfWeekFromFixedDate(XLONG fixedDate);

  /**
  * Returns Gregorian year number of given date
  */
  static  int getYear(XLONG fixedDate);

  /**
   * @return true if the given year is a Gregorian leap year.
   */
  static  bool isLeapYear(int year);

private:
  /**
   * Floor function working with negative number.
   * floor(3.14) = 3 and floor(-3.14) = -4.
   */
  static  XLONG floorDivide(XLONG n, XLONG d);
  static  int floorDivide(int n, int d) ;

  static  XLONG mod(XLONG x, XLONG y);
  static  int mod(int x, int y);

public:
  // 0-based, for day-in-year
  static const int NUM_DAYS[12]; //= { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };
  static const int LEAP_NUM_DAYS[12]; //= { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335 };

  // 0-based, for day-in-month
  static const int MONTH_LENGTH[12]; //= { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }; // 0-based
  static const int LEAP_MONTH_LENGTH[12]; //= { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }; // 0-based

  static const int JAN_1_1_JULIAN_DAY; // January 1, year 1 (Gregorian)
  static const int EPOCH_JULIAN_DAY; // January 1, 1970 (Gregorian)
  static const int EPOCH_YEAR;

   /**
   * The number of days between Gregorian January 1, 1 and January
   * 1, 1970.
   */
  static const int EPOCH_DATE;

  /**
   * Value of the <code>ERA</code> field indicating
   * the period before the common era (before Christ), also known as BCE.
   * The sequence of years at the transition from <code>BC</code> to <code>AD</code> is
   * ..., 2 BC, 1 BC, 1 AD, 2 AD,...
   */
  static const int BC;

  /**
   * Value of the <code>ERA</code> field indicating
   * the common era (Anno Domini), also known as CE.
   * The sequence of years at the transition from <code>BC</code> to <code>AD</code> is
   * ..., 2 BC, 1 BC, 1 AD, 2 AD,...
   */
  static const int AD;

};


/**
 * <code>GregorianCalendar</code> is a concrete subclass of
 * {@link Calendar}
 * and provides the standard calendar used by most of the world.
 *
 * <p>
 * The standard (Gregorian) calendar has 2 eras, BC and AD.
 *
 * <p>
 * This implementation handles a single discontinuity, which corresponds by
 * default to the date the Gregorian calendar was instituted (October 15, 1582
 * in some countries, later in others).  The cutover date may be changed by the
 * caller by calling <code>setGregorianChange()</code>.
 *
 * <p>
 * Historically, in those countries which adopted the Gregorian calendar first,
 * October 4, 1582 was thus followed by October 15, 1582. This calendar models
 * this correctly.  Before the Gregorian cutover, <code>GregorianCalendar</code>
 * implements the Julian calendar.  The only difference between the Gregorian
 * and the Julian calendar is the leap year rule. The Julian calendar specifies
 * leap years every four years, whereas the Gregorian calendar omits century
 * years which are not divisible by 400.
 *
 * <p>
 * <code>GregorianCalendar</code> implements <em>proleptic</em> Gregorian and
 * Julian calendars. That is, dates are computed by extrapolating the current
 * rules indefinitely far backward and forward in time. As a result,
 * <code>GregorianCalendar</code> may be used for all years to generate
 * meaningful and consistent results. However, dates obtained using
 * <code>GregorianCalendar</code> are historically accurate only from March 1, 4
 * AD onward, when modern Julian calendar rules were adopted.  Before this date,
 * leap year rules were applied irregularly, and before 45 BC the Julian
 * calendar did not even exist.
 *
 * <p>
 * Prior to the institution of the Gregorian calendar, New Year's Day was
 * March 25. To avoid confusion, this calendar always uses January 1. A manual
 * adjustment may be made if desired for dates that are prior to the Gregorian
 * changeover and which fall between January 1 and March 24.
 *
 * <p>Values calculated for the <code>WEEK_OF_YEAR</code> field range from 1 to
 * 53.  Week 1 for a year is the earliest seven day period starting on
 * <code>getFirstDayOfWeek()</code> that contains at least
 * <code>getMinimalDaysInFirstWeek()</code> days from that year.  It thus
 * depends on the values of <code>getMinimalDaysInFirstWeek()</code>,
 * <code>getFirstDayOfWeek()</code>, and the day of the week of January 1.
 * Weeks between week 1 of one year and week 1 of the following year are
 * numbered sequentially from 2 to 52 or 53 (as needed).

 * <p>For example, January 1, 1998 was a Thursday.  If
 * <code>getFirstDayOfWeek()</code> is <code>MONDAY</code> and
 * <code>getMinimalDaysInFirstWeek()</code> is 4 (these are the values
 * reflecting ISO 8601 and many national standards), then week 1 of 1998 starts
 * on December 29, 1997, and ends on January 4, 1998.  If, however,
 * <code>getFirstDayOfWeek()</code> is <code>SUNDAY</code>, then week 1 of 1998
 * starts on January 4, 1998, and ends on January 10, 1998; the first three days
 * of 1998 then are part of week 53 of 1997.
 *
 * <p>Values calculated for the <code>WEEK_OF_MONTH</code> field range from 0
 * to 6.  Week 1 of a month (the days with <code>WEEK_OF_MONTH =
 * 1</code>) is the earliest set of at least
 * <code>getMinimalDaysInFirstWeek()</code> contiguous days in that month,
 * ending on the day before <code>getFirstDayOfWeek()</code>.  Unlike
 * week 1 of a year, week 1 of a month may be shorter than 7 days, need
 * not start on <code>getFirstDayOfWeek()</code>, and will not include days of
 * the previous month.  Days of a month before week 1 have a
 * <code>WEEK_OF_MONTH</code> of 0.
 *
 * <p>For example, if <code>getFirstDayOfWeek()</code> is <code>SUNDAY</code>
 * and <code>getMinimalDaysInFirstWeek()</code> is 4, then the first week of
 * January 1998 is Sunday, January 4 through Saturday, January 10.  These days
 * have a <code>WEEK_OF_MONTH</code> of 1.  Thursday, January 1 through
 * Saturday, January 3 have a <code>WEEK_OF_MONTH</code> of 0.  If
 * <code>getMinimalDaysInFirstWeek()</code> is changed to 3, then January 1
 * through January 3 have a <code>WEEK_OF_MONTH</code> of 1.
 *
 *
 *
 * <<Implementation Notes>>
 *
 * The Julian day number, as used here, is a modified number which has its
 * onset at midnight, rather than noon.
 *
 * The epoch is the number of days or milliseconds from some defined
 * starting point. The epoch for base.Date is used here; that is,
 * milliseconds from January 1, 1970 (Gregorian), midnight UTC.  Other
 * epochs which are used are January 1, year 1 (Gregorian), which is day 1
 * of the Gregorian calendar, and December 30, year 0 (Gregorian), which is
 * day 1 of the Julian calendar.
 *
 * We implement the proleptic Julian and Gregorian calendars.  This means we
 * implement the modern definition of the calendar even though the
 * historical usage differs.  For example, if the Gregorian change is set
 * to new Date(XLONG_MIN), we have a pure Gregorian calendar which
 * labels dates preceding the invention of the Gregorian calendar in 1582 as
 * if the calendar existed then.
 *
 * Likewise, with the Julian calendar, we assume a consistent 4-year leap
 * rule, even though the historical pattern of leap years is irregular,
 * being every 3 years from 45 BC through 9 BC, then every 4 years from 8 AD
 * onwards, with no leap years in-between.  Thus date computations and
 * functions such as isLeapYear() are not intended to be historically
 * accurate.
 *
 * Given that milliseconds are a XLONG, day numbers such as Julian day
 * numbers, Gregorian or Julian calendar days, or epoch days, are also
 * LONG_LONGs. Years can fit into an int.
 *
 */ 
class __OPENCODE_CORE_API_DECL GregorianCalendar : public Calendar
{
private:
  virtual Calendar* doClone() const;

public:
  ~GregorianCalendar() ;

  GregorianCalendar(const GregorianCalendar& rhs);
  GregorianCalendar& operator=(const GregorianCalendar& rhs); 
 
public:
  /**
   * Constructs a default GregorianCalendar using the current time
   * in the default time zone with the default locale.
   */
  GregorianCalendar();

  /**
   * Constructs a GregorianCalendar based on the current time
   * in the given time zone with the default locale.
   * @param aTimeZone the given time zone.
   */
  GregorianCalendar(const TimeZone* aTimeZone);

  /**
   * Constructs a GregorianCalendar based on the current time
   * in the default time zone with the given locale.
   * @param aLocale the given locale.
   */
  GregorianCalendar(const Locale& aLocale);

  /**
   * Constructs a GregorianCalendar based on the current time
   * in the given time zone with the given locale.
   * @param aTimeZone the given time zone.
   * @param aLocale the given locale.
   */
  GregorianCalendar(const TimeZone* aTimeZone, const Locale& aLocale);

  /**
   * Constructs a GregorianCalendar with the given date
   * and time set for the default time zone with the default locale.
   * @param year the value used to set the YEAR time field in the calendar.
   * @param month the value used to set the MONTH time field in the calendar.
   * Month value is 0-based. e.g., 0 for January.
   * @param date the value used to set the DATE time field in the calendar.
   * @param hour the value used to set the HOUR_OF_DAY time field
   * in the calendar.
   * @param minute the value used to set the MINUTE time field
   * in the calendar.
   * @param second the value used to set the SECOND time field
   * in the calendar.
   */
  GregorianCalendar(int year, int month, int date, int hour = 0, int minute = 0, int second = 0);
  
public:
  /**
   * Sets the GregorianCalendar change date. This is the point when the switch
   * from Julian dates to Gregorian dates occurred. Default is October 15,
   * 1582. Previous to this, dates will be in the Julian calendar.
   * <p>
   * To obtain a pure Julian calendar, set the change date to
   * <code>XLONG_MAX</code>.  To obtain a pure Gregorian calendar,
   * set the change date to <code>XLONG_MIN</code>.
   *
   * @param date the given Gregorian cutover date.
   */
  void setGregorianChange(XLONG date);

  /**
   * Gets the Gregorian Calendar change date.  This is the point when the
   * switch from Julian dates to Gregorian dates occurred. Default is
   * October 15, 1582. Previous to this, dates will be in the Julian
   * calendar.
   * @return the Gregorian cutover date for this calendar.
   */
  XLONG getGregorianChange() const;

  /**
   * Determines if the given year is a leap year. Returns true if the
   * given year is a leap year.
   * @param year the given year.
   * @return true if the given year is a leap year; false otherwise.
   */
  bool isLeapYear(int year) const;

  /**
   * Adds the specified (signed) amount of time to the given time field,
   * based on the calendar's rules.
   * <p><em>Add rule 1</em>. The value of <code>field</code>
   * after the call minus the value of <code>field</code> before the
   * call is <code>amount</code>, modulo any overflow that has occurred in
   * <code>field</code>. Overflow occurs when a field value exceeds its
   * range and, as a result, the next larger field is incremented or
   * decremented and the field value is adjusted back into its range.</p>
   *
   * <p><em>Add rule 2</em>. If a smaller field is expected to be
   * invariant, but it is impossible for it to be equal to its
   * prior value because of changes in its minimum or maximum after
   * <code>field</code> is changed, then its value is adjusted to be as close
   * as possible to its expected value. A smaller field represents a
   * smaller unit of time. <code>HOUR</code> is a smaller field than
   * <code>DAY_OF_MONTH</code>. No adjustment is made to smaller fields
   * that are not expected to be invariant. The calendar system
   * determines what fields are expected to be invariant.</p>
   * @param field the time field.
   * @param amount the amount of date or time to be added to the field.
   */
  void add(int field, int amount);

  /**
   * Add to field a signed amount without changing larger fields.
   * A negative roll amount means to subtract from field without changing 
   * larger fields.
   * <p>
   * <em>Example</em>: Consider a <code>GregorianCalendar</code>
   * originally set to August 31, 1999. Calling <code>roll(Calendar.MONTH,
   * 8)</code> sets the calendar to April 30, <strong>1999</strong>. Using a
   * <code>GregorianCalendar</code>, the <code>DAY_OF_MONTH</code> field cannot
   * be 31 in the month April. <code>DAY_OF_MONTH</code> is set to the closest possible
   * value, 30. The <code>YEAR</code> field maintains the value of 1999 because it
   * is a larger field than <code>MONTH</code>.
   * <p>
   * <em>Example</em>: Consider a <code>GregorianCalendar</code>
   * originally set to Sunday June 6, 1999. Calling
   * <code>roll(Calendar.WEEK_OF_MONTH, -1)</code> sets the calendar to
   * Tuesday June 1, 1999, whereas calling
   * <code>add(Calendar.WEEK_OF_MONTH, -1)</code> sets the calendar to
   * Sunday May 30, 1999. This is because the roll rule imposes an
   * additional constraint: The <code>MONTH</code> must not change when the
   * <code>WEEK_OF_MONTH</code> is rolled. Taken together with add rule 1,
   * the resultant date must be between Tuesday June 1 and Saturday June
   * 5. According to add rule 2, the <code>DAY_OF_WEEK</code>, an invariant
   * when changing the <code>WEEK_OF_MONTH</code>, is set to Tuesday, the
   * closest possible value to Sunday (where Sunday is the first day of the
   * week).</p>
   * @param field the time field.
   * @param amount the signed amount to add to <code>field</code>.
   */
 virtual void roll(int field, int amount);

  /**
   * Returns minimum value for the given field.
   * e.g. for Gregorian DAY_OF_MONTH, 1
   * Please see Calendar.getMinimum for descriptions on parameters and
   * the return value.
   */
  int getMinimum(int field) const;
  
  /**
   * Returns maximum value for the given field.
   * e.g. for Gregorian DAY_OF_MONTH, 31
   * Please see Calendar.getMaximum for descriptions on parameters and
   * the return value.
   */
  int getMaximum(int field) const;


  /**
   * Returns highest minimum value for the given field if varies.
   * Otherwise same as getMinimum(). For Gregorian, no difference.
   * Please see Calendar.getGreatestMinimum for descriptions on parameters
   * and the return value.
   */
  int getGreatestMinimum(int field) const;

  /**
   * Returns lowest maximum value for the given field if varies.
   * Otherwise same as getMaximum(). For Gregorian DAY_OF_MONTH, 28
   * Please see Calendar.getLeastMaximum for descriptions on parameters and
   * the return value.
   */
  int getLeastMaximum(int field) const;

  /**
   * Return the minimum value that this field could have, given the current date.
   * For the Gregorian calendar, this is the same as getMinimum() and getGreatestMinimum().
   */
  int getActualMinimum(int field) const;

  /**
   * Return the maximum value that this field could have, given the current date.
   * For example, with the date "Feb 3, 1997" and the DAY_OF_MONTH field, the actual
   * maximum would be 28; for "Feb 3, 1996" it s 29.  Similarly for a Hebrew calendar,
   * for some years the actual maximum for MONTH is 12, and for others 13.
   */
  int getActualMaximum(int field) const;

  /**
   * Return the year that corresponds to the <code>WEEK_OF_YEAR</code> field.
   * This may be one year before or after the calendar year stored
   * in the <code>YEAR</code> field.  For example, January 1, 1999 is considered
   * Friday of week 53 of 1998 (if minimal days in first week is
   * 2 or less, and the first day of the week is Sunday).  Given
   * these same settings, the ISO year of January 1, 1999 is
   * 1998.
   * <p>
   * Warning: This method will complete all fields.
   * @return the year corresponding to the <code>WEEK_OF_YEAR</code> field, which
   * may be one year before or after the <code>YEAR</code> field.
   */
  int getISOYear() const;

  /////////////////////////////
  // Time => Fields computation
  /////////////////////////////

protected:
  /**
   * Converts UTC as milliseconds to time field values.
   * The time is <em>not</em>
   * recomputed first; to recompute the time, then the fields, call the
   * <code>complete</code> method.
   */
  void computeFields();

  /**
   * Overrides Calendar
   * Converts time field values to UTC as milliseconds.
   */
  void computeTime();
  
private:
  /**
   * This computeFieldsImpl implements the conversion from UTC (a
   * millisecond offset from 1970-01-01T00:00:00.000Z) to calendar
   * field values.
   */
  void computeFieldsImpl();
  
  /**
   * Convert the time as milliseconds to the date fields.  Millis must be
   * given as local wall millis to get the correct local day.  For example,
   * if it is 11:30 pm Standard, and DST is in effect, the correct DST millis
   * must be passed in to get the right date.
   * <p>
   * Fields that are completed by this method: ERA, YEAR, MONTH, DATE,
   * DAY_OF_WEEK, DAY_OF_YEAR, WEEK_OF_YEAR, WEEK_OF_MONTH,
   * DAY_OF_WEEK_IN_MONTH.
   * @param theTime the wall-clock time in milliseconds (either Standard or DST),
   * whichever is in effect
   * @param quick if true, only compute the ERA, YEAR, MONTH, DATE,
   * DAY_OF_WEEK, and DAY_OF_YEAR.
   */
  void timeToFields(XLONG theTime, bool quick);
  
  /**
   * Compute the Julian day number under either the Gregorian or the
   * Julian calendar, using the given year and the remaining fields.
   * @param isGregorian if true, use the Gregorian calendar
   * @param year the adjusted year number, with 0 indicating the
   * year 1 BC, -1 indicating 2 BC, etc.
   * @param fieldMaskParam fieldMaskParam[0] is a bit mask to
   * specify which fields have been used to determine the date. The
   * value is updated upon return.
   * @return the Julian day number
   */
  XLONG computeJulianDay(bool isGregorian, int year, vector<int>& fieldMaskParam);

private:
  /**
   * Converts time as milliseconds to Julian day.
   * @param millis the given milliseconds.
   * @return the Julian day number.
   */
  static  XLONG millisToJulianDay(XLONG millis);

  /**
   * Converts Julian day to time as milliseconds.
   * @param julian the given Julian day number.
   * @return time as milliseconds.
   */
 static XLONG julianDayToMillis(XLONG julian);

 static int julianDayToDayOfWeek(XLONG julian);

  /**
   * Divide two XLONG integers, returning the floor of the quotient.
   * <p>
   * Unlike the built-in division, this is mathematically well-behaved.
   * E.g., <code>-1/4</code> => 0
   * but <code>floorDivide(-1,4)</code> => -1.
   * @param numerator the numerator
   * @param denominator a divisor which must be > 0
   * @return the floor of the quotient.
   */
 static XLONG floorDivide(XLONG numerator, XLONG denominator);

  /**
   * Divide two integers, returning the floor of the quotient.
   * <p>
   * Unlike the built-in division, this is mathematically well-behaved.
   * E.g., <code>-1/4</code> => 0
   * but <code>floorDivide(-1,4)</code> => -1.
   * @param numerator the numerator
   * @param denominator a divisor which must be > 0
   * @return the floor of the quotient.
   */
  static int floorDivide(int numerator, int denominator);

  /**
   * Divide two integers, returning the floor of the quotient, and
   * the modulus remainder.
   * <p>
   * Unlike the built-in division, this is mathematically well-behaved.
   * E.g., <code>-1/4</code> => 0 and <code>-1%4</code> => -1,
   * but <code>floorDivide(-1,4)</code> => -1 with <code>remainder[0]</code> => 3.
   * @param numerator the numerator
   * @param denominator a divisor which must be > 0
   * @param remainder an array of at least one element in which the value
   * <code>numerator mod denominator</code> is returned. Unlike <code>numerator
   * % denominator</code>, this will always be non-negative.
   * @return the floor of the quotient.
   */
  static  int floorDivide(int numerator, int denominator, vector<int>& remainder);

  /**
   * Divide two integers, returning the floor of the quotient, and
   * the modulus remainder.
   * <p>
   * Unlike the built-in division, this is mathematically well-behaved.
   * E.g., <code>-1/4</code> => 0 and <code>-1%4</code> => -1,
   * but <code>floorDivide(-1,4)</code> => -1 with <code>remainder[0]</code> => 3.
   * @param numerator the numerator
   * @param denominator a divisor which must be > 0
   * @param remainder an array of at least one element in which the value
   * <code>numerator mod denominator</code> is returned. Unlike <code>numerator
   * % denominator</code>, this will always be non-negative.
   * @return the floor of the quotient.
   */
  static  int floorDivide(XLONG numerator, int denominator,  vector<int>& remainder);

  /**
   * Return the pseudo-time-stamp for two fields, given their
   * individual pseudo-time-stamps.  If either of the fields
   * is unset, then the aggregate is unset.  Otherwise, the
   * aggregate is the later of the two stamps.
   */
  static int aggregateStamp(int stamp_a, int stamp_b);

  /**
   * Return the week number of a day, within a period. This may be the week number in
   * a year, or the week number in a month. Usually this will be a value >= 1, but if
   * some initial days of the period are excluded from week 1, because
   * minimalDaysInFirstWeek is > 1, then the week number will be zero for those
   * initial days. Requires the day of week for the given date in order to determine
   * the day of week of the first day of the period.
   *
   * @param dayOfPeriod  Day-of-year or day-of-month. Should be 1 for first day of period.
   * @param day   Day-of-week for given dayOfPeriod. 1-based with 1=Sunday.
   * @return      Week number, one-based, or zero if the day falls in part of the
   *              month before the first week, when there are days before the first
   *              week because the minimum days in the first week is more than one.
   */
  int weekNumber(int dayOfPeriod, int dayOfWeek);

  int monthLength(int month, int year);

  int monthLength(int month);

  /**
   * Returns the length of the previous month.  For January, returns the
   * arbitrary value 31, which will not be used:  This value is passed to
   * SimpleTimeZone.getOffset(), and if the month is -1 (the month before
   * January), the day value will be ignored.
   */
  int prevMonthLength(int month);

  int yearLength(int year);

  int yearLength();

  /**
   * After adjustments such as add(MONTH), add(YEAR), we don't want the
   * month to jump around.  E.g., we don't want Jan 31 + 1 month to go to Mar
   * 3, we want it to go to Feb 28.  Adjustments which might run into this
   * problem call this method to retain the proper month.
   */
  void pinDayOfMonth();

  /**
   * Validates the values of the set time fields.
   */
  bool validateFields();

  /**
   * Validates the value of the given time field.
   */
  bool boundsCheck(int value, int field);

  /**
   * Return the day number with respect to the epoch.  January 1, 1970 (Gregorian)
   * is day zero.
   */
  XLONG getEpochDay();

  /**
   * Return the ERA.  We need a special method for this because the
   * default ERA is AD, but a zero (unset) ERA is BC.
   */
  int internalGetEra();


public:
  /**
   * Value of the <code>ERA</code> field indicating
   * the period before the common era (before Christ), also known as BCE.
   * The sequence of years at the transition from <code>BC</code> to <code>AD</code> is
   * ..., 2 BC, 1 BC, 1 AD, 2 AD,...
   */
  static const int BC;

  /**
   * Value of the <code>ERA</code> field indicating
   * the common era (Anno Domini), also known as CE.
   * The sequence of years at the transition from <code>BC</code> to <code>AD</code> is
   * ..., 2 BC, 1 BC, 1 AD, 2 AD,...
   */
  static const int AD;

private:
  /*
   * <pre>
   *                            Greatest       Least 
   * Field name        Minimum   Minimum     Maximum     Maximum
   * ----------        -------   -------     -------     -------
   * ERA                     0         0           1           1
   * YEAR                    1         1   292269054   292278994
   * MONTH                   0         0          11          11
   * WEEK_OF_YEAR            1         1          52          53
   * WEEK_OF_MONTH           0         0           4           6
   * DAY_OF_MONTH            1         1          28          31
   * DAY_OF_YEAR             1         1         365         366
   * DAY_OF_WEEK             1         1           7           7
   * DAY_OF_WEEK_IN_MONTH   -1        -1           4           6
   * AM_PM                   0         0           1           1
   * HOUR                    0         0          11          11
   * HOUR_OF_DAY             0         0          23          23
   * MINUTE                  0         0          59          59
   * SECOND                  0         0          59          59
   * MILLISECOND             0         0         999         999
   * ZONE_OFFSET           -12*      -12*         12*         12*
   * DST_OFFSET              0         0           1*          1*
   * </pre>
   * (*) In units of one-hour
   */
  static const int MIN_VALUES[] ;//= { 0, 1, 0, 1, 0, 1, 1, 1, -1, 0, 0, 0, 0, 0, 0, -12 * MILLIS_PER_HOUR, 0 };
  static const int LEAST_MAX_VALUES[];// ={ 1, 292269054, 11, 52, 4, 28, 365, 7, 4, 1, 11, 23, 59, 59, 999, 12 * MILLIS_PER_HOUR, 1 * MILLIS_PER_HOUR };
  static const int MAX_VALUES[] ;//={ 1, 292278994, 11, 53, 6, 31, 366, 7, 6, 1, 11, 23, 59, 59, 999, 12 * MILLIS_PER_HOUR, 1 * MILLIS_PER_HOUR };

  /////////////////////
  // Instance Variables
  /////////////////////

  /**
   * The point at which the Gregorian calendar rules are used, measured in
   * milliseconds from the standard epoch.  Default is October 15, 1582
   * (Gregorian) 00:00:00 UTC or -12219292800000L.  For this value, October 4,
   * 1582 (Julian) is followed by October 15, 1582 (Gregorian).  This
   * corresponds to Julian day number 2299161.
   */
  XLONG gregorianCutover;

  /**
   * Midnight, local time (using this Calendar's TimeZone) at or before the
   * gregorianCutover. This is a pure date value with no time of day or
   * timezone component.
   */
  XLONG normalizedGregorianCutover;

  /**
   * The year of the gregorianCutover, with 0 representing
   * 1 BC, -1 representing 2 BC, etc.
   */
  int gregorianCutoverYear;
};

__OPENCODE_END_NAMESPACE
#endif
