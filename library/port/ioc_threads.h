#ifndef __OPENCODE_PORTABLE_THREADS_H__
#define __OPENCODE_PORTABLE_THREADS_H__

#include "port/ioc_G_config.h"

#if defined(PORT_WINDOWS)
#  define OPENCODE_TYPE_MUTEX      HANDLE
#  define OPENCODE_TYPE_CONDITION  HANDLE
#  define OPENCODE_TYPE_TSDKEY     DWORD
#  define OPENCODE_TYPE_THREAD     HANDLE
#else
#  include <pthread.h>
#  include <signal.h>
#  define OPENCODE_TYPE_MUTEX      pthread_mutex_t
#  define OPENCODE_TYPE_CONDITION  pthread_cond_t
#  define OPENCODE_TYPE_TSDKEY     pthread_key_t
#  define OPENCODE_TYPE_THREAD     pthread_t
#endif

#endif



