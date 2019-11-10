/*
 *  socketHandler.h  
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */

#ifndef  __OPENCODE_SOCKETHANDLER_H__
#define  __OPENCODE_SOCKETHANDLER_H__ 

#include "socket/socketdefs.h"
#include "socket/socket.h"

__OPENCODE_BEGIN_NAMESPACE

class __OPENCODE_NET_API_DECL SocketHandler
{
public:
  virtual ~SocketHandler(){}
  SocketHandler(){}

public:
  virtual void handleConnection(Socket& socket) = 0;
};

__OPENCODE_END_NAMESPACE

#endif
