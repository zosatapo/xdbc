#include "xdbc/odbcClob.h"

__OPENCODE_BEGIN_NAMESPACE

int OdbcClob::length(__XDBC_HOOK_DECLARE_ONCE) const __XDBC_THROW_DECLARE(SQLException)
{
  return (int)lob_stream.length();
}

int OdbcClob::write(__XDBC_HOOK_DECLARE_MORE int pos, const string& str) __XDBC_THROW_DECLARE(SQLException)
{
  lob_stream.insert(pos-1,str);

  return (int)str.length();
}

int OdbcClob::write(__XDBC_HOOK_DECLARE_MORE int pos, const char* str, int offset, int len) __XDBC_THROW_DECLARE(SQLException)
{
  lob_stream.insert(pos-1,str+offset,len);
  return len;
}

int OdbcClob::read(__XDBC_HOOK_DECLARE_MORE int pos, char* str,int offset,int length) const __XDBC_THROW_DECLARE(SQLException)
{
  int count = this->length(__XDBC_HOOK_INVOKE_ONCE) - pos + 1;
  __XDBC_HOOK_CHECK(return 0);

  
  if(count < 0)
  {
    return 0;
  }

  const char* start = lob_stream.data() + pos - 1;
  
  count = __M_min(count,length);
  memcpy(str+offset,start,count);
      
  return count;
}

void OdbcClob::truncate(__XDBC_HOOK_DECLARE_MORE int len) __XDBC_THROW_DECLARE(SQLException)
{
  lob_stream.resize(len);
}


__OPENCODE_END_NAMESPACE
