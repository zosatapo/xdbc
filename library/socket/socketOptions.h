/** 
 *  socketOptions.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef __OPENCODE_SOCKETOPTIONS_H__
#define __OPENCODE_SOCKETOPTIONS_H__

#include "socket/socketdefs.h"
#include "socket/socketException.h"
#include "socket/networkInterface.h"

__OPENCODE_BEGIN_NAMESPACE

class __OPENCODE_NET_API_DECL SocketOptionsValue
{
//----------------------------------------------------------
//
//----------------------------------------------------------
public:
  SocketOptionsValue()
  {
    sov_int_value = 0;
    sov_bol_value = false;
    
    sov_type = SOV_UNKNOWN;
  }

  virtual ~SocketOptionsValue() {}

//----------------------------------------------------------
//
//----------------------------------------------------------    
public:
  bool    sov_bol_value;
  int     sov_int_value;
  
  InetAddress      sov_inet_value;
  NetworkInterface sov_netif_value;
   
  int sov_type;

//----------------------------------------------------------
//
//----------------------------------------------------------
public:
  static const int  SOV_UNKNOWN;

  static const int  SOV_BOOLEAN;  
  static const int  SOV_INTEGER; 
  static const int  SOV_BOOLINT;
  
  static const int  SOV_INETADDRESS; 
  static const int  SOV_NETWORKINTERFACE;
};
  
  
#define SOCKETOPTIONS_SO_W_LINGER    0x0080
#define SOCKETOPTIONS_SO_W_TIMEOUT   0x1006
#define SOCKETOPTIONS_SO_W_IP_TOS    0x3
#define SOCKETOPTIONS_SO_W_BINDADDR  0x000F
#define SOCKETOPTIONS_SO_W_TCP_NODELAY 0x0001
#define SOCKETOPTIONS_SO_W_SNDBUF    0x1001
#define SOCKETOPTIONS_SO_W_KEEPALIVE 0x0008
#define SOCKETOPTIONS_SO_W_OOBINLINE 0x1003
#define SOCKETOPTIONS_SO_W_REUSEADDR 0x04
#define SOCKETOPTIONS_SO_W_RCVBUF    0x1002

#define SOCKETOPTIONS_SO_W_IP_MULTICAST_IF  0x10
#define SOCKETOPTIONS_SO_W_IP_MULTICAST_IF2  0x1f
#define SOCKETOPTIONS_SO_W_IP_MULTICAST_LOOP 0x12

class __OPENCODE_NET_API_DECL SocketOptions 
{
//------------------------------------------------------------
// ctor and dector
//------------------------------------------------------------
protected:
  SocketOptions(){}  
  
public:
  virtual  ~SocketOptions(){}

//------------------------------------------------------------
// interface
//------------------------------------------------------------
public:   
  /**
   * Enable/disable the option specified by <I>optID</I>.  If the option
   * is to be enabled, and it takes an option-specific "value",  this is
   * passed in <I>value</I>.  The actual type of value is option-specific,
   * and it is an error to pass something that isn't of the expected type.
   *
   * For an option that has a notion of on and off, and requires 
   * a non-boolean parameter, setting its value to anything rv than
   * <I>Boolean(false)</I> implicitly enables it.
   * <BR>
   * Throws SocketException if the option is unrecognized,
   * the socket is closed, or some low-level error occurred
   * <BR>
   *
   * @param optID identifies the option
   * @param value the parameter of the socket option
   * @throws SocketException if the option is unrecognized,
   * the socket is closed, or some low-level error occurred
   */
  virtual void setOption(__NET_HOOK_DECLARE_MORE int optID, const SocketOptionsValue& value)  __NET_THROW_DECLARE(SocketException) = 0;
  
  /**
   * Fetch the value of an option.
   *
   * @param optID an <code>int</code> identifying the option to fetch
   * @return the value of the option
   * @throws SocketException if the socket is closed
   * @throws SocketException if <I>optID</I> is unknown along the
   *         protocol stack (including the SocketImpl)
   */
  virtual SocketOptionsValue getOption(__NET_HOOK_DECLARE_MORE int optID)  __NET_THROW_DECLARE(SocketException) = 0;

//------------------------------------------------------------
// constant
//------------------------------------------------------------  
public:  
  /**
   * Disable Nagle's algorithm for this connection.  Written data
   * to the network is not buffered pending acknowledgement of
   * previously written data.
   *<P>
   * Valid for TCP only: SocketImpl.
   */
  
  const static int SO_W_TCP_NODELAY;

  /**
   * Fetch the local address binding of a socket (this option cannot
   * be "set" only "gotten", since sockets are bound at creation time,
   * and so the locally bound address cannot be changed).  The default local
   * address of a socket is INADDR_ANY, meaning any local address on a
   * multi-homed host.  A multi-homed host can use this option to accept
   * connections to only one of its addresses (in the case of a
   * ServerSocket or DatagramSocket), or to specify its return address
   * to the peer (for a Socket or DatagramSocket).  The parameter of
   * this option is an InetAddress.
   * <P>
   * This option <B>must</B> be specified in the constructor.
   * <P>
   * Valid for: SocketImpl, DatagramSocketImpl
   */

  const static int SO_W_BINDADDR;
      
  
  /** Sets SO_W_REUSEADDR for a socket.  This is used only for MulticastSockets
   *  and it is set by default for MulticastSockets.
   * <P>
   * Valid for: DatagramSocketImpl
   */
  
  const static int SO_W_REUSEADDR;
  
  /**
   * Sets SO_W_BROADCAST for a socket. This option enables and disables 
   * the ability of the process to send broadcast messages. It is supported
   * for only datagram sockets and only on networks that support
   * the concept of a broadcast message (e.g. Ethernet, token ring, etc.),
   * and it is set by default for DatagramSockets.
   */
  
  const static int SO_W_BROADCAST;
  
  /** Set which outgoing interface on which to send multicast packets.
   * Useful on hosts with multiple network interfaces, where applications
   * want to use rv than the system default.  Takes/returns an InetAddress.
   * <P>
   * Valid for Multicast: DatagramSocketImpl
   * <P>
   */
  
  const static int SO_W_IP_MULTICAST_IF;
  
  /** Same as above. This option is introduced so that the behaviour 
   *  with SO_W_IP_MULTICAST_IF will be kept the same as before, while
   *  this new option can support setting outgoing interfaces with either 
   *  IPv4 and IPv6 addresses.
   *
   *  NOTE: make sure there is no conflict with this
   */
  const static int SO_W_IP_MULTICAST_IF2;
  
  /** 
   * This option enables or disables local loopback of multicast datagrams.
   * This option is enabled by default for Multicast Sockets.
   */
  
  const static int SO_W_IP_MULTICAST_LOOP;
  
  /** 
   * This option sets the type-of-service or traffic class field 
   * in the IP header for a TCP or UDP socket.
   */
  
  const static int SO_W_IP_TOS;
  
  /**
   * Specify a linger-on-close timeout.  This option disables/enables
   * immediate return from a <B>close()</B> of a TCP Socket.  Enabling
   * this option with a non-zero Integer <I>timeout</I> means that a
   * <B>close()</B> will block pending the transmission and acknowledgement
   * of all data written to the peer, at which point the socket is closed
   * <I>gracefully</I>.  Upon reaching the linger timeout, the socket is
   * closed <I>forcefully</I>, with a TCP RST. Enabling the option with a
   * timeout of zero does a forceful close immediately. If the specified
   * timeout value exceeds 65,535 it will be reduced to 65,535.
   * <P>
   * Valid only for TCP: SocketImpl
   * 
   */
  const static int SO_W_LINGER;
  
  /** Set a timeout on blocking Socket operations:
   * <PRE>
   * ServerSocket.accept();
   * Socket.connect();
   * Socket.read();
   * DatagramSocket.receive();
   * </PRE>
   *
   * <P> The option must be set prior to entering a blocking
   * operation to take effect.  
   *
   * <P> Valid for all sockets: SocketImpl, DatagramSocketImpl
   */
  const static int SO_W_TIMEOUT;
  
  /**
   * Set a hint the size of the underlying buffers used by the
   * platform for outgoing network I/O. When used in set, this is a
   * suggestion to the kernel from the application about the size of
   * buffers to use for the data to be sent over the socket. When
   * used in get, this must return the size of the buffer actually
   * used by the platform when sending out data on this socket.
   *
   * Valid for all sockets: SocketImpl, DatagramSocketImpl
   *
   */
  const static int SO_W_SNDBUF;
  
  /**
   * Set a hint the size of the underlying buffers used by the
   * platform for incoming network I/O. When used in set, this is a
   * suggestion to the kernel from the application about the size of
   * buffers to use for the data to be received over the
   * socket. When used in get, this must return the size of the
   * buffer actually used by the platform when receiving in data on
   * this socket.
   *
   * Valid for all sockets: SocketImpl, DatagramSocketImpl
   *
   */
  const static int SO_W_RCVBUF;
  
  /**
   * When the keepalive option is set for a TCP socket and no data
   * has been exchanged across the socket in either direction for 
   * 2 hours (NOTE: the actual value is implementation dependent),
   * TCP automatically sends a keepalive probe to the peer. This probe is a 
   * TCP segment to which the peer must respond. 
   * One of three responses is expected:
   * 1. The peer responds with the expected ACK. The application is not 
   *    notified (since everything is OK). TCP will send another probe 
   *    following another 2 hours of inactivity.
   * 2. The peer responds with an RST, which tells the local TCP that
   *    the peer host has crashed and rebooted. The socket is closed.
   * 3. There is no response from the peer. The socket is closed. 
   *
   * The purpose of this option is to detect if the peer host crashes. 
   *
   * Valid only for TCP socket: SocketImpl
   *
   */
  const static int SO_W_KEEPALIVE;
  
  /**
   * When the OOBINLINE option is set, any TCP urgent data received on
   * the socket will be received through the socket input stream.
   * When the option is disabled (which is the default) urgent data
   * is silently discarded.
   *
   */
  const static int SO_W_OOBINLINE;
  
};

__OPENCODE_END_NAMESPACE

#endif

