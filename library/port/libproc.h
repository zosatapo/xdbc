/** 
 *  libproc.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef __OPENCODE_OS_PROCESS_H__
#define __OPENCODE_OS_PROCESS_H__

#include "port/libdefs.h"
#include "port/file.h"
#include "port/ioc_vector.h"

__OPENCODE_BEGIN_NAMESPACE

class __OPENCODE_CORE_API_DECL Process
{
public:
  Process();
 
  Process(const vector<string>& cmdarray, const vector<string>& envp) __OPENCODE_THROW_DECLARE(IOException);
  Process(const vector<string>& cmdarray, const vector<string>& envp, const string& path) __OPENCODE_THROW_DECLARE(IOException);

public:
  /**
   * causes the current thread to wait, if necessary, until the 
   * process represented by this <code>Process</code> object has 
   * terminated. This method returns 
   * immediately if the subprocess has already terminated. If the
   * subprocess has not yet terminated, the calling thread will be
   * blocked until the subprocess exits.
   *
   * @return     the exit value of the process. By convention, 
   *             <code>0</code> indicates normal termination.
   * @exception  IOException  if the current thread is 
   *             interrupted by another thread 
   *             while it is waiting, then the wait is ended and an 
   *             <code>IOException</code> is thrown.
   */
  XLONG __OPENCODE_NATIVE_CODE waitFor() __OPENCODE_THROW_DECLARE(IOException);
  
  /**
   * Returns the exit value for the subprocess.
   *
   * @return  the exit value of the subprocess represented by this 
   *          <code>Process</code> object. by convention, the value 
   *          <code>0</code> indicates normal termination.
   * @exception  IOException  if the subprocess represented 
   *             by this <code>Process</code> object has not yet terminated.
   */
  XLONG __OPENCODE_NATIVE_CODE exitValue() __OPENCODE_THROW_DECLARE(IOException);
  
  /**
   * Kills the subprocess. The subprocess represented by this 
   * <code>Process</code> object is forcibly terminated.
   */
  void __OPENCODE_NATIVE_CODE destroy();

public:
  static XLONG    __OPENCODE_NATIVE_CODE makeDaemon();
  static Process  __OPENCODE_NATIVE_CODE forkP();
  static XLONG    __OPENCODE_NATIVE_CODE waitForProcessExit(const Process& proc);
  static UXLONG   __OPENCODE_NATIVE_CODE getCurrentProcessId();
  static bool     __OPENCODE_NATIVE_CODE isAlive(const Process& proc);
  
public:
  /**
   * Executes the specified string command in a separate process. 
   * <p>
   * The <code>command</code> argument is parsed into tokens and then 
   * executed as a command in a separate process. The token parsing is 
   * done by a {@link base.StringTokenizer} created by the call:
   * <blockquote><pre>
   * new StringTokenizer(command)
   * </pre></blockquote> 
   * with no further modifications of the character categories. 
   * This method has exactly the same effect as 
   * <code>exec(command, null)</code>. 
   *
   * @param      command   a specified system command.
   * @return     a <code>Process</code> object for managing the subprocess.
   * @exception  IOException if an I/O error occurs
   */ 
  static Process __OPENCODE_NATIVE_CODE exec(const string& command) __OPENCODE_THROW_DECLARE(IOException);

  /**
   * Executes the specified string command in a separate process with the 
   * specified environment. 
   * <p>
   * This method breaks the <code>command</code> string into tokens and 
   * creates a new array <code>cmdarray</code> containing the tokens in the 
   * order that they were produced by the string tokenizer; it 
   * then performs the call <code>exec(cmdarray, envp)</code>. The token
   * parsing is done by a {@link base.StringTokenizer} created by 
   * the call: 
   * <blockquote><pre>
   * new StringTokenizer(command)
   * </pre></blockquote>
   * with no further modification of the character categories. 
   *
   * <p>
   * The environment variable settings are specified by <tt>envp</tt>.
   * If <tt>envp</tt> is <tt>null</tt>, the subprocess inherits the 
   * environment settings of the current process.
   *
   * @param      cmd       a specified system command.
   * @param      envp      array of strings, each element of which 
   *                       has environment variable settings in format
   *                       <i>name</i>=<i>value</i>.
   * @return     a <code>Process</code> object for managing the subprocess.
   * @exception  IOException if an I/O error occurs
   */
  static Process __OPENCODE_NATIVE_CODE exec(const string& command,const vector<string>& envp) __OPENCODE_THROW_DECLARE(IOException);

  /**
   * Executes the specified string command in a separate process with the
   * specified environment and working directory. 
   * <p>
   * This method breaks the <code>command</code> string into tokens and 
   * creates a new array <code>cmdarray</code> containing the tokens in the 
   * order that they were produced by the string tokenizer; it 
   * then performs the call <code>exec(cmdarray, envp)</code>. The token
   * parsing is done by a {@link base.StringTokenizer} created by 
   * the call: 
   * <blockquote><pre>
   * new StringTokenizer(command)
   * </pre></blockquote>
   * with no further modification of the character categories. 
   *
   * <p>
   * The environment variable settings are specified by <tt>envp</tt>.
   * If <tt>envp</tt> is <tt>null</tt>, the subprocess inherits the 
   * environment settings of the current process.
   *
   * <p>
   * The working directory of the new subprocess is specified by <tt>dir</tt>.
   * If <tt>dir</tt> is <tt>null</tt>, the subprocess inherits the 
   * current working directory of the current process.
   *
   * @param      command   a specified system command.
   * @param      envp      array of strings, each element of which 
   *                       has environment variable settings in format
   *                       <i>name</i>=<i>value</i>.
   * @param      dir       the working directory of the subprocess, or
   *                       <tt>null</tt> if the subprocess should inherit
   *                       the working directory of the current process.
   * @return     a <code>Process</code> object for managing the subprocess.
   * @exception  IOException if an I/O error occurs
   */
  static Process __OPENCODE_NATIVE_CODE exec(const string& command,const vector<string>& envp,const File* dir) __OPENCODE_THROW_DECLARE(IOException);

  /**
   * Executes the specified command and arguments in a separate process.
   * <p>
   * The command specified by the tokens in <code>cmdarray</code> is 
   * executed as a command in a separate process. This has exactly the 
   * same effect as <code>exec(cmdarray, null)</code>. 
   * <p>
   * If there is a security manager, its <code>checkExec</code> 
   * method is called with the first component of the array 
   * <code>cmdarray</code> as its argument. This may result in a security 
   * exception. 
   *
   * @param      cmdarray   array containing the command to call and
   *                        its arguments.
   * @return     a <code>Process</code> object for managing the subprocess.
   * @exception  IOException if an I/O error occurs
   */
  static Process __OPENCODE_NATIVE_CODE exec(const vector<string>& cmdarray) __OPENCODE_THROW_DECLARE(IOException);

  /**
   * Executes the specified command and arguments in a separate process
   * with the specified environment. 
   * <p>
   * Given an array of strings <code>cmdarray</code>, representing the 
   * tokens of a command line, and an array of strings <code>envp</code>, 
   * representing "environment" variable settings, this method creates 
   * a new process in which to execute the specified command. 
   *
   * <p>
   * If <tt>envp</tt> is <tt>null</tt>, the subprocess inherits the 
   * environment settings of the current process.
   *
   * @param      cmdarray   array containing the command to call and
   *                        its arguments.
   * @param      envp       array of strings, each element of which 
   *                        has environment variable settings in format
   *                        <i>name</i>=<i>value</i>.
   * @return     a <code>Process</code> object for managing the subprocess.
   * @exception  IOException if an I/O error occurs
   */
  static Process __OPENCODE_NATIVE_CODE exec(const vector<string>& cmdarray,const vector<string>& envp) __OPENCODE_THROW_DECLARE(IOException);

  /**
   * Executes the specified command and arguments in a separate process with
   * the specified environment and working directory. 
   * <p>
   * If there is a security manager, its <code>checkExec</code> 
   * method is called with the first component of the array 
   * <code>cmdarray</code> as its argument. This may result in a security 
   * exception. 
   * <p>
   * Given an array of strings <code>cmdarray</code>, representing the 
   * tokens of a command line, and an array of strings <code>envp</code>, 
   * representing "environment" variable settings, this method creates 
   * a new process in which to execute the specified command. 
   *
   * <p>
   * If <tt>envp</tt> is <tt>null</tt>, the subprocess inherits the 
   * environment settings of the current process.
   *
   * <p>
   * The working directory of the new subprocess is specified by <tt>dir</tt>.
   * If <tt>dir</tt> is <tt>null</tt>, the subprocess inherits the 
   * current working directory of the current process.
   *
   *
   * @param      cmdarray   array containing the command to call and
   *                        its arguments.
   * @param      envp       array of strings, each element of which 
   *                        has environment variable settings in format
   *                        <i>name</i>=<i>value</i>.
   * @param      dir        the working directory of the subprocess, or
   *                        <tt>null</tt> if the subprocess should inherit
   *                        the working directory of the current process.
   * @return     a <code>Process</code> object for managing the subprocess.
   * @exception  IOException if an I/O error occurs.
   */
  static Process __OPENCODE_NATIVE_CODE exec(const vector<string>& cmdarray,const vector<string>& envp,const File* dir) __OPENCODE_THROW_DECLARE(IOException);

private:
  static Process __OPENCODE_NATIVE_CODE execInternal(const vector<string>& cmdarray, const vector<string>& envp, const string& path) __OPENCODE_THROW_DECLARE(IOException);   
  static Process __OPENCODE_NATIVE_CODE forkAndExec(const vector<string>& cmdarray, const vector<string>& envp, const string& path) __OPENCODE_THROW_DECLARE(IOException);
  static void    __OPENCODE_NATIVE_CODE copyProcessData(Process& to,const Process& from);

  UXLONG __mul_proc_pid;
  bool   __mb_hasexited;  	
  XLONG  __mi_proc_exitcode;
#if defined(PORT_WINDOWS)  
  HANDLE __mh_proc_handle;
#endif
  
 };

__OPENCODE_END_NAMESPACE

#endif
