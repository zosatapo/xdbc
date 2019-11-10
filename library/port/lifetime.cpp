#include "port/lifetime.h"

__OPENCODE_BEGIN_NAMESPACE

volatile  bool               LifeTimeGlobals::__lifetime_initialized = false;
OPENCODE_TYPE_LIFEGUARD      LifeTimeGlobals::__lifetime_guard;
OPENCODE_MAP<string,void*>*  LifeTimeGlobals::__lifetime_map = 0; 
TrackerArray                 LifeTimeGlobals::__lifetime_array = 0;
unsigned int                 LifeTimeGlobals::__lifetime_elements = 0;

void  __lifetime_callDestructionFunction()
{
  __OPENCODE_ASSERT(LifeTimeGlobals::__lifetime_elements > 0 && LifeTimeGlobals::__lifetime_array != 0);
 
  //pickup the element at the top of the stack
  LifeTimeTrack* pTop = LifeTimeGlobals::__lifetime_array[LifeTimeGlobals::__lifetime_elements - 1];

  LOGGING_DEBUG("LifeTimeTrack::<finit>|callDestructionFunction to {" << (pTop->getTrackedClassName()) <<"}"); 

  //remove that object off stack
  //don't check errors-realloc with less memory can't fail
  LifeTimeGlobals::__lifetime_array = (TrackerArray)(std::realloc(LifeTimeGlobals::__lifetime_array,sizeof(LifeTimeGlobals::__lifetime_array)* --LifeTimeGlobals::__lifetime_elements));

  //destroy the element
  delete pTop;
}

void   LifeTimeGlobals::__lifetime_initialize()
{
  if(!__lifetime_initialized)
  {
#if defined(PORT_WINDOWS)
   InitializeCriticalSection(&__lifetime_guard);
#else
  pthread_mutexattr_t  mutexattr;
  ::pthread_mutexattr_init(&mutexattr);
  ::pthread_mutexattr_settype(&mutexattr,PTHREAD_MUTEX_RECURSIVE);
  int rc = ::pthread_mutex_init(&__lifetime_guard,&mutexattr);
  ::pthread_mutexattr_destroy(&mutexattr);
#endif

		__lifetime_map = new OPENCODE_MAP<string,void*>();
		__lifetime_array = 0;
		__lifetime_elements = 0;
		__lifetime_initialized = true;
  }
}

void   LifeTimeGlobals::__lifetime_destroy()
{
#if (defined(OPENCODE_LACKS_FIXED_ATEXIT) || defined(OPENCODE_DISABLE_ATEXIT))
  __OPENCODE_ASSERT(LifeTimeGlobals::__lifetime_elements > 0 && LifeTimeGlobals::__lifetime_array != 0);
 
  for(int i = LifeTimeGlobals::__lifetime_elements;i > 0 ;i--)
  {
    //pickup the element at the top of the stack
    LifeTimeTrack* pTop = LifeTimeGlobals::__lifetime_array[i - 1];
    LOGGING_DEBUG("LifeTimeTrack::<finit>|callDestructionFunction to {" << (pTop->getTrackedClassName()) <<"}"); 
    
    //destroy the element
    delete pTop;
  }

  std::free(LifeTimeGlobals::__lifetime_array);
#endif

  if(__lifetime_map !=0)
  {
    delete __lifetime_map;
    __lifetime_map = 0;
  }
  __lifetime_map = new OPENCODE_MAP<string,void*>();
#if defined(PORT_WINDOWS)
  DeleteCriticalSection(&__lifetime_guard);
#else
  ::pthread_mutex_destroy(&__lifetime_guard);
#endif
}

bool LifeTimeGlobals::__lifetime_compare( const  LifeTimeTrack* pTrack1,const  LifeTimeTrack* pTrack2)
{
  return (pTrack2 == 0) ? true:(pTrack2->__longevity < pTrack1->__longevity);
}

void LifeTimeGlobals::__lifetime_lock(OPENCODE_TYPE_LIFEGUARD* __GUARD,const char* __classname)
{ 
   if(!LifeTimeGlobals::__lifetime_initialized)
   {
	    LifeTimeGlobals::__lifetime_initialize();
      __GUARD = &(LifeTimeGlobals::__lifetime_guard);
   } 

#if defined(PORT_WINDOWS)
  ::EnterCriticalSection(__GUARD);
#else
  int rc = pthread_mutex_lock(__GUARD);  
#endif
}

void LifeTimeGlobals::__lifetime_unlock(OPENCODE_TYPE_LIFEGUARD* __GUARD,const char* __classname)
{
   if(!LifeTimeGlobals::__lifetime_initialized)
   {
	    LifeTimeGlobals::__lifetime_initialize();
      __GUARD = &(LifeTimeGlobals::__lifetime_guard);
   } 

#if defined(PORT_WINDOWS)
  ::LeaveCriticalSection(__GUARD);
#else
  int rc = pthread_mutex_unlock(__GUARD);
#endif
}


void  LifeTimeGlobals::__lifetime_register(OPENCODE_MAP<string,void*>* __pmap ,const char* __classname,void* __pInstance)
{
   if(!LifeTimeGlobals::__lifetime_initialized)
   {
	    LifeTimeGlobals::__lifetime_initialize();
      __pmap = LifeTimeGlobals::__lifetime_map;
   } 

  LOGGING_DEBUG("LifeTimeTrack::<init>|endCreateInstance to {" << __classname << "}");

  if(__pmap != 0)
  {
    (*__pmap)[__classname] = __pInstance;
  }  
}

void*  LifeTimeGlobals::__lifetime_deregister(OPENCODE_MAP<string,void*>* __pmap ,const char* __classname)
{
   if(!LifeTimeGlobals::__lifetime_initialized)
   {
	    LifeTimeGlobals::__lifetime_initialize();
      __pmap = LifeTimeGlobals::__lifetime_map;
   } 

  void* __p_rc = 0;
  
  if(__pmap != 0)
  {
    OPENCODE_MAP<string,void*>::iterator __pos = __pmap->find(__classname);
    if(__pos != __pmap->end())
    {
      __p_rc = __pos->second;
      __pmap->erase(__pos);
    }  
  }
  
  return __p_rc;
}

void* LifeTimeGlobals::__lifetime_find(OPENCODE_MAP<string,void*>* __pmap ,const char*  __classname)
{
  if(!LifeTimeGlobals::__lifetime_initialized)
  {
    LifeTimeGlobals::__lifetime_initialize(); 
    __pmap = LifeTimeGlobals::__lifetime_map;
  } 

  void* __pInstance = 0;
  
  if(__pmap != 0)
  {
    OPENCODE_MAP<string,void*>::iterator __pos = __pmap->find(__classname);
    if(__pos != __pmap->end())
    {
      __pInstance = __pos->second;  
      LOGGING_DEBUG("LifeTimeTrack[callFindInstance] {" << __classname <<"} found, STL bug fixed");    
    }  
  }
   
  return __pInstance;
}

__OPENCODE_END_NAMESPACE
