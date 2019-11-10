/**
 *  libmath.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */

#ifndef  __OPENCODE_MATH_H__
#define  __OPENCODE_MATH_H__

#include "port/libdefs.h"
#include "port/libnumber.h"

__OPENCODE_BEGIN_NAMESPACE

/**
 * The class <code>Math</code> contains methods for performing basic
 * numeric operations such as the elementary exponential, logarithm,
 * square root, and trigonometric functions.
 * <p>
 * Unlike some of the numeric methods of class
 * <code>StrictMath</code>, all implementations of the equivalent
 * functions of class <code>Math</code> are not defined to return the
 * bit-for-bit same results.  This relaxation permits
 * better-performing implementations where strict reproducibility is
 * not required.
 * <p>
 * By default many of the <code>Math</code> methods simply call
 * the equivalent method in <code>StrictMath</code> for their
 * implementation.  Code generators are encouraged to use
 * platform-specific native libraries or microprocessor instructions,
 * where available, to provide higher-performance implementations of
 * <code>Math</code> methods.  Such higher-performance
 * implementations still must conform to the specification for
 * <code>Math</code>.
 * <p>
 * The quality of implementation specifications concern two
 * properties, accuracy of the returned result and monotonicity of the
 * method.  Accuracy of the floating-point <code>Math</code> methods
 * is measured in terms of <i>ulps</i>, units in the last place.  For
 * a given floating-point format, an ulp of a specific real number
 * value is the difference between the two floating-point values
 * closest to that numerical value.  When discussing the accuracy of a
 * method as a whole rather than at a specific argument, the number of
 * ulps cited is for the worst-case error at any argument.  If a
 * method always has an error less than 0.5 ulps, the method always
 * returns the floating-point number nearest the exact result; such a
 * method is <i>correctly rounded</i>.  A correctly rounded method is
 * generally the best a floating-point approximation can be; however,
 * it is impractical for many floating-point methods to be correctly
 * rounded.  Instead, for the <code>Math</code> class, a larger error
 * bound of 1 or 2 ulps is allowed for certain methods.  Informally,
 * with a 1 ulp error bound, when the exact result is a representable
 * number the exact result should be returned; otherwise, either of
 * the two floating-point numbers closest to the exact result may be
 * returned.  Besides accuracy at individual arguments, maintaining
 * proper relations between the method at different arguments is also
 * important.  Therefore, methods with more than 0.5 ulp errors are
 * required to be <i>semi-monotonic</i>: whenever the mathematical
 * function is non-decreasing, so is the floating-point approximation,
 * likewise, whenever the mathematical function is non-increasing, so
 * is the floating-point approximation.  Not all approximations that
 * have 1 ulp accuracy will automatically meet the monotonicity
 * requirements.
 */

class __OPENCODE_CORE_API_DECL Math 
{
//-------------------------------------------------------------------------------------------
//Constructor
//-------------------------------------------------------------------------------------------
private: 
 /**
   * Don't let anyone instantiate this class.
   */
  Math();

//-------------------------------------------------------------------------------------------
//Static methods
//-------------------------------------------------------------------------------------------  
public:
  /**
   * Returns the trigonometric sine of an angle.  
   * <p>
   * A result must be within 1 ulp of the correctly rounded result.  Results
   * must be semi-monotonic.
   *
   * @param   a   an angle, in radians.
   * @return  the sine of the argument.
   */
  static double __OPENCODE_NATIVE_CODE sin(double a) ;
    
  /**
   * Returns the trigonometric cosine of an angle. 
   * <p>
   * A result must be within 1 ulp of the correctly rounded result.  Results
   * must be semi-monotonic.
   *
   * @param   a   an angle, in radians.
   * @return  the cosine of the argument.
   */
  static double __OPENCODE_NATIVE_CODE cos(double a) ;
   
  /**
   * Returns the trigonometric tangent of an angle. 
   * <p>
   * A result must be within 1 ulp of the correctly rounded result.  Results
   * must be semi-monotonic.
   *
   * @param   a   an angle, in radians.
   * @return  the tangent of the argument.
   */
  static double __OPENCODE_NATIVE_CODE tan(double a);
  
  /**
   * Returns the arc sine of an angle, in the range of -<i>pi</i>/2 through
   * <i>pi</i>/2. 
   * <p>
   * A result must be within 1 ulp of the correctly rounded result.  Results
   * must be semi-monotonic.
   *
   * @param   a   the value whose arc sine is to be returned.
   * @return  the arc sine of the argument.
   */
  static double __OPENCODE_NATIVE_CODE asin(double a) ;
  /**
   * Returns the arc cosine of an angle, in the range of 0.0 through
   * <i>pi</i>. 
   * <p>
   * A result must be within 1 ulp of the correctly rounded result.  Results 
   * must be semi-monotonic.
   *
   * @param   a   the value whose arc cosine is to be returned.
   * @return  the arc cosine of the argument.
   */
  static double __OPENCODE_NATIVE_CODE acos(double a);
  
  /**
   * Returns the arc tangent of an angle, in the range of -<i>pi</i>/2
   * through <i>pi</i>/2.  
   * <p>
   * A result must be within 1 ulp of the correctly rounded result.  Results
   * must be semi-monotonic.
   *
   * @param   a   the value whose arc tangent is to be returned.
   * @return  the arc tangent of the argument.
   */
  static double __OPENCODE_NATIVE_CODE atan(double a);
  
  /**
   * Converts an angle measured in degrees to an approximately
   * equivalent angle measured in radians.  The conversion from
   * degrees to radians is generally inexact.
   *
   * @param   angdeg   an angle, in degrees
   * @return  the measurement of the angle <code>angdeg</code>
   *          in radians.
   */
  static double __OPENCODE_NATIVE_CODE toRadians(double angdeg);
  
  /**
   * Converts an angle measured in radians to an approximately
   * equivalent angle measured in degrees.  The conversion from
   * radians to degrees is generally inexact; users should
   * <i>not</i> expect <code>cos(toRadians(90.0))</code> to exactly
   * equal <code>0.0</code>.
   *
   * @param   angrad   an angle, in radians
   * @return  the measurement of the angle <code>angrad</code>
   *          in degrees.
   */
  static double __OPENCODE_NATIVE_CODE toDegrees(double angrad);
  
  /**
   * Returns Euler's number <i>e</i> raised to the power of a
   * <code>double</code> value. 
   * <p>
   * A result must be within 1 ulp of the correctly rounded result.  Results
   * must be semi-monotonic.
   *
   * @param   a   the exponent to raise <i>e</i> to.
   * @return  the value <i>e</i><sup><code>a</code></sup>, 
   *          where <i>e</i> is the base of the natural logarithms.
   */
  static double __OPENCODE_NATIVE_CODE  exp(double a);
  
  /**
   * Returns the natural logarithm (base <i>e</i>) of a <code>double</code>
   * value.  
   * <p>
   * A result must be within 1 ulp of the correctly rounded result.  Results
   * must be semi-monotonic.
   *
   * @param   a   a number greater than <code>0.0</code>.
   * @return  the value ln&nbsp;<code>a</code>, the natural logarithm of
   *          <code>a</code>.
   */
  static double __OPENCODE_NATIVE_CODE log(double a);

  /**
   * Returns the 10 logarithm (base <i>10</i>) of a <code>double</code>
   * value.  
   * A result must be within 1 ulp of the correctly rounded result.  Results
   * must be semi-monotonic.
   *
   * @param   a   a number greater than <code>0.0</code>.
   * @return  the value ln&nbsp;<code>a</code>, the 10 logarithm of
   *          <code>a</code>.
   */
  static double __OPENCODE_NATIVE_CODE log10(double a);

  /**
   * Returns the 2 logarithm (base <i>2</i>) of a <code>double</code>
   * value.  
   * A result must be within 1 ulp of the correctly rounded result.  Results
   * must be semi-monotonic.
   *
   * @param   a   a number greater than <code>0.0</code>.
   * @return  the value ln&nbsp;<code>a</code>, the 2 logarithm of
   *          <code>a</code>.
   */
  static double __OPENCODE_NATIVE_CODE log2(double a);
      
  /**
   * Returns the correctly rounded positive square root of a 
   * <code>double</code> value.
   * Special cases:
   * <ul><li>If the argument is NaN or less than zero, then the result 
   * is NaN. 
   * <li>If the argument is positive infinity, then the result is positive 
   * infinity. 
   * <li>If the argument is positive zero or negative zero, then the 
   * result is the same as the argument.</ul>
   * Otherwise, the result is the <code>double</code> value closest to 
   * the true mathematical square root of the argument value.
   * 
   * @param   a   a value.
   * <!--@return  the value of &radic;&nbsp;<code>a</code>.-->
   * @return  the positive square root of <code>a</code>.
   *          If the argument is NaN or less than zero, the result is NaN.
   */
  static double __OPENCODE_NATIVE_CODE sqrt(double a);
  
  /**
   * Returns the smallest (closest to negative infinity) 
   * <code>double</code> value that is not less than the argument and is 
   * equal to a mathematical integer.
   *
   * @param   a   a value.
   * <!--@return  the value &lceil;&nbsp;<code>a</code>&nbsp;&rceil;.-->
   * @return  the smallest (closest to negative infinity) 
   *          floating-point value that is not less than the argument
   *          and is equal to a mathematical integer. 
   */
  static double __OPENCODE_NATIVE_CODE ceil(double a);
  
  /**
   * Returns the largest (closest to positive infinity) 
   * <code>double</code> value that is not greater than the argument and 
   * is equal to a mathematical integer. 
   *
   * @param   a   a value.
   * <!--@return  the value &lfloor;&nbsp;<code>a</code>&nbsp;&rfloor;.-->
   * @return  the largest (closest to positive infinity) 
   *          floating-point value that is not greater than the argument
   *          and is equal to a mathematical integer. 
   */
  static double __OPENCODE_NATIVE_CODE floor(double a);
  
  /**
   * Converts rectangular coordinates (<code>x</code>,&nbsp;<code>y</code>)
   * to polar (r,&nbsp;<i>theta</i>).
   * This method computes the phase <i>theta</i> by computing an arc tangent
   * of <code>y/x</code> in the range of -<i>pi</i> to <i>pi</i>. Special 
   * cases:
   * <ul><li>If either argument is NaN, then the result is NaN. 
   * <li>If the first argument is positive zero and the second argument 
   * is positive, or the first argument is positive and finite and the 
   * second argument is positive infinity, then the result is positive 
   * zero. 
   * <li>If the first argument is negative zero and the second argument 
   * is positive, or the first argument is negative and finite and the 
   * second argument is positive infinity, then the result is negative zero. 
   * <li>If the first argument is positive zero and the second argument 
   * is negative, or the first argument is positive and finite and the 
   * second argument is negative infinity, then the result is the 
   * <code>double</code> value closest to <i>pi</i>. 
   * <li>If the first argument is negative zero and the second argument 
   * is negative, or the first argument is negative and finite and the 
   * second argument is negative infinity, then the result is the 
   * <code>double</code> value closest to -<i>pi</i>. 
   * <li>If the first argument is positive and the second argument is 
   * positive zero or negative zero, or the first argument is positive 
   * infinity and the second argument is finite, then the result is the 
   * <code>double</code> value closest to <i>pi</i>/2. 
   * <li>If the first argument is negative and the second argument is 
   * positive zero or negative zero, or the first argument is negative 
   * infinity and the second argument is finite, then the result is the 
   * <code>double</code> value closest to -<i>pi</i>/2. 
   * <li>If both arguments are positive infinity, then the result is the 
   * <code>double</code> value closest to <i>pi</i>/4. 
   * <li>If the first argument is positive infinity and the second argument 
   * is negative infinity, then the result is the <code>double</code> 
   * value closest to 3*<i>pi</i>/4. 
   * <li>If the first argument is negative infinity and the second argument 
   * is positive infinity, then the result is the <code>double</code> value 
   * closest to -<i>pi</i>/4. 
   * <li>If both arguments are negative infinity, then the result is the 
   * <code>double</code> value closest to -3*<i>pi</i>/4.</ul>
   * <p>
   * A result must be within 2 ulps of the correctly rounded result.  Results
   * must be semi-monotonic.
   *
   * @param   y   the ordinate coordinate
   * @param   x   the abscissa coordinate
   * @return  the <i>theta</i> component of the point
   *          (<i>r</i>,&nbsp;<i>theta</i>)
   *          in polar coordinates that corresponds to the point
   *          (<i>x</i>,&nbsp;<i>y</i>) in Cartesian coordinates.
   */
  static double __OPENCODE_NATIVE_CODE atan2(double y, double x);
  
  /**
   * Returns the value of the first argument raised to the power of the
   * second argument. Special cases:
   *
   * <ul><li>If the second argument is positive or negative zero, then the 
   * result is 1.0. 
   * <li>If the second argument is 1.0, then the result is the same as the 
   * first argument.
   * <li>If the second argument is NaN, then the result is NaN. 
   * <li>If the first argument is NaN and the second argument is nonzero, 
   * then the result is NaN. 
   *
   * <li>If
   * <ul>
   * <li>the absolute value of the first argument is greater than 1
   * and the second argument is positive infinity, or
   * <li>the absolute value of the first argument is less than 1 and
   * the second argument is negative infinity,
   * </ul>
   * then the result is positive infinity. 
   *
   * <li>If 
   * <ul>
   * <li>the absolute value of the first argument is greater than 1 and 
   * the second argument is negative infinity, or 
   * <li>the absolute value of the 
   * first argument is less than 1 and the second argument is positive 
   * infinity,
   * </ul>
   * then the result is positive zero. 
   *
   * <li>If the absolute value of the first argument equals 1 and the 
   * second argument is infinite, then the result is NaN. 
   *
   * <li>If 
   * <ul>
   * <li>the first argument is positive zero and the second argument
   * is greater than zero, or
   * <li>the first argument is positive infinity and the second
   * argument is less than zero,
   * </ul>
   * then the result is positive zero. 
   *
   * <li>If 
   * <ul>
   * <li>the first argument is positive zero and the second argument
   * is less than zero, or
   * <li>the first argument is positive infinity and the second
   * argument is greater than zero,
   * </ul>
   * then the result is positive infinity.
   *
   * <li>If 
   * <ul>
   * <li>the first argument is negative zero and the second argument
   * is greater than zero but not a finite odd integer, or
   * <li>the first argument is negative infinity and the second
   * argument is less than zero but not a finite odd integer,
   * </ul>
   * then the result is positive zero. 
   *
   * <li>If 
   * <ul>
   * <li>the first argument is negative zero and the second argument
   * is a positive finite odd integer, or
   * <li>the first argument is negative infinity and the second
   * argument is a negative finite odd integer,
   * </ul>
   * then the result is negative zero. 
   *
   * <li>If
   * <ul>
   * <li>the first argument is negative zero and the second argument
   * is less than zero but not a finite odd integer, or
   * <li>the first argument is negative infinity and the second
   * argument is greater than zero but not a finite odd integer,
   * </ul>
   * then the result is positive infinity. 
   *
   * <li>If 
   * <ul>
   * <li>the first argument is negative zero and the second argument
   * is a negative finite odd integer, or
   * <li>the first argument is negative infinity and the second
   * argument is a positive finite odd integer,
   * </ul>
   * then the result is negative infinity. 
   *
   * <li>If the first argument is finite and less than zero
   * <ul>
   * <li> if the second argument is a finite even integer, the
   * result is equal to the result of raising the absolute value of
   * the first argument to the power of the second argument
   *
   * <li>if the second argument is a finite odd integer, the result
   * is equal to the negative of the result of raising the absolute
   * value of the first argument to the power of the second
   * argument
   *
   * <li>if the second argument is finite and not an integer, then
   * the result is NaN.
   * </ul>
   *
   * <li>If both arguments are integers, then the result is exactly equal 
   * to the mathematical result of raising the first argument to the power 
   * of the second argument if that result can in fact be represented 
   * exactly as a <code>double</code> value.</ul>
   * 
   * <p>(In the foregoing descriptions, a floating-point value is
   * considered to be an integer if and only if it is finite and a
   * fixed point of the method {@link #ceil <tt>ceil</tt>} or,
   * equivalently, a fixed point of the method {@link #floor
   * <tt>floor</tt>}. A value is a fixed point of a one-argument
   * method if and only if the result of applying the method to the
   * value is equal to the value.)
   *
   * <p>A result must be within 1 ulp of the correctly rounded
   * result.  Results must be semi-monotonic.
   *
   * @param   a   the base.
   * @param   b   the exponent.
   * @return  the value <code>a<sup>b</sup></code>.
   */
  static double __OPENCODE_NATIVE_CODE pow(double a, double b);
  
  /**
   * Returns the closest <code>int</code> to the argument. The 
   * result is rounded to an integer by adding 1/2, taking the 
   * floor of the result, and casting the result to type <code>int</code>. 
   * In rv words, the result is equal to the value of the expression:
   * <p><pre>(int)Math::floor(a + 0.5f)</pre>
   *
   * @param   a   a floating-point value to be rounded to an integer.
   * @return  the value of the argument rounded to the nearest
   *          <code>int</code> value.
   */
  static int __OPENCODE_NATIVE_CODE round(float a) ;
  static int __OPENCODE_NATIVE_CODE round(float a,int mode) ;
  
  /**
   * Returns the closest <code>XLONG</code> to the argument. The result 
   * is rounded to an integer by adding 1/2, taking the floor of the 
   * result, and casting the result to type <code>XLONG</code>. In rv 
   * words, the result is equal to the value of the expression:
   * <p><pre>(XLONG)Math::floor(a + 0.5d)</pre>
   *
   * @param   a   a floating-point value to be rounded to a 
   *    <code>XLONG</code>.
   * @return  the value of the argument rounded to the nearest
   *          <code>XLONG</code> value.
   */
  static XLONG __OPENCODE_NATIVE_CODE round(double a) ;
  static XLONG __OPENCODE_NATIVE_CODE round(double a,int mode) ;

  /**
   * Returns the absolute value of an <code>int</code> value.
   * If the argument is not negative, the argument is returned.
   * If the argument is negative, the negation of the argument is returned.  
   *
   * @param   a   the argument whose absolute value is to be determined
   * @return  the absolute value of the argument.
   */
  static int __OPENCODE_NATIVE_CODE abs(int a) ;
  
  /**
   * Returns the absolute value of a <code>XLONG</code> value.
   * If the argument is not negative, the argument is returned.
   * If the argument is negative, the negation of the argument is returned. 
   *
   * @param   a   the argument whose absolute value is to be determined
   * @return  the absolute value of the argument.
   */
  static XLONG __OPENCODE_NATIVE_CODE abs(XLONG a) ;
  
  /**
   * Returns the absolute value of a <code>float</code> value.
   * If the argument is not negative, the argument is returned.
   * If the argument is negative, the negation of the argument is returned.
   *
   * @param   a   the argument whose absolute value is to be determined
   * @return  the absolute value of the argument.
   */
  static float __OPENCODE_NATIVE_CODE abs(float a);
  
  /**
   * Returns the absolute value of a <code>double</code> value.
   * If the argument is not negative, the argument is returned.
   * If the argument is negative, the negation of the argument is returned.
   *
   * @param   a   the argument whose absolute value is to be determined
   * @return  the absolute value of the argument.
   */
  static double __OPENCODE_NATIVE_CODE abs(double a);
  
//-------------------------------------------------------------------------------------------
//Static Constants
//-------------------------------------------------------------------------------------------
public:
  /**
   * The <code>double</code> value that is closer than any rv to
   * <i>e</i>, the base of the natural logarithms.
   */
  static const double E;
  
  /**
   * The <code>double</code> value that is closer than any rv to
   * <i>pi</i>, the ratio of the circumference of a circle to its
   * diameter.
   */
  static const double PI;
  
  static const double LOG2E;   /* log_2 e */

  static const double LOG10E;  /* log_10 e */

  static const double LN2;  /* log_e 2 */

  static const double LN10;  /* log_e 10 */

  static const double PI_OVER_2;  /* pi/2 */

  static const double PI_OVER_4;  /* pi/4 */

  static const double SQRT2;  /* sqrt(2) */
};

__OPENCODE_END_NAMESPACE

#endif
