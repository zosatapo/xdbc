#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

dnl Statically (i.e. at autoconf-time) determine the version of OPENCODE.
dnl This is necessary since the version argument to AC_INIT is
dnl supposed to be a static value, not a dynamic one (e.g. a shell
dnl variable).
dnl
dnl Note that this macro removes the newline output by the M4
dnl "esyscmd" built-in.  Unless you understand what you're doing,
dnl particularly with M4, do not modify this macro definition.
define([OPENCODE_VERSION], patsubst(esyscmd(grep OPENCODE_VERSION library/port/opencode.h | sed 's/.*\" *\(.*\)\".*/\1/'), [
]))dnl remove newline ending every `esyscmd' answer

AC_INIT([opencode],[OPENCODE_VERSION],[dertyang@hotmail.com])

AC_PREREQ(2.59)
AC_CANONICAL_TARGET([])
AM_INIT_AUTOMAKE

AC_CONFIG_HEADER([library/port/config_auto.h])
AC_CONFIG_SRCDIR([library/port/libdefs.cpp])
AC_CONFIG_MACRO_DIR([m4])

# Disable building of static libraries by default
AC_DISABLE_STATIC

# Check environment options
OPENCODE_ENABLE_DEBUG
OPENCODE_ENABLE_LP64

OPENCODE_ENABLE_EH4SOCKET
OPENCODE_ENABLE_EH4XDBC

OPENCODE_WITH_ODBCOPTIONS

# Checks for programs 
AC_LANG([C++])
AC_PROG_CXX([aCC xlC_r CC g++])
OPENCODE_PROG_CXX
AC_PROG_CXXCPP
AC_PROG_LD
AC_PROG_NM
OPENCODE_PROG_LIBTOOL
AC_PROG_LIBTOOL
AC_PROG_INSTALL
AC_PROG_MAKE_SET

# Checks for libraries 
OPENCODE_CHECK_XDBC

# Checks for header files.
OPENCODE_CHECK_STL_HASHMAP

# Checks for typedefs, structures, and compiler characteristics.
AC_C_BIGENDIAN
OPENCODE_CHECK_TYPES

# Checks for library functions.
AC_FUNC_MKTIME
AC_FUNC_STAT
AC_FUNC_STRERROR_R
AC_FUNC_STRFTIME
AC_FUNC_STRTOD
AC_CHECK_FUNCS([atexit gettimeofday clock_gettime setenv strcasestr strcasecmp strerror strtof strtoll strtol])

AC_CONFIG_FILES([Makefile
     library/Makefile
     library/port/Makefile
     library/socket/Makefile
     library/xdbc/Makefile
     xdbcImpl/Makefile
     xdbcImpl/informix/Makefile
     xdbcImpl/informix/xdbc/Makefile
     xdbcImpl/oracle/Makefile
     xdbcImpl/oracle/xdbc/Makefile
     xdbcImpl/mysql/Makefile
     examples/Makefile
     examples/port/Makefile
     examples/socket/Makefile
     examples/xdbc/Makefile
     msvc/Makefile
     msvc/library/Makefile	
     msvc/xdbcImpl/Makefile
     msvc/examples/Makefile])
AC_OUTPUT

