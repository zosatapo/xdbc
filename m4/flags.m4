dnl -------------------------------------------------------------------------
dnl flags.m4
dnl
dnl OPENCODE M4 include file which contains OPENCODE specific M4 macros
dnl for checking certain os feature.
dnl
dnl -------------------------------------------------------------------------

AC_DEFUN([OPENCODE_CHECK_LP64],
[
  AC_LANG_SAVE
  AC_LANG_CPLUSPLUS

  AC_MSG_CHECKING([whether the C++ compiler ($CXX) supports LP64 option ($opencode_v_lp64_cxxflags)])
  opencode_v_saved_cxxflags="$CXXFLAGS"
  CXXFLAGS="$opencode_v_lp64_cxxflags"
  AC_TRY_COMPILE([#include <stdio.h>],
  [
    int  __opencode_check_lp64 = 8023;
  ],
  opencode_v_lp64_avaliable=yes
  ,
  opencode_v_lp64_avaliable=no
  opencode_v_lp64_cxxflags="")
  CXXFLAGS="$opencode_v_saved_cxxflags"
  AC_MSG_RESULT([$opencode_v_lp64_avaliable])

  AC_LANG_RESTORE
])

AC_DEFUN([OPENCODE_CHECK_WORDS64],
[
  AC_LANG_SAVE
  AC_LANG_CPLUSPLUS

  AC_CHECK_SIZEOF(long)

  AC_MSG_CHECKING([whether the platform is 64 bit])
  if test "$ac_cv_sizeof_long" = "8"; then
	  AC_MSG_RESULT([yes]);
	  opencode_v_word64=yes
    AC_DEFINE(OPENCODE_ON_LP64, 1,[Define to 1 if you run on 64bit os.])
  else
	  AC_MSG_RESULT([no]);
  fi

  AC_LANG_RESTORE
])


AC_DEFUN([OPENCODE_SLIBS_SET],
[
case $host_os in
hpux*)
  AC_SUBST(opencode_DEFS,["-DPORT_HP_UNIX"])
  AC_SUBST(opencode_LDFLAGS,["-lpthread  -lrt"])
;;
aix*)
  AC_SUBST(opencode_DEFS,["-DPORT_IBM_AIX"])
  AC_SUBST(opencode_LDFLAGS,["-lpthread -lm -lc -ldl -ltli"])
  ;;
solaris*)
  AC_SUBST(opencode_DEFS,["-DPORT_SUN_SOLARIS"])
  AC_SUBST(opencode_LDFLAGS,["-lpthread -ldl -lc -lrt -lsocket -lnsl"])
  ;;
linux*)
  AC_SUBST(opencode_DEFS,["-DPORT_LINUX"])
  AC_SUBST(opencode_LDFLAGS,["-lpthread  -ldl -lc"])
  ;;
esac
AC_SUBST(opencode_DEFAULT_INCLUDES,[""])
])

AC_DEFUN([OPENCODE_PROG_CXX],
[
if test "$opencode_v_enable_debug" = "yes"; then
  AC_DEFINE(DEBUG, 1,[Define to 1 if you want a debugging version.])
fi

opencode_v_dbgflags=""
opencode_v_CXXFLAGS=""
opencode_v_OBJFLAGS=""
opencode_v_LIBFLAGS=""

case $CXX in
aCC*)
  if test "$opencode_v_enable_debug" = "yes"; then
    opencode_v_dbgflags="-g0"
  else
    opencode_v_dbgflags="-fast"
  fi

  opencode_v_mtflags="-D_REENTRANT -D_RW_MULTI_THREAD -D_RWSTD_MULTI_THREAD -D_POSIX_C_SOURCE=199506L -D_HPUX_SOURCE"
  opencode_v_CXXFLAGS="$opencode_v_mtflags -AA +W829 +W849 +W749 +W652 +W129 +W921"
  opencode_v_OBJFLAGS="+z"
  opencode_v_LIBFLAGS="-b"
;;
xlC*)
  if test "$opencode_v_enable_debug" = "yes"; then
    opencode_v_dbgflags="-g -qfullpath -qlinedebug"
  else
    opencode_v_dbgflags="-O3 -qstrict"
  fi

  opencode_v_mtflags=""
  opencode_v_CXXFLAGS="-w -brtl -bhalt:5 -qstaticinline -qnortti"
  opencode_v_OBJFLAGS="-qmkshrobj"
  opencode_v_LIBFLAGS="-G -qmkshrobj"
;;
CC*)
  if test "$opencode_v_enable_debug" = "yes"; then
    opencode_v_dbgflags="-g"
  else
    opencode_v_dbgflags="-O2"
  fi
  
  opencode_v_mtflags="-mt"
  opencode_v_CXXFLAGS="$opencode_v_mtflags +w"
  opencode_v_OBJFLAGS="-KPIC"
  opencode_v_LIBFLAGS="-G"
;;
g++*)
  if test "$opencode_v_enable_debug" = "yes"; then
    opencode_v_dbgflags="-g"
  else
    opencode_v_dbgflags="-O2"
  fi

  opencode_v_mtflags=""
  opencode_v_CXXFLAGS="$opencode_v_mtflags -Wno-deprecated"
  opencode_v_OBJFLAGS=""
  opencode_v_LIBFLAGS=""
;;
*)
;;
esac

opencode_v_word64=no
opencode_v_lp64_avaliable=no
opencode_v_lp64_cxxflags=""

OPENCODE_CHECK_WORDS64

if test "$opencode_v_word64" = "no"; then
if test "$opencode_v_enable_lp64" = "yes"; then
  case $CXX in
  aCC*)   
    if test "$opencode_v_enable_lp64" = "yes"; then
      opencode_v_lp64_cxxflags="+DD64"
      OPENCODE_CHECK_LP64  
    fi
  ;;
  xlC*)
    if test "$opencode_v_enable_lp64" = "yes"; then
      opencode_v_lp64_cxxflags="-q64"
      OPENCODE_CHECK_LP64
    fi
  ;;
  CC*)
    if test "$opencode_v_enable_lp64" = "yes"; then
      opencode_v_lp64_cxxflags="-m64"
      OPENCODE_CHECK_LP64
    fi

    if test "$opencode_v_lp64_avaliable" = "yes"; then
      opencode_v_LIBFLAGS="-Wc,-m64 -G"
    fi
  ;;
  g++*)
    if test "$opencode_v_enable_lp64" = "yes"; then
      opencode_v_lp64_cxxflags="-m64"
      OPENCODE_CHECK_LP64
    fi
  ;;
  *)
  ;;
  esac
fi
fi

CXX="$CXX $opencode_v_lp64_cxxflags"
CXXFLAGS="$opencode_v_dbgflags"
  
AC_SUBST(opencode_CXXFLAGS,["$opencode_v_CXXFLAGS"])
AC_SUBST(opencode_OBJFLAGS,["$opencode_v_OBJFLAGS"])
AC_SUBST(opencode_LIBFLAGS,["$opencode_v_LIBFLAGS"])

OPENCODE_SLIBS_SET
])
