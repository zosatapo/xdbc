/** 
 *  properties.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
 
#ifndef __OPENCODE_PROPERTY_H__
#define __OPENCODE_PROPERTY_H__

#include "port/libdefs.h"
#include "port/ioc_sbuf.h"
#include "port/ioc_map.h" 
#include "port/ioc_vector.h"
  
__OPENCODE_BEGIN_NAMESPACE

class __OPENCODE_CORE_API_DECL Properties
{
public:
  ~Properties();
  
  Properties();  
  Properties(const string& fileName);  
  Properties(const Properties& rv);
  Properties& operator=(const Properties& rv);
 
  void append(const string& fileName);  
  void append(const Properties& rv);
    
public:
  const string& getProperty(const string& key) const;
  string getProperty(const string& key,const string& defaultValue) const;
  
  void setProperty(const string& key,const string& value);
  void removeProperty(const string& key);
  
  void propertyNames(vector<string>& names) const;  
  void dump(StringBuffer& sb) const;
    
private:
  OPENCODE_MAP<string,string> props;
  string VALUE_NULL;
};

__OPENCODE_END_NAMESPACE

#endif
