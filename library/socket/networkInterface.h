/** 
 *  networkInterface.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef __OPENCODE_NETWORKINTERFACE_H__
#define __OPENCODE_NETWORKINTERFACE_H__

#include "socket/socketdefs.h"

#include "socket/inetAddress.h"
#include "socket/socketException.h"
#include "port/ioc_vector.h"

__OPENCODE_BEGIN_NAMESPACE


/**
 * This class represents a Network Interface made up of a name, 
 * and a list of IP addresses assigned to this interface.
 * It is used to identify the local interface on which a multicast group
 * is joined. 
 *
 * Interfaces are normally known by names such as "le0".
 *
 */
class __OPENCODE_NET_API_DECL NetworkInterface 
{
//------------------------------------------------------------------------
//
//------------------------------------------------------------------------
public:
  /**
   * Returns an NetworkInterface object with index set to 0 and name to null.
   * Setting such an interface on a MulticastSocket will cause the 
   * kernel to choose one interface for sending multicast packets.
   *  
   */
  NetworkInterface(); 
  
  NetworkInterface(const string& name, int index, const vector<unsigned char>& ifaddr,const vector<InetAddress>& addrs); 
  NetworkInterface(const string& name, int index, const vector<unsigned char>& ifaddr,const vector<InetAddress>& addrs,const string& displayName);

  NetworkInterface(const NetworkInterface& rv);
  NetworkInterface& operator=(const NetworkInterface& rv);

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------
public:  
  /**
   * Get the name of this network interface.
   *
   * @return the name of this network interface
   */
  const string& getName() const ;
  
  /**
   * Get the index of this network interface.
   *
   * @return the index of this network interface
   */
  int getIndex() const;
  
  /**
   * Get the display name of this network interface.
   * A display name is a human readable const string& describing the network
   * device.
   *
   * @return the display name of this network interface, 
   *         or null if no display name is available.
   */
  const string& getDisplayName() const;

  /**
   * Get Ethernet MAC address of this network interface.
   */  
  const vector<unsigned char>& getInterfaceAddress() const;
  
  /**
   * Convenience method to return an Enumeration with all or a
   * subset of the InetAddresses bound to this network interface.
   * <p>
   * If there is a security manager, its <code>checkConnect</code>
   * method is called for each InetAddress. Only InetAddresses where
   * the <code>checkConnect</code> doesn't throw a SecurityException
   * will be returned in the Enumeration.
   * @return an Enumeration object with all or a subset of the InetAddresses
   * bound to this network interface
   */
  const vector<InetAddress>& getInetAddresses() const;

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------ 
public:  
  /**
   * Searches for the network interface with the specified name.
   *
   * @param   name 
   *    The name of the network interface.
   *
   * @return  A <tt>NetworkInterface</tt> with the specified name,
   *          or <tt>null</tt> if there is no network interface
   *    with the specified name.
   *
   * @throws  SocketException  
   *          If an I/O error occurs.
   */
  static NetworkInterface getByName(__NET_HOOK_DECLARE_MORE const string& name)  __NET_THROW_DECLARE(SocketException);
  
  /**
   * Get a network interface given its index.
   *
   * @param index an integer, the index of the interface
   * @return the NetworkInterface obtained from its index
   * @exception  SocketException  if an I/O error occurs.
   */
  static NetworkInterface getByIndex(__NET_HOOK_DECLARE_MORE int index)  __NET_THROW_DECLARE(SocketException);
  
  /**
   * Convenience method to search for a network interface that
   * has the specified Internet Protocol (IP) address bound to
   * it.
   * <p>
   * If the specified IP address is bound to multiple network 
   * interfaces it is not defined which network interface is
   * returned.
   *
   * @param   addr
   *    The <tt>InetAddress</tt> to search with.
   *
   * @return  A <tt>NetworkInterface</tt> 
   *          or <tt>null</tt> if there is no network interface
   *          with the specified IP address.
   *
   * @throws  SocketException  
   *          If an I/O error occurs. 
   *
   */
  static NetworkInterface getByInetAddress(__NET_HOOK_DECLARE_MORE const InetAddress& addr)  __NET_THROW_DECLARE(SocketException);
  
  /**
   * Returns all the interfaces on this machine. Returns null if no
   * network interfaces could be found on this machine.
   * 
   * NOTE: can use getNetworkInterfaces()+getInetAddresses() 
   *       to obtain all IP addresses for this node
   *
   * @return an Enumeration of NetworkInterfaces found on this machine
   * @exception  SocketException  if an I/O error occurs.
   */
  
  static void getNetworkInterfaces(__NET_HOOK_DECLARE_MORE vector<NetworkInterface>& ifs)  __NET_THROW_DECLARE(SocketException);

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------ 
private:
  string name;
  string displayName;
  int index;
  vector<unsigned char> ifaddr;

  vector<InetAddress> addrs;
};
__OPENCODE_END_NAMESPACE

#endif

