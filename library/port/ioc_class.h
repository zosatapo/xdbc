#ifndef  __OPENCODE_CORE_CLASS_H__
#define  __OPENCODE_CORE_CLASS_H__

#include "port/ioc_G_config.h"
#include "port/ioc_types.h" 
#include "port/ioc_strings.h"

__OPENCODE_BEGIN_NAMESPACE

//------------------------------------------------------------------------------
// basealgo
//------------------------------------------------------------------------------

template <class _Tp> __OPENCODE_INLINE  const _Tp& __M_min(const _Tp& __a, const _Tp& __b) 
{
  return (__b < __a) ? __b : __a;
}

template <class _Tp> __OPENCODE_INLINE  const _Tp& __M_max(const _Tp& __a, const _Tp& __b) 
{
  return  (__a < __b) ? __b : __a;
}

__OPENCODE_INLINE  size_t  __M_hash_string(const char* __s)
{
  size_t __h = 0; 
  for ( ; *__s; ++__s)
  {
     __h = 5*__h + *__s;
  }

  return size_t(__h);
}

__OPENCODE_INLINE  size_t  __M_hash_string(const string& __s)
{
  size_t __size = __s.length();
  size_t __h = 0;
 
  for ( size_t i = 0; i < __size; ++i)
  {
     __h = 5*__h + __s[i];
  }

  return size_t(__h);
}

template <class __T> class  opencode_hash 
{
};

template <> class  opencode_hash<string> 
{
public:
   size_t operator()(const string& __s) const { return __M_hash_string(__s.c_str()); }    
};

template <>  class  opencode_hash<const string>
{
public:
   size_t operator()(const string& __s) const { return __M_hash_string(__s.c_str()); }
};

//------------------------------------------------------------------------------
// baseclass
//------------------------------------------------------------------------------
class  BaseException
{
public:
  ~BaseException(){}
  BaseException():__internal_code(0){}
  
  BaseException(const string& message)
  :__internal_msg(message),__internal_code(0){}
  	
  BaseException(const string& message,const string& addition)
  :__internal_msg(message),__internal_addition(addition),__internal_code(0){}
  	
  BaseException(const string& message,XLONG errcode)
  :__internal_msg(message),__internal_code(errcode){}
  	
  BaseException(const string& message,const string& addition,XLONG errcode)
  :__internal_msg(message),__internal_addition(addition),__internal_code(errcode){}
 
  	
  __OPENCODE_INLINE  const string& getMessage()   const
  {
    return __internal_msg;
  }
	
  __OPENCODE_INLINE  const string& getAddition()  const
  {
    return __internal_addition;
  }
  
  __OPENCODE_INLINE  XLONG         getErrorCode() const
  {
    return __internal_code;
  }

public:
  __OPENCODE_INLINE  void setException(const string& message)
  {
    this->__internal_msg  = message;
    this->__internal_code  = 0;
    this->__internal_addition.clear();
  }
	
  __OPENCODE_INLINE  void setException(const string& message,XLONG errcode)
  {
   this->__internal_msg  = message;
   this->__internal_code  = errcode;
   this->__internal_addition.clear();
  }
	
  __OPENCODE_INLINE  void setException(const string& message,const string& addition,XLONG errcode)  
  {
	  this->__internal_msg  = message;
	  this->__internal_code  = errcode;
	  this->__internal_addition = addition;
	}
	
  __OPENCODE_INLINE  void reset()  
  {
	  this->__internal_msg.clear();
	  this->__internal_code = 0;
	  this->__internal_addition.clear();
	}
	
  
  __OPENCODE_INLINE  bool checkPassed() const 
  { 
  	return __internal_msg.empty() && (__internal_code == 0);
  } 
  
protected:
  string     __internal_msg;
  string     __internal_addition;
  XLONG      __internal_code;
};

__OPENCODE_END_NAMESPACE

#endif
