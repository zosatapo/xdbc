/**
 *  sqltypes.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 *
 * This file defines the types used in ODBC
 *
 * Created for 3.51 specification
 *
 */
 
#ifndef __SQLTYPES_H__
#define __SQLTYPES_H__

/* if ODBCVER is not defined, assume version 3.51 */
#ifndef ODBCVER
#define ODBCVER 0x0351
#endif

#ifdef OPENCODE_HAS_WCHAR
/* 
 * Use this if you want to use the C/C++ portable definition of  a wide char, wchar_t
 *  Microsoft hardcoded a definition of  unsigned short which may not be compatible with
 *  your platform specific wide char definition.
 */
#include <wchar.h>
#endif

#ifdef __cplusplus
extern "C" {                         /* Assume C declarations for C++ */
#endif  /* __cplusplus */

/* 
 * NOTE: The Microsoft unicode define is only for apps that want to use TCHARs and 
 *  be able to compile for both unicode and non-unicode with the same source.
 *  This is not recommanded for linux applications and is not supported
 *  by the standard linux string header files.
 */
#ifdef OPENCODE_HAS_WCHAR
typedef wchar_t             SQLWCHAR;
#else
typedef signed short        SQLWCHAR;
#endif

/****************************
 * standard SQL* data types. use these as much as possible when using ODBC calls/vars
 ***************************/
#if (ODBCVER >= 0x0300)
  typedef void *                SQLHANDLE; 
  typedef SQLHANDLE             SQLHENV;
  typedef SQLHANDLE             SQLHDBC;
  typedef SQLHANDLE             SQLHSTMT;
  typedef SQLHANDLE             SQLHDESC;
#else
  typedef void *                SQLHENV;
  typedef void *                SQLHDBC;
  typedef void *                SQLHSTMT;
  typedef void *                SQLHANDLE; 
#endif

typedef void*                   SQLPOINTER;
/* placehold for future O/S GUI window handle definition */
typedef SQLPOINTER              SQLHWND;
                               
typedef double                  SQLDOUBLE;
typedef double                  SQLFLOAT;
typedef float                   SQLREAL;
  
#if (ODBCVER >= 0x0300)
  typedef unsigned char         SQLDATE;
  typedef unsigned char         SQLTIME;
  typedef unsigned char         SQLTIMESTAMP;
  typedef unsigned char         SQLVARCHAR;
  typedef unsigned char         SQLDECIMAL;
#endif

typedef unsigned char           SQLCHAR;
typedef signed char             SQLSCHAR;
                                                              
typedef short int               SQLSMALLINT;
typedef unsigned short int      SQLUSMALLINT;

typedef int                     SQLINTEGER;
typedef unsigned int            SQLUINTEGER;

/*
** Use the predefined platform-specific 64-bit macros here so
** that the correct definitions are picked up automatically when
** compiling end-user applications.
*/

#if defined(OPENCODE_LP64)
  typedef signed long           SQLLEN;
  typedef unsigned long         SQLULEN;
  #define SQLSETPOSIROW         SQLUINTEGER
#else                           
  #define SQLLEN                SQLINTEGER
  #define SQLULEN               SQLUINTEGER
  #define SQLSETPOSIROW         SQLUSMALLINT
//----------------------------------------------------
// ONLY USED BY XDBC-ODBC
//---------------------------------------------------- 
  #ifndef SQLLEN_IS_INTEGER
  #define SQLLEN_IS_INTEGER
  #endif
#endif  
                                
#define SQLROWSETSIZE           SQLUINTEGER                                                     
#define SQLROWOFFSET            SQLLEN
#define SQLROWCOUNT             SQLULEN
                                                                                            
typedef SQLSMALLINT             SQLRETURN;
                                
/****************************
 * standard structs for working with date/times
 ***************************/
typedef struct tagDATE_STRUCT
{
  SQLSMALLINT    year;
  SQLUSMALLINT   month;
  SQLUSMALLINT   day;
} DATE_STRUCT;

#if (ODBCVER >= 0x0300)
  typedef DATE_STRUCT SQL_DATE_STRUCT;
#endif

typedef struct tagTIME_STRUCT
{
  SQLUSMALLINT   hour;
  SQLUSMALLINT   minute;
  SQLUSMALLINT   second;
} TIME_STRUCT;

#if (ODBCVER >= 0x0300)
  typedef TIME_STRUCT SQL_TIME_STRUCT;
#endif

typedef struct tagTIMESTAMP_STRUCT
{
  SQLSMALLINT    year;
  SQLUSMALLINT   month;
  SQLUSMALLINT   day;
  SQLUSMALLINT   hour;
  SQLUSMALLINT   minute;
  SQLUSMALLINT   second;
  SQLUINTEGER    fraction;
} TIMESTAMP_STRUCT;

#if (ODBCVER >= 0x0300)
  typedef TIMESTAMP_STRUCT  SQL_TIMESTAMP_STRUCT;
#endif

#if (ODBCVER >= 0x0300)
typedef enum
{
  SQL_IS_YEAR           = 1,
  SQL_IS_MONTH          = 2,
  SQL_IS_DAY            = 3,
  SQL_IS_HOUR           = 4,
  SQL_IS_MINUTE         = 5,
  SQL_IS_SECOND         = 6,
  SQL_IS_YEAR_TO_MONTH      = 7,
  SQL_IS_DAY_TO_HOUR        = 8,
  SQL_IS_DAY_TO_MINUTE      = 9,
  SQL_IS_DAY_TO_SECOND      = 10,
  SQL_IS_HOUR_TO_MINUTE     = 11,
  SQL_IS_HOUR_TO_SECOND     = 12,
  SQL_IS_MINUTE_TO_SECOND     = 13
} SQLINTERVAL;

#endif

#if (ODBCVER >= 0x0300)
typedef struct tagSQL_YEAR_MONTH
{
  SQLUINTEGER   year;
  SQLUINTEGER   month;
} SQL_YEAR_MONTH_STRUCT;

typedef struct tagSQL_DAY_SECOND
{
  SQLUINTEGER   day;
  SQLUINTEGER   hour;
  SQLUINTEGER   minute;
  SQLUINTEGER   second;
  SQLUINTEGER   fraction;
} SQL_DAY_SECOND_STRUCT;

typedef struct tagSQL_INTERVAL_STRUCT
{
  SQLINTERVAL   interval_type;
  SQLSMALLINT   interval_sign;
  union 
  {
    SQL_YEAR_MONTH_STRUCT   year_month;
    SQL_DAY_SECOND_STRUCT   day_second;
  } intval;

} SQL_INTERVAL_STRUCT;

#endif

#define SQL_MAX_NUMERIC_LEN 16
typedef struct tagSQL_NUMERIC_STRUCT
{
  SQLCHAR        precision;
  SQLSCHAR       scale;
  SQLCHAR        sign;/* 1 if positive, 0 if negative */
  SQLCHAR        val[SQL_MAX_NUMERIC_LEN];
} SQL_NUMERIC_STRUCT;

#if (ODBCVER >= 0x0200)
typedef unsigned int            BOOKMARK;
#endif  /* ODBCVER >= 0x0200 */

/*
 * Definitions for 64-bit data types
 */
#if defined(OPENCODE_LP64)
typedef signed   long           SQLBIGINT;
typedef unsigned long           SQLUBIGINT;
#else                           
typedef signed   long long      SQLBIGINT;
typedef unsigned long long      SQLUBIGINT;
#endif                          

#ifdef __cplusplus
}       /* End of extern "C" { */
#endif  /* __cplusplus */

#endif /* #ifndef __SQLTYPES_H__ */
