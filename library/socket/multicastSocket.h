/** 
 *  multicastSocket.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef __OPENCODE_MULTICASTSOCKET_H__
#define __OPENCODE_MULTICASTSOCKET_H__

#include "socket/socketdefs.h"
#include "socket/socketDescriptor.h"
#include "socket/inetAddress.h"
#include "socket/socketException.h"
#include "socket/socketDescriptor.h"
#include "socket/socketAddress.h"
#include "socket/networkInterface.h"
#include "socket/datagramSocket.h"

__OPENCODE_BEGIN_NAMESPACE

/**
 * The multicast datagram socket class is useful for sending
 * and receiving IP multicast packets.  A MulticastSocket is
 * a (UDP) DatagramSocket, with additional capabilities for
 * joining "groups" of rv multicast hosts on the internet.
 * <P>
 * A multicast group is specified by a class D IP address
 * and by a standard UDP port number. Class D IP addresses
 * are in the range <CODE>224.0.0.0</CODE> to <CODE>239.255.255.255</CODE>,
 * inclusive. The address 224.0.0.0 is reserved and should not be used.
 * <P>
 * One would join a multicast group by first creating a MulticastSocket
 * with the desired port, then invoking the
 * <CODE>joinGroup(InetAddress groupAddr)</CODE>
 * method:
 * <PRE>
 * // join a Multicast group and send the group salutations
 * ...
 * String msg = "Hello";
 * InetAddress group = InetAddress.getByName("228.5.6.7");
 * MulticastSocket s(6789);
 * s.joinGroup(group);
 *                            
 * s.send(xxx);
 *
 * // get their responses!
 * s.receive(xxx);
 * ...
 * // OK, I'm done talking - leave the group...
 * s.leaveGroup(group);
 * </PRE>
 *
 * When one sends a message to a multicast group, <B>all</B> subscribing
 * recipients to that host and port receive the message (within the
 * time-to-live range of the packet, see below).  The socket needn't
 * be a member of the multicast group to send messages to it.
 * <P>
 * When a socket subscribes to a multicast group/port, it receives
 * datagrams sent by rv hosts to the group/port, as do all rv
 * members of the group and port.  A socket relinquishes membership
 * in a group by the leaveGroup(InetAddress addr) method.  <B>
 * Multiple MulticastSocket's</B> may subscribe to a multicast group
 * and port concurrently, and they will all receive group datagrams.
 *
 */

class __OPENCODE_NET_API_DECL MulticastSocket : public DatagramSocket 
{
//------------------------------------------------------------------------
//
//------------------------------------------------------------------------ 
public:
  virtual ~MulticastSocket() ;

  /**
   * Create a multicast socket.
   * 
   */
  MulticastSocket();
  
  /**
   * Create a multicast socket and bind it to a specific port.
   * 
   * When the socket is created the
   * {@link DatagramSocket#setReuseAddress(true)} method is
   * called to enable the SO_REUSEADDR socket option. 
   * 
   * @param port port to use
   * @exception SocketException if an I/O exception occurs
   * while creating the MulticastSocket
   */
  MulticastSocket(__NET_HOOK_DECLARE_MORE int port)  __NET_THROW_DECLARE(SocketException);
  
  /**
   * Create a MulticastSocket bound to the specified socket address.
   * <p>
   * Or, if the address is <code>null</code>, create an unbound socket.
   * <p>
   * When the socket is created the
   * {@link DatagramSocket#setReuseAddress(true)} method is
   * called to enable the SO_REUSEADDR socket option. 
   *
   * @param bindaddr Socket address to bind to, or <code>null</code> for
   *                 an unbound socket.
   * @exception SocketException if an I/O exception occurs
   * while creating the MulticastSocket
   */
  MulticastSocket(__NET_HOOK_DECLARE_MORE const SocketAddress& bindaddr)  __NET_THROW_DECLARE(SocketException);

  MulticastSocket(const MulticastSocket& rv);
  MulticastSocket& operator=(const MulticastSocket& rv);

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------ 
public:
  /**
   * Set the default time-to-live for multicast packets sent out
   * on this <code>MulticastSocket</code> in order to control the 
   * scope of the multicasts.
   *
   * <P> The ttl <B>must</B> be in the range <code> 0 <= ttl <=
   * 255</code> .
   * @exception IOException if an I/O exception occurs
   * while setting the default time-to-live value
   * @param ttl the time-to-live
   */
  virtual void setTimeToLive(__NET_HOOK_DECLARE_MORE int ttl)  __NET_THROW_DECLARE(SocketException);
 
   /**
   * Get the default time-to-live for multicast packets sent out on
   * the socket.
   * @exception IOException if an I/O exception occurs while
   * getting the default time-to-live value
   * @return the default time-to-live value
   */
  virtual int getTimeToLive(__NET_HOOK_DECLARE_ONCE)  __NET_THROW_DECLARE(SocketException);
     
  /**
   * Joins a multicast group. Its behavior may be affected by
   * <code>setInterface</code> or <code>setNetworkInterface</code>.
   * 
   * 
   * @param mcastaddr is the multicast address to join
   * 
   * @exception SocketException if there is an error joining
   * or when the address is not a multicast address.
   */
  virtual void joinGroup(__NET_HOOK_DECLARE_MORE const InetAddress& mcastaddr)  __NET_THROW_DECLARE(SocketException);
  
  /**
   * Leave a multicast group. Its behavior may be affected by
   * <code>setInterface</code> or <code>setNetworkInterface</code>.
   * 
   * @param mcastaddr is the multicast address to leave
   * @exception SocketException if there is an error leaving
   * or when the address is not a multicast address.
   */
  virtual void leaveGroup(__NET_HOOK_DECLARE_MORE const InetAddress& mcastaddr)  __NET_THROW_DECLARE(SocketException); 
  
  /**
   * Joins the specified multicast group at the specified interface.
   * 
   * @param mcastaddr is the multicast address to join
   * @param netIf specifies the local interface to receive multicast
   *        datagram packets, or <i>null</i> to defer to the interface set by
   *       {@link MulticastSocket#setInterface(InetAddress)} or 
   *       {@link MulticastSocket#setNetworkInterface(NetworkInterface)}
   *
   * @exception SocketException if there is an error joining
   * or when the address is not a multicast address.
   */
  virtual void joinGroup(__NET_HOOK_DECLARE_MORE const SocketAddress& mcastaddr,const NetworkInterface& netIf) __NET_THROW_DECLARE(SocketException);
  
  /**
   * Leave a multicast group on a specified local interface.
   * 
   * 
   * @param mcastaddr is the multicast address to leave
   * @param netIf specifies the local interface or <i>null</i> to defer
   *       to the interface set by
   *       {@link MulticastSocket#setInterface(InetAddress)} or 
   *       {@link MulticastSocket#setNetworkInterface(NetworkInterface)}
   * @exception SocketException if there is an error leaving
   * or when the address is not a multicast address.
   */
  virtual void leaveGroup(__NET_HOOK_DECLARE_MORE const SocketAddress& mcastaddr, const NetworkInterface& netIf) __NET_THROW_DECLARE(SocketException);
  
  /**
   * Set the multicast network interface used by methods
   * whose behavior would be affected by the value of the
   * network interface. Useful for multihomed hosts.
   * @param inf the InetAddress
   * @exception SocketException if there is an error in 
   * the underlying protocol, such as a TCP error. 
   */
  virtual void setInterface(__NET_HOOK_DECLARE_MORE const InetAddress& inf)  __NET_THROW_DECLARE(SocketException);
  
  /**
   * Retrieve the address of the network interface used for
   * multicast packets.
   * 
   * @return An <code>InetAddress</code> representing
   *  the address of the network interface used for 
   *  multicast packets.
   *
   * @exception SocketException if there is an error in 
   * the underlying protocol, such as a TCP error.
   * 
   */
  virtual InetAddress getInterface(__NET_HOOK_DECLARE_ONCE)  __NET_THROW_DECLARE(SocketException);
  
  /**
   * Specify the network interface for outgoing multicast datagrams 
   * sent on this socket.
   *
   * @param netIf the interface
   * @exception SocketException if there is an error in 
   * the underlying protocol, such as a TCP error. 
   */
  virtual void setNetworkInterface(__NET_HOOK_DECLARE_MORE const NetworkInterface& netIf) __NET_THROW_DECLARE(SocketException);
  
  /**
   * Get the multicast network interface set.
   *
   * @exception SocketException if there is an error in 
   * the underlying protocol, such as a TCP error. 
   * @return the multicast <code>NetworkInterface</code> currently set
   */
  virtual NetworkInterface getNetworkInterface(__NET_HOOK_DECLARE_ONCE)  __NET_THROW_DECLARE(SocketException);
  
  /**
   * Disable/Enable local loopback of multicast datagrams
   * The option is used by the platform's networking code as a hint 
   * for setting whether multicast data will be looped back to 
   * the local socket.
   *
   * <p>Because this option is a hint, applications that want to
   * verify what loopback mode is set to should call 
   * {@link #getLoopbackMode()}
   * @param disable <code>true</code> to disable the LoopbackMode
   * @throw SocketException if an error occurs while setting the value
   */
  virtual void setLoopbackMode(__NET_HOOK_DECLARE_MORE bool disable)  __NET_THROW_DECLARE(SocketException);
  
  /**
   * Get the setting for local loopback of multicast datagrams.
   *
   * @throw SocketException if an error occurs while getting the value
   * @return true if the LoopbackMode has been disabled
   */
  virtual bool getLoopbackMode(__NET_HOOK_DECLARE_ONCE)  __NET_THROW_DECLARE(SocketException);
  
//------------------------------------------------------------------------
//
//------------------------------------------------------------------------ 
private:
  /**
   * The "last" interface set by setInterface on this MulticastSocket
   */
  InetAddress infAddress ;

};


__OPENCODE_END_NAMESPACE

#endif
