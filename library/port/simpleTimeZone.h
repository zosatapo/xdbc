/** 
 *  simpleTimeZone.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef __OPENCODE_SIMPLETIMEZONE_H__
#define __OPENCODE_SIMPLETIMEZONE_H__

#include "port/libdefs.h"
#include "port/timeZone.h"

__OPENCODE_BEGIN_NAMESPACE

/**
 * <code>SimpleTimeZone</code> is a concrete subclass of <code>TimeZone</code>
 * that represents a time zone for use with a Gregorian calendar.
 * The class holds an offset from GMT, called <em>raw offset</em>, and start
 * and end rules for a daylight saving time schedule.  Since it only holds
 * single values for each, it cannot handle historical changes in the offset
 * from GMT and the daylight saving schedule, except that the {@link
 * #setStartYear setStartYear} method can specify the year when the daylight
 * saving time schedule starts in effect.
 * <p>
 * To construct a <code>SimpleTimeZone</code> with a daylight saving time
 * schedule, the schedule can be described with a set of rules,
 * <em>start-rule</em> and <em>end-rule</em>. A day when daylight saving time
 * starts or ends is specified by a combination of <em>month</em>,
 * <em>day-of-month</em>, and <em>day-of-week</em> values. The <em>month</em>
 * value is represented by a Calendar {@link Calendar#MONTH MONTH} field
 * value, such as {@link Calendar#MARCH}. The <em>day-of-week</em> value is
 * represented by a Calendar {@link Calendar#DAY_OF_WEEK DAY_OF_WEEK} value,
 * such as {@link Calendar#SUNDAY SUNDAY}. The meanings of value combinations
 * are as follows.
 *
 * <ul>
 * <li><b>Exact day of month</b><br>
 * To specify an exact day of month, set the <em>month</em> and
 * <em>day-of-month</em> to an exact value, and <em>day-of-week</em> to zero. For
 * example, to specify March 1, set the <em>month</em> to {@link Calendar#MARCH
 * MARCH}, <em>day-of-month</em> to 1, and <em>day-of-week</em> to 0.</li>
 *
 * <li><b>Day of week on or after day of month</b><br>
 * To specify a day of week on or after an exact day of month, set the
 * <em>month</em> to an exact month value, <em>day-of-month</em> to the day on
 * or after which the rule is applied, and <em>day-of-week</em> to a {@link
 * Calendar#DAY_OF_WEEK DAY_OF_WEEK} field value. For example, to specify the
 * second Sunday of April, set <em>month</em> to {@link Calendar#APRIL APRIL},
 * <em>day-of-month</em> to 8, and <em>day-of-week</em> to {@link
 * Calendar#SUNDAY SUNDAY}.</li>
 *
 * <li><b>Day of week on or before day of month</b><br>
 * To specify a day of the week on or before an exact day of the month, set
 * <em>day-of-month</em> and <em>day-of-week</em> to a negative value. For
 * example, to specify the last Wednesday on or before the 21st of March, set
 * <em>month</em> to {@link Calendar#MARCH MARCH}, <em>day-of-month</em> is -21
 * and <em>day-of-week</em> is {@link Calendar#WEDNESDAY -WEDNESDAY}. </li>
 *
 * <li><b>Last day-of-week of month</b><br>
 * To specify, the last day-of-week of the month, set <em>day-of-week</em> to a
 * {@link Calendar#DAY_OF_WEEK DAY_OF_WEEK} value and <em>day-of-month</em> to
 * -1. For example, to specify the last Sunday of October, set <em>month</em>
 * to {@link Calendar#OCTOBER OCTOBER}, <em>day-of-week</em> to {@link
 * Calendar#SUNDAY SUNDAY} and <em>day-of-month</em> to -1.  </li>
 *
 * </ul>
 * The time of the day at which daylight saving time starts or ends is
 * specified by a millisecond value within the day. There are three kinds of
 * <em>mode</em>s to specify the time: {@link #WALL_TIME}, {@link
 * #STANDARD_TIME} and {@link #UTC_TIME}. For example, if daylight
 * saving time ends
 * at 2:00 am in the wall clock time, it can be specified by 7200000
 * milliseconds in the {@link #WALL_TIME} mode. In this case, the wall clock time
 * for an <em>end-rule</em> means the same thing as the daylight time.
 * <p>
 * The following are examples of parameters for constructing time zone objects.
 * <pre><code>
 *      // Base GMT offset: -8:00
 *      // DST starts:      at 2:00am in standard time
 *      //                  on the first Sunday in April
 *      // DST ends:        at 2:00am in daylight time
 *      //                  on the last Sunday in October
 *      // Save:            1 hour
 *      SimpleTimeZone(-28800000,
 *                     "America/Los_Angeles",
 *                     Calendar::APRIL, 1, -Calendar::SUNDAY,
 *                     7200000,
 *                     Calendar::OCTOBER, -1, Calendar::SUNDAY,
 *                     7200000,
 *                     3600000)
 *
 *      // Base GMT offset: +1:00
 *      // DST starts:      at 1:00am in UTC time
 *      //                  on the last Sunday in March
 *      // DST ends:        at 1:00am in UTC time
 *      //                  on the last Sunday in October
 *      // Save:            1 hour
 *      SimpleTimeZone(3600000,
 *                     "Europe/Paris",
 *                     Calendar::MARCH, -1, Calendar::SUNDAY,
 *                     3600000, SimpleTimeZone.UTC_TIME,
 *                     Calendar::OCTOBER, -1, Calendar::SUNDAY,
 *                     3600000, SimpleTimeZone.UTC_TIME,
 *                     3600000)
 * </code></pre>
 * These parameter rules are also applicable to the set rule methods, such as
 * <code>setStartRule</code>.
 */

class __OPENCODE_CORE_API_DECL SimpleTimeZone :public TimeZone
{
public:
  /**
   * Returns a string representation of this time zone.
   * @return the string
   */
  virtual string   toString() const;

private:
  virtual TimeZone* doClone() const;

public:
  virtual ~SimpleTimeZone();
  
  SimpleTimeZone(const SimpleTimeZone& rhs);
  SimpleTimeZone& operator=(const SimpleTimeZone& rhs); 

public:
  /**
   * Constructs a SimpleTimeZone with the given base time zone offset from GMT
   * and time zone ID with no daylight saving time schedule.
   *
   * @param rawOffset  The base time zone offset in milliseconds to GMT.
   * @param ID         The time zone name that is given to this instance.
   */
  SimpleTimeZone(XLONG rawOffset, const string& ID);

  /**
   * Constructs a SimpleTimeZone with the given base time zone offset from
   * GMT, time zone ID, and rules for starting and ending the daylight
   * time.
   * This constructor takes the full set of the start and end rules
   * parameters, including modes of <code>startTime</code> and
   * <code>endTime</code>. The mode specifies either {@link #WALL_TIME wall
   * time} or {@link #STANDARD_TIME standard time} or {@link #UTC_TIME UTC
   * time}.
   *
   * @param rawOffset       The given base time zone offset from GMT.
   * @param ID              The time zone ID which is given to this object.
   * @param startMonth      The daylight saving time starting month. Month is
   *                        a {@link Calendar#MONTH MONTH} field
   *                        value (0-based. e.g., 0 for January).
   * @param startDay        The day of the month on which the daylight saving time starts.
   *                        See the class description for the special cases of this parameter.
   * @param startDayOfWeek  The daylight saving time starting day-of-week.
   *                        See the class description for the special cases of this parameter.
   * @param startTime       The daylight saving time starting time in the time mode
   *                        specified by <code>startTimeMode</code>.
   * @param endMonth        The daylight saving time ending month. Month is
   *                        a {@link Calendar#MONTH MONTH} field
   *                        value (0-based. e.g., 9 for October).
   * @param endDay          The day of the month on which the daylight saving time ends.
   *                        See the class description for the special cases of this parameter.
   * @param endDayOfWeek    The daylight saving time ending day-of-week.
   *                        See the class description for the special cases of this parameter.
   * @param endTime         The daylight saving ending time in time time mode
   *                        specified by <code>endTimeMode</code>.
   * @param dstSavings      The amount of time in milliseconds saved during
   *                        daylight saving time.
   */
  SimpleTimeZone(
    XLONG rawOffset,
    const string& ID,
    XLONG startMonth,
    XLONG startDay,
    XLONG startDayOfWeek,
    XLONG startTime,
    XLONG endMonth,
    XLONG endDay,
    XLONG endDayOfWeek,
    XLONG endTime,
    XLONG dstSavings);

  /**
   * Constructs a SimpleTimeZone with the given base time zone offset from
   * GMT, time zone ID, and rules for starting and ending the daylight
   * time.
   * This constructor takes the full set of the start and end rules
   * parameters, including modes of <code>startTime</code> and
   * <code>endTime</code>. The mode specifies either {@link #WALL_TIME wall
   * time} or {@link #STANDARD_TIME standard time} or {@link #UTC_TIME UTC
   * time}.
   *
   * @param rawOffset       The given base time zone offset from GMT.
   * @param ID              The time zone ID which is given to this object.
   * @param startMonth      The daylight saving time starting month. Month is
   *                        a {@link Calendar#MONTH MONTH} field
   *                        value (0-based. e.g., 0 for January).
   * @param startDay        The day of the month on which the daylight saving time starts.
   *                        See the class description for the special cases of this parameter.
   * @param startDayOfWeek  The daylight saving time starting day-of-week.
   *                        See the class description for the special cases of this parameter.
   * @param startTime       The daylight saving time starting time in the time mode
   *                        specified by <code>startTimeMode</code>.
   * @param startTimeMode   The mode of the start time specified by startTime.
   * @param endMonth        The daylight saving time ending month. Month is
   *                        a {@link Calendar#MONTH MONTH} field
   *                        value (0-based. e.g., 9 for October).
   * @param endDay          The day of the month on which the daylight saving time ends.
   *                        See the class description for the special cases of this parameter.
   * @param endDayOfWeek    The daylight saving time ending day-of-week.
   *                        See the class description for the special cases of this parameter.
   * @param endTime         The daylight saving ending time in time time mode
   *                        specified by <code>endTimeMode</code>.
   * @param endTimeMode     The mode of the end time specified by endTime
   * @param dstSavings      The amount of time in milliseconds saved during
   *                        daylight saving time.
   */
  SimpleTimeZone(
    XLONG rawOffset,
    const string& ID,
    XLONG startMonth,
    XLONG startDay,
    XLONG startDayOfWeek,
    XLONG startTime,
    XLONG startTimeMode,
    XLONG endMonth,
    XLONG endDay,
    XLONG endDayOfWeek,
    XLONG endTime,
    XLONG endTimeMode,
    XLONG dstSavings);

public:
  /**
   * Sets the daylight saving time starting year.
   *
   * @param year  The daylight saving starting year.
   */
  void setStartYear(XLONG year);

  /**
   * Sets the daylight saving time start rule. For example, if daylight saving
   * time starts on the first Sunday in April at 2 am in local wall clock
   * time, you can set the start rule by calling:
   * <pre><code>setStartRule(Calendar::APRIL, 1, Calendar::SUNDAY, 2*60*60*1000);</code></pre>
   *
   * @param startMonth      The daylight saving time starting month. Month is
   *                        a {@link Calendar#MONTH MONTH} field
   *                        value (0-based. e.g., 0 for January).
   * @param startDay        The day of the month on which the daylight saving time starts.
   *                        See the class description for the special cases of this parameter.
   * @param startDayOfWeek  The daylight saving time starting day-of-week.
   *                        See the class description for the special cases of this parameter.
   * @param startTime       The daylight saving time starting time in local wall clock
   *                        time, which is local standard time in this case.
   */
  void setStartRule(XLONG startMonth, XLONG startDay, XLONG startDayOfWeek, XLONG startTime);

  /**
   * Sets the daylight saving time start rule to a fixed date within a month.
   * This method is equivalent to:
   * <pre><code>setStartRule(startMonth, startDay, 0, startTime)</code></pre>
   *
   * @param startMonth      The daylight saving time starting month. Month is
   *                        a {@link Calendar#MONTH MONTH} field
   *                        value (0-based. e.g., 0 for January).
   * @param startDay        The day of the month on which the daylight saving time starts.
   * @param startTime       The daylight saving time starting time in local wall clock
   *                        time, which is local standard time in this case.
   *                        See the class description for the special cases of this parameter.
   * <code>startDayOfMonth</code>, or <code>startTime</code> parameters are out of range
   */
  void setStartRule(XLONG startMonth, XLONG startDay, XLONG startTime);

  /**
   * Sets the daylight saving time start rule to a weekday before or after the given date within
   * a month, e.g., the first Monday on or after the 8th.
   *
   * @param startMonth      The daylight saving time starting month. Month is
   *                        a {@link Calendar#MONTH MONTH} field
   *                        value (0-based. e.g., 0 for January).
   * @param startDay        The day of the month on which the daylight saving time starts.
   * @param startDayOfWeek  The daylight saving time starting day-of-week.
   * @param startTime       The daylight saving time starting time in local wall clock
   *                        time, which is local standard time in this case.
   * @param after           If true, this rule selects the first <code>dayOfWeek</code> on or
   *                        <em>after</em> <code>dayOfMonth</code>.  If false, this rule
   *                        selects the last <code>dayOfWeek</code> on or <em>before</em>
   *                        <code>dayOfMonth</code>.
   */
  void setStartRule(XLONG startMonth, XLONG startDay, XLONG startDayOfWeek, XLONG startTime, bool after);

  /**
   * Sets the daylight saving time end rule. For example, if daylight saving time
   * ends on the last Sunday in October at 2 am in wall clock time,
   * you can set the end rule by calling:
   * <code>setEndRule(Calendar::OCTOBER, -1, Calendar::SUNDAY, 2*60*60*1000);</code>
   *
   * @param endMonth        The daylight saving time ending month. Month is
   *                        a {@link Calendar#MONTH MONTH} field
   *                        value (0-based. e.g., 9 for October).
   * @param endDay          The day of the month on which the daylight saving time ends.
   *                        See the class description for the special cases of this parameter.
   * @param endDayOfWeek    The daylight saving time ending day-of-week.
   *                        See the class description for the special cases of this parameter.
   * @param endTime         The daylight saving ending time in local wall clock time,
   *                        (in milliseconds within the day) which is local daylight
   *                        time in this case.
   */
  void setEndRule(XLONG endMonth, XLONG endDay, XLONG endDayOfWeek, XLONG endTime);

  /**
   * Sets the daylight saving time end rule to a fixed date within a month.
   * This method is equivalent to:
   * <pre><code>setEndRule(endMonth, endDay, 0, endTime)</code></pre>
   *
   * @param endMonth        The daylight saving time ending month. Month is
   *                        a {@link Calendar#MONTH MONTH} field
   *                        value (0-based. e.g., 9 for October).
   * @param endDay          The day of the month on which the daylight saving time ends.
   * @param endTime         The daylight saving ending time in local wall clock time,
   *                        (in milliseconds within the day) which is local daylight
   *                        time in this case.
   */
  void setEndRule(XLONG endMonth, XLONG endDay, XLONG endTime);

  /**
   * Sets the daylight saving time end rule to a weekday before or after the given date within
   * a month, e.g., the first Monday on or after the 8th.
   *
   * @param endMonth        The daylight saving time ending month. Month is
   *                        a {@link Calendar#MONTH MONTH} field
   *                        value (0-based. e.g., 9 for October).
   * @param endDay          The day of the month on which the daylight saving time ends.
   * @param endDayOfWeek    The daylight saving time ending day-of-week.
   * @param endTime         The daylight saving ending time in local wall clock time,
   *                        (in milliseconds within the day) which is local daylight
   *                        time in this case.
   * @param after           If true, this rule selects the first <code>endDayOfWeek</code> on
   *                        or <em>after</em> <code>endDay</code>.  If false, this rule
   *                        selects the last <code>endDayOfWeek</code> on or before
   *                        <code>endDay</code> of the month.
   */
  void setEndRule(XLONG endMonth, XLONG endDay, XLONG endDayOfWeek, XLONG endTime, bool after);

public:
  /**
   * Returns the offset of this time zone from UTC at the given
   * time. If daylight saving time is in effect at the given time,
   * the offset value is adjusted with the amount of daylight
   * saving.
   *
   * @param date the time at which the time zone offset is found
   * @return the amount of time in milliseconds to add to UTC to get
   * local time.
   */
  XLONG getOffset(XLONG date) const;
  XLONG getOffsets(XLONG date, vector<XLONG>* offsets) const;

  /**
    * Returns the difference in milliseconds between local time and
    * UTC, taking into account both the raw offset and the effect of
    * daylight saving, for the specified date and time.  This method
    * assumes that the start and end month are distinct.  It also
    * uses a default {@link GregorianCalendar} object as its
    * underlying calendar, such as for determining leap years.  Do
    * not use the result of this method with a calendar rv than a
    * default <code>GregorianCalendar</code>.
    *
    * <p><em>Note:  In general, clients should use
    * <code>Calendar::get(ZONE_OFFSET) + Calendar::get(DST_OFFSET)</code>
    * instead of calling this method.</em>
    *
    * @param era       The era of the given date.
    * @param year      The year in the given date.
    * @param month     The month in the given date. Month is 0-based. e.g.,
    *                  0 for January.
    * @param day       The day-in-month of the given date.
    * @param dayOfWeek The day-of-week of the given date.
    * @param millis    The milliseconds in day in <em>standard</em> local time.
    * @return          The milliseconds to add to UTC to get local time.
    * @exception       IllegalArgumentException the <code>era</code>,
    *     <code>month</code>, <code>day</code>, <code>dayOfWeek</code>,
    *     or <code>millis</code> parameters are out of range
    */
  XLONG getOffset(XLONG era, XLONG year, XLONG month, XLONG day, XLONG dayOfWeek, XLONG millis) const;

  /**
   * Gets the GMT offset for this time zone.
   * @return the GMT offset value in milliseconds
   */
  XLONG getRawOffset() const;

  /**
   * Sets the base time zone offset to GMT.
   * This is the offset to add to UTC to get local time.
   */
  void setRawOffset(XLONG offsetMillis);

  /**
   * Sets the amount of time in milliseconds that the clock is advanced
   * during daylight saving time.
   * @param millisSavedDuringDST the number of milliseconds the time is
   * advanced with respect to standard time when the daylight saving time rules
   * are in effect. A positive number, typically one hour (3600000).
   */
  void setDSTSavings(XLONG millisSavedDuringDST);

  /**
   * Returns the amount of time in milliseconds that the clock is
   * advanced during daylight saving time.
   *
   * @return the number of milliseconds the time is advanced with
   * respect to standard time when the daylight saving rules are in
   * effect, or 0 (zero) if this time zone doesn't observe daylight
   * saving time.
   */
  XLONG getDSTSavings() const;

  /**
   * Queries if this time zone uses daylight saving time.
   * @return true if this time zone uses daylight saving time;
   * false otherwise.
   */
  bool useDaylightTime() const;

  /**
   * Queries if the given date is in daylight saving time.
   * @return true if daylight saving time is in effective at the
   * given date; false otherwise.
   */
  bool inDaylightTime(XLONG date) const;

private:
  /**
   * Gets offset, for current date, modified in case of
   * daylight saving time. This is the offset to add <em>to</em> UTC to get local time.
   * Gets the time zone offset, for current date, modified in case of daylight
   * saving time. This is the offset to add to UTC to get local time. Assume
   * that the start and end month are distinct.
   * @param era           The era of the given date.
   * @param year          The year in the given date.
   * @param month         The month in the given date. Month is 0-based. e.g.,
   *                      0 for January.
   * @param day           The day-in-month of the given date.
   * @param dayOfWeek     The day-of-week of the given date.
   * @param millis        The milliseconds in day in <em>standard</em> local time.
   * @param monthLength   The length of the given month in days.
   * @param prevMonthLength The length of the previous month in days.
   * @return              The offset to add to GMT to get local time.
   * @exception IllegalArgumentException the era, month, day,
   * dayOfWeek, millis, or monthLength parameters are out of range
   */
  XLONG getOffset(
    XLONG   era,
    XLONG   year,
    XLONG   month,
    XLONG   day,
    XLONG   dayOfWeek,
    XLONG   millis,
    XLONG   monthLength,
    XLONG   prevMonthLength) const;

  /**
   * Compares the given date in the year to the given rule and returns 1, 0,
   * or -1, depending on whether the date is after, equal to, or before the
   * rule date. The millis are compared directly against the ruleMillis, so
   * any standard-daylight adjustments must be handled by the caller.
   *
   * @return  1 if the date is after the rule date, -1 if the date is before
   *          the rule date, or 0 if the date is equal to the rule date.
   */
  static int compareToRule(
    XLONG   month,
    XLONG   monthLen,
    XLONG   prevMonthLen,
    XLONG   dayOfMonth,
    XLONG   dayOfWeek,
    XLONG   millis,
    XLONG   millisDelta,
    XLONG   ruleMode,
    XLONG   ruleMonth,
    XLONG   ruleDayOfWeek,
    XLONG   ruleDay,
    XLONG   ruleMillis);

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
  void decodeRules();

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
  void decodeStartRule();

  /**
   * Decode the end rule and validate the parameters.  This method is exactly
   * analogous to decodeStartRule().
   */
  void decodeEndRule();

private:
  /**
   * The month in which daylight saving time starts.  This value must be
   * between <code>Calendar::JANUARY</code> and
   * <code>Calendar::DECEMBER</code> inclusive.  This value must not equal
   * <code>endMonth</code>.
   * <p>If <code>useDaylight</code> is false, this value is ignored.
   */
  XLONG startMonth;

  /**
   * This field has two possible interpretations:
   * <dl>
   * <dt><code>startMode == DOW_IN_MONTH</code></dt>
   * <dd>
   * <code>startDay</code> indicates the day of the month of
   * <code>startMonth</code> on which daylight
   * saving time starts, from 1 to 28, 30, or 31, depending on the
   * <code>startMonth</code>.
   * </dd>
   * <dt><code>startMode != DOW_IN_MONTH</code></dt>
   * <dd>
   * <code>startDay</code> indicates which <code>startDayOfWeek</code> in th
   * month <code>startMonth</code> daylight
   * saving time starts on.  For example, a value of +1 and a
   * <code>startDayOfWeek</code> of <code>Calendar::SUNDAY</code> indicates the
   * first Sunday of <code>startMonth</code>.  Likewise, +2 would indicate the
   * second Sunday, and -1 the last Sunday.  A value of 0 is illegal.
   * </dd>
   * </ul>
   * <p>If <code>useDaylight</code> is false, this value is ignored.
   */
  XLONG startDay;

  /**
   * The day of the week on which daylight saving time starts.  This value
   * must be between <code>Calendar::SUNDAY</code> and
   * <code>Calendar::SATURDAY</code> inclusive.
   * <p>If <code>useDaylight</code> is false or
   * <code>startMode == DAY_OF_MONTH</code>, this value is ignored.
   */
  XLONG startDayOfWeek;

  /**
   * The time in milliseconds after midnight at which daylight saving
   * time starts.  This value is expressed as wall time, standard time,
   * or UTC time, depending on the setting of <code>startTimeMode</code>.
   * <p>If <code>useDaylight</code> is false, this value is ignored.
   */
  XLONG startTime;

  /**
   * The format of startTime, either WALL_TIME, STANDARD_TIME, or UTC_TIME.
   */
  XLONG startTimeMode;

  /**
   * The month in which daylight saving time ends.  This value must be
   * between <code>Calendar::JANUARY</code> and
   * <code>Calendar::UNDECIMBER</code>.  This value must not equal
   * <code>startMonth</code>.
   * <p>If <code>useDaylight</code> is false, this value is ignored.
   */
  XLONG endMonth;

  /**
   * This field has two possible interpretations:
   * <dl>
   * <dt><code>endMode == DOW_IN_MONTH</code></dt>
   * <dd>
   * <code>endDay</code> indicates the day of the month of
   * <code>endMonth</code> on which daylight
   * saving time ends, from 1 to 28, 30, or 31, depending on the
   * <code>endMonth</code>.
   * </dd>
   * <dt><code>endMode != DOW_IN_MONTH</code></dt>
   * <dd>
   * <code>endDay</code> indicates which <code>endDayOfWeek</code> in th
   * month <code>endMonth</code> daylight
   * saving time ends on.  For example, a value of +1 and a
   * <code>endDayOfWeek</code> of <code>Calendar::SUNDAY</code> indicates the
   * first Sunday of <code>endMonth</code>.  Likewise, +2 would indicate the
   * second Sunday, and -1 the last Sunday.  A value of 0 is illegal.
   * </dd>
   * </ul>
   * <p>If <code>useDaylight</code> is false, this value is ignored.
   */
  XLONG endDay;

  /**
   * The day of the week on which daylight saving time ends.  This value
   * must be between <code>Calendar::SUNDAY</code> and
   * <code>Calendar::SATURDAY</code> inclusive.
   * <p>If <code>useDaylight</code> is false or
   * <code>endMode == DAY_OF_MONTH</code>, this value is ignored.
   * @serial
   */
  XLONG endDayOfWeek;

  /**
   * The time in milliseconds after midnight at which daylight saving
   * time ends.  This value is expressed as wall time, standard time,
   * or UTC time, depending on the setting of <code>endTimeMode</code>.
   * <p>If <code>useDaylight</code> is false, this value is ignored.
   */
  XLONG endTime;

  /**
   * The format of endTime, either WALL_TIME, STANDARD_TIME, or UTC_TIME.
   */
  XLONG endTimeMode;

  /**
   * The year in which daylight saving time is first observed.  This is an AD
   * value.  If this value is less than 1 then daylight saving time is observed
   * for all AD years.
   * <p>If <code>useDaylight</code> is false, this value is ignored.
   */
  XLONG startYear;

  /**
   * The offset in milliseconds between this zone and GMT.  Negative offsets
   * are to the west of Greenwich.  To obtain local <em>standard</em> time,
   * add the offset to GMT time.  To obtain local wall time it may also be
   * necessary to add <code>dstSavings</code>.
   * @serial
   */
  XLONG rawOffset;

  /**
   * A boolean value which is true if and only if this zone uses daylight
   * saving time.  If this value is false, several rv fields are ignored.
   * @serial
   */
  bool useDaylight; // indicate if this time zone uses DST

  /**
   * Variables specifying the mode of the start rule.  Takes the following
   * values:
   * <dl>
   * <dt><code>DOM_MODE</code></dt>
   * <dd>
   * Exact day of week; e.g., March 1.
   * </dd>
   * <dt><code>DOW_IN_MONTH_MODE</code></dt>    
   * <dd>
   * Day of week in month; e.g., last Sunday in March.
   * </dd>
   * <dt><code>DOW_GE_DOM_MODE</code></dt>
   * <dd>
   * Day of week after day of month; e.g., Sunday on or after March 15.
   * </dd>
   * <dt><code>DOW_LE_DOM_MODE</code></dt>
   * <dd>
   * Day of week before day of month; e.g., Sunday on or before March 15.
   * </dd>
   * </dl>
   * The setting of this field affects the interpretation of the
   * <code>startDay</code> field.
   * <p>If <code>useDaylight</code> is false, this value is ignored.
   */
  XLONG startMode;

  /**
   * Variables specifying the mode of the end rule.  Takes the following
   * values:
   * <dl>
   * <dt><code>DOM_MODE</code></dt>
   * <dd>
   * Exact day of week; e.g., March 1.
   * </dd>
   * <dt><code>DOW_IN_MONTH_MODE</code></dt>    
   * <dd>
   * Day of week in month; e.g., last Sunday in March.
   * </dd>
   * <dt><code>DOW_GE_DOM_MODE</code></dt>
   * <dd>
   * Day of week after day of month; e.g., Sunday on or after March 15.
   * </dd>
   * <dt><code>DOW_LE_DOM_MODE</code></dt>
   * <dd>
   * Day of week before day of month; e.g., Sunday on or before March 15.
   * </dd>
   * </dl>
   * The setting of this field affects the interpretation of the
   * <code>endDay</code> field.
   * <p>If <code>useDaylight</code> is false, this value is ignored.
   */
  XLONG endMode;

  /**
   * A positive value indicating the amount of time saved during DST in
   * milliseconds.
   * Typically one hour (3600000); sometimes 30 minutes (1800000).
   * <p>If <code>useDaylight</code> is false, this value is ignored.
   */
  XLONG dstSavings;

  /**
   * Constants specifying values of startMode and endMode.
   */
  static const XLONG DOM_MODE; // Exact day of month, "Mar 1"
  static const XLONG DOW_IN_MONTH_MODE; // Day of week in month, "lastSun"
  static const XLONG DOW_GE_DOM_MODE; // Day of week after day of month, "Sun>=15"
  static const XLONG DOW_LE_DOM_MODE; // Day of week before day of month, "Sun<=21"

public:
  /**
   * Constant for a mode of start or end time specified as wall clock
   * time.  Wall clock time is standard time for the onset rule, and
   * daylight time for the end rule.
   */
  static const XLONG WALL_TIME; // Zero for backward compatibility

  /**
   * Constant for a mode of start or end time specified as standard time.
   */
  static const XLONG STANDARD_TIME;

  /**
   * Constant for a mode of start or end time specified as UTC. European
   * Union rules are specified as UTC time, for example.
   */
  static const XLONG UTC_TIME;  
};

__OPENCODE_END_NAMESPACE
#endif
