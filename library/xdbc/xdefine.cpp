#include "xdbc/xdefine.h"

__OPENCODE_BEGIN_NAMESPACE

SQLException::~SQLException()
{
}

SQLException::SQLException()
:BaseException()
{
}

SQLException::SQLException(const string& message)
:BaseException(message)
{
}

SQLException::SQLException(const string& message,const string& sqlstate)
:BaseException(message,sqlstate)
{
}

SQLException::SQLException(const string& message,XLONG sqlcode)
:BaseException(message,sqlcode)
{
}

SQLException::SQLException(const string& message,const string& sqlstate,XLONG sqlcode)
:BaseException(message,sqlstate,sqlcode)
{
}
  
XLONG SQLException::getSQLCode() const
{
  return __internal_code;
}

const string& SQLException::getSQLState() const
{
  return __internal_addition;
}

const __XDBC_ATTRIBUTES&  SQLManageable::getAttributes() const
{        
  return __mo_attributes;
}

string  SQLManageable::getAttribute(const string& key) const
{
  __XDBC_ATTRIBUTES::const_iterator __find_attribute = __mo_attributes.find(key);
  if(__find_attribute != __mo_attributes.end())
  {
    return __find_attribute->second;
  }  
  
  return string();
}

void    SQLManageable::removeAttribute(const string& key)
{
  __XDBC_ATTRIBUTES::iterator __find_attribute = __mo_attributes.find(key);
  if(__find_attribute != __mo_attributes.end())
  {
    __mo_attributes.erase(__find_attribute);
  }
}
  
void    SQLManageable::setAttribute(const string& key,const string& attribute)
{
  __mo_attributes[key] = attribute;
}    
  
const int SQLImplementation::paramCountUnknown = -1;
  
__OPENCODE_END_NAMESPACE
