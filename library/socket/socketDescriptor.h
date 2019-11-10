/** 
 *  socketDescriptor.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef __OPENCODE_SOCKETDESCRIPTIOR_H__
#define __OPENCODE_SOCKETDESCRIPTIOR_H__

#include "socket/socketdefs.h"

__OPENCODE_BEGIN_NAMESPACE

class __OPENCODE_NET_API_DECL SocketDescriptor
{
//-------------------------------------------------------
//
//-------------------------------------------------------
public:
  SocketDescriptor();
  SocketDescriptor(OPENCODE_TYPE_SOCKET fd);
  SocketDescriptor(OPENCODE_TYPE_SOCKET fd,int family);
  
//-------------------------------------------------------
//
//-------------------------------------------------------
public:
  bool   isValid() const;
  OPENCODE_TYPE_SOCKET  getDescriptor() const;  
  int   getFamily() const;  
  
//-------------------------------------------------------
//
//-------------------------------------------------------
public:
  void  invalidate();  
  void  setDescriptor(const OPENCODE_TYPE_SOCKET& shm);
  void  setDescriptor(const OPENCODE_TYPE_SOCKET& shm,int family);
  
//-------------------------------------------------------
//
//-------------------------------------------------------  
private:
  OPENCODE_TYPE_SOCKET  int_fd;  
  int     int_family;
};

__OPENCODE_END_NAMESPACE

#endif
