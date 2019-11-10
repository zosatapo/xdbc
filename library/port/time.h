/** 
 *  time.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef __OPENCODE_TIME_H__
#define __OPENCODE_TIME_H__

#include "port/libdefs.h"
#include "port/date.h"

__OPENCODE_BEGIN_NAMESPACE

class __OPENCODE_CORE_API_DECL Time
{
public:
  ~Time();
  
  Time(const Time& rhs);
  Time& operator=(const Time& rhs);
  Time();

public:  
  /**
   * Constructs a <code>Time</code> object initialized with the 
   * given values for the hour, minute, and second.
   * The driver sets the date components to January 1, 1970.
   * <P>
   * The result is undefined if a given argument is out of bounds.
   *
   * @param hour 0 to 23
   * @param minute 0 to 59
   * @param second 0 to 59
   *
   */
Time(int hour, int minute, int second); 
  
  /**
   * Constructs a <code>Time</code> object using a milliseconds time value.
   *
   * @param time milliseconds since January 1, 1970, 00:00:00 GMT;
   *             a negative number is milliseconds before
   *               January 1, 1970, 00:00:00 GMT
   */
Time(XLONG time);

public:
  /**
   * Returns the hour represented by this <tt>Time</tt> object. The 
   * returned value is a number (<tt>0</tt> through <tt>23</tt>) 
   * representing the hour within the day that contains or begins 
   * with the instant in time represented by this <tt>Time</tt> 
   * object, as interpreted in the local time zone.
   *
   * @return  the hour represented by this time.
   * 
   */
   int getHours() const;

  /**
   * Sets the hour of this <tt>Time</tt> object to the specified value. 
   * This <tt>Time</tt> object is modified so that it represents a point 
   * in time within the specified hour of the day, with the year, month, 
   * date, minute, and second the same as before, as interpreted in the 
   * local time zone.
   *
   * @param   hours   the hour value.
   * 
   */
   void setHours(int hours);

  /**
   * Returns the number of minutes past the hour represented by this time, 
   * as interpreted in the local time zone. 
   * The value returned is between <code>0</code> and <code>59</code>.
   *
   * @return  the number of minutes past the hour represented by this time.
   * 
   */
   int getMinutes() const;

  /**
   * Sets the minutes of this <tt>Time</tt> object to the specified value. 
   * This <tt>Time</tt> object is modified so that it represents a point 
   * in time within the specified minute of the hour, with the year, month, 
   * date, hour, and second the same as before, as interpreted in the 
   * local time zone.
   *
   * @param   minutes   the value of the minutes.
   * 
   */
   void setMinutes(int minutes);

  /**
   * Returns the number of seconds past the minute represented by this time.
   * The value returned is between <code>0</code> and <code>61</code>. The
   * values <code>60</code> and <code>61</code> can only occur on those 
   * Operation System that take leap seconds into account.
   *
   * @return  the number of seconds past the minute represented by this time.
   * 
   */
   int getSeconds() const;

  /**
   * Sets the seconds of this <tt>Time</tt> to the specified value. 
   * This <tt>Time</tt> object is modified so that it represents a 
   * point in time within the specified second of the minute, with 
   * the year, month, date, hour, and minute the same as before, as 
   * interpreted in the local time zone.
   *
   * @param   seconds   the seconds value.
   * 
   */
   void setSeconds(int seconds);
  
public:  
  /**
   * Sets a <code>Time</code> object using a milliseconds time value.
   *
   * @param time milliseconds since January 1, 1970, 00:00:00 GMT;
   *             a negative number is milliseconds before
   *               January 1, 1970, 00:00:00 GMT
   */
   void setTime(XLONG time);

  /**
   * Returns the number of milliseconds since January 1, 1970, 00:00:00 GMT
   * represented by this <code>Time</code> object.
   *
   * @return  the number of milliseconds since January 1, 1970, 00:00:00 GMT
   *          represented by this time.
   */
   XLONG getTime()const;  

   string toString() const;

protected:
  Date __date;  
};

__OPENCODE_END_NAMESPACE
#endif

