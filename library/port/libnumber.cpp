#include "port/libnumber.h"
#include "port/libmath.h"

__OPENCODE_BEGIN_NAMESPACE

ArithmeticException::~ArithmeticException()
{
}

ArithmeticException::ArithmeticException():BaseException()
{
}

ArithmeticException::ArithmeticException(const string& message):BaseException(message)
{
}

NumberFormatException::NumberFormatException():BaseException()
{
}

NumberFormatException::NumberFormatException(const string& message):BaseException(message)
{
}


NumberFormatException::~NumberFormatException()
{
}

NumberFormatException NumberFormatException::forInputString(const string& s) 
{
  return  NumberFormatException(string("For input string: \"") + s + string("\""));
}

//-------------------------------------------------------------------------
// RADIX Limit
//-------------------------------------------------------------------------

const  int Number::MIN_RADIX = 2;

const  int Number::MAX_RADIX = 36;

//-------------------------------------------------------------------------------------------
//Rounding Modes
//-------------------------------------------------------------------------------------------

const int Number::ROUND_UP =            0;


const int Number::ROUND_DOWN =          1;


const int Number::ROUND_CEILING =       2;


const int Number::ROUND_FLOOR =         3;


const int Number::ROUND_HALF_UP =       4;


const int Number::ROUND_HALF_DOWN =     5;


const int Number::ROUND_HALF_EVEN =     6;

//-------------------------------------------------------------------------------------------
// Other Constants
//-------------------------------------------------------------------------------------------

const int Number::BITS_PER_BYTE = 8;


const UXLONG  Number::ULONG_MASK = 0xffffffffL;

const char Number::digits[] = {
  '0' , '1' , '2' , '3' , '4' , '5' ,
  '6' , '7' , '8' , '9' , 'A' , 'B' ,
  'C' , 'D' , 'E' , 'F' , 'G' , 'H' ,
  'I' , 'J' , 'K' , 'L' , 'M' , 'N' ,
  'O' , 'P' , 'Q' , 'R' , 'S' , 'T' ,
  'U' , 'V' , 'W' , 'X' , 'Y' , 'Z'
};


const char Number::digitTens[] = {
  '0', '0', '0', '0', '0', '0', '0', '0', '0', '0',
  '1', '1', '1', '1', '1', '1', '1', '1', '1', '1',
  '2', '2', '2', '2', '2', '2', '2', '2', '2', '2',
  '3', '3', '3', '3', '3', '3', '3', '3', '3', '3',
  '4', '4', '4', '4', '4', '4', '4', '4', '4', '4',
  '5', '5', '5', '5', '5', '5', '5', '5', '5', '5',
  '6', '6', '6', '6', '6', '6', '6', '6', '6', '6',
  '7', '7', '7', '7', '7', '7', '7', '7', '7', '7',
  '8', '8', '8', '8', '8', '8', '8', '8', '8', '8',
  '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
} ; 

const char Number::digitOnes[] = { 
  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
} ;

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------
Number::~Number() {}

Number::Number(){}

short Number::shortValue() const
{
  return (short)intValue();
}


//------------------------------------------------------------------------
//
//------------------------------------------------------------------------
int   Number::parseInt(const char* s, int radix) __OPENCODE_THROW_DECLARE(NumberFormatException)
{
  if (radix < MIN_RADIX)
  {
    string sb;
    sb.append("radix ");
    sb.append(Number::toString(radix));
    sb.append(" less than Number.MIN_RADIX");
    __OPENCODE_THROW(NumberFormatException(sb));
  }

  if (radix > MAX_RADIX)
  {
    string sb;
    sb.append("radix ");
    sb.append(Number::toString(radix));
    sb.append(" greater than Number.MAX_RADIX");
    __OPENCODE_THROW(NumberFormatException(sb));
  }

  return (int)strtol(s,0,radix);
}

  
int   Number::parseInt(const string& s, int radix) __OPENCODE_THROW_DECLARE(NumberFormatException)
{
  return Number::parseInt(s.c_str(),radix);
}

XLONG  Number::parseLong(const char* s, int radix) __OPENCODE_THROW_DECLARE(NumberFormatException)
{
  if (radix < MIN_RADIX)
  {
    string sb;
    sb.append("radix ");
    sb.append(Number::toString(radix));
    sb.append(" less than Number.MIN_RADIX");
    __OPENCODE_THROW(NumberFormatException(sb));
  }

  if (radix > MAX_RADIX)
  {
    string sb;
    sb.append("radix ");
    sb.append(Number::toString(radix));
    sb.append(" greater than Number.MAX_RADIX");
    __OPENCODE_THROW(NumberFormatException(sb));
  }

#if defined(OPENCODE_HAS_STRTOLL)
#  ifndef PORT_HPUX
     return (XLONG)strtoll(s,0,radix);
#  elif defined(OPENCODE_HAS_STRTOL) && defined(OPENCODE_LP64)
     return (XLONG)strtol(s,0,radix);
#  else
     return (XLONG)Number::strtoLong(s,radix);
#  endif
#elif  defined(OPENCODE_HAS_STRTOL)
#  ifdef OPENCODE_LP64
     return (XLONG)strtol(s,0,radix); 
#  else
     return (XLONG)Number::strtoLong(s,radix);
#  endif 
#else
   return (XLONG)Number::strtoLong(s,radix);
#endif
}

XLONG  Number::parseLong(const string& s, int radix) __OPENCODE_THROW_DECLARE(NumberFormatException)
{
  return  Number::parseLong(s.c_str(),radix); 
}

float  Number::parseFloat(const char* s)  __OPENCODE_THROW_DECLARE(NumberFormatException)
{
#ifndef OPENCODE_HAS_STRTOF
  return (float)strtod((char*)s,0);
#else
  return (float)strtof((char*)s,0);
#endif
}

float  Number::parseFloat(const string& s)  __OPENCODE_THROW_DECLARE(NumberFormatException)
{
  return Number::parseFloat(s.c_str());
}
  
double  Number::parseDouble(const char* s)  __OPENCODE_THROW_DECLARE(NumberFormatException)
{
  return (double)strtod(s,0);
}

double  Number::parseDouble(const string& s)  __OPENCODE_THROW_DECLARE(NumberFormatException)
{
  return Number::parseDouble(s.c_str());
}

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------
string  Number::toString(int i, int radix)
{
  if (radix == 10 ||radix < MIN_RADIX || radix > MAX_RADIX)
  {
    char __internal_buf[1024] = {0};
    sprintf(__internal_buf,"%d",i);
    return string(__internal_buf);
  }
  
  string sb;

  if (i < 0)
  {
    i = - i;
    sb.append("-");
  }
  
  char __internal_buf[1024] = {0};
  Number::toString(__internal_buf,(unsigned int)i,radix);
  sb.append(__internal_buf);

  return sb;
}

void  Number::toString(char* buffer,int i, int radix)
{
  if (radix == 10 ||radix < MIN_RADIX || radix > MAX_RADIX)
  {
    sprintf(buffer,"%d",i);
    return;
  }
  

  if (i < 0)
  {
    i = - i;
    *buffer = '-';
  }
  
  Number::toString(buffer + 1,(unsigned int)i,radix);
}

string  Number::toString(unsigned int i,int radix)
{
  if (radix == 10 ||radix < MIN_RADIX || radix > MAX_RADIX)
  {
    char __internal_buf[1024] = {0};
    sprintf(__internal_buf,"%d",i);
    return string(__internal_buf);
  }
  
  int bits = sizeof(unsigned int) * 8;
  double ln2 = Math::log(2);
  double lnr = Math::log(radix);
  int size = (int)(bits* (ln2/lnr)) +1;

  string buf(size,'\0');

  int charPos = size;

  while (i >= radix)
  {
    buf[--charPos] = digits[(i % radix)];
    i = i / radix;
  }

  buf[--charPos] = digits[i];

  return buf.substr(charPos,(size - charPos));

}

void  Number::toString(char* buffer,unsigned int i,int radix)
{
  if (radix == 10 ||radix < MIN_RADIX || radix > MAX_RADIX)
  {
    sprintf(buffer,"%d",i);
    return;
  }
  
  int bits = sizeof(unsigned int) * 8;
  double ln2 = Math::log(2);
  double lnr = Math::log(radix);
  int size = (int)(bits* (ln2/lnr)) +1;
  char __internal_buf[1024] = {0};
  
  int charPos = size;
  while (i >= radix)
  {
    __internal_buf[--charPos] = digits[(i % radix)];
    i = i / radix;
  }
  __internal_buf[--charPos] = digits[i];

  memcpy(buffer,__internal_buf + charPos,(size - charPos));
}

string  Number::toString(long i, int radix)
{
  if (radix == 10 ||radix < MIN_RADIX || radix > MAX_RADIX)
  {
    char __internal_buf[1024] = {0};
    sprintf(__internal_buf,"%ld",i);
    return string(__internal_buf);
  }
  
  string sb;

  if (i < 0)
  {
    i = - i;
    sb.append("-");
  }
  
  char __internal_buf[1024] = {0};
  Number::toString(__internal_buf,(unsigned long)i,radix);
  sb.append(__internal_buf);

  return sb;
}

void  Number::toString(char* buffer,long i, int radix)
{
  if (radix == 10 ||radix < MIN_RADIX || radix > MAX_RADIX)
  {
    sprintf(buffer,"%ld",i);
    return;
  }
  

  if (i < 0)
  {
    i = - i;
    *buffer = '-';
  }
  
  Number::toString(buffer + 1,(unsigned long)i,radix);
}

string  Number::toString(unsigned long i,int radix)
{
  if (radix == 10 ||radix < MIN_RADIX || radix > MAX_RADIX)
  {
    char __internal_buf[1024] = {0};
    sprintf(__internal_buf,"%ld",i);
    return string(__internal_buf);
  }
  
  int bits = sizeof(unsigned long) * 8;
  double ln2 = Math::log(2);
  double lnr = Math::log(radix);
  int size = (int)(bits* (ln2/lnr)) +1;

  string buf(size,'\0');

  int charPos = size;

  while (i >= radix)
  {
    buf[--charPos] = digits[(i % radix)];
    i = i / radix;
  }

  buf[--charPos] = digits[i];

  return buf.substr(charPos,(size - charPos));

}

void  Number::toString(char* buffer,unsigned long i,int radix)
{
  if (radix == 10 ||radix < MIN_RADIX || radix > MAX_RADIX)
  {
    sprintf(buffer,"%ld",i);
    return;
  }
  
  int bits = sizeof(unsigned long) * 8;
  double ln2 = Math::log(2);
  double lnr = Math::log(radix);
  int size = (int)(bits* (ln2/lnr)) +1;
  char __internal_buf[1024] = {0};
  
  int charPos = size;
  while (i >= radix)
  {
    __internal_buf[--charPos] = digits[(i % radix)];
    i = i / radix;
  }
  __internal_buf[--charPos] = digits[i];

  memcpy(buffer,__internal_buf + charPos,(size - charPos));
}

string  Number::toString(long long i, int radix)
{
  if (radix == 10 ||radix < MIN_RADIX || radix > MAX_RADIX)
  {
    char __internal_buf[1024] = {0};
    sprintf(__internal_buf,"%lld",i);
    return string(__internal_buf);
  }
  
  string sb;

  if (i < 0)
  {
    i = - i;
    sb.append("-");
  }
  
  char __internal_buf[1024] = {0};
  Number::toString(__internal_buf,(unsigned long long)i,radix);
  sb.append(__internal_buf);

  return sb;
}

void  Number::toString(char* buffer,long long i, int radix)
{
  if (radix == 10 ||radix < MIN_RADIX || radix > MAX_RADIX)
  {
    sprintf(buffer,"%lld",i);
    return;
  }
  

  if (i < 0)
  {
    i = - i;
    *buffer = '-';
  }
  
  Number::toString(buffer + 1,(unsigned long long)i,radix);
}

string  Number::toString(unsigned long long i,int radix)
{
  if (radix == 10 ||radix < MIN_RADIX || radix > MAX_RADIX)
  {
    char __internal_buf[1024] = {0};
    sprintf(__internal_buf,"%lld",i);
    return string(__internal_buf);
  }
  
  int bits = sizeof(unsigned long long) * 8;
  double ln2 = Math::log(2);
  double lnr = Math::log(radix);
  int size = (int)(bits* (ln2/lnr)) +1;

  string buf(size,'\0');

  int charPos = size;

  while (i >= radix)
  {
    buf[--charPos] = digits[(i % radix)];
    i = i / radix;
  }

  buf[--charPos] = digits[i];

  return buf.substr(charPos,(size - charPos));

}

void  Number::toString(char* buffer,unsigned long long i,int radix)
{
  if (radix == 10 ||radix < MIN_RADIX || radix > MAX_RADIX)
  {
    sprintf(buffer,"%lld",i);
    return;
  }
  
  int bits = sizeof(unsigned long long) * 8;
  double ln2 = Math::log(2);
  double lnr = Math::log(radix);
  int size = (int)(bits* (ln2/lnr)) +1;
  char __internal_buf[1024] = {0};
  
  int charPos = size;
  while (i >= radix)
  {
    __internal_buf[--charPos] = digits[(i % radix)];
    i = i / radix;
  }
  __internal_buf[--charPos] = digits[i];

  memcpy(buffer,__internal_buf + charPos,(size - charPos));
}

string  Number::toString(float f,const char* fmt)
{
  char format[10] = {0};
  char buffer[1024] = {0};
  sprintf(format,"%%%s",fmt);
  sprintf(buffer,format,f);
  
  return string(buffer);
}

void  Number::toString(char* buffer,float f,const char* fmt)
{
  char format[10] = {0};
  sprintf(format,"%%%s",fmt);
  sprintf(buffer,format,f);
}

string  Number::toString(double d,const char* fmt)
{
  char format[10] = {0};
  char buffer[1024] = {0};
  sprintf(format,"%%%s",fmt);
  sprintf(buffer,format,d);
  
  return string(buffer);
}

void  Number::toString(char* buffer,double d,const char* fmt)
{
  char format[10] = {0};
  sprintf(format,"%%%s",fmt);
  sprintf(buffer,format,d);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------

int Number::ushiftRight(int n,int s )
{
  if(n==0)
  {
    return 0;
  }
  else if(n>0)
  {
    return (int)(n >> s);
  }
  else
  {
    int two = 2;
    return (int)((n >> s)+(two<<~s));
  }
}

unsigned int Number::ushiftRight(unsigned int n,int s )
{
  return n >> s;
}

XLONG Number::ushiftRight(XLONG n,int s)
{
  if(n==0)
  {
    return 0;
  }
  else if(n>0)
  {
    return (XLONG)(n >> s);
  }
  else
  {
    XLONG two = 2; 
    return (XLONG)((n >> s)+(two<<~s));
  }
}

UXLONG Number::ushiftRight(UXLONG n,int s)
{
  return n >> s;
}

XLONG  Number::strtoLong(const char* __pstr, int radix)  __OPENCODE_THROW_DECLARE(NumberFormatException)
{
  if (radix < MIN_RADIX)
  {
    string sb;
    sb.append("radix ");
    sb.append(Number::toString(radix));
    sb.append(" less than Number.MIN_RADIX");
    __OPENCODE_THROW(NumberFormatException(sb));
  }

  if (radix > MAX_RADIX)
  {
    string sb;
    sb.append("radix ");
    sb.append(Number::toString(radix));
    sb.append(" greater than Number.MAX_RADIX");
    __OPENCODE_THROW(NumberFormatException(sb));
  }

  XLONG result = 0;
  bool negative = false;
  int i = 0;
  
  string str(__pstr);
  int max = (int)(str.size());
  
  XLONG limit = 0;
  XLONG multmin = 0;
  
  int digit = 0;
  
  if (max > 0) 
  {

    if (str[0] == '-') 
    {
      negative = true;
      limit = XLONG_MIN;
      i++;
    } 
    else 
    {
      limit = - XLONG_MAX;;
      
    }//~end if (str[0] == '-') 
  
    multmin = limit / radix;
    
    if (i < max) 
    {
      digit = Number::parseInt(str.substr(i++,1),radix);
      result = -digit;
      
    }//~end if (i < max) 
  
    while (i < max) 
    {
      // Accumulating negatively avoids surprises near MAX_VALUE
      digit = Number::parseInt(str.substr(i++,1),radix);
      
      if (result < multmin) 
      {
        __OPENCODE_THROW(NumberFormatException::forInputString(str));
      }
      
      result *= radix;
      
      if (result < (limit + digit)) 
      {
        __OPENCODE_THROW(NumberFormatException::forInputString(str));
      }
      
      result -= digit;

    }//~end while (i < max) 
  
  } 
  else 
  {
    __OPENCODE_THROW(NumberFormatException::forInputString(str));
  }

  if (negative) 
  {
    if (i > 1) 
    {
      return result;
    } 
    else 
    { 
      /* Only got "-" */
      __OPENCODE_THROW(NumberFormatException::forInputString(str));
    }
  } 
  else 
  {
    return -result;
  }
}

XLONG  Number::strtoLong(const string& str, int radix)  __OPENCODE_THROW_DECLARE(NumberFormatException)
{
  return Number::strtoLong(str.c_str(),radix);
}
  
float  Number::strtoFloat(const char* str)  __OPENCODE_THROW_DECLARE(NumberFormatException)
{
  return (float)strtod(str,0);
}

float  Number::strtoFloat(const string& str)  __OPENCODE_THROW_DECLARE(NumberFormatException)
{
  return Number::strtoFloat(str.c_str());
}

//-------------------------------------------------------------------------
//static block: IEEE 754 floating-point   Support
//-------------------------------------------------------------------------

int   Number::floatToRawIntBits(float value)
{
  union 
  {
    XINT32 i;
    float f;
  } u;
  
  u.f = (float)value;
  
  return (int)u.i;  
}

float Number::intBitsToFloat(int bits)
{
  union 
  {
    XINT32 i;
    float f;
  } u;
  
  u.i = (XINT32)bits;
  return (float)u.f;
}  

XLONG Number::doubleToRawLongBits(double value)
{
  union 
  {
    XINT64 l;
    double d;
  } u;
  
  u.d = (double)value;
  return (XLONG)u.l;  
}

double Number::longBitsToDouble(XLONG bits)
{
  union 
  {
    XINT64 l;
    double d;
  } u;
  
  u.l = (XINT64)bits;
  return (double)u.d;  
}

bool Number::isNaN(float v)
{
  return false;
}

bool Number::isInfinite(float v)
{
  return false;
}

bool Number::isNaN(double v)
{
  return false;  
}
  
bool Number::isInfinite(double v)
{
  return false;  
}  

__OPENCODE_END_NAMESPACE
