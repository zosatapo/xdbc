/**
 *  xmarco.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */

#ifndef __OPENCODE_XDBC_MARCO_H__
#define __OPENCODE_XDBC_MARCO_H__

#define __XDBC_FORCED_TRACE(className,message)\
{\
  Logging::forcedLog("opencode::xdbc","DEBUG",message,className);\
}

#define __XDBC_TRACE(className,message)\
if(DriverManager::isTracingOn)\
{\
  __XDBC_FORCED_TRACE(className,message);\
}

#define __XDBC_INTERFACE_CLOSE(XDBCOBJ)\
if(XDBCOBJ != 0)\
{\
  XDBCOBJ->close();\
}

#define __XDBC_INTERFACE_DESTROY(XDBCOBJ)\
if(XDBCOBJ != 0)\
{\
  XDBCOBJ->destroy();\
}

#define __XDBC_INTERFACE_CLOSEANDDESTROY(XDBCOBJ)\
if(XDBCOBJ != 0)\
{\
  XDBCOBJ->close();\
  XDBCOBJ->destroy();\
}

#ifdef  OPENCODE_DISABLE_XDBC_SEH
#define __XDBC_HOOK_DEFINE()      SQLException  __xdbc_hook;
#define __XDBC_HOOK_RESET()       __xdbc_hook.reset()
#define __XDBC_HOOK_DECLARE_ONCE  SQLException& __xdbc_hook
#define __XDBC_HOOK_DECLARE_MORE  SQLException& __xdbc_hook,
#define __XDBC_HOOK_INVOKE_ONCE   __xdbc_hook
#define __XDBC_HOOK_INVOKE_MORE   __xdbc_hook,
#define __XDBC_HOOK_INVOKE_CHECKPASSED  (__xdbc_hook.checkPassed())
#define __XDBC_HOOK_CHECK(__xdbc_stmt_list)\
{\
  if(!__XDBC_HOOK_INVOKE_CHECKPASSED)\
  {\
    __xdbc_stmt_list;\
  }\
}
#define __XDBC_THROW_DECLARE(...)
#define __XDBC_HOOK_RETURN(__xdbc_return,...)\
{\
  __xdbc_hook.setException(__VA_ARGS__);\
  __xdbc_return;\
}
#else
#define __XDBC_HOOK_DEFINE()
#define __XDBC_HOOK_RESET()

#define __XDBC_HOOK_DECLARE_ONCE
#define __XDBC_HOOK_DECLARE_MORE
#define __XDBC_HOOK_INVOKE_ONCE
#define __XDBC_HOOK_INVOKE_MORE
#define __XDBC_HOOK_INVOKE_CHECKPASSED true
#define __XDBC_HOOK_CHECK(__xdbc_stmt_list)

#define __XDBC_THROW_DECLARE(...) __OPENCODE_THROW_DECLARE(__VA_ARGS__)
#define __XDBC_HOOK_RETURN(__xdbc_return,...)\
{\
  __OPENCODE_THROW(SQLException(__VA_ARGS__));\
  __xdbc_return;\
}
#endif

//////////////////////////////////////////////////////////////////////////////////


#ifndef OPENCODE_DISABLE_HIDES_USING

#define __XDBC_INTERFACE_CONNECTION_HIDES(__XDBC_BASE_CLASS)\
  using __XDBC_BASE_CLASS::prepareStatement;\
  using __XDBC_BASE_CLASS::prepareCall;\
  using __XDBC_BASE_CLASS::prepareArray;

#define __XDBC_INTERFACE_RESULTSET_HIDES(__XDBC_BASE_CLASS)\
  using __XDBC_BASE_CLASS::getString;\
  using __XDBC_BASE_CLASS::getByte;\
  using __XDBC_BASE_CLASS::getShort;\
  using __XDBC_BASE_CLASS::getInt;\
  using __XDBC_BASE_CLASS::getLong;\
  using __XDBC_BASE_CLASS::getFloat;\
  using __XDBC_BASE_CLASS::getDouble;\
  using __XDBC_BASE_CLASS::getDate;\
  using __XDBC_BASE_CLASS::getTime;\
  using __XDBC_BASE_CLASS::getTimestamp;\
  using __XDBC_BASE_CLASS::getInterval;\
  using __XDBC_BASE_CLASS::getBlob;\
  using __XDBC_BASE_CLASS::getClob;

#define __XDBC_INTERFACE_PREPAREDSTATEMENT_HIDES(__XDBC_BASE_CLASS)\
  using __XDBC_BASE_CLASS::setString;\
  using __XDBC_BASE_CLASS::setByte;\
  using __XDBC_BASE_CLASS::setShort;\
  using __XDBC_BASE_CLASS::setInt;\
  using __XDBC_BASE_CLASS::setLong;\
  using __XDBC_BASE_CLASS::setFloat;\
  using __XDBC_BASE_CLASS::setDouble;\
  using __XDBC_BASE_CLASS::setDate;\
  using __XDBC_BASE_CLASS::setTime;\
  using __XDBC_BASE_CLASS::setTimestamp;\
  using __XDBC_BASE_CLASS::setInterval;\
  using __XDBC_BASE_CLASS::setBlob;\
  using __XDBC_BASE_CLASS::setClob;

#define __XDBC_INTERFACE_CALLABLESTATEMENT_HIDES(__XDBC_BASE_CLASS)\
  using __XDBC_BASE_CLASS::getString;\
  using __XDBC_BASE_CLASS::getByte;\
  using __XDBC_BASE_CLASS::getShort;\
  using __XDBC_BASE_CLASS::getInt;\
  using __XDBC_BASE_CLASS::getLong;\
  using __XDBC_BASE_CLASS::getFloat;\
  using __XDBC_BASE_CLASS::getDouble;\
  using __XDBC_BASE_CLASS::getDate;\
  using __XDBC_BASE_CLASS::getTime;\
  using __XDBC_BASE_CLASS::getTimestamp;\
  using __XDBC_BASE_CLASS::getInterval;\
  using __XDBC_BASE_CLASS::getBlob;\
  using __XDBC_BASE_CLASS::getClob;

#else

#define __XDBC_INTERFACE_CONNECTION_HIDES(__XDBC_BASE_CLASS)\
  virtual PreparedStatement* prepareStatement(__XDBC_HOOK_DECLARE_MORE const string& sql) __XDBC_THROW_DECLARE(SQLException)\
  {\
    return __XDBC_BASE_CLASS::prepareStatement(__XDBC_HOOK_INVOKE_MORE sql);\
  }\
  virtual CallableStatement* prepareCall(__XDBC_HOOK_DECLARE_MORE const string& sql) __XDBC_THROW_DECLARE(SQLException)\
  {\
    return __XDBC_BASE_CLASS::prepareCall(__XDBC_HOOK_INVOKE_MORE sql);\
  }\
  virtual CallableStatement* prepareArray(__XDBC_HOOK_DECLARE_MORE const string& sql) __XDBC_THROW_DECLARE(SQLException)\
  {\
    return __XDBC_BASE_CLASS::prepareArray(__XDBC_HOOK_INVOKE_MORE sql);\
  }\
  virtual PreparedStatement* prepareStatement(__XDBC_HOOK_DECLARE_MORE const string& sql, int resultSetType, int resultSetConcurrency) __XDBC_THROW_DECLARE(SQLException)\
  {\
    return __XDBC_BASE_CLASS::prepareStatement(__XDBC_HOOK_INVOKE_MORE sql,resultSetType,resultSetConcurrency);\
  }\
  virtual CallableStatement* prepareCall(__XDBC_HOOK_DECLARE_MORE const string& sql, int resultSetType, int resultSetConcurrency) __XDBC_THROW_DECLARE(SQLException)\
  {\
    return __XDBC_BASE_CLASS::prepareCall(__XDBC_HOOK_INVOKE_MORE sql,resultSetType,resultSetConcurrency);\
  }\
  virtual CallableStatement* prepareArray(__XDBC_HOOK_DECLARE_MORE const string& sql, int resultSetType, int resultSetConcurrency) __XDBC_THROW_DECLARE(SQLException)\
  {\
    return __XDBC_BASE_CLASS::prepareArray(__XDBC_HOOK_INVOKE_MORE sql,resultSetType,resultSetConcurrency);\
  }

#define __XDBC_INTERFACE_RESULTSET_HIDES(__XDBC_BASE_CLASS)\
  virtual char getByte(__XDBC_HOOK_DECLARE_MORE int columnIndex) __XDBC_THROW_DECLARE(SQLException)\
  {\
    return __XDBC_BASE_CLASS::getByte(__XDBC_HOOK_INVOKE_MORE columnIndex);\
  }\
  virtual short getShort(__XDBC_HOOK_DECLARE_MORE int columnIndex) __XDBC_THROW_DECLARE(SQLException)
  {\
    return __XDBC_BASE_CLASS::getShort(__XDBC_HOOK_INVOKE_MORE columnIndex);\
  }\
  virtual int  getInt(__XDBC_HOOK_DECLARE_MORE int columnIndex) __XDBC_THROW_DECLARE(SQLException)\
  {\
    return __XDBC_BASE_CLASS::getInt(__XDBC_HOOK_INVOKE_MORE columnIndex);\
  }\
  virtual XLONG getLong(__XDBC_HOOK_DECLARE_MORE int columnIndex) __XDBC_THROW_DECLARE(SQLException)\
  {\
    return __XDBC_BASE_CLASS::getLong(__XDBC_HOOK_INVOKE_MORE columnIndex);\
  }\
  virtual float getFloat(__XDBC_HOOK_DECLARE_MORE int columnIndex) __XDBC_THROW_DECLARE(SQLException)\
  {\
    return __XDBC_BASE_CLASS::getFloat(__XDBC_HOOK_INVOKE_MORE columnIndex);\
  }\
  virtual double getDouble(__XDBC_HOOK_DECLARE_MORE int columnIndex) __XDBC_THROW_DECLARE(SQLException)\
  {\
    return __XDBC_BASE_CLASS::getDouble(__XDBC_HOOK_INVOKE_MORE columnIndex);\
  }\
  virtual XDBC_DATE getDate(__XDBC_HOOK_DECLARE_MORE int columnIndex) __XDBC_THROW_DECLARE(SQLException)\
  {\
    return __XDBC_BASE_CLASS::getDate(__XDBC_HOOK_INVOKE_MORE columnIndex);\
  }\
  virtual XDBC_TIME getTime(__XDBC_HOOK_DECLARE_MORE int columnIndex) __XDBC_THROW_DECLARE(SQLException)\
  {\
    return __XDBC_BASE_CLASS::getTime(__XDBC_HOOK_INVOKE_MORE columnIndex);\
  }\
  virtual XDBC_TIMESTAMP getTimestamp(__XDBC_HOOK_DECLARE_MORE int columnIndex) __XDBC_THROW_DECLARE(SQLException)\
  {\
    return __XDBC_BASE_CLASS::getTimestamp(__XDBC_HOOK_INVOKE_MORE columnIndex);\
  }\
  virtual XDBC_INTERVAL getInterval(__XDBC_HOOK_DECLARE_MORE int columnIndex) __XDBC_THROW_DECLARE(SQLException)\
  {\
    return __XDBC_BASE_CLASS::getInterval(__XDBC_HOOK_INVOKE_MORE columnIndex);\
  }\
  virtual bool getByte(__XDBC_HOOK_DECLARE_MORE const string& columnName,char& colval) __XDBC_THROW_DECLARE(SQLException)\
  {\
    return __XDBC_BASE_CLASS::getByte(__XDBC_HOOK_INVOKE_MORE columnName,colval);\
  }\
  virtual short getShort(__XDBC_HOOK_DECLARE_MORE const string& columnName) __XDBC_THROW_DECLARE(SQLException)\
  {\
    return __XDBC_BASE_CLASS::getShort(__XDBC_HOOK_INVOKE_MORE columnName);\
  }\
  virtual bool  getShort(__XDBC_HOOK_DECLARE_MORE const string& columnName,short& colval) __XDBC_THROW_DECLARE(SQLException)\
  {\
    return __XDBC_BASE_CLASS::getShort(__XDBC_HOOK_INVOKE_MORE columnName,colval);\
  }\
  virtual int  getInt(__XDBC_HOOK_DECLARE_MORE const string& columnName) __XDBC_THROW_DECLARE(SQLException)\
  {\
    return __XDBC_BASE_CLASS::getInt(__XDBC_HOOK_INVOKE_MORE columnName);\
  }\
  virtual bool getInt(__XDBC_HOOK_DECLARE_MORE const string& columnName,int& colval) __XDBC_THROW_DECLARE(SQLException)\
  {\
    return __XDBC_BASE_CLASS::getInt(__XDBC_HOOK_INVOKE_MORE columnName,colval);\
  }\
  virtual XLONG getLong(__XDBC_HOOK_DECLARE_MORE const string& columnName) __XDBC_THROW_DECLARE(SQLException)\
  {\
    return __XDBC_BASE_CLASS::getLong(__XDBC_HOOK_INVOKE_MORE columnName);\
  }\
  virtual bool      getLong(__XDBC_HOOK_DECLARE_MORE const string& columnName,XLONG& colval) __XDBC_THROW_DECLARE(SQLException)\
  {\
    return __XDBC_BASE_CLASS::getLong(__XDBC_HOOK_INVOKE_MORE columnName,colval);\
  }\
  virtual float getFloat(__XDBC_HOOK_DECLARE_MORE const string& columnName) __XDBC_THROW_DECLARE(SQLException)\
  {\
    return __XDBC_BASE_CLASS::getFloat(__XDBC_HOOK_INVOKE_MORE columnName);\
  }\
  virtual bool  getFloat(__XDBC_HOOK_DECLARE_MORE const string& columnName,float& colval) __XDBC_THROW_DECLARE(SQLException)\
  {\
    return __XDBC_BASE_CLASS::getFloat(__XDBC_HOOK_INVOKE_MORE columnName,colval);\
  }\
  virtual double getDouble(__XDBC_HOOK_DECLARE_MORE const string& columnName) __XDBC_THROW_DECLARE(SQLException)\
  {\
    return __XDBC_BASE_CLASS::getDouble(__XDBC_HOOK_INVOKE_MORE columnName);\
  }\
  virtual bool   getDouble(__XDBC_HOOK_DECLARE_MORE const string& columnName,double& colval) __XDBC_THROW_DECLARE(SQLException)\
  {\
    return __XDBC_BASE_CLASS::getDouble(__XDBC_HOOK_INVOKE_MORE columnName,colval);\
  }\
  virtual XDBC_DATE getDate(__XDBC_HOOK_DECLARE_MORE const string& columnName) __XDBC_THROW_DECLARE(SQLException)\
  {\
    return __XDBC_BASE_CLASS::getDate(__XDBC_HOOK_INVOKE_MORE columnName);\
  }\
  virtual bool    getDate(__XDBC_HOOK_DECLARE_MORE const string& columnName,XDBC_DATE& colval) __XDBC_THROW_DECLARE(SQLException)\
  {\
    return __XDBC_BASE_CLASS::getDate(__XDBC_HOOK_INVOKE_MORE columnName,colval);\
  }\
  virtual XDBC_TIME getTime(__XDBC_HOOK_DECLARE_MORE const string& columnName) __XDBC_THROW_DECLARE(SQLException)\
  {\
    return __XDBC_BASE_CLASS::getTime(__XDBC_HOOK_INVOKE_MORE columnName);\
  }\
  virtual bool    getTime(__XDBC_HOOK_DECLARE_MORE const string& columnName,XDBC_TIME& colval) __XDBC_THROW_DECLARE(SQLException)\
  {\
    return __XDBC_BASE_CLASS::getTime(__XDBC_HOOK_INVOKE_MORE columnName,colval);\
  }\
  virtual XDBC_TIMESTAMP getTimestamp(__XDBC_HOOK_DECLARE_MORE const string& columnName) __XDBC_THROW_DECLARE(SQLException)\
  {\
    return __XDBC_BASE_CLASS::getTimestamp(__XDBC_HOOK_INVOKE_MORE columnName);\
  }\
  virtual bool         getTimestamp(__XDBC_HOOK_DECLARE_MORE const string& columnName,XDBC_TIMESTAMP& colval) __XDBC_THROW_DECLARE(SQLException)\
  {\
    return __XDBC_BASE_CLASS::getTimestamp(__XDBC_HOOK_INVOKE_MORE columnName,colval);\
  }\
  virtual XDBC_INTERVAL getInterval(__XDBC_HOOK_DECLARE_MORE const string& columnName) __XDBC_THROW_DECLARE(SQLException)\
  {\
    return __XDBC_BASE_CLASS::getInterval(__XDBC_HOOK_INVOKE_MORE columnName);\
  }\
  virtual bool     getInterval(__XDBC_HOOK_DECLARE_MORE const string& columnName,XDBC_INTERVAL& colval) __XDBC_THROW_DECLARE(SQLException)\
  {\
    return __XDBC_BASE_CLASS::getInterval(__XDBC_HOOK_INVOKE_MORE columnName,colval);\
  }\
  virtual  const char* getString(__XDBC_HOOK_DECLARE_MORE const string& columnName,int& sizeBytes) __XDBC_THROW_DECLARE(SQLException)\
  {\
    return __XDBC_BASE_CLASS::getString(__XDBC_HOOK_INVOKE_MORE columnName,sizeBytes);\
  }\
  virtual Blob* getBlob(__XDBC_HOOK_DECLARE_MORE const string& columnName) __XDBC_THROW_DECLARE(SQLException)\
  {\
    return __XDBC_BASE_CLASS::getBlob(__XDBC_HOOK_INVOKE_MORE columnName);\
  }\
  virtual Clob* getClob(__XDBC_HOOK_DECLARE_MORE const string& columnName) __XDBC_THROW_DECLARE(SQLException) \
  {\
    return __XDBC_BASE_CLASS::getClob(__XDBC_HOOK_INVOKE_MORE columnName);\
  }

#define __XDBC_INTERFACE_PREPAREDSTATEMENT_HIDES(__XDBC_BASE_CLASS)\
  virtual void setByte(__XDBC_HOOK_DECLARE_MORE int paramIndex, const char& value) __XDBC_THROW_DECLARE(SQLException)\
  {\
    __XDBC_BASE_CLASS::setByte(__XDBC_HOOK_INVOKE_MORE paramIndex,value);\
  }\
  virtual void setShort(__XDBC_HOOK_DECLARE_MORE int paramIndex, const short& value) __XDBC_THROW_DECLARE(SQLException)\
  {\
    __XDBC_BASE_CLASS::setShort(__XDBC_HOOK_INVOKE_MORE paramIndex,value);\
  }\
  virtual void setInt(__XDBC_HOOK_DECLARE_MORE int paramIndex, const int& value) __XDBC_THROW_DECLARE(SQLException)\
  {\
    __XDBC_BASE_CLASS::setInt(__XDBC_HOOK_INVOKE_MORE paramIndex,value);\
  }\
  virtual void setLong(__XDBC_HOOK_DECLARE_MORE int paramIndex, const XLONG& value) __XDBC_THROW_DECLARE(SQLException)\
  {\
    __XDBC_BASE_CLASS::setLong(__XDBC_HOOK_INVOKE_MORE paramIndex,value);\
  }\
  virtual void setFloat(__XDBC_HOOK_DECLARE_MORE int paramIndex, const float& value) __XDBC_THROW_DECLARE(SQLException)\
  {\
    __XDBC_BASE_CLASS::setFloat(__XDBC_HOOK_INVOKE_MORE paramIndex,value);\
  }\
  virtual void setDouble(__XDBC_HOOK_DECLARE_MORE int paramIndex, const double& value) __XDBC_THROW_DECLARE(SQLException)\
  {\
    __XDBC_BASE_CLASS::setDouble(__XDBC_HOOK_INVOKE_MORE paramIndex,value);\
  }\
  virtual void setString(__XDBC_HOOK_DECLARE_MORE int paramIndex,  const string& value) __XDBC_THROW_DECLARE(SQLException)\
  {\
    __XDBC_BASE_CLASS::setString(__XDBC_HOOK_INVOKE_MORE paramIndex,value);\
  }\
  virtual void setString(__XDBC_HOOK_DECLARE_MORE int paramIndex,  const char* value,int count) __XDBC_THROW_DECLARE(SQLException)\
  {\
    __XDBC_BASE_CLASS::setString(__XDBC_HOOK_INVOKE_MORE paramIndex,value,count);\
  }\
  virtual void setDate(__XDBC_HOOK_DECLARE_MORE int paramIndex, const XDBC_DATE& value)__XDBC_THROW_DECLARE(SQLException)\
  {\
    __XDBC_BASE_CLASS::setDate(__XDBC_HOOK_INVOKE_MORE paramIndex,value);\
  }\
  virtual void setTime(__XDBC_HOOK_DECLARE_MORE int paramIndex,  const XDBC_TIME& value) __XDBC_THROW_DECLARE(SQLException)\
  {\
    __XDBC_BASE_CLASS::setTime(__XDBC_HOOK_INVOKE_MORE paramIndex,value);\
  }\
  virtual void setTimestamp(__XDBC_HOOK_DECLARE_MORE int paramIndex,  const XDBC_TIMESTAMP& value) __XDBC_THROW_DECLARE(SQLException)\
  {\
    __XDBC_BASE_CLASS::setTimestamp(__XDBC_HOOK_INVOKE_MORE paramIndex,value);\
  }\
  virtual void setInterval(__XDBC_HOOK_DECLARE_MORE int paramIndex,  const XDBC_INTERVAL& value) __XDBC_THROW_DECLARE(SQLException)\
  {\
    __XDBC_BASE_CLASS::setInterval(__XDBC_HOOK_INVOKE_MORE paramIndex,value);\
  }\
  virtual void setBlob (__XDBC_HOOK_DECLARE_MORE int paramIndex,  const char* value,int count) __XDBC_THROW_DECLARE(SQLException)\
  {\
    __XDBC_BASE_CLASS::setBlob(__XDBC_HOOK_INVOKE_MORE paramIndex,value,count);\
  }\
  virtual void setClob (__XDBC_HOOK_DECLARE_MORE int paramIndex,  const char* value,int count) __XDBC_THROW_DECLARE(SQLException)\
  {\
    __XDBC_BASE_CLASS::setClob(__XDBC_HOOK_INVOKE_MORE paramIndex,value,count);\
  }
  virtual void setBlob (__XDBC_HOOK_DECLARE_MORE int paramIndex,  const Blob& value) __XDBC_THROW_DECLARE(SQLException)\
  {\
    __XDBC_BASE_CLASS::setBlob(__XDBC_HOOK_INVOKE_MORE paramIndex,value);\
  }\
  virtual void setClob (__XDBC_HOOK_DECLARE_MORE int paramIndex, const Clob& value) __XDBC_THROW_DECLARE(SQLException)\
  {\
    __XDBC_BASE_CLASS::setClob(__XDBC_HOOK_INVOKE_MORE paramIndex,value);\
  }

#define __XDBC_INTERFACE_CALLABLESTATEMENT_HIDES(__XDBC_BASE_CLASS)\
  virtual char getByte(__XDBC_HOOK_DECLARE_MORE int paramIndex) __XDBC_THROW_DECLARE(SQLException)\
  {\
    return __XDBC_BASE_CLASS::getByte(__XDBC_HOOK_INVOKE_MORE paramIndex);\
  }\
  virtual short getShort(__XDBC_HOOK_DECLARE_MORE int paramIndex) __XDBC_THROW_DECLARE(SQLException)
  {\
    return __XDBC_BASE_CLASS::getShort(__XDBC_HOOK_INVOKE_MORE paramIndex);\
  }\
  virtual int  getInt(__XDBC_HOOK_DECLARE_MORE int paramIndex) __XDBC_THROW_DECLARE(SQLException)\
  {\
    return __XDBC_BASE_CLASS::getInt(__XDBC_HOOK_INVOKE_MORE paramIndex);\
  }\
  virtual XLONG getLong(__XDBC_HOOK_DECLARE_MORE int paramIndex) __XDBC_THROW_DECLARE(SQLException)\
  {\
    return __XDBC_BASE_CLASS::getLong(__XDBC_HOOK_INVOKE_MORE paramIndex);\
  }\
  virtual float getFloat(__XDBC_HOOK_DECLARE_MORE int paramIndex) __XDBC_THROW_DECLARE(SQLException)\
  {\
    return __XDBC_BASE_CLASS::getFloat(__XDBC_HOOK_INVOKE_MORE paramIndex);\
  }\
  virtual double getDouble(__XDBC_HOOK_DECLARE_MORE int paramIndex) __XDBC_THROW_DECLARE(SQLException)\
  {\
    return __XDBC_BASE_CLASS::getDouble(__XDBC_HOOK_INVOKE_MORE paramIndex);\
  }\
  virtual XDBC_DATE getDate(__XDBC_HOOK_DECLARE_MORE int paramIndex) __XDBC_THROW_DECLARE(SQLException)\
  {\
    return __XDBC_BASE_CLASS::getDate(__XDBC_HOOK_INVOKE_MORE paramIndex);\
  }\
  virtual XDBC_TIME getTime(__XDBC_HOOK_DECLARE_MORE int paramIndex) __XDBC_THROW_DECLARE(SQLException)\
  {\
    return __XDBC_BASE_CLASS::getTime(__XDBC_HOOK_INVOKE_MORE paramIndex);\
  }\
  virtual XDBC_TIMESTAMP getTimestamp(__XDBC_HOOK_DECLARE_MORE int paramIndex) __XDBC_THROW_DECLARE(SQLException)\
  {\
    return __XDBC_BASE_CLASS::getTimestamp(__XDBC_HOOK_INVOKE_MORE paramIndex);\
  }\
  virtual XDBC_INTERVAL getInterval(__XDBC_HOOK_DECLARE_MORE int paramIndex) __XDBC_THROW_DECLARE(SQLException)\
  {\
    return __XDBC_BASE_CLASS::getInterval(__XDBC_HOOK_INVOKE_MORE paramIndex);\
  }

#endif

//////////////////////////////////////////////////////////////////////////////////

#define __XDBC_INTERFACE_BLOB(__XDBC_BASE_CLASS)\
  virtual  int                  length                  (__XDBC_HOOK_DECLARE_ONCE) const __XDBC_THROW_DECLARE(SQLException);\
  virtual  int                  write                   (__XDBC_HOOK_DECLARE_MORE int pos, const char* str, int offset, int len) __XDBC_THROW_DECLARE(SQLException);\
  virtual  int                  read                    (__XDBC_HOOK_DECLARE_MORE int pos, char* str,int offset,int length) const __XDBC_THROW_DECLARE(SQLException);\
  virtual  void                 truncate                (__XDBC_HOOK_DECLARE_MORE int len) __XDBC_THROW_DECLARE(SQLException);

#define __XDBC_INTERFACE_CLOB(__XDBC_BASE_CLASS)\
  virtual  int                  length                  (__XDBC_HOOK_DECLARE_ONCE) const  __XDBC_THROW_DECLARE(SQLException);\
  virtual  int                  write                   (__XDBC_HOOK_DECLARE_MORE int pos, const string& str) __XDBC_THROW_DECLARE(SQLException);\
  virtual  int                  write                   (__XDBC_HOOK_DECLARE_MORE int pos, const char* str, int offset, int len) __XDBC_THROW_DECLARE(SQLException);\
  virtual  int                  read                    (__XDBC_HOOK_DECLARE_MORE int pos, char* str,int offset,int length) const __XDBC_THROW_DECLARE(SQLException);\
  virtual  void                 truncate                (__XDBC_HOOK_DECLARE_MORE int len) __XDBC_THROW_DECLARE(SQLException);

#define __XDBC_INTERFACE_RESULTSETMETADATA(__XDBC_BASE_CLASS)\
  virtual  int                  getColumnCount          (__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException);\
  virtual  int                  isNullable              (__XDBC_HOOK_DECLARE_MORE int column) __XDBC_THROW_DECLARE(SQLException);\
  virtual  int                  isCharSemantics         (__XDBC_HOOK_DECLARE_MORE int column) __XDBC_THROW_DECLARE(SQLException);\
  virtual  int                  getColumnDisplaySize    (__XDBC_HOOK_DECLARE_MORE int column) __XDBC_THROW_DECLARE(SQLException);\
  virtual  string               getColumnName           (__XDBC_HOOK_DECLARE_MORE int column) __XDBC_THROW_DECLARE(SQLException);\
  virtual  string               getSchemaName           (__XDBC_HOOK_DECLARE_MORE int column) __XDBC_THROW_DECLARE(SQLException);\
  virtual  int                  getPrecision            (__XDBC_HOOK_DECLARE_MORE int column) __XDBC_THROW_DECLARE(SQLException);\
  virtual  int                  getScale                (__XDBC_HOOK_DECLARE_MORE int column) __XDBC_THROW_DECLARE(SQLException);\
  virtual  string               getTableName            (__XDBC_HOOK_DECLARE_MORE int column) __XDBC_THROW_DECLARE(SQLException);\
  virtual  int                  getColumnType           (__XDBC_HOOK_DECLARE_MORE int column) __XDBC_THROW_DECLARE(SQLException);\
  virtual  string               getColumnTypeName       (__XDBC_HOOK_DECLARE_MORE int column) __XDBC_THROW_DECLARE(SQLException);\
  virtual  int                  getColumnRawType        (__XDBC_HOOK_DECLARE_MORE int column) __XDBC_THROW_DECLARE(SQLException);

#define __XDBC_INTERFACE_MANAGEABLE(__XDBC_BASE_CLASS)\
  virtual  void                 close();\
  virtual  void                 destroy();

#define __XDBC_INTERFACE_IMPLEMENTATION(__XDBC_BASE_CLASS)\
  __XDBC_INTERFACE_MANAGEABLE(__XDBC_BASE_CLASS)\
  virtual  int                  getResultSetType        (__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException);\
  virtual  Connection*          getConnection           (__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException);

#define __XDBC_INTERFACE_CONNECTION(__XDBC_BASE_CLASS)\
  __XDBC_INTERFACE_MANAGEABLE(__XDBC_BASE_CLASS)\
  __XDBC_INTERFACE_CONNECTION_HIDES(__XDBC_BASE_CLASS)\
  virtual  bool                 connect                 (__XDBC_HOOK_DECLARE_MORE const string& url) __XDBC_THROW_DECLARE(SQLException);\
  virtual  bool                 connect                 (__XDBC_HOOK_DECLARE_MORE const string& url, const string& user, const string& password) __XDBC_THROW_DECLARE(SQLException);\
  virtual  bool                 connect                 (__XDBC_HOOK_DECLARE_MORE const string& url, __XDBC_ATTRIBUTES& info) __XDBC_THROW_DECLARE(SQLException);\
  virtual  string               normalizeSQL            (const string& sql,int& paramCount);\
  virtual  bool                 isClosed                ();\
  virtual  int                  pingDatabase            (int timeout);\
  virtual  void                 setAutoCommit           (__XDBC_HOOK_DECLARE_MORE bool autoCommit) __XDBC_THROW_DECLARE(SQLException);\
  virtual  bool                 getAutoCommit           (__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException);\
  virtual  void                 commit                  (__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException);\
  virtual  void                 rollback                (__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException);\
  virtual  void                 setTransactionIsolation (__XDBC_HOOK_DECLARE_MORE int level) __XDBC_THROW_DECLARE(SQLException);\
  virtual  int                  getTransactionIsolation (__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException);\
  virtual  Statement*           createStatement         (__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException);\
  virtual  PreparedStatement*   prepareStatement        (__XDBC_HOOK_DECLARE_MORE const string& sql,int paramCount) __XDBC_THROW_DECLARE(SQLException);\
  virtual  CallableStatement*   prepareCall             (__XDBC_HOOK_DECLARE_MORE const string& sql,int paramCount) __XDBC_THROW_DECLARE(SQLException);\
  virtual  ArrayStatement*      prepareArray            (__XDBC_HOOK_DECLARE_MORE const string& sql,int paramCount) __XDBC_THROW_DECLARE(SQLException); \
  virtual  Statement*           createStatement         (__XDBC_HOOK_DECLARE_MORE int resultSetType, int resultSetConcurrency)__XDBC_THROW_DECLARE(SQLException);\
  virtual  PreparedStatement*   prepareStatement        (__XDBC_HOOK_DECLARE_MORE const string& sql, int resultSetType, int resultSetConcurrency,int paramCount)__XDBC_THROW_DECLARE(SQLException);\
  virtual  CallableStatement*   prepareCall             (__XDBC_HOOK_DECLARE_MORE const string& sql, int resultSetType, int resultSetConcurrency,int paramCount) __XDBC_THROW_DECLARE(SQLException);\
  virtual  ArrayStatement*      prepareArray            (__XDBC_HOOK_DECLARE_MORE const string& sql,int resultSetType, int resultSetConcurrency,int paramCount) __XDBC_THROW_DECLARE(SQLException);

#define __XDBC_INTERFACE_STATEMENT(__XDBC_BASE_CLASS)\
  __XDBC_INTERFACE_IMPLEMENTATION(__XDBC_BASE_CLASS)\
  virtual  ResultSet*           executeQuery            (__XDBC_HOOK_DECLARE_MORE const string& sql) __XDBC_THROW_DECLARE(SQLException);\
  virtual  int                  executeUpdate           (__XDBC_HOOK_DECLARE_MORE const string& sql) __XDBC_THROW_DECLARE(SQLException);

#define __XDBC_INTERFACE_PREPAREDSTATEMENT(__XDBC_BASE_CLASS)\
  __XDBC_INTERFACE_IMPLEMENTATION(__XDBC_BASE_CLASS)\
  __XDBC_INTERFACE_PREPAREDSTATEMENT_HIDES(__XDBC_BASE_CLASS)\
  virtual  ResultSet*           executeQuery            (__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException);\
  virtual  int                  executeUpdate           (__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException);\
  virtual  void                 setByte                 (__XDBC_HOOK_DECLARE_MORE int paramIndex, const char& x,bool is_null) __XDBC_THROW_DECLARE(SQLException);\
  virtual  void                 setShort                (__XDBC_HOOK_DECLARE_MORE int paramIndex, const short& x,bool is_null) __XDBC_THROW_DECLARE(SQLException);\
  virtual  void                 setInt                  (__XDBC_HOOK_DECLARE_MORE int paramIndex, const int& x,bool is_null) __XDBC_THROW_DECLARE(SQLException);\
  virtual  void                 setLong                 (__XDBC_HOOK_DECLARE_MORE int paramIndex, const XLONG& x,bool is_null) __XDBC_THROW_DECLARE(SQLException);\
  virtual  void                 setFloat                (__XDBC_HOOK_DECLARE_MORE int paramIndex, const float& x,bool is_null) __XDBC_THROW_DECLARE(SQLException);\
  virtual  void                 setDouble               (__XDBC_HOOK_DECLARE_MORE int paramIndex, const double& x,bool is_null) __XDBC_THROW_DECLARE(SQLException);\
  virtual  void                 setString               (__XDBC_HOOK_DECLARE_MORE int paramIndex, const string* x,bool is_null) __XDBC_THROW_DECLARE(SQLException);\
  virtual  void                 setString               (__XDBC_HOOK_DECLARE_MORE int paramIndex, const char* x,int count,bool is_null) __XDBC_THROW_DECLARE(SQLException);\
  virtual  void                 setDate                 (__XDBC_HOOK_DECLARE_MORE int paramIndex, const XDBC_DATE* x,bool is_null)__XDBC_THROW_DECLARE(SQLException);\
  virtual  void                 setTime                 (__XDBC_HOOK_DECLARE_MORE int paramIndex, const XDBC_TIME* x,bool is_null) __XDBC_THROW_DECLARE(SQLException);\
  virtual  void                 setTimestamp            (__XDBC_HOOK_DECLARE_MORE int paramIndex, const XDBC_TIMESTAMP* x,bool is_null) __XDBC_THROW_DECLARE(SQLException);\
  virtual  void                 setInterval             (__XDBC_HOOK_DECLARE_MORE int paramIndex, const XDBC_INTERVAL* x,bool is_null) __XDBC_THROW_DECLARE(SQLException);\
  virtual  void                 setBlob                 (__XDBC_HOOK_DECLARE_MORE int paramIndex, const char* x,int count,bool is_null) __XDBC_THROW_DECLARE(SQLException);\
  virtual  void                 setClob                 (__XDBC_HOOK_DECLARE_MORE int paramIndex, const char* x,int count,bool is_null) __XDBC_THROW_DECLARE(SQLException);\
  virtual  void                 setBlob                 (__XDBC_HOOK_DECLARE_MORE int paramIndex, const Blob* x,bool is_null) __XDBC_THROW_DECLARE(SQLException);\
  virtual  void                 setClob                 (__XDBC_HOOK_DECLARE_MORE int paramIndex, const Clob* x,bool is_null) __XDBC_THROW_DECLARE(SQLException);

#define __XDBC_INTERFACE_CALLABLESTATEMENT(__XDBC_BASE_CLASS)\
  __XDBC_INTERFACE_PREPAREDSTATEMENT(__XDBC_BASE_CLASS)\
  __XDBC_INTERFACE_CALLABLESTATEMENT_HIDES(__XDBC_BASE_CLASS)\
  virtual  void                 registerOutParameter    (__XDBC_HOOK_DECLARE_MORE int paramIndex, int sqlType) __XDBC_THROW_DECLARE(SQLException);\
  virtual  void                 registerOutParameter    (__XDBC_HOOK_DECLARE_MORE int paramIndex, int sqlType, int scale) __XDBC_THROW_DECLARE(SQLException);\
  virtual  bool                 wasNull                 (__XDBC_HOOK_DECLARE_MORE int paramIndex)  __XDBC_THROW_DECLARE(SQLException);\
  virtual  const                char* getString         (__XDBC_HOOK_DECLARE_MORE int paramIndex,int& sizeBytes) __XDBC_THROW_DECLARE(SQLException);\
  virtual  bool                 getByte                 (__XDBC_HOOK_DECLARE_MORE int paramIndex,char& colval) __XDBC_THROW_DECLARE(SQLException);\
  virtual  bool                 getShort                (__XDBC_HOOK_DECLARE_MORE int paramIndex,short& colval) __XDBC_THROW_DECLARE(SQLException);\
  virtual  bool                 getInt                  (__XDBC_HOOK_DECLARE_MORE int paramIndex,int& colval) __XDBC_THROW_DECLARE(SQLException);\
  virtual  bool                 getLong                 (__XDBC_HOOK_DECLARE_MORE int paramIndex,XLONG& colval) __XDBC_THROW_DECLARE(SQLException);\
  virtual  bool                 getFloat                (__XDBC_HOOK_DECLARE_MORE int paramIndex,float& colval) __XDBC_THROW_DECLARE(SQLException);\
  virtual  bool                 getDouble               (__XDBC_HOOK_DECLARE_MORE int paramIndex,double& colval) __XDBC_THROW_DECLARE(SQLException);\
  virtual  bool                 getDate                 (__XDBC_HOOK_DECLARE_MORE int paramIndex,XDBC_DATE& colval) __XDBC_THROW_DECLARE(SQLException);\
  virtual  bool                 getTime                 (__XDBC_HOOK_DECLARE_MORE int paramIndex,XDBC_TIME& colval) __XDBC_THROW_DECLARE(SQLException);\
  virtual  bool                 getTimestamp            (__XDBC_HOOK_DECLARE_MORE int paramIndex,XDBC_TIMESTAMP& colval) __XDBC_THROW_DECLARE(SQLException);\
  virtual  bool                 getInterval             (__XDBC_HOOK_DECLARE_MORE int paramIndex,XDBC_INTERVAL& colval) __XDBC_THROW_DECLARE(SQLException);\
  virtual  Blob*                getBlob                 (__XDBC_HOOK_DECLARE_MORE int paramIndex) __XDBC_THROW_DECLARE(SQLException);\
  virtual  Clob*                getClob                 (__XDBC_HOOK_DECLARE_MORE int paramIndex) __XDBC_THROW_DECLARE(SQLException);

#define __XDBC_INTERFACE_ARRAYSTATEMENT(__XDBC_BASE_CLASS)\
  __XDBC_INTERFACE_IMPLEMENTATION(__XDBC_BASE_CLASS)\
  virtual  int                  executeUpdate           (__XDBC_HOOK_DECLARE_MORE DataTable* paramTable,vector<ArrayException>* errors) __XDBC_THROW_DECLARE(SQLException);\
  virtual  ArrayResultSet*      executeQuery            (__XDBC_HOOK_DECLARE_MORE DataTable* dataTable,DataTable* paramTable) __XDBC_THROW_DECLARE(SQLException);\
  virtual  int                  getMaxArraySize         () const;\
  virtual  void                 setMaxArraySize         (int size);

#define __XDBC_INTERFACE_RESULTSET(__XDBC_BASE_CLASS)\
  __XDBC_INTERFACE_MANAGEABLE(__XDBC_BASE_CLASS)\
  __XDBC_INTERFACE_RESULTSET_HIDES(__XDBC_BASE_CLASS)\
  virtual  int                  findColumn              (__XDBC_HOOK_DECLARE_MORE const string& columnName) __XDBC_THROW_DECLARE(SQLException);\
  virtual  bool                 wasNull                 (__XDBC_HOOK_DECLARE_MORE int columnIndex) __XDBC_THROW_DECLARE(SQLException);\
  virtual  bool                 next                    (__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException);\
  virtual  const char*          getString               (__XDBC_HOOK_DECLARE_MORE int columnIndex,int& sizeBytes) __XDBC_THROW_DECLARE(SQLException);\
  virtual  bool                 getByte                 (__XDBC_HOOK_DECLARE_MORE int columnIndex,char& colval) __XDBC_THROW_DECLARE(SQLException);\
  virtual  bool                 getShort                (__XDBC_HOOK_DECLARE_MORE int columnIndex,short& colval) __XDBC_THROW_DECLARE(SQLException);\
  virtual  bool                 getInt                  (__XDBC_HOOK_DECLARE_MORE int columnIndex,int& colval) __XDBC_THROW_DECLARE(SQLException);\
  virtual  bool                 getLong                 (__XDBC_HOOK_DECLARE_MORE int columnIndex,XLONG& colval) __XDBC_THROW_DECLARE(SQLException);\
  virtual  bool                 getFloat                (__XDBC_HOOK_DECLARE_MORE int columnIndex,float& colval) __XDBC_THROW_DECLARE(SQLException);\
  virtual  bool                 getDouble               (__XDBC_HOOK_DECLARE_MORE int columnIndex,double& colval) __XDBC_THROW_DECLARE(SQLException);\
  virtual  bool                 getDate                 (__XDBC_HOOK_DECLARE_MORE int columnIndex,XDBC_DATE& colval) __XDBC_THROW_DECLARE(SQLException);\
  virtual  bool                 getTime                 (__XDBC_HOOK_DECLARE_MORE int columnIndex,XDBC_TIME& colval) __XDBC_THROW_DECLARE(SQLException);\
  virtual  bool                 getTimestamp            (__XDBC_HOOK_DECLARE_MORE int columnIndex,XDBC_TIMESTAMP& colval) __XDBC_THROW_DECLARE(SQLException);\
  virtual  bool                 getInterval             (__XDBC_HOOK_DECLARE_MORE int columnIndex,XDBC_INTERVAL& colval) __XDBC_THROW_DECLARE(SQLException);\
  virtual  Blob*                getBlob                 (__XDBC_HOOK_DECLARE_MORE int columnIndex) __XDBC_THROW_DECLARE(SQLException);\
  virtual  Clob*                getClob                 (__XDBC_HOOK_DECLARE_MORE int columnIndex) __XDBC_THROW_DECLARE(SQLException);\
  virtual  int                  getType                 (__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException);\
  virtual  int                  getConcurrency          (__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException);\
  virtual  ResultSetMetaData*   getMetaData             (__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException);\
  virtual  SQLImplementation*      getImplementation       (__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException);\
  virtual  int                  getRow                  (__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException);\
  virtual  bool                 isBeforeFirst           (__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException);\
  virtual  bool                 isAfterLast             (__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException);\
  virtual  bool                 isFirst                 (__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException);\
  virtual  bool                 isLast                  (__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException);\
  virtual  bool                 first                   (__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException);\
  virtual  bool                 last                    (__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException);\
  virtual  bool                 absolute                (__XDBC_HOOK_DECLARE_MORE int row ) __XDBC_THROW_DECLARE(SQLException);\
  virtual  bool                 relative                (__XDBC_HOOK_DECLARE_MORE int rows ) __XDBC_THROW_DECLARE(SQLException);\
  virtual  bool                 previous                (__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException);\
  virtual  void                 setFetchSize            (__XDBC_HOOK_DECLARE_MORE int rows) __XDBC_THROW_DECLARE(SQLException);\
  virtual  int                  getFetchSize            (__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException);

#define __XDBC_INTERFACE_ARRAYRESULTSET(__XDBC_BASE_CLASS)\
  __XDBC_INTERFACE_MANAGEABLE(__XDBC_BASE_CLASS)\
  virtual  int                  findColumn              (__XDBC_HOOK_DECLARE_MORE const string& columnName) __XDBC_THROW_DECLARE(SQLException);\
  virtual  int                  next                    (__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException) ;\
  virtual  ResultSetMetaData*   getMetaData             (__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException);

#endif


