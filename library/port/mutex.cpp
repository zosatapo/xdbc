#include "port/mutex.h"
#include "port/thread.h"
#include "port/libsys.h"
#include "port/ioc_sbuf.h"
#include "port/logging.h"

__OPENCODE_BEGIN_NAMESPACE

Mutex::Mutex ()
{
#if defined(PORT_WINDOWS)
  __mo_mutex = ::CreateMutex(0, 0, 0);
#else
  pthread_mutexattr_t  mutexattr;
  ::pthread_mutexattr_init(&mutexattr);

  ::pthread_mutexattr_settype(&mutexattr,PTHREAD_MUTEX_RECURSIVE);

  int rc = ::pthread_mutex_init(&__mo_mutex,&mutexattr);
  ::pthread_mutexattr_destroy(&mutexattr);
#endif

  owningThread = 0;
}

Mutex::~Mutex () 
{
#if defined(PORT_WINDOWS)
  ::CloseHandle(__mo_mutex);
#else
  ::pthread_mutex_destroy(&__mo_mutex);
#endif
}

int Mutex::lock() 
{
  int rc = 0;

#if defined(PORT_WINDOWS)
  if (::WaitForSingleObject(__mo_mutex, INFINITE) == WAIT_ABANDONED)
  {
    StringBuffer __sb;
    __sb << "Mutex::lock|failed to lock mutex: " <<  System::getLastError();   
    System::loggingAndExit(__sb.str(),-1);
  }
#else
  if ( (rc = ::pthread_mutex_lock(&__mo_mutex)) != 0)
  {
    StringBuffer __sb;
    __sb << "Mutex::lock|failed to lock mutex: " <<  System::getLastError(rc);
    
    System::loggingAndExit(__sb.str(),-1);
  }
#endif

  owningThread = Thread::getCurrentThreadId();

  return 0;
}

int Mutex::unlock()  
{
  int rc = 0;

#if defined(PORT_WINDOWS)
  if (!::ReleaseMutex(__mo_mutex))
	{
    StringBuffer __sb;
    __sb << "Mutex::unlock|failed to unlock mutex: " <<  System::getLastError();

    System::loggingAndExit(__sb.str(),-1);
	}
#else
  if ( (rc = ::pthread_mutex_unlock(&__mo_mutex)) != 0)
  {
    StringBuffer __sb;
    __sb << "Mutex::unlock|failed to unlock mutex: " <<  System::getLastError(rc);

    System::loggingAndExit(__sb.str(),-1);
  }
#endif

  owningThread = 0; 

  return 0;
}

const UXLONG&  Mutex::getOwningThread()
{
  return owningThread;
}
//----------------------------------------------------------

Mutex::Guard::Guard (Mutex& mutex)   
: __mutex(mutex),__useThreads(true) 
{
  __mutex.lock();
}

Mutex::Guard::Guard(Mutex& mutex,bool useThreads)
: __mutex(mutex),__useThreads(useThreads)
{
  if(__useThreads)
  {
    __mutex.lock();
  }
}

Mutex::Guard::~Guard () 
{
  if(__useThreads)
  {
    __mutex.unlock();
  }
}

__OPENCODE_END_NAMESPACE
