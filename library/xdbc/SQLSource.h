/**
 *  SQLSourceImpl.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef   __OPENCODE_XDBC_SQLSOURCE_H__
#define   __OPENCODE_XDBC_SQLSOURCE_H__

#include "xdbc/xdefine.h"
#include "port/lifetime.h"
#include "port/ioc_map.h" 
#include "port/ioc_vector.h"

__OPENCODE_BEGIN_NAMESPACE

#define SQLDATA_SQLID_NULL   (-1)

class __OPENCODE_XDBC_API_DECL SQLData
{
public:
  SQLData();
  SQLData(const string& SQL);
  SQLData(const SQLData& rv);
  SQLData& operator=(const SQLData& rv);

public:
  bool   isNull() const;
  string toString() const;
  
public:
  string raw_SQL;
  string SQL;
  int    SQLID; 
   
  int    paramCount;  
  bool   pooled;
  string dbname;   
};

class __OPENCODE_XDBC_API_DECL SQLSource
{
  DECLARE_LIFETIME_BEGIN(SQLSource)
  DECLARE_LIFETIME_END(SQLSource)
  
public: 
  ~SQLSource();
  SQLSource();

public:  
  const SQLData& getSQLData(const string& key) const; 
  const SQLData& getSQLData(int SQLID) const; 
  void  keys(vector<string>& sqlkeys) const;
  
private:  
  void loadSource(const __XDBC_ATTRIBUTES& props);
  void parseSQLData(const string& key,const string& sql);

  OPENCODE_MAP<string,SQLData> __SQL_map;
  OPENCODE_MAP<int,SQLData>    __SQLID_map; 
   
  SQLData  __SQLData_NULL;
};

typedef SingletonHolder<SQLSource,CreateUsingNew<SQLSource>,SingletonWithLongevity<SQLSource>,MultiThread<SQLSource> > SQLSourceImpl;
__OPENCODE_INLINE unsigned int getLongevity(SQLSource* sqlsource) {return __XDBC_LONGEVITY_SQLSOURCE;}

__OPENCODE_END_NAMESPACE

#endif

