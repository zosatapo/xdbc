#include "port/buddhistCalendar.h"

__OPENCODE_BEGIN_NAMESPACE

//------------------------------------------------------------------
//Static Blocks
//------------------------------------------------------------------

const int BuddhistCalendar::buddhistOffset = 543;

//------------------------------------------------------------------
//Constructor
//------------------------------------------------------------------

Calendar* BuddhistCalendar::doClone() const
{
  return new BuddhistCalendar(*this);
}

BuddhistCalendar::~BuddhistCalendar() 
{
}

BuddhistCalendar::BuddhistCalendar(const BuddhistCalendar& rhs)
:GregorianCalendar(rhs)
{
  yearOffset = rhs.yearOffset;
}

BuddhistCalendar& BuddhistCalendar::operator=(const BuddhistCalendar& rhs)
{
  if(&rhs == this)
  {
    return *this;
  }

  GregorianCalendar::operator=(rhs);
  
  yearOffset = rhs.yearOffset;

  return *this;
}

BuddhistCalendar::BuddhistCalendar() : GregorianCalendar()
{
  yearOffset = buddhistOffset;
}


BuddhistCalendar::BuddhistCalendar(const TimeZone* zone) : GregorianCalendar(zone)
{
  yearOffset = buddhistOffset;
}


BuddhistCalendar::BuddhistCalendar(const Locale& aLocale) : GregorianCalendar(aLocale)
{
  yearOffset = buddhistOffset;
}


BuddhistCalendar::BuddhistCalendar(const TimeZone* zone, const Locale& aLocale) : GregorianCalendar(zone, aLocale)
{
  yearOffset = buddhistOffset;
}


int BuddhistCalendar::get(int field) const
{
  if (field == YEAR)
  {
    return GregorianCalendar::get(field) + yearOffset;
  }
  return GregorianCalendar::get(field);
}


void BuddhistCalendar::set(int field, int value)
{
  if (field == YEAR)
  {
    GregorianCalendar::set(field, value - yearOffset);
  }
  else
  {
    GregorianCalendar::set(field, value);
  }
}


void BuddhistCalendar::add(int field, int amount)
{
  yearOffset = 0;
  GregorianCalendar::add(field, amount);
  yearOffset = buddhistOffset;
}


void BuddhistCalendar::roll(int field, int amount)
{
  yearOffset = 0;
  GregorianCalendar::roll(field, amount);
  yearOffset = buddhistOffset;
}

__OPENCODE_END_NAMESPACE
