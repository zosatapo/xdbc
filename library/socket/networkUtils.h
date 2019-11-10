/** 
 *  networkUtils.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef __OPENCODE_NETWORKUTILS_H__
#define __OPENCODE_NETWORKUTILS_H__

#include "socket/socketdefs.h"
#include "socket/socketException.h"
#include "socket/inetAddress.h"
#include "socket/socketAddress.h"
#include "socket/networkInterface.h"
#include "socket/socketOptions.h"
#include "socket/socketDescriptor.h" 
#include "port/lifetime.h"

__OPENCODE_BEGIN_NAMESPACE

//======================================================================================
//
//======================================================================================

class __OPENCODE_NET_API_DECL NetworkDoImpl
{
  DECLARE_LIFETIME_BEGIN(NetworkDoImpl)
  DECLARE_LIFETIME_END(NetworkDoImpl)
  
private:
  NetworkDoImpl(const NetworkDoImpl& rv);
  NetworkDoImpl& operator=(const NetworkDoImpl& rv);

public:
  ~NetworkDoImpl() ;
  NetworkDoImpl();

public:
  bool  __OPENCODE_NATIVE_CODE IPv6_supported();
  bool  __OPENCODE_NATIVE_CODE IPv6_available();
  bool  __OPENCODE_NATIVE_CODE isRcvTimeoutSupported();
  void  __OPENCODE_NATIVE_CODE setRcvTimeoutSupported(bool isRcvTimeoutSupported);

private:
  bool __mb_IPv6_supported;
  bool __mb_IPv6_available;
  bool __mb_isRcvTimeoutSupported;
};

typedef SingletonHolder<NetworkDoImpl,CreateUsingNew<NetworkDoImpl>,SingletonWithLongevity<NetworkDoImpl>,MultiThread<NetworkDoImpl> > NetworkUtilsImpl;
__OPENCODE_INLINE unsigned int getLongevity(NetworkDoImpl* impl){return __SOCKET_LONGEVITY_NETWORK;}

//======================================================================================
//
//======================================================================================

class __OPENCODE_NET_API_DECL NetworkUtils
{
public:
  typedef struct _netaddr  
  {
    struct sockaddr* addr;
    int family; /* to make searches simple */    
    struct _netaddr* next;
  } netaddr;

  typedef struct _netif 
  {
    char* name;
    char* displayName;
    int   dwIndex;		/* Internal index for windows*/ 
    int   index;       /* Friendly index */
    unsigned char ifaddr[14];    
    netaddr* addr;   
    struct _netif* next;
  } netif;

//------------------------------------------------------------
// ctor and dector
//------------------------------------------------------------
protected:
  ~NetworkUtils() ;
  NetworkUtils();

//------------------------------------------------------------
// static Utils
//------------------------------------------------------------
public:
  static bool  __OPENCODE_NATIVE_CODE IPv6_supported();
  static bool  __OPENCODE_NATIVE_CODE IPv6_available();
  static bool  __OPENCODE_NATIVE_CODE isRcvTimeoutSupported();
  static void  __OPENCODE_NATIVE_CODE setRcvTimeoutSupported(bool isRcvTimeoutSupported);
  
  static int   __OPENCODE_NATIVE_CODE inet_aton(const char *host_name, struct in_addr *addr);

//------------------------------------------------------------
// static byte order Utils
//------------------------------------------------------------
public:
  static UXINT64   __OPENCODE_NATIVE_CODE net_htonq(UXINT64 hostq);
  static UXINT32   __OPENCODE_NATIVE_CODE net_htonl(UXINT32  hostlong);
  static UXINT16   __OPENCODE_NATIVE_CODE net_htons(UXINT16 hostshort);

  static UXINT64   __OPENCODE_NATIVE_CODE net_ntohq(UXINT64 netq);
  static UXINT32   __OPENCODE_NATIVE_CODE net_ntohl(UXINT32 netlong);
  static UXINT16   __OPENCODE_NATIVE_CODE net_ntohs(UXINT16 netshort);

//------------------------------------------------------------
// static InetAddress interface
//------------------------------------------------------------
public:
  static string               __OPENCODE_NATIVE_CODE  inetGetLocalHostName();
  
  static void                 __OPENCODE_NATIVE_CODE  inetGetAllByName(__NET_HOOK_DECLARE_MORE vector<InetAddress>& addresses,const string& host,int maxcount = -1) __NET_THROW_DECLARE(UnknownHostException);
  static InetAddress          __OPENCODE_NATIVE_CODE  inetGetLocalHost(__NET_HOOK_DECLARE_ONCE) __NET_THROW_DECLARE(UnknownHostException);
  static InetAddress          __OPENCODE_NATIVE_CODE  inetGetByName(__NET_HOOK_DECLARE_MORE const string& host) __NET_THROW_DECLARE(UnknownHostException);
  static string               __OPENCODE_NATIVE_CODE  inetGetDotAddress(const InetAddress& address);

//------------------------------------------------------------
// static NetworkInterface interface
//------------------------------------------------------------
public:
  static void __OPENCODE_NATIVE_CODE netifGetAll(__NET_HOOK_DECLARE_MORE vector<NetworkInterface>& ifs)  __NET_THROW_DECLARE(SocketException);
  static NetworkInterface __OPENCODE_NATIVE_CODE  netifGetByIndex(__NET_HOOK_DECLARE_MORE int index)  __NET_THROW_DECLARE(SocketException);
  static NetworkInterface __OPENCODE_NATIVE_CODE  netifGetByName(__NET_HOOK_DECLARE_MORE const string& name)  __NET_THROW_DECLARE(SocketException);
  static NetworkInterface __OPENCODE_NATIVE_CODE  netifGetByInetAddress(__NET_HOOK_DECLARE_MORE const InetAddress& addr)  __NET_THROW_DECLARE(SocketException);

//------------------------------------------------------------
// static multicastSocket interface
//------------------------------------------------------------
public:
  static void __OPENCODE_NATIVE_CODE mcastJoinGroup(__NET_HOOK_DECLARE_MORE const SocketDescriptor& sd,const InetAddress& mcastaddr, const NetworkInterface* netIf)  __NET_THROW_DECLARE(SocketException) ;
  static void __OPENCODE_NATIVE_CODE mcastLeaveGroup(__NET_HOOK_DECLARE_MORE const SocketDescriptor& sd,const InetAddress& mcastaddr, const NetworkInterface* netIf)  __NET_THROW_DECLARE(SocketException) ;

//------------------------------------------------------------
// static address format interface
//------------------------------------------------------------
public:
  static InetAddress   __OPENCODE_NATIVE_CODE  makeInetAddress(struct in_addr& addr);
  static string        __OPENCODE_NATIVE_CODE  inetGetDotAddress(struct in_addr& addr);

#ifdef AF_INET6
  static InetAddress   __OPENCODE_NATIVE_CODE  makeInetAddress(struct in6_addr& addr);
  static string        __OPENCODE_NATIVE_CODE  inetGetDotAddress(struct in6_addr& addr);
  static void          __OPENCODE_NATIVE_CODE  make_sockaddr_in6(struct sockaddr_in6* him,const InetAddress& addr,int port);
#endif
  
  static InetAddress   __OPENCODE_NATIVE_CODE  makeInetAddress(struct sockaddr* addr,int family);  
  static SocketAddress __OPENCODE_NATIVE_CODE  makeSocketAddress(struct sockaddr* addr,int family);

  static void          __OPENCODE_NATIVE_CODE make_sockaddr(struct sockaddr* saddr,const InetAddress& addr,int port);
  static void          __OPENCODE_NATIVE_CODE make_sockaddr(struct sockaddr* saddr,const SocketAddress& addr);

//------------------------------------------------------------
// static Utils
//------------------------------------------------------------
public:
  /*
   * mcast_join_leave: Join or leave a multicast group.
   *
   * For IPv4 sockets use IP_ADD_MEMBERSHIP/IP_DROP_MEMBERSHIP socket option
   * to join/leave multicast group.
   *
   * For IPv6 sockets use IPV6_ADD_MEMBERSHIP/IPV6_DROP_MEMBERSHIP socket option
   * to join/leave multicast group. If multicast group is an IPv4 address then
   * an IPv4-mapped address is used.
   *
   * On Linux with IPv6 if we wish to join/leave an IPv4 multicast group then
   * we must use the IPv4 socket options. This is because the IPv6 socket options
   * don't support IPv4-mapped addresses. This is true as per 2.2.19 and 2.4.7
   * kernel releases. In the future it's possible that IP_ADD_MEMBERSHIP 
   * will be updated to return ENOPROTOOPT if uses with an IPv6 socket (Solaris
   * already does this). Thus to cater for this we first try with the IPv4
   * socket options and if they fail we use the IPv6 socket options. This
   * seems a reasonable failsafe solution.
   */
  static void __OPENCODE_NATIVE_CODE mcast_join_leave(__NET_HOOK_DECLARE_MORE const SocketDescriptor& sd,const InetAddress& mcastaddr, const NetworkInterface* netIf,bool join) __NET_THROW_DECLARE(SocketException) ;

  /* 
   * Enumerates all interfaces
   */
  static netif*  __OPENCODE_NATIVE_CODE  enumInterfaces(__NET_HOOK_DECLARE_ONCE)  __NET_THROW_DECLARE(SocketException) ;

  /*
   * Enumerates and returns all IPv4 interfaces
   */  
  static netif*  __OPENCODE_NATIVE_CODE  enumIPv4Interfaces(__NET_HOOK_DECLARE_MORE netif *ifs)  __NET_THROW_DECLARE(SocketException) ;

#ifdef AF_INET6
  /*
   * Enumerates and returns all IPv6 interfaces
   */  
  static netif*  __OPENCODE_NATIVE_CODE  enumIPv6Interfaces(__NET_HOOK_DECLARE_MORE netif *ifs)  __NET_THROW_DECLARE(SocketException) ;
#endif
    
  /*
   * Add an interface to the list. If known interface just link
   * a new netaddr onto the list. If new interface create new
   * netif structure.
   */
  static netif* __OPENCODE_NATIVE_CODE addif(netif *ifs, char *if_name,char* if_displayame,int if_index,int if_dwIndex,unsigned char* ifaddr,int ifaddrlen,int family,struct sockaddr *new_addrP, int new_addrlen);   
  /*
   * Free an interface list (including any attached addresses)
   */
  static void __OPENCODE_NATIVE_CODE freeif(netif *ifs);


  /*
   * Create a NetworkInterface object, populate the name and index, and
   * populate the InetAddress array based on the IP addresses for this
   * interface.
   */
  static NetworkInterface __OPENCODE_NATIVE_CODE createNetworkInterface(netif *ifs);
};

#  if defined(OPENCODE_WORDS_BIGENDIAN)
#    define NETWORK_HTONQ(hostq)     (UXINT64)(hostq)
#    define NETWORK_HTONL(hostl)     (UXINT32)(hostl)
#    define NETWORK_HTONS(hosts)     (UXINT16)(hosts)
#    define NETWORK_NTOHQ(netq)      (UXINT64)(netq)
#    define NETWORK_NTOHL(netl)      (UXINT32)(netl)
#    define NETWORK_NTOHS(nets)      (UXINT16)(nets)
#  else
#    define NETWORK_HTONQ(hostq)      NetworkUtils::net_htonq(hostq)
#    define NETWORK_HTONL(hostl)      NetworkUtils::net_htonl(hostl)
#    define NETWORK_HTONS(hosts)      NetworkUtils::net_htons(hosts)
#    define NETWORK_NTOHQ(netq)       NetworkUtils::net_ntohq(netq)
#    define NETWORK_NTOHL(netl)       NetworkUtils::net_ntohl(netl)
#    define NETWORK_NTOHS(nets)       NetworkUtils::net_ntohs(nets)
#endif

__OPENCODE_END_NAMESPACE

#endif

