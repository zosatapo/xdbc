#ifndef __OPENCODE_PORTABLE_CORE_H__
#define __OPENCODE_PORTABLE_CORE_H__

#include "port/ioc_G_config.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#if !defined(PORT_WINDOWS)
#  include <unistd.h>
#  include <sys/wait.h>
#endif

#endif


