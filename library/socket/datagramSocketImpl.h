/** 
 *  datagramSocketImpl.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef __OPENCODE_DATAGRAMSOCKETIMPL_H__
#define __OPENCODE_DATAGRAMSOCKETIMPL_H__

#include "socket/socketdefs.h"

#include "socket/inetAddress.h"
#include "socket/socketException.h"
#include "socket/socketDescriptor.h"
#include "socket/socketAddress.h"
#include "socket/socketOptions.h"

__OPENCODE_BEGIN_NAMESPACE


/**
 * Abstract datagram and multicast socket implementation base class.
 */

class __OPENCODE_NET_API_DECL DatagramSocketImpl : public SocketOptions 
{
//------------------------------------------------------------------------
//
//------------------------------------------------------------------------ 
protected:
  DatagramSocketImpl();

  DatagramSocketImpl(const DatagramSocketImpl& rv);
  DatagramSocketImpl& operator=(const DatagramSocketImpl& rv);
  
  virtual DatagramSocketImpl* doClone() const = 0;

public:   
  virtual ~DatagramSocketImpl();
  DatagramSocketImpl* clone() const;

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------ 
public: 
  /**
   * Creates a datagram socket.
   * @exception SocketException if there is an error in the 
   * underlying protocol, such as a TCP error. 
   */
  virtual void create(__NET_HOOK_DECLARE_ONCE)  __NET_THROW_DECLARE(SocketException) = 0;
  
  /**
   * Binds a datagram socket to a local port and address.
   * @param laddr the local address
   * @param lport the local port
   * @exception SocketException if there is an error in the
   * underlying protocol, such as a TCP error.
   */
  virtual void bind(__NET_HOOK_DECLARE_MORE const InetAddress& laddr,int lport)  __NET_THROW_DECLARE(SocketException) = 0;
  
  /**
   * Connects a datagram socket to a remote destination. This associates the remote
   * address with the local socket so that datagrams may only be sent to this destination
   * and received from this destination. This may be overridden to call a native
   * system connect. 
   *
   * <p>If the remote destination to which the socket is connected does not
   * exist, or is otherwise unreachable, and if an ICMP destination unreachable
   * packet has been received for that address, then a subsequent call to 
   * send or receive may throw a PortUnreachableException. 
   * Note, there is no guarantee that the exception will be thrown.
   * @param address the remote InetAddress to connect to
   * @param port the remote port number
   * @exception   SocketException may be thrown if the socket cannot be
   * connected to the remote destination
   */
  virtual void connect(__NET_HOOK_DECLARE_MORE const InetAddress& address, int port)  __NET_THROW_DECLARE(SocketException) {}   
  
  /**
   * Close the socket.
   */
  virtual void close() = 0;
    
//------------------------------------------------------------------------
//
//------------------------------------------------------------------------ 
public:
  /**
   * Receive the datagram packet.
   *
   * @param   address  the destination socket address.
   * @param b the buffer into which the data is read
   * @param length the maximum number of bytes read
   * @exception SocketException if an I/O exception occurs
   * while receiving the datagram packet.
   */
  virtual int receive(__NET_HOOK_DECLARE_MORE SocketAddress* address,char* b, int length)  __NET_THROW_DECLARE(SocketException) = 0;

  /**
   * Receive the datagram packet.
   *
   * @param   address  the destination socket address.
   * @param b the buffer into which the data is read
   * @param length the maximum number of bytes read
   * @exception SocketException if an I/O exception occurs
   * while receiving the datagram packet.
   */
  virtual int peek(__NET_HOOK_DECLARE_MORE SocketAddress* address,char* b, int length)  __NET_THROW_DECLARE(SocketException) = 0;
  
    
  /**
   * Sends a datagram packet. The packet contains the data and the
   * destination address to send the packet to.
   *
   * @param   address  the destination socket address.
   * @param b the data to be written
   * @param length the number of bytes that are written
   * @exception SocketException if an I/O exception occurs while sending the 
   * datagram packet.
   */
  virtual void send(__NET_HOOK_DECLARE_MORE const SocketAddress* address,const char* b, int length)  __NET_THROW_DECLARE(SocketException) = 0;   
    
//------------------------------------------------------------------------
//
//------------------------------------------------------------------------ 
public:
  /**
   * Set the TTL (time-to-live) option.
   * @param ttl an <tt>int</tt> specifying the time-to-live value
   * @exception SocketException if an I/O exception occurs
   * while setting the time-to-live option.
   */
  virtual void setTimeToLive(__NET_HOOK_DECLARE_MORE int ttl)  __NET_THROW_DECLARE(SocketException) = 0;
  
  /**
   * Retrieve the TTL (time-to-live) option.
   * @exception SocketException if an I/O exception occurs
   * while retrieving the time-to-live option
   * @return an <tt>int</tt> representing the time-to-live value
   */
  virtual int getTimeToLive(__NET_HOOK_DECLARE_ONCE)  __NET_THROW_DECLARE(SocketException) = 0;

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------ 
public:
  /**
   * Join the multicast group.
   * @param inetaddr multicast address to join.
   * @exception SocketException if an I/O exception occurs
   * while joining the multicast group.
   */
  virtual void join(__NET_HOOK_DECLARE_MORE const InetAddress& inetaddr)  __NET_THROW_DECLARE(SocketException) = 0;
  
  /**
   * Leave the multicast group.
   * @param inetaddr multicast address to leave.
   * @exception SocketException if an I/O exception occurs
   * while leaving the multicast group.
   */
  virtual void leave(__NET_HOOK_DECLARE_MORE const InetAddress& inetaddr)  __NET_THROW_DECLARE(SocketException) = 0;
  
  /**
   * Join the multicast group.
   * @param mcastaddr address to join.
   * @param netIf specifies the local interface to receive multicast
   *        datagram packets
   * @throw SocketException if an I/O exception occurs while joining
   * the multicast group
   */
  virtual void joinGroup(__NET_HOOK_DECLARE_MORE const SocketAddress& mcastaddr, const NetworkInterface& netIf)  __NET_THROW_DECLARE(SocketException) = 0;
  
  /**
   * Leave the multicast group.
   * @param mcastaddr address to leave.
   * @param netIf specified the local interface to leave the group at
   * @throw SocketException if an I/O exception occurs while leaving
   * the multicast group
   */
  virtual void leaveGroup(__NET_HOOK_DECLARE_MORE const SocketAddress& mcastaddr, const NetworkInterface& netIf)  __NET_THROW_DECLARE(SocketException) = 0;

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------ 
public:
  /**
   * Gets the local port.
   * @return an <tt>int</tt> representing the local port value
   */
  int getLocalPort() const;
  
  /**
   * Gets the datagram socket file descriptor.
   * @return a <tt>SocketDescriptor</tt> object representing the datagram socket
   * file descriptor
   */
  const SocketDescriptor& getSocketDescriptor() const;

protected:

  /**
   * The local port number.
   */   
  int localPort;
  
  /**
   * The file descriptor object.
   */
  SocketDescriptor fd;
};

__OPENCODE_END_NAMESPACE

#endif
