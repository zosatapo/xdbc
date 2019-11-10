/**
 *  condition.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */

#ifndef __OPENCODE_CONDITION_H__
#define __OPENCODE_CONDITION_H__

#include "port/libdefs.h"
#include "port/mutex.h"

__OPENCODE_BEGIN_NAMESPACE

class __OPENCODE_CORE_API_DECL Condition 
{
private:
  Condition (const Condition&);
  Condition& operator= (const Condition&);

public:
  ~Condition();
  Condition ();

public:  
  /**
   * Signal one waiting thread.
   */
  int signal();

  /**
   * Signal -all- waiting threads.
   */
  int broadcast();

  /**
   * Block on condition. 
   */
  int wait(Mutex& mutex);

protected:
#if defined(PORT_WINDOWS)
  /** Number of waiting threads.*/
  long volatile waiters;

  /** Queue up threads waiting for the condition to become signaled.*/
  OPENCODE_TYPE_CONDITION sema;

  /**
   * An auto reset event used by the broadcast/signal thread to wait
   * for the waiting thread(s) to wake up and get a chance at the
   * semaphore.
   */
  OPENCODE_TYPE_CONDITION waitersDone;

  /** Keeps track of whether we were broadcasting or just signaling. */
  bool wasBroadCast;  
#else
  OPENCODE_TYPE_CONDITION condition;
#endif

};

__OPENCODE_END_NAMESPACE

#endif
