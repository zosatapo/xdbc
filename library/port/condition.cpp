#include "port/condition.h"
#include "port/libsys.h"
#include "port/logging.h"
#include "port/ioc_sbuf.h"

__OPENCODE_BEGIN_NAMESPACE

Condition::Condition ()
{
#if defined(PORT_WINDOWS)
	waiters = 0;
	wasBroadCast = false;
  sema = ::CreateSemaphore(0, (long)0, LONG_MAX, 0);
	waitersDone = ::CreateEvent(0, FALSE, FALSE, NULL);
#else
  ::pthread_cond_init(&condition, 0); 
#endif
}

Condition::~Condition () 
{ 
#if defined(PORT_WINDOWS)
  ::CloseHandle(waitersDone);
  ::CloseHandle(sema);
#else
  ::pthread_cond_destroy(&condition);
#endif
  
}

int Condition::signal() 
{
  int rc = 0;

#if defined(PORT_WINDOWS)
	// If there aren't any waiters, then this is a no-op.  Note that
	// this function *must* be called with the <external_mutex> held
	// since other wise there is a race condition that can lead to the
	// lost wakeup bug...  This is needed to ensure that the <waiters>
	// value is not in an inconsistent internal state while being
	// updated by another thread.

	// if (waiters != 0) (atomic comparison)
	if ((long)InterlockedCompareExchange(&waiters, 0, 0) != 0)
	{
    long previousCount;
	  if (!::ReleaseSemaphore(sema, 1, &previousCount))
	  {
      StringBuffer __sb;
      __sb << "Condition::signal|failed to signal condition: " <<  System::getLastError();  
      System::loggingAndExit(__sb.str(),-1);
	  }
	}
#else
  if ( (rc = ::pthread_cond_signal(&condition)) != 0)
  {
    StringBuffer __sb;
    __sb << "Condition::signal|failed to signal condition: " <<  System::getLastError(rc);  
    System::loggingAndExit(__sb.str(),-1);
  }
#endif

  return 0;
}

int Condition::broadcast () 
{
  int rc = 0;
#if defined(PORT_WINDOWS)
#else
  if ( (rc = ::pthread_cond_broadcast(&condition)) != 0)
  {
    StringBuffer __sb;
    __sb << "Condition::broadcast|failed to broadcast condition: " <<  System::getLastError(rc);   
    System::loggingAndExit(__sb.str(),-1);
  }
#endif

  return 0;
}

int Condition::wait(Mutex& mutex) 
{
  int rc = 0;

#if defined(PORT_WINDOWS)
  ::InterlockedIncrement(&waiters);

  if (SignalObjectAndWait(mutex.__mo_mutex, sema, INFINITE, FALSE) == WAIT_ABANDONED)
	{
    StringBuffer __sb;
    __sb << "Condition::wait|failed to wait condition: " <<  System::getLastError();
    System::loggingAndExit(__sb.str(),-1);
	}

  long oldWaiters = ::InterlockedDecrement(&waiters);
  bool lastWaiter = wasBroadCast && (oldWaiters == 0);

  if (lastWaiter)
	{
		// This call atomically signals the <waiters_done_> event and
		// waits until it can acquire the mutex.  This is important to
		// prevent unfairness.
    if (::SignalObjectAndWait(waitersDone, mutex.__mo_mutex, INFINITE, FALSE)== WAIT_ABANDONED)
		{
      StringBuffer __sb;
      __sb << "Condition::wait|failed to wait condition: " <<  System::getLastError();
      System::loggingAndExit(__sb.str(),-1);
		}
	}

  mutex.lock();
#else
  if ( (rc = ::pthread_cond_wait(&condition,&(mutex.__mo_mutex))) != 0)
  {
    StringBuffer __sb;
    __sb << "Condition::wait|failed to wait condition: " <<  System::getLastError(rc);
    System::loggingAndExit(__sb.str(),-1);
  }
#endif

  return 0;
}

__OPENCODE_END_NAMESPACE
