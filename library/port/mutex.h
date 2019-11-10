/**
 *  mutex.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */

#ifndef __OPENCODE_MUTEX_H__
#define __OPENCODE_MUTEX_H__

#include "port/libdefs.h"
#include "port/ioc_threads.h" 

__OPENCODE_BEGIN_NAMESPACE

class __OPENCODE_CORE_API_DECL Mutex
{
public:
  friend class Guard;
  friend class Condition;

private:
  Mutex (const Mutex&);
  Mutex& operator= (const Mutex&);

public:
  ~Mutex() ;
  Mutex ();

  int lock()  ;
  int unlock()  ;

  const UXLONG& getOwningThread();

//------------------------------------------------
// -BEGIN- UTIL LOCK CLASS
//------------------------------------------------
public:
  class __OPENCODE_CORE_API_DECL Guard 
  {
    public:    
      ~Guard();

      explicit Guard (Mutex& mutex)  ; 
      explicit Guard (Mutex& mutex,bool useThreads)  ; 
      
   private:
      Guard (const Guard&);
      Guard& operator= (const Guard&);
    
    private:
      Mutex& __mutex;
      bool   __useThreads;
  };
//------------------------------------------------
// -END- UTIL LOCK CLASS
//------------------------------------------------

protected:
  OPENCODE_TYPE_MUTEX  __mo_mutex;
  UXLONG     owningThread;
}; 
__OPENCODE_END_NAMESPACE

#endif
