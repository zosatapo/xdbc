#ifndef  __OPENCODE_CORE_SMART_PTR_H__
#define  __OPENCODE_CORE_SMART_PTR_H__

#include "port/ioc_G_config.h"

__OPENCODE_BEGIN_NAMESPACE

template <class _Tp> class  smart_ptr 
{
private:
  _Tp* _M_ptr;

public:
  typedef _Tp element_type;

public:
  ~smart_ptr()  { delete _M_ptr; }
  
  explicit smart_ptr(_Tp* __p = 0)  : _M_ptr(__p) {}
  
  smart_ptr(smart_ptr& __a)  : _M_ptr(__a.release()) {}
  
  template <class _Tp1> smart_ptr(smart_ptr<_Tp1>& __a) 
  : _M_ptr(__a.release()) {}
  
  template <class _Tp1> smart_ptr& operator=(smart_ptr<_Tp1>& __a) 
  {
    if (__a.get() == this->get()) 
    {
      return *this;
    }
    
    delete _M_ptr;
    _M_ptr = __a.release();
      
    return *this;
  }

public:
  
  smart_ptr& operator=(smart_ptr& __a)  
  {
    if (&__a == this) 
    {
      return *this;
    }
    
    delete _M_ptr;
    _M_ptr = __a.release();
    
    return *this;
  }
  
  _Tp& operator*() const  
  {
    return *_M_ptr;
  }
  
  _Tp* operator->() const  
  {
    return _M_ptr;
  }
  
  _Tp* get() const  
  {
    return _M_ptr;
  }
  
  _Tp* release()  
  {
    _Tp* __tmp = _M_ptr;
    _M_ptr = 0;
    return __tmp;
  }
  
  void reset(_Tp* __p = 0)  
  {
    delete _M_ptr;
    _M_ptr = __p;
  }
  
#ifdef OPENCODE_USE_AUTO_PTR_CONVERSIONS

  // According to the C++ standard, these conversions are required.  Most
  // present-day compilers, however, do not enforce that requirement---and, 
  // in fact, most present-day compilers do not support the language 
  // features that these conversions rely on.
  
private:
  template<class _Tp1> struct auto_ptr_ref 
  {
    _Tp1* _M_ptr;
    auto_ptr_ref(_Tp1* __p) : _M_ptr(__p) {}
  };

public:
  smart_ptr(auto_ptr_ref<_Tp> __ref) 
    : _M_ptr(__ref._M_ptr) {}
    
  template <class _Tp1> operator auto_ptr_ref<_Tp1>()  
    { return auto_ptr_ref<_Tp>(this->release()); }
    
  template <class _Tp1> operator smart_ptr<_Tp1>() 
    { return smart_ptr<_Tp1>(this->release()); }

#endif /* OPENCODE_USE_AUTO_PTR_CONVERSIONS */
};


__OPENCODE_END_NAMESPACE

#endif
