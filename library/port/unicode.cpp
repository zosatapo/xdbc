#include "port/unicode.h"
#include "port/libmath.h" 
#include "port/ioc_sbuf.h" 
#include "port/ioc_smart.h" 

__OPENCODE_BEGIN_NAMESPACE

#ifdef OPENCODE_HAS_TYPE_WCHAR
#  if (OPENCODE_SIZEOF_WCHAR == 4)
#    define __WCHAR_EQUAL_TO_UTF32
#  endif
#  if (OPENCODE_SIZEOF_WCHAR == 2)
#    define __WCHAR_EQUAL_TO_UTF16
#  endif
#else
#  define wchar_t UTF16
#  define __WCHAR_EQUAL_TO_UTF16
#endif


const UTF32 Unicode::MAX_BMP          = 0x0000FFFF;
const UTF32 Unicode::MAX_UTF16        = 0x0010FFFF;
const UTF32 Unicode::MAX_UTF32        = 0x7FFFFFFF;
const UTF32 Unicode::MAX_LEGAL_UTF32  = 0x0010FFFF;

const UTF32 Unicode::REPLACEMENT_CHAR = 0x0000FFFD;

const UTF32 Unicode::SUR_HIGH_START   = 0xD800;
const UTF32 Unicode::SUR_HIGH_END     = 0xDBFF;
const UTF32 Unicode::SUR_LOW_START    = 0xDC00;
const UTF32 Unicode::SUR_LOW_END      = 0xDFFF;

const UTF32 Unicode::HALFSHIFT  = 10; /* used for shifting by 10 bits */
const UTF32 Unicode::HALFBASE = 0x0010000UL;
const UTF32 Unicode::HALFMASK = 0x3FFUL;

UnicodeException::UnicodeException():BaseException()
{
}

UnicodeException::UnicodeException(const string& message):BaseException(message)
{
}

UnicodeException::UnicodeException(const string& message,XLONG errcode)
:BaseException(message,errcode)
{
}

UnicodeException::~UnicodeException()
{
}

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////

/**
 * Index into the table(8 * 32) below with the first byte of a UTF-8 sequence to
 * get the number of trailing bytes that are supposed to follow it.
 *
 */
static const UTF8 __trailingBytesForUTF8[256] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, //0x00  -  0x1F
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, //0x20  -  0x3F
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, //0x40  -  0x5F
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, //0x60  -  0x7F
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, //0x80  -  0x9F
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, //0xA0  -  0xBF 
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, //0xC0  -  0xDF
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5  //0xE0  -  0xEF , 0xF0 - 0xF7 ,  0xF8 - 0xFB , 0xFC - 0xFF
};

/**
 * Magic values subtracted from a buffer value during UTF8 conversion.
 * This table contains as many values as there might be trailing bytes
 * in a UTF-8 sequence.
 */
static const UTF32 __offsetsFromUTF8[6] = { 0x00000000, 0x00003080, 0x000E2080,0x03C82080, 0xFA082080, 0x82082080 };

/**
 * Once the bits are split out into bytes of UTF-8, this is a mask OR-ed
 * into the first byte, depending on how many bytes follow.  There are
 * as many entries in this table as there are UTF-8 sequence types.
 * (I.e., one byte sequence, two byte... etc.). Remember that sequencs
 * for *legal* UTF-8 will be 6 or fewer bytes total.
 */
static const UTF8 __firstByteMark[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };

/** UTF8 first byte magic*/
static const UTF8 __firstByteAnd[7]  = { 0x00, 0x80, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE };

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////

bool UTF8Checker::check(const UTF8* source, int sourceSize, bool nullCheck) __OPENCODE_THROW_DECLARE(UnicodeException)
{
  const UTF8* __source = source;
  const UTF8* __utf8_source = __source;
  
  int __sourcePos = 0;
  UTF8 __utf8_ch = 0;
  UTF8 __utf8_ch_tb = 0;
  int __utf8_size = 0;
    
  while(__sourcePos < sourceSize)
  {   
    __utf8_source = __source + __sourcePos;
    __utf8_ch = *__utf8_source;
    __utf8_ch_tb =  __utf8_ch;
    
    //-------------------------------------------------------------    
    // 1. Check the first octet of the current UTF8 byte. 
    //------------------------------------------------------------- 
    if( (__utf8_ch == 0x00) && nullCheck) // Null check
    {  
      __OPENCODE_THROW(UnicodeException("Null UTF8 bytes"));
      return false;
    }           

    //-------------------------------------------------------------    
    // 2. Check the current UTF8 bytes length. 
    //-------------------------------------------------------------         
    __utf8_size = __trailingBytesForUTF8[__utf8_ch] + 1;    
    if( (__sourcePos + __utf8_size) > sourceSize)
    {
      __OPENCODE_THROW(UnicodeException("Invalid  UTF8  bytes length")); 
      return false;
    }
      
    //-------------------------------------------------------------    
    // 3. Check the current UTF8 trailing bytes. 
    //------------------------------------------------------------- 
       
    for(int __i = __utf8_size - 1; __i > 0; --__i)
    {
      __utf8_ch_tb = __utf8_source[__i];
      
      if( (__utf8_ch_tb < 0x80) || ( __utf8_ch_tb > 0xBF))
      {
        StringBuffer __sb;
        __sb << "Invalid  UTF8 trailing bytes [" << __utf8_size << "," << __i << "]";
        __OPENCODE_THROW(UnicodeException(__sb.str())); 
        return false;
      }  
      
    }//~end for(int __i = 0; __i < __trailingBytes_size;)

////////////////////////////////////////////////////////////////////////////////
 
    //-------------------------------------------------------------    
    // 4. Check the current UTF8 first byte. 
    //------------------------------------------------------------- 
    
    switch(__utf8_size)
    {
  //-------------------------------------------------------------   
      case 6:
      {
        if(__utf8_ch != 0xFD)
        {
          StringBuffer __sb;
          __sb << "Invalid  UTF8 first byte [" << __utf8_size << "," << __utf8_ch << "]"; 
          __OPENCODE_THROW(UnicodeException(__sb.str())); 
          return false;
        }
      }
      break;
  //-------------------------------------------------------------   
      case 5:
      {
        if( (__utf8_ch < 0xF8) || (__utf8_ch > 0xFB) )
        {
          StringBuffer __sb;
          __sb << "Invalid  UTF8 first byte [" << __utf8_size << "," << __utf8_ch << "]"; 
          __OPENCODE_THROW(UnicodeException(__sb.str())); 
          return false;
        }
      }
      break;
  //-------------------------------------------------------------   
      case 4:
      {
        if( (__utf8_ch < 0xF0) || (__utf8_ch > 0xF7) )
        {
          StringBuffer __sb;
          __sb << "Invalid  UTF8 first byte [" << __utf8_size << "," << __utf8_ch << "]"; 
          __OPENCODE_THROW(UnicodeException(__sb.str())); 
          return false;
        }
      }
      break;
  //-------------------------------------------------------------   
      case 3:
      {
        if( (__utf8_ch < 0xE0) || (__utf8_ch > 0xFB) )
        {
          StringBuffer __sb;
          __sb << "Invalid  UTF8 first byte [" << __utf8_size << "," << __utf8_ch << "]"; 
          __OPENCODE_THROW(UnicodeException(__sb.str())); 
          return false;
        }
      }
      break;
  //-------------------------------------------------------------   
      case 2:
      {
        if( (__utf8_ch < 0xC0) || (__utf8_ch > 0xDF) )
        {
          StringBuffer __sb;
          __sb << "Invalid  UTF8 first byte [" << __utf8_size << "," << __utf8_ch << "]"; 
          __OPENCODE_THROW(UnicodeException(__sb.str())); 
          return false;
        }
      }
      break;
  //-------------------------------------------------------------   
      case 1:
      {
        if(__utf8_ch >= 0x80)
        {
          StringBuffer __sb;
          __sb << "Invalid  UTF8 first byte [" << __utf8_size << "," << __utf8_ch << "]"; 
          __OPENCODE_THROW(UnicodeException(__sb.str())); 
          return false;
        }
      }
      break; 
                           
    }//~end switch(__utf8_size)

////////////////////////////////////////////////////////////////////////////////
         
    //-------------------------------------------------------------    
    // 5. Move to next UTF8 bytes. 
    //-------------------------------------------------------------         
    __sourcePos += __utf8_size;
    
  }//~end while(__sourcePos < sourceSize)
  
  return true;
}  

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////
int Unicode::utf8_to_utf16 (const UTF8* source,int sourceSize, UTF16* target, int targetSize,bool strict) __OPENCODE_THROW_DECLARE(UnicodeException)
{
  UTF8Checker::check(source,sourceSize);
  
  const UTF8* __source = source;
  const UTF8* __sourceEnd = __source + sourceSize;
  
  UTF16* __target = target;
  UTF16* __targetEnd = target + targetSize; 
  UTF32  __utf32_ch = 0;
  int __extraBytesToRead = 0;
  
  while (__source < __sourceEnd) 
  {
    __utf32_ch = 0;   
    __extraBytesToRead = __trailingBytesForUTF8[*__source];
    
    if ( (__source + __extraBytesToRead) >= __sourceEnd) 
    {
      __OPENCODE_THROW(UnicodeException("partial character in __target, but hit end"));
      break;
    }
  
    /*
    * The cases all fall through. See "Note A" below.
    */
    switch (__extraBytesToRead) 
    {
      case 5: 
      {
        __utf32_ch += *__source++; 
        __utf32_ch <<= 6;
      }
      case 4: 
      {
        __utf32_ch += *__source++; 
        __utf32_ch <<= 6;
      }
      case 3: 
      {
        __utf32_ch += *__source++; 
        __utf32_ch <<= 6;
      }
      case 2: 
      {
        __utf32_ch += *__source++; 
        __utf32_ch <<= 6;
      }
      case 1: 
      {
        __utf32_ch += *__source++; 
        __utf32_ch <<= 6;
      }
      case 0: 
      {
        __utf32_ch += *__source++;
      }
    }//~end switch (__extraBytesToRead) 
    
    __utf32_ch -= __offsetsFromUTF8[__extraBytesToRead];
  
    if (__target >= __targetEnd) 
    {
      __source -= (__extraBytesToRead + 1); 
      __OPENCODE_THROW(UnicodeException("Target buffer not enough"));
      break;
    }
    
    if (__utf32_ch <= Unicode::MAX_BMP) 
    { 
      /* Target is a character <= 0xFFFF */
      /* UTF-16 surrogate values are illegal in UTF-32 */
      if ( (__utf32_ch >= Unicode::SUR_HIGH_START) && (__utf32_ch <= Unicode::SUR_LOW_END) ) 
      {
        if(strict)
        {
          __source -= (__extraBytesToRead+1); 
          __OPENCODE_THROW(UnicodeException("__target sequence is illegal/malformed"));
          break;
        }
        else
        {
          *__target++ = Unicode::REPLACEMENT_CHAR;
        }          
      } 
      else 
      {
        *__target++ = (UTF16)__utf32_ch; /* normal case */
      }
      
    } 
    else if (__utf32_ch > Unicode::MAX_UTF16) 
    {
      if(strict)
      {
        __source -= (__extraBytesToRead + 1); /* return to the start */
        __OPENCODE_THROW(UnicodeException("__target sequence is illegal/malformed"));
        break; /* Bail out; shouldn't continue */
      }
      else
      {
        *__target++ = Unicode::REPLACEMENT_CHAR;
      } 
    } 
    else 
    {
      /* __target is a character in range 0xFFFF - 0x10FFFF. */
      if (__target + 1 >= __targetEnd) 
      {
        __source -= (__extraBytesToRead + 1);
        __OPENCODE_THROW(UnicodeException("Target buffer not enough"));
        break;
      }
      
      __utf32_ch -= Unicode::HALFBASE;
      *__target++ = (UTF16)((__utf32_ch >> Unicode::HALFSHIFT) + Unicode::SUR_HIGH_START);
      *__target++ = (UTF16)((__utf32_ch & Unicode::HALFMASK) + Unicode::SUR_LOW_START);
      
    }//~end if (__utf32_ch <= Unicode::MAX_BMP) 
    
  }//~end while (__source < __sourceEnd) 
  
  return (int)(__target - target);
}

int Unicode::utf16_to_utf8 (const UTF16* source,int sourceSize, UTF8* target, int targetSize,bool strict) __OPENCODE_THROW_DECLARE(UnicodeException)
{
  const UTF16* __source = source;
  const UTF16* __sourceEnd = source + sourceSize;
  
  UTF8* __target = target;
  UTF8* __targetEnd = target + targetSize; 
  
  int __bytesToWrite = 0;
  
  UTF32  __utf32_ch = 0;
  UTF32  __utf32_ch2 = 0;
    
  const UTF32 byteMask = 0xBF;
  const UTF32 byteMark = 0x80; 
  
  while (__source < __sourceEnd) 
  {
    __utf32_ch = 0;
    __bytesToWrite = 0;
  
    const UTF16* __old_source = __source; /* In case we have to back up because of _target overflow. */
    __utf32_ch = *__target++;
    
    /* If we have a surrogate pair, convert to UTF32 first. */
    if ( (__utf32_ch >= Unicode::SUR_HIGH_START) && (__utf32_ch <= Unicode::SUR_HIGH_END)) 
    {
      /* If the 16 bits following the high surrogate are in the __target buffer... */
      if (__source < __sourceEnd) 
      {
        __utf32_ch2 = *__source;
        /* If it's a low surrogate, convert to UTF32. */
        if ( (__utf32_ch2 >= Unicode::SUR_LOW_START) && (__utf32_ch2 <= Unicode::SUR_LOW_END)) 
        {
            __utf32_ch =   ((__utf32_ch - Unicode::SUR_HIGH_START) << Unicode::HALFSHIFT)
                         + (__utf32_ch2 - Unicode::SUR_LOW_START) + Unicode::HALFBASE;
            ++__target;
        } 
        else if(strict)
        { 
          /* it's an unpaired high surrogate */
          --__target; 
          __OPENCODE_THROW(UnicodeException("__source sequence is illegal/malformed"));
          break;
        }
      } 
      else 
      { 
        /* We don't have the 16 bits following the high surrogate. */
        --__target; /* return to the high surrogate */
        __OPENCODE_THROW(UnicodeException("partial character in __source, but hit end"));
        break;
      }
    } 
    else if(strict)
    {
      /* UTF-16 surrogate values are illegal in UTF-32 */
      if ( (__utf32_ch >= Unicode::SUR_LOW_START) && (__utf32_ch <= Unicode::SUR_LOW_END)) 
      {
        --__target; /* return to the illegal value itself */
        __OPENCODE_THROW(UnicodeException("__source sequence is illegal/malformed"));
        break;
      }
    }
    
    /* Figure out how many bytes the result will require */
    if (__utf32_ch < (UTF32)0x80) 
    {     
      __bytesToWrite = 1;
    } 
    else if (__utf32_ch < (UTF32)0x800) 
    {    
      __bytesToWrite = 2;
    } 
    else if (__utf32_ch < (UTF32)0x10000) 
    {  
      __bytesToWrite = 3;
    } 
    else if (__utf32_ch < (UTF32)0x110000) 
    { 
       __bytesToWrite = 4;
    } 
    else 
    { 
      __bytesToWrite = 3;
      __utf32_ch = Unicode::REPLACEMENT_CHAR;
    }
  
    __target += __bytesToWrite;
    
    if (__target > __targetEnd) 
    {
      __source = __old_source;
      __target -= __bytesToWrite; 
      __OPENCODE_THROW(UnicodeException("Target buffer not enough"));
      break;
    }
    
    switch (__bytesToWrite) 
    { 
      /* note: everything falls through. */
      case 4: 
      {
        *--__target = (UTF8)((__utf32_ch | byteMark) & byteMask); 
        __utf32_ch >>= 6;
      }
      case 3: 
      {
        *--__target = (UTF8)((__utf32_ch | byteMark) & byteMask); 
        __utf32_ch >>= 6;
      }
      case 2: 
      {
        *--__target = (UTF8)((__utf32_ch | byteMark) & byteMask); 
        __utf32_ch >>= 6;
      }
      case 1: 
      {
        *--__target =  (UTF8)(__utf32_ch | __firstByteMark[__bytesToWrite]);
      }
      
    }//~end switch (__bytesToWrite) 
    
    __target += __bytesToWrite;
    
  }//~end while (__target < __sourceEnd) 
    
  return (int)(__target - target);
}

int Unicode::utf8_to_utf32 (const UTF8* source,int sourceSize, UTF32* target, int targetSize,bool strict) __OPENCODE_THROW_DECLARE(UnicodeException)
{
  UTF8Checker::check(source,sourceSize);
  
  const UTF8* __source    = source;
  const UTF8* __sourceEnd = source + sourceSize;
  
  UTF32* __target    = target;
  UTF32* __targetEnd = target + targetSize;
  
  UTF32 __utf32_ch = 0;
  int  __extraBytesToRead = 0;
  
  while (__source < __sourceEnd) 
  {   
    __utf32_ch = 0;
    __extraBytesToRead = __trailingBytesForUTF8[*__source];
    
    if ( (__source + __extraBytesToRead) >= __sourceEnd) 
    {
      __OPENCODE_THROW(UnicodeException("Invalid  UTF8  bytes length"));   
      break;
    }
    
    /** The cases all fall through. See "Note A" below.*/
    switch (__extraBytesToRead) 
    {
      case 5: 
      {
        __utf32_ch += *__source++; 
        __utf32_ch <<= 6;
      }
      case 4: 
      {
        __utf32_ch += *__source++; 
        __utf32_ch <<= 6;
      }
      case 3: 
      {
        __utf32_ch += *__source++; 
        __utf32_ch <<= 6;
      }
      case 2: 
      {
        __utf32_ch += *__source++; 
        __utf32_ch <<= 6;
      }
      case 1: 
      {
        __utf32_ch += *__source++; 
        __utf32_ch <<= 6;
      }
      case 0: 
      {
        __utf32_ch += *__source++;
      }
      
    }//~end switch (__extraBytesToRead) 

    __utf32_ch -= __offsetsFromUTF8[__extraBytesToRead];

    if (__target >= __targetEnd) 
    {
      __source -= (__extraBytesToRead+1);
      __OPENCODE_THROW(UnicodeException("Target buffer not enough"));
      break;
    }
    
    if (__utf32_ch <= Unicode::MAX_LEGAL_UTF32) 
    {
      /**
       * UTF-16 surrogate values are illegal in UTF-32, and anything over Plane 17 (> 0x10FFFF) is illegal.
       */
      if ( (__utf32_ch >= Unicode::SUR_HIGH_START) && (__utf32_ch <= Unicode::SUR_LOW_END) ) 
      {
        if( strict)
        {
          __source -= (__extraBytesToRead + 1); 
          __OPENCODE_THROW(UnicodeException("__target sequence is illegal/malformed"));
          break;
        }
        else
        {
          *__target++ = Unicode::REPLACEMENT_CHAR;
        }  
      } 
      else 
      {
        *__target++ = __utf32_ch;
      }
    } 
    else 
    { 
      *__target++ = Unicode::REPLACEMENT_CHAR;
      __OPENCODE_THROW(UnicodeException("__target sequence is illegal/malformed"));
      break;
      
    }//~end if (__utf32_ch <= Unicode::MAX_LEGAL_UTF32) 
    
  }//~end while (__source < __sourceEnd) 
        
  return (int)(__target - target);
}

int Unicode::utf32_to_utf8 (const UTF32* source,int sourceSize, UTF8* target, int targetSize,bool strict) __OPENCODE_THROW_DECLARE(UnicodeException)
{
  const UTF32* __source    = source;
  const UTF32* __sourceEnd = source + sourceSize;
  
  UTF8* __target    = target;
  UTF8* __targetEnd = target + targetSize;
  
  UTF32 __utf32_ch = 0;
  int  __bytesToWrite = 0;
  
  const UTF32 byteMask = 0xBF;
  const UTF32 byteMark = 0x80; 
  
  while (__source < __sourceEnd) 
  {
    __utf32_ch = 0;
    __bytesToWrite = 0;
  
    __utf32_ch = *__source++;
    
    if(strict)
    {
      /* UTF-16 surrogate values are illegal in UTF-32 */
      if ( (__utf32_ch >= Unicode::SUR_HIGH_START) && (__utf32_ch <= Unicode::SUR_LOW_END) ) 
      {
        --__source;
        __OPENCODE_THROW(UnicodeException("__source sequence is illegal/malformed"));
        break;
      }
    }
    
    /*
     * Figure out how many bytes the result will require. Turn any
     * illegally large UTF32 things (> Plane 17) into replacement chars.
     */
    if (__utf32_ch < (UTF32)0x80) 
    {     
       __bytesToWrite = 1;
    } 
    else if (__utf32_ch < (UTF32)0x800) 
    {     
      __bytesToWrite = 2;
    } 
    else if (__utf32_ch < (UTF32)0x10000) 
    {   
      __bytesToWrite = 3;
    } 
    else if (__utf32_ch <= Unicode::MAX_LEGAL_UTF32) 
    {  
      __bytesToWrite = 4;
    } 
    else 
    { __bytesToWrite = 3;
      __utf32_ch = Unicode::REPLACEMENT_CHAR;
      __OPENCODE_THROW(UnicodeException("__source sequence is illegal/malformed"));
    }
    
    __target += __bytesToWrite;
    if (__target > __targetEnd) 
    {
      --__source; /* Back up __source pointer! */
      __target -= __bytesToWrite; 
      __OPENCODE_THROW(UnicodeException("Target buffer not enough"));
      break;
    }
    
    switch (__bytesToWrite) 
    { 
      /* note: everything falls through. */
      case 4: 
      {
        *--__target = (UTF8)((__utf32_ch | byteMark) & byteMask); 
        __utf32_ch >>= 6;
      }
      case 3: 
      {
        *--__target = (UTF8)((__utf32_ch | byteMark) & byteMask); 
        __utf32_ch >>= 6;
      }
      case 2: 
      {
        *--__target = (UTF8)((__utf32_ch | byteMark) & byteMask); 
        __utf32_ch >>= 6;
      }
      case 1: 
      {
        *--__target = (UTF8) (__utf32_ch | __firstByteMark[__bytesToWrite]);
      }  
    }//~end switch (__bytesToWrite) 
    
    __target += __bytesToWrite;
    
  }//~end while (__source < __sourceEnd) 

  return (int)(__target - target);
}

int Unicode::utf16_to_utf32 (const UTF16* source,int sourceSize, UTF32* target, int targetSize,bool strict) __OPENCODE_THROW_DECLARE(UnicodeException)
{
  const UTF16* __source    = source;
  const UTF16* __sourceEnd = source + sourceSize;
  
  UTF32* __target    = target;
  UTF32* __targetEnd = target + targetSize;
  
  UTF32 __utf32_ch = 0;
  UTF32 __utf32_ch2 = 0;
      
  while (__source < __sourceEnd) 
  {
    const UTF16* __old__source = __source; /*  In case we have to back up because of __target overflow. */
    __utf32_ch = *__source++;
    
    /* If we have a surrogate pair, convert to UTF32 first. */
    if ( (__utf32_ch >= Unicode::SUR_HIGH_START) && (__utf32_ch <= Unicode::SUR_HIGH_END)) 
    {
      /* If the 16 bits following the high surrogate are in the __source buffer... */
      if (__source < __sourceEnd) 
      {
        __utf32_ch2 = *__source;
        
        /* If it's a low surrogate, convert to UTF32. */
        if (__utf32_ch2 >= Unicode::SUR_LOW_START && __utf32_ch2 <= Unicode::SUR_LOW_END) 
        {
          __utf32_ch = ((__utf32_ch - Unicode::SUR_HIGH_START) << Unicode::HALFSHIFT)
          + (__utf32_ch2 - Unicode::SUR_LOW_START) + Unicode::HALFBASE;
          ++__source;
        } 
        else if(strict)
        {
          --__source; /* return to the illegal value itself */
          __OPENCODE_THROW(UnicodeException("__source sequence is illegal/malformed"));
          break;
        }
      } 
      else 
      { 
        /* We don't have the 16 bits following the high surrogate. */
        --__source; 
        __OPENCODE_THROW(UnicodeException("partial character in __source, but hit end"));
        break;
      }
    } 
    else if(strict)
    {
      /* UTF-16 surrogate values are illegal in UTF-32 */
      if ( (__utf32_ch >= Unicode::SUR_LOW_START ) && (__utf32_ch <= Unicode::SUR_LOW_END)) 
      {
        --__source; /* return to the illegal value itself */
        __OPENCODE_THROW(UnicodeException("__source sequence is illegal/malformed"));
        break;
      }
      
    }//~end if ( (__utf32_ch >= Unicode::SUR_HIGH_START) && (__utf32_ch <= Unicode::SUR_HIGH_END)) 
    
    if (__target >= __targetEnd) 
    {
      __source = __old__source; 
      __OPENCODE_THROW(UnicodeException("Target buffer not enough"));
      break;
    }
    *__target++ = __utf32_ch;
  
  }//~end while (__source < __sourceEnd) 
  
  return (int)(__target - target);
}

int Unicode::utf32_to_utf16 (const UTF32* source,int sourceSize, UTF16* target, int targetSize,bool strict) __OPENCODE_THROW_DECLARE(UnicodeException)
{
  const UTF32* __source    = source;
  const UTF32* __sourceEnd = __source + sourceSize;
  
  UTF16* __target    = target;
  UTF16* __targetEnd = target + targetSize;

  UTF32 __utf32_ch = 0;
  
  while (__source < __sourceEnd) 
  { 
    __utf32_ch = 0;
  
    if (__target >= __targetEnd) 
    {
      __OPENCODE_THROW(UnicodeException("Target buffer not enough"));
      break;
    }
    __utf32_ch = *__source++;
    
    if (__utf32_ch <= Unicode::MAX_BMP) 
    { 
      /* Target is a character <= 0xFFFF */
      /* UTF-16 surrogate values are illegal in UTF-32; 0xffff or 0xfffe are both reserved values */
      if ( (__utf32_ch >= Unicode::SUR_HIGH_START) && (__utf32_ch <= Unicode::SUR_LOW_END)) 
      {
        if(strict)
        {
          --__source; 
          __OPENCODE_THROW(UnicodeException("source sequence is illegal/malformed"));
          break; 
        }
        else
        {
          *__target++ = Unicode::REPLACEMENT_CHAR;
        }  
      } 
      else 
      {
        *__target++ = (UTF16)__utf32_ch; /* normal case */
      }
    } 
    else if (__utf32_ch > Unicode::MAX_LEGAL_UTF32) 
    {
      if (strict) 
      {
        __OPENCODE_THROW(UnicodeException("source sequence is illegal/malformed"));
        break;
      } 
      else 
      {
        *__target++ = Unicode::REPLACEMENT_CHAR;
      } 
    } 
    else 
    {
      /* __target is a character in range 0xFFFF - 0x10FFFF. */
      if ( (__target + 1) >= __targetEnd) 
      {
        --__source; 
        __OPENCODE_THROW(UnicodeException("Target buffer not enough"));  
        break;
      }
      
      __utf32_ch -= Unicode::HALFBASE;
      *__target++ = (UTF16)((__utf32_ch >> Unicode::HALFSHIFT) + Unicode::SUR_HIGH_START);
      *__target++ = (UTF16)((__utf32_ch & Unicode::HALFMASK) + Unicode::SUR_LOW_START);
    }
    
  }//~end while (__source < __sourceEnd) 
     
  return (int)(__target - target);
} 

int Unicode::ansi_to_utf32(const char* source, int sourceSize,UTF32* target,int targetSize) __OPENCODE_THROW_DECLARE(UnicodeException)
{
  int __size = 0;
  
#if defined(__WCHAR_EQUAL_TO_UTF32)
  wchar_t* __target = (wchar_t*)target;
  #if defined(PORT_WINDOWS)
    __size = MultiByteToWideChar(CP_ACP, 0, source,sourceSize, __target, targetSize);
  #else
    __size = (int)mbstowcs(__target,source,sourceSize); 
  #endif
#elif defined(__WCHAR_EQUAL_TO_UTF16)  
  int __utf16_alloc_size = sourceSize + 1;
  UTF16* __target_utf16 = new UTF16[__utf16_alloc_size];
  memset((char*)__target_utf16,0,sizeof(UTF16) * __utf16_alloc_size);

  #if defined(PORT_WINDOWS)
    int __size_utf16 = (int)MultiByteToWideChar(CP_ACP, 0, source,sourceSize, (wchar_t*)__target_utf16, __utf16_alloc_size);
  #else
    int __size_utf16 = (int)mbstowcs((wchar_t*)__target_utf16,source,sourceSize); 
  #endif
   
  __size = utf16_to_utf32(__target_utf16,__size_utf16,target,targetSize); 
  delete []__target_utf16;  
#endif

  return __size;  
}
  
int Unicode::utf32_to_ansi(const UTF32* source, int sourceSize,char* target,int targetSize) __OPENCODE_THROW_DECLARE(UnicodeException)
{
  int __size = 0;
  
#if defined(__WCHAR_EQUAL_TO_UTF32)
  wchar_t* __source = (wchar_t*)source;
 
  #if defined(PORT_WINDOWS)
    __size = WideCharToMultiByte(CP_ACP, 0,__source,sourceSize,target,targetSize,0,0);
  #else
    __size = (int)wcstombs(target,__source,targetSize);   
  #endif
#elif defined(__WCHAR_EQUAL_TO_UTF16) 
  int __utf16_alloc_size = sourceSize * 2 + 1;
  UTF16* __source_utf16 = new UTF16[__utf16_alloc_size];
  memset((char*)__source_utf16,0,sizeof(UTF16) * __utf16_alloc_size);  
  int __size_utf16  = utf32_to_utf16(source,sourceSize,__source_utf16,__utf16_alloc_size); 
  if(__size_utf16 > 0)
  {
    __source_utf16[__size_utf16] = 0;
  }

  #if defined(PORT_WINDOWS)
    __size = WideCharToMultiByte(CP_ACP, 0,(wchar_t*)__source_utf16,__utf16_alloc_size,target,targetSize,0,0);
  #else
    __size = (int)wcstombs(target,(wchar_t*)__source_utf16,targetSize);  
  #endif
  
  delete []__source_utf16;     
#endif

  return __size;
}

int Unicode::ansi_to_utf16(const  char* source, int sourceSize,UTF16* target,int targetSize) __OPENCODE_THROW_DECLARE(UnicodeException)
{
  int __size = 0;
    
#if defined(__WCHAR_EQUAL_TO_UTF16)
  wchar_t* __target = (wchar_t*)target;
  #if defined(PORT_WINDOWS)
    __size = MultiByteToWideChar(CP_ACP, 0, source,sourceSize, __target, targetSize);
  #else
    __size = (int)mbstowcs(__target,source,sourceSize); 
  #endif 
#elif defined(__WCHAR_EQUAL_TO_UTF32)
  int __utf32_alloc_size = sourceSize + 1;
  UTF32* __target_utf32 = new UTF32[__utf32_alloc_size];
  memset((char*)__target_utf32,0,sizeof(UTF32) * __utf32_alloc_size); 

  #if defined(PORT_WINDOWS)
    int __size_utf32 = (int)MultiByteToWideChar(CP_ACP, 0, source,sourceSize, (wchar_t*)__target_utf32, __utf32_alloc_size);
  #else
    int __size_utf32 = (int)mbstowcs((wchar_t*)__target_utf32,source,sourceSize); 
  #endif
  
  __size = utf32_to_utf16(__target_utf32,__size_utf32,target,targetSize); 
  delete []__target_utf32;    
#endif 

  return __size;
}

int Unicode::utf16_to_ansi(const UTF16* source, int sourceSize, char* target,int targetSize) __OPENCODE_THROW_DECLARE(UnicodeException)
{
  int __size = 0;
  
#if defined(__WCHAR_EQUAL_TO_UTF16)
  wchar_t* __source = (wchar_t*)source;
  #if defined(PORT_WINDOWS)
    __size = WideCharToMultiByte(CP_ACP, 0,__source,sourceSize,target,targetSize,0,0);
  #else
    __size = (int)wcstombs(target,__source,targetSize);   
  #endif
#elif defined(__WCHAR_EQUAL_TO_UTF32) 

  int __utf32_alloc_size = sourceSize + 1;
  UTF32* __source_utf32 = new UTF32[__utf32_alloc_size];
  memset(__source_utf32,0,sizeof(UTF32) * __utf32_alloc_size);  
  int __size_utf32  = utf16_to_utf32(source,sourceSize,__source_utf32,__utf32_alloc_size); 
  if(__size_utf32 > 0)
  {
    __source_utf32[__size_utf32] = 0;
  }

  #if defined(PORT_WINDOWS)
    __size = WideCharToMultiByte(CP_ACP, 0,(wchar_t*)__source_utf32,__utf32_alloc_size,target,targetSize,0,0);
  #else
    __size = (int)wcstombs(target,(wchar_t*)__source_utf32,targetSize);  
  #endif

  delete []__source_utf32;    
#endif

  return __size;  
}
  
int Unicode::ansi_to_utf8(const  char* source, int sourceSize,UTF8* target,int targetSize) __OPENCODE_THROW_DECLARE(UnicodeException)
{
  UTF32STRING* __s_utf32 = new UTF32STRING(sourceSize,'\0');
  smart_ptr<UTF32STRING> __sp_utf32(__s_utf32);
  
  int __ret32_size = ansi_to_utf32(source,sourceSize,(UTF32*)__s_utf32->data(),sourceSize);
  //printf("ansi_to_utf32 : %03d [%s]\n",__ret32_size,MemoryKit::numDump((const char*)__s_utf32->data(),__ret32_size*4).c_str()); 
  int __size = utf32_to_utf8(__s_utf32->data(),__ret32_size,target,targetSize);  
  
  return __size;
}

int Unicode::utf8_to_ansi(const UTF8* source, int sourceSize, char* target,int targetSize) __OPENCODE_THROW_DECLARE(UnicodeException)
{
//  printf("utf8_to_ansi : %03d [%s]\n",sourceSize,MemoryKit::numDump((const char*)source,sourceSize).c_str());
  UTF32STRING* __s_utf32 = new UTF32STRING(sourceSize,'\0');
  smart_ptr<UTF32STRING> __sp_utf32(__s_utf32);
  
  int __ret32_size = utf8_to_utf32(source,sourceSize,(UTF32*)__s_utf32->data(),sourceSize); 
//  printf("utf8_to_utf32 : %03d [%s]\n",__ret32_size,MemoryKit::numDump((const char*)__s_utf32->data(),__ret32_size*4).c_str());
  int __size = utf32_to_ansi(__s_utf32->data(),__ret32_size,target,targetSize); 
  
  return __size;
}

int Unicode::getBytesUTF8(UTF8 first)
{
  return __trailingBytesForUTF8[first] + 1;
}

/* ---------------------------------------------------------------------

    Note A.
    The fall-through switches in UTF-8 reading code save a
    temp variable, some decrements & conditionals.  The switches
    are equivalent to the following loop:
  {
      int __tmpBytesToRead = __extraBytesToRead+1;
      do 
      {
        __utf32_ch += *__source++;
        --__tmpBytesToRead;
        if (__tmpBytesToRead != 0) 
        {
          __utf32_ch <<= 6;
        }  
      } while (__tmpBytesToRead > 0);
  }
    In UTF-8 writing code, the switches on "__bytesToWrite" are
    similarly unrolled loops.

   --------------------------------------------------------------------- */

__OPENCODE_END_NAMESPACE

