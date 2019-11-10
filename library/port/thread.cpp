#include "port/thread.h"
#include "port/libsys.h"
#include "port/ioc_sbuf.h"
#include "port/logging.h"

__OPENCODE_BEGIN_NAMESPACE

//------------------------------------------------------
//0. class ThreadSpecificData
//------------------------------------------------------

ThreadSpecificData::~ThreadSpecificData()
{
#if defined(PORT_WINDOWS)
  TlsFree((OPENCODE_TYPE_TSDKEY)key);
#else
  pthread_key_delete(key);
#endif
}

ThreadSpecificData::ThreadSpecificData() : key(0)
{
#if defined(PORT_WINDOWS)
  key = (OPENCODE_TYPE_TSDKEY)TlsAlloc();
#else
  pthread_key_create(&key, 0);
#endif
}


void * ThreadSpecificData::getData() const
{
#if defined(PORT_WINDOWS)
  return TlsGetValue((OPENCODE_TYPE_TSDKEY)key);
#else
  return pthread_getspecific((pthread_key_t)key);
#endif
}

void ThreadSpecificData::setData(void * data)
{
#if defined(PORT_WINDOWS)
  TlsSetValue((OPENCODE_TYPE_TSDKEY)key, data);
#else
  pthread_setspecific((pthread_key_t)key, data);
#endif
}


//------------------------------------------------------
//1. class JobData
//------------------------------------------------------

//------------------------------------------------------
//2. class Job
//------------------------------------------------------

//------------------------------------------------------
//3. class Thread
//------------------------------------------------------

#if defined(PORT_WINDOWS)
DWORD WINAPI       opencode_thread_proc(void * param)
#else
extern "C"  void*  opencode_thread_proc(void * param)
#endif
{
  if(param != 0)
  {
    Thread* __p_thread = (Thread *)param;
    
    if(__p_thread->getStackSize() <= 0)
    {
      #if !defined(PORT_WINDOWS)
      pthread_attr_t attr;
      pthread_attr_init(&attr);     
      size_t stacksize;
      pthread_attr_getstacksize(&attr,&stacksize);      
      __p_thread->setStackSize(stacksize);
      #endif
    } 

    __p_thread->run();

    if(__p_thread->deleteOnExit())
    {
      delete __p_thread;
    }
  }

  #if !defined(PORT_WINDOWS)
  pthread_exit(0);
  #endif

  return 0;
}

#if defined(PORT_WINDOWS)
#  define PTHREAD_CREATE_JOINABLE        0
#  define PTHREAD_CREATE_DETACHED        0

#  define SCHED_FIFO                     0
#  define SCHED_OTHER                    0
#  define SCHED_RR                       0
#  define PTHREAD_EXPLICIT_SCHED         0

#  define PTHREAD_SCOPE_SYSTEM           0
#endif

Thread::~Thread() 
{
}

Thread::Thread(Job* job ,JobData* jobData, int flags , bool deleteOnExit)
:thread_flags(flags),thread_deleteOnExit(deleteOnExit)
{
  if(job != 0)
  {
    sp_job.reset(job);
  }

  if(jobData != 0)
  {
    sp_jobData.reset(jobData);
  }

  this->thread_priority = THR_PRIORITY_DEFAULT;
  //this->thread_stackSize = 0;
  this->thread_stackSize = 262144 * 10;

  this->started = false;

  this->h_thread = 0;
  this->threadid = 0;
}

UXLONG   Thread::createThread(const Thread* pThread,int spuid) __OPENCODE_THROW_DECLARE(IOException)
{

  UXLONG createThreadID = 0;

  int flags = pThread->thread_flags;
  int priority = pThread->thread_priority;

  if(((flags & THREAD_CREATE_DETACHED) == 0) && ((flags & THREAD_CREATE_JOINABLE) == 0))
  {
    flags |= THREAD_CREATE_JOINABLE;
  }

  #if !defined(PORT_WINDOWS)
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  #endif

  if(flags != 0)
  {

//-------------------------------------------------------------------------------------------

//    ###################################################
//    # 1. Set Detach state 
//    ###################################################

    if(    ((flags & THREAD_CREATE_DETACHED) != 0) 
       ||  ((flags & THREAD_CREATE_JOINABLE) != 0))
    {
      int dstate = PTHREAD_CREATE_JOINABLE;

      if((flags & THREAD_CREATE_DETACHED) != 0)
      {
        dstate = PTHREAD_CREATE_DETACHED;
      }

      #if !defined(PORT_WINDOWS)
      pthread_attr_setdetachstate(&attr, dstate);
      #endif     
    }

//    ###################################################
//    # 2. Set Policy 
//    ###################################################

    if(priority != THR_PRIORITY_DEFAULT)
    {
      if(    ((flags & THREAD_SCHED_FIFO) == 0)
         &&  ((flags & THREAD_SCHED_RR) == 0) 
         &&  ((flags & THREAD_SCHED_OTHER) == 0))
      {
        flags |= THREAD_SCHED_OTHER;
      }
    }
    
    if(   ((flags & THREAD_SCHED_FIFO) != 0) 
       || ((flags & THREAD_SCHED_RR) != 0) 
       || ((flags & THREAD_SCHED_OTHER) != 0))
    {
      int spolicy = SCHED_OTHER;
      
      if( (flags & THREAD_SCHED_OTHER) != 0)
      {
        spolicy = SCHED_OTHER;
      }
      else if( (flags & THREAD_SCHED_FIFO) != 0)
      {
        spolicy = SCHED_FIFO;
      }
      else if( (flags & THREAD_SCHED_RR) != 0)
      {
        spolicy = SCHED_RR;
      }

      #if !defined(PORT_WINDOWS)
      pthread_attr_setschedpolicy(&attr,spolicy);
      #endif      
    }

//    ###################################################
//    # 3. Set Priority (use reasonable default priorities) 
//    ###################################################
      #if !defined(PORT_WINDOWS)
      struct sched_param sparam;
      memset ((void *) &sparam, 0, sizeof(sparam));
      sparam.sched_priority = priority;
      pthread_attr_setschedparam(&attr,&sparam);
      #endif    


//    ###################################################
//    # 4. Set scheduling explicit or inherited 
//    ###################################################

    if(    ((flags & THREAD_INHERIT_SCHED) != 0) 
        || ((flags & THREAD_EXPLICIT_SCHED) != 0))
    {
      int sched = PTHREAD_EXPLICIT_SCHED;
        
      if( (flags & THREAD_EXPLICIT_SCHED) != 0)
      {
        sched = THREAD_EXPLICIT_SCHED;
      }
      else if( (flags & THREAD_INHERIT_SCHED) != 0)
      {
        sched = THREAD_INHERIT_SCHED;
      }       

      #if !defined(PORT_WINDOWS)
      pthread_attr_setinheritsched (&attr, sched);
      #endif
    }

//    ###################################################
//    # 5. Set scope
//    ###################################################
    if(    ((flags & THREAD_SCOPE_SYSTEM) != 0) 
        || ((flags & THREAD_SCOPE_PROCESS) != 0))
    {

      #if defined(PORT_HPUX)      
      int scope = PTHREAD_SCOPE_SYSTEM;
      #elif defined(PORT_AIX)
      int scope = PTHREAD_SCOPE_PROCESS;
      #else
      int scope = PTHREAD_SCOPE_SYSTEM;
      #endif

      if( (flags & THREAD_SCOPE_SYSTEM) != 0)
      {
        scope = PTHREAD_SCOPE_SYSTEM;
      }
      
      #if !defined(PORT_WINDOWS)
      pthread_attr_setscope(&attr, scope);
      #endif
    }

//    ###################################################
//    # 6. Set processor_np & bind_type
//    ###################################################

    #if defined(PORT_HPUX)  
    
    if(   ((flags & THREAD_PROCESSOR_SPUINHERIT) != 0) 
       || ((flags & THREAD_PROCESSOR_SPUFLOAT) != 0)
       || ((flags & THREAD_PROCESSOR_LDOMINHERIT) != 0) 
       || ((flags & THREAD_PROCESSOR_LDOMFLOAT) != 0))
    {
      
      pthread_spu_t __spu_t = PTHREAD_SPUINHERIT_NP;
      int  __bind_type = 0;  
      
      if( (flags & THREAD_PROCESSOR_SPUINHERIT) != 0)
      {
        __spu_t = PTHREAD_SPUINHERIT_NP;
      }     
      else if( (flags & THREAD_PROCESSOR_SPUFLOAT) != 0)
      {
        __spu_t = PTHREAD_SPUFLOAT_NP;
      }
      else if( (flags & THREAD_PROCESSOR_LDOMINHERIT) != 0)
      {
        __spu_t = PTHREAD_LDOMINHERIT_NP;
      }
      else if( (flags & THREAD_PROCESSOR_LDOMFLOAT) != 0)
      {
        __spu_t = PTHREAD_LDOMFLOAT_NP;
      }
            
      pthread_attr_setprocessor_np(&attr, __spu_t,__bind_type);
    }
    else if(    ((flags & THREAD_BIND_ADVISORY) != 0)  
             || ((flags & THREAD_BIND_FORCED) != 0)  
             || ((flags & THREAD_BIND_LDOM) != 0))
    {
      pthread_spu_t __spu_t = spuid;
      int  __bind_type = 0;  

      if( (flags & THREAD_BIND_ADVISORY) != 0)
      {
        __bind_type = PTHREAD_BIND_ADVISORY_NP;
      }     
      else if( (flags & THREAD_BIND_FORCED) != 0)
      {
        __bind_type = PTHREAD_BIND_FORCED_NP;
      }
      else if( (flags & THREAD_BIND_LDOM) != 0)
      {
        __bind_type = PTHREAD_BIND_LDOM_NP;
      }
      
      pthread_attr_setprocessor_np(&attr, __spu_t,__bind_type);
    }
        
   #endif
    
//-------------------------------------------------------------------------------------------
  }//~end if(flags != 0)

//    ###################################################
//    # 7. Set StackSize
//    ###################################################

  if(thread_stackSize > 0)
  {
    size_t stacksize = (size_t)thread_stackSize;
    #if !defined(PORT_WINDOWS)
    pthread_attr_setstacksize(&attr,stacksize);
    #endif
  }

//    ###################################################
//    # 8. create Thread 
//    ###################################################
#if defined(PORT_WINDOWS)
  unsigned long  threadId = 0;
  h_thread  = (OPENCODE_TYPE_THREAD)::CreateThread(NULL, 0, opencode_thread_proc, (void*)pThread, 0, &threadId);

  while(threadId == 0);
  threadid = threadId;
  createThreadID = threadId; 

#else
  pthread_t  threadId = 0;
  int __create_rc = 0;
  bool __has_tried = false;

  while(true)
  { 
    if( (__create_rc = pthread_create(&threadId, &attr,opencode_thread_proc , (void*)pThread)) != 0)
    {

      #if defined(PORT_AIX)
      if( (!__has_tried) && (__create_rc == EPERM) && (flags != 0) && ( (flags & THREAD_SCOPE_SYSTEM) != 0) )
      {
        pthread_attr_setscope(&attr, PTHREAD_SCOPE_PROCESS);
        __has_tried = true;
        continue;
      }
      #endif
 
      pthread_attr_destroy(&attr);
      __OPENCODE_THROW(IOException(string("pthread_create failed - ").append(System::getLastError(__create_rc))));
      break;
    }

    pthread_attr_destroy(&attr);
    break;

  }//~end while(true)

  while(threadId == 0);
  h_thread = threadId;
  threadid = threadId;
  createThreadID = threadId; 
#endif

  return createThreadID;
}

bool  Thread::deleteOnExit() const
{
  return thread_deleteOnExit;
}

void  Thread::setPriority(int newPriority)
{
  this->thread_priority = newPriority;
}

int   Thread::getPriority() const
{
  return thread_priority;
}

void   Thread::setStackSize(XLONG stackSize)
{
  thread_stackSize = stackSize;
}

XLONG    Thread::getStackSize() const
{
  return thread_stackSize;
}
    
UXLONG  Thread::getID()const
{
  return threadid;
}

int  Thread::start(int spuid) __OPENCODE_THROW_DECLARE(IOException)
{
  if(started)
  {
    return 0;
  }

  Thread::createThread(this,spuid);
 
  started = true;

  return 0;
}

int  Thread::join(XLONG millis, XLONG nanos) __OPENCODE_THROW_DECLARE(IOException)
{
  if (millis < 0)
  {
    return 0;
  }

  if (nanos < 0 || nanos > 999999999)
  {
    nanos = 0;
  }

  nanos += ((millis%1000)*1000*1000);

  if(nanos < 0 || nanos > 999999999)
  {
    return 0;
  }

  if( (thread_flags & THREAD_CREATE_JOINABLE) == 0)
  {
    StringBuffer sb;
    sb << "Thread::join|" << (UXLONG)threadid << " not joinable";
    LOGGING_ERROR(sb.toString());
  }

  bool bSuccess = true; 
  #if defined(PORT_WINDOWS)
  if (::WaitForSingleObject((HANDLE)h_thread, INFINITE) != WAIT_OBJECT_0)
	{
		bSuccess = false;
	}
	::CloseHandle((HANDLE)h_thread);
  #else
  ::pthread_join(h_thread,0);
  #endif

  h_thread = 0;

  if (!bSuccess)
  {
    __OPENCODE_THROW(IOException());
  }

  return 0;

}

int  Thread::stop()
{
  if(sp_job.get() != 0)
  {
    sp_job->stop();
  }

  #if defined(PORT_WINDOWS)
  #else
  ::pthread_cancel(h_thread);
  #endif

  return 0;
}


void  Thread::run() 
{
  Thread::runJob(sp_job.get(),sp_jobData.get());
}

int  Thread::sleep(XLONG millis, XLONG nanos)
{
  if (millis < 0)
  {
    return 0 ;
  }

  if (nanos < 0 || nanos > 999999999)
  {
    nanos = 0;
  }

  nanos += ((millis%1000)*1000*1000);

  if(nanos < 0 || nanos > 999999999)
  {
    return 0;
  }

#if defined(PORT_WINDOWS)
  ::Sleep(millis);
#else
  time_t seconds  = (time_t)(millis/1000);
  nanos += (millis % 1000)* 1000000;

  struct timespec ts;
  ts.tv_sec = seconds ;
  ts.tv_nsec=(long)nanos;

  struct timespec ts_remains;
  while(true)
  {
    int ret_code=::nanosleep(&ts,&ts_remains);
    if(ret_code < 0 && errno == EINTR)
    {
      ts = ts_remains;
      continue;
    }

    break;
  }
#endif

  return 0;
}

UXLONG   Thread::getCurrentThreadId()
{
#if defined(PORT_WINDOWS)
  return (UXLONG)::GetCurrentThreadId();
#else
  return (UXLONG)::pthread_self();
#endif
}

bool  Thread::isAlive(UXLONG threadid)
{
#if defined(PORT_WINDOWS)
#else
  int ret_code=::pthread_kill((pthread_t)threadid,0);
  if( (ret_code==-1 && errno != EPERM))
  {
    return false;
  }
#endif

  return true;
}

void  Thread::runJob(Job* job,JobData* jobData) 
{
  if(job != 0)
  {
    job->run(jobData);
  }
}

__OPENCODE_END_NAMESPACE
