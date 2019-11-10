/** 
 *  plainSocketImpl.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef __OPENCODE_PLAINSOCKETIMPL_H__
#define __OPENCODE_PLAINSOCKETIMPL_H__

#include "socket/socketImpl.h"

#include "socket/socketdefs.h"
#include "socket/socketDescriptor.h"
#include "socket/inetAddress.h"
#include "socket/socketException.h"

__OPENCODE_BEGIN_NAMESPACE

/**
 * Default Socket Implementation. This implementation does
 * not implement any security checks.
 * Note this class should <b>NOT</b> be public.
 */
class __OPENCODE_NET_API_DECL PlainSocketImpl :public SocketImpl 
{
//------------------------------------------------------------
// ctor and dector
//------------------------------------------------------------
public:
  PlainSocketImpl(); 

  /**
   * Constructs an instance with the given file descriptor.
   */
  PlainSocketImpl(const SocketDescriptor& fd);

  PlainSocketImpl(const PlainSocketImpl& rv);
  PlainSocketImpl& operator=(const PlainSocketImpl& rv);

  virtual PlainSocketImpl* doClone() const;

public:
  virtual  ~PlainSocketImpl();


//------------------------------------------------------------
// interface
//------------------------------------------------------------
public:
  /**
   * Creates a socket with a bool that specifies whether this
   * is a stream socket (true) or an unconnected UDP socket (false).
   */
  virtual void create(__NET_HOOK_DECLARE_MORE bool stream)  __NET_THROW_DECLARE(SocketException) ;

  /**
   * Creates a socket and connects it to the specified port on
   * the specified host.
   * @param host the specified host
   * @param port the specified port
   * @throw SocketException if connection fails
   */
  virtual void connect(__NET_HOOK_DECLARE_MORE const string& host, int port) __NET_THROW_DECLARE(UnknownHostException, SocketException);

  /**
   * Creates a socket and connects it to the specified address on
   * the specified port.
   * @param address the address
   * @param port the specified port
   * @throw SocketException if connection fails
   */
  virtual void connect(__NET_HOOK_DECLARE_MORE const InetAddress& address, int port)  __NET_THROW_DECLARE(SocketException);

  /**
   * Creates a socket and connects it to the specified address on
   * the specified port.
   * @param address the address
   * @param timeout the timeout value in milliseconds, or zero for no timeout.
   * @throw SocketException if connection fails
   */
  virtual void connect(__NET_HOOK_DECLARE_MORE const SocketAddress& address, int timeout)  __NET_THROW_DECLARE(SocketException);


  /**
   * Binds the socket to the specified address of the specified local port.
   * @param address the address
   * @param port the port
   */
  virtual void bind(__NET_HOOK_DECLARE_MORE const InetAddress& address, int lport) __NET_THROW_DECLARE(SocketException);

  /**
   * Listens, for a specified amount of time, for connections.
   * @param count the amount of time to listen for connections
   */
  virtual void listen(__NET_HOOK_DECLARE_MORE int count)  __NET_THROW_DECLARE(SocketException);

  /**
   * Accepts connections.
   * @param s the connection
   */
  virtual  void accept(__NET_HOOK_DECLARE_MORE SocketImpl* ps)  __NET_THROW_DECLARE(SocketException);

 /**
   * Shutdown read-half of the socket connection;
   */
  virtual void shutdownInput();

  /**
   * Shutdown write-half of the socket connection;
   */
  virtual void shutdownOutput();

  /**
   * Closes the socket.
   */
  virtual void close();

//------------------------------------------------------------
// interface
//------------------------------------------------------------
public: 
  /** 
   * Reads into a byte array <i>b</i> , <i>length</i> bytes of data.
   * @param b the buffer into which the data is read
   * @param length the maximum number of bytes read
   * @return the actual number of bytes read, -1 is
   *          returned when the end of the stream is reached. 
   * @exception SocketException If an I/O error has occurred.
   */
  virtual int read(__NET_HOOK_DECLARE_MORE char* b, int length)  __NET_THROW_DECLARE(SocketException);

  /** 
   * Reads into a byte array <i>b</i> , <i>length</i> bytes of data.
   * @param b the buffer into which the data is read
   * @param length the maximum number of bytes read
   * @return the actual number of bytes read, -1 is
   *          returned when the end of the stream is reached. 
   * @exception SocketException If an I/O error has occurred.
   */
  virtual int peek(__NET_HOOK_DECLARE_MORE char* b, int length)  __NET_THROW_DECLARE(SocketException);
    
  /** 
   * Writes a byte array <i>b</i> , <i>length</i> bytes of data to sockets.
   * @param b the data to be written
   * @param length the number of bytes that are written
   * @exception SocketException If an I/O error has occurred.
   */
  virtual void write(__NET_HOOK_DECLARE_MORE const char* b, int length)  __NET_THROW_DECLARE(SocketException);

  virtual bool supportsUrgentData ();

  virtual void sendUrgentData (__NET_HOOK_DECLARE_MORE int data)  __NET_THROW_DECLARE(SocketException);
    
//------------------------------------------------------------
// interface
//------------------------------------------------------------
public:    
  virtual void setOption(__NET_HOOK_DECLARE_MORE int opt,const SocketOptionsValue& val)  __NET_THROW_DECLARE(SocketException); 

  virtual SocketOptionsValue getOption(__NET_HOOK_DECLARE_MORE int opt)  __NET_THROW_DECLARE(SocketException); 
  
  /*
   * Return the current value of SO_W_TIMEOUT
   */
  virtual int getTimeout();
  
private:
  /**
   * The workhorse of the connection operation.  Tries several times to
   * establish a connection to the given <host, port>.  If unsuccessful,
   * throws an SocketException indicating what went wrong.
   */
  void connectToAddress(__NET_HOOK_DECLARE_MORE const InetAddress& address, int port, int timeout)  __NET_THROW_DECLARE(SocketException);

public:
  /*
   * "Acquires" and returns the SocketDescriptor for this impl
   *
   * A corresponding releaseFD is required to "release" the
   * SocketDescriptor.
   */
  SocketDescriptor& acquireFD();

  /*
   * "Release" the SocketDescriptor for this impl. 
   *
   * If the use count goes to -1 then the socket is closed.
   */
  void releaseFD();

public:
  bool isConnectionReset();

  bool isConnectionResetPending();

  void setConnectionReset();

  void setConnectionResetPending();

  /*
   * Return true if already closed or close is pending
   */
  bool isClosedOrPending();
    
//------------------------------------------------------
// private data member
//------------------------------------------------------
private:
  /* instance variable for SO_W_TIMEOUT */
  int timeout;   // timeout in millisec
  
  // traffic class
  int trafficClass;

  bool shut_rd ;
  bool shut_wr ;

  /* number of threads using the SocketDescriptor */
  int fdUseCount;

  /* lock when increment/decrementing fdUseCount */
  //Object fdLock = new Object();

  /* indicates a close is pending on the file descriptor */
  bool closePending ;
  
  int resetState;
  //Object resetLock;

private:
  /* indicates connection reset state */
  const static int CONNECTION_NOT_RESET;
  const static int CONNECTION_RESET_PENDING;
  const static int CONNECTION_RESET;

  const static int SHUT_W_RD;
  const static int SHUT_W_WR;
};

__OPENCODE_END_NAMESPACE

#endif


