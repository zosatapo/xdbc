#include "socket/acceptor.h"
#include "socket/socketUtils.h"

#include "port/logging.h"
#include "port/ioc_sbuf.h"
#include "port/utilities.h"
#include "port/libnumber.h"
#include "port/booleanComponent.h"
#include "port/libsys.h"

__OPENCODE_BEGIN_NAMESPACE 

//------------------------------------------------------------------
//
//------------------------------------------------------------------

AcceptorJob::~AcceptorJob() 
{
}

AcceptorJob::AcceptorJob(Acceptor* acceptor)
:__p_acceptor(acceptor)
{
  shouldTerminate = false;
}

void AcceptorJob::stop()
{
  if(shouldTerminate)
  {
    return;
  }

  shouldTerminate = true;

  int i = 0;
 
  while(++i <= 2)
  { 
    __OPENCODE_TRY 
    {
      __NET_HOOK_DEFINE();
      
      InetAddress address;
      if(__p_acceptor->getNameOrIp().empty())
      {
        address = InetAddress::getLocalHost(__NET_HOOK_INVOKE_ONCE);
      }
      else
      {
        address = InetAddress::getByName(__NET_HOOK_INVOKE_MORE __p_acceptor->getNameOrIp());
      }
      
      Socket socket(__NET_HOOK_INVOKE_MORE address,__p_acceptor->getPort());
      socket.close();  
      break;
    }
    __OPENCODE_CATCH(SocketException & ex)
    {
    }
  }
  
  __p_acceptor->__p_ServerSocket->close();
}

void AcceptorJob::run(JobData* pJobData) 
{
  __p_acceptor->__p_ServerSocket =  new ServerSocket();
  ServerSocket& serverSocket = *(__p_acceptor->__p_ServerSocket);
  
  InetAddress address;

  __OPENCODE_TRY 
  {
    __NET_HOOK_DEFINE();
    if(__p_acceptor->getNameOrIp().empty())
    {
      address = InetAddress::AnyAddress_IPV4;
    }
    else
    {
      address = InetAddress::getByName(__NET_HOOK_INVOKE_MORE __p_acceptor->getNameOrIp());
    }

    serverSocket.bind(__NET_HOOK_INVOKE_MORE address,__p_acceptor->getPort(),__p_acceptor->getAcceptCount());

    __OPENCODE_TRY 
    {
      __NET_HOOK_DEFINE();
      serverSocket.setReuseAddress(__NET_HOOK_INVOKE_MORE true);
    } 
    __OPENCODE_CATCH(SocketException & ex){}
          
  }
  __OPENCODE_CATCH(SocketException & ex)
  {
    StringBuffer sb;
    sb << "AcceptorJob::run|can not start AcceptorJob[" << address.toString() << "]" << ex.getMessage();
    System::loggingAndExit(sb.str(),-1);
    return;
  }

  int connectionLinger = __p_acceptor->getConnectionLinger(); 
  int connectionTimeout = __p_acceptor->getConnectionTimeout();
  bool tcpNoDelay = __p_acceptor->getTcpNoDelay();
  
  SocketHandler* __p_SocketHandler = __p_acceptor->getSocketHandler();
  
  while(true)
  {
    Socket socket;    
    serverSocket.accept(__NET_HOOK_INVOKE_MORE socket);
   
    if(shouldTerminate)
    {
      socket.close();
      break;
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
        
  }//~end while(true)
  
  __p_acceptor->__p_ServerSocket->close();
}

//------------------------------------------------------------------
//
//------------------------------------------------------------------
Acceptor*   Acceptor::createAcceptor(const Profile& profile)
{
  Properties props = profile.getProperties("ACCEPTOR_CONF");
  return Acceptor::createAcceptor(props);  
}
  
Acceptor*   Acceptor::createAcceptor(const Properties& props)
{
  Acceptor* __p_acceptor = new Acceptor();  
  
  string address = StringsKit::trim(props.getProperty("Address"));
  string port = StringsKit::trim(props.getProperty("Port"));
  string acceptCount = StringsKit::trim(props.getProperty("AcceptCount"));
  
  string connectionLinger = StringsKit::trim(props.getProperty("ConnectionLinger")); 
  string connectionTimeout = StringsKit::trim(props.getProperty("ConnectionTimeout"));
  string tcpNoDelay = StringsKit::trim(props.getProperty("TcpNoDelay"));
  
  if(!address.empty())
  {
    __p_acceptor->setNameOrIp(address);
  }
  
  if(!port.empty())
  {
    __p_acceptor->setPort(Number::parseInt(port));
  }
  
  if(!acceptCount.empty())
  {
    __p_acceptor->setAcceptCount(Number::parseInt(acceptCount));
  }    
  
  if(!connectionLinger.empty())
  {
    __p_acceptor->setConnectionLinger(Number::parseInt(connectionLinger));
  }
  
  if(!connectionTimeout.empty())
  {
    __p_acceptor->setConnectionTimeout(Number::parseInt(connectionTimeout)); 
  }
  
  if(!tcpNoDelay.empty())
  {
    __p_acceptor->setTcpNoDelay(BooleanComponent::valueOf(tcpNoDelay));
  }

  return __p_acceptor;
}

//------------------------------------------------------------------
//
//------------------------------------------------------------------

Acceptor::~Acceptor()
{
  delete __p_ServerSocket;
}

Acceptor::Acceptor(SocketHandler* socketHandler)
:__p_SocketHandler(socketHandler)
{
  __p_ServerSocket = 0;
  port = 0 ;
  acceptCount = 10;

  connectionLinger = -1;
  connectionTimeout = 0;
  tcpNoDelay = true;
}

SocketHandler* Acceptor::getSocketHandler()
{
  return __p_SocketHandler;
}

SocketHandler* Acceptor::setSocketHandler(SocketHandler* socketHandler)
{
  SocketHandler* __tmp = this->__p_SocketHandler;
  this->__p_SocketHandler = socketHandler;
  return __tmp;
}

//------------------------------------------------------------------
//
//------------------------------------------------------------------
int Acceptor::getPort() const
{
  return port;
}


void Acceptor::setPort(int portValue)
{
  this->port = portValue;
}


const string& Acceptor::getNameOrIp() const
{
  return address;
}


void   Acceptor::setNameOrIp(const string& addressValue)
{
  this->address = addressValue;
}


int Acceptor::getAcceptCount() const
{
  return acceptCount;
}


void Acceptor::setAcceptCount(int countValue)
{
  this->acceptCount = countValue;
}

int Acceptor::getConnectionLinger() const
{
  return connectionLinger;
}


void Acceptor::setConnectionLinger(int connectionLingerValue)
{
  this->connectionLinger = connectionLingerValue;
}


int Acceptor::getConnectionTimeout() const
{
  return connectionTimeout;
}


void Acceptor::setConnectionTimeout(int connectionTimeoutValue)
{
  this->connectionTimeout = connectionTimeoutValue;
}


bool Acceptor::getTcpNoDelay() const
{
  return tcpNoDelay;
}


void Acceptor::setTcpNoDelay(bool tcpNoDelayValue)
{
  this->tcpNoDelay = tcpNoDelayValue;
}

__OPENCODE_END_NAMESPACE
