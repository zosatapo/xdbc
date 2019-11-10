#include "port/stringTokenizer.h"

__OPENCODE_BEGIN_NAMESPACE

StringTokenizer::StringTokenizer(const string& str, const string& delim, bool returnDelims)
:str_value(str),str_delimiters(delim)
{
  int_currentPosition = 0;
  int_newPosition=-1;
  int_maxPosition = (int)(str_value.length());
  bol_retDelims = returnDelims;
}

StringTokenizer::StringTokenizer(const char* str,int size, const string& delim, bool returnDelims)
:str_value(str,size),str_delimiters(delim)
{
  int_currentPosition = 0;
  int_newPosition=-1;
  int_maxPosition = (int)(str_value.length());
  bol_retDelims = returnDelims;

}

StringTokenizer::StringTokenizer(const string& str)
:str_value(str),str_delimiters(" \t\n\r\f")
{
  int_currentPosition = 0;
  int_newPosition=-1;
  int_maxPosition = (int)(str_value.length());
  bol_retDelims = false;
}

StringTokenizer::StringTokenizer(const char* str,int size)
:str_value(str,size),str_delimiters(" \t\n\r\f")
{
  int_currentPosition = 0;
  int_newPosition=-1;
  int_maxPosition = (int)(str_value.length());
  bol_retDelims = false;
}

vector<string> StringTokenizer::parseTokens(const string& str, const string& delim, bool returnDelims)
{
  vector<string> values;
  parseTokens(values,str,delim,returnDelims);
  return values;
}

void StringTokenizer::parseTokens(vector<string>& values,const string& str, const string& delim, bool returnDelims)
{
  StringTokenizer tokens(str,delim,returnDelims);
  while(tokens.hasMoreTokens())
  {
    values.push_back(tokens.nextToken());
  }
}

vector<string>  StringTokenizer::parseTokens(const char* str,int size, const string& delim, bool returnDelims)
{
  vector<string> values;
  parseTokens(values,str,size,delim,returnDelims);
  return values;
}

void  StringTokenizer::parseTokens(vector<string>& values,const char* str,int size, const string& delim, bool returnDelims)
{
  StringTokenizer tokens(str,size,delim,returnDelims);
  while(tokens.hasMoreTokens())
  {
    values.push_back(tokens.nextToken());
  }
}

vector<string> StringTokenizer::parseTokens(const string& str)
{
  vector<string> values;
  parseTokens(values,str);
  return values; 
}

void StringTokenizer::parseTokens(vector<string>& values,const string& str)
{
  StringTokenizer tokens(str);
  while(tokens.hasMoreTokens())
  {
    values.push_back(tokens.nextToken());
  }
}

vector<string> StringTokenizer::parseTokens(const char* str,int size)
{
  vector<string> values;
  parseTokens(values,str,size);
  return values;
}

void StringTokenizer::parseTokens(vector<string>& values,const char* str,int size)
{
  StringTokenizer tokens(str,size);
  while(tokens.hasMoreTokens())
  {
    values.push_back(tokens.nextToken());
  }
}

int StringTokenizer::skipDelimiters(int startPos) 
{
  int position = startPos;
  while (!bol_retDelims && position < int_maxPosition) 
  {
    char c = str_value.at(position);
    if ((str_delimiters.find_first_of(c) ==string::npos))
    {
      break;
    }  
    position++;
  }
  
  return position;
}


int StringTokenizer::scanToken(int startPos) 
{
  int position = startPos;
  while (position < int_maxPosition) 
  {
    char c = str_value.at(position);
    if ((str_delimiters.find_first_of(c) !=string::npos))
    {
      break;
    }  
    position++;
  }
  
  if (bol_retDelims && (startPos == position)) 
  {
    char c = str_value.at(position);
    if ((str_delimiters.find_first_of(c) !=string::npos))
    {
      position++;
    }  
  }
  return position;
}

bool StringTokenizer::hasMoreTokens() 
{
  /*
  * Temporary store this position and use it in the following
  * nextToken() method only if the delimiters have'nt been changed in
  * that nextToken() invocation.
  */
  int_newPosition = skipDelimiters(int_currentPosition);
  return (int_newPosition < int_maxPosition);
}

string StringTokenizer::nextToken() 
{
  int_currentPosition = (int_newPosition >= 0 ) ?  int_newPosition : skipDelimiters(int_currentPosition);
  int_newPosition = -1;
  
  int start = int_currentPosition;
  int_currentPosition = scanToken(int_currentPosition);
  return str_value.substr(start, int_currentPosition-start);
}

int StringTokenizer::countTokens() 
{
  int count = 0;
  int currpos = int_currentPosition;
  while (currpos < int_maxPosition) 
  {
    currpos = skipDelimiters(currpos);
    if (currpos >= int_maxPosition)
    {
      break;
    }
    currpos = scanToken(currpos);
    count++;
  }
  return count;
}

__OPENCODE_END_NAMESPACE


