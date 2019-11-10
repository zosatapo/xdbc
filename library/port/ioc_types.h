#ifndef  __OPENCODE_PORTABLE_TYPES_H__
#define  __OPENCODE_PORTABLE_TYPES_H__

#include "port/ioc_G_config.h" 
#include "port/ioc_core.h" 

//-------------------------------------------------------------------------
// 1. Basic Supplimental Types For Multi-Platform Support 
//-------------------------------------------------------------------------

#if defined(PORT_WINDOWS)
	typedef          __int8    XINT8;
	typedef unsigned __int8    UXINT8;
	typedef          __int16   XINT16;
	typedef unsigned __int16   UXINT16;
	typedef          __int32   XINT32;
	typedef unsigned __int32   UXINT32;
	typedef          __int64   XINT64;
	typedef unsigned __int64   UXINT64;
#else
	typedef           int8_t   XINT8;
	typedef          uint8_t   UXINT8;
	typedef          int16_t   XINT16;
	typedef         uint16_t   UXINT16;
	typedef          int32_t   XINT32;
	typedef         uint32_t   UXINT32;
	typedef          int64_t   XINT64;
	typedef          uint64_t  UXINT64;		
#endif
   
//-------------------------------------------------------------------------
// 2. XLONG  Types
//-------------------------------------------------------------------------
#if defined(PORT_WINDOWS)
#  if (defined(_WIN64) || defined(WIN64))
#    define OPENCODE_WIN64	
#  endif	
#else
#  if OPENCODE_SIZEOF_LONG == 8
#    if defined(__LP64__) || defined(_LP64) || defined(__64BIT__) || defined(__ia64) || defined(__alpha)
#      define OPENCODE_LP64
#    endif
#  endif
#endif

#ifdef OPENCODE_LP64  
#  define XLONG                                 long
#  define UXLONG                                unsigned long

#  define XLONG_MAX                             9223372036854775807L
#  define XLONG_MIN                             (-XLONG_MAX - 1L)
#  define UNSIGNED_XLONG_MAX                    18446744073709551615UL 

#  define XLONG_LITERAL(n)                      n##L
#  define UNSIGNED_XLONG_LITERAL(n)             n##UL

#  define XLONG_FORMAT_SPECIFIER                "%ld"
#  define UNSIGNED_XLONG_FORMAT_SPECIFIER       "%lu"

#else                               
#  define XLONG                                 long long
#  define UXLONG                                unsigned long long

#  define XLONG_MAX                             9223372036854775807LL
#  define XLONG_MIN                             (-XLONG_MAX - 1LL)
#  define UNSIGNED_XLONG_MAX                    18446744073709551615ULL

#  define XLONG_LITERAL(n)                      n##LL
#  define UNSIGNED_XLONG_LITERAL(n)             n##ULL

#  define XLONG_FORMAT_SPECIFIER                "%lld"
#  define UNSIGNED_XLONG_FORMAT_SPECIFIER       "%llu"
#endif
                                     
#endif
