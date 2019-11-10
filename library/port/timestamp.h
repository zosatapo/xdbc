/** 
 *  timestamp.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef __OPENCODE_TIMESTAMP_H__
#define __OPENCODE_TIMESTAMP_H__

#include "port/libdefs.h"
#include "port/date.h"

__OPENCODE_BEGIN_NAMESPACE

/**
 * This type is a composite of a <code>base.Date</code> and a
 * separate nanoseconds value. Only integral seconds are stored in the
 * <code>base.Date</code> component. The fractional seconds - the nanos - are
 * separate.  
 */
class __OPENCODE_CORE_API_DECL Timestamp
{
public:
  ~Timestamp() ;

  Timestamp(const Timestamp& rhs);
  Timestamp& operator=(const Timestamp& rhs);
  Timestamp();
  
public:
   /**
    * Constructs a <code>Timestamp</code> object initialized
    * with the given values.
    *
    * @param year the year
    * @param month 0 to 11
    * @param date 1 to 31
    * @param hour 0 to 23
    * @param minute 0 to 59
    * @param second 0 to 59
    * @param nano 0 to 999,999,999
    */
  Timestamp(int year, int month, int date,int hour, int minute, int second, int nano = 0);
  
   /**
    * Constructs a <code>Timestamp</code> object
    * using a milliseconds time value. The
    * integral seconds are stored in the underlying date value; the
    * fractional seconds are stored in the <code>nanos</code> field of
    * the <code>Timestamp</code> object.
    *
    * @param time milliseconds since January 1, 1970, 00:00:00 GMT.
    *        A negative number is the number of milliseconds before
    *         January 1, 1970, 00:00:00 GMT.
    */
  Timestamp(XLONG time);

public:
    
  /**
   * Returns a value that is  the 
   * year that contains or begins with the instant in time represented 
   * by this <code>Date</code> object, as interpreted in the local 
   * time zone.
   *
   * @return  the year represented by this timestamp.
   * 
   */
   int getYear() const;

  /**
   * Sets the year of this <tt>Timestamp</tt> object to be the specified 
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
   * with the instant in time represented by this <tt>Timestamp</tt> object. 
   * The value returned is between <code>0</code> and <code>11</code>, 
   * with the value <code>0</code> representing January.
   *
   * @return  the month represented by this timestamp.
   * 
   */
   int getMonth() const;
  /**
   * Sets the month of this timestamp to the specified value. This 
   * <tt>Timestamp</tt> object is modified so that it represents a point 
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
   * Returns the day of the month represented by this <tt>Timestamp</tt> object. 
   * The value returned is between <code>1</code> and <code>31</code> 
   * representing the day of the month that contains or begins with the 
   * instant in time represented by this <tt>Timestamp</tt> object, as 
   * interpreted in the local time zone.
   *
   * @return  the day of the month represented by this timestamp.
   * 
   */
   int getDate() const;

  /**
   * Sets the day of the month of this <tt>Timestamp</tt> object to the 
   * specified value. This <tt>Timestamp</tt> object is modified so that 
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
   * Returns the day of the week represented by this timestamp. The 
   * returned value (<tt>0</tt> = Sunday, <tt>1</tt> = Monday, 
   * <tt>2</tt> = Tuesday, <tt>3</tt> = Wednesday, <tt>4</tt> = 
   * Thursday, <tt>5</tt> = Friday, <tt>6</tt> = Saturday) 
   * represents the day of the week that contains or begins with 
   * the instant in time represented by this <tt>Timestamp</tt> object, 
   * as interpreted in the local time zone.
   *
   * @return  the day of the week represented by this timestamp.
   * 
   */
   int getDay() const;

  /**
   * Returns the hour represented by this <tt>Timestamp</tt> object. The 
   * returned value is a number (<tt>0</tt> through <tt>23</tt>) 
   * representing the hour within the day that contains or begins 
   * with the instant in time represented by this <tt>Timestamp</tt> 
   * object, as interpreted in the local time zone.
   *
   * @return  the hour represented by this timestamp.
   * 
   */
   int getHours() const;

  /**
   * Sets the hour of this <tt>Timestamp</tt> object to the specified value. 
   * This <tt>Timestamp</tt> object is modified so that it represents a point 
   * in time within the specified hour of the day, with the year, month, 
   * date, minute, and second the same as before, as interpreted in the 
   * local time zone.
   *
   * @param   hours   the hour value.
   * 
   */
   void setHours(int hours);

  /**
   * Returns the number of minutes past the hour represented by this timestamp, 
   * as interpreted in the local time zone. 
   * The value returned is between <code>0</code> and <code>59</code>.
   *
   * @return  the number of minutes past the hour represented by this timestamp.
   * 
   */
   int getMinutes() const;

  /**
   * Sets the minutes of this <tt>Timestamp</tt> object to the specified value. 
   * This <tt>Timestamp</tt> object is modified so that it represents a point 
   * in time within the specified minute of the hour, with the year, month, 
   * date, hour, and second the same as before, as interpreted in the 
   * local time zone.
   *
   * @param   minutes   the value of the minutes.
   * 
   */
   void setMinutes(int minutes);

  /**
   * Returns the number of seconds past the minute represented by this timestamp.
   * The value returned is between <code>0</code> and <code>61</code>. The
   * values <code>60</code> and <code>61</code> can only occur on those 
   * Operation System that take leap seconds into account.
   *
   * @return  the number of seconds past the minute represented by this timestamp.
   */
   int getSeconds() const;

  /**
   * Sets the seconds of this <tt>Timestamp</tt> to the specified value. 
   * This <tt>Timestamp</tt> object is modified so that it represents a 
   * point in time within the specified second of the minute, with 
   * the year, month, date, hour, and minute the same as before, as 
   * interpreted in the local time zone.
   *
   * @param   seconds   the seconds value.
   */
   void setSeconds(int seconds);

  /**
   * Gets this <code>Timestamp</code> object's <code>nanos</code> value.
   *
   * @return this <code>Timestamp</code> object's fractional seconds component
   * @see #setNanos
   */
   int getNanos()const;

  /**
   * Sets this <code>Timestamp</code> object's <code>nanos</code> field
   * to the given value.
   *
   * @param n the new fractional seconds component
   * @see #getNanos
   */
   void setNanos(int nano);
    
public:
  /**
   * Sets this <code>Timestamp</code> object to represent a point in time that is
   * <tt>time</tt> milliseconds after January 1, 1970 00:00:00 GMT.
   *
   * @param time   the number of milliseconds.
   */
   void setTime(XLONG time) ;
  
  /**
   * Returns the number of milliseconds since January 1, 1970, 00:00:00 GMT
   * represented by this <code>Timestamp</code> object.
   *
   * @return  the number of milliseconds since January 1, 1970, 00:00:00 GMT
   *          represented by this timestamp.
   */
   XLONG getTime()const;

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
   * this <tt>Timestamp</tt> object. 
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
   * @return  the time-zone offset, in minutes, for the current time zone.
   */
   int getTimezoneOffset() const;

public:
   string toString() const;

protected:
  Date  __date;
  
  /**
   * this <code>Timestamp</code> object's fractional seconds component
   */
  int nanos;  
};

__OPENCODE_END_NAMESPACE
#endif

