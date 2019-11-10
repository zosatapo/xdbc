/** 
 *  socketUtils.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef __OPENCODE_SOCKETUTILS_H__
#define __OPENCODE_SOCKETUTILS_H__

#include "socket/socketdefs.h"
#include "socket/socketException.h"
#include "socket/inetAddress.h"
#include "socket/socketAddress.h"
#include "socket/networkInterface.h"
#include "socket/socketOptions.h"
#include "socket/datagramSocket.h" 
#include "socket/socket.h" 
#include "socket/serverSocket.h" 

__OPENCODE_BEGIN_NAMESPACE

struct SocketOptionsMap
{
  int cmd;
  int level;
  int optname;
};

#define  TIMEOUT_TYPE_WRITE      0
#define  TIMEOUT_TYPE_ACCEPT    1
#define  TIMEOUT_TYPE_CONNECT  2
#define  TIMEOUT_TYPE_READ        3

#if defined(PORT_WINDOWS)
__OPENCODE_NET_API_DECL int __netwin_getDefaultTOS();

__OPENCODE_NET_API_DECL int __netwin_socketClose(int fd);
__OPENCODE_NET_API_DECL int __netwin_bind(int s, struct sockaddr *him, int len);

__OPENCODE_NET_API_DECL int __netwin_setSockOpt(int s, int level, int optname, const void *optval,int optlen);
__OPENCODE_NET_API_DECL int __netwin_getSockOpt(int s, int level, int optname, void *optval,int *optlen);
#endif

class __OPENCODE_NET_API_DECL SocketUtils
{
//------------------------------------------------------------
// ctor and dector
//------------------------------------------------------------
protected:
  SocketUtils();

//------------------------------------------------------------
// static socket local bind address interface
//------------------------------------------------------------
public:
  static InetAddress  __OPENCODE_NATIVE_CODE socketGetLocalAddress(__NET_HOOK_DECLARE_MORE const SocketDescriptor& sd)  __NET_THROW_DECLARE(SocketException);
  static int  __OPENCODE_NATIVE_CODE socketGetLocalPort(__NET_HOOK_DECLARE_MORE const SocketDescriptor& sd)  __NET_THROW_DECLARE(SocketException);
  
//------------------------------------------------------------
// static socket interface
//------------------------------------------------------------
public:
  static void  __OPENCODE_NATIVE_CODE socketCreate(__NET_HOOK_DECLARE_MORE bool stream,SocketDescriptor* psd)  __NET_THROW_DECLARE(SocketException);

  static void  __OPENCODE_NATIVE_CODE socketConnect(__NET_HOOK_DECLARE_MORE const SocketDescriptor& sd,const InetAddress& address, int port) __NET_THROW_DECLARE(SocketException);
  static void  __OPENCODE_NATIVE_CODE socketConnect(__NET_HOOK_DECLARE_MORE const SocketDescriptor& sd,const InetAddress& address, int port, XLONG timeout) __NET_THROW_DECLARE(SocketException);
  
  static void  __OPENCODE_NATIVE_CODE socketBind(__NET_HOOK_DECLARE_MORE const SocketDescriptor& sd,const InetAddress& address, int port) __NET_THROW_DECLARE(SocketException);
  static void  __OPENCODE_NATIVE_CODE socketListen(__NET_HOOK_DECLARE_MORE const SocketDescriptor& sd,int count) __NET_THROW_DECLARE(SocketException);
  
  static void  __OPENCODE_NATIVE_CODE socketAccept(__NET_HOOK_DECLARE_MORE const SocketDescriptor& sd,SocketDescriptor* psd,SocketAddress* paddr) __NET_THROW_DECLARE(SocketException);
  static void  __OPENCODE_NATIVE_CODE socketAccept(__NET_HOOK_DECLARE_MORE const SocketDescriptor& sd,SocketDescriptor* psd,SocketAddress* paddr, XLONG timeout) __NET_THROW_DECLARE(SocketException);
     
  static void  __OPENCODE_NATIVE_CODE socketClose(const SocketDescriptor& sd) ;
  static void  __OPENCODE_NATIVE_CODE socketShutdown(const SocketDescriptor& sd,int howto) ;   

  static void  __OPENCODE_NATIVE_CODE socketSendUrgentData(__NET_HOOK_DECLARE_MORE const SocketDescriptor& sd,int data) __NET_THROW_DECLARE(SocketException);
  
  static int   __OPENCODE_NATIVE_CODE socketRead(__NET_HOOK_DECLARE_MORE const SocketDescriptor& sd,char* b,int length,unsigned  int flags) __NET_THROW_DECLARE(SocketException,ConnectionResetException);
  static int   __OPENCODE_NATIVE_CODE socketRead(__NET_HOOK_DECLARE_MORE const SocketDescriptor& sd,char* b,int length,unsigned  int flags,XLONG timeout) __NET_THROW_DECLARE(SocketException,ConnectionResetException);
  static void  __OPENCODE_NATIVE_CODE socketWrite(__NET_HOOK_DECLARE_MORE const SocketDescriptor& sd,const char* b,int length,unsigned  int flags) __NET_THROW_DECLARE(SocketException);

  static int   __OPENCODE_NATIVE_CODE socketRecvFrom(__NET_HOOK_DECLARE_MORE const SocketDescriptor& sd,SocketAddress* addr,char* buf, int len, unsigned int flags) __NET_THROW_DECLARE(SocketException,ConnectionResetException);
  static int   __OPENCODE_NATIVE_CODE socketRecvFrom(__NET_HOOK_DECLARE_MORE const SocketDescriptor& sd,SocketAddress* addr,char* buf, int len, unsigned int flags,XLONG timeout) __NET_THROW_DECLARE(SocketException,ConnectionResetException);
  static void  __OPENCODE_NATIVE_CODE socketSendTo(__NET_HOOK_DECLARE_MORE const SocketDescriptor& sd,const SocketAddress* addr,const char* msg, int len, unsigned  int flags) __NET_THROW_DECLARE(SocketException);
      
  static int  __OPENCODE_NATIVE_CODE socketTimeout(const SocketDescriptor& sd, XLONG timeout,int type);
   
//------------------------------------------------------------
// static socket  Options Support
//------------------------------------------------------------
public:
  static void  __OPENCODE_NATIVE_CODE socketSetOption(__NET_HOOK_DECLARE_MORE const SocketDescriptor& sd,int cmd, bool on, const SocketOptionsValue& value) __NET_THROW_DECLARE(SocketException);
  static SocketOptionsValue  __OPENCODE_NATIVE_CODE socketGetOption(__NET_HOOK_DECLARE_MORE const SocketDescriptor& sd,int opt) __NET_THROW_DECLARE(SocketException);
 
  static void  __OPENCODE_NATIVE_CODE socketSetTimeToLive(__NET_HOOK_DECLARE_MORE const SocketDescriptor& sd,int ttl) __NET_THROW_DECLARE(SocketException);
  static int   __OPENCODE_NATIVE_CODE socketGetTimeToLive(__NET_HOOK_DECLARE_MORE const SocketDescriptor& sd) __NET_THROW_DECLARE(SocketException);

  /*
   * Map the Socket level socket option to the platform specific
   * level and option name. 
   */
  static int  __OPENCODE_NATIVE_CODE socketMapOption(const SocketDescriptor& sd,int cmd,int *level, int *optname);

  /*
   * Sets the multicast interface. 
   *
   * SocketOptions.IP_MULTICAST_IF :-
   *  value is a InetAddress
   *  IPv4: set outgoing multicast interface using 
   *    IPPROTO_IP/IP_MULTICAST_IF
   *  IPv6: Get the index of the interface to which the
   *    InetAddress is bound
   *    Set outgoing multicast interface using
   *    IPPROTO_IPV6/IPV6_MULTICAST_IF
   *    On Linux 2.2 record interface index as can't
   *    query the multicast interface.
   *
   * SockOptions.IF_MULTICAST_IF2 :-
   *  value is a NetworkInterface 
   *  IPv4: Obtain IP address bound to network interface
   *    (NetworkInterface.addres[0])
   *    set outgoing multicast interface using 
   *              IPPROTO_IP/IP_MULTICAST_IF
   *  IPv6: Obtain NetworkInterface.index
   *    Set outgoing multicast interface using
   *              IPPROTO_IPV6/IPV6_MULTICAST_IF
   *              On Linux 2.2 record interface index as can't
   *              query the multicast interface. 
   *
   */
  static void  __OPENCODE_NATIVE_CODE setMulticastInterface(__NET_HOOK_DECLARE_MORE const SocketDescriptor& sd,int cmd,int level,int optname, const SocketOptionsValue& value) __NET_THROW_DECLARE(SocketException);


  /*
   * Return the multicast interface:
   *
   * SocketOptions.IP_MULTICAST_IF
   *  IPv4: Query IPPROTO_IP/IP_MULTICAST_IF
   *    Create InetAddress
   *    IP_MULTICAST_IF returns struct ip_mreqn on 2.2
   *    kernel but struct in_addr on 2.4 kernel
   *  IPv6: Query IPPROTO_IPV6 / IPV6_MULTICAST_IF or
   *    obtain from impl is Linux 2.2 kernel
   *    If index == 0 return InetAddress representing
   *    anyLocalAddress.
   *    If index > 0 query NetworkInterface by index
   *    and returns addrs[0]
   *
   * SocketOptions.IP_MULTICAST_IF2
   *  IPv4: Query IPPROTO_IP/IP_MULTICAST_IF
   *    Query NetworkInterface by IP address and
   *    return the NetworkInterface that the address
   *    is bound too.
   *  IPv6: Query IPPROTO_IPV6 / IPV6_MULTICAST_IF
   *    (except Linux .2 kernel)
   *    Query NetworkInterface by index and
   *    return NetworkInterface.  
   */

  static SocketOptionsValue  __OPENCODE_NATIVE_CODE getMulticastInterface(__NET_HOOK_DECLARE_MORE const SocketDescriptor& sd,int cmd,int level,int optname) __NET_THROW_DECLARE(SocketException);
  
//------------------------------------------------------------
// static data
//------------------------------------------------------------  
public:
  const static  int C_MSG_PEEK;

//------------------------------------------------------------
// private static data
//------------------------------------------------------------
private:
  static struct SocketOptionsMap  opts[];  
};

__OPENCODE_END_NAMESPACE

#endif

