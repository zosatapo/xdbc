/**
 *  odbcmac.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */

#ifndef __OPENCODE_XDBC_ODBCMAC_H__
#define __OPENCODE_XDBC_ODBCMAC_H__

//--------------------------------------------------------------------------------------

#define  __ODBC_ARRAY_MAX           30000

#define  __ODBCAPI_OPTIONS_INTEGER  1
#define  __ODBCAPI_OPTIONS_SQLLEN   2
#define  __ODBCAPI_OPTIONS_BOTH     3

#define  __ODBC_UNICODE_ASNI        0
#define  __ODBC_UNICODE_UTF8        1
#define  __ODBC_UNICODE_UTF16       2
#define  __ODBC_UNICODE_UTF32       3

//--------------------------------------------------------------------------------------

#ifndef SQLLEN_IS_INTEGER
#  if OPENCODE_SIZEOF_INT == OPENCODE_SIZEOF_LONG
#    define SQLLEN_IS_INTEGER
#  endif
#endif

#ifndef SQLAPI_INTEGER_OR_SQLLEN
#  define SQLAPI_INTEGER_OR_SQLLEN  __ODBCAPI_OPTIONS_BOTH
#endif

#define  __ODBC_HOOK_CHECK_FAILED(__odbc_sqlreturn) (__odbc_sqlreturn != SQL_SUCCESS)

//--------------------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////////
#if    SQLAPI_INTEGER_OR_SQLLEN == __ODBCAPI_OPTIONS_INTEGER
/////////////////////////////////////////////////////////////////////////////////

#define  __ODBC_GETBINDABLE_IND(__odbc_param,__odbc_ind)\
{\
__odbc_ind = (int)__odbc_param->__mi_bind_ind_int;\
}

#define  __ODBC_SETBINDABLE_IND(__odbc_param,__odbc_ind)\
{\
__odbc_param->__mi_bind_ind_int = (SQLINTEGER)(__odbc_ind);\
}

#define  __ODBC_GETDEFINABLE_IND(__odbc_define,__odbc_ind)\
{\
__odbc_ind = (int)__odbc_define->__mi_define_ind_int;\
}

#define  __ODBC_SETDEFINABLE_IND(__odbc_define,__odbc_ind)\
{\
__odbc_define->__mi_define_ind_int = (SQLINTEGER)(__odbc_ind);\
}

/////////////////////////////////////////////////////////////////////////////////
#elif  SQLAPI_INTEGER_OR_SQLLEN == __ODBCAPI_OPTIONS_SQLLEN
/////////////////////////////////////////////////////////////////////////////////

#define  __ODBC_GETBINDABLE_IND(__odbc_param,__odbc_ind)\
{\
__odbc_ind = (int)(__odbc_param->__mi_bind_ind_len);\
}

#define  __ODBC_SETBINDABLE_IND(__odbc_param,__odbc_ind)\
{\
__odbc_param->__mi_bind_ind_len = (SQLLEN)(__odbc_ind);\
}

#define  __ODBC_GETDEFINABLE_IND(__odbc_define,__odbc_ind)\
{\
__odbc_ind = (int)(__odbc_define->__mi_define_ind_len);\
}

#define  __ODBC_SETDEFINABLE_IND(__odbc_define,__odbc_ind)\
{\
__odbc_define->__mi_define_ind_len = (SQLLEN)(__odbc_ind);\
}

/////////////////////////////////////////////////////////////////////////////////
#else
/////////////////////////////////////////////////////////////////////////////////

#define  __ODBC_GETBINDABLE_IND(__odbc_param,__odbc_ind)\
if(__mb_fintAvailable)\
{\
  __odbc_ind = (int)__odbc_param->__mi_bind_ind_int;\
}\
else\
{\
  __odbc_ind = (int)__odbc_param->__mi_bind_ind_len;\
}

#define  __ODBC_SETBINDABLE_IND(__odbc_param,__odbc_ind)\
if(__mb_fintAvailable)\
{\
  __odbc_param->__mi_bind_ind_int = (SQLINTEGER)(__odbc_ind);\
}\
else\
{\
  __odbc_param->__mi_bind_ind_len = (SQLLEN)(__odbc_ind);\
}

#define  __ODBC_GETDEFINABLE_IND(__odbc_define,__odbc_ind)\
if(__mb_fintAvailable)\
{\
  __odbc_ind = (int)__odbc_define->__mi_define_ind_int;\
}\
else\
{\
  __odbc_ind = (int)__odbc_define->__mi_define_ind_len;\
}

#define  __ODBC_SETDEFINABLE_IND(__odbc_define,__odbc_ind)\
if(__mb_fintAvailable)\
{\
  __odbc_define->__mi_define_ind_int = (SQLINTEGER)(__odbc_ind);\
}\
else\
{\
  __odbc_define->__mi_define_ind_len = (SQLLEN)(__odbc_ind);\
}

/////////////////////////////////////////////////////////////////////////////////
#endif
/////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------

#define  __ODBC_PREPARED_CHECKPARAM(__paramIndex)\
{\
  if(__mb_isClosed)\
  {\
    __XDBC_HOOK_RETURN(return,"[OdbcPreparedStatement] PreparedStatement has always been closed");\
  }\
  if((__paramIndex < 1) || (__paramIndex > __mi_param_count) )\
  {\
    StringBuffer __sb;\
    __sb.append("[OdbcPreparedStatement] IndexOutOfBoundary : Argument index  must be in range [1,");\
    __sb.append(__mi_param_count);\
    __sb.append("]");\
    __XDBC_HOOK_RETURN(return,__sb.str());\
  }\
}

#define  __ODBC_CALL_CHECKNULL_POINTER(__paramIndex)\
__ODBC_CALL_CHECKPARAM_PROTO(__paramIndex,return 0);\
ODBC_PARAM* __odbc_param  = __vector_params[paramIndex];\
{\
  int __ind_tmp = (int)SQL_NULL_DATA;\
  __ODBC_GETBINDABLE_IND(__odbc_param,__ind_tmp);\
  if(SQL_NULL_DATA ==  __ind_tmp)\
  {\
    return 0;\
  }\
}

#define  __ODBC_PREPARED_CHECKBOUND_BEGIN_2(__paramIndex)\
if(is_null) \
{\
  __ODBC_SETBINDABLE_IND(__odbc_param,SQL_NULL_DATA);\
}\
if( (!__odbc_param->__mb_has_bound))\
{\
  __odbc_param->__mb_has_bound = true;

#define  __ODBC_PREPARED_CHECKBOUND_END_2(__paramIndex) }

/////////////////////////////////////////////////////////////////////////////////

#define __ODBC_CALL_CHECKPARAM_PROTO(__paramIndex,__return_stmt_list)\
if(__mb_isClosed)\
{\
  __XDBC_HOOK_RETURN(__return_stmt_list,"[OdbcCallableStatement] PreparedStatement has always been closed");\
}\
if((__paramIndex < 1) || (__paramIndex > __mi_param_count) )\
{\
  StringBuffer __sb;\
  __sb.append("[OdbcCallableStatement] IndexOutOfBoundary : Argument index  must be in range [1,");\
  __sb.append(__mi_param_count);\
  __sb.append("]");\
  __XDBC_HOOK_RETURN(__return_stmt_list,__sb.str());\
}

#define  __ODBC_CALL_CHECKPARAM(__paramIndex) __ODBC_CALL_CHECKPARAM_PROTO(__paramIndex,return)

#define __ODBC_CALL_CHECKNULL_BOOLEAN(__paramIndex)\
__ODBC_CALL_CHECKPARAM_PROTO(__paramIndex,return false);\
ODBC_PARAM* __odbc_param  = __vector_params[paramIndex];\
{\
  int __ind_tmp = (int)SQL_NULL_DATA;\
  __ODBC_GETBINDABLE_IND(__odbc_param,__ind_tmp);\
  if(SQL_NULL_DATA == __ind_tmp)\
  {\
    return false;\
  }\
}

#define  __ODBC_CALL_CHECKBOUND_BEGIN_2(__paramIndex)\
if(is_null) \
{\
  __ODBC_SETBINDABLE_IND(__odbc_param,SQL_NULL_DATA);\
}\
if( (!__odbc_param->__mb_has_bound))\
{\
  __odbc_param->__mb_has_bound = true;

#define  __ODBC_CALL_CHECKBOUND_END_2(__paramIndex) }

/////////////////////////////////////////////////////////////////////////////////

#define __ODBC_RSET_CHECKCLOSED_PROTO(bol_isClosed,__return_stmt_list)\
{\
  if(__mb_isClosed)\
  {\
    __XDBC_HOOK_RETURN(__return_stmt_list,"[OdbcResultSet] resultset has always been closed");\
  }\
}

#define  __ODBC_RSET_CHECKCLOSED(bol_isClosed)  __ODBC_RSET_CHECKCLOSED_PROTO(bol_isClosed,return false)

#define  __ODBC_RSET_CHECKCOLUMN_PROTO(__columnIndex,__return_stmt_list)\
__ODBC_RSET_CHECKCLOSED_PROTO(bol_isClosed,__return_stmt_list);\
if( (__columnIndex < 1) || (__columnIndex > __mi_column_count) )\
{\
  StringBuffer __sb;\
  __sb.append("[OdbcResultSet] IndexOutOfBoundary : column index  must be in range [1,");\
  __sb.append(__mi_column_count);\
  __sb.append("]");\
  __XDBC_HOOK_RETURN(return false,__sb.str());\
}

#define  __ODBC_RSET_CHECKCOLUMN(__columnIndex) __ODBC_RSET_CHECKCOLUMN_PROTO(__columnIndex,return false)

#define  __ODBC_RSET_CHECKNULL_BOOLEAN(__columnIndex)\
__ODBC_RSET_CHECKCOLUMN_PROTO(__columnIndex,return false);\
if(__mi_cur_row == 0 || __mi_cur_row == -1)\
{\
  __XDBC_HOOK_RETURN(return false,"[OdbcResultSet] no data indicated by current position,calling Traversal/Positioning first");\
}\
ODBC_DEFINE* __p_odbc_define = &(__cachedCollection->__vector_defines[columnIndex]);\
{\
  int __ind_tmp = (int)SQL_NULL_DATA;\
  __ODBC_GETDEFINABLE_IND(__p_odbc_define,__ind_tmp);\
  if(SQL_NULL_DATA == __ind_tmp)\
  {\
    return false;\
  }\
}

#define  __ODBC_RSET_CHECKNULL_POINTER(__columnIndex)\
__ODBC_RSET_CHECKCOLUMN_PROTO(__columnIndex,return 0);\
if(__mi_cur_row == 0 || __mi_cur_row == -1)\
{\
  __XDBC_HOOK_RETURN(return 0,"[OdbcResultSet] no data indicated by current position,calling Traversal/Positioning first");\
}\
ODBC_DEFINE* __p_odbc_define = &(__cachedCollection->__vector_defines[columnIndex]);\
{\
  int __ind_tmp = (int)SQL_NULL_DATA;\
  __ODBC_GETDEFINABLE_IND(__p_odbc_define,__ind_tmp);\
  if(SQL_NULL_DATA == __ind_tmp)\
  {\
    return 0;\
  }\
}

//--------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////////
#if    SQLAPI_INTEGER_OR_SQLLEN == __ODBCAPI_OPTIONS_INTEGER
/////////////////////////////////////////////////////////////////////////////////

#define  __ODBC_SQLBINDPARAMETER(__odbc_imode,__odbc_valueType,__odbc_paramType,__odbc_colSize,__odbc_digits,__odbc_valuePtr,__odbc_bufLen)\
{\
__odbc_sqlreturn = __odbc_SQLBindParameter(__mp_conn,odbc_hstmt, paramIndex,\
                                            __odbc_imode,\
                                            __odbc_valueType,\
                                            __odbc_paramType,\
                                            __odbc_colSize, __odbc_digits,\
                                            (void*)__odbc_valuePtr,__odbc_bufLen,\
                                            &(__odbc_param->__mi_bind_ind_int));\
}

/////////////////////////////////////////////////////////////////////////////////
#elif  SQLAPI_INTEGER_OR_SQLLEN == __ODBCAPI_OPTIONS_SQLLEN
/////////////////////////////////////////////////////////////////////////////////

#define  __ODBC_SQLBINDPARAMETER(__odbc_imode,__odbc_valueType,__odbc_paramType,__odbc_colSize,__odbc_digits,__odbc_valuePtr,__odbc_bufLen)\
{\
__odbc_sqlreturn = __odbc_SQLBindParameter(__mp_conn,odbc_hstmt, paramIndex,\
                                              __odbc_imode,\
                                              __odbc_valueType,\
                                              __odbc_paramType,\
                                              __odbc_colSize, __odbc_digits,\
                                              (void*)__odbc_valuePtr,__odbc_bufLen,\
                                              &(__odbc_param->__mi_bind_ind_len));\
}

/////////////////////////////////////////////////////////////////////////////////
#else
/////////////////////////////////////////////////////////////////////////////////

#define  __ODBC_SQLBINDPARAMETER(__odbc_imode,__odbc_valueType,__odbc_paramType,__odbc_colSize,__odbc_digits,__odbc_valuePtr,__odbc_bufLen)\
if(__mb_fintAvailable)\
{\
  __odbc_sqlreturn = __odbc_SQLBindParameter(__mp_conn,odbc_hstmt, paramIndex,\
                                                __odbc_imode,\
                                                __odbc_valueType,\
                                                __odbc_paramType,\
                                                __odbc_colSize, __odbc_digits,\
                                                (void*)__odbc_valuePtr,__odbc_bufLen,\
                                                &(__odbc_param->__mi_bind_ind_int));\
}\
else\
{\
  __odbc_sqlreturn = __odbc_SQLBindParameter(__mp_conn,odbc_hstmt, paramIndex,\
                                                __odbc_imode,\
                                                __odbc_valueType,\
                                                __odbc_paramType,\
                                                __odbc_colSize, __odbc_digits,\
                                                (void*)__odbc_valuePtr,__odbc_bufLen,\
                                                &(__odbc_param->__mi_bind_ind_len));\
}


/////////////////////////////////////////////////////////////////////////////////
#endif
/////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------


/////////////////////////////////////////////////////////////////////////////////
#if    SQLAPI_INTEGER_OR_SQLLEN == __ODBCAPI_OPTIONS_INTEGER
/////////////////////////////////////////////////////////////////////////////////

#define  __ODBC_DATA_MEMCPY(__odbc_srcbuf,__odbc_srcsize,__odbc_dstbuf,__odbc_dstsize)\
{\
  __odbc_param->__mi_bind_ind_int = __M_min(__odbc_srcsize,__odbc_dstsize); \
  memcpy(__odbc_dstbuf,__odbc_srcbuf,__odbc_param->__mi_bind_ind_int);\
}

/////////////////////////////////////////////////////////////////////////////////
#elif  SQLAPI_INTEGER_OR_SQLLEN == __ODBCAPI_OPTIONS_SQLLEN
/////////////////////////////////////////////////////////////////////////////////

#define  __ODBC_DATA_MEMCPY(__odbc_srcbuf,__odbc_srcsize,__odbc_dstbuf,__odbc_dstsize)\
{\
  __odbc_param->__mi_bind_ind_len = __M_min(__odbc_srcsize,__odbc_dstsize); \
  memcpy(__odbc_dstbuf,__odbc_srcbuf,__odbc_param->__mi_bind_ind_len);\
}

/////////////////////////////////////////////////////////////////////////////////
#else
/////////////////////////////////////////////////////////////////////////////////

#define  __ODBC_DATA_MEMCPY(__odbc_srcbuf,__odbc_srcsize,__odbc_dstbuf,__odbc_dstsize)\
if(__mb_fintAvailable)\
{\
  __odbc_param->__mi_bind_ind_int = __M_min(__odbc_srcsize,__odbc_dstsize); \
  memcpy(__odbc_dstbuf,__odbc_srcbuf,__odbc_param->__mi_bind_ind_int);\
}\
else\
{\
  __odbc_param->__mi_bind_ind_len = __M_min(__odbc_srcsize,__odbc_dstsize); \
  memcpy(__odbc_dstbuf,__odbc_srcbuf,__odbc_param->__mi_bind_ind_len);\
}

/////////////////////////////////////////////////////////////////////////////////
#endif
/////////////////////////////////////////////////////////////////////////////////

#endif

