/** 
 *  lifetime.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef  __OPENCODE_LIFETIME_H__
#define  __OPENCODE_LIFETIME_H__

#include "port/libdefs.h"
#include "port/mutex.h"
#include "port/logging.h"
#include "port/utilities.h"
#include "port/libproc.h"

#include "port/ioc_map.h" 

#include <algorithm>

__OPENCODE_BEGIN_NAMESPACE

class LifeTimeGlobals;
class LifeTimeTrack
{
public:
  friend class LifeTimeGlobals;
  
public:
  virtual ~LifeTimeTrack() {} 
  LifeTimeTrack(unsigned int x):__longevity(x){}

  virtual string getTrackedClassName() const = 0;

protected:
  //LifeTimeTrack#__longevity == [0,999999999]
  unsigned int __longevity;    
};

typedef void (*DESTRUCTION_PFN_T)();
typedef LifeTimeTrack** TrackerArray;

#if defined(PORT_WINDOWS)
#  define OPENCODE_TYPE_LIFEGUARD   CRITICAL_SECTION
#else
#  define OPENCODE_TYPE_LIFEGUARD   pthread_mutex_t
#endif

__OPENCODE_CORE_API_DECL extern void   __lifetime_callDestructionFunction();

class  __OPENCODE_CORE_API_DECL LifeTimeGlobals
{
public:
	// This assumes the first invoke is called before threads are started.
  static void   __lifetime_initialize();
  static void   __lifetime_destroy();

public:
  static bool   __lifetime_compare( const  LifeTimeTrack* pTrack1,const  LifeTimeTrack* pTrack2);
  static void   __lifetime_lock(OPENCODE_TYPE_LIFEGUARD* __GUARD,const char* __classname);
  static void   __lifetime_unlock(OPENCODE_TYPE_LIFEGUARD* __GUARD,const char* __classname);
  static void   __lifetime_register(OPENCODE_MAP<string,void*>* __pmap ,const char* __classname,void* __pInstance);
  static void*  __lifetime_deregister(OPENCODE_MAP<string,void*>* __pmap ,const char* __classname);
  static void*  __lifetime_find(OPENCODE_MAP<string,void*>* __pmap ,const char*  __classname);

public:
  static volatile bool                 __lifetime_initialized;
  static OPENCODE_TYPE_LIFEGUARD       __lifetime_guard;
  static OPENCODE_MAP<string,void*>*   __lifetime_map; /** to fix STL template bugs */

	static TrackerArray                  __lifetime_array;
	static unsigned int                  __lifetime_elements;
};

template<class __Tp,class Destroyer> class  ConcretLifeTimeTrack : public LifeTimeTrack
{
public:
  virtual ~ConcretLifeTimeTrack()
  {
    __destroyer(__pTracked);
  } 
  
  ConcretLifeTimeTrack(__Tp* pDynObject,unsigned int longevity,Destroyer destroyer)
  :LifeTimeTrack(longevity),__pTracked(pDynObject),__destroyer(destroyer)
  {
  }

public:
  string getTrackedClassName() const
  {
    if(__pTracked != 0)
    {
      return string(__pTracked->__getClassName());
    }

    return string("");
  }

private:
  __Tp*     __pTracked;
  Destroyer __destroyer; 
};

//==========================================================
// Assigns an object a longevity; ensures ordered 
// destructions of objects ,registered thusly during the 
// exit sequence of the application
//==========================================================
template<class __Tp,class Destroyer> void setLongevity(__Tp* pDynObject,unsigned int longevity,Destroyer d /*= Deleter<__Tp>::destroy*/)
{
  TrackerArray pNewArray = (TrackerArray)(std::realloc(LifeTimeGlobals::__lifetime_array,sizeof(*LifeTimeGlobals::__lifetime_array)* (LifeTimeGlobals::__lifetime_elements + 1)));
  if(pNewArray == 0)
  {
    __OPENCODE_THROW(std::bad_alloc());
  }
  
  LifeTimeGlobals::__lifetime_array = pNewArray; 
  LifeTimeTrack* __p_newLifeTimeTrack = new ConcretLifeTimeTrack<__Tp,Destroyer>(pDynObject,longevity,d);
  TrackerArray pos = std::upper_bound(LifeTimeGlobals::__lifetime_array,LifeTimeGlobals::__lifetime_array + LifeTimeGlobals::__lifetime_elements,__p_newLifeTimeTrack,LifeTimeGlobals::__lifetime_compare);
  std::copy_backward(pos,LifeTimeGlobals::__lifetime_array + LifeTimeGlobals::__lifetime_elements,LifeTimeGlobals::__lifetime_array + LifeTimeGlobals::__lifetime_elements + 1);
  *pos = __p_newLifeTimeTrack;
  ++LifeTimeGlobals::__lifetime_elements;

#if (!defined(OPENCODE_LACKS_FIXED_ATEXIT) && !defined(OPENCODE_DISABLE_ATEXIT))
  std::atexit( __lifetime_callDestructionFunction);
#endif
}

template <class __Tp> class  Adapter
{
public:  
  Adapter(DESTRUCTION_PFN_T pFun)
  :pFun_(pFun){}
  
  void operator()(__Tp*) {  pFun_(); }

private:  
  DESTRUCTION_PFN_T pFun_;
};

template<class __Tp> class  Deleter
{
public:
  void destroy(__Tp* pObj)
  {
    delete pObj;
  }  
};

template <class __Tp>  class  CreateUsingNew
{
public:  
  static __Tp* createInstance(){ return new __Tp; }
  
  static void destroyInstance(__Tp* p){ delete p; }
};

template <class __Tp> class  DefaultLifetime
{
public:  
  static void scheduleDestruction(__Tp*, DESTRUCTION_PFN_T pFun)
  { 
    std::atexit(pFun); 
  }
  
  static void onDeadReference(){ __OPENCODE_THROW(BaseException("Dead Reference Detected")); }
};

template <class __Tp> class  PhoenixSingleton
{
public:
  static void scheduleDestruction(__Tp*, DESTRUCTION_PFN_T pFun)
  {
    #ifndef OPENCODE_HAS_FIXED_ATEXIT
    if (!destroyedOnce_)
    #endif
    {
      std::atexit(pFun);
    }
  }

  static void onDeadReference()
  {
    #ifndef OPENCODE_HAS_FIXED_ATEXIT
    destroyedOnce_ = true;
    #endif
  }

private:
  #ifndef OPENCODE_HAS_FIXED_ATEXIT
    static bool destroyedOnce_;
  #endif
};

#ifndef OPENCODE_HAS_FIXED_ATEXIT
  template <class __Tp> bool PhoenixSingleton<__Tp>::destroyedOnce_ = false;
#endif

template <class __Tp> class  SingletonWithLongevity
{
public:
  static void scheduleDestruction(__Tp* pObj, DESTRUCTION_PFN_T pFun)
  {
    Adapter<__Tp> adapter(pFun);
    setLongevity(pObj, getLongevity(pObj), adapter);
  }
  
  static void onDeadReference(){ __OPENCODE_THROW(BaseException("Dead Reference Detected")); }
};

template <class __Tp> class  NoDestroy
{
public:
  static void scheduleDestruction(__Tp*, DESTRUCTION_PFN_T pFun){}
  static void onDeadReference(){}
};

template <class __Tp> class  SingleThread
{
public:
  typedef __Tp         value_type;
  typedef value_type*  VolatileType;

  class Guard
  {
    public:
      Guard(){}
      ~Guard(){}
  };
};

#define DECLARE_LIFETIME_BEGIN(CLASSNAME)\
public:\
  static const char  __class_name_##CLASSNAME[100];\
public:\
  static const char* __getClassName();
#define DECLARE_LIFETIME_END(CLASSNAME)

#define IMPLEMENT_LIFETIME_BEGIN(CLASSNAME)\
const char  CLASSNAME::__class_name_##CLASSNAME[] = {#CLASSNAME};\
const char* CLASSNAME::__getClassName()\
{\
  return __class_name_##CLASSNAME;\
}
#define IMPLEMENT_LIFETIME_END(CLASSNAME)

template <class __Tp> class  MultiThread
{
public:
  typedef __Tp         value_type;
  typedef value_type*  VolatileType;

public:

public:
  class Guard
  {
    public:
      Guard()
      {
        #if defined(PORT_AIX)
          pthread_lock_global_np(); 
        #else
         LifeTimeGlobals::__lifetime_lock(&LifeTimeGlobals::__lifetime_guard,__Tp::__getClassName()); 
        #endif
      }
      
      ~Guard()
      {
        #if defined(PORT_AIX)
          pthread_unlock_global_np();
        #else
          LifeTimeGlobals::__lifetime_unlock(&LifeTimeGlobals::__lifetime_guard,__Tp::__getClassName()); 
        #endif
      }
  };
};


template
<
  class __Tp,
  class CreationPolicy = CreateUsingNew<__Tp>,
  class LifetimePolicy = SingletonWithLongevity<__Tp>,
  class ThreadingModel = MultiThread<__Tp>
>
class  SingletonHolder
{
public:
  static __Tp& getInstance();

private:
  SingletonHolder();
  SingletonHolder(const SingletonHolder& holder);
  SingletonHolder& operator=(const SingletonHolder& holder);
  
private:
  static void createInstance();
  static void destroyInstance();

public:
  // Data
  typedef typename ThreadingModel::VolatileType PtrInstanceType;
  static PtrInstanceType __pInstance;
  static bool __destroyed;
};

template
<
class __Tp,
class CreationPolicy,
class LifetimePolicy,
class ThreadingModel
>
typename SingletonHolder<__Tp, CreationPolicy, LifetimePolicy, ThreadingModel>::PtrInstanceType SingletonHolder<__Tp, CreationPolicy, LifetimePolicy, ThreadingModel>::__pInstance = 0;

template
<
class __Tp,
class CreationPolicy,
class LifetimePolicy,
class ThreadingModel
>
bool SingletonHolder<__Tp, CreationPolicy, LifetimePolicy, ThreadingModel>::__destroyed = false;

template
<
class __Tp,
class CreationPolicy,
class LifetimePolicy,
class ThreadingModel
>
__OPENCODE_INLINE __Tp& SingletonHolder<__Tp, CreationPolicy, LifetimePolicy, ThreadingModel>::getInstance()
{ 
  if (__pInstance == 0)
  {  
    if (__destroyed)
    {
      LifetimePolicy::onDeadReference();
      __destroyed = false;
    }

    typename ThreadingModel::Guard guard;
     
    if(__pInstance == 0)
    {
      createInstance();    
    }
  }

  return *((__Tp*)__pInstance);
}

template
<
class __Tp,
class CreationPolicy,
class LifetimePolicy,
class ThreadingModel
>
void SingletonHolder<__Tp, CreationPolicy, LifetimePolicy, ThreadingModel>::createInstance()
{ 
  __Tp* __p_instance = (__Tp*)LifeTimeGlobals::__lifetime_find(LifeTimeGlobals::__lifetime_map,__Tp::__getClassName());  

  if(__p_instance != 0)
  {
    __pInstance =  __p_instance;
  }
  else
  {
    LOGGING_DEBUG("LifeTimeTrack::<init>|startCreateInstance to {" << __Tp::__getClassName() << "}");
    __pInstance = CreationPolicy::createInstance();
    LifetimePolicy::scheduleDestruction((__Tp*)__pInstance, &destroyInstance);    
    LifeTimeGlobals::__lifetime_register(LifeTimeGlobals::__lifetime_map,__Tp::__getClassName(),(__Tp*)__pInstance);
  }
}

template
<
class __Tp,
class CreationPolicy,
class LifetimePolicy,
class ThreadingModel
>
void SingletonHolder<__Tp, CreationPolicy, LifetimePolicy, ThreadingModel>::destroyInstance()
{
  __OPENCODE_ASSERT(!__destroyed);
  CreationPolicy::destroyInstance((__Tp*)__pInstance);  
  __pInstance = 0;
  __destroyed = true;
}

__OPENCODE_END_NAMESPACE

#endif
