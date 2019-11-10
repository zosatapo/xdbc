/**
 *  clob.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef   __OPENCODE_XDBC_CLOB_H__
#define   __OPENCODE_XDBC_CLOB_H__

#include "xdbc/xdefine.h"

__OPENCODE_BEGIN_NAMESPACE

class __OPENCODE_XDBC_API_DECL Clob 
{
public:
  virtual ~Clob(){}

public:
  /**
   * Retrieves the number of characters
   * in the <code>CLOB</code> value
   * designated by this <code>Clob</code> object.
   *
   * @return length of the <code>CLOB</code> in characters
   * @exception SQLException if there is an error accessing the
   *            length of the <code>CLOB</code> value
   */
  virtual int length(__XDBC_HOOK_DECLARE_ONCE) const  __XDBC_THROW_DECLARE(SQLException) = 0;
    
  /**
   * Writes the given STL <code>string</code> to the <code>CLOB</code>
   * value that this <code>Clob</code> object designates at the position 
   * <code>pos</code>.
   *
   * @param pos the position at which to start writing to the <code>CLOB</code>
   *         value that this <code>Clob</code> object represents
   * @param str the string to be written to the <code>CLOB</code>
   *        value that this <code>Clob</code> designates
   * @return the number of characters written
   * @exception SQLException if there is an error accessing the 
   *            <code>CLOB</code> value
   *
   */
  virtual int write(__XDBC_HOOK_DECLARE_MORE int pos, const string& str) __XDBC_THROW_DECLARE(SQLException) = 0;
  
  /**
   * Writes <code>len</code> characters of <code>str</code>, starting 
   * at character <code>offset</code>, to the <code>CLOB</code> value
   * that this <code>Clob</code> represents.
   *
   * @param pos the position at which to start writing to this
   *        <code>CLOB</code> object
   * @param str the string to be written to the <code>CLOB</code> 
   *        value that this <code>Clob</code> object represents
   * @param offset the offset into <code>str</code> to start reading
   *        the characters to be written
   * @param len the number of characters to be written
   * @return the number of characters written
   * @exception SQLException if there is an error accessing the 
   *            <code>CLOB</code> value
   */
  virtual int write(__XDBC_HOOK_DECLARE_MORE int pos, const char* str, int offset, int len) __XDBC_THROW_DECLARE(SQLException) = 0;
 

  /**
   * Retrieves all or part of the <code>CLOB</code>
   * value that this <code>Blob</code> object represents, as an array of
   * characters.  This <code>character</code> array contains up to <code>length</code> 
   * consecutive characters starting at position <code>pos</code>.
   *
   * @param pos the ordinal position of the first byte in the 
   *        <code>CLOB</code> value to be extracted; the first byte is at
   *        position 1 
   * @param length the number of consecutive characters to be copied
   * @return a byte array containing up to <code>length</code> 
   *         consecutive characters from the <code>CLOB</code> value designated
   *         by this <code>Clob</code> object, starting with the
   *         byte at position <code>pos</code>
   * @exception SQLException if there is an error accessing the
   *            <code>CLOB</code> value
   */
  virtual int read(__XDBC_HOOK_DECLARE_MORE int pos, char* str,int offset,int length) const __XDBC_THROW_DECLARE(SQLException) = 0; 
 
  /**
   * Truncates the <code>CLOB</code> value that this <code>Clob</code> 
   * designates to have a length of <code>len</code> 
   * characters.
   * @param len the length, in characters, to which the <code>CLOB</code> value
   *        should be truncated
   * @exception SQLException if there is an error accessing the 
   *            <code>CLOB</code> value
   */
  virtual void truncate(__XDBC_HOOK_DECLARE_MORE int len) __XDBC_THROW_DECLARE(SQLException) = 0;
  
};

__OPENCODE_END_NAMESPACE

#endif
