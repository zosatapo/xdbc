/** 
 *  libnumber.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef __OPENCODE_NUMBER_H__
#define __OPENCODE_NUMBER_H__

#include "port/libdefs.h"

__OPENCODE_BEGIN_NAMESPACE

/**
 * Thrown when an exceptional arithmetic condition has occurred. For 
 * example, an integer "divide by zero" throws an 
 * instance of this class. 
 *
 */

class __OPENCODE_CORE_API_DECL ArithmeticException : public BaseException
{
public:
  ~ArithmeticException();
  
  /**
   * Constructs an <code>ArithmeticException</code> with no detail 
   * message. 
   */
  ArithmeticException();
  
  /**
   * Constructs an <code>ArithmeticException</code> with the specified 
   * detail message. 
   *
   * @param   message   the detail message.
   */
  ArithmeticException(const string& message);
};


class __OPENCODE_CORE_API_DECL NumberFormatException : public BaseException
{
public:
  ~NumberFormatException();
  
  NumberFormatException();
  NumberFormatException(const string& message);

public:
  /**
   * Factory method for making a <code>NumberFormatException</code>
   * given the specified input which caused the error.
   *
   * @param   s   the input causing the error
   */
  static NumberFormatException forInputString(const string& s); 
};


class __OPENCODE_CORE_API_DECL Number
{
//-------------------------------------------------------------------------
//ctor
//-------------------------------------------------------------------------
public:
  virtual ~Number() ;

protected:
  Number();  
  
//-------------------------------------------------------------------------
// interface
//-------------------------------------------------------------------------
public:
  /**
   * Returns the value of the specified number as an <code>int</code>.
   * This may involve rounding or truncation.
   *
   * @return  the numeric value represented by this object after conversion
   *          to type <code>int</code>.
   */
  virtual int intValue() const = 0;

  /**
   * Returns the value of the specified number as an <code>XLONG</code>.
   * This may involve rounding or truncation.
   *
   * @return  the numeric value represented by this object after conversion
   *          to type <code>XLONG</code>.
   */
  virtual XLONG longValue() const = 0;  

  /**
   * Returns the value of the specified number as a <code>float</code>.
   * This may involve rounding.
   *
   * @return  the numeric value represented by this object after conversion
   *          to type <code>float</code>.
   */
  virtual  float floatValue() const = 0;
  
  /**
   * Returns the value of the specified number as a <code>double</code>.
   * This may involve rounding.
   *
   * @return  the numeric value represented by this object after conversion
   *          to type <code>double</code>.
   */
  virtual double doubleValue() const = 0;
  
  /**
   * Returns the value of the specified number as a <code>short</code>.
   * This may involve rounding or truncation.
   *
   * @return  the numeric value represented by this object after conversion
   *          to type <code>short</code>.
   */
  virtual short shortValue() const;
   
  /**
   * Returns the decimal String representation of this Number.
   *
   */
  virtual string toString() const = 0;
    
//-------------------------------------------------------------------------
// static block : parseXXX() 
//-------------------------------------------------------------------------
public:
  /**
   * Parses the string argument as a signed integer in the radix 
   * specified by the second argument. The characters in the string 
   * must all be digits of the specified radix (as determined by 
   * whether {@link base.Character#digit(char, int)} returns a 
   * nonnegative value), except that the first character may be an 
   * ASCII minus sign <code>'-'</code> (<code>'&#92;u002D'</code>) to 
   * indicate a negative value. The resulting integer value is returned. 
   * <p>
   * An exception of type <code>NumberFormatException</code> is
   * thrown if any of the following situations occurs:
   * <ul>
   * <li>The first argument is <code>null</code> or is a string of
   * length zero.
   * <li>The <code>radix</code> is either smaller than {@link  MIN_RADIX} or larger than {@link  MAX_RADIX}.
   * <li>Any character of the string is not a digit of the specified
   * radix, except that the first character may be a minus sign
   * <code>'-'</code> (<code>'&#92;u002D'</code>) provided that the
   * string is longer than length 1.
   * <li>The value represented by the string is not a value of type
   * <code>int</code>. 
   * </ul><p>
   * Examples:
   * <blockquote><pre>
   * parseInt("0", 10) returns 0
   * parseInt("473", 10) returns 473
   * parseInt("-0", 10) returns 0
   * parseInt("-FF", 16) returns -255
   * parseInt("1100110", 2) returns 102
   * parseInt("2147483647", 10) returns 2147483647
   * parseInt("-2147483648", 10) returns -2147483648
   * parseInt("2147483648", 10) throws a NumberFormatException
   * parseInt("99", 8) throws a NumberFormatException
   * parseInt("Kona", 10) throws a NumberFormatException
   * parseInt("Kona", 27) returns 411787
   * </pre></blockquote>
   *
   * @param      s   the <code>string</code> containing the integer  representation to be parsed
   * @param      radix   the radix to be used while parsing <code>s</code>.
   * @return     the integer represented by the string argument in the  specified radix.
   * @exception  NumberFormatException if the <code>String</code>
   *       does not contain a parsable <code>int</code>.
   */
  static int __OPENCODE_NATIVE_CODE parseInt(const string& s, int radix = 10) __OPENCODE_THROW_DECLARE(NumberFormatException); 
  static int __OPENCODE_NATIVE_CODE parseInt(const char* s,int radix = 10) __OPENCODE_THROW_DECLARE(NumberFormatException);
  
  /**
   * Parses the string argument as a signed <code>XLONG</code> in the
   * radix specified by the second argument. The characters in the
   * string must all be digits of the specified radix (as determined
   * by whether  returns a nonnegative value), except that the first character may be an
   * ASCII minus sign <code>'-'</code>  to indicate a negative value. 
   * The resulting <code>XLONG</code> value is returned.
   * <p>
   * Note that neither the character <code>L</code>
   * (<code>'&#92;u004C'</code>) nor <code>l</code>
   * (<code>'&#92;u006C'</code>) is permitted to appear at the end
   * of the string as a type indicator, as would be permitted in
   * C++ programming baseuage source code - except that either
   * <code>L</code> or <code>l</code> may appear as a digit for a
   * radix greater than 22.
   * <p>
   * An exception of type <code>NumberFormatException</code> is
   * thrown if any of the following situations occurs:
   * <ul>
   * <li>The  string of length zero.
   * <li>The <code>radix</code> is either smaller than {@link  MIN_RADIX} or larger than {@link  MAX_RADIX}.
   * <li>Any character of the string is not a digit of the specified
   * radix, except that the first character may be a minus sign
   * <code>'-'</code> (<code>'&#92;u002d'</code>) provided that the
   * string is longer than length 1.
   * <li>The value represented by the string is not a value of type
   *      <code>XLONG</code>. 
   * </ul><p>
   * Examples:
   * <blockquote><pre>
   * parseLong("0", 10) returns 0L
   * parseLong("473", 10) returns 473L
   * parseLong("-0", 10) returns 0L
   * parseLong("-FF", 16) returns -255L
   * parseLong("1100110", 2) returns 102L
   * parseLong("99", 8) throws a NumberFormatException
   * parseLong("Hazelnut", 10) throws a NumberFormatException
   * parseLong("Hazelnut", 36) returns 1356099454469L
   * </pre></blockquote>
   * 
   * @param      s       the <code>String</code> containing the
   *                     <code>XLONG</code> representation to be parsed.
   * @param      radix   the radix to be used while parsing <code>s</code>.
   * @return     the <code>XLONG</code> represented by the string argument in
   *             the specified radix.
   * @exception  NumberFormatException  if the string does not contain a
   *               parsable <code>XLONG</code>.
   */
  static XLONG __OPENCODE_NATIVE_CODE parseLong(const string& s, int radix = 10) __OPENCODE_THROW_DECLARE(NumberFormatException);  
  static XLONG __OPENCODE_NATIVE_CODE parseLong(const char* s, int radix = 10) __OPENCODE_THROW_DECLARE(NumberFormatException);
    
  /**
   * Returns a new <code>float</code> initialized to the value
   * represented by the specified <code>String</code>, as performed
   * by the <code>valueOf</code> method of class <code>Float</code>.
   *
   * @param      s   the string to be parsed.
   * @return the <code>float</code> value represented by the string
   *         argument.
   * @exception  NumberFormatException  if the string does not contain a
   *               parsable <code>float</code>.
   */
  static float __OPENCODE_NATIVE_CODE parseFloat(const string& s)  __OPENCODE_THROW_DECLARE(NumberFormatException);
  static float __OPENCODE_NATIVE_CODE parseFloat(const char* s)  __OPENCODE_THROW_DECLARE(NumberFormatException);
  
  /**
   * Returns a new <code>double</code> initialized to the value
   * represented by the specified <code>String</code>, as performed
   * by the <code>valueOf</code> method of class
   * <code>Double</code>.
   *
   * @param      s   the string to be parsed.
   * @return the <code>double</code> value represented by the string
   *         argument.
   * @exception NumberFormatException if the string does not contain
   *            a parsable <code>double</code>.
   */
  static double __OPENCODE_NATIVE_CODE parseDouble(const string& s)  __OPENCODE_THROW_DECLARE(NumberFormatException);  
  static double __OPENCODE_NATIVE_CODE parseDouble(const char* s)  __OPENCODE_THROW_DECLARE(NumberFormatException); 
  
//-------------------------------------------------------------------------
//static block : toString
//-------------------------------------------------------------------------

  /**
   * Returns a string representation of the first argument in the
   * radix specified by the second argument.
   * <p>
   * If the radix is smaller than <code>MIN_RADIX</code>
   * or larger than <code>MAX_RADIX</code>, then the radix
   * <code>10</code> is used instead.
   * <p>
   * If the first argument is negative, the first element of the
   * result is the ASCII minus character <code>'-'</code>
   * (<code>'&#92;u002D'</code>). If the first argument is not
   * negative, no sign character appears in the result.
   * <p>
   * The remaining characters of the result represent the magnitude
   * of the first argument. If the magnitude is zero, it is
   * represented by a single zero character <code>'0'</code>
   * (<code>'&#92;u0030'</code>); otherwise, the first character of
   * the representation of the magnitude will not be the zero
   * character.  The following ASCII characters are used as digits: 
   * <blockquote><pre>
   *   0123456789abcdefghijklmnopqrstuvwxyz
   * </pre></blockquote>
   * These are <code>'&#92;u0030'</code> through
   * <code>'&#92;u0039'</code> and <code>'&#92;u0061'</code> through
   * <code>'&#92;u007A'</code>. If <code>radix</code> is
   * <var>N</var>, then the first <var>N</var> of these characters
   * are used as radix-<var>N</var> digits in the order shown. Thus,
   * the digits for hexadecimal (radix 16) are
   * <code>0123456789abcdef</code>. 
   *
   * @param   i       an integer to be converted to a string.
   * @param   radix   the radix to use in the string representation.
   * @return  a string representation of the argument in the specified radix.
   */
  static string __OPENCODE_NATIVE_CODE toString(int i,int radix = 10);
  static string __OPENCODE_NATIVE_CODE toString(unsigned int i,int radix = 10);

  static void __OPENCODE_NATIVE_CODE toString(char* buffer,int i,int radix = 10);
  static void __OPENCODE_NATIVE_CODE toString(char* buffer,unsigned int i,int radix = 10);
  
  /**
   * Returns a string representation of the first argument in the
   * radix specified by the second argument.
   * <p>
   * If the radix is smaller than <code>MIN_RADIX</code>
   * or larger than <code>MAX_RADIX</code>, then the radix
   * <code>10</code> is used instead.
   * <p>
   * If the first argument is negative, the first element of the
   * result is the ASCII minus sign <code>'-'</code>
   * (<code>'&#92;u002d'</code>). If the first argument is not
   * negative, no sign character appears in the result.
   * <p>
   * The remaining characters of the result represent the magnitude
   * of the first argument. If the magnitude is zero, it is
   * represented by a single zero character <code>'0'</code>
   * (<code>'&#92;u0030'</code>); otherwise, the first character of
   * the representation of the magnitude will not be the zero
   * character.  The following ASCII characters are used as digits:
   * <blockquote><pre>
   *   0123456789abcdefghijklmnopqrstuvwxyz
   * </pre></blockquote>
   * These are <code>'&#92;u0030'</code> through
   * <code>'&#92;u0039'</code> and <code>'&#92;u0061'</code> through
   * <code>'&#92;u007a'</code>. If <code>radix</code> is
   * <var>N</var>, then the first <var>N</var> of these characters
   * are used as radix-<var>N</var> digits in the order shown. Thus,
   * the digits for hexadecimal (radix 16) are
   * <code>0123456789abcdef</code>.
   *
   * @param   i       an integer to be converted to a string.
   * @param   radix   the radix to use in the string representation.
   * @return  a string representation of the argument in the specified radix.
   * 
   */
  static string __OPENCODE_NATIVE_CODE toString(long i,int radix = 10) ;
  static string __OPENCODE_NATIVE_CODE toString(unsigned long i,int radix = 10) ;

  static void __OPENCODE_NATIVE_CODE toString(char* buffer,long i,int radix = 10) ;
  static void __OPENCODE_NATIVE_CODE toString(char* buffer,unsigned long i,int radix = 10) ;
  
  static string __OPENCODE_NATIVE_CODE toString(long long i,int radix = 10) ;
  static string __OPENCODE_NATIVE_CODE toString(unsigned long long i,int radix = 10) ;

  static void __OPENCODE_NATIVE_CODE toString(char* buffer,long long i,int radix = 10) ;
  static void __OPENCODE_NATIVE_CODE toString(char* buffer,unsigned long long i,int radix = 10) ;
    
  /**
   * Returns a string representation of the <code>float</code>
   * argument. 
   *
   * @param   f   the float to be converted.
   * @return a string representation of the argument.
   */
  static string __OPENCODE_NATIVE_CODE toString(float f,const char* fmt = "g");
  static void __OPENCODE_NATIVE_CODE toString(char* buffer,float f,const char* fmt = "g");
  
  /**
   * Returns a string representation of the <code>double</code> 
   * argument. 
   *
   * @param   d   the <code>double</code> to be converted.
   * @return a string representation of the argument.
   */
  static string __OPENCODE_NATIVE_CODE toString(double d,const char* fmt = "g");
  static void  __OPENCODE_NATIVE_CODE toString(char* buffer,double d,const char* fmt = "g");
  
//-------------------------------------------------------------------------
//static block: ushiftRight
//-------------------------------------------------------------------------
public:
  /**
   * The value of ushiftRight(n,s) is n right-shifted s bit positions with zero-extension. If n
   * is positive, then the result is the same as that of n>>s; if n is negative, the result is
   * equal to that of the expression (n>>s)+(2<<~s) if the type of the left-hand operand
   * is int, and to the result of the expression (n>>s)+(2L<<~s) if the type of the
   * left-hand operand is XLONG. The added term (2<<~s) or (2L<<~s) cancels out the
   * propagated sign bit. 
   */
  static int  ushiftRight(int n,int s );
  static unsigned int  ushiftRight(unsigned int n,int s );

  static XLONG  ushiftRight(XLONG n,int s);
  static UXLONG ushiftRight(UXLONG n,int s);

public:
  static XLONG  strtoLong(const string& str, int radix)  __OPENCODE_THROW_DECLARE(NumberFormatException);     
  static XLONG  strtoLong(const char* str, int radix)  __OPENCODE_THROW_DECLARE(NumberFormatException);  

  static float   strtoFloat(const string& str)  __OPENCODE_THROW_DECLARE(NumberFormatException);
  static float   strtoFloat(const char* str)  __OPENCODE_THROW_DECLARE(NumberFormatException);


//-------------------------------------------------------------------------
//static block: IEEE 754 floating-point   Support
//-------------------------------------------------------------------------
public:
  /**
   * Returns a representation of the specified floating-point value
   * according to the IEEE 754 floating-point "single format" bit
   * layout, preserving Not-a-Number (NaN) values.
   * <p>
   * Bit 31 (the bit that is selected by the mask 
   * <code>0x80000000</code>) represents the sign of the floating-point 
   * number. 
   * Bits 30-23 (the bits that are selected by the mask 
   * <code>0x7f800000</code>) represent the exponent. 
   * Bits 22-0 (the bits that are selected by the mask 
   * <code>0x007fffff</code>) represent the significand (sometimes called 
   * the mantissa) of the floating-point number. 
   * <p>If the argument is positive infinity, the result is 
   * <code>0x7f800000</code>. 
   * <p>If the argument is negative infinity, the result is 
   * <code>0xff800000</code>.
   * <p>
   * If the argument is NaN, the result is the integer representing
   * the actual NaN value.  Unlike the <code>floatToIntBits</code>
   * method, <code>intToRawIntBits</code> does not collapse all the
   * bit patterns encoding a NaN to a single &quot;canonical&quot;
   * NaN value.
   * <p>
   * In all cases, the result is an integer that, when given to the
   * {@link #intBitsToFloat(int)} method, will produce a
   * floating-point value the same as the argument to
   * <code>floatToRawIntBits</code>.
   * @param   value   a floating-point number.
   * @return the bits that represent the floating-point number.
   */
  static int   floatToRawIntBits(float value);

  
  /**
   * Returns the <code>float</code> value corresponding to a given
   * bit represention.
   * The argument is considered to be a representation of a
   * floating-point value according to the IEEE 754 floating-point
   * "single format" bit layout.
   * <p>
   * If the argument is <code>0x7f800000</code>, the result is positive
   * infinity.
   * <p>
   * If the argument is <code>0xff800000</code>, the result is negative
   * infinity.
   * <p>
   * If the argument is any value in the range
   * <code>0x7f800001</code> through <code>0x7fffffff</code> or in
   * the range <code>0xff800001</code> through
   * <code>0xffffffff</code>, the result is a NaN.  No IEEE 754
   * floating-point operation provided can distinguish
   * between two NaN values of the same type with different bit
   * patterns.  Distinct values of NaN are only distinguishable by
   * use of the <code>Float.floatToRawIntBits</code> method.
   * <p>
   * In all rv cases, let <i>s</i>, <i>e</i>, and <i>m</i> be three 
   * values that can be computed from the argument: 
   * <blockquote><pre>
   * int s = ((bits &gt;&gt; 31) == 0) ? 1 : -1;
   * int e = ((bits &gt;&gt; 23) & 0xff);
   * int m = (e == 0) ?
   *                 (bits & 0x7fffff) &lt;&lt; 1 :
   *                 (bits & 0x7fffff) | 0x800000;
   * </pre></blockquote>
   * Then the floating-point result equals the value of the mathematical 
   * expression <i>s</i>&middot;<i>m</i>&middot;2<sup><i>e</i>-150</sup>.
   *<p>
   * Note that this method may not be able to return a
   * <code>float</code> NaN with exactly same bit pattern as the
   * <code>int</code> argument.  IEEE 754 distinguishes between two
   * kinds of NaNs, quiet NaNs and <i>signaling NaNs</i>.  
   * Arithmetic operations on signaling NaNs turn
   * them into quiet NaNs with a different, but often similar, bit
   * pattern.  However, on some processors merely copying a
   * signaling NaN also performs that conversion.  In particular,
   * copying a signaling NaN to return it to the calling method may
   * perform this conversion.  So <code>intBitsToFloat</code> may
   * not be able to return a <code>float</code> with a signaling NaN
   * bit pattern.  Consequently, for some <code>int</code> values,
   * <code>floatToRawIntBits(intBitsToFloat(start))</code> may
   * <i>not</i> equal <code>start</code>.  Moreover, which
   * particular bit patterns represent signaling NaNs is platform
   * dependent; although all NaN bit patterns, quiet or signaling,
   * must be in the NaN range identified above.
   *
   * @param   bits   an integer.
   * @return  the <code>float</code> floating-point value with the same bit
   *          pattern.
   */
  static float intBitsToFloat(int bits); 

  /**
   * Returns a representation of the specified floating-point value
   * according to the IEEE 754 floating-point "double
   * format" bit layout, preserving Not-a-Number (NaN) values.
   * <p>
   * Bit 63 (the bit that is selected by the mask 
   * <code>0x8000000000000000L</code>) represents the sign of the 
   * floating-point number. Bits 
   * 62-52 (the bits that are selected by the mask 
   * <code>0x7ff0000000000000L</code>) represent the exponent. Bits 51-0 
   * (the bits that are selected by the mask 
   * <code>0x000fffffffffffffL</code>) represent the significand 
   * (sometimes called the mantissa) of the floating-point number. 
   * <p>
   * If the argument is positive infinity, the result is
   * <code>0x7ff0000000000000L</code>.
   * <p>
   * If the argument is negative infinity, the result is
   * <code>0xfff0000000000000L</code>.
   * <p>
   * If the argument is NaN, the result is the <code>long</code>
   * integer representing the actual NaN value.  Unlike the
   * <code>doubleToLongBits</code> method,
   * <code>doubleToRawLongBits</code> does not collapse all the bit
   * patterns encoding a NaN to a single &quot;canonical&quot; NaN
   * value.
   * <p>
   * In all cases, the result is a <code>long</code> integer that,
   * when given to the {@link #longBitsToDouble(long)} method, will
   * produce a floating-point value the same as the argument to
   * <code>doubleToRawLongBits</code>.
   *
   * @param   value   a <code>double</code> precision floating-point number.
   * @return the bits that represent the floating-point number.
   */
   static XLONG doubleToRawLongBits(double value);

  /**
   * Returns the <code>double</code> value corresponding to a given
   * bit representation.
   * The argument is considered to be a representation of a
   * floating-point value according to the IEEE 754 floating-point
   * "double format" bit layout.
   * <p>
   * If the argument is <code>0x7ff0000000000000L</code>, the result 
   * is positive infinity. 
   * <p>
   * If the argument is <code>0xfff0000000000000L</code>, the result 
   * is negative infinity. 
   * <p>
   * If the argument is any value in the range
   * <code>0x7ff0000000000001L</code> through
   * <code>0x7fffffffffffffffL</code> or in the range
   * <code>0xfff0000000000001L</code> through
   * <code>0xffffffffffffffffL</code>, the result is a NaN.  No IEEE
   * 754 floating-point operation provided by Java can distinguish
   * between two NaN values of the same type with different bit
   * patterns.  Distinct values of NaN are only distinguishable by
   * use of the <code>Double.doubleToRawLongBits</code> method.
   * <p>
   * In all rv cases, let <i>s</i>, <i>e</i>, and <i>m</i> be three 
   * values that can be computed from the argument: 
   * <blockquote><pre>
   * int s = ((bits &gt;&gt; 63) == 0) ? 1 : -1;
   * int e = (int)((bits &gt;&gt; 52) & 0x7ffL);
   * long m = (e == 0) ?
   *                 (bits & 0xfffffffffffffL) &lt;&lt; 1 :
   *                 (bits & 0xfffffffffffffL) | 0x10000000000000L;
   * </pre></blockquote>
   * Then the floating-point result equals the value of the mathematical 
   * expression <i>s</i>&middot;<i>m</i>&middot;2<sup><i>e</i>-1075</sup>.
   *<p>
   * Note that this method may not be able to return a
   * <code>double</code> NaN with exactly same bit pattern as the
   * <code>long</code> argument.  IEEE 754 distinguishes between two
   * kinds of NaNs, quiet NaNs and <i>signaling NaNs</i>. 
   * Arithmetic operations on signaling NaNs turn
   * them into quiet NaNs with a different, but often similar, bit
   * pattern.  However, on some processors merely copying a
   * signaling NaN also performs that conversion.  In particular,
   * copying a signaling NaN to return it to the calling method
   * may perform this conversion.  So <code>longBitsToDouble</code>
   * may not be able to return a <code>double</code> with a
   * signaling NaN bit pattern.  Consequently, for some
   * <code>long</code> values,
   * <code>doubleToRawLongBits(longBitsToDouble(start))</code> may
   * <i>not</i> equal <code>start</code>.  Moreover, which
   * particular bit patterns represent signaling NaNs is platform
   * dependent; although all NaN bit patterns, quiet or signaling,
   * must be in the NaN range identified above.
   *
   * @param   bits   any <code>long</code> integer.
   * @return  the <code>double</code> floating-point value with the same
   *          bit pattern.
   */
  static  double longBitsToDouble(XLONG bits);

  /**
   * Returns <code>true</code> if the specified number is a
   * Not-a-Number (NaN) value, <code>false</code> otherwise.
   *
   * @param   v   the value to be tested.
   * @return  <code>true</code> if the argument is NaN;
   *          <code>false</code> otherwise.
   */
  static bool isNaN(float v);

  /**
   * Returns <code>true</code> if the specified number is infinitely
   * large in magnitude, <code>false</code> otherwise.
   *
   * @param   v   the value to be tested.
   * @return  <code>true</code> if the argument is positive infinity or
   *          negative infinity; <code>false</code> otherwise.
   */
  static bool isInfinite(float v);

  /**
   * Returns <code>true</code> if the specified number is a
   * Not-a-Number (NaN) value, <code>false</code> otherwise.
   *
   * @param   v   the value to be tested.
   * @return  <code>true</code> if the value of the argument is NaN;
   *          <code>false</code> otherwise.
   */
  static bool isNaN(double v) ;
  
  /**
   * Returns <code>true</code> if the specified number is infinitely
   * large in magnitude, <code>false</code> otherwise.
   *
   * @param   v   the value to be tested.
   * @return  <code>true</code> if the value of the argument is positive
   *          infinity or negative infinity; <code>false</code> otherwise.
   */
  static  bool isInfinite(double v);
                 
//-------------------------------------------------------------------------
// RADIX Limit
//-------------------------------------------------------------------------
public:
  /**
   * The minimum radix available for conversion to and from strings.
   *
   */
  const static  int MIN_RADIX;

  /**
   * The maximum radix available for conversion to and from strings.
   *
   */
  const static  int MAX_RADIX;

//-------------------------------------------------------------------------------------------
//Rounding Modes
//-------------------------------------------------------------------------------------------
public:
  /**
   * Rounding mode to round away from zero.  Always increments the
   * digit prior to a non-zero discarded fraction.  Note that this rounding
   * mode never decreases the magnitude of the calculated value.
   */
  const static int ROUND_UP;

  /**
   * Rounding mode to round towards zero.  Never increments the digit
   * prior to a discarded fraction (i.e., truncates).  Note that this
   * rounding mode never increases the magnitude of the calculated value.
   */
  const static int ROUND_DOWN;

  /**
   * Rounding mode to round towards positive infinity.  If the
   * number is positive, behaves as for <tt>ROUND_UP</tt>; if negative,
   * behaves as for <tt>ROUND_DOWN</tt>.  Note that this rounding mode never
   * decreases the calculated value.
   */
  const static int ROUND_CEILING;

  /**
   * Rounding mode to round towards negative infinity.  If the
   * number is positive, behave as for <tt>ROUND_DOWN</tt>; if negative,
   * behave as for <tt>ROUND_UP</tt>.  Note that this rounding mode never
   * increases the calculated value.
   */
  const static int ROUND_FLOOR;

  /**
   * Rounding mode to round towards "nearest neighbor" unless both
   * neighbors are equidistant, in which case round up.
   * Behaves as for <tt>ROUND_UP</tt> if the discarded fraction is &gt;= .5;
   * otherwise, behaves as for <tt>ROUND_DOWN</tt>.  Note that this is the
   * rounding mode that most of us were taught in grade school.
   */
  const static int ROUND_HALF_UP;

  /**
   * Rounding mode to round towards "nearest neighbor" unless both
   * neighbors are equidistant, in which case round down.
   * Behaves as for <tt>ROUND_UP</tt> if the discarded fraction is &gt; .5;
   * otherwise, behaves as for <tt>ROUND_DOWN</tt>.
   */
  const static int ROUND_HALF_DOWN;

  /**
   * Rounding mode to round towards the "nearest neighbor" unless both
   * neighbors are equidistant, in which case, round towards the even
   * neighbor.  Behaves as for ROUND_HALF_UP if the digit to the left of the
   * discarded fraction is odd; behaves as for ROUND_HALF_DOWN if it's even.
   * Note that this is the rounding mode that minimizes cumulative error
   * when applied repeatedly over a sequence of calculations.
   */
  const static int ROUND_HALF_EVEN;

//-------------------------------------------------------------------------------------------
// Other Constants
//-------------------------------------------------------------------------------------------
public:
  /**
   * This mask is used to obtain the value of an int as if it were unsigned.
   */
  const static int BITS_PER_BYTE;

  /**
   * This mask is used to obtain the value of an int as if it were unsigned.
   */
  const static UXLONG  ULONG_MASK;

  /**
   * All possible chars for representing a number as a String
   */
  const static char digits[];

  const static char digitTens[];
  const static char digitOnes[];  
};

__OPENCODE_END_NAMESPACE

#endif
