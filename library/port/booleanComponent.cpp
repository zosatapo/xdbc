#include "port/booleanComponent.h"
#include "port/libsys.h"
#include "port/utilities.h"

__OPENCODE_BEGIN_NAMESPACE


BooleanComponent::BooleanComponent(bool boolValue) 
{
  this->value = boolValue;
}

BooleanComponent::BooleanComponent(const string& s) 
{
  this->value = toBoolean(s);
}

bool BooleanComponent::booleanValue() const
{
  return value;
}

bool BooleanComponent::valueOf(const string& s) 
{
  return toBoolean(s);
}

bool BooleanComponent::valueOf(const string& s,bool& matchStricted)
{
  return toBoolean(s,matchStricted);
}

string BooleanComponent::toString() const
{
  return value ? "true" : "false";
}


bool BooleanComponent::getBoolean(const string& name) 
{  
  string value = System::getProperty(name);
  if(!value.empty())
  {      
    return toBoolean(value);
  }


  StringBuffer __sb_name(name);
  __sb_name.replaceAll(0,".","_");
  value = System::getProperty(StringsKit::toUpperCase(__sb_name.str()));

  if(!value.empty())
  {
    return toBoolean(value);
  }    
    
  return false;
}

bool BooleanComponent::getBoolean(const string& name,bool& matchStricted)
{
  string value = System::getProperty(name);
  if(!value.empty())
  {
    return toBoolean(value,matchStricted);
  }

  StringBuffer __sb_name(name);
  __sb_name.replaceAll(0,".","_");
  value = System::getProperty(StringsKit::toUpperCase(__sb_name.str()));

  if(!value.empty())
  {
    return toBoolean(value,matchStricted);
  }

  matchStricted = false;
  return false;
}

//-------------------------------------------------------
//
//-------------------------------------------------------

bool BooleanComponent::toBoolean(const string& value) 
{ 
  return ((!value.empty()) && StringsKit::equalsIgnoreCase(value,"true"));
}

bool BooleanComponent::toBoolean(const string& value,bool& matchStricted)
{
  matchStricted = false;
  bool rv = false;

  if(!value.empty())
  {
    if( StringsKit::equalsIgnoreCase(value,"true"))
    {
      matchStricted = true;
      rv = true;
    }
    else if( StringsKit::equalsIgnoreCase(value,"false"))
    {
      matchStricted = true;
      rv = false;
    }
  }

  //printf("matchStricted : %d , rv : %d\n",matchStricted,rv);
  return rv;
}

__OPENCODE_END_NAMESPACE

