#ifndef __OPENCODE_XDBC_DEMO_H__
#define __OPENCODE_XDBC_DEMO_H__

#include "xdbc/xdbc.h"
#include <iostream>

USING_OPENCODE_STL_NS
USING_OPENCODE_NS

#define  __XDBC_HOOK_PRINTF() printf("[__XDBC_TRACE] %s\n",__xdbc_hook.getMessage().c_str())

Connection* getConnection(__XDBC_HOOK_DECLARE_MORE const string& dsname) __OPENCODE_THROW_DECLARE(SQLException);

void  dumpMetaData(__XDBC_HOOK_DECLARE_MORE ResultSet* resultSet) __OPENCODE_THROW_DECLARE(SQLException);
void  dumpResultSet(__XDBC_HOOK_DECLARE_MORE ResultSet* resultSet) __OPENCODE_THROW_DECLARE(SQLException);
void  dumpRow(__XDBC_HOOK_DECLARE_MORE ResultSet* resultSet,ResultSetMetaData* metaData) __OPENCODE_THROW_DECLARE(SQLException);

#endif
