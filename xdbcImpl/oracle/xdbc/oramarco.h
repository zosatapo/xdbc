/**
 *  oramarco.h
 *
 *  Copyright (C) 2002-2020 zosatapo 
 *
 *  zosatapo  dertyang@hotmail.com
 */

#ifndef __OPENCODE_ORACLE_MARCO_H__
#define __OPENCODE_ORACLE_MARCO_H__

#include "xdbc/xdefine.h"

__OPENCODE_BEGIN_NAMESPACE

//---------------------------------------------------------------
//
//---------------------------------------------------------------

#define  ORACLE_CHAR_TO_BYTE_MAX  4

#define  SHORT_PREFIX_LENGTH    2
#define  INTEGER_PREFIX_LENGTH  4

#define  SQLT_LNG_BYTE_MAX  4000

#define  SQLT_BIN_BYTE_MAX  2000
#define  SQLT_LBI_BYTE_MAX  4000

#define  SQLT_CHR_BYTE_MAX  4000

#define  SQLT_VCS_BYTE_MAX  4000
#define  SQLT_VBI_BYTE_MAX  4000

#define  SQLT_LVC_BYTE_MAX  8000
#define  SQLT_LVB_BYTE_MAX  8000

#define  SQLT_AVC_BYTE_MAX  2000
#define  SQLT_AFC_BYTE_MAX  2000

#define  SQLT_RDD_BYTE_MAX  4000

//---------------------------------------------------------------
//
//---------------------------------------------------------------

#define  __XDBC_ORACLEPREPARED_CHECKPARAM(__paramIndex)\
{\
  if(__mb_isClosed)\
  {\
    __XDBC_HOOK_RETURN(return,"[OraclePreparedStatement] PreparedStatement has always been closed");\
  }\
  if((__paramIndex < 1) || (__paramIndex > __mi_param_count) )\
  {\
    StringBuffer __sb;\
    __sb.append("[OraclePreparedStatement] IndexOutOfBoundary : Argument index  must be in range [1,");\
    __sb.append(__mi_param_count);\
    __sb.append("]");\
    __XDBC_HOOK_RETURN(return,__sb.str());\
  }\
}

#define  __XDBC_ORACLECALL_CHECKPARAM_PROTO(__paramIndex,__return_stmt_list)\
{\
  if(__sp_pstmt->__mb_isClosed)\
  {\
    __XDBC_HOOK_RETURN(__return_stmt_list,"[OracleCallableStatement] PreparedStatement has always been closed");\
  }\
  if((__paramIndex < 1) || (__paramIndex > __sp_pstmt->__mi_param_count) )\
  {\
    StringBuffer __sb;\
    __sb.append("[OracleCallableStatement] IndexOutOfBoundary : Argument index  must be in range [1,");\
    __sb.append(__sp_pstmt->__mi_param_count);\
    __sb.append("]");\
    __XDBC_HOOK_RETURN(__return_stmt_list,__sb.str());\
  }\
}

#define  __XDBC_ORACLECALL_CHECKPARAM(__paramIndex) __XDBC_ORACLECALL_CHECKPARAM_PROTO(__paramIndex,return)


#define  __XDBC_ORACLECALL_CHECKNULL_BOOLEAN(__paramIndex)\
{\
  __XDBC_ORACLECALL_CHECKPARAM_PROTO(__paramIndex,return false);\
  if(__sp_pstmt->__vector_binds[__paramIndex]->__mi_bind_ind == -1)\
  {\
    return false;\
  }\
}

#define  __XDBC_ORACLECALL_CHECKNULL_POINTER(__paramIndex)\
{\
  __XDBC_ORACLECALL_CHECKPARAM_PROTO(__paramIndex,return 0);\
  if(__sp_pstmt->__vector_binds[__paramIndex]->__mi_bind_ind == -1)\
  {\
    return 0;\
  }\
}
//--------------------------------------------------------------------------------------

#define  __XDBC_ORACLEPREPARED_CHECKBOUND_BEGIN_1(__paramIndex)\
if(is_null) \
{\
  __vector_binds[__paramIndex]->__mi_bind_ind = -1;\
}\
else\
{\
  __vector_binds[__paramIndex]->__mi_bind_ind = 0;\
}

#define  __XDBC_ORACLEPREPARED_CHECKBOUND_END_1(__paramIndex)

//--------------------------------------------------------------------------------------

#define  __XDBC_ORACLEPREPARED_CHECKBOUND_BEGIN_2(__paramIndex)\
if(is_null) \
{\
  __vector_binds[__paramIndex]->__mi_bind_ind = -1;\
}\
else\
{\
  __vector_binds[__paramIndex]->__mi_bind_ind = 0;\
}\
if( (!__vector_binds[__paramIndex]->__mb_has_bound))\
{\
  __vector_binds[__paramIndex]->__mb_has_bound = true;

#define  __XDBC_ORACLEPREPARED_CHECKBOUND_END_2(__paramIndex) }

//-------------------------------------------------------------------------------------

#define  __XDBC_ORACLERSET_CHECKCLOSED_PROTO(__mb_isClosed,__return_stmt_list)\
{\
  if(__mb_isClosed)\
  {\
    __XDBC_HOOK_RETURN(__return_stmt_list,"[OracleResultSet] resultset has always been closed");\
  }\
}

#define  __XDBC_ORACLERSET_CHECKCLOSED(__mb_isClosed)  __XDBC_ORACLERSET_CHECKCLOSED_PROTO(__mb_isClosed,return false)

#define  __XDBC_ORACLERSET_CHECKCOLUMN_PROTO(__columnIndex,__return_stmt_list)\
{\
  __XDBC_ORACLERSET_CHECKCLOSED_PROTO(__mb_isClosed,__return_stmt_list);\
  if( (__columnIndex < 1) || (__columnIndex > __mi_column_count) )\
  {\
    StringBuffer __sb;\
    __sb.append("[OracleResultSet] IndexOutOfBoundary : column index  must be in range [1,");\
    __sb.append(__mi_column_count);\
    __sb.append("]");\
    __XDBC_HOOK_RETURN(__return_stmt_list,__sb.str());\
  }\
}

#define  __XDBC_ORACLERSET_CHECKCOLUMN(__columnIndex) __XDBC_ORACLERSET_CHECKCOLUMN_PROTO(__columnIndex,return false)

#define  __XDBC_ORACLERSET_CHECKNULL_BOOLEAN(__columnIndex)\
{\
  __XDBC_ORACLERSET_CHECKCOLUMN_PROTO(__columnIndex,return false);\
  if(__mi_cur_row == 0 || __mi_cur_row == -1)\
  {\
    __XDBC_HOOK_RETURN(return false,"[OracleResultSet] no data indicated by current position,calling Traversal/Positioning first");\
  }\
  if(__cachedCollection->__vector_defines[__columnIndex].__mi_define_ind == -1)\
  {\
    return false;\
  }\
}

#define  __XDBC_ORACLERSET_CHECKNULL_POINTER(__columnIndex)\
{\
  __XDBC_ORACLERSET_CHECKCOLUMN_PROTO(__columnIndex,return 0);\
  if(__mi_cur_row == 0 || __mi_cur_row == -1)\
  {\
    __XDBC_HOOK_RETURN(return 0,"[OracleResultSet] no data indicated by current position,calling Traversal/Positioning first");\
  }\
  if(__cachedCollection->__vector_defines[__columnIndex].__mi_define_ind == -1)\
  {\
    return 0;\
  }\
}
__OPENCODE_END_NAMESPACE

#endif


