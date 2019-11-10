#ifndef  __OPENCODE_GLOBALS_CONFIG_H__
#define  __OPENCODE_GLOBALS_CONFIG_H__

/** 
 *  PORT_YYY  : OPENCODE_GLOBAL_PLATFORM_YYY 
 *
 *  PORT_AIX 
 *  PORT_HPUX
 *  PORT_LINUX
 *  PORT_WINDOWS
 */
#ifdef PORT_WINDOWS
#  include "port/config_msvc.h"
#else
#  include "port/config_auto.h"
#endif
#include "port/ioc_H_config.h"

#if defined(PORT_WINDOWS)
#  include <windows.h>
#endif

#include <sys/types.h>
#if !defined(PORT_AIX) && !defined(PORT_WINDOWS)
#  include <stdint.h>
#endif

//-------------------------------------------------------------------------
// Operation System  Features Support
//-------------------------------------------------------------------------
#if defined(PORT_HPUX)
#  if !defined(__ia64)
#    define PORT_HPUX_PARISC
#  endif
#endif

//-------------------------------------------------------------------------
// Source Port Features Support
//-------------------------------------------------------------------------
#define __OPENCODE_NATIVE_CODE

#if defined(PORT_WINDOWS)
#  define __OPENCODE_EXPORT_DECL  __declspec(dllexport)
#  define __OPENCODE_IMPORT_DECL  __declspec(dllimport)
#else
#  define __OPENCODE_EXPORT_DECL
#  define __OPENCODE_IMPORT_DECL
#endif

#define  __OPENCODE_INLINE   inline
//-------------------------------------------------------------------------
// Namespace Features Support
//-------------------------------------------------------------------------

# if defined(OPENCODE_HAS_NAMESPACES) && !defined(OPENCODE_DISABLE_NAMESPACES)
#   define OPENCODE_NS opencode
#   define __OPENCODE_BEGIN_NAMESPACE namespace OPENCODE_NS {
#   define __OPENCODE_END_NAMESPACE }
#   define USING_OPENCODE_NS using namespace OPENCODE_NS;
# else
#   define OPENCODE_NS
#   define __OPENCODE_BEGIN_NAMESPACE
#   define __OPENCODE_END_NAMESPACE
#   define USING_OPENCODE_NS
# endif

#define OPENCODE_STL_NS std
#define USING_OPENCODE_STL_NS  using namespace std;

//-------------------------------------------------------------------------
// Exception Handling Features Support
//-------------------------------------------------------------------------
#define __OPENCODE_THROW(stmt)         throw stmt
#define __OPENCODE_TRY                 try
#define __OPENCODE_CATCH(stmt)         catch(stmt)

#define __OPENCODE_RETHROW             throw
#define __OPENCODE_NOTHROW             throw()
#define __OPENCODE_CATCH_ALL           catch(...)
#define __OPENCODE_UNWIND(action)      catch(...){ action; __OPENCODE_RETHROW; }

#define __OPENCODE_THROW_DECLARE(...)    throw(__VA_ARGS__)

#define __OPENCODE_ASSERT(__oc_expr) \
if (!(__oc_expr))\
{\
  fprintf(stderr, "%s:%d OPENCODE assertion failure: %s\n", __FILE__, __LINE__, #__oc_expr);\
  abort();\
}

#define __OPENCODE_CHECK(__oc_expr,__oc_action,__oc_return)\
{\
  if(!(__oc_expr))\
  {\
    __oc_action;\
    __oc_return;\
  }\
}

#endif /* __OPENCODE_GLOBALS_CONFIG_H__ */

