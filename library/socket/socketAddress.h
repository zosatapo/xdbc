/** 
 *  socketAddress.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef __OPENCODE_SOCKETADDRESS_H__
#define __OPENCODE_SOCKETADDRESS_H__

#include "socket/socketdefs.h"
#include "socket/inetAddress.h"

__OPENCODE_BEGIN_NAMESPACE

class __OPENCODE_NET_API_DECL SocketAddress
{
  
public:
  SocketAddress();
  SocketAddress(const InetAddress& inetAddress,int port);

  SocketAddress(const SocketAddress& rv);
  SocketAddress& operator=(const SocketAddress& rv);

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
public:
  bool operator==(const SocketAddress& rhs) const;

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
public: 
  const InetAddress& getInetAddress() const;
  int getPort() const;

  string toString() const;
  
private:
 InetAddress inetAddress;
 
 // host byte order
 int  int_port; 
 
};

__OPENCODE_END_NAMESPACE

#endif
