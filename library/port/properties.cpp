#include "port/properties.h"
#include "port/logging.h"
#include "port/file.h"
#include "port/lineparser.h"

__OPENCODE_BEGIN_NAMESPACE
  
Properties::~Properties()
{
}

Properties::Properties(const Properties& rv)
:props(rv.props)
{
}

Properties& Properties::operator=(const Properties& rv)
{
  if(&rv == this)
  {
    return *this;
  }

  props = rv.props;

  return *this;
}

Properties::Properties()
{
}

Properties::Properties(const string& fileName)
{
  append(fileName);
}

void Properties::append(const Properties& rv)
{
  vector<string> names ;
  rv.propertyNames(names);
 
  for(size_t i=0;i<names.size();++i)
  {
    this->setProperty(names[i],rv.getProperty(names[i]));
  }
}

void Properties::append(const string& fileName)
{
  if(fileName.empty())
  {
    return;
  }

  FILE* fp = fopen(fileName.c_str(),"r");
  if(fp == 0)
  {
    LOGGING_WARN("Properties::<init>|file not found : " << fileName);  
    return;
  }
  
  while(true)
  {
    char line[OPENCODE_MAXLINE + 1] = {0};
    if( LineFile::readLine(fp,line,OPENCODE_MAXLINE) <= 0)
    {
      break;
    }
 
    size_t len = strlen(line);
     
    // Find start of key      
    size_t keyStart;
    for(keyStart = 0; keyStart < len; keyStart++) 
    {
      if(strchr(LineFile::whiteSpaceChars,line[keyStart]) == 0)
      {
        break;
      }
    }
     
    // Blank lines are ignored
    if (keyStart == len)
    {
      continue;
    }

    // Find separation between key and value
    size_t separatorIndex;
    for(separatorIndex = keyStart; separatorIndex < len; separatorIndex++) 
    {
      char currentChar = line[separatorIndex];
      if (currentChar == '\\')
      {
         separatorIndex++;
      }
      else if(strchr(LineFile::keyValueSeparators,currentChar) != 0)
      {
         break;
      }
    }

    // Skip over whitespace after key if any
    size_t valueIndex = 0;
    for (valueIndex = separatorIndex; valueIndex < len; valueIndex++)
    {            
      if (strchr(LineFile::whiteSpaceChars,line[valueIndex]) == 0)
      { 
        break;
      }
    }

    // Skip over one non whitespace key value separators if any
    if (valueIndex < len)
    {            
      if (strchr(LineFile::strictKeyValueSeparators,line[valueIndex]) != 0)
      {
         valueIndex++;
      }
    }

    // Skip over white space after value separators if any
    while (valueIndex < len) 
    {
      if (strchr(LineFile::whiteSpaceChars,line[valueIndex]) == 0)
      {  
        break;
      }
      valueIndex++;
    }

    if(valueIndex >= len)
    {
      //no key-value pair (in fact ,it is no value)
      continue;
    }

    string key(line + keyStart,separatorIndex - keyStart);         
    string value; 

    if(separatorIndex < len)
    {
       value = line + valueIndex;
    }
    // Convert then store key and value
    key = LineFile::loadConvert(key);
    value = LineFile::loadConvert(value);
    props[key] = value;
  }//~end while(true)  
  
  fclose(fp);
}

const string& Properties::getProperty(const string& key) const
{
  OPENCODE_MAP<string,string>::const_iterator __pos = props.find(key);
  if(__pos != props.end())
  {
    return __pos->second;
  }
    
  return VALUE_NULL;
}

string Properties::getProperty(const string& key,const string& defaultValue) const
{
  OPENCODE_MAP<string,string>::const_iterator __pos = props.find(key);
  if(__pos != props.end())
  {
    return __pos->second;
  }

  return defaultValue;
}

void Properties::setProperty(const string& key,const string& value)
{
  props[key] = value; 
}

void Properties::removeProperty(const string& key)
{
  OPENCODE_MAP<string,string>::iterator __pos = props.find(key);
  if(__pos != props.end())
  {
    props.erase(__pos);
  }
}

void Properties::dump(StringBuffer& __sb) const
{
  OPENCODE_MAP<string,string>::const_iterator __pos = props.begin();
  while(__pos != props.end())
  {
    __sb << __pos->first << "=" << __pos->second << File::lineSeparator;
    ++__pos;
  }
}

void Properties::propertyNames(vector<string>& names) const
{
  OPENCODE_MAP<string,string>::const_iterator __pos = props.begin();
  while(__pos != props.end())
  {
    names.push_back( __pos->first);
    __pos++;
  }
}

__OPENCODE_END_NAMESPACE
