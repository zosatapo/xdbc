#ifndef __OPENCODE_PORTABLE_DLLIBRARY_H__
#define __OPENCODE_PORTABLE_DLLIBRARY_H__

#include "port/ioc_G_config.h"

#if defined(PORT_WINDOWS)
#  define OPENCODE_TYPE_SOLIBRARY  HMODULE
#  define OPENCODE_TYPE_SOSYMBOL   void*
#elif defined(PORT_HPUX) && defined(OPENCODE_HAS_SHL_LOAD)
#  include <dl.h>
#  if defined(OPENCODE_HAS_CXXSHL_LOAD)
#    include <cxxdl.h>
#  else
     extern "C" shl_t cxxshl_load(const char *path, int flags, long address);
     extern "C" int   cxxshl_unload(shl_t handle);
#  endif
#  define OPENCODE_TYPE_SOLIBRARY  shl_t
#  define OPENCODE_TYPE_SOSYMBOL   void*
#else
#  include <dlfcn.h>
#  define OPENCODE_TYPE_SOLIBRARY  void*
#  define OPENCODE_TYPE_SOSYMBOL   void*
#endif

#endif



