#ifndef __OPENCODE_PORTABLE_FILE_H__
#define __OPENCODE_PORTABLE_FILE_H__

#include "port/ioc_G_config.h"

#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>


#if defined(PORT_WINDOWS)
#  include <direct.h>
#else
#  include <dirent.h>
#endif

#if defined(PORT_HPUX)
# include <sys/pstat.h>
#endif

#  define  FILE_ATTR_EXISTS      0x01
#  define  FILE_ATTR_REGULAR     0x02
#  define  FILE_ATTR_DIRECTORY   0x04
#  define  FILE_ATTR_LINK        0x08
#  define  FILE_ATTR_HIDDEN      0x10 

#if defined(PORT_WINDOWS)
#  define  FILE_SEPARATORCHAR      '\\'
#  define  FILE_SEPARATOR          "\\"
#  define  FILE_PATHSEPARATORCHAR  ';'
#  define  FILE_PATHSEPARATOR      ";"
#  define  FILE_LINESEPARATOR      "\r\n"
#else
#  define  FILE_SEPARATORCHAR      '/'
#  define  FILE_SEPARATOR          "/"
#  define  FILE_PATHSEPARATORCHAR  ':'
#  define  FILE_PATHSEPARATOR      ":"
#  define  FILE_LINESEPARATOR      "\n"
#endif


#if defined(PORT_WINDOWS)
#  define  OPENCODE_PATH_MAX       MAX_PATH
#else
#  define  OPENCODE_PATH_MAX       PATH_MAX
#endif

#endif

