/** 
 *  timeZone.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef __OPENCODE_TIMEZONE_H__
#define __OPENCODE_TIMEZONE_H__

#include "port/libdefs.h"
#include "port/lifetime.h"
#include "port/locale.h"
#include "port/mutex.h"
#include "port/ioc_vector.h"

__OPENCODE_BEGIN_NAMESPACE

/**
 * <code>TimeZone</code> represents a time zone offset, and also figures out daylight
 * savings.
 *
 * <p>
 * Typically, you get a <code>TimeZone</code> using <code>getDefault</code>
 * which creates a <code>TimeZone</code> based on the time zone where the program
 * is running. For example, for a program running in Japan, <code>getDefault</code>
 * creates a <code>TimeZone</code> object based on Japanese Standard Time.
 *
 * <p>
 * You can also get a <code>TimeZone</code> using <code>getTimeZone</code>
 * along with a time zone ID. For instance, the time zone ID for the
 * U.S. Pacific Time zone is "America/Los_Angeles". So, you can get a
 * U.S. Pacific Time <code>TimeZone</code> object with:
 * <blockquote><pre>
 * TimeZone tz = TimeZone.getTimeZone("America/Los_Angeles");
 * </pre></blockquote>
 * You can use the <code>getAvailableIDs</code> method to iterate through
 * all the supported time zone IDs. You can then choose a
 * supported ID to get a <code>TimeZone</code>.
 * If the time zone you want is not represented by one of the
 * supported IDs, then a custom time zone ID can be specified to
 * produce a TimeZone. The syntax of a custom time zone ID is:
 *
 * <blockquote><pre>
 * <a name="CustomID"><i>CustomID:</i></a>
 *         <code>GMT</code> <i>Sign</i> <i>Hours</i> <code>:</code> <i>Minutes</i>
 *         <code>GMT</code> <i>Sign</i> <i>Hours</i> <i>Minutes</i>
 *         <code>GMT</code> <i>Sign</i> <i>Hours</i>
 * <i>Sign:</i> one of
 *         <code>+ -</code>
 * <i>Hours:</i>
 *         <i>Digit</i>
 *         <i>Digit</i> <i>Digit</i>
 * <i>Minutes:</i>
 *         <i>Digit</i> <i>Digit</i>
 * <i>Digit:</i> one of
 *         <code>0 1 2 3 4 5 6 7 8 9</code>
 * </pre></blockquote>
 *
 * <i>Hours</i> must be between 0 to 23 and <i>Minutes</i> must be
 * between 00 to 59.  For example, "GMT+10" and "GMT+0010" mean ten
 * hours and ten minutes ahead of GMT, respectively.
 * <p>
 * The format is locale independent and digits must be taken from the
 * Basic Latin block of the Unicode standard. No daylight saving time
 * transition schedule can be specified with a custom time zone ID. If
 * the specified string doesn't match the syntax, <code>"GMT"</code>
 * is used.
 * <p>
 * When creating a <code>TimeZone</code>, the specified custom time
 * zone ID is normalized in the following syntax:
 * <blockquote><pre>
 * <a name="NormalizedCustomID"><i>NormalizedCustomID:</i></a>
 *         <code>GMT</code> <i>Sign</i> <i>TwoDigitHours</i> <code>:</code> <i>Minutes</i>
 * <i>Sign:</i> one of
 *         <code>+ -</code>
 * <i>TwoDigitHours:</i>
 *         <i>Digit</i> <i>Digit</i>
 * <i>Minutes:</i>
 *         <i>Digit</i> <i>Digit</i>
 * <i>Digit:</i> one of
 *         <code>0 1 2 3 4 5 6 7 8 9</code>
 * </pre></blockquote>
 * For example, TimeZone.getTimeZone("GMT-8").getID() returns "GMT-08:00".
 *
 * <h4>Three-letter time zone IDs</h4>
 *
 */

class __OPENCODE_CORE_API_DECL TimeZone
{
public:
  TimeZone* clone() const;

  /**
   * Returns a string representation of this time zone.
   * @return the string
   */
  virtual string  toString() const = 0;

private:
  virtual TimeZone* doClone() const = 0; 

public:
  virtual ~TimeZone();
  
  TimeZone(const TimeZone& rhs);
  TimeZone& operator=(const TimeZone& rhs);
 
public :

  TimeZone(const string& ID);

  /**
   * Gets the time zone offset, for current date, modified in case of
   * daylight savings. This is the offset to add to UTC to get local time.
   * <p>
   * This method returns a historically correct offset if an
   * underlying <code>TimeZone</code> implementation subclass
   * supports historical Daylight Saving Time schedule and GMT
   * offset changes.
   *
   * @param era the era of the given date.
   * @param year the year in the given date.
   * @param month the month in the given date.
   * Month is 0-based. e.g., 0 for January.
   * @param day the day-in-month of the given date.
   * @param dayOfWeek the day-of-week of the given date.
   * @param milliseconds the milliseconds in day in <em>standard</em>
   * local time.
   *
   * @return the offset in milliseconds to add to GMT to get local time.
   *
   */
  virtual XLONG getOffset(XLONG era, XLONG year, XLONG month, XLONG day, XLONG dayOfWeek, XLONG milliseconds) const= 0;

  /**
   * Returns the offset of this time zone from UTC at the specified
   * date. If Daylight Saving Time is in effect at the specified
   * date, the offset value is adjusted with the amount of daylight
   * saving.
   * <p>
   * This method returns a historically correct offset value if an
   * underlying TimeZone implementation subclass supports historical
   * Daylight Saving Time schedule and GMT offset changes.
   *
   * @param date the date represented in milliseconds since January 1, 1970 00:00:00 GMT
   * @return the amount of time in milliseconds to add to UTC to get local time.
   *
   */
  virtual XLONG getOffset(XLONG date) const;

  /**
   * Gets the raw GMT offset and the amount of daylight saving of this
   * time zone at the given time.
   * @param date the milliseconds (since January 1, 1970,
   * 00:00:00.000 GMT) at which the time zone offset and daylight
   * saving amount are found
   * @param offset an array of int where the raw GMT offset
   * (offset[0]) and daylight saving amount (offset[1]) are stored,
   * or null if those values are not needed. The method assumes that
   * the length of the given array is two or larger.
   * @return the total amount of the raw GMT offset and daylight
   * saving at the specified date.
   *
   */
  virtual XLONG getOffsets(XLONG date, vector<XLONG>* offsets) const;

  /**
   * Sets the base time zone offset to GMT.
   * This is the offset to add to UTC to get local time.
   * <p>
   * If an underlying <code>TimeZone</code> implementation subclass
   * supports historical GMT offset changes, the specified GMT
   * offset is set as the latest GMT offset and the difference from
   * the known latest GMT offset value is used to adjust all
   * historical GMT offset values.
   *
   * @param offsetMillis the given base time zone offset to GMT.
   */
  virtual  void setRawOffset(XLONG offsetMillis) = 0;

  /**
   * Returns the amount of time in milliseconds to add to UTC to get
   * standard time in this time zone. Because this value is not
   * affected by daylight saving time, it is called <I>raw
   * offset</I>.
   * <p>
   * If an underlying <code>TimeZone</code> implementation subclass
   * supports historical GMT offset changes, the method returns the
   * raw offset value of the current date. In Honolulu, for example,
   * its raw offset changed from GMT-10:30 to GMT-10:00 in 1947, and
   * this method always returns -36000000 milliseconds (i.e., -10
   * hours).
   *
   * @return the amount of raw offset time in milliseconds to add to UTC.
   */
  virtual XLONG getRawOffset() const = 0;

  /**
   * Gets the ID of this time zone.
   * @return the ID of this time zone.
   */
  const string& getID() const;

  /**
   * Sets the time zone ID. This does not change any rv data in
   * the time zone object.
   * @param ID the new time zone ID.
   */
  void setID(const string& ID);

  /**
   * Returns a name of this time zone suitable for presentation to the user
   * in the default locale.
   * This method returns the long name, not including daylight savings.
   * If the display name is not available for the locale,
   * then this method returns a string in the 
   * <a href="#NormalizedCustomID">normalized custom ID format</a>.
   * @return the human-readable name of this time zone in the default locale.
   */
  string getDisplayName() const;

  /**
   * Returns a name of this time zone suitable for presentation to the user
   * in the specified locale.
   * This method returns the long name, not including daylight savings.
   * If the display name is not available for the locale,
   * then this method returns a string in the 
   * <a href="#NormalizedCustomID">normalized custom ID format</a>.
   * @param locale the locale in which to supply the display name.
   * @return the human-readable name of this time zone in the given locale
   * or in the default locale if the given locale is not recognized.
   * @since 1.2
   */
  string getDisplayName(const Locale& locale) const;

  /**
   * Returns a name of this time zone suitable for presentation to the user
   * in the default locale.
   * If the display name is not available for the locale, then this
   * method returns a string in the 
   * <a href="#NormalizedCustomID">normalized custom ID format</a>.
   * @param daylight if true, return the daylight savings name.
   * @param style either <code>LONG_NAME</code> or <code>SHORT_NAME</code>
   * @return the human-readable name of this time zone in the default locale.
   */
  string getDisplayName(bool daylight, int style) const;

  /**
   * Returns a name of this time zone suitable for presentation to the user
   * in the specified locale.
   * If the display name is not available for the locale,
   * then this method returns a string in the 
   * <a href="#NormalizedCustomID">normalized custom ID format</a>.
   * @param daylight if true, return the daylight savings name.
   * @param style either <code>LONG_NAME</code> or <code>SHORT_NAME</code>
   * @param locale the locale in which to supply the display name.
   * @return the human-readable name of this time zone in the given locale
   * or in the default locale if the given locale is not recognized.
   */
  string getDisplayName(bool daylight, int style, const Locale& locale) const;

  /**
   * Returns the amount of time to be added to local standard time
   * to get local wall clock time.
   * <p>
   * The default implementation always returns 3600000 milliseconds
   * (i.e., one hour) if this time zone observes Daylight Saving
   * Time. Otherwise, 0 (zero) is returned.
   * <p>
   * If an underlying TimeZone implementation subclass supports
   * historical Daylight Saving Time changes, this method returns
   * the known latest daylight saving value.
   *
   * @return the amount of saving time in milliseconds
   */
  XLONG getDSTSavings() const;

  /**
   * Queries if this time zone uses daylight savings time.
   * <p>
   * If an underlying <code>TimeZone</code> implementation subclass
   * supports historical Daylight Saving Time schedule changes, the
   * method refers to the latest Daylight Saving Time schedule
   * information.
   *
   * @return true if this time zone uses daylight savings time,
   * false, otherwise.
   */
  virtual bool useDaylightTime() const =0;

  /**
   * Queries if the given date is in daylight savings time in
   * this time zone.
   * @param date the given Date.
   * @return true if the given date is in daylight savings time,
   * false, otherwise.
   */
  virtual bool inDaylightTime(XLONG date) const = 0;

  /**
   * Gets the <code>TimeZone</code> for the given ID.
   *
   * @param ID the ID for a <code>TimeZone</code>, either an abbreviation
   * such as "PST", a full name such as "America/Los_Angeles", or a custom
   * ID such as "GMT-8:00". Note that the support of abbreviations is
   * for JDK 1.1.x compatibility only and full names should be used.
   *
   * @return the specified <code>TimeZone</code>, or the GMT zone if the given ID
   * cannot be understood.
   */
  static TimeZone* getTimeZone(const string& ID);

  static TimeZone* getTimeZone(const string& ID, bool fallback);

  /**
   * Gets the available IDs according to the given time zone offset.
   * @param rawOffset the given time zone GMT offset.
   * @return an array of IDs, where the time zone for that ID has
   * the specified GMT offset. For example, "America/Phoenix" and "America/Denver"
   * both have GMT-07:00, but differ in daylight savings behavior.
   */
  static  vector<string> getAvailableIDs(XLONG rawOffset);

  /**
   * Gets all the available IDs supported.
   * @return an array of IDs.
   */
  static vector<string> getAvailableIDs();

  /**
   * Gets the default <code>TimeZone</code> for this host.
   * The source of the default <code>TimeZone</code> 
   * may vary with implementation.
   * @return a default <code>TimeZone</code>.
   */
  static const TimeZone* getDefault();

  /**
   * Sets the <code>TimeZone</code> that is
   * returned by the <code>getDefault</code> method.  If <code>zone</code>
   * is null, reset the default to the value it had originally when the
   * VM first started.
   * @param zone the new default time zone
   */
  static void setDefault(const TimeZone* zone);

public:
  /**
   * Gets the platform defined TimeZone ID.
   *
   */
  static __OPENCODE_NATIVE_CODE string getSystemTimeZoneID(const string& ziHome, const string& country);

  /**
   * Gets the custom time zone ID based on the GMT offset of the
   * platform. (e.g., "GMT+08:00")
   */
  static __OPENCODE_NATIVE_CODE string getSystemGMTOffsetID();

  /**
   * Parses a custom time zone identifier and returns a corresponding zone.
   * This method doesn't support the RFC 822 time zone format. (e.g., +hhmm)
   *
   * @param id a string of the <a href="#CustomID">custom ID form</a>.
   * @return a newly created TimeZone with the given offset and
   * no daylight saving time, or null if the id cannot be parsed.
   */
  static TimeZone*  parseCustomTimeZone(const string& id);

public:
  /**
   * A style specifier for <code>getDisplayName()</code> indicating
   * a short name, such as "PST."
   */
  static const int SHORT_NAME;

  /**
   * A style specifier for <code>getDisplayName()</code> indicating
   * a long name, such as "Pacific Standard Time."
   */
  static const int LONG_NAME;

  static const char GMT_ID[4];
  static const int GMT_ID_LENGTH;
      
protected:
  /**
   * The string identifier of this <code>TimeZone</code>.  This is a
   * programmatic identifier used internally to look up <code>TimeZone</code>
   * objects from the system table and also to map them to their localized
   * display names.  <code>ID</code> values are unique in the system
   * table but may not be for dynamically created zones.
   * @serial
   */
  string ID;
};

class __OPENCODE_CORE_API_DECL TimeZoneDoImpl
{
  DECLARE_LIFETIME_BEGIN(TimeZoneDoImpl)
  DECLARE_LIFETIME_END(TimeZoneDoImpl)
  
public:
  TimeZoneDoImpl();
  ~TimeZoneDoImpl();

public:
  const TimeZone* getDefault();
  void setDefault(const TimeZone* newTimeZone);

private:
  TimeZone* defaultTimeZone;
  Mutex tz_mutex;  
};

typedef SingletonHolder<TimeZoneDoImpl,CreateUsingNew<TimeZoneDoImpl>,SingletonWithLongevity<TimeZoneDoImpl>,MultiThread<TimeZoneDoImpl> > TimeZoneImpl;
__OPENCODE_INLINE unsigned int getLongevity(TimeZoneDoImpl* impl){return __BASE_LONGEVITY_TZ;}

__OPENCODE_END_NAMESPACE
#endif  
