/** 
 *  stringTokenizer.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
 
#ifndef  _OPENCODE_STRINGTOKENIZER_H__
#define  _OPENCODE_STRINGTOKENIZER_H__

#include "port/libdefs.h"
#include "port/ioc_vector.h"

__OPENCODE_BEGIN_NAMESPACE

class __OPENCODE_CORE_API_DECL StringTokenizer
{
public:
  /**
   * Constructs a string tokenizer for the specified string. All  
   * characters in the <code>delim</code> argument are the delimiters 
   * for separating tokens. 
   * <p>
   * If the <code>returnDelims</code> flag is <code>true</code>, then 
   * the delimiter characters are also returned as tokens. Each 
   * delimiter is returned as a string of length one. If the flag is 
   * <code>false</code>, the delimiter characters are skipped and only 
   * serve as separators between tokens. 
   *
   * @param   str            a string to be parsed.
   * @param   delim          the delimiters.
   * @param   returnDelims   flag indicating whether to return the delimiters
   *                         as tokens.
   */
   StringTokenizer(const string& str, const string& delim, bool returnDelims = false);
   StringTokenizer(const char* str,int size, const string& delim, bool returnDelims = false);

  /**
   * Constructs a string tokenizer for the specified string. The 
   * tokenizer uses the default delimiter set, which is 
   * <code>"&nbsp;&#92;t&#92;n&#92;r&#92;f"</code>: the space character, 
   * the tab character, the newline character, the carriage-return character,
   * and the form-feed character. Delimiter characters themselves will 
   * not be treated as tokens.
   *
   * @param   str   a string to be parsed.
   */
   StringTokenizer(const string& str);
   StringTokenizer(const char* str,int size);

  /**
   * Constructs a string tokenizer for the specified string. All
   * characters in the <code>delim</code> argument are the delimiters
   * for separating tokens.
   * <p>
   * If the <code>returnDelims</code> flag is <code>true</code>, then
   * the delimiter characters are also returned as tokens. Each
   * delimiter is returned as a string of length one. If the flag is
   * <code>false</code>, the delimiter characters are skipped and only
   * serve as separators between tokens.
   *
   * @param   str            a string to be parsed.
   * @param   delim          the delimiters.
   * @param   returnDelims   flag indicating whether to return the delimiters
   *                         as tokens.
   */
   static vector<string> parseTokens(const string& str, const string& delim, bool returnDelims = false);
   static vector<string> parseTokens(const char* str,int size, const string& delim, bool returnDelims = false);

   static void parseTokens(vector<string>& values,const string& str, const string& delim, bool returnDelims = false);
   static void parseTokens(vector<string>& values,const char* str,int size, const string& delim, bool returnDelims = false);

  /**
   * Constructs a string tokenizer for the specified string. The
   * tokenizer uses the default delimiter set, which is
   * <code>"&nbsp;&#92;t&#92;n&#92;r&#92;f"</code>: the space character,
   * the tab character, the newline character, the carriage-return character,
   * and the form-feed character. Delimiter characters themselves will
   * not be treated as tokens.
   *
   * @param   str   a string to be parsed.
   */
   static vector<string> parseTokens(const string& str);
   static vector<string> parseTokens(const char* str,int size);

   static void parseTokens(vector<string>& values,const string& str);
   static void parseTokens(vector<string>& values,const char* str,int size);

public:
  /**
   * Tests if there are more tokens available from this tokenizer's string. 
   * If this method returns <tt>true</tt>, then a subsequent call to 
   * <tt>nextToken</tt> with no argument will successfully return a token.
   *
   * @return  <code>true</code> if and only if there is at least one token 
   *          in the string after the current position; <code>false</code> 
   *          otherwise.
   */
  bool hasMoreTokens() ;
  
  /**
   * Returns the next token from this string tokenizer.
   *
   * @return     the next token from this string tokenizer.
   * @exception  NoSuchElementException  if there are no more tokens in this
   *               tokenizer's string.
   */
  string nextToken();
  
  /**
   * Calculates the number of times that this tokenizer's 
   * <code>nextToken</code> method can be called before it generates an 
   * exception. The current position is not advanced.
   *
   * @return  the number of tokens remaining in the string using the current
   *          delimiter set.
   */
  int countTokens() ;
         
private:
  /**
   * Skips delimiters starting from the specified position. If retDelims
   * is false, returns the index of the first non-delimiter character at or
   * after startPos. If retDelims is true, startPos is returned.
   */
   int skipDelimiters(int startPos);
   
  /**
   * Skips ahead from startPos and returns the index of the next delimiter
   * character encountered, or maxPosition if no such delimiter is found.
   */
   int scanToken(int startPos);
  
private:
  int int_currentPosition;
  int int_maxPosition;
  int int_newPosition;

  string str_value;
  string str_delimiters;

  bool bol_retDelims;
};

__OPENCODE_END_NAMESPACE

#endif

