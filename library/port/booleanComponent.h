/** 
 *  booleanComponent.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
 
#ifndef __OPENCODE_BOOLEANCOMPONENT_H__
#define __OPENCODE_BOOLEANCOMPONENT_H__

#include "port/libdefs.h"

__OPENCODE_BEGIN_NAMESPACE

class __OPENCODE_CORE_API_DECL BooleanComponent
{
public:
  /**
   * Allocates a <code>BooleanComponent</code> object representing the 
   * <code>value</code> argument. 
   *
   * <p><b>Note: It is rarely appropriate to use this constructor.
   * Unless a <i>new</i> instance is required, the static factory
   * {@link #valueOf(bool)} is generally a better choice. It is
   * likely to yield significantly better space and time performance.</b>
   * 
   * @param   value   the value of the <code>BooleanComponent</code>.
   */
  BooleanComponent(bool value);
  
  /**
   * Allocates a <code>BooleanComponent</code> object representing the value 
   * <code>true</code> if the string argument is not <code>null</code> 
   * and is equal, ignoring case, to the string <code>"true"</code>. 
   * Otherwise, allocate a <code>BooleanComponent</code> object representing the 
   * value <code>false</code>. Examples:<p>
   * <tt>new&nbsp;BooleanComponent("True")</tt> produces a <tt>BooleanComponent</tt> object 
   * that represents <tt>true</tt>.<br>
   * <tt>new&nbsp;BooleanComponent("yes")</tt> produces a <tt>BooleanComponent</tt> object 
   * that represents <tt>false</tt>.
   *
   * @param   s   the string to be converted to a <code>BooleanComponent</code>.
   */
  BooleanComponent(const string& s);
  
  /**
   * Returns the value of this <tt>BooleanComponent</tt> object as a bool 
   * primitive.
   *
   * @return  the primitive <code>bool</code> value of this object.
   */
  bool booleanValue() const;

  /**
   * Returns a <code>bool</code> with a value represented by the
   * specified String.  The <code>bool</code> returned represents the
   * value <code>true</code> if the string argument is not <code>null</code>
   * and is equal, ignoring case, to the string <code>"true"</code>. <p>
   * Example: <tt>BooleanComponent::valueOf("True")</tt> returns <tt>true</tt>.<br>
   * Example: <tt>BooleanComponent::valueOf("yes")</tt> returns <tt>false</tt>.
   *
   * @param   s   a string.
   * @return  the <code>bool</code> value represented by the string.
   */
  static bool valueOf(const string& s) ;
  static bool valueOf(const string& s,bool& matchStricted); 

  /**
   * Returns a <tt>String</tt> object representing this BooleanComponent's
   * value.  If this object represents the value <code>true</code>,
   * a string equal to <code>"true"</code> is returned. Otherwise, a
   * string equal to <code>"false"</code> is returned.
   *
   * @return  a string representation of this object. 
   */
  string toString() const;

public:  
  /**
   * Returns <code>true</code> if and only if the system property 
   * named by the argument exists and is equal to ,case insensitive, 
   * the string <code>"true"</code>. A system property is accessible 
   * through <code>getProperty</code>, a method defined by the 
   * <code>System</code> class.
   * <p>
   * If there is no property with the specified name, or if the specified
   * name is empty , then <code>false</code> is returned.
   *
   * @param   name   the system property name.
   * @return  the <code>bool</code> value of the system property.
   */
  static bool getBoolean(const string& name);
  static bool getBoolean(const string& name,bool& matchStricted);

private:
  static bool toBoolean(const string& s);
  static bool toBoolean(const string& s,bool& matchStricted);

private:
  bool value;
};

__OPENCODE_END_NAMESPACE

#endif
