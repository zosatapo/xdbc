dnl -------------------------------------------------------------------------
dnl includes.m4
dnl
dnl OPENCODE M4 include file which contains OPENCODE specific M4 macros
dnl for checking certain header file.
dnl
dnl -------------------------------------------------------------------------

AC_DEFUN([OPENCODE_CHECK_STL_HASHMAP],
[
  AC_LANG_SAVE
  AC_LANG_CPLUSPLUS
  
  AC_MSG_CHECKING([whether the C++ compiler ($CXX $CXXFLAGS) supports STL hash_map])
  AC_TRY_COMPILE([#include <hash_map>],
  [
    std::hash_map<int,int> __xx_map;
  ],
  opencode_v_has_stl_hashmap=yes
  opencode_v_stl_hashmap_ns="std"
  ,
  AC_TRY_COMPILE([#include <ext/hash_map>],
  [
    __gnu_cxx::hash_map<int,int> __xx_map;
  ],
  opencode_v_has_stl_hashmap=yes
  opencode_v_stl_hashmap_ns="__gnu_cxx"
  ,
  opencode_v_has_stl_hashmap=no))

  AC_MSG_RESULT([$opencode_v_has_stl_hashmap])

  if test "x$opencode_v_has_stl_hashmap" = "xyes"; then
    AC_DEFINE(OPENCODE_HAS_STL_HASHMAP, 1,Defined if we have STL hash_map)
    AC_DEFINE_UNQUOTED(OPENCODE_STL_HASHMAP_NS, $opencode_v_stl_hashmap_ns,Defined its namespace if we have STL hash_map) 
  fi

  AC_LANG_RESTORE
])