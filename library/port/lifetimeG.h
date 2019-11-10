/** 
 *  lifetimeG.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 

#ifndef  __OPENCODE_LIFETIME_GLOBALS_H__
#define  __OPENCODE_LIFETIME_GLOBALS_H__

#include "port/libdefs.h"
#include "port/mutex.h"

#include "port/ioc_map.h" 

#include <algorithm>
#include <stdexcept>
#include <cstdlib>


__OPENCODE_BEGIN_NAMESPACE

//------------------------------------------------------------------------------------
// I Global Declaration
//------------------------------------------------------------------------------------
#if defined(PORT_WINDOWS)
#  define OPENCODE_TYPE_LIFEGUARD   CRITICAL_SECTION
#else
#  define OPENCODE_TYPE_LIFEGUARD   pthread_mutex_t
#endif

class LifeTimeTrack;
typedef void (*DESTRUCTION_PFN_T)();
typedef LifeTimeTrack** TrackerArray;

class __OPENCODE_EXPORT_DECL LifeTimeGlobals
{
public:
	// This assumes the first constructor is called before threads
	// are started.
	LifeTimeGlobals();
	
public:
  static volatile bool                __lifetime_initialized;
  static OPENCODE_TYPE_LIFEGUARD        __lifetime_lock;
  static OPENCODE_MAP<string,void*>*  __lifetime_map; /** to fix STL template bugs */

	static TrackerArray                 __lifetime_array;
	static unsigned int                  __lifetime_elements;
};


extern "C"    void  callDestructionFunction();

__OPENCODE_EXPORT_DECL extern void  __lifetime_lock(OPENCODE_TYPE_LIFEGUARD* __GUARD,const char* __classname);
__OPENCODE_EXPORT_DECL extern void  __lifetime_unlock(OPENCODE_TYPE_LIFEGUARD* __GUARD,const char* __classname);

__OPENCODE_EXPORT_DECL extern void  __lifetime_register(OPENCODE_MAP<string,void*>* __pmap ,const char* __classname,void* __pInstance);
__OPENCODE_EXPORT_DECL extern void* __lifetime_deregister(OPENCODE_MAP<string,void*>* __pmap ,const char* __classname);
__OPENCODE_EXPORT_DECL extern void* __lifetime_find(OPENCODE_MAP<string,void*>* __pmap ,const char* __classname);

__OPENCODE_EXPORT_DECL extern bool __lifetime_compare( unsigned int longevity,const  LifeTimeTrack* p);

//------------------------------------------------------------------------------------
// II  Basic Singleton Support Class
//------------------------------------------------------------------------------------
//==========================================================
// II-1 class LifetimeTracker
//==========================================================
class __OPENCODE_EXPORT_DECL LifeTimeTrack
{
public:
  friend  bool __lifetime_compare( unsigned int longevity,const  LifeTimeTrack* p);

  virtual ~LifeTimeTrack() {} 
  LifeTimeTrack(unsigned int x):__longevity(x){}

  virtual string getTrackedClassName() const = 0;

private:
  //LifeTimeTrack#__longevity == [0,999999999]
  unsigned int __longevity;    
};


//==========================================================
// II-2 Concrete lifetime tracker for objects of type T
//==========================================================
template<class T,class Destroyer>
class __OPENCODE_EXPORT_DECL ConcretLifeTimeTrack : public LifeTimeTrack
{
public:
  virtual ~ConcretLifeTimeTrack()
  {
    __destroyer(__pTracked);
  } 
  
  ConcretLifeTimeTrack(T* pDynObject,unsigned int longevity,Destroyer destroyer)
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
  T* __pTracked;
  Destroyer __destroyer; 
};

//==========================================================
// II-4 function template setLongevity
// ====================================== 
//
// Assigns an object a longevity; ensures ordered 
// destructions of objects ,registered thusly during the 
// exit sequence of the application
//==========================================================
template<class T,class Destroyer>
void setLongevity(T* pDynObject,unsigned int longevity,Destroyer d /*= Deleter<T>::destroy*/)
{
  TrackerArray pNewArray = (TrackerArray)(std::realloc(LifeTimeGlobals::__lifetime_array,sizeof(*LifeTimeGlobals::__lifetime_array)* (LifeTimeGlobals::__lifetime_elements + 1)));

  if(pNewArray == 0)
  {
    __OPENCODE_THROW(std::bad_alloc());
  }
  
  LifeTimeGlobals::__lifetime_array = pNewArray;
  
  LifeTimeTrack* p =new ConcretLifeTimeTrack<T,Destroyer>(pDynObject,longevity,d);
  
  TrackerArray pos = std::upper_bound(LifeTimeGlobals::__lifetime_array,LifeTimeGlobals::__lifetime_array + LifeTimeGlobals::__lifetime_elements,longevity,__lifetime_compare);
  
  std::copy_backward(pos,LifeTimeGlobals::__lifetime_array + LifeTimeGlobals::__lifetime_elements,LifeTimeGlobals::__lifetime_array + LifeTimeGlobals::__lifetime_elements + 1);
  *pos = p;
  
  ++LifeTimeGlobals::__lifetime_elements;
  
  std::atexit(callDestructionFunction);
}

//==========================================================
// II-5 class template Adapter
//==========================================================
template <class T>
class __OPENCODE_EXPORT_DECL Adapter
{
public:  
  Adapter(DESTRUCTION_PFN_T pFun):pFun_(pFun){}
  
  void operator()(T*) {  pFun_(); }

private:  
  DESTRUCTION_PFN_T pFun_;
};

__OPENCODE_END_NAMESPACE

#endif
