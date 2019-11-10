/** 
 *  plainDatagramSocket.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef __OPENCODE_PLAINDATAGRAMSOCKETIMPL_H__
#define __OPENCODE_PLAINDATAGRAMSOCKETIMPL_H__

#include "socket/socketOptions.h"
#include "socket/datagramSocketImpl.h"

#include "socket/socketdefs.h"

#include "socket/socketDescriptor.h"
#include "socket/inetAddress.h"
#include "socket/socketException.h"
#include "socket/socketDescriptor.h"

__OPENCODE_BEGIN_NAMESPACE


/**
 * Concrete datagram and multicast socket implementation base class.
 * Note: This is not a public class, so that applets cannot call
 * into the implementation directly and hence cannot bypass the
 * security checks present in the DatagramSocket and MulticastSocket
 * classes.
 *
 */

class __OPENCODE_NET_API_DECL PlainDatagramSocketImpl : public DatagramSocketImpl
{
//------------------------------------------------------------------------
//
//------------------------------------------------------------------------
public:
   virtual ~PlainDatagramSocketImpl();

   PlainDatagramSocketImpl();   
   PlainDatagramSocketImpl(const SocketDescriptor& fd);

   PlainDatagramSocketImpl(const PlainDatagramSocketImpl& rv);
   PlainDatagramSocketImpl& operator=(const PlainDatagramSocketImpl& rv);

   virtual PlainDatagramSocketImpl* doClone() const;

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------ 
public:
  /**
   * Creates a datagram socket
   */
  void create(__NET_HOOK_DECLARE_ONCE)  __NET_THROW_DECLARE(SocketException);
  
  /**
   * Binds a datagram socket to a local port.
   */
  void bind(__NET_HOOK_DECLARE_MORE const InetAddress& laddr,int lport) __NET_THROW_DECLARE(SocketException);
  
  
  /**
   * Connects a datagram socket to a remote destination. This associates the remote
   * address with the local socket so that datagrams may only be sent to this destination
   * and received from this destination.
   * @param address the remote InetAddress to connect to
   * @param port the remote port number
   */
  void connect(__NET_HOOK_DECLARE_MORE const InetAddress& address, int port)  __NET_THROW_DECLARE(SocketException);
  
  /**
   * Close the socket.
   */
  void close();

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
  virtual void send(__NET_HOOK_DECLARE_MORE const SocketAddress* address,const char* b, int length)  __NET_THROW_DECLARE(SocketException) ;   

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------ 
public:
  /**
   * Set the TTL (time-to-live) option.
   * @param TTL to be set.
   */
  void setTimeToLive(__NET_HOOK_DECLARE_MORE int ttl)  __NET_THROW_DECLARE(SocketException);
  
  /**
   * Get the TTL (time-to-live) option.
   */
  int getTimeToLive(__NET_HOOK_DECLARE_ONCE)  __NET_THROW_DECLARE(SocketException);


//------------------------------------------------------------------------
//
//------------------------------------------------------------------------ 
public:
    /**
     * Join the multicast group.
     * @param multicast address to join.
     */
    void join(__NET_HOOK_DECLARE_MORE const InetAddress& inetaddr)  __NET_THROW_DECLARE(SocketException);

    /**
     * Leave the multicast group.
     * @param multicast address to leave.
     */
    void leave(__NET_HOOK_DECLARE_MORE const InetAddress& inetaddr)  __NET_THROW_DECLARE(SocketException);
    
    /**
     * Join the multicast group.
     * @param multicast address to join.
     * @param netIf specifies the local interface to receive multicast
     *        datagram packets
     */

    void joinGroup(__NET_HOOK_DECLARE_MORE const SocketAddress& mcastaddr,const NetworkInterface& netIf) __NET_THROW_DECLARE(SocketException);


    /**
     * Leave the multicast group.
     * @param multicast address to leave.
     * @param netIf specified the local interface to leave the group at
     */
    void leaveGroup(__NET_HOOK_DECLARE_MORE const SocketAddress& mcastaddr, const NetworkInterface& netIf) __NET_THROW_DECLARE(SocketException);

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------ 
public:
  /**
   * set a value - since we only support (setting) binary options
   * here, o must be a Boolean
   */
  
   void setOption(__NET_HOOK_DECLARE_MORE int optID, const SocketOptionsValue& o)  __NET_THROW_DECLARE(SocketException);
  
  /*
   * get option's state - set or not
   */
  
  SocketOptionsValue getOption(__NET_HOOK_DECLARE_MORE int optID)  __NET_THROW_DECLARE(SocketException);
  
//------------------------------------------------------------------------
//
//------------------------------------------------------------------------ 
protected:
  /* timeout value for receive() */
  int timeout;
  int trafficClass;
  bool connected;
  InetAddress connectedAddress;
  int connectedPort ;
  
  /* cached socket options */
  int multicastInterface ;
  bool loopbackMode;
  int ttl;   
};

__OPENCODE_END_NAMESPACE

#endif

