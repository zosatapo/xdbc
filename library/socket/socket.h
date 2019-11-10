/** 
 *  socket.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef __OPENCODE_SOCKET_H__
#define __OPENCODE_SOCKET_H__

#include "socket/socketdefs.h"
#include "socket/socketDescriptor.h"
#include "socket/inetAddress.h"
#include "socket/socketException.h"
#include "socket/socketDescriptor.h"
#include "socket/socketAddress.h"

__OPENCODE_BEGIN_NAMESPACE

class SocketImpl;

/**
 * This class implements client sockets (also called just
 * "sockets"). A socket is an endpoint for communication
 * between two machines.
 *
 */
 
class __OPENCODE_NET_API_DECL Socket 
{ 
public:
  friend class PlainSocketImpl;
  friend class ServerSocket;

public:  
   virtual ~Socket() ;

  /**
   * Creates an unconnected socket, with the
   * system-default type of SocketImpl.
   *
   */
  Socket();

  /**
   * Creates an unconnected Socket with a user-specified
   * SocketImpl.
   * <P>
   * @param impl an instance of a <B>SocketImpl</B>
   * the subclass wishes to use on the Socket.
   *
   */
  Socket(SocketImpl* impl) ; 
      
  /**
   * Creates a stream socket and connects it to the specified port
   * number on the named host.
   *
   * @param      host   the host name, or <code>null</code> for the loopback address.
   * @param      port   the port number.
   *
   * @exception  UnknownHostException if the IP address of 
   * the host could not be determined.
   *
   * @exception  SocketException  if an I/O error occurs when creating the socket.
   */
  Socket(__NET_HOOK_DECLARE_MORE const string& host, int port) __NET_THROW_DECLARE(SocketException);
  
  /**
   * Creates a stream socket and connects it to the specified port
   * number at the specified IP address.
   * 
   * @param      address   the IP address.
   * @param      port      the port number.
   * @exception  SocketException  if an I/O error occurs when creating the socket.
   */
  Socket(__NET_HOOK_DECLARE_MORE const InetAddress& address, int port)  __NET_THROW_DECLARE(SocketException);
  
  /**
   * Creates a socket and connects it to the specified remote host on
   * the specified remote port. The Socket will also bind() to the local
   * address and port supplied.
   * 
   * @param host the name of the remote host, or <code>null</code> for the loopback address.
   * @param port the remote port
   * @param localAddr the local address the socket is bound to
   * @param localPort the local port the socket is bound to
   * @exception  SocketException  if an I/O error occurs when creating the socket.
   * 
   */
  Socket(__NET_HOOK_DECLARE_MORE const string& host, int port, const InetAddress& localAddr,int localPort)  __NET_THROW_DECLARE(SocketException);

  
  /**
   * Creates a socket and connects it to the specified remote address on
   * the specified remote port. The Socket will also bind() to the local
   * address and port supplied.
   * 
   * @param address the remote address
   * @param port the remote port
   * @param localAddr the local address the socket is bound to
   * @param localPort the local port the socket is bound to
   * @exception  SocketException  if an I/O error occurs when creating the socket.
   * 
   */
  Socket(__NET_HOOK_DECLARE_MORE const InetAddress& address, int port, const InetAddress& localAddr,int localPort)  __NET_THROW_DECLARE(SocketException);

//------------------------------------------------------------------------
//Copy Ctor
//------------------------------------------------------------------------
public:
  Socket(const Socket& rhs);
  Socket& operator= (const Socket& rhs);

  bool operator==(const Socket& rhs);

  bool isAccepted() const;
  void setAccepted(bool accepted); 

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------ 
public:  
  /**
   * Connects this socket to the specified port on the named host. 
   *
   * @param      host   the name of the remote host.
   * @param      port   the port number.
   * @exception  SocketException  if an I/O error occurs when connecting to the
   *               remote host.
   */
  void connect(__NET_HOOK_DECLARE_MORE const string& host, int port)  __NET_THROW_DECLARE(SocketException);
  
  /**
   * Connects this socket to the specified port number on the specified host.
   *
   * @param      address   the IP address of the remote host.
   * @param      port      the port number.
   * @exception  SocketException  if an I/O error occurs when attempting a
   *               connection.
   */
  void connect(__NET_HOOK_DECLARE_MORE const InetAddress& address, int port)  __NET_THROW_DECLARE(SocketException);
   
  /**
   * Connects this socket to the server.
   *
   * @param  endpoint the <code>SocketAddress</code>
   * @throws  SocketException if an error occurs during the connection
   */
  void connect(__NET_HOOK_DECLARE_MORE const SocketAddress& endpoint)  __NET_THROW_DECLARE(SocketException);
  
  /**
   * Connects this socket to the server with a specified timeout value.
   * A timeout of zero is interpreted as an infinite timeout. The connection
   * will then block until established or an error occurs.
   *
   * @param  endpoint the <code>SocketAddress</code>
   * @param  timeout  the timeout value to be used in milliseconds.
   * @throws  SocketException if an error occurs during the connection
   * @throws  SocketTimeoutException if timeout expires before connecting
   */
  void connect(__NET_HOOK_DECLARE_MORE const SocketAddress& endpoint, int timeout)  __NET_THROW_DECLARE(SocketException);
    
  /**
   * Binds the socket to a local address.
   * <P>
   * If the address is <code>null</code>, then the system will pick up
   * an ephemeral port and a valid local address to bind the socket.
   *
   * @param  bindpoint the <code>SocketAddress</code> to bind to
   * @throws  SocketException if the bind operation fails, or if the socket
   *         is already bound.
   */
  void bind(__NET_HOOK_DECLARE_MORE const SocketAddress& bindpoint)  __NET_THROW_DECLARE(SocketException);


  /**
   * Binds this socket to the specified port number on the specified host. 
   *
   * @param      host   the IP address of the remote host.
   * @param      port   the port number.
   * @exception  SocketException  if an I/O error occurs when binding this socket.
   */
  void bind(__NET_HOOK_DECLARE_MORE const InetAddress& host, int port)  __NET_THROW_DECLARE(SocketException);
  
  /**
   * Places the input stream for this socket at "end of stream".
   * Any data sent to the input stream side of the socket is acknowledged
   * and then silently discarded.
   * <p>
   * If you read from a socket input stream after invoking 
   * shutdownInput() on the socket, the stream will return EOF.
   *
   */
  void shutdownInput();
  
  /**
   * Disables the output stream for this socket.
   * For a TCP socket, any previously written data will be sent
   * followed by TCP's normal connection termination sequence.
   *
   * If you write to a socket output stream after invoking 
   * shutdownOutput() on the socket, the stream will throw 
   * an SocketException.
   *
   */
  void shutdownOutput() ;  
  
  /**
   * Closes this socket.
   *
   * Once a socket has been closed, it is not available for further networking
   * use (i.e. can't be reconnected or rebound). A new socket needs to be
   * created.
   *
   */
  void close() ; 
  
//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
public:

  /** 
   * Reads into a byte array <i>b</i> , <i>length</i> bytes of data.
   * @param b the buffer into which the data is read
   * @param length the maximum number of bytes read
   * @return the actual number of bytes read, -1 is
   *          returned when the end of the stream is reached. 
   * @exception SocketException If an I/O error has occurred.
   */
  int read(__NET_HOOK_DECLARE_MORE char* b, int length)  __NET_THROW_DECLARE(SocketException);

  /** 
   * Reads into a byte array <i>b</i> , <i>length</i> bytes of data.
   * @param b the buffer into which the data is read
   * @param length the maximum number of bytes read
   * @return the actual number of bytes read, -1 is
   *          returned when the end of the stream is reached. 
   * @exception SocketException If an I/O error has occurred.
   */
  int peek(__NET_HOOK_DECLARE_MORE char* b, int length)  __NET_THROW_DECLARE(SocketException);
    
  /** 
   * Writes a byte array <i>b</i> , <i>length</i> bytes of data to sockets.
   * @param b the data to be written
   * @param length the number of bytes that are written
   * @exception SocketException If an I/O error has occurred.
   */
  void write(__NET_HOOK_DECLARE_MORE const char* b, int length)  __NET_THROW_DECLARE(SocketException);

  /**
   * Send one byte of urgent data on the socket. The byte to be sent is the lowest eight
   * bits of the data parameter. The urgent byte is
   * sent after any preceding writes to the socket OutputStream
   * and before any future writes to the OutputStream.
   * @param data The byte of data to send
   * @exception SocketException if there is an error
   *  sending the data.
   */
  void sendUrgentData (__NET_HOOK_DECLARE_MORE int data)  __NET_THROW_DECLARE(SocketException);


  /**
   * Reads into a byte array <i>b</i> , <i>length</i> bytes of data.
   * @param b the buffer into which the data is read
   * @param length the maximum number of bytes read
   * @return the actual number of bytes read, -1 is
   *          returned when the end of the stream is reached.
   *
   * @exception SocketException If an I/O error has occurred.
   */
  static int jread(__NET_HOOK_DECLARE_MORE Socket& socket,char* b,int& length)  __NET_THROW_DECLARE(SocketException);

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
public:
 /**
   * Returns the address to which the socket is connected.
   *
   * @return  the remote IP address to which this socket is connected,
   *    or <code>null</code> if the socket is not connected.
   */
  InetAddress getInetAddress() const;

  /**
   * Returns the remote port to which this socket is connected.
   *
   * @return  the remote port number to which this socket is connected, or
   *          0 if the socket is not connected yet.
   */
  int getPort() const;
    
  /**
   * Gets the local address to which the socket is bound.
   *
   * @return the local address to which the socket is bound or 
   *         <code>InetAddress.anyLocalAddress()</code>
   *         if the socket is not bound yet.
   * 
   */
  InetAddress getLocalAddress() const;
  
  /**
   * Returns the local port to which this socket is bound.
   *
   * @return  the local port number to which this socket is bound or -1
   *          if the socket is not bound yet.
   */
  int getLocalPort()const;

//-------------------------------------------------------------------------
//
//------------------------------------------------------------------------- 
public:   
  /**
   * Returns the connection state of the socket.
   *
   * @return true if the socket successfuly connected to a server
   */
  bool isConnected() const;
  
  /**
   * Returns the binding state of the socket.
   *
   * @return true if the socket successfuly bound to an address
   */
  bool isBound()const;
  
  
  /**
   * Returns wether the read-half of the socket connection is closed.
   *
   * @return true if the input of the socket has been shutdown
   */
  bool isInputShutdown()const;
  
  /**
   * Returns wether the write-half of the socket connection is closed.
   *
   * @return true if the output of the socket has been shutdown
   */
  bool isOutputShutdown()const;

  /**
   * Returns the closed state of the socket.
   *
   * @return true if the socket has been closed
   */
  bool isClosed()const;
  
    /**
   * Converts this socket to a <code>String</code>.
   *
   * @return  a string representation of this socket.
   */
  string toString()const;
  
//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
  
public:
  /**
   * Enable/disable TCP_NODELAY (disable/enable Nagle's algorithm).
   *
   * @param on <code>true</code> to enable TCP_NODELAY, 
   * <code>false</code> to disable.
   *
   * @exception SocketException if there is an error 
   * in the underlying protocol, such as a TCP error.
   * 
   */
  void setTcpNoDelay(__NET_HOOK_DECLARE_MORE bool on)  __NET_THROW_DECLARE(SocketException);
  
  /**
   * Tests if TCP_NODELAY is enabled.
   *
   * @return a <code>bool</code> indicating whether or not TCP_NODELAY is enabled.
   * @exception SocketException if there is an error
   * in the underlying protocol, such as a TCP error. 
   */
  bool getTcpNoDelay(__NET_HOOK_DECLARE_ONCE) const  __NET_THROW_DECLARE(SocketException);
  
  /**
   * Enable/disable SO_W_LINGER with the specified linger time in seconds. 
   * The maximum timeout value is platform specific.
   *
   * The setting only affects socket close.
   * 
   * @param on     whether or not to linger on.
   * @param linger how XLONG to linger for, if on is true.
   * @exception SocketException if there is an error
   * in the underlying protocol, such as a TCP error. 
   */
  void setSoLinger(__NET_HOOK_DECLARE_MORE bool on, int linger)  __NET_THROW_DECLARE(SocketException);
  
  /**
   * Returns setting for SO_W_LINGER. -1 returns implies that the
   * option is disabled.
   *
   * The setting only affects socket close.
   *
   * @return the setting for SO_W_LINGER.
   * @exception SocketException if there is an error
   * in the underlying protocol, such as a TCP error. 
   * 
   */
  int getSoLinger(__NET_HOOK_DECLARE_ONCE) const  __NET_THROW_DECLARE(SocketException);
  
  /**
   * Enable/disable OOBINLINE (receipt of TCP urgent data)
   *
   * By default, this option is disabled and TCP urgent data received on a 
   * socket is silently discarded. If the user wishes to receive urgent data, then
   * this option must be enabled. When enabled, urgent data is received
   * inline with normal data. 
   * <p>
   * Note, only limited support is provided for handling incoming urgent 
   * data. In particular, no notification of incoming urgent data is provided 
   * and there is no capability to distinguish between normal data and urgent
   * data unless provided by a higher level protocol.
   *
   * @param on <code>true</code> to enable OOBINLINE, 
   * <code>false</code> to disable.
   *
   * @exception SocketException if there is an error 
   * in the underlying protocol, such as a TCP error.
   * 
   */
  void setOOBInline(__NET_HOOK_DECLARE_MORE bool on)  __NET_THROW_DECLARE(SocketException);
  
  /**
   * Tests if OOBINLINE is enabled.
   *
   * @return a <code>bool</code> indicating whether or not OOBINLINE is enabled.
   * @exception SocketException if there is an error
   * in the underlying protocol, such as a TCP error. 
   */
  bool getOOBInline(__NET_HOOK_DECLARE_ONCE) const  __NET_THROW_DECLARE(SocketException);
  
  /**
   *  Enable/disable SO_W_TIMEOUT with the specified timeout, in
   *  milliseconds.  With this option set to a non-zero timeout,
   *  a read() call on the InputStream associated with this Socket
   *  will block for only this amount of time.  If the timeout expires,
   *  a <B>SocketTimeoutException</B> is raised, though the
   *  Socket is still valid. The option <B>must</B> be enabled
   *  prior to entering the blocking operation to have effect. The
   *  timeout must be > 0.
   *  A timeout of zero is interpreted as an infinite timeout.
   * @param timeout the specified timeout, in milliseconds.
   * @exception SocketException if there is an error
   * in the underlying protocol, such as a TCP error. 
   */
  void setSoTimeout(__NET_HOOK_DECLARE_MORE int timeout)  __NET_THROW_DECLARE(SocketException);
  
  /**
   * Returns setting for SO_W_TIMEOUT.  0 returns implies that the
   * option is disabled (i.e., timeout of infinity).
   * @return the setting for SO_W_TIMEOUT
   * @exception SocketException if there is an error
   * in the underlying protocol, such as a TCP error.  
   */
  int getSoTimeout(__NET_HOOK_DECLARE_ONCE) const  __NET_THROW_DECLARE(SocketException);
  
  /**
   * Sets the SO_W_SNDBUF option to the specified value for this
   * <tt>Socket</tt>. The SO_W_SNDBUF option is used by the platform's
   * networking code as a hint for the size to set
   * the underlying network I/O buffers.
   *
   * <p>Because SO_W_SNDBUF is a hint, applications that want to
   * verify what size the buffers were set to should call
   * {@link #getSendBufferSize()}.
   *
   * @exception SocketException if there is an error
   * in the underlying protocol, such as a TCP error. 
   *
   * @param size the size to which to set the send buffer
   * size. This value must be greater than 0.
   *
   */
  void setSendBufferSize(__NET_HOOK_DECLARE_MORE int size) __NET_THROW_DECLARE(SocketException);
  
  /**
   * Get value of the SO_W_SNDBUF option for this <tt>Socket</tt>, 
   * that is the buffer size used by the platform 
   * for output on this <tt>Socket</tt>.
   * @return the value of the SO_W_SNDBUF option for this <tt>Socket</tt>.
   *
   * @exception SocketException if there is an error
   * in the underlying protocol, such as a TCP error. 
   */
  int getSendBufferSize(__NET_HOOK_DECLARE_ONCE) const  __NET_THROW_DECLARE(SocketException);
  
  /**
   * Sets the SO_W_RCVBUF option to the specified value for this
   * <tt>Socket</tt>. The SO_W_RCVBUF option is used by the platform's
   * networking code as a hint for the size to set
   * the underlying network I/O buffers.
   *
   * <p>Increasing the receive buffer size can increase the performance of
   * network I/O for high-volume connection, while decreasing it can
   * help reduce the backlog of incoming data. 
   *
   * <p>Because SO_W_RCVBUF is a hint, applications that want to
   * verify what size the buffers were set to should call
   * {@link #getReceiveBufferSize()}.
   *
   * <p>The value of SO_W_RCVBUF is also used to set the TCP receive window
   * that is advertized to the remote peer. Generally, the window size
   * can be modified at any time when a socket is connected. However, if
   * a receive window larger than 64K is required then this must be requested
   * <B>before</B> the socket is connected to the remote peer. There are two
   * cases to be aware of:<p>
   * <ol>
   * <li>For sockets accepted from a ServerSocket, this must be done by calling
   * {@link ServerSocket#setReceiveBufferSize(int)} before the ServerSocket 
   * is bound to a local address.<p></li>
   * <li>For client sockets, setReceiveBufferSize() must be called before
   * connecting the socket to its remote peer.<p></li></ol>
   * @param size the size to which to set the receive buffer
   * size. This value must be greater than 0.
   *
   * @exception SocketException if there is an error
   * in the underlying protocol, such as a TCP error.
   */
  void setReceiveBufferSize(__NET_HOOK_DECLARE_MORE int size) __NET_THROW_DECLARE(SocketException);
  
  /**
   * Gets the value of the SO_W_RCVBUF option for this <tt>Socket</tt>, 
   * that is the buffer size used by the platform for 
   * input on this <tt>Socket</tt>.
   *
   * @return the value of the SO_W_RCVBUF option for this <tt>Socket</tt>.
   * @exception SocketException if there is an error
   * in the underlying protocol, such as a TCP error. 
   */
  int getReceiveBufferSize(__NET_HOOK_DECLARE_ONCE) const  __NET_THROW_DECLARE(SocketException);
  
  /**
   * Enable/disable SO_W_KEEPALIVE.
   * 
   * @param on     whether or not to have socket keep alive turned on.
   * @exception SocketException if there is an error
   * in the underlying protocol, such as a TCP error. 
   */
  void setKeepAlive(__NET_HOOK_DECLARE_MORE bool on)  __NET_THROW_DECLARE(SocketException);
  
  /**
   * Tests if SO_W_KEEPALIVE is enabled.
   *
   * @return a <code>bool</code> indicating whether or not SO_W_KEEPALIVE is enabled.
   * @exception SocketException if there is an error
   * in the underlying protocol, such as a TCP error. 
   */
  bool getKeepAlive(__NET_HOOK_DECLARE_ONCE) const  __NET_THROW_DECLARE(SocketException);
  
  /**
   * Sets traffic class or type-of-service octet in the IP
   * header for packets sent from this Socket.
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
   * @param tc        an <code>int</code> value for the bitset.
   * @throw SocketException if there is an error setting the traffic class or type-of-service
   */
  void setTrafficClass(__NET_HOOK_DECLARE_MORE int tc)  __NET_THROW_DECLARE(SocketException);
  
  /**
   * Gets traffic class or type-of-service in the IP header
   * for packets sent from this Socket
   * <p>
   * As the underlying network implementation may ignore the
   * traffic class or type-of-service set using {@link #setTrafficClass()}
   * this method may return a different value than was previously
   * set using the {@link #setTrafficClass()} method on this Socket.
   *
   * @return the traffic class or type-of-service already set
   * @throw SocketException if there is an error obtaining the
   * traffic class or type-of-service value.
   */
  int getTrafficClass(__NET_HOOK_DECLARE_ONCE) const  __NET_THROW_DECLARE(SocketException);
  
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
   * When a <tt>Socket</tt> is created the initial setting
   * of <tt>SO_W_REUSEADDR</tt> is disabled.
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

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------ 
protected:  
  /**
   * Creates the socket implementation.
   *
   * @param stream a <code>bool</code> value : <code>true</code> for a TCP socket,
   *         <code>false</code> for UDP.
   * @throw  SocketException if creation fails
   */
   void createImpl(__NET_HOOK_DECLARE_MORE bool stream)  __NET_THROW_DECLARE(SocketException);
  
  /**
   * Get the <code>SocketImpl</code> attached to this socket, creating
   * it if necessary.
   *
   * @return  the <code>SocketImpl</code> attached to that ServerSocket.
   * @throw   SocketException if creation fails
   */
  SocketImpl* getImpl(__NET_HOOK_DECLARE_ONCE) const  __NET_THROW_DECLARE(SocketException);
 
  
  /**
   * set the flags after an accept() call.
   */
  void postAccept();
  
  void setCreated() ;  
  void setBound(); 
  void setConnected();


//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------    
protected:
  /**
   * Various states of this socket.
   */
  bool created ;
  bool bound ;
  bool connected ;

  bool shutIn ;
  bool shutOut;
  
  bool closed ;
  
  /**
   * The implementation of this Socket.
   */
  SocketImpl* p_impl;

//------------------------------------------------
  bool accepted;
};

__OPENCODE_END_NAMESPACE

#endif

