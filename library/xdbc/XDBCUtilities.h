/**
 *  XDBCUtilities.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef   __OPENCODE_XDBC_UTILITIES_H__
#define   __OPENCODE_XDBC_UTILITIES_H__

#include "xdbc/xdefine.h"

#include "xdbc/connection.h"
#include "xdbc/statement.h"
#include "xdbc/resultset.h"

#include "port/date.h"
#include "port/time.h"
#include "port/timestamp.h"

__OPENCODE_BEGIN_NAMESPACE

class __OPENCODE_XDBC_API_DECL XDBCUtilities
{
public:
  static string  parseURL(__XDBC_ATTRIBUTES& props,const string& url,bool doChange = true); 

public:
  static void setDate(const XDBC_DATE& date,Date& rv);
  static void setTime(const XDBC_TIME& time,Time& rv);  
  static void setTimestamp(const XDBC_TIMESTAMP& timestamp,Timestamp& rv); 

  static void setSQLDate(const Date& date,XDBC_DATE& rv);
  static void setSQLTime(const Time& time,XDBC_TIME& rv);  
  static void setSQLTimestamp(const Timestamp& timestamp,XDBC_TIMESTAMP& rv); 
    
public:
  static string toString(const XDBC_DATE& date);
  static string toString(const XDBC_TIME& time);
  static string toString(const XDBC_TIMESTAMP& timestamp);
  static string toString(const XDBC_INTERVAL& interval);
  static string toString(const XDBC_NUMERIC& numberic);
};
  
__OPENCODE_END_NAMESPACE

#endif
