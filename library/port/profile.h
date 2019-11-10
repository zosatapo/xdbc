/** 
 *  profile.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef   __OPENCODE_PROFILE_H__
#define   __OPENCODE_PROFILE_H__

#include "port/libdefs.h"
#include "port/ioc_sbuf.h"
#include "port/properties.h"

__OPENCODE_BEGIN_NAMESPACE

class Section;
class Profile;

class __OPENCODE_CORE_API_DECL Section
{
public:
  Section():next(0) {}
  Section(const string& name):sectionName(name),next(0){}
private:
  string sectionName;

  Properties props;
  Section* next;

public:  
  friend class Profile;
};


class __OPENCODE_CORE_API_DECL Profile
{
public:
  ~Profile();

  Profile();
  Profile(const string& fileName);

  Profile(const Profile& rv);
  Profile& operator=(const Profile& rv);

public:
  void append(const string& fileName);
  void append(const Profile& rv);

public:
  const string& getProperty(const string& key) const;  
  const string& getProperty(const string& section,const string& key) const;

  void   setProperty(const string& key,const string& value);
  void   setProperty(const string& section,const string& key,const string& value);

  void  removeProperty(const string& key);
  void  removeProperty(const string& section,const string& key);

  const Properties& getProperties(const string& section) const; 
  const Properties& getProperties() const; 
  
  void  keys(vector<string>& propKeys) const;  
  void  keys(const string& section,vector<string>& propKeys) const;
  void  sections(vector<string>& propSections) const;

public:
  bool  containsSection(const string& section) const;
  void  createSection(const string& section);
  void  dump(StringBuffer& __sb);
  
private:
  void copySection(Section* section);

private:
  Section* __p_sections;
  Properties __props;
  
  string VALUE_NULL;
};
__OPENCODE_END_NAMESPACE

#endif
