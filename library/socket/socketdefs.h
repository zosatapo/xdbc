/** 
 *  socketdefs.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef __OPENCODE_SOCKETDEFS_H__
#define __OPENCODE_SOCKETDEFS_H__

#ifdef OPENCODE_BUILD_NET_DLL
#  define __OPENCODE_NET_API_DECL  __OPENCODE_EXPORT_DECL
#else
# define __OPENCODE_NET_API_DECL   __OPENCODE_IMPORT_DECL
#endif

#include "port/ioc_G_config.h"
#include "port/ioc_types.h" 
#include "port/ioc_class.h"
#include "port/ioc_socket.h" 

__OPENCODE_BEGIN_NAMESPACE

//-------------------------------------------------------------------------
// 1-2  NET LIFETIME LONGEVITY
//-------------------------------------------------------------------------
#define __SOCKET_LONGEVITY_MAX                (999998999)
#define __SOCKET_LONGEVITY_MIN                (999998000)

#define __SOCKET_LONGEVITY_NETWORK            (__SOCKET_LONGEVITY_MAX - 10)

#ifdef  OPENCODE_DISABLE_NET_SEH
#define __NET_HOOK_DEFINE()      SocketException  __net_hook;  
#define __NET_HOOK_RESET()       __net_hook.reset();      
#define __NET_HOOK_DECLARE_ONCE  SocketException& __net_hook
#define __NET_HOOK_DECLARE_MORE  SocketException& __net_hook,
#define __NET_HOOK_INVOKE_ONCE   __net_hook
#define __NET_HOOK_INVOKE_MORE   __net_hook,
#define __NET_HOOK_INVOKE_CHECKPASSED  (__net_hook.checkPassed())
#define __NET_HOOK_CHECK(__net_stmt_list)\
{\
  if(!__NET_HOOK_INVOKE_CHECKPASSED)\
  {\
    __net_stmt_list;\
  }\
}
#define __NET_THROW_DECLARE(...)
#define __NET_HOOK_RETURN(__net_return,__net_type,...)\
{\
  __net_hook.setException(__VA_ARGS__);\
  __net_return;\
}
#else
#define __NET_HOOK_DEFINE()
#define __NET_HOOK_RESET()

#define __NET_HOOK_DECLARE_ONCE
#define __NET_HOOK_DECLARE_MORE
#define __NET_HOOK_INVOKE_ONCE
#define __NET_HOOK_INVOKE_MORE
#define __NET_HOOK_INVOKE_CHECKPASSED false
#define __NET_HOOK_CHECK(__net_stmt_list)

#define __NET_THROW_DECLARE(...) __OPENCODE_THROW_DECLARE(__VA_ARGS__)
#define __NET_HOOK_RETURN(__net_return,__net_type,...)\
{\
  __OPENCODE_THROW(__net_type(__VA_ARGS__));\
  __net_return;\
}
#endif

__OPENCODE_END_NAMESPACE

#endif
