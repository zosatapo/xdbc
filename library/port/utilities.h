/** 
 *  utilities.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
 
#ifndef __OPENCODE_BASE_UTILITIES_H__
#define __OPENCODE_BASE_UTILITIES_H__

#include "port/libdefs.h"

__OPENCODE_BEGIN_NAMESPACE

#if !defined(OPENCODE_HAS_STRCASECMP)
__OPENCODE_CORE_API_DECL int strcasecmp(const char* s1, const char* s2);
__OPENCODE_CORE_API_DECL int strncasecmp(const char* s1, const char* s2,size_t n);
#endif

class __OPENCODE_CORE_API_DECL StringsKit
{
public:
	static bool equalsIgnoreCase(const char* s1, const char* s2);
	static bool equalsIgnoreCase(const char* s1, const char* s2,size_t n);
	static bool equalsIgnoreCase(const string& s1, const string& s2);

public: 
  static string   toUpperCase(const string& s);
  static string   toLowerCase(const string& s);
  
public:
  static string  trimLeft(const string& s);
  static string  trimRight(const string& s);	
  static string  trim(const string& s);
  
public:
  static bool endsWith(const string& s, const string& suffix);
  static bool startsWith(const string& s, const string& prefix);
  	
public:
  static string  replace(const string& s,size_t pos, size_t n, const string& dst);
  static string  replaceAll(const string& s,size_t pos,const string& src, const string& dst);
  static string  replaceProperty(const string& toval); 	  		
};

class __OPENCODE_CORE_API_DECL BitsKit
{   
public:
  /**
   * Returns <tt>true</tt> if and only if the designated bit is set.
   * 
   * @param   offset index of bit to test. (first bit is 0,second bit is 1 ,and so on ...)
   * @return  true if and only if the designated bit is set. 
   */
  static bool testBit(unsigned char* mem,XLONG offset);

  /**
   * @param offset index of bit to set.(first bit is 0,second bit is 1 ,and so on ...)
   */
  static unsigned char setBit(unsigned char* mem,XLONG offset);

  /**
   * @param offset index of bit to clear.(first bit is 0,second bit is 1 ,and so on ...)
   */
  static unsigned char  clearBit(unsigned char* mem,XLONG offset);

  /**
   * @param offset index of bit to flip.(first bit is 0,second bit is 1 ,and so on ...)
   */
  static unsigned char  flipBit(unsigned char* mem,XLONG offset);
};

class __OPENCODE_CORE_API_DECL MemoryKit
{
public:
  static XLONG   roundUp(XLONG memBytes,XLONG alignBytes); 
	
  static void    decode16(string& out,const char* mem,size_t length,size_t offset = 0);
  static void    encode16(string& out,const char* mem,size_t length,size_t offset = 0);
  
  static void    memDump(string& out,const char* mem,size_t length,size_t offset = 0);
  static void    numDump(string& out,const char* mem,size_t length,size_t offset = 0);

  static string  decode16(const char* mem,size_t length,size_t offset = 0);
  static string  encode16(const char* mem,size_t length,size_t offset = 0);
  
  static string  memDump(const char* mem,size_t length,size_t offset = 0);
  static string  numDump(const char* mem,size_t length,size_t offset = 0);  
};
  
__OPENCODE_END_NAMESPACE


#endif
