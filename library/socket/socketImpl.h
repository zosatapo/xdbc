/** 
 *  socketImpl.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef __OPENCODE_SOCKETIMPL_H__
#define __OPENCODE_SOCKETIMPL_H__

#include "socket/socketdefs.h"

#include "socket/socketDescriptor.h"
#include "socket/socketException.h"
#include "socket/socketOptions.h"
#include "socket/socketAddress.h"

__OPENCODE_BEGIN_NAMESPACE

class Socket;
class ServerSocket;

/**
 * The abstract class <code>SocketImpl</code> is a common superclass 
 * of all classes that actually implement sockets. It is used to 
 * create both client and server sockets. 
 * <p>
 * A "plain" socket implements these methods exactly as 
 * described, without attempting to go through a firewall or proxy. 
 *
 */
class __OPENCODE_NET_API_DECL SocketImpl :public SocketOptions 
{
//------------------------------------------------------------
// friend class
//------------------------------------------------------------
public:
   friend class SocketUtils;
   
//------------------------------------------------------------
// ctor and dector
//------------------------------------------------------------
protected:
  SocketImpl();

  SocketImpl(const SocketImpl& rv);
  SocketImpl& operator=(const SocketImpl& rv);

  virtual SocketImpl* doClone() const = 0;

public:
  virtual  ~SocketImpl();

  SocketImpl* clone() const;
//------------------------------------------------------------
// interface
//------------------------------------------------------------
public:
  /**
   * Creates either a stream or a datagram socket. 
   *
   * @param      stream   if <code>true</code>, create a stream socket;
   *                      otherwise, create a datagram socket.
   * @exception  SocketException  if an I/O error occurs while creating the
   *               socket.
   */
  virtual void create(__NET_HOOK_DECLARE_MORE bool stream)  __NET_THROW_DECLARE(SocketException) = 0;
  
  /**
   * Connects this socket to the specified port on the named host. 
   *
   * @param      host   the name of the remote host.
   * @param      port   the port number.
   * @exception  SocketException  if an I/O error occurs when connecting to the
   *               remote host.
   */
  virtual void connect(__NET_HOOK_DECLARE_MORE const string& host, int port)  __NET_THROW_DECLARE(SocketException) = 0;
  
  /**
   * Connects this socket to the specified port number on the specified host.
   *
   * @param      address   the IP address of the remote host.
   * @param      port      the port number.
   * @exception  SocketException  if an I/O error occurs when attempting a
   *               connection.
   */
  virtual void connect(__NET_HOOK_DECLARE_MORE const InetAddress& address, int port)  __NET_THROW_DECLARE(SocketException) = 0;
  
  /**
   * Connects this socket to the specified port number on the specified host.
   * A timeout of zero is interpreted as an infinite timeout. The connection
   * will then block until established or an error occurs.
   *
   * @param      address   the Socket address of the remote host.
   * @param    timeout  the timeout value, in milliseconds, or zero for no timeout.
   * @exception  SocketException  if an I/O error occurs when attempting a
   *               connection.
   */
  virtual void connect(__NET_HOOK_DECLARE_MORE const SocketAddress& address, int timeout)  __NET_THROW_DECLARE(SocketException) = 0;
  
  /**
   * Binds this socket to the specified port number on the specified host. 
   *
   * @param      host   the IP address of the remote host.
   * @param      port   the port number.
   * @exception  SocketException  if an I/O error occurs when binding this socket.
   */
  virtual void bind(__NET_HOOK_DECLARE_MORE const InetAddress& host, int port)  __NET_THROW_DECLARE(SocketException) = 0;
  
  /**
   * Sets the maximum queue length for incoming connection indications 
   * (a request to connect) to the <code>count</code> argument. If a 
   * connection indication arrives when the queue is full, the 
   * connection is refused. 
   *
   * @param      backlog   the maximum length of the queue.
   * @exception  SocketException  if an I/O error occurs when creating the queue.
   */
  virtual void listen(__NET_HOOK_DECLARE_MORE int backlog)  __NET_THROW_DECLARE(SocketException) = 0;
  
  /**
   * Accepts a connection. 
   *
   * @param      s   the accepted connection.
   * @exception  SocketException  if an I/O error occurs when accepting the
   *               connection.
   */
  virtual void accept(__NET_HOOK_DECLARE_MORE SocketImpl* ps)  __NET_THROW_DECLARE(SocketException) = 0;
  
  /**
   * Closes this socket. 
   *
   */
  virtual void close() = 0;
 
  virtual void reset()  ;
  
//------------------------------------------------------------
// interface
//------------------------------------------------------------
public:
  /**
   * Send one byte of urgent data on the socket.
   * The byte to be sent is the low eight bits of the parameter
   * @param data The byte of data to send
   * @exception SocketException if there is an error
   *  sending the data.
   */
  virtual void sendUrgentData (__NET_HOOK_DECLARE_MORE int data)  __NET_THROW_DECLARE(SocketException) = 0;

  /** 
   * Reads into a byte array <i>b</i> , <i>length</i> bytes of data.
   * @param b the buffer into which the data is read
   * @param length the maximum number of bytes read
   * @return the actual number of bytes read, -1 is
   *          returned when the end of the stream is reached. 
   * @exception SocketException If an I/O error has occurred.
   */
  virtual int read(__NET_HOOK_DECLARE_MORE char* b, int length)  __NET_THROW_DECLARE(SocketException) = 0;

  /** 
   * Reads into a byte array <i>b</i> , <i>length</i> bytes of data.
   * @param b the buffer into which the data is read
   * @param length the maximum number of bytes read
   * @return the actual number of bytes read, -1 is
   *          returned when the end of the stream is reached. 
   * @exception SocketException If an I/O error has occurred.
   */
  virtual int peek(__NET_HOOK_DECLARE_MORE char* b, int length)  __NET_THROW_DECLARE(SocketException) = 0;
    
  /** 
   * Writes a byte array <i>b</i> , <i>length</i> bytes of data to sockets.
   * @param b the data to be written
   * @param length the number of bytes that are written
   * @exception SocketException If an I/O error has occurred.
   */
  virtual void write(__NET_HOOK_DECLARE_MORE const char* b, int length)  __NET_THROW_DECLARE(SocketException) = 0;  
  
//------------------------------------------------------------
// public method
//------------------------------------------------------------
public:
  /**
   * Places the input stream for this socket at "end of stream".
   * Any data sent to this socket is acknowledged and then
   * silently discarded.
   *
   * If you read from a socket input stream after invoking 
   * shutdownInput() on the socket, the stream will return EOF.
   */
  virtual void shutdownInput() ;
  
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
  virtual void shutdownOutput(); 

  /**
   * Returns whether or not this SocketImpl supports sending 
   * urgent data. By default, false is returned
   * unless the method is overridden in a sub-class
   *
   * @return  true if urgent data supported
   */
  virtual bool supportsUrgentData () ;

  /**
   * Returns the value of this socket's <code>address</code> field.
   *
   * @return  the value of this socket's <code>address</code> field.
   */
  virtual const InetAddress& getInetAddress() const ;
  
  /**
   * Returns the value of this socket's <code>port</code> field.
   *
   * @return  the value of this socket's <code>port</code> field.
   */
  virtual int getPort() const;
  
  /**
   * Returns the value of this socket's <code>localport</code> field.
   *
   * @return  the value of this socket's <code>localport</code> field.
   */
  virtual int getLocalPort() const;

  /**
   * Returns the address and port of this socket as a <code>string</code>.
   *
   * @return  a string representation of this socket.
   */
  virtual string toString() const;

//------------------------------------------------------------
// public method
//------------------------------------------------------------
public:
  /**
   * Returns the value of this socket's <code>fd</code> field.
   *
   * @return  the value of this socket's <code>fd</code> field.
   */
  virtual const SocketDescriptor& getSocketDescriptor() const;
        
//------------------------------------------------------------
// public method
//------------------------------------------------------------
public:
  void setSocket(Socket* psoc);
  Socket* getSocket();
  
  void setServerSocket(ServerSocket* psoc);
  ServerSocket* getServerSocket(); 
  
//------------------------------------------------------------
// data member
//------------------------------------------------------------
protected:
  /**
   * The actual Socket object.
   */
  Socket* p_socket;
  ServerSocket* p_serverSocket;
  
  /**
   * The file descriptor object for this socket. 
   */
  SocketDescriptor fd;
  
  /**
   * The IP address of the remote end of this socket. 
   */
  InetAddress address;
  
  /**
   * The port number on the remote host to which this socket is connected. 
   */
  int port;
  
  /**
   * The local port number to which this socket is connected. 
   */
  int localport;   
};

__OPENCODE_END_NAMESPACE

#endif

