/**
 *  xtypes.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef   __OPENCODE_XDBC_TYPES_H__
#define   __OPENCODE_XDBC_TYPES_H__

#include "xdbc/xdefine.h"

__OPENCODE_BEGIN_NAMESPACE

//------------------------------------------------------------
//XDBC UNKNOWN TYPE 
//------------------------------------------------------------

#define XDBC_TYPE_OTHER               (1111)

//------------------------------------------------------------
//XDBC  TYPE
//------------------------------------------------------------

#define XDBC_TYPE_NULL                (0)

#define XDBC_TYPE_BOOLEAN             (16)

#define XDBC_TYPE_CHAR                (1)
#define XDBC_TYPE_VARCHAR             (12)
#define XDBC_TYPE_LONGVARCHAR         (-1)

#define XDBC_TYPE_BIT                 (-7) 
#define XDBC_TYPE_TINYINT             (-6)
#define XDBC_TYPE_SMALLINT            (5)
#define XDBC_TYPE_INTEGER             (4)
#define XDBC_TYPE_BIGINT              (-5)

#define XDBC_TYPE_FLOAT               (6) 
#define XDBC_TYPE_REAL                (7)
#define XDBC_TYPE_DOUBLE              (8)
#define XDBC_TYPE_NUMERIC             (2)
#define XDBC_TYPE_DECIMAL             (3)

#define XDBC_TYPE_DATE                (91)
#define XDBC_TYPE_TIME                (92)
#define XDBC_TYPE_TIMESTAMP           (93)
#define XDBC_TYPE_INTERVAL            (94)

#define XDBC_TYPE_BLOB                (2004)
#define XDBC_TYPE_CLOB                (2005)

#define XDBC_TYPE_BINARY              (-2)
#define XDBC_TYPE_VARBINARY           (-3)
#define XDBC_TYPE_LONGVARBINARY       (-4)

//------------------------------------------------------------
//XDBC EXTENSION TYPE (NOT SUPPORTED) 
//------------------------------------------------------------

#define XDBC_TYPE_DISTINCT            (2001)
#define XDBC_TYPE_STRUCT              (2002)
#define XDBC_TYPE_ARRAY               (2003)
#define XDBC_TYPE_REF                 (2006)
#define XDBC_TYPE_DATALINK            (70)

class  __OPENCODE_XDBC_API_DECL Types 
{
public:
  /**
   * <P>The constant in the XDBC, sometimes referred
   * to as a type code, that identifies the generic SQL type 
   * <code>BIT</code>.
   */
   const static int BIT ;

  /**
   * <P>The constant in the XDBC, sometimes referred
   * to as a type code, that identifies the generic SQL type 
   * <code>TINYINT</code>.
   */
   const static int TINYINT;

  /**
   * <P>The constant in the XDBC, sometimes referred
   * to as a type code, that identifies the generic SQL type 
   * <code>SMALLINT</code>.
   */
   const static int SMALLINT;

  /**
   * <P>The constant in the XDBC, sometimes referred
   * to as a type code, that identifies the generic SQL type 
   * <code>INTEGER</code>.
   */
   const static int INTEGER ;
  
  /**
   * <P>The constant in the XDBC, sometimes referred
   * to as a type code, that identifies the generic SQL type 
   * <code>BIGINT</code>.
   */
   const static int BIGINT ;
  
  /**
   * <P>The constant in the XDBC, sometimes referred
   * to as a type code, that identifies the generic SQL type 
   * <code>FLOAT</code>.
   */
   const static int FLOAT ;
  
  /**
   * <P>The constant in the XDBC, sometimes referred
   * to as a type code, that identifies the generic SQL type 
   * <code>REAL</code>.
   */
   const static int REAL;
  
  
  /**
   * <P>The constant in the XDBC, sometimes referred
   * to as a type code, that identifies the generic SQL type 
   * <code>DOUBLE</code>.
   */
   const static int DOUBLE ;
  
  /**
   * <P>The constant in the XDBC, sometimes referred
   * to as a type code, that identifies the generic SQL type 
   * <code>NUMERIC</code>.
   */
   const static int NUMERIC ;
  
  /**
   * <P>The constant in the XDBC, sometimes referred
   * to as a type code, that identifies the generic SQL type 
   * <code>DECIMAL</code>.
   */
   const static int DECIMAL ;
  
  /**
   * <P>The constant in the XDBC, sometimes referred
   * to as a type code, that identifies the generic SQL type 
   * <code>CHAR</code>.
   */
   const static int CHAR ;
  
  /**
   * <P>The constant in the XDBC, sometimes referred
   * to as a type code, that identifies the generic SQL type 
   * <code>VARCHAR</code>.
   */
   const static int VARCHAR;
  
  /**
   * <P>The constant in the XDBC, sometimes referred
   * to as a type code, that identifies the generic SQL type 
   * <code>LONGVARCHAR</code>.
   */
   const static int LONGVARCHAR ;
  
  
  /**
   * <P>The constant in the XDBC, sometimes referred
   * to as a type code, that identifies the generic SQL type 
   * <code>DATE</code>.
   */
   const static int DATE;
  
  /**
   * <P>The constant in the XDBC, sometimes referred
   * to as a type code, that identifies the generic SQL type 
   * <code>TIME</code>.
   */
   const static int TIME;
  
  /**
   * <P>The constant in the XDBC, sometimes referred
   * to as a type code, that identifies the generic SQL type 
   * <code>TIMESTAMP</code>.
   */
   const static int TIMESTAMP;

  /**
   * <P>The constant in the XDBC, sometimes referred
   * to as a type code, that identifies the generic SQL type
   * <code>INTERVAL</code>.
   */
   const static int INTERVAL ;

  /**
   * <P>The constant in the XDBC, sometimes referred
   * to as a type code, that identifies the generic SQL type 
   * <code>BINARY</code>.
   */
   const static int BINARY ;

  /**
   * <P>The constant in the XDBC, sometimes referred
   * to as a type code, that identifies the generic SQL type 
   * <code>VARBINARY</code>.
   */
   const static int VARBINARY ;
  
  /**
   * <P>The constant in the XDBC, sometimes referred
   * to as a type code, that identifies the generic SQL type 
   * <code>LONGVARBINARY</code>.
   */
   const static int LONGVARBINARY;
  
  /**
   * <P>The constant in the XDBC, sometimes referred
   * to as a type code, that identifies the generic SQL type 
   * <code>NULL</code>.
   */
   const static int xNULL ;


  /**
   * The constant in the XDBC programming language that indicates
   * that the SQL type is database-specific 
   */
   const static int OTHER;

  /**
   * The constant in the XDBC, sometimes referred to
   * as a type code, that identifies the generic SQL type
   * <code>DISTINCT</code>.
   * 
   */
   const static int DISTINCT ;

  /**
   * The constant in the XDBC, sometimes referred to
   * as a type code, that identifies the generic SQL type
   * <code>STRUCT</code>.
   * 
   */
   const static int STRUCT;

  /**
   * The constant in the XDBC, sometimes referred to
   * as a type code, that identifies the generic SQL type
   * <code>ARRAY</code>.
   * 
   */
   const static int ARRAY ;

  /**
   * The constant in the XDBC, sometimes referred to
   * as a type code, that identifies the generic SQL type
   * <code>BLOB</code>.
   * 
   */
   const static int BLOB ;

  /**
   * The constant in the XDBC, sometimes referred to
   * as a type code, that identifies the generic SQL type
   * <code>CLOB</code>.
   * 
   */
   const static int CLOB ;

  /**
   * The constant in the XDBC, sometimes referred to
   * as a type code, that identifies the generic SQL type
   * <code>REF</code>.
   * 
   */
   const static int REF;
      
  /**
   * The constant in the XDBC, somtimes referred to
   * as a type code, that identifies the generic SQL type <code>DATALINK</code>.
   * 
   */
  const static int DATALINK ;

  /**
   * The constant in the XDBC, somtimes referred to
   * as a type code, that identifies the generic SQL type <code>BOOLEAN</code>.
   * 
   */
  const static int BOOLEAN ;
};

const char* __xdbc_getname(int xdbcTypes);

//---------------------------------------------------------------------
//     XDBC common marco  declaration
//---------------------------------------------------------------------

/* XDBC option values */
#define XDBC_FALSE           0
#define XDBC_TRUE            1

//---------------------------------------------------------------------
//     XDBC common class  declaration
//---------------------------------------------------------------------

typedef struct __xdbc_DATE
{
public:
  static void init(struct __xdbc_DATE* __x_date)
  {
    __x_date->year = 1970;
    __x_date->month = 1;
    __x_date->day  = 1;
  }

public:
  int year;
  int month;
  int day;
}XDBC_DATE;

typedef struct __xdbc_TIME
{
public:
  static void init(struct __xdbc_TIME* __x_time)
  {
    __x_time->hour = 0;
    __x_time->minute = 0;
    __x_time->second = 0;
  }

public:
  int hour;
  int minute;
  int second;
}XDBC_TIME;

typedef struct __xdbc_TIMESTAMP
{
public:
  static void init(struct __xdbc_TIMESTAMP* __x_timestamp)
  {
    __x_timestamp->year = 1970;
    __x_timestamp->month = 1;
    __x_timestamp->day = 1;
    __x_timestamp->hour = 0;
    __x_timestamp->minute = 0;
    __x_timestamp->second = 0;
    __x_timestamp->fraction = 0;
    __x_timestamp->tzOffset = 0;
  }

public:
  int   year;
  int   month;
  int   day;
  int   hour;
  int   minute;
  int   second;
  int   fraction;
  XLONG tzOffset;
}XDBC_TIMESTAMP;

typedef enum
{
  XDBC_IS_YEAR           = 1,
  XDBC_IS_MONTH          = 2,
  XDBC_IS_DAY            = 3,
  XDBC_IS_HOUR           = 4,
  XDBC_IS_MINUTE         = 5,
  XDBC_IS_SECOND         = 6,
  XDBC_IS_YEAR_TO_MONTH      = 7,
  XDBC_IS_DAY_TO_HOUR        = 8,
  XDBC_IS_DAY_TO_MINUTE      = 9,
  XDBC_IS_DAY_TO_SECOND      = 10,
  XDBC_IS_HOUR_TO_MINUTE     = 11,
  XDBC_IS_HOUR_TO_SECOND     = 12,
  XDBC_IS_MINUTE_TO_SECOND     = 13
} XDBCINTERVAL;

typedef struct __xdbc_YEAR_MONTH
{
public:
 static void init(struct __xdbc_YEAR_MONTH* __x_ym)
 {
   __x_ym->year = 0;
   __x_ym->month = 0;
 }

public:
  int   year;
  int   month;
}XDBC_YEAR_MONTH;

typedef struct __xdbc_DAY_SECOND
{
public:
  static void init(struct __xdbc_DAY_SECOND* __x_ds)
  {
    __x_ds->day = 0;
    __x_ds->hour = 0;
    __x_ds->minute = 0;
    __x_ds->second = 0;
    __x_ds->fraction = 0;
  }
   
public:
  int   day;
  int   hour;
  int   minute;
  int   second;
  int   fraction;
} XDBC_DAY_SECOND;

/**
 * The interval_type field of the XDBC_INTERVAL indicates to the application 
 * what structure is held in the union and also what members of the structure are 
 * relevant. The interval_sign field has the value XDBC_FALSE if the interval leading 
 * field is unsigned; if it is XDBC_TRUE, the leading field is negative. The value in 
 * the leading field itself is always unsigned, regardless of the value of interval_sign. 
 * The interval_sign field acts as a sign bit.
 */
typedef struct __xdbc_INTERVAL
{
public:
  static void init(struct __xdbc_INTERVAL* __x_interval)
  {
    __x_interval->interval_type = XDBC_IS_YEAR;
    __x_interval->interval_sign = XDBC_FALSE;
    __x_interval->intval.year_month.year = 0;
  }

public:
  XDBCINTERVAL   interval_type;
  int            interval_sign;
  union 
  {
    XDBC_YEAR_MONTH   year_month;
    XDBC_DAY_SECOND   day_second;
  } intval;

} XDBC_INTERVAL;


#define XDBC_MAX_NUMERIC_LEN 16
typedef struct __xdbc_NUMERIC
{
public:
 static void init(struct __xdbc_NUMERIC* __x_numeric)
 {
   memset(__x_numeric->val,0,XDBC_MAX_NUMERIC_LEN);
   __x_numeric->sign = 1;
   __x_numeric->precision = 0;
   __x_numeric->scale = 0;  
 }   
 
 static string stringValue(const struct __xdbc_NUMERIC* __x_numeric);
 static double doubleValue(const struct __xdbc_NUMERIC* __x_numeric);
 static XLONG  longValue(const struct __xdbc_NUMERIC* __x_numeric);
 
 static bool valueOf(struct __xdbc_NUMERIC* __x_numeric,const string& value); 
 static bool valueOf(struct __xdbc_NUMERIC* __x_numeric,XLONG value);

protected: 
 static bool parseNumeric(const struct __xdbc_NUMERIC* __x_numeric,UXLONG* __numval,UXLONG* __divisor);
  
public:
  char   precision;
  char   scale;
  char   sign;/* 1 if positive, 0 if negative */
  char   val[XDBC_MAX_NUMERIC_LEN];
} XDBC_NUMERIC;

__OPENCODE_END_NAMESPACE

#endif
