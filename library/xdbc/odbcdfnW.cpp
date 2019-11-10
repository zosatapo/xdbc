#include "xdbc/odbcdfn.h"
#include "xdbc/odbcConnection.h"
#include "port/unicode.h"

USING_OPENCODE_NS

#ifdef __cplusplus
extern "C" {                /* Assume C declarations for C++   */
#endif  /* __cplusplus */

SQLINTEGER __default_odbcAnsiToUnicode(SQLPOINTER* UnicodeBufferP,SQLINTEGER* AllocLength,SQLINTEGER* AvailableLength,SQLINTEGER AllocType,SQLCHAR* AnsiBuffer, SQLINTEGER BufferLenth)
{
  *AllocLength = 0;
  *AvailableLength = 0;
  
  void* __unicode_bufreturn = (void*)0;

  if(AnsiBuffer == 0)
  {
    return *AvailableLength;
  }

  SQLINTEGER __tmp_BufferLenth = BufferLenth;
  int __unicode_multiply = 1;

  if(BufferLenth == SQL_NTS)
  {
    __tmp_BufferLenth = (SQLINTEGER)strlen((const char*)AnsiBuffer);
  }
  
  switch(AllocType)
  {
    case __ODBC_UNICODE_UTF32:
    {
      __unicode_multiply = (int)sizeof(UTF32);
      SQLINTEGER __tmp_AllocLength = __tmp_BufferLenth + 1;
      *AllocLength = __tmp_AllocLength;
      UTF32* __buf_unicode = new UTF32[__tmp_AllocLength];
      memset((char*)__buf_unicode,0,__unicode_multiply * __tmp_AllocLength);
      *AvailableLength = Unicode::ansi_to_utf32((char*)AnsiBuffer,__tmp_BufferLenth,__buf_unicode,__tmp_AllocLength);
      __unicode_bufreturn = (void*)(__buf_unicode);
    }
    break;

    case __ODBC_UNICODE_UTF16:
    {
      __unicode_multiply = (int)sizeof(UTF16);
      SQLINTEGER __tmp_AllocLength = __tmp_BufferLenth + 1;
      *AllocLength = __tmp_AllocLength;
      UTF16* __buf_unicode = new UTF16[__tmp_AllocLength];
      memset((char*)__buf_unicode,0,__unicode_multiply * __tmp_AllocLength);
      *AvailableLength = Unicode::ansi_to_utf16((const char*)AnsiBuffer,__tmp_BufferLenth,__buf_unicode,__tmp_AllocLength);
      __unicode_bufreturn = (void*)(__buf_unicode); 
    }
    break;

    case __ODBC_UNICODE_UTF8:
    {
      __unicode_multiply = (int)sizeof(UTF8);
      SQLINTEGER __tmp_AllocLength = __tmp_BufferLenth * 6 + 1;
      *AllocLength = __tmp_AllocLength;
      UTF8* __buf_unicode = new UTF8[__tmp_AllocLength];
      memset((char*)__buf_unicode,0,__unicode_multiply * __tmp_AllocLength);
      *AvailableLength = Unicode::ansi_to_utf8((const char*)AnsiBuffer,__tmp_BufferLenth,__buf_unicode,__tmp_AllocLength);
      __unicode_bufreturn = (void*)(__buf_unicode);
    }
    break;

  }//~end switch(AllocType)

#if defined(DEBUG) && (DEBUG > 100) 
  printf("==== __default_odbcAnsiToUnicode ====\n");
  printf("ASCII[%d]:\n%s\n",(int)__tmp_BufferLenth,MemoryKit::memDump((const char*)AnsiBuffer,__tmp_BufferLenth).c_str());
  printf("UNICO[%d]:\n%s\n",(int)(*AvailableLength),MemoryKit::memDump((const char*)__unicode_bufreturn,(*AvailableLength)*__unicode_multiply).c_str());
#endif
  *UnicodeBufferP = __unicode_bufreturn;

  return *AvailableLength;
}

SQLINTEGER __default_odbcUnicodeToAnsi(SQLCHAR* AnsiBuffer,SQLINTEGER BufferLenth1,SQLINTEGER AllocType,SQLCHAR* UnicodeBuffer, SQLINTEGER BufferLenth2)
{
  if(UnicodeBuffer == 0)
  {
    return 0;
  }

  SQLINTEGER __size = 0;
  int __unicode_multiply = 1;

  switch(AllocType)
  {
    case __ODBC_UNICODE_UTF32:
    {
      __unicode_multiply = (int)sizeof(UTF32);
      SQLINTEGER __tmp_AllocLength = BufferLenth2 * __unicode_multiply + 1;
      SQLCHAR* __buf_sqlchar = new SQLCHAR[__tmp_AllocLength];
      memset(__buf_sqlchar,0,sizeof(SQLCHAR) * __tmp_AllocLength);
      int __sqlchar_size = Unicode::utf32_to_ansi((UTF32*)(UnicodeBuffer),BufferLenth2,(char*)__buf_sqlchar,__tmp_AllocLength);
      __size = __M_min(__sqlchar_size,BufferLenth1);
      memcpy(AnsiBuffer,__buf_sqlchar,__size);
      delete []__buf_sqlchar;
    }
    break;

    case __ODBC_UNICODE_UTF16:
    {
      __unicode_multiply = (int)sizeof(UTF16);
      SQLINTEGER __tmp_AllocLength = BufferLenth2 * __unicode_multiply + 1;
      SQLCHAR* __buf_sqlchar = new SQLCHAR[__tmp_AllocLength];
      memset(__buf_sqlchar,0,sizeof(SQLCHAR) * __tmp_AllocLength);

      int __sqlchar_size = Unicode::utf16_to_ansi((UTF16*)(UnicodeBuffer),BufferLenth2,(char*)__buf_sqlchar,__tmp_AllocLength);
      
      __size = __M_min(__sqlchar_size,BufferLenth1);
      memcpy(AnsiBuffer,__buf_sqlchar,__size);
      delete []__buf_sqlchar;
    }
    break;

    case __ODBC_UNICODE_UTF8:
    {
      __unicode_multiply = (int)sizeof(UTF8);
      SQLINTEGER __tmp_AllocLength = BufferLenth2 * __unicode_multiply + 1;
      SQLCHAR* __buf_sqlchar = new SQLCHAR[__tmp_AllocLength];
      memset(__buf_sqlchar,0,sizeof(SQLCHAR) * __tmp_AllocLength);
      int __sqlchar_size = Unicode::utf8_to_ansi((UTF8*)(UnicodeBuffer),BufferLenth2,(char*)__buf_sqlchar,__tmp_AllocLength);
      __size = __M_min(__sqlchar_size,BufferLenth1);
      memcpy(AnsiBuffer,__buf_sqlchar,__size);
      delete []__buf_sqlchar;
    }
    break;

  }//~end switch(AllocType)


#if defined(DEBUG) && (DEBUG > 100) 
  printf("==== __default_odbcUnicodeToAnsi ====\n");
  printf("UNICO[%d]:\n%s\n",(int)BufferLenth2,MemoryKit::memDump((const char*)UnicodeBuffer,BufferLenth2*__unicode_multiply).c_str());
  printf("ASCII[%d]:\n%s\n",(int)__size,MemoryKit::memDump((const char*)AnsiBuffer,__size).c_str());
#endif

  return __size;
}

#ifdef __cplusplus
}                                    /* End of extern "C" { */
#endif  /* __cplusplus */

__OPENCODE_BEGIN_NAMESPACE

SQLRETURN __odbc_SQLDescribeCol   (OdbcConnection*   ODBCConnection,
                                   SQLHSTMT          StatementHandle,
                                   SQLSMALLINT       ColumnNumber,
                                   SQLCHAR*          ColumnName,
                                   SQLSMALLINT       BufferLength,
                                   SQLSMALLINT*      NameLengthPtr,
                                   SQLSMALLINT*      DataTypePtr,
                                   SQLUINTEGER*      ColumnSizePtr,
                                   SQLSMALLINT*      DecimalDigitsPtr,
                                   SQLSMALLINT*      NullablePtr)
{
  struct ODBC_LIBRARY_APIS*   __odbc_apis    = ODBCConnection->__odbc_apis;
  struct ODBC_DRIVER_OPTIONS* __odbc_options = ODBCConnection->__odbc_options;

  ODBCSQLOPTIONS_ANSITOUNICODE   __options_ANSITOUNICODE = __odbc_options->__options_ANSITOUNICODE;
  ODBCSQLOPTIONS_UNICODETOANSI   __options_UNICODETOANSI = __odbc_options->__options_UNICODETOANSI;
  
  if(__odbc_options->__mb_fintAvailable)
  {
    ODBCSQLAPI_SQLDESCRIBECOL_INT   __api_SQLDESCRIBECOL_INT  = __odbc_apis->__api_SQLDESCRIBECOL_INT;
    ODBCSQLAPI_SQLDESCRIBECOLW_INT  __api_SQLDESCRIBECOLW_INT = __odbc_apis->__api_SQLDESCRIBECOLW_INT;

    if(__api_SQLDESCRIBECOL_INT != 0)
    {
      return (__api_SQLDESCRIBECOL_INT)(StatementHandle,
                                        ColumnNumber,
                                        ColumnName,
                                        BufferLength,
                                        NameLengthPtr,
                                        DataTypePtr,
                                        ColumnSizePtr,
                                        DecimalDigitsPtr,
                                        NullablePtr);
    }
    else
    {
      int __db_unicode = __odbc_options->__mi_driver_unicode;
      SQLRETURN __odbc_sqlreturn = SQL_ERROR;

      switch(__db_unicode)
      {
        case __ODBC_UNICODE_UTF32:
        {
          SQLSMALLINT   __uni_AllocLength   = 0;
          SQLSMALLINT*  __uni_NameLengthPtr = 0;
          UTF32*        __uni_ColumnName    = 0;
          SQLSMALLINT   __uni_NameLength    = 0;

          if((ColumnName != 0) && (BufferLength > 0))
          {
            __uni_AllocLength = BufferLength + 1;
            __uni_ColumnName    = new UTF32[__uni_AllocLength];
            memset((char*)__uni_ColumnName,0,sizeof(UTF32) * __uni_AllocLength);

            __uni_NameLengthPtr = &__uni_NameLength;
          }

          __odbc_sqlreturn = (__api_SQLDESCRIBECOLW_INT)(StatementHandle,
                                                        ColumnNumber,
                                                        (SQLWCHAR*)__uni_ColumnName,
                                                        __uni_AllocLength,
                                                        __uni_NameLengthPtr,
                                                        DataTypePtr,
                                                        ColumnSizePtr,
                                                        DecimalDigitsPtr,
                                                        NullablePtr);         

          if(__uni_ColumnName != 0)
          {
            int __size_NameLength = (__options_UNICODETOANSI)(ColumnName,BufferLength,__db_unicode,(SQLCHAR*)__uni_ColumnName,__uni_NameLength);
            if(NameLengthPtr != 0)
            {
              *NameLengthPtr = (SQLSMALLINT)__size_NameLength;
            }

            delete []__uni_ColumnName;
          }

        }
        break;

        case __ODBC_UNICODE_UTF16:
        {
          SQLSMALLINT   __uni_AllocLength   = 0;
          SQLSMALLINT*  __uni_NameLengthPtr = 0;
          UTF16*        __uni_ColumnName    = 0;
          SQLSMALLINT   __uni_NameLength    = 0;

          if((ColumnName != 0) && (BufferLength > 0))
          {
            __uni_AllocLength = BufferLength + 1;
            __uni_ColumnName    = new UTF16[__uni_AllocLength];
            memset((char*)__uni_ColumnName,0,sizeof(UTF16) * __uni_AllocLength);

            __uni_NameLengthPtr = &__uni_NameLength;
          }

          __odbc_sqlreturn = (__api_SQLDESCRIBECOLW_INT)(StatementHandle,
                                                        ColumnNumber,
                                                        (SQLWCHAR*)__uni_ColumnName,
                                                        __uni_AllocLength,
                                                        __uni_NameLengthPtr,
                                                        DataTypePtr,
                                                        ColumnSizePtr,
                                                        DecimalDigitsPtr,
                                                        NullablePtr);
          
          if(__uni_ColumnName != 0)
          {
            int __size_NameLength = (__options_UNICODETOANSI)(ColumnName,BufferLength,__db_unicode,(SQLCHAR*)__uni_ColumnName,__uni_NameLength);
            if(NameLengthPtr != 0)
            {
              *NameLengthPtr = (SQLSMALLINT)__size_NameLength;
            }

            delete []__uni_ColumnName;
          }
        }
        break;

        case __ODBC_UNICODE_UTF8:
        default:
        {
          SQLSMALLINT   __uni_AllocLength   = 0;
          SQLSMALLINT*  __uni_NameLengthPtr = 0;
          UTF8*         __uni_ColumnName    = 0;
          SQLSMALLINT   __uni_NameLength    = 0;

          if((ColumnName != 0) && (BufferLength > 0))
          {
            __uni_AllocLength = BufferLength * 6 + 1;
            __uni_ColumnName    = new UTF8[__uni_AllocLength];
            memset((char*)__uni_ColumnName,0,sizeof(UTF8) * __uni_AllocLength);

            __uni_NameLengthPtr = &__uni_NameLength;
          }

          __odbc_sqlreturn = (__api_SQLDESCRIBECOLW_INT)(StatementHandle,
                                                        ColumnNumber,
                                                        (SQLWCHAR*)__uni_ColumnName,
                                                        __uni_AllocLength,
                                                        __uni_NameLengthPtr,
                                                        DataTypePtr,
                                                        ColumnSizePtr,
                                                        DecimalDigitsPtr,
                                                        NullablePtr);

          if(__uni_ColumnName != 0)
          {
            int __size_NameLength = (__options_UNICODETOANSI)(ColumnName,BufferLength,__db_unicode,(SQLCHAR*)__uni_ColumnName,__uni_NameLength);
            if(NameLengthPtr != 0)
            {
              *NameLengthPtr = (SQLSMALLINT)__size_NameLength;
            }

            delete []__uni_ColumnName;
          }
        }
        break;

      }//~end switch(__db_unicode)

      return __odbc_sqlreturn;

    }//~end if(__api_SQLDESCRIBECOL_INT != 0)
  }
  else
  {
    ODBCSQLAPI_SQLDESCRIBECOL_LEN   __api_SQLDESCRIBECOL_LEN  = __odbc_apis->__api_SQLDESCRIBECOL_LEN;
    ODBCSQLAPI_SQLDESCRIBECOLW_LEN  __api_SQLDESCRIBECOLW_LEN = __odbc_apis->__api_SQLDESCRIBECOLW_LEN;
    if(__api_SQLDESCRIBECOL_LEN != 0)
    {
      SQLULEN __val_ColumnSize = 0;
      SQLRETURN __odbc_sqlreturn =  (__api_SQLDESCRIBECOL_LEN)(StatementHandle,
                                                               ColumnNumber,
                                                               ColumnName,
                                                               BufferLength,
                                                               NameLengthPtr,
                                                               DataTypePtr,
                                                               &__val_ColumnSize,
                                                               DecimalDigitsPtr,
                                                               NullablePtr);

      *ColumnSizePtr = (SQLUINTEGER)__val_ColumnSize;
      return __odbc_sqlreturn;
    }
    else
    {
      int __db_unicode = __odbc_options->__mi_driver_unicode;
      SQLRETURN __odbc_sqlreturn = SQL_ERROR;
      SQLULEN __val_ColumnSize = 0;

      switch(__db_unicode)
      {
        case __ODBC_UNICODE_UTF32:
        {
          SQLSMALLINT   __uni_AllocLength   = 0;
          SQLSMALLINT*  __uni_NameLengthPtr = 0;
          UTF32*        __uni_ColumnName    = 0;
          SQLSMALLINT   __uni_NameLength    = 0;

          if((ColumnName != 0) && (BufferLength > 0))
          {
            __uni_AllocLength = BufferLength + 1;
            __uni_ColumnName    = new UTF32[__uni_AllocLength];
            memset((char*)__uni_ColumnName,0,sizeof(UTF32) * __uni_AllocLength);

            __uni_NameLengthPtr = &__uni_NameLength;
          }

          __odbc_sqlreturn = (__api_SQLDESCRIBECOLW_LEN)(StatementHandle,
                                                        ColumnNumber,
                                                        (SQLWCHAR*)__uni_ColumnName,
                                                        __uni_AllocLength,
                                                        __uni_NameLengthPtr,
                                                        DataTypePtr,
                                                        &__val_ColumnSize,
                                                        DecimalDigitsPtr,
                                                        NullablePtr);

          if(__uni_ColumnName != 0)
          {
            int __size_NameLength = (__options_UNICODETOANSI)(ColumnName,BufferLength,__db_unicode,(SQLCHAR*)__uni_ColumnName,__uni_NameLength);
            if(NameLengthPtr != 0)
            {
              *NameLengthPtr = (SQLSMALLINT)__size_NameLength;
            }

            delete []__uni_ColumnName;
          }

        }
        break;

        case __ODBC_UNICODE_UTF16:
        {
          SQLSMALLINT   __uni_AllocLength   = 0;
          SQLSMALLINT*  __uni_NameLengthPtr = 0;
          UTF16*        __uni_ColumnName    = 0;
          SQLSMALLINT   __uni_NameLength    = 0;

          if((ColumnName != 0) && (BufferLength > 0))
          {
            __uni_AllocLength = BufferLength + 1;
            __uni_ColumnName    = new UTF16[__uni_AllocLength];
            memset((char*)__uni_ColumnName,0,sizeof(UTF16) * __uni_AllocLength);

            __uni_NameLengthPtr = &__uni_NameLength;
          }
          
          __odbc_sqlreturn = (__api_SQLDESCRIBECOLW_LEN)(StatementHandle,
                                                        ColumnNumber,
                                                        (SQLWCHAR*)__uni_ColumnName,
                                                        __uni_AllocLength,
                                                        __uni_NameLengthPtr,
                                                        DataTypePtr,
                                                        &__val_ColumnSize,
                                                        DecimalDigitsPtr,
                                                        NullablePtr);
          //printf("type %d\n",*DataTypePtr);
          //


          if(__uni_ColumnName != 0)
          {
            int __size_NameLength = (__options_UNICODETOANSI)(ColumnName,BufferLength,__db_unicode,(SQLCHAR*)__uni_ColumnName,__uni_NameLength);
            if(NameLengthPtr != 0)
            {
              *NameLengthPtr = (SQLSMALLINT)__size_NameLength;
            }

            delete []__uni_ColumnName;
          }
        }
        break;

        case __ODBC_UNICODE_UTF8:
        default:
        {
          SQLSMALLINT   __uni_AllocLength   = 0;
          SQLSMALLINT*  __uni_NameLengthPtr = 0;
          UTF8*         __uni_ColumnName    = 0;
          SQLSMALLINT   __uni_NameLength    = 0;

          if((ColumnName != 0) && (BufferLength > 0))
          {
            __uni_AllocLength = BufferLength * 6 + 1;
            __uni_ColumnName    = new UTF8[__uni_AllocLength];
            memset((char*)__uni_ColumnName,0,sizeof(UTF8) * __uni_AllocLength);

            __uni_NameLengthPtr = &__uni_NameLength;
          }

          __odbc_sqlreturn = (__api_SQLDESCRIBECOLW_LEN)(StatementHandle,
                                                        ColumnNumber,
                                                        (SQLWCHAR*)__uni_ColumnName,
                                                        __uni_AllocLength,
                                                        __uni_NameLengthPtr,
                                                        DataTypePtr,
                                                        &__val_ColumnSize,
                                                        DecimalDigitsPtr,
                                                        NullablePtr);

          if(__uni_ColumnName != 0)
          {
            int __size_NameLength = (__options_UNICODETOANSI)(ColumnName,BufferLength,__db_unicode,(SQLCHAR*)__uni_ColumnName,__uni_NameLength);
            if(NameLengthPtr != 0)
            {
              *NameLengthPtr = (SQLSMALLINT)__size_NameLength;
            }

            delete []__uni_ColumnName;
          }
        }
        break;

      }//~end switch(__db_unicode)

     *ColumnSizePtr = (SQLUINTEGER)__val_ColumnSize;
      return __odbc_sqlreturn;
    }
  }

  return SQL_ERROR;
}

SQLRETURN __odbc_SQLGetConnectAttr(OdbcConnection*   ODBCConnection,
                                   SQLHDBC           ConnectionHandle,
                                   SQLINTEGER        Attribute,
                                   SQLPOINTER        ValuePtr,
                                   SQLINTEGER        BufferLength,
                                   SQLINTEGER*       StringLengthPtr)
{
  struct ODBC_LIBRARY_APIS*   __odbc_apis    = ODBCConnection->__odbc_apis;
  struct ODBC_DRIVER_OPTIONS* __odbc_options = ODBCConnection->__odbc_options;

  ODBCSQLOPTIONS_ANSITOUNICODE   __options_ANSITOUNICODE = __odbc_options->__options_ANSITOUNICODE;
  ODBCSQLOPTIONS_UNICODETOANSI   __options_UNICODETOANSI = __odbc_options->__options_UNICODETOANSI;
    
  ODBCSQLAPI_SQLGETCONNECTATTR   __api_SQLGETCONNECTATTR  = __odbc_apis->__api_SQLGETCONNECTATTR;
  ODBCSQLAPI_SQLGETCONNECTATTRW  __api_SQLGETCONNECTATTRW = __odbc_apis->__api_SQLGETCONNECTATTRW;

  if(__api_SQLGETCONNECTATTR != 0)
  {
    return (__api_SQLGETCONNECTATTR)(ConnectionHandle,Attribute,ValuePtr,BufferLength,StringLengthPtr);
  }
  else if (__api_SQLGETCONNECTATTRW != 0)
  {
    return (__api_SQLGETCONNECTATTRW)(ConnectionHandle,Attribute,ValuePtr,BufferLength,StringLengthPtr); 
  }
  else
  {
    return (__odbc_apis->__api_SQLGETCONNECTOPTION)(ConnectionHandle,Attribute,ValuePtr);
    
  }//~end if(__api_SQLGETCONNECTATTR != 0)

  return SQL_ERROR; /*never reach here*/
}

SQLRETURN __odbc_SQLSetConnectAttr(OdbcConnection*   ODBCConnection,
                                   SQLHDBC           ConnectionHandle,
                                   SQLINTEGER        Attribute,
                                   SQLPOINTER        ValuePtr,
                                   SQLINTEGER        StringLength,
                                   SQLPOINTER        AdditionValuePtr,
                                   SQLINTEGER        AdditionStringLength)
{
  struct ODBC_LIBRARY_APIS*   __odbc_apis    = ODBCConnection->__odbc_apis;
  struct ODBC_DRIVER_OPTIONS* __odbc_options = ODBCConnection->__odbc_options;

  ODBCSQLOPTIONS_ANSITOUNICODE   __options_ANSITOUNICODE = __odbc_options->__options_ANSITOUNICODE;
  ODBCSQLOPTIONS_UNICODETOANSI   __options_UNICODETOANSI = __odbc_options->__options_UNICODETOANSI;
    
  ODBCSQLAPI_SQLSETCONNECTATTR   __api_SQLSETCONNECTATTR  = __odbc_apis->__api_SQLSETCONNECTATTR;
  ODBCSQLAPI_SQLSETCONNECTATTRW  __api_SQLSETCONNECTATTRW = __odbc_apis->__api_SQLSETCONNECTATTRW;

  if(__api_SQLSETCONNECTATTR != 0)
  {
    return (__api_SQLSETCONNECTATTR)(ConnectionHandle,Attribute,ValuePtr,StringLength);
  }
  else if(__api_SQLSETCONNECTATTRW != 0)
  {
    return (__api_SQLSETCONNECTATTRW)(ConnectionHandle,Attribute,ValuePtr,StringLength);
  }
  else
  {
    if(__odbc_options->__mb_fintAvailable)
    {
      SQLRETURN __odbc_sqlreturn = SQL_SUCCESS;
      SQLINTEGER __odbc_value = *((SQLINTEGER*)AdditionValuePtr);
      __odbc_sqlreturn =  (__odbc_apis->__api_SQLSETCONNECTOPTION_INT)(ConnectionHandle,Attribute,__odbc_value);
      return __odbc_sqlreturn;
    }
    else
    {
      SQLRETURN __odbc_sqlreturn = SQL_SUCCESS;
      SQLLEN    __odbc_value = *((SQLLEN*)AdditionValuePtr);
      __odbc_sqlreturn =  (__odbc_apis->__api_SQLSETCONNECTOPTION_LEN)(ConnectionHandle,Attribute,__odbc_value);
      return __odbc_sqlreturn;
    }
  }//~end if(__api_SQLSETCONNECTATTR != 0)

  return SQL_ERROR; /*never reach here*/
}

SQLRETURN __odbc_SQLGetStmtAttr   (OdbcConnection*   ODBCConnection,
                                   SQLHSTMT          StatementHandle,
                                   SQLINTEGER        Attribute,
                                   SQLPOINTER        ValuePtr,
                                   SQLINTEGER        BufferLength,
                                   SQLINTEGER*       StringLengthPtr)
{
  struct ODBC_LIBRARY_APIS*   __odbc_apis    = ODBCConnection->__odbc_apis;
  struct ODBC_DRIVER_OPTIONS* __odbc_options = ODBCConnection->__odbc_options;

  ODBCSQLOPTIONS_ANSITOUNICODE   __options_ANSITOUNICODE = __odbc_options->__options_ANSITOUNICODE;
  ODBCSQLOPTIONS_UNICODETOANSI   __options_UNICODETOANSI = __odbc_options->__options_UNICODETOANSI;
    
  ODBCSQLAPI_SQLGETSTMTATTR  __api_SQLGETSTMTATTR  = __odbc_apis->__api_SQLGETSTMTATTR;
  ODBCSQLAPI_SQLGETSTMTATTRW __api_SQLGETSTMTATTRW = __odbc_apis->__api_SQLGETSTMTATTRW;

  if(__api_SQLGETSTMTATTR != 0)
  {
    return (__api_SQLGETSTMTATTR)(StatementHandle,Attribute,ValuePtr,BufferLength,StringLengthPtr);
  }
  else if(__api_SQLGETSTMTATTRW != 0)
  {
    return (__api_SQLGETSTMTATTRW)(StatementHandle,Attribute,ValuePtr,BufferLength,StringLengthPtr);
    
  }//~end if(__api_SQLGETSTMTATTR != 0)

  return SQL_ERROR; /*never reach here*/
}

SQLRETURN __odbc_SQLSetStmtAttr   (OdbcConnection*   ODBCConnection,
                                   SQLHSTMT          StatementHandle,
                                   SQLINTEGER        Attribute,
                                   SQLPOINTER        ValuePtr,
                                   SQLINTEGER        StringLength,
                                   SQLPOINTER        AdditionValuePtr,
                                   SQLINTEGER        AdditionStringLength)
{
  struct ODBC_LIBRARY_APIS*   __odbc_apis    = ODBCConnection->__odbc_apis;
  struct ODBC_DRIVER_OPTIONS* __odbc_options = ODBCConnection->__odbc_options;
 
  ODBCSQLOPTIONS_ANSITOUNICODE   __options_ANSITOUNICODE = __odbc_options->__options_ANSITOUNICODE;
  ODBCSQLOPTIONS_UNICODETOANSI   __options_UNICODETOANSI = __odbc_options->__options_UNICODETOANSI;
   
  ODBCSQLAPI_SQLSETSTMTATTR  __api_SQLSETSTMTATTR  = __odbc_apis->__api_SQLSETSTMTATTR;
  ODBCSQLAPI_SQLSETSTMTATTRW __api_SQLSETSTMTATTRW = __odbc_apis->__api_SQLSETSTMTATTRW;

  if(__api_SQLSETSTMTATTR != 0)
  {
    return (__api_SQLSETSTMTATTR)(StatementHandle,Attribute,ValuePtr,StringLength);
  }
  else if(__api_SQLSETSTMTATTRW != 0)
  {
    return (__api_SQLSETSTMTATTRW)(StatementHandle,Attribute,ValuePtr,StringLength);
  }
  else
  {
    if(__odbc_options->__mb_fintAvailable)
    {
      SQLRETURN __odbc_sqlreturn = SQL_SUCCESS;
      SQLINTEGER __odbc_value = *((SQLINTEGER*)AdditionValuePtr);
      __odbc_sqlreturn = (__odbc_apis->__api_SQLSETSTMTOPTION_INT)(StatementHandle,Attribute,__odbc_value);
      return __odbc_sqlreturn;
    }
    else
    {
      SQLRETURN __odbc_sqlreturn = SQL_SUCCESS;
      SQLLEN    __odbc_value = *((SQLLEN*)AdditionValuePtr);
      __odbc_sqlreturn = (__odbc_apis->__api_SQLSETSTMTOPTION_LEN)(StatementHandle,Attribute,__odbc_value);
      
      return __odbc_sqlreturn;
    }
  }//~end if(__api_SQLSETSTMTATTR != 0)

  return SQL_ERROR; /*never reach here*/
}

SQLRETURN __odbc_SQLGetDiagRec    (OdbcConnection*   ODBCConnection,
                                   SQLSMALLINT       HandleType,
                                   SQLHANDLE         Handle,
                                   SQLSMALLINT       RecNumber,
                                   SQLCHAR*          SQLState,
                                   SQLSMALLINT       StateLength,
                                   SQLSMALLINT*      StateLengthPtr,
                                   SQLINTEGER*       NativeErrorPtr,
                                   SQLCHAR*          MessageText,
                                   SQLSMALLINT       BufferLength,
                                   SQLSMALLINT*      TextLengthPtr)
{
  struct ODBC_LIBRARY_APIS*   __odbc_apis    = ODBCConnection->__odbc_apis;
  struct ODBC_DRIVER_OPTIONS* __odbc_options = ODBCConnection->__odbc_options;

  ODBCSQLOPTIONS_ANSITOUNICODE   __options_ANSITOUNICODE = __odbc_options->__options_ANSITOUNICODE;
  ODBCSQLOPTIONS_UNICODETOANSI   __options_UNICODETOANSI = __odbc_options->__options_UNICODETOANSI;
    
  ODBCSQLAPI_SQLGETDIAGREC  __api_SQLGETDIAGREC  = __odbc_apis->__api_SQLGETDIAGREC;
  ODBCSQLAPI_SQLGETDIAGRECW __api_SQLGETDIAGRECW = __odbc_apis->__api_SQLGETDIAGRECW;
  ODBCSQLAPI_SQLERROR       __api_SQLERROR       = __odbc_apis->__api_SQLERROR;

  if(__api_SQLGETDIAGREC != 0)
  {
    return (__api_SQLGETDIAGREC)(HandleType,Handle,RecNumber,SQLState,NativeErrorPtr,MessageText,BufferLength,TextLengthPtr);
  }
  else if(__api_SQLGETDIAGRECW != 0)
  {
    int __db_unicode = __odbc_options->__mi_driver_unicode;
    SQLRETURN __odbc_sqlreturn = SQL_ERROR;

    switch(__db_unicode)
    {
      case __ODBC_UNICODE_UTF32:
      {
        SQLSMALLINT   __uni_m_AllocLength  = 0;
        SQLSMALLINT*  __uni_TextLengthPtr  = 0;
        UTF32*        __uni_MessageText    = 0;
        SQLSMALLINT   __uni_TextLength      = 0;

        SQLSMALLINT   __uni_s_AllocLength  = 0;
        UTF32*        __uni_SQLState       = 0;
        SQLSMALLINT   __uni_SQLStateLength = 0;

        if((MessageText != 0) && (BufferLength > 0))
        {
          __uni_m_AllocLength  = BufferLength + 1;
          __uni_MessageText    = new UTF32[__uni_m_AllocLength];
          memset((char*)__uni_MessageText,0,sizeof(UTF32) * __uni_m_AllocLength);

          __uni_TextLengthPtr = &__uni_TextLength;
        }

        if((SQLState != 0) && (StateLength > 0))
        {
          __uni_SQLStateLength = StateLength;
          __uni_s_AllocLength  =  __uni_SQLStateLength+ 1;
          __uni_SQLState       = new UTF32[__uni_s_AllocLength];
          memset((char*)__uni_SQLState,0,sizeof(UTF32) * __uni_s_AllocLength);

        }
        __odbc_sqlreturn = (__api_SQLGETDIAGRECW)(HandleType,Handle,RecNumber,(SQLWCHAR*)__uni_SQLState,NativeErrorPtr,(SQLWCHAR*)__uni_MessageText,__uni_m_AllocLength,__uni_TextLengthPtr);

        if(__uni_MessageText != 0)
        {
          int __size_TextLength = (__options_UNICODETOANSI)(MessageText,BufferLength,__db_unicode,(SQLCHAR*)__uni_MessageText,__uni_TextLength);
          if(TextLengthPtr != 0)
          {
            *TextLengthPtr = (SQLSMALLINT)__size_TextLength;
          }

          delete []__uni_MessageText;
        }


        if(__uni_SQLState != 0)
        {
          int __size_TextLength = (__options_UNICODETOANSI)(SQLState,__uni_SQLStateLength,__db_unicode,(SQLCHAR*)__uni_SQLState,__uni_s_AllocLength);
          delete []__uni_SQLState;
        }
      }
      break;

      case __ODBC_UNICODE_UTF16:
      {
        SQLSMALLINT   __uni_m_AllocLength  = 0;
        SQLSMALLINT*  __uni_TextLengthPtr  = 0;
        UTF16*        __uni_MessageText    = 0;
        SQLSMALLINT   __uni_TextLength      = 0;

        SQLSMALLINT   __uni_s_AllocLength  = 0;
        UTF16*        __uni_SQLState       = 0;
        SQLSMALLINT   __uni_SQLStateLength = 0;

        if((MessageText != 0) && (BufferLength > 0))
        {
          __uni_m_AllocLength  = BufferLength + 1;
          __uni_MessageText    = new UTF16[__uni_m_AllocLength];
          memset((char*)__uni_MessageText,0,sizeof(UTF16) * __uni_m_AllocLength);

          __uni_TextLengthPtr = &__uni_TextLength;
        }

        if((SQLState != 0) && (StateLength > 0))
        {
          __uni_SQLStateLength = StateLength;
          __uni_s_AllocLength  =  __uni_SQLStateLength+ 1;
          __uni_SQLState       = new UTF16[__uni_s_AllocLength];
          memset((char*)__uni_SQLState,0,sizeof(UTF16) * __uni_s_AllocLength);

        }
        __odbc_sqlreturn = (__api_SQLGETDIAGRECW)(HandleType,Handle,RecNumber,(SQLWCHAR*)__uni_SQLState,NativeErrorPtr,(SQLWCHAR*)__uni_MessageText,__uni_m_AllocLength,__uni_TextLengthPtr);

        if(__uni_MessageText != 0)
        {
          int __size_TextLength = (__options_UNICODETOANSI)(MessageText,BufferLength,__db_unicode,(SQLCHAR*)__uni_MessageText,__uni_TextLength);
          if(TextLengthPtr != 0)
          {
            *TextLengthPtr = (SQLSMALLINT)__size_TextLength;
          }

          delete []__uni_MessageText;
        }


        if(__uni_SQLState != 0)
        {
          int __size_TextLength = (__options_UNICODETOANSI)(SQLState,__uni_SQLStateLength,__db_unicode,(SQLCHAR*)__uni_SQLState,__uni_s_AllocLength);
          delete []__uni_SQLState;
        }
      }
      break;

      case __ODBC_UNICODE_UTF8:
      default:
      {
        SQLSMALLINT   __uni_m_AllocLength  = 0;
        SQLSMALLINT*  __uni_TextLengthPtr  = 0;
        UTF8*        __uni_MessageText    = 0;
        SQLSMALLINT   __uni_TextLength      = 0;

        SQLSMALLINT   __uni_s_AllocLength  = 0;
        UTF8*        __uni_SQLState       = 0;
        SQLSMALLINT   __uni_SQLStateLength = 0;

        if((MessageText != 0) && (BufferLength > 0))
        {
          __uni_m_AllocLength  = BufferLength * 6 + 1;
          __uni_MessageText    = new UTF8[__uni_m_AllocLength];
          memset((char*)__uni_MessageText,0,sizeof(UTF8) * __uni_m_AllocLength);

          __uni_TextLengthPtr = &__uni_TextLength;
        }

        if((SQLState != 0) && (StateLength > 0))
        {
          __uni_SQLStateLength = StateLength;
          __uni_s_AllocLength  =  __uni_SQLStateLength * 6 + 1;
          __uni_SQLState       = new UTF8[__uni_s_AllocLength];
          memset((char*)__uni_SQLState,0,sizeof(UTF8) * __uni_s_AllocLength);

        }
        __odbc_sqlreturn = (__api_SQLGETDIAGRECW)(HandleType,Handle,RecNumber,(SQLWCHAR*)__uni_SQLState,NativeErrorPtr,(SQLWCHAR*)__uni_MessageText,__uni_m_AllocLength,__uni_TextLengthPtr);

        if(__uni_MessageText != 0)
        {
          int __size_TextLength = (__options_UNICODETOANSI)(MessageText,BufferLength,__db_unicode,(SQLCHAR*)__uni_MessageText,__uni_TextLength);
          if(TextLengthPtr != 0)
          {
            *TextLengthPtr = (SQLSMALLINT)__size_TextLength;
          }

          delete []__uni_MessageText;
        }


        if(__uni_SQLState != 0)
        {
          int __size_TextLength = (__options_UNICODETOANSI)(SQLState,__uni_SQLStateLength,__db_unicode,(SQLCHAR*)__uni_SQLState,__uni_s_AllocLength);
          delete []__uni_SQLState;
        }
      }
      break;

    }//~end switch(__db_unicode)

    return __odbc_sqlreturn;
  }
  else
  {

    SQLHENV  odbc_henv  =  SQL_NULL_HENV;
    SQLHDBC  odbc_hdbc  =  SQL_NULL_HDBC;
    SQLHSTMT odbc_hstmt =  SQL_NULL_HSTMT;

    switch( HandleType)
    {
      case SQL_HANDLE_ENV:
      {
        odbc_henv  =  (SQLHENV)Handle;
        odbc_hdbc  =  SQL_NULL_HDBC;
        odbc_hstmt =  SQL_NULL_HSTMT;
      }
      break;

      case SQL_HANDLE_DBC:
      {
        odbc_henv  =  ODBCConnection->odbc_henv;
        odbc_hdbc  =  (SQLHDBC)Handle;
        odbc_hstmt =  SQL_NULL_HSTMT;
      }
      break;

      case SQL_HANDLE_STMT:
      {
        odbc_henv  =  ODBCConnection->odbc_henv;
        odbc_hdbc  =  ODBCConnection->odbc_hdbc;
        odbc_hstmt =  (SQLHSTMT)Handle;
      }
      break;

    }//~end switch( HandleType)

    return (__api_SQLERROR)(odbc_henv,odbc_hdbc,odbc_hstmt,SQLState,NativeErrorPtr,MessageText,BufferLength,TextLengthPtr);

  }//~end if(__api_SQLGETDIAGREC != 0)

  return SQL_ERROR; /*never reach here*/
}

SQLRETURN  __odbc_SQLDriverConnect   (OdbcConnection*   ODBCConnection,
                                       SQLHDBC          ConnectionHandle,
                                       SQLHWND          WindowHandle,
                                       SQLCHAR*         InConnectionString,
                                       SQLSMALLINT      InConnectionStringLength,
                                       SQLCHAR*         OutConnectionString,
                                       SQLSMALLINT      OutConnectionStringCapacity,
                                       SQLSMALLINT*     OutConnectionStringLengthPtr,
                                       SQLUSMALLINT     DriverCompletion)
{
  struct ODBC_LIBRARY_APIS*   __odbc_apis    = ODBCConnection->__odbc_apis;
  struct ODBC_DRIVER_OPTIONS* __odbc_options = ODBCConnection->__odbc_options;

  ODBCSQLOPTIONS_ANSITOUNICODE   __options_ANSITOUNICODE = __odbc_options->__options_ANSITOUNICODE;
  ODBCSQLOPTIONS_UNICODETOANSI   __options_UNICODETOANSI = __odbc_options->__options_UNICODETOANSI;
    
  ODBCSQLAPI_SQLDRIVERCONNECT  __api_SQLDRIVERCONNECT  = __odbc_apis->__api_SQLDRIVERCONNECT;
  ODBCSQLAPI_SQLDRIVERCONNECTW __api_SQLDRIVERCONNECTW = __odbc_apis->__api_SQLDRIVERCONNECTW;

  if(__api_SQLDRIVERCONNECT != 0)
  {
    return (__api_SQLDRIVERCONNECT)(ConnectionHandle,
                                    WindowHandle,
                                    InConnectionString,
                                    InConnectionStringLength,
                                    OutConnectionString,
                                    OutConnectionStringCapacity,
                                    OutConnectionStringLengthPtr,
                                    DriverCompletion);
  }
  else if (__api_SQLDRIVERCONNECTW != 0)
  {
    int __db_unicode = __odbc_options->__mi_driver_unicode;
    SQLRETURN __odbc_sqlreturn = SQL_ERROR;

    switch(__db_unicode)
    {
//-------------------------------------------------------------
      case __ODBC_UNICODE_UTF32:
      {
        SQLINTEGER __uni_AllocLength = 0;
        int    __uni_InConnectionStringLength = 0;
        UTF32* __uni_InConnectionString = 0;
        (__options_ANSITOUNICODE)((SQLPOINTER*)&__uni_InConnectionString,&__uni_AllocLength,&__uni_InConnectionStringLength,__db_unicode,InConnectionString,InConnectionStringLength);

        SQLSMALLINT   __uni_o_AllocLength                  = 0;
        SQLSMALLINT*  __uni_OutConnectionStringLengthPtr   = 0;
        UTF32*        __uni_OutConnectionString            = 0;
        SQLSMALLINT   __uni_OutConnectionStringLength      = 0;

        if((OutConnectionString != 0) && (OutConnectionStringCapacity > 0))
        {
          __uni_o_AllocLength = OutConnectionStringCapacity + 1;
          __uni_OutConnectionString   = new UTF32[__uni_o_AllocLength];
          memset((char*)__uni_OutConnectionString,0,sizeof(UTF32) * __uni_o_AllocLength);

          __uni_OutConnectionStringLengthPtr = &__uni_OutConnectionStringLength;
        }

        __odbc_sqlreturn = (__api_SQLDRIVERCONNECTW)(ConnectionHandle,
                                                     WindowHandle,
                                                     (SQLWCHAR*)__uni_InConnectionString,
                                                     __uni_InConnectionStringLength,
                                                     (SQLWCHAR*)__uni_OutConnectionString,
                                                     __uni_o_AllocLength,
                                                     __uni_OutConnectionStringLengthPtr,
                                                     DriverCompletion);

        if(__uni_OutConnectionString != 0)
        {
          int __size_uni_OutConnectionStringLength = (__options_UNICODETOANSI)(OutConnectionString,OutConnectionStringCapacity,__db_unicode,(SQLCHAR*)__uni_OutConnectionString,__uni_OutConnectionStringLength);

          if(OutConnectionStringLengthPtr != 0)
          {
            *OutConnectionStringLengthPtr = (SQLSMALLINT)__size_uni_OutConnectionStringLength;
          }

          delete []__uni_OutConnectionString;
        }

        if(__uni_InConnectionString != 0)
        {
          delete []__uni_InConnectionString;
        }

      }
      break;
//-------------------------------------------------------------
      case __ODBC_UNICODE_UTF16:
      {
        SQLINTEGER __uni_AllocLength = 0;
        int    __uni_InConnectionStringLength = 0;
        UTF16* __uni_InConnectionString = 0;
        (__options_ANSITOUNICODE)((SQLPOINTER*)&__uni_InConnectionString,&__uni_AllocLength,&__uni_InConnectionStringLength,__db_unicode,InConnectionString,InConnectionStringLength);

        SQLSMALLINT   __uni_o_AllocLength                  = 0;
        SQLSMALLINT*  __uni_OutConnectionStringLengthPtr   = 0;
        UTF16*        __uni_OutConnectionString            = 0;
        SQLSMALLINT   __uni_OutConnectionStringLength      = 0;

        if((OutConnectionString != 0) && (OutConnectionStringCapacity > 0))
        {
          __uni_o_AllocLength = OutConnectionStringCapacity + 1;
          __uni_OutConnectionString   = new UTF16[__uni_o_AllocLength];
          memset((char*)__uni_OutConnectionString,0,sizeof(UTF16) * __uni_o_AllocLength);

          __uni_OutConnectionStringLengthPtr = &__uni_OutConnectionStringLength;
        }

        __odbc_sqlreturn = (__api_SQLDRIVERCONNECTW)(ConnectionHandle,
                                                     WindowHandle,
                                                     (SQLWCHAR*)__uni_InConnectionString,
                                                     __uni_InConnectionStringLength,
                                                     (SQLWCHAR*)__uni_OutConnectionString,
                                                     __uni_o_AllocLength,
                                                     __uni_OutConnectionStringLengthPtr,
                                                     DriverCompletion);

        if(__uni_OutConnectionString != 0)
        {
          int __size_uni_OutConnectionStringLength = (__options_UNICODETOANSI)(OutConnectionString,OutConnectionStringCapacity,__db_unicode,(SQLCHAR*)__uni_OutConnectionString,__uni_OutConnectionStringLength);

          if(OutConnectionStringLengthPtr != 0)
          {
            *OutConnectionStringLengthPtr = (SQLSMALLINT)__size_uni_OutConnectionStringLength;
          }

          delete []__uni_OutConnectionString;
        }

        if(__uni_InConnectionString != 0)
        {
          delete []__uni_InConnectionString;
        }
      }
      break;
//-------------------------------------------------------------
      case __ODBC_UNICODE_UTF8:
      default:
      {
        SQLINTEGER __uni_AllocLength = 0;
        int    __uni_InConnectionStringLength = 0;
        UTF8* __uni_InConnectionString = 0;
        (__options_ANSITOUNICODE)((SQLPOINTER*)&__uni_InConnectionString,&__uni_AllocLength,&__uni_InConnectionStringLength,__db_unicode,InConnectionString,InConnectionStringLength);

        SQLSMALLINT   __uni_o_AllocLength                  = 0;
        SQLSMALLINT*  __uni_OutConnectionStringLengthPtr   = 0;
        UTF8*        __uni_OutConnectionString            = 0;
        SQLSMALLINT   __uni_OutConnectionStringLength      = 0;

        if((OutConnectionString != 0) && (OutConnectionStringCapacity > 0))
        {
          __uni_o_AllocLength = OutConnectionStringCapacity * 6 + 1;
          __uni_OutConnectionString   = new UTF8[__uni_o_AllocLength];
          memset((char*)__uni_OutConnectionString,0,sizeof(UTF8) * __uni_o_AllocLength);

          __uni_OutConnectionStringLengthPtr = &__uni_OutConnectionStringLength;
        }

        __odbc_sqlreturn = (__api_SQLDRIVERCONNECTW)(ConnectionHandle,
                                                     WindowHandle,
                                                     (SQLWCHAR*)__uni_InConnectionString,
                                                     __uni_InConnectionStringLength,
                                                     (SQLWCHAR*)__uni_OutConnectionString,
                                                     __uni_o_AllocLength,
                                                     __uni_OutConnectionStringLengthPtr,
                                                     DriverCompletion);

        if(__uni_OutConnectionString != 0)
        {
          int __size_uni_OutConnectionStringLength = (__options_UNICODETOANSI)(OutConnectionString,OutConnectionStringCapacity,__db_unicode,(SQLCHAR*)__uni_OutConnectionString,__uni_OutConnectionStringLength);

          if(OutConnectionStringLengthPtr != 0)
          {
            *OutConnectionStringLengthPtr = (SQLSMALLINT)__size_uni_OutConnectionStringLength;
          }

          delete []__uni_OutConnectionString;
        }

        if(__uni_InConnectionString != 0)
        {
          delete []__uni_InConnectionString;
        }
      }
      break;
//-------------------------------------------------------------
    }//~end switch(__db_unicode)

    return __odbc_sqlreturn;
  }
  else
  {
    return SQL_ERROR;
  }

  return SQL_ERROR; /*never reach here*/
}

SQLRETURN   __odbc_SQLExecDirect     (OdbcConnection*   ODBCConnection,
                                      SQLHSTMT          StatementHandle,
                                      SQLCHAR*          StatementText,
                                      SQLINTEGER        TextLength)
{
  struct ODBC_LIBRARY_APIS*   __odbc_apis    = ODBCConnection->__odbc_apis;
  struct ODBC_DRIVER_OPTIONS* __odbc_options = ODBCConnection->__odbc_options;

  ODBCSQLOPTIONS_ANSITOUNICODE   __options_ANSITOUNICODE = __odbc_options->__options_ANSITOUNICODE;
  ODBCSQLOPTIONS_UNICODETOANSI   __options_UNICODETOANSI = __odbc_options->__options_UNICODETOANSI;
    
  ODBCSQLAPI_SQLEXECDIRECT  __api_SQLEXECDIRECT  = __odbc_apis->__api_SQLEXECDIRECT;
  ODBCSQLAPI_SQLEXECDIRECTW __api_SQLEXECDIRECTW = __odbc_apis->__api_SQLEXECDIRECTW;
  if(__api_SQLEXECDIRECT != 0)
  {
    return (__api_SQLEXECDIRECT)(StatementHandle,StatementText,TextLength);
  }
  else if (__api_SQLEXECDIRECTW != 0)
  {
    int __db_unicode = __odbc_options->__mi_driver_unicode;
    SQLRETURN __odbc_sqlreturn = SQL_ERROR;

    switch(__db_unicode)
    {
//-------------------------------------------------------------
      case __ODBC_UNICODE_UTF32:
      {
        SQLINTEGER __uni_AllocLength = 0;
        int    __uni_TextLength = 0;
        UTF32* __uni_StatementText = 0;
        (__options_ANSITOUNICODE)((SQLPOINTER*)&__uni_StatementText,&__uni_AllocLength,&__uni_TextLength,__db_unicode,StatementText,TextLength);

        __odbc_sqlreturn = (__api_SQLEXECDIRECTW)(StatementHandle,(SQLWCHAR*)__uni_StatementText,__uni_TextLength);

        if(__uni_StatementText != 0)
        {
          delete []__uni_StatementText;
        }
      }
      break;
//-------------------------------------------------------------
      case __ODBC_UNICODE_UTF16:
      {
        SQLINTEGER __uni_AllocLength = 0;
        int    __uni_TextLength = 0;
        UTF16* __uni_StatementText = 0;
        (__options_ANSITOUNICODE)((SQLPOINTER*)&__uni_StatementText,&__uni_AllocLength,&__uni_TextLength,__db_unicode,StatementText,TextLength);

        __odbc_sqlreturn = (__api_SQLEXECDIRECTW)(StatementHandle,(SQLWCHAR*)__uni_StatementText,__uni_TextLength);

        if(__uni_StatementText != 0)
        {
          delete []__uni_StatementText;
        }
      }
      break;
//-------------------------------------------------------------
      case __ODBC_UNICODE_UTF8:
      default:
      {
        SQLINTEGER __uni_AllocLength = 0;
        int    __uni_TextLength = 0;
        UTF8* __uni_StatementText = 0;
        (__options_ANSITOUNICODE)((SQLPOINTER*)&__uni_StatementText,&__uni_AllocLength,&__uni_TextLength,__db_unicode,StatementText,TextLength);

        __odbc_sqlreturn = (__api_SQLEXECDIRECTW)(StatementHandle,(SQLWCHAR*)__uni_StatementText,__uni_TextLength);

        if(__uni_StatementText != 0)
        {
          delete []__uni_StatementText;
        }
      }
      break;
//-------------------------------------------------------------
    }//~end switch(__db_unicode)

    return __odbc_sqlreturn;
  }
  else
  {
    return SQL_ERROR;
  }

  return SQL_ERROR; /*never reach here*/
}

SQLRETURN   __odbc_SQLGetInfo       (OdbcConnection*   ODBCConnection,
                                     SQLHDBC           ConnectionHandle,
                                     SQLUSMALLINT      InfoType,
                                     SQLPOINTER        InfoValuePtr,
                                     SQLSMALLINT       BufferLength,
                                     SQLSMALLINT*      StringLengthPtr)
{
  struct ODBC_LIBRARY_APIS*   __odbc_apis    = ODBCConnection->__odbc_apis;
  struct ODBC_DRIVER_OPTIONS* __odbc_options = ODBCConnection->__odbc_options;

  ODBCSQLOPTIONS_ANSITOUNICODE   __options_ANSITOUNICODE = __odbc_options->__options_ANSITOUNICODE;
  ODBCSQLOPTIONS_UNICODETOANSI   __options_UNICODETOANSI = __odbc_options->__options_UNICODETOANSI;
    
  ODBCSQLAPI_SQLGETINFO  __api_SQLGETINFO  = __odbc_apis->__api_SQLGETINFO;
  ODBCSQLAPI_SQLGETINFOW __api_SQLGETINFOW = __odbc_apis->__api_SQLGETINFOW;

  if(__api_SQLGETINFO != 0)
  {
    return (__api_SQLGETINFO)(ConnectionHandle,InfoType,InfoValuePtr,BufferLength,StringLengthPtr);
  }
  else if (__api_SQLGETINFOW != 0)
  {
    int __db_unicode = __odbc_options->__mi_driver_unicode;
    SQLRETURN __odbc_sqlreturn = SQL_ERROR;

    switch(__db_unicode)
    {
//-------------------------------------------------------------
      case __ODBC_UNICODE_UTF32:
      {
        SQLSMALLINT   __uni_AllocLength     = 0;
        SQLSMALLINT*  __uni_StringLengthPtr = 0;
        UTF32*        __uni_InfoValue       = 0;
        SQLSMALLINT   __uni_StringLength    = 0;

        if((InfoValuePtr != 0) && (BufferLength > 0))
        {
          __uni_AllocLength = BufferLength + 1;
          __uni_InfoValue   = new UTF32[__uni_AllocLength];
          memset((char*)__uni_InfoValue,0,sizeof(UTF32) * __uni_AllocLength);

          __uni_StringLengthPtr = &__uni_StringLength;
        }

        __odbc_sqlreturn = (__api_SQLGETINFOW)(ConnectionHandle,InfoType,(SQLWCHAR*)__uni_InfoValue,__uni_AllocLength,__uni_StringLengthPtr);

        if(__uni_InfoValue != 0)
        {
          int __size_InfoValueLength = (__options_UNICODETOANSI)((SQLCHAR*)(InfoValuePtr),BufferLength,__db_unicode,(SQLCHAR*)__uni_InfoValue,__uni_StringLength);
          if(StringLengthPtr != 0)
          {
            *StringLengthPtr = (SQLSMALLINT)__size_InfoValueLength;
          }

          delete []__uni_InfoValue;
        }
      }
      break;
//-------------------------------------------------------------
      case __ODBC_UNICODE_UTF16:
      {
        SQLSMALLINT   __uni_AllocLength     = 0;
        SQLSMALLINT*  __uni_StringLengthPtr = 0;
        UTF16*        __uni_InfoValue       = 0;
        SQLSMALLINT   __uni_StringLength    = 0;

        if((InfoValuePtr != 0) && (BufferLength > 0))
        {
          __uni_AllocLength = BufferLength + 1;
          __uni_InfoValue   = new UTF16[__uni_AllocLength];
          memset((char*)__uni_InfoValue,0,sizeof(UTF16) * __uni_AllocLength);

          __uni_StringLengthPtr = &__uni_StringLength;
        }

        __odbc_sqlreturn = (__api_SQLGETINFOW)(ConnectionHandle,InfoType,(SQLWCHAR*)__uni_InfoValue,__uni_AllocLength,__uni_StringLengthPtr);

        if(__uni_InfoValue != 0)
        {
          int __size_InfoValueLength = (__options_UNICODETOANSI)((SQLCHAR*)(InfoValuePtr),BufferLength,__db_unicode,(SQLCHAR*)__uni_InfoValue,__uni_StringLength);
          if(StringLengthPtr != 0)
          {
            *StringLengthPtr = (SQLSMALLINT)__size_InfoValueLength;
          }

          delete []__uni_InfoValue;
        }
      }
      break;
//-------------------------------------------------------------
      case __ODBC_UNICODE_UTF8:
      default:
      {
        SQLSMALLINT   __uni_AllocLength     = 0;
        SQLSMALLINT*  __uni_StringLengthPtr = 0;
        UTF8*         __uni_InfoValue       = 0;
        SQLSMALLINT   __uni_StringLength    = 0;

        if((InfoValuePtr != 0) && (BufferLength > 0))
        {
          __uni_AllocLength = BufferLength * 6 + 1;
          __uni_InfoValue   = new UTF8[__uni_AllocLength];
          memset((char*)__uni_InfoValue,0,sizeof(UTF8) * __uni_AllocLength);

          __uni_StringLengthPtr = &__uni_StringLength;
        }

        __odbc_sqlreturn = (__api_SQLGETINFOW)(ConnectionHandle,InfoType,(SQLWCHAR*)__uni_InfoValue,__uni_AllocLength,__uni_StringLengthPtr);

        if(__uni_InfoValue != 0)
        {
          int __size_InfoValueLength = (__options_UNICODETOANSI)((SQLCHAR*)(InfoValuePtr),BufferLength,__db_unicode,(SQLCHAR*)__uni_InfoValue,__uni_StringLength);
          if(StringLengthPtr != 0)
          {
            *StringLengthPtr = (SQLSMALLINT)__size_InfoValueLength;
          }

          delete []__uni_InfoValue;
        }
      }
      break;
//-------------------------------------------------------------
    }//~end switch(__db_unicode)

    return __odbc_sqlreturn;
  }
  else
  {
    return SQL_ERROR;
  }

  return SQL_ERROR; /*never reach here*/
}

SQLRETURN   __odbc_SQLPrepare        (OdbcConnection*   ODBCConnection,
                                      SQLHSTMT          StatementHandle,
                                      SQLCHAR*          StatementText,
                                      SQLINTEGER        TextLength)
{
  struct ODBC_LIBRARY_APIS*   __odbc_apis    = ODBCConnection->__odbc_apis;
  struct ODBC_DRIVER_OPTIONS* __odbc_options = ODBCConnection->__odbc_options;

  ODBCSQLOPTIONS_ANSITOUNICODE   __options_ANSITOUNICODE = __odbc_options->__options_ANSITOUNICODE;
  ODBCSQLOPTIONS_UNICODETOANSI   __options_UNICODETOANSI = __odbc_options->__options_UNICODETOANSI;
    
  ODBCSQLAPI_SQLPREPARE  __api_SQLPREPARE  = __odbc_apis->__api_SQLPREPARE;
  ODBCSQLAPI_SQLPREPAREW __api_SQLPREPAREW = __odbc_apis->__api_SQLPREPAREW;

  if(__api_SQLPREPARE != 0)
  {
    return (__api_SQLPREPARE)(StatementHandle,StatementText,TextLength);
  }
  else if (__api_SQLPREPAREW != 0)
  {
    int __db_unicode = __odbc_options->__mi_driver_unicode;
    SQLRETURN __odbc_sqlreturn = SQL_ERROR;

    switch(__db_unicode)
    {
//-------------------------------------------------------------
      case __ODBC_UNICODE_UTF32:
      {
        SQLINTEGER __uni_AllocLength = 0;
        int    __uni_TextLength = 0;
        UTF32* __uni_StatementText = 0;
        (__options_ANSITOUNICODE)((SQLPOINTER*)&__uni_StatementText,&__uni_AllocLength,&__uni_TextLength,__db_unicode,StatementText,TextLength);

        __odbc_sqlreturn = (__api_SQLPREPAREW)(StatementHandle,(SQLWCHAR*)__uni_StatementText,__uni_TextLength);

        if(__uni_StatementText != 0)
        {
          delete []__uni_StatementText;
        }
      }
      break;
//-------------------------------------------------------------
      case __ODBC_UNICODE_UTF16:
      {
        SQLINTEGER __uni_AllocLength = 0;
        int    __uni_TextLength = 0;
        UTF16* __uni_StatementText = 0;
        (__options_ANSITOUNICODE)((SQLPOINTER*)&__uni_StatementText,&__uni_AllocLength,&__uni_TextLength,__db_unicode,StatementText,TextLength);

        __odbc_sqlreturn = (__api_SQLPREPAREW)(StatementHandle,(SQLWCHAR*)__uni_StatementText,__uni_TextLength);

        if(__uni_StatementText != 0)
        {
          delete []__uni_StatementText;
        }
      }
      break;
//-------------------------------------------------------------
      case __ODBC_UNICODE_UTF8:
      default:
      {
        SQLINTEGER __uni_AllocLength = 0;
        int    __uni_TextLength = 0;
        UTF8* __uni_StatementText = 0;
        (__options_ANSITOUNICODE)((SQLPOINTER*)&__uni_StatementText,&__uni_AllocLength,&__uni_TextLength,__db_unicode,StatementText,TextLength);

        __odbc_sqlreturn = (__api_SQLPREPAREW)(StatementHandle,(SQLWCHAR*)__uni_StatementText,__uni_TextLength);

        if(__uni_StatementText != 0)
        {
          delete []__uni_StatementText;
        }
      }
      break;
//-------------------------------------------------------------
    }//~end switch(__db_unicode)

    return __odbc_sqlreturn;
  }
  else
  {
    return SQL_ERROR;
  }

  return SQL_ERROR; /*never reach here*/
}


SQLRETURN   __odbc_SQLProcedureColumns(OdbcConnection*   ODBCConnection,
                                       SQLHSTMT          StatementHandle,
                                       SQLCHAR*          CatalogName,
                                       SQLSMALLINT       NameLength1,
                                       SQLCHAR*          SchemaName,
                                       SQLSMALLINT       NameLength2,
                                       SQLCHAR*          ProcName,
                                       SQLSMALLINT       NameLength3,
                                       SQLCHAR*          ColumnName,
                                       SQLSMALLINT       NameLength4)
{
  struct ODBC_LIBRARY_APIS*   __odbc_apis    = ODBCConnection->__odbc_apis;
  struct ODBC_DRIVER_OPTIONS* __odbc_options = ODBCConnection->__odbc_options;

  ODBCSQLOPTIONS_ANSITOUNICODE   __options_ANSITOUNICODE = __odbc_options->__options_ANSITOUNICODE;
  ODBCSQLOPTIONS_UNICODETOANSI   __options_UNICODETOANSI = __odbc_options->__options_UNICODETOANSI;
    
  ODBCSQLAPI_SQLPROCEDURECOLUMNS  __api_SQLPROCEDURECOLUMNS  = __odbc_apis->__api_SQLPROCEDURECOLUMNS;
  ODBCSQLAPI_SQLPROCEDURECOLUMNSW __api_SQLPROCEDURECOLUMNSW = __odbc_apis->__api_SQLPROCEDURECOLUMNSW;

  if(__api_SQLPROCEDURECOLUMNS != 0)
  {
    return (__api_SQLPROCEDURECOLUMNS)(StatementHandle,CatalogName,NameLength1,SchemaName,NameLength2,ProcName,NameLength3,ColumnName,NameLength4);
  }
  else if (__api_SQLPROCEDURECOLUMNSW != 0)
  {
    int __db_unicode = __odbc_options->__mi_driver_unicode;
    SQLRETURN __odbc_sqlreturn = SQL_ERROR;

    switch(__db_unicode)
    {
//-------------------------------------------------------------
      case __ODBC_UNICODE_UTF32:
      {
        SQLINTEGER __uni_AllocLength = 0;

        int    __uni_NameLength1 = 0;
        UTF32* __uni_CatalogName = 0;
        (__options_ANSITOUNICODE)((SQLPOINTER*)&__uni_CatalogName,&__uni_AllocLength,&__uni_NameLength1,__db_unicode,CatalogName,NameLength1);

        int    __uni_NameLength2 = 0;
        UTF32* __uni_SchemaName = 0;
        (__options_ANSITOUNICODE)((SQLPOINTER*)&__uni_SchemaName,&__uni_AllocLength,&__uni_NameLength2,__db_unicode,SchemaName,NameLength2);

        int    __uni_NameLength3 = 0;
        UTF32* __uni_ProcName = 0;
        (__options_ANSITOUNICODE)((SQLPOINTER*)&__uni_ProcName,&__uni_AllocLength,&__uni_NameLength3,__db_unicode,ProcName,NameLength3);

        int    __uni_NameLength4 = 0;
        UTF32* __uni_ColumnName = 0;
        (__options_ANSITOUNICODE)((SQLPOINTER*)&__uni_ColumnName,&__uni_AllocLength,&__uni_NameLength4,__db_unicode,ColumnName,NameLength4);

        __odbc_sqlreturn = (__api_SQLPROCEDURECOLUMNSW)(StatementHandle,(SQLWCHAR*)__uni_CatalogName,__uni_NameLength1,(SQLWCHAR*)__uni_SchemaName,__uni_NameLength2,(SQLWCHAR*)__uni_ProcName,__uni_NameLength3,(SQLWCHAR*)__uni_ColumnName,__uni_NameLength4);

        if(__uni_CatalogName != 0)
        {
          delete []__uni_CatalogName;
        }

        if(__uni_SchemaName != 0)
        {
          delete []__uni_SchemaName;
        }

        if(__uni_ProcName != 0)
        {
          delete []__uni_ProcName;
        }

        if(__uni_ColumnName != 0)
        {
          delete []__uni_ColumnName;
        }

      }
      break;
//-------------------------------------------------------------
      case __ODBC_UNICODE_UTF16:
      {
        SQLINTEGER __uni_AllocLength = 0;

        int    __uni_NameLength1 = 0;
        UTF16* __uni_CatalogName = 0;
        (__options_ANSITOUNICODE)((SQLPOINTER*)&__uni_CatalogName,&__uni_AllocLength,&__uni_NameLength1,__db_unicode,CatalogName,NameLength1);

        int    __uni_NameLength2 = 0;
        UTF16* __uni_SchemaName = 0;
        (__options_ANSITOUNICODE)((SQLPOINTER*)&__uni_SchemaName,&__uni_AllocLength,&__uni_NameLength2,__db_unicode,SchemaName,NameLength2);

        int    __uni_NameLength3 = 0;
        UTF16* __uni_ProcName = 0;
        (__options_ANSITOUNICODE)((SQLPOINTER*)&__uni_ProcName,&__uni_AllocLength,&__uni_NameLength3,__db_unicode,ProcName,NameLength3);

        int    __uni_NameLength4 = 0;
        UTF16* __uni_ColumnName = 0;
        (__options_ANSITOUNICODE)((SQLPOINTER*)&__uni_ColumnName,&__uni_AllocLength,&__uni_NameLength4,__db_unicode,ColumnName,NameLength4);

        __odbc_sqlreturn = (__api_SQLPROCEDURECOLUMNSW)(StatementHandle,(SQLWCHAR*)__uni_CatalogName,__uni_NameLength1,(SQLWCHAR*)__uni_SchemaName,__uni_NameLength2,(SQLWCHAR*)__uni_ProcName,__uni_NameLength3,(SQLWCHAR*)__uni_ColumnName,__uni_NameLength4);

        if(__uni_CatalogName != 0)
        {
          delete []__uni_CatalogName;
        }

        if(__uni_SchemaName != 0)
        {
          delete []__uni_SchemaName;
        }

        if(__uni_ProcName != 0)
        {
          delete []__uni_ProcName;
        }

        if(__uni_ColumnName != 0)
        {
          delete []__uni_ColumnName;
        }
      }
      break;
//-------------------------------------------------------------
      case __ODBC_UNICODE_UTF8:
      default:
      {
        SQLINTEGER __uni_AllocLength = 0;

        int    __uni_NameLength1 = 0;
        UTF8* __uni_CatalogName = 0;
        (__options_ANSITOUNICODE)((SQLPOINTER*)&__uni_CatalogName,&__uni_AllocLength,&__uni_NameLength1,__db_unicode,CatalogName,NameLength1);

        int    __uni_NameLength2 = 0;
        UTF8* __uni_SchemaName = 0;
        (__options_ANSITOUNICODE)((SQLPOINTER*)&__uni_SchemaName,&__uni_AllocLength,&__uni_NameLength2,__db_unicode,SchemaName,NameLength2);

        int    __uni_NameLength3 = 0;
        UTF8* __uni_ProcName = 0;
        (__options_ANSITOUNICODE)((SQLPOINTER*)&__uni_ProcName,&__uni_AllocLength,&__uni_NameLength3,__db_unicode,ProcName,NameLength3);

        int    __uni_NameLength4 = 0;
        UTF8* __uni_ColumnName = 0;
        (__options_ANSITOUNICODE)((SQLPOINTER*)&__uni_ColumnName,&__uni_AllocLength,&__uni_NameLength4,__db_unicode,ColumnName,NameLength4);

        __odbc_sqlreturn = (__api_SQLPROCEDURECOLUMNSW)(StatementHandle,(SQLWCHAR*)__uni_CatalogName,__uni_NameLength1,(SQLWCHAR*)__uni_SchemaName,__uni_NameLength2,(SQLWCHAR*)__uni_ProcName,__uni_NameLength3,(SQLWCHAR*)__uni_ColumnName,__uni_NameLength4);

        if(__uni_CatalogName != 0)
        {
          delete []__uni_CatalogName;
        }

        if(__uni_SchemaName != 0)
        {
          delete []__uni_SchemaName;
        }

        if(__uni_ProcName != 0)
        {
          delete []__uni_ProcName;
        }

        if(__uni_ColumnName != 0)
        {
          delete []__uni_ColumnName;
        }
      }
      break;
//-------------------------------------------------------------
    }//~end switch(__db_unicode)

    return __odbc_sqlreturn;
  }
  else
  {
    return SQL_ERROR;
  }

  return SQL_ERROR; /*never reach here*/
}

__OPENCODE_END_NAMESPACE
