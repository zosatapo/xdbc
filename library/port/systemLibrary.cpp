#include "port/systemLibrary.h"
#include "port/libsys.h"

#if defined(PORT_HPUX) && defined(OPENCODE_HAS_SHL_LOAD)
#  if !defined(OPENCODE_HAS_CXXSHL_LOAD)
extern "C" shl_t cxxshl_load(const char *path, int flags, long address)
{
  return ::shl_load(path,flags,address);
}

extern "C" int   cxxshl_unload(shl_t handle)
{
  return ::shl_unload(handle);
}
#  endif
#endif

__OPENCODE_BEGIN_NAMESPACE

SystemLibrary::~SystemLibrary() 
{
}

SystemLibrary::SystemLibrary()
:openTime(0),closeTime(0)
{
  m_slh = (OPENCODE_TYPE_SOLIBRARY)0;
}

SystemLibrary::SystemLibrary(const OPENCODE_TYPE_SOLIBRARY slhValue ,const string& pathnameValue,XLONG openTimeValue)
{
  this->m_slh = (OPENCODE_TYPE_SOLIBRARY)slhValue;
  this->pathname = pathnameValue;
  this->openTime = openTimeValue;
  this->closeTime = 0;
}

SystemLibrary  SystemLibrary::getInstance(const string& pathnameValue) __OPENCODE_THROW_DECLARE(IOException)
{
#if defined(PORT_WINDOWS)
  OPENCODE_TYPE_SOLIBRARY slh =(OPENCODE_TYPE_SOLIBRARY)::LoadLibrary(pathnameValue.c_str());
#elif defined(PORT_HPUX) && defined(OPENCODE_HAS_SHL_LOAD)
  OPENCODE_TYPE_SOLIBRARY slh = (OPENCODE_TYPE_SOLIBRARY)cxxshl_load(pathnameValue.c_str(),BIND_DEFERRED|DYNAMIC_PATH,0);
  if(slh == (OPENCODE_TYPE_SOLIBRARY)0)
  {
    __OPENCODE_THROW(IOException(System::getLastError()));
  }
#else
  OPENCODE_TYPE_SOLIBRARY slh = (OPENCODE_TYPE_SOLIBRARY)::dlopen(pathnameValue.c_str(),RTLD_LAZY);
  if(slh == (OPENCODE_TYPE_SOLIBRARY)0)
  {
    __OPENCODE_THROW(IOException(dlerror()));
  }
#endif

  XLONG openTime = System::currentTimeMillis();
  SystemLibrary __syslib((const OPENCODE_TYPE_SOLIBRARY)slh,pathnameValue,openTime);
  return __syslib;          
}

OPENCODE_TYPE_SOSYMBOL  SystemLibrary::findSymbol(const string& sym) const
{
  if(m_slh == (OPENCODE_TYPE_SOLIBRARY)0)
  {
    return (OPENCODE_TYPE_SOSYMBOL)0;
  }

#if defined(PORT_WINDOWS)
  return (OPENCODE_TYPE_SOSYMBOL)::GetProcAddress(m_slh,sym.c_str());
#elif defined(PORT_HPUX) && defined(OPENCODE_HAS_SHL_LOAD)
  OPENCODE_TYPE_SOSYMBOL symbol_value = 0;
  ::shl_findsym((shl_t*)&m_slh,sym.c_str(),TYPE_PROCEDURE,(void*)&symbol_value); 
  return symbol_value;
#else
  return (OPENCODE_TYPE_SOSYMBOL)::dlsym(m_slh , sym.c_str());
#endif
}

void  SystemLibrary::close()
{
  if(m_slh != (OPENCODE_TYPE_SOLIBRARY)0)
  {
    closeTime = System::currentTimeMillis();
#if defined(PORT_WINDOWS)
    ::FreeLibrary(m_slh);
#elif defined(PORT_HPUX) && defined(OPENCODE_HAS_SHL_LOAD)
    cxxshl_unload(m_slh); 
#else
    ::dlclose(m_slh);
#endif 

    m_slh = (OPENCODE_TYPE_SOLIBRARY)0;
  }
}

bool SystemLibrary::isOpened() const
{
  return (m_slh != (OPENCODE_TYPE_SOLIBRARY)0);
}

XLONG SystemLibrary::getOpenTime() const
{
  return openTime;
}

XLONG SystemLibrary::getCloseTime() const
{
  return closeTime;
}

const string& SystemLibrary::getPathname() const
{
  return pathname;
}

bool operator==(const SystemLibrary& lhs,const SystemLibrary& rhs)
{
  return lhs.getPathname() == rhs.getPathname();
}

bool operator==(const SystemLibrary& lhs,const string& rhs)
{
  return lhs.getPathname() == rhs;
}

__OPENCODE_END_NAMESPACE
