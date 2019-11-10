/** 
 *  date.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef __OPENCODE_DATE_H__
#define __OPENCODE_DATE_H__

#include "port/libdefs.h"
#include "port/calendar.h"

__OPENCODE_BEGIN_NAMESPACE

/**
 * The class <code>Date</code> represents a specific instant
 * in time, with millisecond precision.
 * <p>
 * Although the <code>Date</code> class is intended to reflect 
 * coordinated universal time (UTC), it may not do so exactly, 
 * depending on the Operation System host environment. 
 * Nearly all modern operating systems assume that 1&nbsp;day&nbsp;=
 * 24&nbsp;&times;&nbsp;60&nbsp;&times;&nbsp;60&nbsp;= 86400 seconds 
 * in all cases. In UTC, however, about once every year or two there 
 * is an extra second, called a "leap second." The leap 
 * second is always added as the last second of the day, and always 
 * on December 31 or June 30. For example, the last minute of the 
 * year 1995 was 61 seconds long, thanks to an added leap second. 
 * Most computer clocks are not accurate enough to be able to reflect 
 * the leap-second distinction. 
 * <p>
 * Some computer standards are defined in terms of Greenwich mean 
 * time (GMT), which is equivalent to universal time (UT).  GMT is 
 * the "civil" name for the standard; UT is the 
 * "scientific" name for the same standard. The 
 * distinction between UTC and UT is that UTC is based on an atomic 
 * clock and UT is based on astronomical observations, which for all 
 * practical purposes is an invisibly fine hair to split. Because the 
 * earth's rotation is not uniform (it slows down and speeds up 
 * in complicated ways), UT does not always flow uniformly. Leap 
 * seconds are introduced as needed into UTC so as to keep UTC within 
 * 0.9 seconds of UT1, which is a version of UT with certain 
 * corrections applied. There are rv time and date systems as 
 * well; for example, the time scale used by the satellite-based 
 * global positioning system (GPS) is synchronized to UTC but is 
 * <i>not</i> adjusted for leap seconds. An interesting source of 
 * further information is the U.S. Naval Observatory, particularly 
 * the Directorate of Time at:
 * <blockquote><pre>
 *     <a href=http://tycho.usno.navy.mil>http://tycho.usno.navy.mil</a>
 * </pre></blockquote>
 * <p>
 * and their definitions of "Systems of Time" at:
 * <blockquote><pre>
 *     <a href=http://tycho.usno.navy.mil/systime.html>http://tycho.usno.navy.mil/systime.html</a>
 * </pre></blockquote>
 * <p>
 * In all methods of class <code>Date</code> that accept or return 
 * year, month, date, hours, minutes, and seconds values, the 
 * following representations are used: 
 * <ul>
 * <li>A year <i>y</i> is represented by the integer  <i>y</i>. 
 * <li>A month is represented by an integer from 0 to 11; 0 is January, 
 *     1 is February, and so forth; thus 11 is December. 
 * <li>A date (day of month) is represented by an integer from 1 to 31 
 *     in the usual manner. 
 * <li>An hour is represented by an integer from 0 to 23. Thus, the hour 
 *     from midnight to 1 a.m. is hour 0, and the hour from noon to 1 
 *     p.m. is hour 12. 
 * <li>A minute is represented by an integer from 0 to 59 in the usual manner.
 * <li>A second is represented by an integer from 0 to 61; the values 60 and 
 *     61 occur only for leap seconds and even then only in
 *     Operation System that actually track leap seconds correctly. Because 
 *     of the manner in which leap seconds are currently introduced, it is 
 *     extremely unlikely that two leap seconds will occur in the same 
 *     minute, but this specification follows the date and time conventions 
 *     for ISO C.
 * </ul>
 * <p>
 * In all cases, arguments given to methods for these purposes need 
 * not fall within the indicated ranges; for example, a date may be 
 * specified as January 32 and is interpreted as meaning February 1.
 *
 */
class __OPENCODE_CORE_API_DECL Date
{
public:
  ~Date() ;

  Date(const Date& rhs);
  Date& operator=(const Date& rhs);
   
public:
  /**
   * Allocates a <code>Date</code> object and initializes it so that 
   * it represents the time at which it was allocated, measured to the 
   * nearest millisecond. 
   */
  Date();

  /**
   * Allocates a <code>Date</code> object and initializes it to 
   * represent the specified number of milliseconds since the 
   * standard base time known as "the epoch", namely January 1, 
   * 1970, 00:00:00 GMT. 
   *
   * @param   date   the milliseconds since January 1, 1970, 00:00:00 GMT.
   */
  Date(XLONG date);

  /**
   * Allocates a <code>Date</code> object and initializes it so that 
   * it represents the instant at the start of the second specified 
   * by the <code>year</code>, <code>month</code>, <code>date</code>, 
   * <code>hrs</code>, <code>min</code>, and <code>sec</code> arguments, 
   * in the local time zone. 
   *
   * @param   year    the year .
   * @param   month   the month between 0-11.
   * @param   date    the day of the month between 1-31.
   * @param   hrs     the hours between 0-23.
   * @param   min     the minutes between 0-59.
   * @param   sec     the seconds between 0-59.
   * 
   */
  Date(int year, int month, int date, int hrs = 0, int min = 0, int sec = 0);

    
public:
    
  /**
   * Returns a value that is the  
   * year that contains or begins with the instant in time represented 
   * by this <code>Date</code> object, as interpreted in the local 
   * time zone.
   *
   * @return  the year represented by this date .
   * 
   */
   int getYear() const;

  /**
   * Sets the year of this <tt>Date</tt> object to be the specified 
   * value . This <code>Date</code> object is modified so 
   * that it represents a point in time within the specified year, 
   * with the month, date, hour, minute, and second the same as 
   * before, as interpreted in the local time zone. (Of course, if 
   * the date was February 29, for example, and the year is set to a 
   * non-leap year, then the new date will be treated as if it were 
   * on March 1.)
   *
   * @param   year    the year value.
   * 
   */
   void setYear(int year);

  /**
   * Returns a number representing the month that contains or begins 
   * with the instant in time represented by this <tt>Date</tt> object. 
   * The value returned is between <code>0</code> and <code>11</code>, 
   * with the value <code>0</code> representing January.
   *
   * @return  the month represented by this date.
   * 
   */
   int getMonth() const;

  /**
   * Sets the month of this date to the specified value. This 
   * <tt>Date</tt> object is modified so that it represents a point 
   * in time within the specified month, with the year, date, hour, 
   * minute, and second the same as before, as interpreted in the 
   * local time zone. If the date was October 31, for example, and 
   * the month is set to June, then the new date will be treated as 
   * if it were on July 1, because June has only 30 days.
   *
   * @param   month   the month value between 0-11.
   * 
   */
   void setMonth(int month);

  /**
   * Returns the day of the month represented by this <tt>Date</tt> object. 
   * The value returned is between <code>1</code> and <code>31</code> 
   * representing the day of the month that contains or begins with the 
   * instant in time represented by this <tt>Date</tt> object, as 
   * interpreted in the local time zone.
   *
   * @return  the day of the month represented by this date.
   * 
   */
   int getDate() const;

  /**
   * Sets the day of the month of this <tt>Date</tt> object to the 
   * specified value. This <tt>Date</tt> object is modified so that 
   * it represents a point in time within the specified day of the 
   * month, with the year, month, hour, minute, and second the same 
   * as before, as interpreted in the local time zone. If the date 
   * was April 30, for example, and the date is set to 31, then it 
   * will be treated as if it were on May 1, because April has only 
   * 30 days.
   *
   * @param   date   the day of the month value between 1-31.
   * 
   */
   void setDate(int date);

  /**
   * Returns the day of the week represented by this date. The 
   * returned value (<tt>0</tt> = Sunday, <tt>1</tt> = Monday, 
   * <tt>2</tt> = Tuesday, <tt>3</tt> = Wednesday, <tt>4</tt> = 
   * Thursday, <tt>5</tt> = Friday, <tt>6</tt> = Saturday) 
   * represents the day of the week that contains or begins with 
   * the instant in time represented by this <tt>Date</tt> object, 
   * as interpreted in the local time zone.
   *
   * @return  the day of the week represented by this date.
   * 
   */
   int getDay() const;

  /**
   * Returns the hour represented by this <tt>Date</tt> object. The 
   * returned value is a number (<tt>0</tt> through <tt>23</tt>) 
   * representing the hour within the day that contains or begins 
   * with the instant in time represented by this <tt>Date</tt> 
   * object, as interpreted in the local time zone.
   *
   * @return  the hour represented by this date.
   * 
   */
   int getHours() const;

  /**
   * Sets the hour of this <tt>Date</tt> object to the specified value. 
   * This <tt>Date</tt> object is modified so that it represents a point 
   * in time within the specified hour of the day, with the year, month, 
   * date, minute, and second the same as before, as interpreted in the 
   * local time zone.
   *
   * @param   hours   the hour value.
   * 
   */
   void setHours(int hours);

  /**
   * Returns the number of minutes past the hour represented by this date, 
   * as interpreted in the local time zone. 
   * The value returned is between <code>0</code> and <code>59</code>.
   *
   * @return  the number of minutes past the hour represented by this date.
   * 
   */
   int getMinutes() const;

  /**
   * Sets the minutes of this <tt>Date</tt> object to the specified value. 
   * This <tt>Date</tt> object is modified so that it represents a point 
   * in time within the specified minute of the hour, with the year, month, 
   * date, hour, and second the same as before, as interpreted in the 
   * local time zone.
   *
   * @param   minutes   the value of the minutes.
   * 
   */
    void setMinutes(int minutes);

  /**
   * Returns the number of seconds past the minute represented by this date.
   * The value returned is between <code>0</code> and <code>61</code>. The
   * values <code>60</code> and <code>61</code> can only occur on those 
   * Operation System that take leap seconds into account.
   *
   * @return  the number of seconds past the minute represented by this date.
   * 
   */
   int getSeconds() const;

  /**
   * Sets the seconds of this <tt>Date</tt> to the specified value. 
   * This <tt>Date</tt> object is modified so that it represents a 
   * point in time within the specified second of the minute, with 
   * the year, month, date, hour, and minute the same as before, as 
   * interpreted in the local time zone.
   *
   * @param   seconds   the seconds value.
   */
   void setSeconds(int seconds);
  
public:
  /**
   * Returns the number of milliseconds since January 1, 1970, 00:00:00 GMT
   * represented by this <tt>Date</tt> object.
   *
   * @return  the number of milliseconds since January 1, 1970, 00:00:00 GMT
   *          represented by this date.
   */
   XLONG getTime() const;

  /**
   * Sets this <tt>Date</tt> object to represent a point in time that is 
   * <tt>time</tt> milliseconds after January 1, 1970 00:00:00 GMT. 
   *
   * @param   time   the number of milliseconds.
   */
   void setTime(XLONG time);

public:
  /**
   * Sets the time zone with the given time zone value.
   * @param aTimeZone the given time zone.
   */
  void setTimeZone(const TimeZone* aTimeZone);

  /**
   * Gets the time zone.
   * @return the time zone object associated with this customized date.
   */
  const TimeZone* getTimeZone() const;

  /**
   * Returns the offset, measured in minutes, for the local time zone 
   * relative to UTC that is appropriate for the time represented by 
   * this <tt>Date</tt> object. 
   * <p>
   * For example, in Massachusetts, five time zones west of Greenwich:
   * <blockquote><pre>
   * new Date(96, 1, 14).getTimezoneOffset() returns 300</pre></blockquote>
   * because on February 14, 1996, standard time (Eastern Standard Time) 
   * is in use, which is offset five hours from UTC; but:
   * <blockquote><pre>
   * new Date(96, 5, 1).getTimezoneOffset() returns 240</pre></blockquote>
   * because on June 1, 1996, daylight saving time (Eastern Daylight Time) 
   * is in use, which is offset only four hours from UTC.<p>
   * This method produces the same result as if it computed:
   * <blockquote><pre>
   * (this.getTime() - UTC(this.getYear(), 
   *                       this.getMonth(), 
   *                       this.getDate(),
   *                       this.getHours(), 
   *                       this.getMinutes(), 
   *                       this.getSeconds())) / (60 * 1000)
   * </pre></blockquote>
   *
   * @return  the ntime-zone offset, in minutes, for the current time zone.
   */
   int getTimezoneOffset() const;

public:
  string toString() const;  

public:
  /**
   * Determines the date and time based on the arguments. The 
   * arguments are interpreted as a year, month, day of the month, 
   * hour of the day, minute within the hour, and second within the 
   * minute, exactly as for the <tt>Date</tt> constructor with six 
   * arguments, except that the arguments are interpreted relative 
   * to UTC rather than to the local time zone. The time indicated is 
   * returned represented as the distance, measured in milliseconds, 
   * of that time from the epoch (00:00:00 GMT on January 1, 1970).
   *
   * @param   year    the year .
   * @param   month   the month between 0-11.
   * @param   date    the day of the month between 1-31.
   * @param   hrs     the hours between 0-23.
   * @param   min     the minutes between 0-59.
   * @param   sec     the seconds between 0-59.
   * @return  the number of milliseconds since January 1, 1970, 00:00:00 GMT for
   *          the date and time specified by the arguments. 
   */
  static XLONG UTC(int year, int month, int date,int hrs, int min, int sec);

public:
   static vector<int> parseFields(const string& s /* YYYY-MM-DD HH24:MM:SS.F*/,int year,int month,int date /* or day for interval*/);
         
protected:
  Calendar* p_cal;
};

__OPENCODE_END_NAMESPACE
#endif
