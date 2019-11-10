/** 
 *  socketException.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef __OPENCODE_SOCKETEXCEPTION_H__
#define __OPENCODE_SOCKETEXCEPTION_H__

#include "socket/socketdefs.h"

__OPENCODE_BEGIN_NAMESPACE

class __OPENCODE_NET_API_DECL SocketException : public BaseException
{
public:
  virtual ~SocketException();
  
  SocketException();
  SocketException(const string& message);
  SocketException(const string& message,XLONG errcode);
};

class __OPENCODE_NET_API_DECL SocketTimeoutException : public SocketException
{
public:
  virtual ~SocketTimeoutException();
  
  SocketTimeoutException();
  SocketTimeoutException(const string& reason);
};

class __OPENCODE_NET_API_DECL UnknownHostException : public SocketException
{
public:
  virtual ~UnknownHostException();
  
  UnknownHostException();
  UnknownHostException(const string& reason);
};

class __OPENCODE_NET_API_DECL ConnectionResetException : public SocketException
{
public:
  virtual ~ConnectionResetException();
  
  ConnectionResetException();
  ConnectionResetException(const string& reason);
  ConnectionResetException(const string& message,XLONG errcode);
};

__OPENCODE_END_NAMESPACE

#endif
