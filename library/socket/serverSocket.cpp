#include "socket/serverSocket.h"
#include "socket/socketAddress.h"

#include "socket/socket.h"
#include "socket/plainSocketImpl.h"

#include "port/ioc_sbuf.h" 

__OPENCODE_BEGIN_NAMESPACE

//--------------------------------------------------------------------
// ServerSocket Ctor
//--------------------------------------------------------------------
ServerSocket::~ServerSocket() 
{
  if(p_impl != 0)
  {
    delete p_impl;
  }
}

ServerSocket::ServerSocket() 
{
  created = false;
  bound = false;
  closed = false;

  p_impl = 0;
  setImpl(); 
}

ServerSocket::ServerSocket(__NET_HOOK_DECLARE_MORE int port)  __NET_THROW_DECLARE(SocketException)
{
  created = false;
  bound = false;
  closed = false;

  p_impl = 0;
  setImpl();
  bind(__NET_HOOK_INVOKE_MORE InetAddress::getLocalHost(__NET_HOOK_INVOKE_ONCE),port);
}


ServerSocket::ServerSocket(__NET_HOOK_DECLARE_MORE int port, int backlog)  __NET_THROW_DECLARE(SocketException)
{
  created = false;
  bound = false;
  closed = false;

  p_impl = 0;
  setImpl();
  bind(__NET_HOOK_INVOKE_MORE InetAddress::getLocalHost(__NET_HOOK_INVOKE_ONCE),port,backlog);
}

ServerSocket::ServerSocket(__NET_HOOK_DECLARE_MORE int port, int backlog, const InetAddress& bindAddr)  __NET_THROW_DECLARE(SocketException)
{
  created = false;
  bound = false;
  closed = false;

  p_impl = 0;
  setImpl();
  bind(__NET_HOOK_INVOKE_MORE bindAddr,port,backlog);
}


//------------------------------------------------------------------------
//Copy Ctor
//------------------------------------------------------------------------ 
ServerSocket::ServerSocket(const ServerSocket& rhs)
{
  created = rhs.created;
  bound = rhs.bound;
  closed = rhs.closed;

  p_impl = 0;

  if(rhs.p_impl != 0)
  {
    p_impl = rhs.p_impl->clone();
    p_impl->setServerSocket(this);
  }
}

ServerSocket& ServerSocket::operator= (const ServerSocket& rhs)
{
  created = rhs.created;
  bound = rhs.bound;
  closed = rhs.closed;
 
  if(p_impl != 0)
  {
    delete p_impl;
    p_impl = 0;
  }

  if(rhs.p_impl != 0)
  {
    p_impl = rhs.p_impl->clone();
    p_impl->setServerSocket(this);
  }

  return *this;
}

bool ServerSocket::operator== (const ServerSocket& rhs)
{
  if(p_impl != 0 && rhs.p_impl !=0)
  {
    return (p_impl->getSocketDescriptor().getDescriptor() == rhs.p_impl->getSocketDescriptor().getDescriptor());
  }

  return (p_impl == rhs.p_impl);
}

//--------------------------------------------------------------------
// ServerSocket Options Support
//--------------------------------------------------------------------
void ServerSocket::bind(__NET_HOOK_DECLARE_MORE const SocketAddress& endpoint)  __NET_THROW_DECLARE(SocketException)
{
  bind(__NET_HOOK_INVOKE_MORE endpoint.getInetAddress(),endpoint.getPort());
}

void ServerSocket::bind(__NET_HOOK_DECLARE_MORE const InetAddress& host, int port)  __NET_THROW_DECLARE(SocketException)
{
  bind(__NET_HOOK_INVOKE_MORE host,port,50);
}

  
void ServerSocket::bind(__NET_HOOK_DECLARE_MORE const SocketAddress& endpoint, int backlog)  __NET_THROW_DECLARE(SocketException)
{
  bind(__NET_HOOK_INVOKE_MORE endpoint.getInetAddress(),endpoint.getPort(),backlog);
}

void ServerSocket::bind(__NET_HOOK_DECLARE_MORE const InetAddress& host, int port, int backlog)  __NET_THROW_DECLARE(SocketException)
{
  if (isClosed())
  {
    __NET_HOOK_RETURN(return,SocketException,"Socket is closed");
  }
  
  if (isBound())
  {
     __NET_HOOK_RETURN(return,SocketException,"Already bound");
  }

  if (port < 0 || port > 0xFFFF)
  {
    StringBuffer __sb;
    __sb << "Port value out of range: " << port;
     __NET_HOOK_RETURN(return,SocketException,__sb.str() );
  }
      
  if (backlog < 1)
  {
    backlog = 50;  
  }

  createImpl(__NET_HOOK_INVOKE_ONCE);
  __NET_HOOK_CHECK(return);
  
  getImpl(__NET_HOOK_INVOKE_ONCE)->bind(__NET_HOOK_INVOKE_MORE host, port);
  __NET_HOOK_CHECK(return);
  
  getImpl(__NET_HOOK_INVOKE_ONCE)->listen(__NET_HOOK_INVOKE_MORE backlog);
  __NET_HOOK_CHECK(return);
  
  this->localaddress = host;
      
  bound = true;
}

void ServerSocket::accept(__NET_HOOK_DECLARE_MORE Socket& s)  __NET_THROW_DECLARE(SocketException) 
{
  if (isClosed())
  { 
    __NET_HOOK_RETURN(return,SocketException,"Socket is closed");
  }
  
  if (!isBound())
  {
     __NET_HOOK_RETURN(return,SocketException,"Socket is not bound yet");
  }
  
  SocketImpl* si = s.getImpl();  
  getImpl(__NET_HOOK_INVOKE_ONCE)->accept(__NET_HOOK_INVOKE_MORE si);
  s.postAccept();
  s.setAccepted(true);
}


void ServerSocket::close() 
{
  if (isClosed())
  {
    return;
  }
  if (created)
  {
    __NET_HOOK_DEFINE();
    getImpl(__NET_HOOK_INVOKE_ONCE)->close();
  }
  closed = true;
}

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------ 
InetAddress ServerSocket::getInetAddress() const
{
  if (!isBound())
  {
    return InetAddress::nullAddress();
  }
      
  return localaddress;;    
}
  

int ServerSocket::getLocalPort() const
{
  if (!isBound())
  { 
    return 0;
  }
  
  __OPENCODE_TRY 
  { 
    __NET_HOOK_DEFINE();
    return getImpl(__NET_HOOK_INVOKE_ONCE)->getLocalPort();
  } 
  __OPENCODE_CATCH (SocketException& e) 
  {
    // Shouldn't happen as we're connected
  }
  
  return -1;  
}
    

bool ServerSocket::isBound() const
{
  return bound ;
}

bool ServerSocket::isClosed() const
{
  return closed;
}

string ServerSocket::toString() const
{
  if (!isBound())
  {
    return "ServerSocket[unbound]";
  }
  
  StringBuffer sb;
  sb.append("ServerSocket[localaddr=");
  sb.append(InetAddress::getDotAddress(getInetAddress()));
  sb.append(",localport=");
  sb.append(getLocalPort());
  sb.append("]");

  return sb.str();
}
    
//--------------------------------------------------------------------
// ServerSocket Options Support
//--------------------------------------------------------------------

void ServerSocket::setSoTimeout(__NET_HOOK_DECLARE_MORE int timeout)  __NET_THROW_DECLARE(SocketException) 
{
  if (timeout < 0)
  {
     __NET_HOOK_RETURN(return,SocketException,"timeout can't be negative");    
  }
  
  if (isClosed())
  { 
    __NET_HOOK_RETURN(return,SocketException,"Socket is closed");
  }

  SocketOptionsValue sov;
  sov.sov_type      = SocketOptionsValue::SOV_INTEGER;
  sov.sov_int_value = timeout;   
   
  getImpl(__NET_HOOK_INVOKE_ONCE)->setOption(__NET_HOOK_INVOKE_MORE SocketOptions::SO_W_TIMEOUT, sov);
}

int ServerSocket::getSoTimeout(__NET_HOOK_DECLARE_ONCE) const  __NET_THROW_DECLARE(SocketException) 
{
  if (isClosed())
  {  
    __NET_HOOK_RETURN(return -1,SocketException,"Socket is closed");
  }
  
  return (getImpl(__NET_HOOK_INVOKE_ONCE)->getOption(__NET_HOOK_INVOKE_MORE SocketOptions::SO_W_TIMEOUT)).sov_int_value;    
}

void ServerSocket::setReceiveBufferSize(__NET_HOOK_DECLARE_MORE int size) __NET_THROW_DECLARE(SocketException)
{
  if (size <= 0) 
  {
     __NET_HOOK_RETURN(return,SocketException,"invalid receive size");
  }
  
  if (isClosed())
  {
    __NET_HOOK_RETURN(return,SocketException,"Socket is closed");
  }
  
  SocketOptionsValue sov;
  sov.sov_type      = SocketOptionsValue::SOV_INTEGER;
  sov.sov_int_value = size;     
  getImpl(__NET_HOOK_INVOKE_ONCE)->setOption(__NET_HOOK_INVOKE_MORE SocketOptions::SO_W_RCVBUF, sov);
}

int ServerSocket::getReceiveBufferSize(__NET_HOOK_DECLARE_ONCE) const  __NET_THROW_DECLARE(SocketException)
{
  if (isClosed())
  { 
    __NET_HOOK_RETURN(return -1,SocketException,"Socket is closed");
  }
  
  return  (getImpl(__NET_HOOK_INVOKE_ONCE)->getOption(__NET_HOOK_INVOKE_MORE SocketOptions::SO_W_RCVBUF)).sov_int_value;
}


void ServerSocket::setReuseAddress(__NET_HOOK_DECLARE_MORE bool on)  __NET_THROW_DECLARE(SocketException) 
{
  if (isClosed())
  {
    __NET_HOOK_RETURN(return,SocketException,"Socket is closed");
  }

  SocketOptionsValue sov;
  sov.sov_type      = SocketOptionsValue::SOV_BOOLEAN;
  sov.sov_bol_value = on;
      
  getImpl(__NET_HOOK_INVOKE_ONCE)->setOption(__NET_HOOK_INVOKE_MORE SocketOptions::SO_W_REUSEADDR, sov);
}

bool ServerSocket::getReuseAddress(__NET_HOOK_DECLARE_ONCE) const  __NET_THROW_DECLARE(SocketException) 
{
  if (isClosed())
  { 
    __NET_HOOK_RETURN(return false,SocketException,"Socket is closed");
  }
  
  return (getImpl(__NET_HOOK_INVOKE_ONCE)->getOption(__NET_HOOK_INVOKE_MORE SocketOptions::SO_W_REUSEADDR)).sov_bol_value;
}

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------ 

SocketImpl* ServerSocket::getImpl(__NET_HOOK_DECLARE_ONCE) const  __NET_THROW_DECLARE(SocketException) 
{
  return p_impl;
}

void ServerSocket::setImpl() 
{
  p_impl = new PlainSocketImpl();
  p_impl->setServerSocket(this);
}

void ServerSocket::createImpl(__NET_HOOK_DECLARE_ONCE)  __NET_THROW_DECLARE(SocketException) 
{
  if(!created)
  {
    p_impl->create(__NET_HOOK_INVOKE_MORE true);
    __NET_HOOK_CHECK(return);
    created = true;
  }
}
  
//------------------------------------------------------------------------
//
//------------------------------------------------------------------------ 
void ServerSocket::setBound() 
{
  bound = true;
}

void ServerSocket::setCreated() 
{
  created = true;
}

__OPENCODE_END_NAMESPACE
