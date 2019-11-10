#include "port/libmath.h"
#include "port/libnumber.h"

#include "port/ioc_math.h" 

__OPENCODE_BEGIN_NAMESPACE

//-------------------------------------------------------------------------------------------
//Satatic Blocks
//-------------------------------------------------------------------------------------------

const double Math::E = 2.7182818284590452354;

const double Math::PI = 3.14159265358979323846;

const double Math::LOG2E     =   1.4426950408889634074;   /* log_2 e */

const double Math::LOG10E    =   0.43429448190325182765;  /* log_10 e */

const double Math::LN2       =   0.69314718055994530942;  /* log_e 2 */

const double Math::LN10      =   2.30258509299404568402;  /* log_e 10 */

const double Math::PI_OVER_2 =   1.57079632679489661923;  /* pi/2 */

const double Math::PI_OVER_4 =   0.78539816339744830962;  /* pi/4 */

const double Math::SQRT2     =   1.41421356237309504880;  /* sqrt(2) */


//-------------------------------------------------------------------------------------------
//Constructor
//-------------------------------------------------------------------------------------------

Math::Math() {}

//-------------------------------------------------------------------------------------------
//Static methods
//-------------------------------------------------------------------------------------------  

double  Math::sin(double a) 
{
  return ::sin(a);
}
  
double  Math::cos(double a) 
{
  return ::cos(a); 
}
 

double  Math::tan(double a) 
{
  return ::tan(a); 
}

double  Math::asin(double a) 
{
  return ::asin(a); 
}

double  Math::acos(double a) 
{
  return ::acos(a); 
}

double  Math::atan(double a) 
{
  return ::atan(a); // default impl. delegates to StrictMath
}

double  Math::toRadians(double angdeg) 
{
  return angdeg / 180.0 * PI;
}

double  Math::toDegrees(double angrad) 
{
  return angrad * 180.0 / PI;
}

double   Math::exp(double a) 
{
  return ::exp(a);
}

double  Math::log(double a) 
{
  return ::log(a); 
}

double  Math::log10(double a) 
{
  return ::log10(a); 
}

double  Math::log2(double a) 
{
#if defined(PORT_WINDOWS)
  return (::log10(a)/::log10(2.0)); 
#elif defined(PORT_AIX)
  return (::log10(a)/::log10(2.0));
#else
  return ::log2(a); 
#endif
}

double  Math::sqrt(double a) 
{
  return ::sqrt(a); 
}

double  Math::ceil(double a) 
{
  return ::ceil(a);
}

double  Math::floor(double a) 
{
  return ::floor(a);
}

double  Math::atan2(double y, double x) 
{
  return ::atan2(y, x);
}

double  Math::pow(double a, double b) 
{
  return ::pow(a, b); 
}

int  Math::round(float a) 
{
  return (int)Math::floor(a + 0.5f);
}

int  Math::round(float a,int mode)
{
  if (mode < Number::ROUND_UP || mode > Number::ROUND_HALF_EVEN)
  {
    mode = Number::ROUND_HALF_UP;
  }

  if(mode == Number::ROUND_CEILING)
  {
    return  (int)Math::ceil(a);
  }
  else if(mode == Number::ROUND_FLOOR)
  {
    return  (int)Math::floor(a);
  }

  if(( (int)(a * 10) %10 ) == 0)
  {
    return 0;
  }

  int  adjust = (a < 0 ? -1 : 1);
  bool increment = false;
  int round_x = (int)a;

  if (mode == Number::ROUND_UP) 
  {        
    /* Away from zero */
    increment = true;
  } 
  else if (mode == Number::ROUND_DOWN) 
  {    
    /* Towards zero */
    increment = false;
  } 
  else
  { 
    int frac_10 = ((int)(a * 10) %10);

    if(frac_10 < 5)
    {
      increment = false;
    } 
    else if(frac_10 > 5)
    {
      increment = true;
    }
    else
    {
      if(mode == Number::ROUND_HALF_DOWN)
      {
        increment = true;
      }
      else if(mode == Number::ROUND_HALF_DOWN)
      {
        increment = false;
      }
      else
      {
        /* mode == ROUND_HALF_EVEN */
        increment = ((round_x % 2) != 0);
      }

    }//~end if(frac_10 < 5)

  }//~end top if (mode == ROUND_UP)

  if(increment)
  {
    return (round_x + adjust);
  }

  return round_x;
}

XLONG  Math::round(double a) 
{
  return (XLONG)Math::floor(a + 0.5);
}

XLONG  Math::round(double a,int mode)
{
  if (mode < Number::ROUND_UP || mode > Number::ROUND_HALF_EVEN)
  {
    mode = Number::ROUND_HALF_UP;
  }

  if(mode == Number::ROUND_CEILING)
  {
    return  (XLONG)Math::ceil(a);
  }
  else if(mode == Number::ROUND_FLOOR)
  {
    return  (XLONG)Math::floor(a);
  }

  if(( (XLONG)(a * 10) %10 ) == 0)
  {
    return 0;
  }

  int  adjust = (a < 0 ? -1 : 1);
  bool increment = false;
  XLONG round_x = (XLONG)a;

  if (mode == Number::ROUND_UP)
  {
    /* Away from zero */
    increment = true;
  }
  else if (mode == Number::ROUND_DOWN)
  {
    /* Towards zero */
    increment = false;
  }
  else
  {
    XLONG frac_10 = ((XLONG)(a * 10) %10);

    if(frac_10 < 5)
    {
      increment = false;
    }
    else if(frac_10 > 5)
    {
      increment = true;
    }
    else
    {
      if(mode == Number::ROUND_HALF_DOWN)
      {
        increment = true;
      }
      else if(mode == Number::ROUND_HALF_DOWN)
      {
        increment = false;
      }
      else
      {
        /* mode == ROUND_HALF_EVEN */
        increment = ((round_x % 2) != 0);
      }

    }//~end if(frac_10 < 5)

  }//~end top if (mode == ROUND_UP)

  if(increment)
  {
    return (round_x + adjust);
  }

  return round_x;
}


int  Math::abs(int a) 
{
  return (a < 0) ? -a : a;
}

XLONG  Math::abs(XLONG a) 
{
  return (a < 0) ? -a : a;
}

float  Math::abs(float a) 
{
    return (a <= 0.0F) ? 0.0F - a : a;
}

double  Math::abs(double a) 
{
   return (a <= 0.0) ? 0.0 - a : a;
}

__OPENCODE_END_NAMESPACE
