/**
 *  lineparser.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */

#ifndef __OPENCODE_LINEPARSER_H__
#define __OPENCODE_LINEPARSER_H__

#include "port/libdefs.h"

__OPENCODE_BEGIN_NAMESPACE

class __OPENCODE_CORE_API_DECL LineError
{
public:
  ~LineError();
  
  LineError();
  LineError(const XLONG& errcode,int errorline = -1,const char* errmsg = 0);

public:
  XLONG    code;
  int      line;
  string   msg;
};

class __OPENCODE_CORE_API_DECL LineParser
{
public:
  virtual ~LineParser(){}
  virtual XLONG parse(const char* line,XLONG size,LineError& error) = 0;    
};

class __OPENCODE_CORE_API_DECL LineReader
{
public:
  virtual ~LineReader(){}
  virtual XLONG read(FILE* fp,char* line,XLONG size,LineError& error) = 0;    
};

class __OPENCODE_CORE_API_DECL LineErrorHandler
{
public:
  virtual ~LineErrorHandler(){}
  virtual XLONG handle(const LineError& error) = 0;
}; 

class __OPENCODE_CORE_API_DECL DefaultLineErrorHandler : public LineErrorHandler
{
public:
  virtual ~DefaultLineErrorHandler();
  DefaultLineErrorHandler();

public:  
  virtual XLONG handle(const LineError& error);  
};   

class __OPENCODE_CORE_API_DECL DefaultLineReader : public LineReader
{
public:
  virtual ~DefaultLineReader();
  DefaultLineReader();
  
  virtual XLONG read(FILE* fp,char* line,XLONG size,LineError& error);    
};


//-------------------------------------------------------------------------
//
//
//-------------------------------------------------------------------------

#define OPENCODE_MAXLINE   8192

class __OPENCODE_CORE_API_DECL LineFile
{
private:
  FILE* fp;
  
public:
  ~LineFile(); 
  LineFile(FILE* fp);
  
  LineError process(LineReader& reader,LineParser& parser, LineErrorHandler& errorHandler);   

public:
  const  static  char keyValueSeparators[];
  const  static  char strictKeyValueSeparators[];
  const  static  char specialSaveChars[];
  const  static  char whiteSpaceChars[];
  const  static  char commentLineChars[];
  
  /** read a line from fp and return bytes read */
  static XLONG readLine(FILE* fp,char* line,XLONG size,bool supportContineLine = true);
    
  /**
   * Converts encoded &#92;uxxxx to unicode chars
   * and changes special saved chars to their original forms
   */
  static string loadConvert(const string& theString);
  
  /**
   * Converts unicodes to encoded &#92;uxxxx
   * and writes out any of the characters in specialSaveChars
   * with a preceding slash
   */  
  static string saveConvert(const string& theString, bool escapeSpace); 

  /**
   * Returns true if the given line is a line that must
   * be appended to the next line
   */
  static bool   continueLine(const char* line);

  /** chop trailing CR & LF sequence characters */  
  static int    chopCRLF(char* line);  
};  

__OPENCODE_END_NAMESPACE

#endif

