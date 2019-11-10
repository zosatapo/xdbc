/**
 *  datagramSocket.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */

#ifndef __OPENCODE_DATAGRAMSOCKET_H__
#define __OPENCODE_DATAGRAMSOCKET_H__

#include "socket/socketdefs.h"
#include "socket/socketDescriptor.h"
#include "socket/inetAddress.h"
#include "socket/socketAddress.h"
#include "socket/socketException.h"
#include "socket/socketDescriptor.h"

__OPENCODE_BEGIN_NAMESPACE

class DatagramSocketImpl;

class __OPENCODE_NET_API_DECL DatagramSocket
{
public:
  virtual ~DatagramSocket();

public:

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------  
  /**
   * Constructs a datagram socket and binds it to any available port
   * on the local host machine.  The socket will be bound to the wildcard
   * address, an IP address chosen by the kernel.
   *
   */
  DatagramSocket();
  
  /**
   * Creates a datagram socket, bound to the specified local
   * socket address.
   * 
   * @param bindaddr local socket address to bind
   * 
   * @exception  SocketException  if the socket could not be opened,
   *               or the socket could not bind to the specified local port.
   */
  DatagramSocket(__NET_HOOK_DECLARE_MORE const SocketAddress& bindaddr)  __NET_THROW_DECLARE(SocketException);
  
  /**
   * Constructs a datagram socket and binds it to the specified port
   * on the local host machine.  The socket will be bound to the wildcard
   * address, an IP address chosen by the kernel.
   *
   * @param      port port to use.
   * @exception  SocketException  if the socket could not be opened,
   *               or the socket could not bind to the specified local port.
   */
  DatagramSocket(__NET_HOOK_DECLARE_MORE int port)  __NET_THROW_DECLARE(SocketException);
  
  /**
   * Creates a datagram socket, bound to the specified local
   * address.  The local port must be between 0 and 65535 inclusive.
   * If the IP address is 0.0.0.0, the socket will be bound to the
   * wildcard address, an IP address chosen by the kernel.
   * 
   * @param port local port to use
   * @param laddr local address to bind
   * 
   * @exception  SocketException  if the socket could not be opened,
   *               or the socket could not bind to the specified local port.
   */
  DatagramSocket(__NET_HOOK_DECLARE_MORE int port,const InetAddress& laddr)  __NET_THROW_DECLARE(SocketException);

//------------------------------------------------------------------------
//Copy Ctor
//------------------------------------------------------------------------
public:
  DatagramSocket(const DatagramSocket& rhs);
  DatagramSocket& operator= (const DatagramSocket& rhs);

//------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------
public: 
  /**
   * Binds this DatagramSocket to a specific address & port.
   * <p>
   * If the address is <code>null</code>, then the system will pick up
   * an ephemeral port and a valid local address to bind the socket.
   *<p>
   * @param  addr The address & port to bind to.
   * @throws  SocketException if any error happens during the bind, or if the
   *    socket is already bound.
   */
  void bind(__NET_HOOK_DECLARE_MORE const SocketAddress& addr)  __NET_THROW_DECLARE(SocketException);

  /**
   * Binds a datagram socket to a local port and address.
   * @param laddr the local address
   * @param lport the local port
   * @exception SocketException if there is an error in the
   * underlying protocol, such as a TCP error.
   */
  virtual void bind(__NET_HOOK_DECLARE_MORE const InetAddress& laddr,int lport)  __NET_THROW_DECLARE(SocketException);
    
  /** 
   * Connects the socket to a remote address for this socket. When a
   * socket is connected to a remote address, packets may only be
   * sent to or received from that address. By default a datagram
   * socket is not connected.
   *
   * <p>If the remote destination to which the socket is connected does not
   * exist, or is otherwise unreachable, and if an ICMP destination unreachable
   * packet has been received for that address, then a subsequent call to 
   * send or receive may throw a PortUnreachableException. Note, there is no 
   * guarantee that the exception will be thrown.
   *
   * <p>A caller's permission to send and receive datagrams to a
   * given host and port are checked at connect time. When a socket
   * is connected, receive and send <b>will not
   * perform any security checks</b> on incoming and outgoing
   * packets, rv than matching the packet's and the socket's
   * address and port. A socket
   * connected to a multicast address may only be used to send packets.
   *
   * @param address the remote address for the socket
   *
   * @param port the remote port for the socket.
   * @throws  SocketException if the connect fails
   *
   */
  void connect(__NET_HOOK_DECLARE_MORE const InetAddress& address, int port) __NET_THROW_DECLARE(SocketException);
  
  /**
   * Connects this socket to a remote socket address (IP address + port number).
   * <p>
   * @param  addr  The remote address.
   * @throws  SocketException if the connect fails
   */
  void connect(__NET_HOOK_DECLARE_MORE const SocketAddress& addr)  __NET_THROW_DECLARE(SocketException);

  /**
   * Closes this datagram socket.
   */
  void close() ;

//------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------
  /**
   * Receive the datagram packet.
   *
   * @param   address  the destination socket address.
   * @param b the buffer into which the data is read
   * @param length the maximum number of bytes read
   * @exception SocketException if an I/O exception occurs
   * while receiving the datagram packet.
   */
  virtual int receive(__NET_HOOK_DECLARE_MORE SocketAddress* address,char* b, int length)  __NET_THROW_DECLARE(SocketException);

  /**
   * Receive the datagram packet.
   *
   * @param   address  the destination socket address.
   * @param b the buffer into which the data is read
   * @param length the maximum number of bytes read
   * @exception SocketException if an I/O exception occurs
   * while receiving the datagram packet.
   */
  virtual int peek(__NET_HOOK_DECLARE_MORE SocketAddress* address,char* b, int length)  __NET_THROW_DECLARE(SocketException);
  
    
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
  virtual void send(__NET_HOOK_DECLARE_MORE const SocketAddress* address,const char* b, int length)  __NET_THROW_DECLARE(SocketException);
 
//------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------
  /**
   * Returns wether the socket is closed or not.
   *
   * @return true if the socket has been closed
   */
  bool isClosed() const;
  
  /**
   * Returns the binding state of the socket.
   *
   * @return true if the socket succesfuly bound to an address
   */
  bool isBound() const;
  
  /**
   * Returns the connection state of the socket.
   *
   * @return true if the socket succesfuly connected to a server
   */
  bool isConnected() const;

//------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------

  /**
   * Returns the address to which this socket is connected. Returns null
   * if the socket is not connected.
   *
   * @return the address to which this socket is connected.
   */
  InetAddress getInetAddress() const;
  
  /**
   * Returns the port for this socket. Returns -1 if the socket is not
   * connected.
   *
   * @return the port to which this socket is connected.
   */
  int getPort() const;


  /**
   * Gets the local address to which the socket is bound.
   *
   * @return  the local address to which the socket is bound, or
   *            an <code>InetAddress</code> representing any local
   *            address if either the socket is not bound
   */
  InetAddress getLocalAddress() const;

  /**
   * Returns the port number on the local host to which this socket is bound.
   *
   * @return  the port number on the local host to which this socket is bound.
   */
  int getLocalPort() const;

  /**
   * Converts this socket to a <code>String</code>.
   *
   * @return  a string representation of this socket.
   */
  string toString()const;

//------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------  
public:

  /** Enable/disable SO_W_TIMEOUT with the specified timeout, in
   *  milliseconds. With this option set to a non-zero timeout,
   *  a call to receive() for this DatagramSocket
   *  will block for only this amount of time.  If the timeout expires,
   *  a <B>SocketTimeoutException</B> is raised, though the
   *  DatagramSocket is still valid.  The option <B>must</B> be enabled
   *  prior to entering the blocking operation to have effect.  The
   *  timeout must be > 0.
   *  A timeout of zero is interpreted as an infinite timeout.
   *
   * @param timeout the specified timeout in milliseconds.
   * @throw SocketException if there is an error in the underlying protocol, such as an UDP error. 
   */
  void setSoTimeout(__NET_HOOK_DECLARE_MORE int timeout)  __NET_THROW_DECLARE(SocketException);
  
  /**
   * Retrive setting for SO_W_TIMEOUT.  0 returns implies that the
   * option is disabled (i.e., timeout of infinity).
   *
   * @return the setting for SO_W_TIMEOUT
   * @throw SocketException if there is an error in the underlying protocol, such as an UDP error.
   */
  int getSoTimeout(__NET_HOOK_DECLARE_ONCE) const  __NET_THROW_DECLARE(SocketException) ;
  
  /**
   * Sets the SO_W_SNDBUF option to the specified value for this
   * <tt>DatagramSocket</tt>. The SO_W_SNDBUF option is used by the 
   * network implementation as a hint to size the underlying
   * network I/O buffers. The SO_W_SNDBUF setting may also be used 
   * by the network implementation to determine the maximum size
   * of the packet that can be sent on this socket.
   * <p>
   * As SO_W_SNDBUF is a hint, applications that want to verify
   * what size the buffer is should call {@link #getSendBufferSize()}.
   * <p>
   * Increasing the buffer size may allow multiple outgoing packets 
   * to be queued by the network implementation when the send rate
   * is high. 
   * <p>
   * Note: If {@link #send()} is used to send a 
   * <code>DatagramPacket</code> that is larger than the setting
   * of SO_W_SNDBUF then it is implementation specific if the
   * packet is sent or discarded.
   *
   * @param size the size to which to set the send buffer
   * size. This value must be greater than 0.
   *
   * @exception SocketException if there is an error 
   * in the underlying protocol, such as an UDP error.
   */
  void setSendBufferSize(__NET_HOOK_DECLARE_MORE int size)  __NET_THROW_DECLARE(SocketException);
  
  /**
   * Get value of the SO_W_SNDBUF option for this <tt>DatagramSocket</tt>, that is the
   * buffer size used by the platform for output on this <tt>DatagramSocket</tt>.
   *
   * @return the value of the SO_W_SNDBUF option for this <tt>DatagramSocket</tt>
   * @exception SocketException if there is an error in 
   * the underlying protocol, such as an UDP error.
   */
  int getSendBufferSize(__NET_HOOK_DECLARE_ONCE) const  __NET_THROW_DECLARE(SocketException) ;
  
  /**
   * Sets the SO_W_RCVBUF option to the specified value for this
   * <tt>DatagramSocket</tt>. The SO_W_RCVBUF option is used by the
   * the network implementation as a hint to size the underlying
   * network I/O buffers. The SO_W_RCVBUF setting may also be used
   * by the network implementation to determine the maximum size
   * of the packet that can be received on this socket.
   * <p>
   * Because SO_W_RCVBUF is a hint, applications that want to
   * verify what size the buffers were set to should call
   * {@link #getReceiveBufferSize()}.
   * <p>
   * Increasing SO_W_RCVBUF may allow the network implementation
   * to buffer multiple packets when packets arrive faster than
   * are being received using {@link #receive()}.
   * <p>
   * Note: It is implementation specific if a packet larger
   * than SO_W_RCVBUF can be received.
   *
   * @param size the size to which to set the receive buffer
   * size. This value must be greater than 0.
   *
   * @exception SocketException if there is an error in 
   * the underlying protocol, such as an UDP error.
   */
  void setReceiveBufferSize(__NET_HOOK_DECLARE_MORE int size) __NET_THROW_DECLARE(SocketException);
  
  /**
   * Get value of the SO_W_RCVBUF option for this <tt>DatagramSocket</tt>, that is the
   * buffer size used by the platform for input on this <tt>DatagramSocket</tt>.
   *
   * @return the value of the SO_W_RCVBUF option for this <tt>DatagramSocket</tt>
   * @exception SocketException if there is an error in the underlying protocol, such as an UDP error.
   */
  int getReceiveBufferSize(__NET_HOOK_DECLARE_ONCE) const  __NET_THROW_DECLARE(SocketException);
  
  /**
   * Enable/disable the SO_W_REUSEADDR socket option.
   * <p>
   * For UDP sockets it may be necessary to bind more than one
   * socket to the same socket address. This is typically for the
   * purpose of receiving multicast packets
   * (See {@link #MulticastSocket}). The
   * <tt>SO_W_REUSEADDR</tt> socket option allows multiple
   * sockets to be bound to the same socket address if the
   * <tt>SO_W_REUSEADDR</tt> socket option is enabled prior
   * to binding the socket using {@link #bind(SocketAddress)}.
   * <p>
   * When a <tt>DatagramSocket</tt> is created the initial setting
   * of <tt>SO_W_REUSEADDR</tt> is disabled.
   * <p>
   * The behaviour when <tt>SO_W_REUSEADDR</tt> is enabled or
   * disabled after a socket is bound (See {@link #isBound()})
   * is not defined.
   * 
   * @param on  whether to enable or disable the 
   * @exception SocketException if an error occurs enabling or
   *            disabling the <tt>SO_W_RESUEADDR</tt> socket option,
   *         or the socket is closed.

   */
  void setReuseAddress(__NET_HOOK_DECLARE_MORE bool on)  __NET_THROW_DECLARE(SocketException);
  
  /**
   * Tests if SO_W_REUSEADDR is enabled.
   *
   * @return a <code>boolean</code> indicating whether or not SO_W_REUSEADDR is enabled.
   * @exception SocketException if there is an error
   * in the underlying protocol, such as an UDP error. 
   */
  bool getReuseAddress(__NET_HOOK_DECLARE_ONCE) const  __NET_THROW_DECLARE(SocketException);
  
  /**
   * Enable/disable SO_W_BROADCAST.
   * @param on     whether or not to have broadcast turned on.
   * @exception SocketException if there is an error
   * in the underlying protocol, such as an UDP error.
   */
  void setBroadcast(__NET_HOOK_DECLARE_MORE bool on)  __NET_THROW_DECLARE(SocketException);
  
  /**
   * Tests if SO_W_BROADCAST is enabled.
   * @return a <code>boolean</code> indicating whether or not SO_W_BROADCAST is enabled.
   * @exception SocketException if there is an error
   * in the underlying protocol, such as an UDP error.
   */
  bool getBroadcast(__NET_HOOK_DECLARE_ONCE) const  __NET_THROW_DECLARE(SocketException) ;
  
  /**
   * Sets traffic class or type-of-service octet in the IP
   * datagram header for datagrams sent from this DatagramSocket.
   * As the underlying network implementation may ignore this
   * value applications should consider it a hint.
   *
   * <P> The tc <B>must</B> be in the range <code> 0 <= tc <=
   * 255</code>.
   * <p>Notes:
   * <p> for Internet Protocol v4 the value consists of an octet
   * with precedence and TOS fields as detailed in RFC 1349. The
   * TOS field is bitset created by bitwise-or'ing values such
   * the following :-
   * <p>
   * <UL>
   * <LI><CODE>IPTOS_LOWCOST (0x02)</CODE></LI>
   * <LI><CODE>IPTOS_RELIABILITY (0x04)</CODE></LI>
   * <LI><CODE>IPTOS_THROUGHPUT (0x08)</CODE></LI>
   * <LI><CODE>IPTOS_LOWDELAY (0x10)</CODE></LI>
   * </UL>
   * The last low order bit is always ignored as this
   * corresponds to the MBZ (must be zero) bit.
   * <p>
   * Setting bits in the precedence field may result in a 
   * SocketException indicating that the operation is not
   * permitted.
   * <p>
   * for Internet Protocol v6 <code>tc</code> is the value that 
   * would be placed into the sin6_flowinfo field of the IP header.
   *
   * @param tc  an <code>int</code> value for the bitset.
   * @throw SocketException if there is an error setting the
   * traffic class or type-of-service 
   */
  void setTrafficClass(__NET_HOOK_DECLARE_MORE int tc)  __NET_THROW_DECLARE(SocketException);
  
  /**
   * Gets traffic class or type-of-service in the IP datagram 
   * header for packets sent from this DatagramSocket.
   * <p>
   * As the underlying network implementation may ignore the
   * traffic class or type-of-service set using {@link #setTrafficClass()}
   * this method may return a different value than was previously
   * set using the {@link #setTrafficClass()} method on this 
   * DatagramSocket.
   *
   * @return the traffic class or type-of-service already set
   * @throw SocketException if there is an error obtaining the
   * traffic class or type-of-service value.
   */
  int getTrafficClass(__NET_HOOK_DECLARE_ONCE) const  __NET_THROW_DECLARE(SocketException) ;
  
//------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------ 
protected:
  /**
   * Creates the socket implementation.
   *
   * @throw SocketException if creation fails
   */
   void createImpl(__NET_HOOK_DECLARE_ONCE)  __NET_THROW_DECLARE(SocketException);
  
  /**
   * Get the <code>DatagramSocketImpl</code> attached to this socket, creating
   * it if necessary.
   *
   * @return  the <code>DatagramSocketImpl</code> attached to that ServerSocket.
   * @throw SocketException if creation fails
   */
  DatagramSocketImpl* getImpl(__NET_HOOK_DECLARE_ONCE) const  __NET_THROW_DECLARE(SocketException);
  
//------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------ 
private:
  /**
   * Various states of this socket.
   */
  bool created;
  bool bound;
  bool closed;

  /*
   * The implementation of this DatagramSocket.
   */
  DatagramSocketImpl* p_impl;

  /*
   * Connection state:
   * OPENCODE_ST_NOT_CONNECTED = socket not connected
   * OPENCODE_ST_CONNECTED = socket connected
   * OPENCODE_ST_CONNECTED_NO_IMPL = socket connected but not at impl level
   */
  static const int OPENCODE_ST_NOT_CONNECTED;
  static const int OPENCODE_ST_CONNECTED;
  static const int OPENCODE_ST_CONNECTED_NO_IMPL;

  int connectState;

  /*
   * Connected address & port
   */
  InetAddress connectedAddress;
  int connectedPort;
 
};

__OPENCODE_END_NAMESPACE

#endif
