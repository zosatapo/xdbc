/**
 *  thread.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */

#ifndef __OPENCODE_THREAD_H__
#define __OPENCODE_THREAD_H__

#include "port/libdefs.h"
#include "port/file.h"
#include "port/ioc_smart.h"
#include "port/ioc_threads.h" 

__OPENCODE_BEGIN_NAMESPACE

class Thread;

//------------------------------------------------------
//0. class ThreadSpecificData
//------------------------------------------------------
class  __OPENCODE_CORE_API_DECL ThreadSpecificData
{
public:
  ~ThreadSpecificData();  
  ThreadSpecificData();

  void * getData() const;
  void setData(void * data);

protected:
  OPENCODE_TYPE_TSDKEY key;
};

//------------------------------------------------------
//1. class JobData 
//------------------------------------------------------
class __OPENCODE_CORE_API_DECL JobData
{
private:
  JobData(const JobData& jobData);
  JobData& operator=(const JobData& jobData);

protected:  
  JobData(){}
     
public:
  virtual ~JobData() {}
};

//------------------------------------------------------
//2. class Job
//------------------------------------------------------
class __OPENCODE_CORE_API_DECL Job
{
private:
  Job(const Job& job);
  Job& operator=(const Job& job);

protected:
  Job(){}
     
public:
  virtual ~Job(){}

public:  
  virtual void run(JobData* jobData)  = 0;
  virtual void stop() = 0;
};

//------------------------------------------------------
//3. class Thread 
//------------------------------------------------------

//##########################
//  detachstate (0-3)
//##########################
#define THREAD_CREATE_JOINABLE          0x00000001
#define THREAD_CREATE_DETACHED          0x00000002

//##########################
//  schedpolicy (4-7) 
//##########################
#define THREAD_SCHED_FIFO               0x00000010
#define THREAD_SCHED_RR                 0x00000020 
#define THREAD_SCHED_OTHER              0x00000040

//##########################
//  inheritsched (8-11)
//##########################
#define THREAD_INHERIT_SCHED            0x00000100
#define THREAD_EXPLICIT_SCHED           0x00000200

//##########################
//  inheritsched (12-15)
//##########################
#define THREAD_SCOPE_SYSTEM             0x00001000
#define THREAD_SCOPE_PROCESS            0x00002000

///////////////////////////////////////////////////////////////////////////////////////

//##########################
//  processor_np  (16-19)
//##########################
#define THREAD_PROCESSOR_SPUINHERIT      0x00010000
#define THREAD_PROCESSOR_SPUFLOAT        0x00020000
#define THREAD_PROCESSOR_LDOMINHERIT     0x00040000
#define THREAD_PROCESSOR_LDOMFLOAT       0x00080000

//##########################
//  bind_type  (20-23)
//##########################
#define THREAD_BIND_ADVISORY              0x00100000
#define THREAD_BIND_FORCED                0x00200000
#define THREAD_BIND_LDOM                  0x00400000

///////////////////////////////////////////////////////////////////////////////////////


#define THR_CREATE_DEFAULT             (THREAD_CREATE_JOINABLE | THREAD_SCHED_RR | THREAD_EXPLICIT_SCHED | THREAD_SCOPE_SYSTEM | THREAD_PROCESSOR_SPUFLOAT) 

#define THREAD_PRIORITY_LOW            0
#define THREAD_PRIORITY_NORM           1
#define THREAD_PRIORITY_HIGH           2

#define THR_PRIORITY_DEFAULT          (THREAD_PRIORITY_NORM)

//------------------------------------------------------
#if defined(PORT_WINDOWS)
DWORD WINAPI       opencode_thread_proc(void * param);
#else
extern "C"  void*  opencode_thread_proc(void * param);
#endif

//------------------------------------------------------

class __OPENCODE_CORE_API_DECL Thread
{
protected:
  smart_ptr<Job> sp_job;
  smart_ptr<JobData> sp_jobData;

  bool thread_deleteOnExit;
  bool started;

  UXLONG threadid;
  OPENCODE_TYPE_THREAD h_thread;

  int thread_flags;
  int thread_priority;
  XLONG thread_stackSize;
  
public:
  virtual ~Thread() ;

  /**
   * @param job         the object whose <code>run</code> method is called.
   *                    Thread must own the Job
   *
   * @param jobData     the thread data to be given to Job#run . 
   *                    Thread must own the JobData
   */
  Thread(Job* job ,JobData* jobData = 0, int flags = THR_CREATE_DEFAULT , bool deleteOnExit = false);   
  
public:
  
  bool  deleteOnExit() const;
 
  void  __OPENCODE_NATIVE_CODE setPriority(int newPriority);
  int   __OPENCODE_NATIVE_CODE getPriority() const;

  void  __OPENCODE_NATIVE_CODE setStackSize(XLONG stackSize);
  XLONG __OPENCODE_NATIVE_CODE getStackSize() const;
  
  UXLONG getID()const;
  int getFlags() const; 

public:
  virtual int __OPENCODE_NATIVE_CODE start(int spuid = -1) __OPENCODE_THROW_DECLARE(IOException);
  virtual int __OPENCODE_NATIVE_CODE join(XLONG millis = 0, XLONG nanos = 0) __OPENCODE_THROW_DECLARE(IOException);
  virtual int __OPENCODE_NATIVE_CODE stop();

  virtual void run() ;

protected:

  UXLONG  __OPENCODE_NATIVE_CODE createThread(const Thread* pThread ,int spuid = -1) __OPENCODE_THROW_DECLARE(IOException);
  
//--------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------

public:
  static int             __OPENCODE_NATIVE_CODE  sleep(XLONG millis, XLONG nanos = 0) ;
  static UXLONG  __OPENCODE_NATIVE_CODE  getCurrentThreadId();
  static bool            __OPENCODE_NATIVE_CODE  isAlive(UXLONG threadid);
  
  static void            runJob(Job* job,JobData* jobData) ;
};

__OPENCODE_END_NAMESPACE

#endif
