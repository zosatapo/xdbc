/** 
 *  inetAdrress.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef __OPENCODE_INETADDRESS_H__
#define __OPENCODE_INETADDRESS_H__

#include "socket/socketdefs.h"
#include "socket/socketException.h"
#include "port/ioc_vector.h"

__OPENCODE_BEGIN_NAMESPACE

class __OPENCODE_NET_API_DECL InetAddress
{
public:
  friend class NetworkUtils;
public:
  ~InetAddress();
  InetAddress();

  //---------------------------------------------------
  // @param addr for  IPV4 & IPV6 , addr is in network byte order
  //---------------------------------------------------
  InetAddress(const unsigned char* addr,int addrlen);
  InetAddress(const unsigned char* addr,int addrlen,const string& hostname);

  InetAddress(const unsigned char* addr,int addrlen,int family);
  InetAddress(const unsigned char* addr,int addrlen,int family,const string& hostname);
  
  InetAddress(const InetAddress& rv); 
  InetAddress& operator=(const InetAddress& rv);


  bool operator==(const InetAddress& rv) const;

  bool equals(const InetAddress& rv)   const;
  string toString() const;
  

public:
  const string&   getHostName()  const;
  int             getFamily()    const;

  /**
   * Returns the raw IP address of this <code>InetAddress</code>
   * object. The result is in network byte order: the highest order
   * byte of the address is in <code>getAddress()[0]</code>.
   *
   * @return  the raw IP address of this object.
   */  
  const unsigned char*  getAddress() const; 
  int   getLength() const;
  
public:
  static InetAddress       getLocalHost(__NET_HOOK_DECLARE_ONCE) __NET_THROW_DECLARE(UnknownHostException);
  
  static InetAddress        getByName(__NET_HOOK_DECLARE_MORE const string& host) __NET_THROW_DECLARE(UnknownHostException);
  static void               getAllByName(__NET_HOOK_DECLARE_MORE vector<InetAddress>& addresses,const string& host) __NET_THROW_DECLARE(UnknownHostException);

  static string  getDotAddress(const InetAddress& address);

  static const  InetAddress& nullAddress(); 
  static const  InetAddress& anyAddress();
  
  static bool   isNullAddress(const InetAddress& address);
  static bool   isAnyAddress(const InetAddress& address);
  static bool   isLoopbackAddress(const InetAddress& address);
  static bool   isMulticastAddress(const InetAddress& address);

private:
  string  str_hostname;
  int     int_family;
  
  // in network byte order
  unsigned char*  m_address;
  int    int_addrlen;
public:
#ifdef AF_INET6
  UXINT32  sin6_flowinfo;
  UXINT32  sin6_scope_id;
  static InetAddress AnyAddress_IPV6;
  static InetAddress LoopbackAddress_IPV6;  
#endif

  static InetAddress NullAddress;
  static InetAddress AnyAddress_IPV4;
  static InetAddress LoopbackAddress_IPV4;
};

__OPENCODE_END_NAMESPACE

#endif
