/** 
 *  serverSocket.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef  __CSB_SERVERSOCKET_H__
#define  __CSB_SERVERSOCKET_H__

#include  "socket/socketdefs.h"
#include  "socket/socket.h"
#include  "socket/inetAddress.h"
#include  "socket/socketDescriptor.h"
#include  "socket/socketException.h"
#include  "socket/socketAddress.h"

__OPENCODE_BEGIN_NAMESPACE

class SocketImpl;

/**
 * This class implements server sockets. A server socket waits for 
 * requests to come in over the network. It performs some operation 
 * based on that request, and then possibly returns a result to the requester.
 *
 */

class __OPENCODE_NET_API_DECL ServerSocket 
{
public:
  friend class PlainSocketImpl;

public:
  virtual ~ServerSocket() ;

  /**
   * Creates an unbound server socket.
   */
  ServerSocket();
  
  /**
   * Creates a server socket, bound to the specified port. A port of 
   * <code>0</code> creates a socket on any free port. 
   * <p>
   * The maximum queue length for incoming connection indications (a 
   * request to connect) is set to <code>50</code>. If a connection 
   * indication arrives when the queue is full, the connection is refused.
   *
   *
   * @param      port  the port number, or <code>0</code> to use any
   *                   free port.
   * 
   * @exception  SocketException  if an I/O error occurs when opening the socket.
   */
  ServerSocket(__NET_HOOK_DECLARE_MORE int port)  __NET_THROW_DECLARE(SocketException);
  
  /**
   * Creates a server socket and binds it to the specified local port 
   * number, with the specified backlog. 
   * A port number of <code>0</code> creates a socket on any 
   * free port. 
   * <p>
   * The maximum queue length for incoming connection indications (a 
   * request to connect) is set to the <code>backlog</code> parameter. If 
   * a connection indication arrives when the queue is full, the 
   * connection is refused. 
   *
   * <P>The <code>backlog</code> argument must be a positive
   * value greater than 0. If the value passed if equal or less
   * than 0, then the default value will be assumed.
   * <P>
   *
   * @param      port     the specified port, or <code>0</code> to use
   *                      any free port.
   * @param      backlog  the maximum length of the queue.
   * 
   * @exception  SocketException  if an I/O error occurs when opening the socket.
   */
  ServerSocket(__NET_HOOK_DECLARE_MORE int port, int backlog)  __NET_THROW_DECLARE(SocketException);
  
  /** 
   * Create a server with the specified port, listen backlog, and 
   * local IP address to bind to.  The <i>bindAddr</i> argument
   * can be used on a multi-homed host for a ServerSocket that
   * will only accept connect requests to one of its addresses.
   * If <i>bindAddr</i> is null, it will default accepting
   * connections on any/all local addresses.
   * The port must be between 0 and 65535, inclusive.
   *
   * <P>The <code>backlog</code> argument must be a positive
   * value greater than 0. If the value passed if equal or less
   * than 0, then the default value will be assumed.
   * <P>
   * @param port the local TCP port
   * @param backlog the listen backlog
   * @param bindAddr the local InetAddress the server will bind to
   * 
   * 
   * @throws  SocketException if an I/O error occurs when opening the socket.
   */
  ServerSocket(__NET_HOOK_DECLARE_MORE int port, int backlog, const InetAddress& bindAddr)  __NET_THROW_DECLARE(SocketException);

//------------------------------------------------------------------------
//Copy Ctor
//------------------------------------------------------------------------
public:
  ServerSocket(const ServerSocket& rhs);
  ServerSocket& operator= (const ServerSocket& rhs);

  bool operator== (const ServerSocket& rhs);

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------ 
public: 
  /**
   *
   * Binds the <code>ServerSocket</code> to a specific address
   * (IP address and port number).
   * <p>
   * If the address is <code>null</code>, then the system will pick up
   * an ephemeral port and a valid local address to bind the socket.
   * <p>
   * @param  endpoint  The IP address & port number to bind to.
   * @throws  SocketException if the bind operation fails, or if the socket
   *         is already bound.
   */
  void bind(__NET_HOOK_DECLARE_MORE const SocketAddress& endpoint)  __NET_THROW_DECLARE(SocketException);

  /**
   * Binds this socket to the specified port number on the specified host. 
   *
   * @param      host   the IP address of the remote host.
   * @param      port   the port number.
   * @exception  SocketException  if an I/O error occurs when binding this socket.
   */
  void bind(__NET_HOOK_DECLARE_MORE const InetAddress& host, int port)  __NET_THROW_DECLARE(SocketException);
    
  /**
   *
   * Binds the <code>ServerSocket</code> to a specific address
   * (IP address and port number).
   * <p>
   * If the address is <code>null</code>, then the system will pick up
   * an ephemeral port and a valid local address to bind the socket.
   * <P>
   * The <code>backlog</code> argument must be a positive
   * value greater than 0. If the value passed if equal or less
   * than 0, then the default value will be assumed.
   * @param  endpoint  The IP address & port number to bind to.
   * @param  backlog    The listen backlog length.
   * @throws  SocketException if the bind operation fails, or if the socket
   *         is already bound.
   */
  void bind(__NET_HOOK_DECLARE_MORE const SocketAddress& endpoint, int backlog)  __NET_THROW_DECLARE(SocketException);

  /**
   * Binds this socket to the specified port number on the specified host. 
   *
   * @param      host   the IP address of the remote host.
   * @param      port   the port number.
   * @param       backlog    The listen backlog length.
   * @throws  SocketException if the bind operation fails, or if the socket
   *         is already bound.
   */
  void bind(__NET_HOOK_DECLARE_MORE const InetAddress& host, int port, int backlog)  __NET_THROW_DECLARE(SocketException);
  

  /**
   * Listens for a connection to be made to this socket and accepts 
   * it. The method blocks until a connection is made. 
   * 
   * @exception  SocketException  if an I/O error occurs when waiting for a
   *               connection.
   * @exception  SocketTimeoutException if a timeout was previously set with setSoTimeout and
   *             the timeout has been reached.
   *
   * @return the new Socket
   */
  void accept(__NET_HOOK_DECLARE_MORE Socket& s)  __NET_THROW_DECLARE(SocketException);

  /**
   * Closes this socket. 
   *
   */
  void close() ;

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------ 
public: 
  /**
   * Returns the local address of this server socket.
   *
   * @return  the address to which this socket is bound,
   *          or <code>null</code> if the socket is unbound.
   */
  InetAddress getInetAddress() const;
  
  /**
   * Returns the port on which this socket is listening.
   *
   * @return  the port number to which this socket is listening or
   *          -1 if the socket is not bound yet.
   */
  int getLocalPort() const;
    
  /**
   * Returns the binding state of the ServerSocket.
   *
   * @return true if the ServerSocket succesfuly bound to an address
   */
  bool isBound() const;

  /**
   * Returns the closed state of the ServerSocket.
   *
   * @return true if the socket has been closed

   */
  bool isClosed() const;
  
  /**
   * Returns the implementation address and implementation port of 
   * this socket as a <code>String</code>.
   *
   * @return  a string representation of this socket.
   */
   string toString() const;

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------ 
public: 
  /**
   * Enable/disable SO_W_TIMEOUT with the specified timeout, in
   * milliseconds.  With this option set to a non-zero timeout,
   * a call to accept() for this ServerSocket
   * will block for only this amount of time.  If the timeout expires,
   * a <B>SocketTimeoutException</B> is raised, though the
   * ServerSocket is still valid.  The option <B>must</B> be enabled
   * prior to entering the blocking operation to have effect.  The 
   * timeout must be > 0.
   * A timeout of zero is interpreted as an infinite timeout.  
   * @param timeout the specified timeout, in milliseconds
   * @exception SocketException if there is an error in 
   * the underlying protocol, such as a TCP error. 
   */
  void setSoTimeout(__NET_HOOK_DECLARE_MORE int timeout)  __NET_THROW_DECLARE(SocketException);
  
  /** 
   * Retrive setting for SO_W_TIMEOUT.  0 returns implies that the
   * option is disabled (i.e., timeout of infinity).
   * @return the SO_W_TIMEOUT value
   * @exception SocketException if an I/O error occurs
   */
  int getSoTimeout(__NET_HOOK_DECLARE_ONCE) const  __NET_THROW_DECLARE(SocketException);

  /**
   * Enable/disable the SO_W_REUSEADDR socket option.
   * <p>
   * When a TCP connection is closed the connection may remain
   * in a timeout state for a period of time after the connection
   * is closed (typically known as the <tt>TIME_WAIT</tt> state 
   * or <tt>2MSL</tt> wait state).
   * For applications using a well known socket address or port 
   * it may not be possible to bind a socket to the required
   * <tt>SocketAddress</tt> if there is a connection in the
   * timeout state involving the socket address or port. 
   * <p>
   * Enabling <tt>SO_W_REUSEADDR</tt> prior to binding the socket
   * using {@link #bind(SocketAddress)} allows the socket to be
   * bound even though a previous connection is in a timeout
   * state.
   * <p>
   * When a <tt>ServerSocket</tt> is created the initial setting
   * of <tt>SO_W_REUSEADDR</tt> is not defined. Applications can
   * use {@link getReuseAddress()} to determine the initial 
   * setting of <tt>SO_W_REUSEADDR</tt>. 
   * <p>
   * The behaviour when <tt>SO_W_REUSEADDR</tt> is enabled or
   * disabled after a socket is bound (See {@link #isBound()})
   * is not defined.
   * 
   * @param on  whether to enable or disable the socket option
   * @exception SocketException if an error occurs enabling or
   *            disabling the <tt>SO_W_RESUEADDR</tt> socket option,
   *      or the socket is closed.
   */     
  void setReuseAddress(__NET_HOOK_DECLARE_MORE bool on)  __NET_THROW_DECLARE(SocketException);
  
  /**
   * Tests if SO_W_REUSEADDR is enabled.
   *
   * @return a <code>bool</code> indicating whether or not SO_W_REUSEADDR is enabled.
   * @exception SocketException if there is an error
   * in the underlying protocol, such as a TCP error. 
   */
  bool getReuseAddress(__NET_HOOK_DECLARE_ONCE) const  __NET_THROW_DECLARE(SocketException);
    
  /**
   * Sets a default proposed value for the SO_W_RCVBUF option for sockets 
   * accepted from this <tt>ServerSocket</tt>. The value actually set 
   * in the accepted socket must be determined by calling 
   * {@link Socket#getReceiveBufferSize()} after the socket 
   * is returned by {@link #accept()}. 
   * <p>
   * The value of SO_W_RCVBUF is used both to set the size of the internal
   * socket receive buffer, and to set the size of the TCP receive window
   * that is advertized to the remote peer.
   * <p>
   * It is possible to change the value subsequently, by calling 
   * {@link Socket#setReceiveBufferSize(int)}. However, if the application 
   * wishes to allow a receive window larger than 64K bytes, as defined by RFC1323
   * then the proposed value must be set in the ServerSocket <B>before</B> 
   * it is bound to a local address. This implies, that the ServerSocket must be 
   * created with the no-argument constructor, then setReceiveBufferSize() must 
   * be called and lastly the ServerSocket is bound to an address by calling bind(). 
   * <p>
   * Failure to do this will not cause an error, and the buffer size may be set to the
   * requested value but the TCP receive window in sockets accepted from 
   * this ServerSocket will be no larger than 64K bytes.
   *
   * @exception SocketException if there is an error
   * in the underlying protocol, such as a TCP error. 
   *
   * @param size the size to which to set the receive buffer
   * size. This value must be greater than 0.
   *
   */
   void setReceiveBufferSize (__NET_HOOK_DECLARE_MORE int size)  __NET_THROW_DECLARE(SocketException);
  
  /**
   * Gets the value of the SO_W_RCVBUF option for this <tt>ServerSocket</tt>, 
   * that is the proposed buffer size that will be used for Sockets accepted
   * from this <tt>ServerSocket</tt>.
   * 
   * <p>Note, the value actually set in the accepted socket is determined by
   * calling {@link Socket#getReceiveBufferSize()}.
   * @return the value of the SO_W_RCVBUF option for this <tt>Socket</tt>.
   * @exception SocketException if there is an error
   * in the underlying protocol, such as a TCP error. 
   */
  int getReceiveBufferSize(__NET_HOOK_DECLARE_ONCE) const  __NET_THROW_DECLARE(SocketException);

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------ 
protected:
  /**
   * Get the <code>SocketImpl</code> attached to this socket, creating
   * it if necessary.
   *
   * @return  the <code>SocketImpl</code> attached to that ServerSocket.
   * @throw SocketException if creation fails.
   */
  SocketImpl* getImpl(__NET_HOOK_DECLARE_ONCE) const  __NET_THROW_DECLARE(SocketException);
  
  void setImpl();
 
  /**
   * Creates the socket implementation.
   *
   * @throw SocketException if creation fails
   */
  void createImpl(__NET_HOOK_DECLARE_ONCE)  __NET_THROW_DECLARE(SocketException);
        
//------------------------------------------------------------------------
//
//------------------------------------------------------------------------ 
protected:
  void setBound();

  void setCreated();
    
//------------------------------------------------------------------------
//
//------------------------------------------------------------------------ 
private:
  /**
   * Various states of this socket.
   */
  bool created ;
  bool bound ;
  bool closed;

  InetAddress localaddress;

  /**
   * The implementation of this Socket.
   */
  SocketImpl* p_impl;
};

__OPENCODE_END_NAMESPACE

#endif
