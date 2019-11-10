#include "socket/connector.h"
#include "socket/socketUtils.h"

#include "port/utilities.h"
#include "port/libnumber.h"
#include "port/booleanComponent.h"

__OPENCODE_BEGIN_NAMESPACE 

//------------------------------------------------------------------
//
//------------------------------------------------------------------

ConnectorJob::~ConnectorJob() 
{
}

ConnectorJob::ConnectorJob(Connector* connector)
:__p_connector(connector)
{
}

void ConnectorJob::stop()
{
}

void ConnectorJob::run(JobData* pJobData) 
{
  InetAddress address;

  __OPENCODE_TRY
  {
    __NET_HOOK_DEFINE();
    address = InetAddress::getByName(__NET_HOOK_INVOKE_MORE __p_connector->getNameOrIp());
    __NET_HOOK_CHECK(return);
  }
  __OPENCODE_CATCH(SocketException& ex)
  {
    return;
  }

  int count = __p_connector->getConnectCount() - __p_connector->currentConnectCount;

  for(int i = 0; i < count;++i)
  {
    __p_connector->createConnectionInternal(address,__p_connector->getPort());    
  }
}

//------------------------------------------------------------------
//
//------------------------------------------------------------------
Connector*  Connector::createConnector(const Profile& profile)
{
  Properties props = profile.getProperties("CONNECTOR_CONF");
  return Connector::createConnector(props);
}
  
Connector*  Connector::createConnector(const Properties& props)
{
  Connector* __p_connector = new Connector();
  
  string address = StringsKit::trim(props.getProperty("Address"));
  string port = StringsKit::trim(props.getProperty("Port"));
  string connectCount = StringsKit::trim(props.getProperty("ConnectCount"));
  
  string connectionLinger = StringsKit::trim(props.getProperty("ConnectionLinger"));
  string connectionTimeout = StringsKit::trim(props.getProperty("ConnectionTimeout"));
  string tcpNoDelay = StringsKit::trim(props.getProperty("TcpNoDelay"));
  
  if(!address.empty())
  {
    __p_connector->setNameOrIp(address);
  }
  
  if(!port.empty())
  {
    __p_connector->setPort(Number::parseInt(port));
  }
  
  if(!connectCount.empty())
  {
    __p_connector->setConnectCount(Number::parseInt(connectCount));
  }
  
  if(!connectionLinger.empty())
  {
    __p_connector->setConnectionLinger(Number::parseInt(connectionLinger));
  }
  
  if(!connectionTimeout.empty())
  {
    __p_connector->setConnectionTimeout(Number::parseInt(connectionTimeout));
  }
  
  if(!tcpNoDelay.empty())
  {
    __p_connector->setTcpNoDelay(BooleanComponent::valueOf(tcpNoDelay));
  }

  return __p_connector;
}

//------------------------------------------------------------------
//
//------------------------------------------------------------------
Connector::~Connector()
{
  
}

Connector::Connector(SocketHandler* socketHandler)
:__p_SocketHandler(socketHandler)
{
  port = 0 ;
  connectCount = 1;
  currentConnectCount = 0;

  connectionLinger = -1;
  connectionTimeout = 0;
  tcpNoDelay = true;

  maxAttempts = 100;
  attemptDelay = 10 * 1000; 
}

SocketHandler* Connector::getSocketHandler()
{
  return __p_SocketHandler;
}

SocketHandler* Connector::setSocketHandler(SocketHandler* socketHandlerValue)
{
  SocketHandler* __tmp = this->__p_SocketHandler;
  this->__p_SocketHandler = socketHandlerValue;
  return __tmp;
}

//------------------------------------------------------------------
//
//------------------------------------------------------------------
int Connector::getPort() const
{
  return port;
}


void Connector::setPort(int portValue)
{
  this->port = portValue;
}


const string& Connector::getNameOrIp() const
{
  return address;
}


void   Connector::setNameOrIp(const string& addressValue)
{
  this->address = addressValue;
}


int Connector::getConnectCount() const
{
  return connectCount;
}


void Connector::setConnectCount(int countValue)
{
  this->connectCount = countValue;
}

int Connector::getConnectionLinger() const
{
  return connectionLinger;
}

void Connector::setConnectionLinger(int connectionLingerValue)
{
  this->connectionLinger = connectionLingerValue;
}


int Connector::getConnectionTimeout() const
{
  return connectionTimeout;
}

void Connector::setConnectionTimeout(int connectionTimeoutValue)
{
  this->connectionTimeout = connectionTimeoutValue;
}


bool Connector::getTcpNoDelay() const
{
  return tcpNoDelay;
}


void Connector::setTcpNoDelay(bool tcpNoDelayValue)
{
  this->tcpNoDelay = tcpNoDelayValue;
}

int Connector::getMaxAttempts()
{
  return maxAttempts;
}

void Connector::setMaxAttempts(int maxAttemptsValue)
{
  this->maxAttempts = maxAttemptsValue;
}

int Connector::getAttemptDelay()
{
  return attemptDelay;
}

void Connector::setAttemptDelay(int attemptDelayValue)
{
  this->attemptDelay = attemptDelayValue;
}

//------------------------------------------------------------------
//
//------------------------------------------------------------------
bool  Connector::createConnectionOnce(const InetAddress& addressValue,int portValue)
{
  Socket socket;

  __OPENCODE_TRY
  {
    __NET_HOOK_DEFINE();    
    socket.connect(__NET_HOOK_INVOKE_MORE addressValue,portValue);
  }
  __OPENCODE_CATCH(SocketException& ex)
  {
    return false;
  }

  /* Warning :
   *
   * To be able to close more quickly a connection, it's recommanded
   * to set linger to a small value.
   *
   * I (zosatapo) recommand to set Linger to 0, making socket closed immediatly
   * so the OS will free faster the underlying io descriptor and resources.
   * It's very important under heavy load !
   */
  if(connectionLinger < 0)
  {
    __OPENCODE_TRY 
    {
      __NET_HOOK_DEFINE();
      socket.setSoLinger(__NET_HOOK_INVOKE_MORE false,0);
    } 
    __OPENCODE_CATCH(SocketException & ex){} 
  }
  else
  {
    __OPENCODE_TRY 
    {
      __NET_HOOK_DEFINE();
      socket.setSoLinger(__NET_HOOK_INVOKE_MORE true,connectionLinger);
    } 
    __OPENCODE_CATCH(SocketException & ex){} 
  }

  if (connectionTimeout >= 0)
  {
    __OPENCODE_TRY 
    {
      __NET_HOOK_DEFINE();
      socket.setSoTimeout(__NET_HOOK_INVOKE_MORE connectionTimeout);
    } 
    __OPENCODE_CATCH(SocketException & ex){} 
  }

  if(tcpNoDelay)
  {
    __OPENCODE_TRY 
    {
      __NET_HOOK_DEFINE();
      socket.setTcpNoDelay(__NET_HOOK_INVOKE_MORE tcpNoDelay);
    } 
    __OPENCODE_CATCH(SocketException & ex){}  
  }
  
  if(__p_SocketHandler != 0)
  {
    __p_SocketHandler->handleConnection(socket);
  }
  else
  {
    socket.close();  
  }
  
  return true;
}

bool Connector::createConnectionInternal(const InetAddress& addressValue,int portValue)
{
  if(createConnectionOnce(addressValue,portValue))
  {
    return true;
  }

  if(maxAttempts == 0)
  {
    while(true)
    {
      if(createConnectionOnce(addressValue,portValue))
      {
        return true;
      }
      
      Thread::sleep(attemptDelay); 
      
    }//~end while(true)
    
  }
  else if(maxAttempts > 0)
  {
    for(int i = 0;i < maxAttempts;++i)
    {
      if(createConnectionOnce(addressValue,portValue))
      {
        return true;
      }  
      Thread::sleep(attemptDelay);
    }
  }
 
  return false;
}

__OPENCODE_END_NAMESPACE
