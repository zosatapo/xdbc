/** 
 *  unicode.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
 
#ifndef __OPENCODE_UNICODE_H__
#define __OPENCODE_UNICODE_H__

#include "port/libdefs.h"
#include "port/ioc_types.h"

__OPENCODE_BEGIN_NAMESPACE

#ifndef UTF8
/* typically 8 bits */
#define  UTF8   UXINT8
  
#endif

#ifndef UTF16
/* typically 16 bits */
#define  UTF16  UXINT16
#endif

#ifndef UTF32
/* typically 32 bits */
#define  UTF32  UXINT32

#endif

typedef std::basic_string<UTF8>   UTF8STRING;
typedef std::basic_string<UTF16>  UTF16STRING;
typedef std::basic_string<UTF32>  UTF32STRING;
  
/**
 * UCS4 (UTF32) -> UCS Transformation Format 8 (UTF-8) converter
 * It's represented like below.
 *
 * # Bits   Bit pattern
 * 1    7   0xxxxxxx
 * 2   11   110xxxxx 10xxxxxx
 * 3   16   1110xxxx 10xxxxxx 10xxxxxx
 * 4   21   11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
 * 5   26   111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
 * 6   31   1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
 *
 *     UCS2 uses 1-3 / UTF16 uses 1-4 / UCS4 uses 1-6
 * 
 *   
 * a BOM can be used as a signature no matter how the Unicode text is transformed: UTF-16, UTF-8, UTF-7, etc. 
 * The exact bytes comprising the BOM will be whatever the Unicode character FEFF is converted into by that 
 * transformation format. In that form, the BOM serves to indicate both that it is a Unicode file, and 
 * which of the formats it is in. Examples:

 * Bytes          Encoding Form 
 * 00 00 FE FF    UTF-32, big-endian 
 * FF FE 00 00    UTF-32, little-endian 
 * FE FF          UTF-16, big-endian 
 * FF FE          UTF-16, little-endian 
 * EF BB BF       UTF-8  
 *  
 */

class __OPENCODE_CORE_API_DECL UnicodeException : public BaseException
{
public:
  ~UnicodeException();
  
  UnicodeException();
  UnicodeException(const string& message);
  UnicodeException(const string& message,XLONG errcode);
};

/**
 * UTF8 string format check.
 * 
 * @return  If the data contains a valid UTF8 string, it returns true.
 *          Otherwise, it throws MalformedInputException.
 */
class __OPENCODE_CORE_API_DECL UTF8Checker
{
public:
  /** When nullCheck is true, the check will fail if a null octet is  found in the data.*/
  static bool check(const UTF8* source, int sourceSize, bool nullCheck = false) __OPENCODE_THROW_DECLARE(UnicodeException);
};

class __OPENCODE_CORE_API_DECL Unicode
{
public:
  static const UTF32 MAX_BMP;
  static const UTF32 MAX_UTF16;
  static const UTF32 MAX_UTF32;
  static const UTF32 MAX_LEGAL_UTF32;

  static const UTF32 REPLACEMENT_CHAR;

  static const UTF32 SUR_HIGH_START;
  static const UTF32 SUR_HIGH_END;
  static const UTF32 SUR_LOW_START;
  static const UTF32 SUR_LOW_END;

  static const UTF32 HALFSHIFT; /* used for shifting by 10 bits */
  static const UTF32 HALFBASE;
  static const UTF32 HALFMASK;
  
////////////////////////////////////////////////////////////////////////////////////

public:
  /** 
   * These conversion functions take a ConversionFlags =strict= argument. When this
   * flag is set to true, both irregular sequences and isolated surrogates
   * will cause an error.  When the flag is set to false, both irregular
   * sequences and isolated surrogates are converted.
   *   
   * Whether the flag is true or false, all illegal sequences will cause
   * an error return. This includes sequences such as: <F4 90 80 80>, <C0 80>,
   * or <A0> in UTF-8, and values above 0x10FFFF in UTF-32. Conformant code
   * must check for illegal sequences.
   *   
   * When the flag is set to false, characters over 0x10FFFF are converted
   * to the replacement character; otherwise (when the flag is set to true)
   * they constitute an error.
   *
   * @param  source        pointer to the source buffer.
   * @param  sourceSize    code units number  to the source buffer.
   * @param  target        pointer to  the target buffer.
   * @param  targetSize    code units number to the target buffer.
   * @param  strict        ConversionFlags (see above)
   *
   */
  static int utf8_to_utf16 (const UTF8* source,int sourceSize, UTF16* target, int targetSize,bool strict = false) __OPENCODE_THROW_DECLARE(UnicodeException);
  static int utf16_to_utf8 (const UTF16* source,int sourceSize, UTF8* target, int targetSize,bool strict = false) __OPENCODE_THROW_DECLARE(UnicodeException);
  
  static int utf8_to_utf32 (const UTF8* source,int sourceSize, UTF32* target, int targetSize,bool strict = false) __OPENCODE_THROW_DECLARE(UnicodeException);
  static int utf32_to_utf8 (const UTF32* source,int sourceSize, UTF8* target, int targetSize,bool strict = false) __OPENCODE_THROW_DECLARE(UnicodeException);
  
  static int utf16_to_utf32 (const UTF16* source,int sourceSize, UTF32* target, int targetSize,bool strict = false) __OPENCODE_THROW_DECLARE(UnicodeException);
  static int utf32_to_utf16 (const UTF32* source,int sourceSize, UTF16* target, int targetSize,bool strict = false) __OPENCODE_THROW_DECLARE(UnicodeException);

////////////////////////////////////////////////////////////////////////////////////

public:
  static int ansi_to_utf32(const  char* source, int sourceSize,UTF32* target,int targetSize) __OPENCODE_THROW_DECLARE(UnicodeException);
  static int utf32_to_ansi(const UTF32* source, int sourceSize, char* target,int targetSize) __OPENCODE_THROW_DECLARE(UnicodeException);

  static int ansi_to_utf16(const  char* source, int sourceSize,UTF16* target,int targetSize) __OPENCODE_THROW_DECLARE(UnicodeException);
  static int utf16_to_ansi(const UTF16* source, int sourceSize, char* target,int targetSize) __OPENCODE_THROW_DECLARE(UnicodeException);

  static int ansi_to_utf8(const  char* source, int sourceSize,UTF8* target,int targetSize) __OPENCODE_THROW_DECLARE(UnicodeException);
  static int utf8_to_ansi(const UTF8* source, int sourceSize, char* target,int targetSize) __OPENCODE_THROW_DECLARE(UnicodeException);

////////////////////////////////////////////////////////////////////////////////////
    
public:
  static int getBytesUTF8(UTF8 first);
};  
__OPENCODE_END_NAMESPACE


#endif
