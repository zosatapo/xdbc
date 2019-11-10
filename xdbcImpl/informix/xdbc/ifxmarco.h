/**
 *  oramarco.h
 *
 *  Copyright (C) 2002-2020 zosatapo 
 *
 *  zosatapo  dertyang@hotmail.com
 */

#ifndef __OPENCODE_XDBC_INFORMIX_MARCO_H__
#define __OPENCODE_XDBC_INFORMIX_MARCO_H__

#include "xdbc/xdefine.h"

__OPENCODE_BEGIN_NAMESPACE

//---------------------------------------------------------------
//
//---------------------------------------------------------------

#define  INFORMIX_CHAR_TO_BYTE_MAX  4

#define  SQLVCHAR_BYTE_MAX  4000
#define  SQLCHAR_BYTE_MAX   4000

//---------------------------------------------------------------
//
//---------------------------------------------------------------

#define  __XDBC_INFORMIXPREPARED_CHECKPARAM(__paramIndex)\
{\
  if(__mb_isClosed)\
  {\
    __XDBC_HOOK_RETURN(return,"[IfxPreparedStatement] PreparedStatement has always been closed");\
  }\
  if((__paramIndex < 1) || (__paramIndex > __mi_var_count) )\
  {\
    StringBuffer __sb;\
    __sb.append("[IfxPreparedStatement] IndexOutOfBoundary : Argument index  must be in range [1,");\
    __sb.append(__mi_var_count);\
    __sb.append("]");\
    __XDBC_HOOK_RETURN(return,__sb.str());\
  }\
}

#define  __XDBC_INFORMIXCALL_CHECKPARAM_PROTO(__paramIndex,__return_stmt_list)\
{\
  if(__sp_pstmt->__mb_isClosed)\
  {\
    __XDBC_HOOK_RETURN(__return_stmt_list,"[IfxCallableStatement] PreparedStatement has always been closed");\
  }\
  if((__paramIndex < 1) || (__paramIndex > __sp_pstmt->__mi_var_count) )\
  {\
    StringBuffer __sb;\
    __sb.append("[IfxCallableStatement] IndexOutOfBoundary : Argument index  must be in range [1,");\
    __sb.append(__sp_pstmt->__mi_var_count);\
    __sb.append("]");\
    __XDBC_HOOK_RETURN(__return_stmt_list,__sb.str());\
  }\
}

#define  __XDBC_INFORMIXCALL_CHECKPARAM(__paramIndex) __XDBC_INFORMIXCALL_CHECKPARAM_PROTO(__paramIndex,return)

#define  __XDBC_INFORMIXCALL_CHECKNULL_BOOLEAN(__paramIndex)\
{\
  __XDBC_INFORMIXCALL_CHECKPARAM_PROTO(__paramIndex,return false);\
  if(__sp_pstmt->__vector_binds[__paramIndex].__mi_bind_ind == -1)\
  {\
    return false;\
  }\
}

#define  __XDBC_INFORMIXCALL_CHECKNULL_POINTER(__paramIndex)\
{\
  __XDBC_INFORMIXCALL_CHECKPARAM_PROTO(__paramIndex,return 0);\
  if(__sp_pstmt->__vector_binds[__paramIndex].__mi_bind_ind == -1)\
  {\
    return 0;\
  }\
}
//--------------------------------------------------------------------------------------

#define  __XDBC_INFORMIXPREPARED_CHECKBOUND_BEGIN_2(__paramIndex)\
if(is_null) \
{\
  __vector_binds[__paramIndex].__mi_bind_ind = -1;\
}\
else\
{\
  __vector_binds[__paramIndex].__mi_bind_ind = 0;\
}\

#define  __XDBC_INFORMIXPREPARED_CHECKBOUND_END_2(__paramIndex)

//-------------------------------------------------------------------------------------

#define  __XDBC_INFORMIXRSET_CHECKCLOSED_PROTO(__mb_isClosed,__return_stmt_list)\
{\
  if(__mb_isClosed)\
  {\
    __XDBC_HOOK_RETURN(__return_stmt_list,"[IfxResultSet] resultset has always been closed");\
  }\
}

#define  __XDBC_INFORMIXRSET_CHECKCLOSED(__mb_isClosed)  __XDBC_INFORMIXRSET_CHECKCLOSED_PROTO(__mb_isClosed,return false)



#define  __XDBC_INFORMIXRSET_CHECKCOLUMN_PROTO(__columnIndex,__return_stmt_list)\
{\
  __XDBC_INFORMIXRSET_CHECKCLOSED_PROTO(__mb_isClosed,__return_stmt_list);\
  if( (__columnIndex < 1) || (__columnIndex > __mi_column_count) )\
  {\
    StringBuffer __sb;\
    __sb.append("[IfxResultSet] IndexOutOfBoundary : column index  must be in range [1,");\
    __sb.append(__mi_column_count);\
    __sb.append("]");\
    __XDBC_HOOK_RETURN(__return_stmt_list,__sb.str());\
  }\
}

#define  __XDBC_INFORMIXRSET_CHECKCOLUMN(__columnIndex) __XDBC_INFORMIXRSET_CHECKCOLUMN_PROTO(__columnIndex,return false)

#define  __XDBC_INFORMIXRSET_CHECKNULL_BOOLEAN(__columnIndex)\
{\
  __XDBC_INFORMIXRSET_CHECKCOLUMN_PROTO(__columnIndex,return false);\
  if(__mi_cur_row == 0 || __mi_cur_row == -1)\
  {\
    __XDBC_HOOK_RETURN(return false,"[IfxResultSet] no data indicated by current position,calling Traversal/Positioning first");\
  }\
  if(__cachedCollection->__vector_defines[__columnIndex].__mi_define_ind == -1)\
  {\
    return false;\
  }\
}

#define  __XDBC_INFORMIXRSET_CHECKNULL_POINTER(__columnIndex)\
{\
  __XDBC_INFORMIXRSET_CHECKCOLUMN_PROTO(__columnIndex,return 0);\
  if(__mi_cur_row == 0 || __mi_cur_row == -1)\
  {\
    __XDBC_HOOK_RETURN(return 0,"[IfxResultSet] no data indicated by current position,calling Traversal/Positioning first");\
  }\
  if(__cachedCollection->__vector_defines[__columnIndex].__mi_define_ind == -1)\
  {\
    return 0;\
  }\
}

__OPENCODE_END_NAMESPACE

#endif


