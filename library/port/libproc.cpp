#include "port/libproc.h"
#include "port/stringTokenizer.h"
#include "port/libsys.h"

__OPENCODE_BEGIN_NAMESPACE

Process::Process()
{
#if defined(PORT_WINDOWS)
  __mh_proc_handle = 0;
#endif
  __mi_proc_exitcode = 0;
  __mb_hasexited = false;
  __mul_proc_pid = 0; 
}

Process::Process(const vector<string>& cmdarray, const vector<string>& envp) __OPENCODE_THROW_DECLARE(IOException)
{
  Process __proc_forked = Process::forkAndExec(cmdarray,envp,"");
  Process::copyProcessData(*this,__proc_forked);	
}

Process::Process(const vector<string>& cmdarray, const vector<string>& envp, const string& path) __OPENCODE_THROW_DECLARE(IOException)
{
  Process __proc_forked = Process::forkAndExec(cmdarray,envp,path);
  Process::copyProcessData(*this,__proc_forked);	
}

Process   Process::exec(const string& command) __OPENCODE_THROW_DECLARE(IOException)
{
  vector<string> envp;
  return Process::exec(command,envp);
}

Process  Process::exec(const string& command,const vector<string>& envp) __OPENCODE_THROW_DECLARE(IOException)
{
  return Process::exec(command,envp,0);
}

Process   Process::exec(const string& command,const vector<string>& envp,const File* dir) __OPENCODE_THROW_DECLARE(IOException)
{
  if (command.empty())
  { 
    System::loggingAndExit("Process::exec|Empty command",-1);  
  }

  StringTokenizer st(command);
  vector<string> cmdarray;

  while (st.hasMoreTokens()) 
  {
    cmdarray.push_back(st.nextToken());
  }

  return  Process::exec(cmdarray, envp, dir); 
}

Process  Process::exec(const vector<string>& cmdarray) __OPENCODE_THROW_DECLARE(IOException)
{
  vector<string> envp;
  return Process::exec(cmdarray,envp); 
}

Process  Process::exec(const vector<string>& cmdarray,const vector<string>& envp) __OPENCODE_THROW_DECLARE(IOException)
{
  return Process::exec(cmdarray,envp,0);
}

Process  Process::exec(const vector<string>& cmdarray,const vector<string>& envp,const File* dir) __OPENCODE_THROW_DECLARE(IOException)
{
  int size = (int)(cmdarray.size());

  if (size == 0) 
  {
    System::loggingAndExit("Process::exec|cmdarray is empty",-1);        
  }

  for(int i = 0; i < size;++i)
  {
    if(cmdarray[i].empty())
    {
      System::loggingAndExit("Process::exec|cmdarray is empty",-1); 
    }
  }

  string path; 

  if(dir != 0)
  {
    path = dir->getPath();
  }

  return Process::execInternal(cmdarray,envp,path);
}

Process  Process::execInternal(const vector<string>& cmdarray,const vector<string>& envp, const string& path) __OPENCODE_THROW_DECLARE(IOException)
{
  Process __sys_proc(cmdarray,envp,path);
  return __sys_proc;
}

XLONG  Process::waitFor() __OPENCODE_THROW_DECLARE(IOException)
{
  __mi_proc_exitcode = Process::waitForProcessExit(*this);
  __mb_hasexited = true;
  return __mi_proc_exitcode;
}

XLONG   Process::exitValue() __OPENCODE_THROW_DECLARE(IOException)
{
  if (!__mb_hasexited) 
  {
    System::loggingAndExit("Process::exitValue|process hasn't exited",-1);
  }

  return __mi_proc_exitcode;
}

void  Process::copyProcessData(Process& to,const Process& from)
{
#if defined(PORT_WINDOWS)
  to.__mh_proc_handle   = from.__mh_proc_handle;
#endif
	
  to.__mul_proc_pid     = from.__mul_proc_pid;
  to.__mb_hasexited     = from.__mb_hasexited;
  to.__mi_proc_exitcode = from.__mi_proc_exitcode;
}

#if !defined(PORT_WINDOWS)

XLONG   Process::makeDaemon()
{
  pid_t pid = 0;

  if ((pid = fork())  <  0){  exit(-1);}
  else if (pid != 0) {exit(0);}
  
  setsid();

  if ((pid = fork())  <  0){  exit(-1);}
  else if (pid != 0) {exit(0);}
  
  chdir("/");
  umask(0);
  
  return 0;
}

Process  Process::forkP()
{
	Process __proc_forked;
	__proc_forked.__mul_proc_pid = (UXLONG)::fork();
  return __proc_forked;
}

bool  Process::isAlive(const Process& proc)
{
  int retcode = ::kill((pid_t)proc.__mul_proc_pid, 0);
  
  if((retcode == -1) && (errno == ESRCH))
  {
    return false;
  }

  return true;
}

void  Process::destroy()
{
  ::kill((pid_t)__mul_proc_pid, SIGTERM);
}


UXLONG  Process::getCurrentProcessId()
{
  return (UXLONG)::getpid();
}

XLONG  Process::waitForProcessExit(const Process& proc)
{
  int status = 0;
  int info = 0;

  /** Wait for the child process to exit.  This returns immediately if the child has already exited. */
  if (waitpid((pid_t)proc.__mul_proc_pid, &info, 0) < 0)
  {
    if (errno == ECHILD) 
    {
      return 0;
    } 
    else 
    {
      return -1;
    }
  }
  
  if (WIFEXITED(info)) 
  {
    /** The child exited normally, get its exit code */
    status = (signed char)WEXITSTATUS(info);
  } 
  else if (WIFSIGNALED(info)) 
  {
    /** The child exited because of a signal, compute its exit code based on the signal number.*/
    status = 0x80 + WTERMSIG(status);
  } 
  else 
  {
    /** Unknown exit code, pass it through.*/
    status = info;
  }

  return status;  
}

Process   Process::forkAndExec(const vector<string>& cmdarray,const vector<string>& envp, const string& path) __OPENCODE_THROW_DECLARE(IOException)
{
  int cmdlen = (int)(cmdarray.size());
  char** cmdv = (char **)malloc((cmdlen + 1) * sizeof(char *));

  for (int i = 0; i < cmdlen; ++i)
  {
    cmdv[i] = (char*)malloc(cmdarray[i].size() + 1);
    memset(cmdv[i],0,cmdarray[i].size() + 1);
    memcpy(cmdv[i],cmdarray[i].c_str(),cmdarray[i].size());
  }

  int envlen = (int)(envp.size());
  char** envv = (char **)malloc((envlen + 1) * sizeof(char *));

  for (int i = 0; i < envlen; ++i)
  {
    envv[i] = (char*)malloc(envp[i].size() + 1);
    memset(envv[i],0,envp[i].size() + 1);
    memcpy(envv[i],envp[i].c_str(),envp[i].size());
  }

  Process __proc_forked = Process::forkP();
  pid_t __internal_procid = (pid_t)__proc_forked.__mul_proc_pid;
  if(__internal_procid == 0)
  {
    if (0 != envp.size()) 
    {
      /* So why isn't there an execvpe(), then? */
      //extern char ** _environ;
      //_environ = envv;
    }

    execvp(cmdv[0], cmdv);
    exit(-1);
  }

  for (int j = 0; j < envlen; ++j) 
  { 
    free(envv[j]);
  }
  free(envv);

  for (int j = 1; j < cmdlen; ++j) 
  {
    free(cmdv[j]);
  }
  free(cmdv);

  return __proc_forked;
}

#endif

__OPENCODE_END_NAMESPACE

