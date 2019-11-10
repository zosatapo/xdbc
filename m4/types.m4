dnl -------------------------------------------------------------------------
dnl types.m4
dnl
dnl OPENCODE M4 include file which contains OPENCODE specific M4 macros
dnl for checking certain type feature.
dnl
dnl -------------------------------------------------------------------------

AC_DEFUN([OPENCODE_CHECK_WCHAR_T],
[
  AC_LANG_SAVE
  AC_LANG_CPLUSPLUS

  AC_TRY_COMPILE([#include <wchar.h>],
  [
    wchar_t __xx_wchar_t = 8023;
  ],
  opencode_v_has_wchar_t=yes
  ,
  opencode_v_has_wchar_t=no)

  if test "x$opencode_v_has_wchar_t" = "xyes"; then
    AC_DEFINE(HAVE_TYPE_WCHAR_T, 1,Defined if we have wchar_t)
    AC_CHECK_SIZEOF(wchar_t)
  fi

  AC_LANG_RESTORE
])

AC_DEFUN([OPENCODE_CHECK_TYPES],
[
  AC_CHECK_SIZEOF(int,4)
  OPENCODE_CHECK_WCHAR_T
])
