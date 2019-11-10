/** 
 *  buddhistCalendar.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef __OPENCODE_BUDDHISTCALENDAR_H__
#define __OPENCODE_BUDDHISTCALENDAR_H__

#include "port/libdefs.h"
#include "port/gregorianCalendar.h"

__OPENCODE_BEGIN_NAMESPACE

class __OPENCODE_CORE_API_DECL BuddhistCalendar : public GregorianCalendar
{
public:
#ifndef OPENCODE_DISABLE_HIDES_USING  
  using GregorianCalendar::set;
#else
  void set(int year, int month, int date)
  {
  	GregorianCalendar::set(year,month,date);
  }

  void set(int year, int month, int date, int hour, int minute)
  {
  	GregorianCalendar::set(year,month,date,hour,minute);
  }

  void set(int year, int month, int date, int hour, int minute, int second)
  {
  	GregorianCalendar::set(year,month,date,hour,minute,second);
  }
#endif


public:
  ~BuddhistCalendar() ;

  BuddhistCalendar(const BuddhistCalendar& rhs);
  BuddhistCalendar& operator=(const BuddhistCalendar& rhs);

  /**
   * Constructs a default BuddhistCalendar using the current time
   * in the default time zone with the default locale.
   */
  BuddhistCalendar();

  /**
   * Constructs a BuddhistCalendar based on the current time
   * in the given time zone with the default locale.
   * @param zone the given time zone.
   */
  BuddhistCalendar(const TimeZone* zone);

  /**
   * Constructs a BuddhistCalendar based on the current time
   * in the default time zone with the given locale.
   * @param aLocale the given locale.
   */
  BuddhistCalendar(const Locale& aLocale);

  /**
   * Constructs a BuddhistCalendar based on the current time
   * in the given time zone with the given locale.
   * @param zone the given time zone.
   * @param aLocale the given locale.
   */
   BuddhistCalendar(const TimeZone* zone,const Locale& aLocale);

public :

  /**
   * Gets the value for a given time field.
   * @param field the given time field.
   * @return the value for the given time field.
   */
   virtual int get(int field) const;

  /**
   * Sets the time field with the given value.
   * @param field the given time field.
   * @param value the value to be set for the given time field.
   */
   virtual void set(int field, int value);

  /**
   * Adds the specified (signed) amount of time to the given time field.
   * @param field the time field.
   * @param amount the amount of date or time to be added to the field.
   */
  virtual void add(int field, int amount);

  /**
   * Add to field a signed amount without changing larger fields.
   * A negative roll amount means to subtract from field without changing
   * larger fields.
   * @param field the time field.
   * @param amount the signed amount to add to <code>field</code>.
   */
  virtual void roll(int field, int amount);

private:
  virtual Calendar* doClone() const;

private :
  int yearOffset;

private:
  static const int buddhistOffset;
};

__OPENCODE_END_NAMESPACE
#endif
