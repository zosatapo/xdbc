#ifndef  __OPENCODE_CONFIG_BASE_H__
#define  __OPENCODE_CONFIG_BASE_H__

#include "port/ioc_G_config.h"
#include "port/ioc_types.h" 
#include "port/ioc_class.h"

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------

#ifdef OPENCODE_BUILD_CORE_DLL
#  define __OPENCODE_CORE_API_DECL __OPENCODE_EXPORT_DECL
#else
# define __OPENCODE_CORE_API_DECL  __OPENCODE_IMPORT_DECL
#endif

__OPENCODE_BEGIN_NAMESPACE

//-----------------------------------------------------------------------------
// 1-1  BASE LIFETIME LONGEVITY
//-----------------------------------------------------------------------------
#define __BASE_LONGEVITY_MAX                 (999999999)
#define __BASE_LONGEVITY_MIN                 (999999000)
                                             
#define __BASE_LONGEVITY_FS                  (__BASE_LONGEVITY_MAX - 10)
#define __BASE_LONGEVITY_SYSTEM              (__BASE_LONGEVITY_MAX - 15 )
                                             
#define __BASE_LONGEVITY_ZIF                 (__BASE_LONGEVITY_MAX - 20)
#define __BASE_LONGEVITY_LOCALE              (__BASE_LONGEVITY_MAX - 25)
#define __BASE_LONGEVITY_TZ                  (__BASE_LONGEVITY_MAX - 25)
                                             
__OPENCODE_END_NAMESPACE

#endif
