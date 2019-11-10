/**
 *  blob.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef   __OPENCODE_XDBC_BLOB_H__
#define   __OPENCODE_XDBC_BLOB_H__

#include "xdbc/xdefine.h"

__OPENCODE_BEGIN_NAMESPACE

class __OPENCODE_XDBC_API_DECL Blob 
{
public:
  virtual ~Blob(){}

public:
  /**
   * Retrieves the number of bytes
   * in the <code>BLOB</code> value
   * designated by this <code>Blob</code> object.
   *
   * @return length of the <code>BLOB</code> in bytes
   * @exception SQLException if there is an error accessing the
   *            length of the <code>BLOB</code> value
   */
  virtual int length(__XDBC_HOOK_DECLARE_ONCE) const __XDBC_THROW_DECLARE(SQLException) = 0;

  /**
   * Writes <code>len</code> bytes of <code>str</code>, starting
   * at byte <code>offset</code>, to the <code>BLOB</code> value
   * that this <code>Blob</code> represents.
   *
   * @param pos the position at which to start writing to this
   *        <code>BLOB</code> object
   * @param str the string to be written to the <code>BLOB</code>
   *        value that this <code>Blob</code> object represents
   * @param offset the offset into <code>str</code> to start reading
   *        the bytes to be written
   * @param len the number of bytes to be written
   * @return the number of bytes written
   * @exception SQLException if there is an error accessing the
   *            <code>BLOB</code> value
   */
  virtual int write(__XDBC_HOOK_DECLARE_MORE int pos, const char* str, int offset, int len) __XDBC_THROW_DECLARE(SQLException) = 0;


  /**
   * Retrieves all or part of the <code>BLOB</code>
   * value that this <code>Blob</code> object represents, as an array of
   * bytes.  This <code>character</code> array contains up to <code>length</code>
   * consecutive bytes starting at position <code>pos</code>.
   *
   * @param pos the ordinal position of the first byte in the
   *        <code>BLOB</code> value to be extracted; the first byte is at
   *        position 1
   * @param length the number of consecutive bytes to be copied
   * @return a byte array containing up to <code>length</code>
   *         consecutive bytes from the <code>BLOB</code> value designated
   *         by this <code>Blob</code> object, starting with the
   *         byte at position <code>pos</code>
   * @exception SQLException if there is an error accessing the
   *            <code>BLOB</code> value
   */
  virtual int read(__XDBC_HOOK_DECLARE_MORE int pos, char* str,int offset,int length) const __XDBC_THROW_DECLARE(SQLException) = 0;

  /**
   * Truncates the <code>BLOB</code> value that this <code>Blob</code>
   * designates to have a length of <code>len</code>
   * bytes.
   * @param len the length, in bytes, to which the <code>BLOB</code> value
   *        should be truncated
   * @exception SQLException if there is an error accessing the
   *            <code>BLOB</code> value
   */
  virtual void truncate(__XDBC_HOOK_DECLARE_MORE int len) __XDBC_THROW_DECLARE(SQLException) = 0;
};

__OPENCODE_END_NAMESPACE

#endif
