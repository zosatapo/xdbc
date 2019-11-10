/**
 *  connector.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */

#ifndef __OPENCODE_CONNECTOR_H__
#define __OPENCODE_CONNECTOR_H__

#include "socket/socketdefs.h"
#include "socket/socketHandler.h"
#include "port/thread.h"
#include "port/profile.h"
#include "port/ioc_vector.h"

__OPENCODE_BEGIN_NAMESPACE

class Connector;

class __OPENCODE_NET_API_DECL ConnectorJob : public Job
{
public:
  virtual ~ConnectorJob() ;
  ConnectorJob(Connector* connector);

public:
  virtual void stop();
  virtual void run(JobData* pJobData) ;

protected:
  Connector* __p_connector;
};

class __OPENCODE_NET_API_DECL Connector 
{
//-------------------------------------------------------
//
//-------------------------------------------------------
public:
  friend class ConnectorJob;

//-------------------------------------------------------
//
//-------------------------------------------------------
public:
  static Connector*  createConnector(const Profile& profile);
  static Connector*  createConnector(const Properties& props);
  
//-------------------------------------------------------
//
//-------------------------------------------------------
public:
  ~Connector();
  Connector(SocketHandler* socketHandler = 0);

  SocketHandler* getSocketHandler();
  SocketHandler* setSocketHandler(SocketHandler* socketHandler);

//-------------------------------------------------------
//
//-------------------------------------------------------
public:
  /**
   * Return the port number we connect to for Connector.
   */
   int getPort() const;

  /**
   * Set the port number we connect to for Connector.
   *
   * @param port The new port number
   */
   void setPort(int port); 

  /**
   * Return the host name or ip  we connect to for Connector.
   */  
  const string&   getNameOrIp() const;

  /**
   * Set the host name or ip  we connect to for Connector.
   *
   * @param port The new port number
   */
  void  setNameOrIp(const string& name_or_ip);

  /**
   * Return the connection count for this Connectoror.
   */
  int getConnectCount() const;
  
  /**
   * Set the connection count for this Connectoror.
   *
   * @param count The new connection count
   */
  void setConnectCount(int count);

//-------------------------------------------------------
//
//-------------------------------------------------------
public:
  /**
   * Return the connection linger (milliseconds) for this Connector.
   */
  int getConnectionLinger() const;
  
  /**
   * Set the connection linger  (milliseconds) for this Connector.
   *
   * @param count The new connection linge
   */
  void setConnectionLinger(int connectionLinger);

  /**
   * Return the connection timeout  (milliseconds) for this Connector.
   */
  int getConnectionTimeout() const;
  
  /**
   * Set the connection timeout (milliseconds) for this Connector.
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

  int  getMaxAttempts();  
  void setMaxAttempts(int maxAttempts);

  int  getAttemptDelay();
  void setAttemptDelay(int attemptDelay);

//-------------------------------------------------------
//
//-------------------------------------------------------
protected:
  bool createConnectionOnce(const InetAddress& address,int port);  
  bool createConnectionInternal(const InetAddress& address,int port);

protected:
  /**
   * The port number on which we listen for requests.
   */
  int port ; 

  /**
   * The connect count for this Connector.
   */
  int connectCount;

  /**
   * The IP address on which to bind, if any.  If <code>empty</code>, all
   * addresses on the server will be bound.
   */
  string  address;

  /**
   * when enabling a connection (or if a problem occurs on the connection), the PPSContext attempts to 
   * reopen the connection according to the MaxAttempts parameter setting.
   *
   * Values can be:
   * 1) -1: no new attempt after a failure 
   * 2) 0: infinite attempts 
   * 3) any rv positive value
   */
  int maxAttempts;

  /**
   * delay between two connection attempts in case of failure.
   * in millisecond
   */
  int attemptDelay;

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
  SocketHandler* __p_SocketHandler;
  int currentConnectCount;
};

__OPENCODE_END_NAMESPACE

#endif

