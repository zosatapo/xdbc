#ifndef __OPENCODE_PORTABLE_SYSINFO_H__
#define __OPENCODE_PORTABLE_SYSINFO_H__

#include "port/ioc_G_config.h"

#include <locale.h>

#if   defined(PORT_WINDOWS)
#elif defined(PORT_HPUX)
#  include <langinfo.h>
#  include <sys/utsname.h>
#  include <pwd.h>
#elif defined(PORT_AIX)
#  include <langinfo.h>
#  include <sys/procfs.h>
#  include <sys/utsname.h>
#  include <pwd.h>
#elif defined(PORT_SUN_SOLARIS)
#  include <langinfo.h>
#  include <procfs.h>
#  include <sys/utsname.h>
#  include <pwd.h>
#else
#  include <langinfo.h>
#  include <sys/sysinfo.h>
#  include <sys/utsname.h>
#  include <pwd.h>
#endif



#endif


