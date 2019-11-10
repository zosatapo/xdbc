#include "port/locale.h"
#include "port/libsys.h"
#include "port/ioc_sbuf.h"

__OPENCODE_BEGIN_NAMESPACE

//------------------------------------------------------------------------------------------
// LocaleDoImpl class
//------------------------------------------------------------------------------------------

IMPLEMENT_LIFETIME_BEGIN(LocaleDoImpl)
IMPLEMENT_LIFETIME_END(LocaleDoImpl)

LocaleDoImpl::LocaleDoImpl()
{
  defaultLocale = 0;
}

LocaleDoImpl::~LocaleDoImpl()
{
  if(defaultLocale != 0)
  {
    delete defaultLocale;
    defaultLocale = 0;
  }
}

const Locale& LocaleDoImpl::getDefault()
{
  Mutex::Guard g(locale_mutex,System::__M_multithreading); 
  
  if(defaultLocale == 0)
  {
    string xlanguage, xregion, xcountry, xvariant;
  
    xlanguage  = System::getProperty("user.language", "en");
  
    // for compatibility, check for old user.region property
    if ((xregion = System::getProperty("user.region")).length() >0 )
    {
        // region can be of form country, country_variant, or _variant
        size_t i = xregion.find_first_of('_');
        if (i != string::npos)
        {
          xcountry = xregion.substr(0, i);
          xvariant = xregion.substr(i + 1);
        }
        else
        {
          xcountry = xregion;
          xvariant = "";
        }
    }
    else
    {
      xcountry =System::getProperty("user.country");
      xvariant =System::getProperty("user.variant","");
    }
  
    defaultLocale = new Locale(xlanguage, xcountry, xvariant);
  }

  return *defaultLocale;
}

void  LocaleDoImpl::setDefault(const Locale& newLocale)
{
  Mutex::Guard g(locale_mutex,System::__M_multithreading); 

  if(defaultLocale != 0)
  {  
    delete defaultLocale;
  }

  defaultLocale = new Locale(newLocale);
}
//------------------------------------------------------------------------------------------
//Locale class
//------------------------------------------------------------------------------------------

Locale::~Locale()
{
  
}

Locale::Locale(const string& languageValue, const string& countryValue, const string& variantValue) 
{
  this->language = languageValue;
  this->country = countryValue;
  this->variant = variantValue;
}

Locale::Locale() 
{
}

Locale::Locale(const Locale& rhs)
:language(rhs.language),country(rhs.country),variant(rhs.variant)
{
  
}

Locale&  Locale::operator=(const Locale& rv)
{
  if(&rv == this)
  {
    return *this;
  }
  
  this->language = rv.language;
  this->country = rv.country;
  this->variant = rv.variant;
  
  return *this;
}

bool Locale::operator==(const Locale& rv) const
{
  if (language != rv.language) 
  {
    return false;
  }
  
  if (country != rv.country) 
  {
    return false;
  }
  
  if (variant != rv.variant) 
  {
    return false;
  }
  
  return true;
}

bool Locale::operator!=(const Locale& rv) const
{ 
  return !operator==(rv);
}

bool Locale::operator<(const Locale& rv) const
{
  if (language > rv.language) 
  {
    return false;
  }
  
  if (country > rv.country) 
  {
    return false;
  }
  
  if (variant > rv.variant) 
  {
    return false;
  }  

  return true;
}

bool Locale::operator>(const Locale& rv) const
{
  if (language < rv.language) 
  {
    return false;
  }
  
  if (country < rv.country) 
  {
    return false;
  }
  
  if (variant < rv.variant) 
  {
    return false;
  } 

  return true;
}

bool Locale::operator<=(const Locale& rv) const
{
  return !operator>(rv);  
}

bool Locale::operator>=(const Locale& rv) const
{
  return !operator<(rv);  
}
    
const Locale& Locale::getDefault() 
{
  return LocaleImpl::getInstance().getDefault();
}
  
void Locale::setDefault(const Locale& newLocale) 
{
  LocaleImpl::getInstance().setDefault(newLocale);
} 

const string& Locale::getLanguage() const
{
  return language;
}
  
const string& Locale::getCountry() const
{
  return country;
}

const string& Locale::getVariant() const
{
  return variant;
}


string  Locale::getDisplayLanguage() const 
{
  return getDisplayLanguage(getDefault());
}


string  Locale::getDisplayLanguage(const Locale& inLocale) const
{

  return "";
}


string Locale::getDisplayCountry() const
{
  return getDisplayCountry(getDefault());
}


string Locale::getDisplayCountry(const Locale& inLocale) const 
{
  return "";
}


string Locale::getDisplayVariant() const
{
  return getDisplayVariant(getDefault());
}


string Locale::getDisplayVariant(const Locale& inLocale) const
{
  return "";  
}

string Locale::getDisplayName() const
{
  return getDisplayName(getDefault());
}

string Locale::getDisplayName(const Locale& inLocale) const
{
  return "";
}

string  Locale::toString() const
{
  bool l = language.length() != 0;
  bool c = country.length() != 0;
  bool v = variant.length() != 0;
  StringBuffer result(language);
  if (c||(l&&v)) 
  {
    result.append('_').append(country); // This may just append '_'
  }
  if (v&&(l||c)) 
  {
    result.append('_').append(variant);
  }
  
  return result.toString();
}

__OPENCODE_END_NAMESPACE
