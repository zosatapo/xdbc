/**
 *  acceptor.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */

#ifndef __OPENCODE_ACCEPTOR_H__
#define __OPENCODE_ACCEPTOR_H__

#include "socket/socketdefs.h"
#include "socket/socketHandler.h" 
#include "socket/serverSocket.h" 
#include "port/thread.h"
#include "port/ioc_smart.h"
#include "port/profile.h"

__OPENCODE_BEGIN_NAMESPACE

class Acceptor;

class __OPENCODE_NET_API_DECL AcceptorJob : public Job
{
public:
  virtual ~AcceptorJob() ;
  AcceptorJob(Acceptor* acceptor);

public:
  virtual void stop();
  virtual void run(JobData* pJobData) ;

protected:
  Acceptor* __p_acceptor;
  bool shouldTerminate;
};


class __OPENCODE_NET_API_DECL Acceptor 
{
//-------------------------------------------------------
//
//-------------------------------------------------------
public:
  friend class AcceptorJob;

//-------------------------------------------------------
//
//-------------------------------------------------------
public:
  static Acceptor*   createAcceptor(const Profile& profile);
  static Acceptor*   createAcceptor(const Properties& props);
  
//-------------------------------------------------------
//
//-------------------------------------------------------
public:
  ~Acceptor();
 
  Acceptor(SocketHandler* socketHandler = 0);
 
  SocketHandler* getSocketHandler();
  SocketHandler* setSocketHandler(SocketHandler* socketHandler);
  
//-------------------------------------------------------
//
//-------------------------------------------------------
public:
  /**
   * Return the port number we listen to for Acceptor.
   */
   int getPort() const;

  /**
   * Set the port number we listen to for Acceptor.
   *
   * @param port The new port number
   */
   void setPort(int port); 

  /**
   * Return the host name or ip  we listen to for Acceptor.
   */  
  const string&   getNameOrIp() const;

  /**
   * Set the host name or ip  we listen to for Acceptor.
   *
   * @param port The new port number
   */
  void     setNameOrIp(const string& name_or_ip);

  /**
   * Return the accept count for this Acceptor.
   */
  int getAcceptCount() const;
  
  /**
   * Set the accept count for this Acceptor.
   *
   * @param count The new accept count
   */
  void setAcceptCount(int count);

//-------------------------------------------------------
//
//-------------------------------------------------------
public:
  /**
   * Return the connection linger (milliseconds) for this Acceptor.
   */
  int getConnectionLinger() const;
  
  /**
   * Set the connection linger  (milliseconds) for this Acceptor.
   *
   * @param count The new connection linge
   */
  void setConnectionLinger(int connectionLinger);

  /**
   * Return the connection timeout  (milliseconds) for this Acceptor.
   */
  int getConnectionTimeout() const;
  
  /**
   * Set the connection timeout (milliseconds) for this Acceptor.
   *
   * @param count The new connection timeout
   */
  void setConnectionTimeout(int connectionTimeout);

  /**
   * Return the TCP no delay flag value.
   */
  bool getTcpNoDelay() const;
  
  /**
   * Set the TCP no delay flag which will be set on the socket after
   * accepting a connection.
   *
   * @param tcpNoDelay The new TCP no delay flag
   */
  void setTcpNoDelay(bool tcpNoDelay); 

protected:
  /**
   * The port number on which we listen for requests.
   */
  int port ; 

  /**
   * The accept count for this Acceptor.
   */
  int acceptCount;

  /**
   * The IP address on which to bind, if any.  If <code>empty</code>, all
   * addresses on the server will be bound.
   */
  string  address;

  /**
   * Linger value on the incoming connection.
   * Note : a value inferior to 0 means no linger.
   */
  int connectionLinger;

  /**
   * Timeout value on the incoming connection.
   * Note : a value of 0 means no timeout.
   */
  int connectionTimeout;

  /**
   * Use TCP no delay ?
   */
  bool tcpNoDelay;

//----------------------------------------------------------
//
//----------------------------------------------------------
  ServerSocket*  __p_ServerSocket;
  SocketHandler* __p_SocketHandler;
};

__OPENCODE_END_NAMESPACE

#endif

