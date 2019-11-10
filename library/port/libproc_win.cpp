#include "port/libproc.h"
#include "port/stringTokenizer.h"
#include "port/libsys.h"
#include "port/utilities.h"

__OPENCODE_BEGIN_NAMESPACE

#if defined(PORT_WINDOWS)

XLONG Process::makeDaemon()
{	
  return 0;
}

bool  Process::isAlive(const Process& proc)
{
  DWORD __exit_code = 0;
  HANDLE __event_handle = proc.__mh_proc_handle;
  GetExitCodeProcess(__event_handle, &__exit_code);
  if (__exit_code == STILL_ACTIVE)
  {
  	return true;
	}	
	
  return false;
}

void  Process::destroy()
{
	TerminateProcess(__mh_proc_handle, 1);
	CloseHandle(__mh_proc_handle);
	__mb_hasexited = true;
}

UXLONG  Process::getCurrentProcessId()
{
  return (UXLONG)::GetCurrentProcessId();
}

XLONG  Process::waitForProcessExit(const Process& proc)
{
  if(proc.__mb_hasexited == 0)
  {
  	return 0;
  }	
  
  DWORD __exit_code = 0;
  HANDLE __event_handle = proc.__mh_proc_handle;
  
  DWORD which = WaitForSingleObject(__event_handle, INFINITE);
  GetExitCodeProcess(__event_handle, &__exit_code);
  
  return (XLONG)__exit_code; 
}

Process  Process::forkP()
{
	Process __proc_forked;
  return __proc_forked;
}

Process   Process::forkAndExec(const vector<string>& cmdarray,const vector<string>& envp, const string& path) __OPENCODE_THROW_DECLARE(IOException)
{
	StringBuffer cmdbuf;
	for (int i = 0; i < cmdarray.size(); i++) 
	{
		if (i > 0) 
		{
		  cmdbuf.append(' ');
		}
		
		string s = cmdarray[i];
		if (s.find_first_of(' ') != string::npos || s.find_first_of('\t') != string::npos) 
		{
			if (s.at(0) != '"') 
			{
				cmdbuf.append('"');
				cmdbuf.append(s);
				if (StringsKit::endsWith(s,"\\"))
				{
					cmdbuf.append("\\");
				}
				cmdbuf.append('"');
			} 
			else if (StringsKit::endsWith(s,"\"")) 
			{
				/* The argument has already been quoted. */
				cmdbuf.append(s);
			} 
			else 
			{
				/* Unmatched quote for the argument. */
				//throw IllegalArgumentException();
			}
		} 
		else 
		{
			cmdbuf.append(s);
		}
	}
	
  StringBuffer envbuf;
  for (int i = 0; i < envp.size(); i++) 
  {
    envbuf.append(envp[i]).append('\0');
  }
  
  string cmdstr = cmdbuf.toString();
  string envstr = envbuf.toString();
         
  STARTUPINFO si;
  memset(&si, 0, sizeof(si));
  si.cb = sizeof(si);
  si.dwFlags = STARTF_USESTDHANDLES;  
  
  DWORD processFlag = CREATE_NO_WINDOW;
  char* __cwd = (char*)path.c_str();
  PROCESS_INFORMATION pi;
  BOOL  ret = ::CreateProcess(0,                       /* executable name */
                            (LPSTR)cmdstr.c_str(),   /* command line */
                            0,                       /* process security attribute */
                            0,                       /* thread security attribute */
                            TRUE,                    /* inherits system handles */
                            processFlag,             /* selected based on exe type */ 
                            (LPVOID)envstr.c_str(),          /* environment block */
                            (LPSTR)path.c_str(),    /* change to the new current directory */
                            &si,                     /* (in)  startup information */
                            &pi);                    /* (out) process information */
                                    
//#########################

  Process __proc_created;    
  __proc_created.__mul_proc_pid   = pi.dwProcessId;
  __proc_created.__mh_proc_handle = pi.hProcess;
  
  return __proc_created;
}

#endif

__OPENCODE_END_NAMESPACE

