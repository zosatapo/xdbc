#include "port/profile.h"
#include "port/logging.h"
#include "port/file.h"
#include "port/lineparser.h"

__OPENCODE_BEGIN_NAMESPACE

Profile::~Profile()
{
  Section* ptr = __p_sections;

  while(ptr != 0)
  {
    Section* temp = ptr->next;
    delete ptr;
    ptr = temp;
  }
}

Profile::Profile(const Profile& rv)
:__props(rv.__props),__p_sections(0)
{
  Section* ptr = rv.__p_sections;
  while(ptr!=0)
  {
    copySection(ptr);
    ptr = ptr->next;
  }  
}

Profile& Profile::operator=(const Profile& rv)
{
  if(&rv == this)
  {
    return *this;
  } 

  __props = rv.__props;

  Section* ptr = __p_sections;
  while(ptr!=0)
  {
    Section* temp=ptr->next;
    delete ptr;
    ptr=temp;
  }
  __p_sections = 0;

  ptr = rv.__p_sections;
  while(ptr!=0)
  {
    copySection(ptr);
    ptr = ptr->next;
  }

  return *this;
}

Profile::Profile()
{ 
  __p_sections = 0;
}

Profile::Profile(const string& fileName)
{  
  __p_sections = 0;

  append(fileName);
}

void Profile::append(const Profile& rv)
{
  vector<string> keys1;
  rv.keys(keys1); 
  for(size_t i=0;i<keys1.size();++i)
  {
    setProperty(keys1[i],rv.getProperty(keys1[i]));  
  }

  vector<string> sections1;
  rv.sections(sections1);
  
  for( size_t i = 0; i < sections1.size(); ++i)
  {
    if(! containsSection(sections1[i]) )
    {
      createSection(sections1[i]);
    }

    keys1.clear();
    rv.keys(sections1[i],keys1);
    for(size_t j=0;j<keys1.size();++j)
    {
      setProperty(sections1[i],keys1[j],rv.getProperty(sections1[i],keys1[j]));
    }

  }
}

void Profile::append(const string& fileName)
{
  if(fileName.empty())
  {
    return;
  }

  Properties* pTable=&__props;
  
  FILE* fp = fopen(fileName.c_str(),"r");

  if(fp == 0)
  {
    LOGGING_WARN("Profile::<init>|file not found : " << fileName);
    return;
  }
  
  while(true)
  {
    char line[OPENCODE_MAXLINE + 1] = {0};
    if( LineFile::readLine(fp,line,OPENCODE_MAXLINE) <= 0)
    {
      break;
    }

    //Find start of Section    
    if(line[0] == '[' && strlen(line) > 2)
    {
      char* sectionStopIndex = strchr(line,']');
    
      if(sectionStopIndex != 0)
      {
        *sectionStopIndex ='\0';
        
        Section* ptr = __p_sections;
        Properties* props_tmp = 0;
        
        string __section_name(line + 1);
        __section_name = LineFile::loadConvert(__section_name);
          
        while(ptr != 0)
        {
          if(__section_name == ptr->sectionName)
          {
             props_tmp = &(ptr->props);
             break;
          }
          ptr = ptr->next;
        }

        if(props_tmp==0)
        {
          Section* pair=new Section(__section_name);
          pTable=&(pair->props);   
                   
          pair->next=__p_sections;
          __p_sections=pair;
        }   
      }//~end if(sectionStopIndex != 0)
      
      continue;
    
    }//~end if(line[0] == '[' && strlen(line) > 2)
                              
    size_t len = strlen(line);
                
    // Find start of key
    size_t keyStart;
    for(keyStart=0; keyStart<len; keyStart++) 
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
    for(separatorIndex=keyStart; separatorIndex<len; separatorIndex++) 
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
    size_t valueIndex;
    for (valueIndex=separatorIndex; valueIndex<len; valueIndex++)
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

    // Skip over white space after rv separators if any
    while (valueIndex < len) 
    {
      if (strchr(LineFile::whiteSpaceChars,line[valueIndex]) == 0)
      { 
        break;
      }
      valueIndex++;
    }

    if(valueIndex>=len)
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

    pTable->setProperty(key, value);

  }//~end while(true)
  
  fclose(fp);
}

const string& Profile::getProperty(const string& key) const
{
  return __props.getProperty(key);
}

const string& Profile::getProperty(const string& section,const string& key) const
{
  Section* ptr=__p_sections;
  Properties* pTable=0;
  
  while(ptr!=0)
  {
    if(ptr->sectionName == section)
    {
      pTable=&(ptr->props);
      break;
    }
    ptr=ptr->next;
  }
    
  if(pTable!=0)
  {
    return pTable->getProperty(key);
  }
  
  return VALUE_NULL;
}


void Profile::removeProperty(const string& section,const string& key)
{
  Section* ptr=__p_sections;
  Properties* pTable=0;

  while(ptr!=0)
  {
    if(ptr->sectionName == section)
    {
      pTable=&(ptr->props);
      break;
    }
    ptr=ptr->next;
  }

  if(pTable!=0)
  {
    pTable->getProperty(key);
  }
}

void Profile::removeProperty(const string& key)
{
  __props.removeProperty(key);
}

void Profile::setProperty(const string& key,const string& value)
{
  __props.setProperty(key,value);
}

void Profile::setProperty(const string& section,const string& key,const string& value)
{
  Section* ptr=__p_sections;
  Properties* pTable=0;
  
  while(ptr!=0)
  {
    if(ptr->sectionName == section)
    {
      pTable=&(ptr->props);
      break;
    }
    ptr=ptr->next;
  }
   
  if(pTable!=0)
  {
    pTable->setProperty(key,value);
  }
}

void Profile::dump(StringBuffer& __sb)
{ 
  __props.dump(__sb);
  
  Section* ptr=__p_sections;
  Properties* pTable=0;
  
  while(ptr!=0)
  {
    pTable=&(ptr->props);

    __sb << "[" << ptr->sectionName << "]" << File::lineSeparator;
    pTable->dump(__sb);
    ptr=ptr->next;
  }
  
}

void  Profile::keys(vector<string>& propKeys) const
{
  __props.propertyNames(propKeys);
}

void Profile::keys(const string& section,vector<string>& propKeys) const
{
  Section* ptr=__p_sections;
  Properties* pTable=0;

  while(ptr!=0)
  {
    if(ptr->sectionName == section)
    {
      pTable=&(ptr->props);
      break;
    }
    ptr=ptr->next;
  }
 
  if(pTable != 0)
  {
    pTable->propertyNames(propKeys);
  }
}

void Profile::sections(vector<string>& propSections) const
{
  Section*   pSection = __p_sections;;
  while(pSection != 0)
  {
    propSections.push_back(pSection->sectionName);

    pSection = pSection->next;
  }
}

const Properties& Profile::getProperties(const string& section) const
{
  const Properties* __p_props = &__props;
  
  Section* ptr = __p_sections;
  while(ptr != 0)
  {
    if(ptr->sectionName == section)
    {
      __p_props = &(ptr->props);
      break;
    }
    ptr=ptr->next;
  }   
  
  return *__p_props;
}

const Properties& Profile::getProperties() const
{
  return __props;
}  
bool Profile::containsSection(const string& section) const
{
  Section* ptr=__p_sections;
  while(ptr!=0)
  {
    if(ptr->sectionName == section)
    {
      return true;
    }
    ptr=ptr->next;
  }  
  
  return false;
}

void Profile::createSection(const string& section)
{
  if(containsSection(section))
  {
    return;
  }

  Section* pair = new Section(section);

  pair->next = __p_sections;
  __p_sections = pair;
}

void Profile::copySection(Section* section)
{
  Section* pair = new Section(section->sectionName);
  pair->props = section->props;

  pair->next = __p_sections;
  __p_sections = pair;
}

__OPENCODE_END_NAMESPACE
