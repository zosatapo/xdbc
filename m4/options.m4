dnl -------------------------------------------------------------------------
dnl options.m4
dnl
dnl OPENCODE M4 include file which contains OPENCODE specific M4 macros
dnl for enabling/disabling certain OPENCODE features.
dnl
dnl -------------------------------------------------------------------------

AC_DEFUN([OPENCODE_ENABLE_DEBUG],
[
  AC_ARG_ENABLE([debug],
    AS_HELP_STRING([--enable-debug], [compiles opencode with debug support [[default=no]]]),
      [
        case "$enableval" in
        yes)
          opencode_v_enable_debug=yes
          ;;
        no)
          opencode_v_enable_debug=no
          ;;
        *)
          AC_MSG_ERROR([bad value $enableval for --enable-debug])
          ;;
        esac
      ],
      [
        dnl The default is to disable debug support. 
        opencode_v_enable_debug=no
       ])
])


AC_DEFUN([OPENCODE_ENABLE_LP64],
[
  AC_ARG_ENABLE([lp64],
    AS_HELP_STRING([--enable-lp64], [compiles opencode with 64 bit support if possiable [[default=yes]]]),
      [
        case "$enableval" in
        yes)
          opencode_v_enable_lp64=yes
          ;;
        no)
          opencode_v_enable_lp64=no
          ;;
        *)
          AC_MSG_ERROR([bad value $enableval for --enable-lp64])
          ;;
        esac
      ],
      [
        dnl The default is to enable 64 bit support if possiable.
        opencode_v_enable_lp64=yes
       ])
])

AC_DEFUN([OPENCODE_ENABLE_EH4SOCKET],
[
AC_ARG_ENABLE([eh4socket],
  AS_HELP_STRING([--enable-eh4socket], [compiles socket with EH support [[default=yes]]]),
    [
      case "$enableval" in
      yes)
        opencode_v_enable_eh4socket=yes
        ;;
      no)
        opencode_v_enable_eh4socket=no
        ;;
      *)
        opencode_v_enable_eh4socket=yes
        ;;
      esac
    ],
    [
      dnl The default is to enable EH support.
      opencode_v_enable_eh4socket=yes
     ])

if test "x$opencode_v_enable_eh4socket" = "xno"; then
  AC_DEFINE(OPENCODE_DISABLE_NET_SEH, 1,Defined if we have socket with SEH support)
fi

AC_MSG_CHECKING([whether compiles socket with EH support])
AC_MSG_RESULT([$opencode_v_enable_eh4socket])
])


AC_DEFUN([OPENCODE_ENABLE_EH4XDBC],
[
AC_ARG_ENABLE([eh4xdbc],
  AS_HELP_STRING([--enable-eh4xdbc], [compiles xdbc with EH support [[default=yes]]]),
    [
      case "$enableval" in
      yes)
        opencode_v_enable_eh4xdbc=yes
        ;;
      no)
        opencode_v_enable_eh4xdbc=no
        ;;
      *)
        opencode_v_enable_eh4xdbc=yes
        ;;
      esac
    ],
    [
      dnl The default is to enable EH support.
      opencode_v_enable_eh4xdbc=yes
     ])

if test "x$opencode_v_enable_eh4xdbc" = "xno"; then
  AC_DEFINE(OPENCODE_DISABLE_XDBC_SEH, 1,Defined if we have xdbc with SEH support)
fi

AC_MSG_CHECKING([whether compiles xdbc with EH support])
AC_MSG_RESULT([$opencode_v_enable_eh4xdbc])

if test "x$opencode_v_enable_eh4xdbc" = "xno"; then
  AC_DEFINE(OPENCODE_DISABLE_XDBC_SEH, 1,Defined if we have xdbc with SEH support)
fi
])

AC_DEFUN([OPENCODE_WITH_ODBCOPTIONS],
[
AC_ARG_WITH([odbcOptions],
  AS_HELP_STRING([--with-odbcOptions], [compiles XDBC-ODBC support INTEGER or SQLLEN [[default=BOTH]]]),
    [
      case "$withval" in
      INTEGER)
        opencode_v_with_odbcOptions=INTEGER
        ;;
      SQLLEN)
        opencode_v_with_odbcOptions=SQLLEN
        ;;
      *)
        opencode_v_with_odbcOptions=BOTH
        ;;
      esac
    ],
    [
      dnl The default is to support SQLAPI select with BOTH (decided by driver configuration).
      opencode_v_with_odbcOptions=BOTH
     ])

AC_MSG_CHECKING([XDBC-ODBC support INTEGER or SQLLEN])
AC_MSG_RESULT([$opencode_v_with_odbcOptions])

case "$opencode_v_with_odbcOptions" in
INTEGER)
  AC_DEFINE_UNQUOTED(SQLAPI_INTEGER_OR_SQLLEN, 1,Defined odbcOptions level to INTEGER )
  ;;
SQLLEN)
  AC_DEFINE_UNQUOTED(SQLAPI_INTEGER_OR_SQLLEN, 2,Defined odbcOptions level to SQLLEN)
  ;;
*)
  AC_DEFINE_UNQUOTED(SQLAPI_INTEGER_OR_SQLLEN, 3,Defined odbcOptions level to BOTH)
  ;;
esac

])


AC_DEFUN([OPENCODE_CHECK_XDBC],
[
dnl Checks for xdbcImpl oracle SDK support
AC_MSG_CHECKING([whether oracle   sdk works])
opencode_ORACLE_SDK_HOME=
if test -n "${ORACLE_SDK_HOME}"; then
  opencode_ORACLE_SDK_HOME="${ORACLE_SDK_HOME}"
else
  if test -d "${ORACLE_HOME}/include"; then
    opencode_ORACLE_SDK_HOME="${ORACLE_HOME}"
  else
    opencode_ORACLE_SDK_HOME="${HOME}/instantclient"
  fi
fi
AC_SUBST(opencode_ORACLE_SDK_HOME)

opencode_v_enable_xdbc_oracle=no
if test -d "$opencode_ORACLE_SDK_HOME"; then
AC_MSG_RESULT([($opencode_ORACLE_SDK_HOME) yes])
  opencode_v_enable_xdbc_oracle=yes
else
  AC_MSG_RESULT([($opencode_ORACLE_SDK_HOME) no])
  opencode_v_enable_xdbc_oracle=no
fi
AM_CONDITIONAL([BUILD_XDBC_ORACLE], [test X$opencode_v_enable_xdbc_oracle = Xyes])

dnl Checks for xdbcImpl informix SDK support
AC_MSG_CHECKING([whether informix sdk works])
opencode_INFORMIX_SDK_HOME=
if test -n "${INFORMIX_SDK_HOME}"; then
  opencode_INFORMIX_SDK_HOME="${INFORMIX_SDK_HOME}"
else
  if test -d "/opt/informix/incl"; then
    opencode_INFORMIX_SDK_HOME="/opt/informix"
  else
    opencode_INFORMIX_SDK_HOME="${HOME}/informix"
  fi
fi
AC_SUBST(opencode_INFORMIX_SDK_HOME)
AC_SUBST(opencode_ESQLC_EXEC,["$opencode_INFORMIX_SDK_HOME/bin/esql"])
AC_SUBST(opencode_ESQLC_OPTS,["-e"])

opencode_v_enable_xdbc_informix=no
if test -d "$opencode_INFORMIX_SDK_HOME"; then
  AC_MSG_RESULT([($opencode_INFORMIX_SDK_HOME) yes])
  opencode_v_enable_xdbc_informix=yes
else
  AC_MSG_RESULT([($opencode_INFORMIX_SDK_HOME) no])
  opencode_v_enable_xdbc_informix=no
fi
AM_CONDITIONAL([BUILD_XDBC_INFORMIX], [test X$opencode_v_enable_xdbc_informix = Xyes])
])

