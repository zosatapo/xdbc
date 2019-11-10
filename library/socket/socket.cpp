#include "socket/socket.h"
#include "socket/socketAddress.h"
#include "socket/socketException.h"
#include "socket/socketImpl.h"
#include "socket/plainSocketImpl.h"
 
#include  "port/ioc_sbuf.h"

__OPENCODE_BEGIN_NAMESPACE

//--------------------------------------------------------------------
// Socket Ctor
//--------------------------------------------------------------------
Socket::~Socket() 
{
  if(p_impl != 0)
  {
    delete p_impl;
  }
}

Socket::Socket() 
{
  created = false;
  bound = false;
  connected = false;
  closed = false;
  shutIn = false;
  shutOut = false;
 
  accepted = false;

  p_impl = 0;
}

Socket::Socket(SocketImpl* impl) 
{
  created = false;
  bound = false;
  connected = false;
  closed = false;
  shutIn = false;
  shutOut = false;

  accepted = false;

  this->p_impl = impl;
  if (impl != 0) 
  {
    this->p_impl->setSocket(this);
  }
}
  
Socket::Socket(__NET_HOOK_DECLARE_MORE const string& host, int port) __NET_THROW_DECLARE(SocketException)
{
  created = false;
  bound = false;
  connected = false;
  closed = false;
  shutIn = false;
  shutOut = false;

  accepted = false;

  p_impl = 0;

  connect(__NET_HOOK_INVOKE_MORE host,port);
}


Socket::Socket(__NET_HOOK_DECLARE_MORE const InetAddress& address, int port)  __NET_THROW_DECLARE(SocketException)
{
  created = false;
  bound = false;
  connected = false;
  closed = false;
  shutIn = false;
  shutOut = false;

  accepted = false;

  p_impl = 0;

  connect(__NET_HOOK_INVOKE_MORE address,port);
}

Socket::Socket(__NET_HOOK_DECLARE_MORE const string& host, int port, const InetAddress& localAddr,int localPort)  __NET_THROW_DECLARE(SocketException)
{
  created = false;
  bound = false;
  connected = false;
  closed = false;
  shutIn = false;
  shutOut = false;

  accepted = false;

  p_impl = 0;

  bind(__NET_HOOK_INVOKE_MORE localAddr,localPort);
  __NET_HOOK_CHECK(return);

  connect(__NET_HOOK_INVOKE_MORE host,port);
}

Socket::Socket(__NET_HOOK_DECLARE_MORE const InetAddress& address, int port, const InetAddress& localAddr,int localPort)  __NET_THROW_DECLARE(SocketException)
{
  created = false;
  bound = false;
  connected = false;
  closed = false;
  shutIn = false;
  shutOut = false;

  accepted = false;

  p_impl = 0;

  bind(__NET_HOOK_INVOKE_MORE localAddr,localPort);
  __NET_HOOK_CHECK(return);

  connect(__NET_HOOK_INVOKE_MORE address,port);  
}

//------------------------------------------------------------------------
//Copy Ctor
//------------------------------------------------------------------------ 
Socket::Socket(const Socket& rhs)
{
  created = rhs.created;
  bound = rhs.bound;
  connected = rhs.connected;
  closed = rhs.closed;
  shutIn = rhs.closed;
  shutOut = rhs.closed;

  accepted = rhs.accepted;

  if(rhs.p_impl != 0)
  {
    p_impl = rhs.p_impl->clone() ; 
    p_impl->setSocket(this);
  }
}

Socket& Socket::operator= (const Socket& rhs)
{
  created = rhs.created;
  bound = rhs.bound;
  connected = rhs.connected;
  closed = rhs.closed;
  shutIn = rhs.closed;
  shutOut = rhs.closed;

  accepted = rhs.accepted;
  
  if(p_impl != 0)
  {
    delete p_impl;
    p_impl = 0;
  }
 
  if(rhs.p_impl != 0)
  {
    p_impl = rhs.p_impl->clone() ;
    p_impl->setSocket(this);
  }
  
  return *this;
}

bool Socket::operator== (const Socket& rhs)
{
  if(p_impl != 0 && rhs.p_impl !=0)
  {
    return (p_impl->getSocketDescriptor().getDescriptor() == rhs.p_impl->getSocketDescriptor().getDescriptor());    
  }

  return (p_impl == rhs.p_impl);
}  

bool Socket::isAccepted() const
{
  return accepted;
}

void  Socket::setAccepted(bool acceptedValue)
{
  this->accepted = acceptedValue;
}

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------ 
void Socket::connect(__NET_HOOK_DECLARE_MORE const string& host, int port)  __NET_THROW_DECLARE(SocketException)
{
  connect(__NET_HOOK_INVOKE_MORE InetAddress::getByName(__NET_HOOK_INVOKE_MORE host),port);
}

void Socket::connect(__NET_HOOK_DECLARE_MORE const InetAddress& address, int port)  __NET_THROW_DECLARE(SocketException)
{
  SocketAddress addr(address,port);
  connect(__NET_HOOK_INVOKE_MORE addr);
}
 
void Socket::connect(__NET_HOOK_DECLARE_MORE const SocketAddress& endpoint)  __NET_THROW_DECLARE(SocketException) 
{
  connect(__NET_HOOK_INVOKE_MORE endpoint, 0);
}

void Socket::connect(__NET_HOOK_DECLARE_MORE const SocketAddress& endpoint, int timeout)  __NET_THROW_DECLARE(SocketException) 
{  
  if (timeout < 0)
  {
     __NET_HOOK_RETURN(return,SocketException,"connect: timeout can't be negative");
  }
  
  if (isClosed())
  {
    __NET_HOOK_RETURN(return,SocketException,"Socket is closed");
  }
  
  if (isConnected())
  {
     __NET_HOOK_RETURN(return,SocketException,"already connected");
  }
  
  if(timeout > 0)
  {
    getImpl(__NET_HOOK_INVOKE_ONCE)->connect(__NET_HOOK_INVOKE_MORE endpoint, timeout);
  }
  else
  {
    getImpl(__NET_HOOK_INVOKE_ONCE)->connect(__NET_HOOK_INVOKE_MORE endpoint.getInetAddress(), endpoint.getPort());
  }
 
  connected = true;
  /*
  * If the socket was not bound before the connect, it is now because
  * the kernel will have picked an ephemeral port & a local address
  */
  bound = true;
}
  
void Socket::bind(__NET_HOOK_DECLARE_MORE const SocketAddress& bindpoint)  __NET_THROW_DECLARE(SocketException) 
{ 
  bind(__NET_HOOK_INVOKE_MORE bindpoint.getInetAddress(),bindpoint.getPort());
}

void Socket::bind(__NET_HOOK_DECLARE_MORE const InetAddress& host, int port)  __NET_THROW_DECLARE(SocketException)
{
  if (isClosed())
  {
    __NET_HOOK_RETURN(return,SocketException,"Socket is closed"); 
  }
  
  if (isBound())
  { 
     __NET_HOOK_RETURN(return,SocketException,"Already bound"); 
  }
 
  getImpl(__NET_HOOK_INVOKE_ONCE)->bind(__NET_HOOK_INVOKE_MORE host,port);
  
  bound = true;
}

void Socket::shutdownInput()
{
  if (isClosed())
  { 
    return;
  }
  
  if (!isConnected())
  {
    return;
  }
  
  if (isInputShutdown())
  { 
    return;
  }
 
  __NET_HOOK_DEFINE(); 
  getImpl(__NET_HOOK_INVOKE_ONCE)->shutdownInput();
  shutIn = true;
}

void Socket::shutdownOutput()
{
  if (isClosed())
  { 
    return;
  }
  
  if (!isConnected())
  {
    return;
  }
  
  if (isOutputShutdown())
  { 
    return;
  }
  
  __NET_HOOK_DEFINE(); 
  getImpl(__NET_HOOK_INVOKE_ONCE)->shutdownOutput();
  shutOut = true;
}

void Socket::close()
{
  if (isClosed())
  {
    return;
  }
  
  if (created)
  {
    p_impl->close();
  }
  closed = true;
}  
//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------

void Socket::sendUrgentData (__NET_HOOK_DECLARE_MORE int data)  __NET_THROW_DECLARE(SocketException)  
{
  if (!getImpl(__NET_HOOK_INVOKE_ONCE)->supportsUrgentData ()) 
  {
     __NET_HOOK_RETURN(return,SocketException,"Urgent data not supported");
  }

  getImpl(__NET_HOOK_INVOKE_ONCE)->sendUrgentData (__NET_HOOK_INVOKE_MORE data);
} 

int Socket::read(__NET_HOOK_DECLARE_MORE char* b, int length)  __NET_THROW_DECLARE(SocketException)
{
  if (isClosed())
  { 
    __NET_HOOK_RETURN(return -1,SocketException,"Socket is closed");
  }
  
  if (!isConnected())
  {
     __NET_HOOK_RETURN(return -1,SocketException,"Socket is not connected");
  }
  
  if (isInputShutdown())
  { 
     __NET_HOOK_RETURN(return -1,SocketException,"Socket input is already shutdown");
  }
  
  return getImpl(__NET_HOOK_INVOKE_ONCE)->read(__NET_HOOK_INVOKE_MORE b,length);    
}

int Socket::peek(__NET_HOOK_DECLARE_MORE char* b, int length)  __NET_THROW_DECLARE(SocketException)
{
  if (isClosed())
  { 
    __NET_HOOK_RETURN(return -1,SocketException,"Socket is closed"); 
  }
  
  if (!isConnected())
  {
    __NET_HOOK_RETURN(return -1,SocketException,"Socket is not connected"); 
  }
  
  if (isInputShutdown())
  { 
     __NET_HOOK_RETURN(return -1,SocketException,"Socket input is already shutdown"); 
  }

  return getImpl(__NET_HOOK_INVOKE_ONCE)->peek(__NET_HOOK_INVOKE_MORE b,length);    
}
  
void Socket::write(__NET_HOOK_DECLARE_MORE const char* b, int length)  __NET_THROW_DECLARE(SocketException)
{
  if (isClosed())
  { 
    __NET_HOOK_RETURN(return,SocketException,"Socket is closed"); 
  }
  
  if (!isConnected())
  {
     __NET_HOOK_RETURN(return,SocketException,"Socket is not connected"); 
  }
  
  if (isOutputShutdown())
  { 
     __NET_HOOK_RETURN(return,SocketException,"Socket output is already shutdown"); 
  }
  
  return getImpl(__NET_HOOK_INVOKE_ONCE)->write(__NET_HOOK_INVOKE_MORE b,length);     
}  

int Socket::jread(__NET_HOOK_DECLARE_MORE Socket& socket,char* b,int& length)  __NET_THROW_DECLARE(SocketException)
{
  int __size = length;

  if(__size <= 0)
  {
    return 0;
  }

  int __bytesRead = 0;
  int __bytesCount = 0;
 
  while(__size > 0)
  {
    __bytesRead  = socket.read(__NET_HOOK_INVOKE_MORE b,__size); 
    __NET_HOOK_CHECK(return __bytesCount);
    
    if(__bytesRead <= -1)
    {
      length = __bytesCount;
      return __bytesRead;

    }//~end if(__bytesRead <= -1)

    
    __bytesCount += __bytesRead;  
    __size -= __bytesRead;  
    
    b += __bytesRead;

    length = __bytesCount;
 
  }//~end while(__size > 0)

  length = __bytesCount;

  return __bytesCount;
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------

InetAddress Socket::getInetAddress() const
{
  if (!isConnected())
  {
    return InetAddress::nullAddress();
  }

  __OPENCODE_TRY 
  {
    __NET_HOOK_DEFINE();
    return getImpl(__NET_HOOK_INVOKE_ONCE)->getInetAddress();
  } 
  __OPENCODE_CATCH (SocketException& e) 
  {
  }
  
  return InetAddress::nullAddress();;
}

InetAddress Socket::getLocalAddress() const 
{
  // This is for backward compatibility
  if (!isBound())
  {
    return InetAddress::anyAddress();
  }

  __OPENCODE_TRY 
  {
    __NET_HOOK_DEFINE();
    SocketOptionsValue val = getImpl(__NET_HOOK_INVOKE_ONCE)->getOption(__NET_HOOK_INVOKE_MORE SocketOptions::SO_W_BINDADDR);
    return val.sov_inet_value;
  } 
  __OPENCODE_CATCH (SocketException& e) 
  {
  }
  
  return InetAddress::anyAddress();
}

int Socket::getPort() const
{
  if (!isConnected())
  { 
    return 0;
  }

  __OPENCODE_TRY 
  {
    __NET_HOOK_DEFINE();
    return getImpl(__NET_HOOK_INVOKE_ONCE)->getPort();
  } 
  __OPENCODE_CATCH (SocketException& e) 
  {
    // Shouldn't happen as we're connected
  }
  
  return -1;
}

int Socket::getLocalPort() const 
{
  if (!isBound())
  { 
    return -1;
  }

  __OPENCODE_TRY 
  {
    __NET_HOOK_DEFINE();
    return getImpl(__NET_HOOK_INVOKE_ONCE)->getLocalPort();   
  } 
  __OPENCODE_CATCH(SocketException& e) 
  {
    // shouldn't happen as we're bound
  }
  
  return -1;
}

//-------------------------------------------------------------------------
//
//------------------------------------------------------------------------- 
bool Socket::isConnected()const 
{
  return connected;
}

bool Socket::isBound() const
{
  return bound;
}

bool Socket::isInputShutdown() const 
{
  return shutIn;
}

bool Socket::isOutputShutdown() const
{
  return shutOut;
}

bool Socket::isClosed() const
{
  return closed;
}

string Socket::toString() const
{
  __OPENCODE_TRY 
  { 
    if (isConnected())
    {
      __NET_HOOK_DEFINE();

      StringBuffer sb;
      sb.append("Socket[addr=");
      sb.append(InetAddress::getDotAddress(getImpl(__NET_HOOK_INVOKE_ONCE)->getInetAddress()));
      sb.append(",port=");
      sb.append(getImpl(__NET_HOOK_INVOKE_ONCE)->getPort());
      sb.append(",localport=");
      sb.append(getImpl(__NET_HOOK_INVOKE_ONCE)->getLocalPort());
      sb.append("]");
      
      return sb.str();
    }     
  } 
  __OPENCODE_CATCH (SocketException e) 
  {
  }
 
  return "Socket[unconnected]";
}
  
//--------------------------------------------------------------------
// Socket Options Support
//--------------------------------------------------------------------
void Socket::setTcpNoDelay(__NET_HOOK_DECLARE_MORE bool on)  __NET_THROW_DECLARE(SocketException) 
{
  if (isClosed())
  {
    __NET_HOOK_RETURN(return,SocketException,"Socket is closed"); 
  }
  
  SocketOptionsValue sov;
  sov.sov_type      = SocketOptionsValue::SOV_BOOLEAN;
  sov.sov_bol_value = on;
  
  getImpl(__NET_HOOK_INVOKE_ONCE)->setOption(__NET_HOOK_INVOKE_MORE SocketOptions::SO_W_TCP_NODELAY, sov);
}

bool Socket::getTcpNoDelay(__NET_HOOK_DECLARE_ONCE) const  __NET_THROW_DECLARE(SocketException) 
{
  if (isClosed())
  {
    __NET_HOOK_RETURN(return false,SocketException,"Socket is closed"); 
  }
  
  return ( getImpl(__NET_HOOK_INVOKE_ONCE)->getOption(__NET_HOOK_INVOKE_MORE SocketOptions::SO_W_TCP_NODELAY)).sov_bol_value;
}

void Socket::setSoLinger(__NET_HOOK_DECLARE_MORE bool on, int linger)  __NET_THROW_DECLARE(SocketException) 
{
  if (isClosed())
  {
    __NET_HOOK_RETURN(return,SocketException,"Socket is closed"); 
  }
    
  if (!on) 
  {
    SocketOptionsValue sov;
    sov.sov_type      = SocketOptionsValue::SOV_BOOLEAN;
    sov.sov_bol_value = on;
  
    getImpl(__NET_HOOK_INVOKE_ONCE)->setOption(__NET_HOOK_INVOKE_MORE SocketOptions::SO_W_LINGER, sov);
  } 
  else 
  {
    if (linger < 0) 
    {
      __NET_HOOK_RETURN(return,SocketException,"invalid value for SO_W_LINGER");
    }
    
    if (linger > 65535)
    {
      linger = 65535;
    }
    
    SocketOptionsValue sov;
    sov.sov_type      = SocketOptionsValue::SOV_INTEGER;
    sov.sov_int_value = linger;
    
    getImpl(__NET_HOOK_INVOKE_ONCE)->setOption(__NET_HOOK_INVOKE_MORE SocketOptions::SO_W_LINGER, sov);
  }
}

int Socket::getSoLinger(__NET_HOOK_DECLARE_ONCE) const   __NET_THROW_DECLARE(SocketException) 
{
  if (isClosed())
  { 
    __NET_HOOK_RETURN(return -1,SocketException,"Socket is closed"); 
  }
  
  SocketOptionsValue value = getImpl(__NET_HOOK_INVOKE_ONCE)->getOption(__NET_HOOK_INVOKE_MORE SocketOptions::SO_W_LINGER);
  
  if(SocketOptionsValue::SOV_BOOLINT == value.sov_type)
  {
    return value.sov_int_value;
  }

  return -1;
}

void Socket::setOOBInline(__NET_HOOK_DECLARE_MORE bool on)  __NET_THROW_DECLARE(SocketException) 
{
  if (isClosed())
  { 
    __NET_HOOK_RETURN(return,SocketException,"Socket is closed"); 
  }
  
  SocketOptionsValue sov;
  sov.sov_type      = SocketOptionsValue::SOV_BOOLEAN;
  sov.sov_bol_value = on;    
  getImpl(__NET_HOOK_INVOKE_ONCE)->setOption(__NET_HOOK_INVOKE_MORE SocketOptions::SO_W_OOBINLINE, sov);
}

bool Socket::getOOBInline(__NET_HOOK_DECLARE_ONCE) const  __NET_THROW_DECLARE(SocketException) 
{
  if (isClosed())
  {
    __NET_HOOK_RETURN(return false,SocketException,"Socket is closed"); 
  }
  return (getImpl(__NET_HOOK_INVOKE_ONCE)->getOption(__NET_HOOK_INVOKE_MORE SocketOptions::SO_W_OOBINLINE)).sov_bol_value;
}

void Socket::setSoTimeout(__NET_HOOK_DECLARE_MORE int timeout)  __NET_THROW_DECLARE(SocketException) 
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

int Socket::getSoTimeout(__NET_HOOK_DECLARE_ONCE) const   __NET_THROW_DECLARE(SocketException) 
{
  if (isClosed())
  {  
    __NET_HOOK_RETURN(return -1,SocketException,"Socket is closed"); 
  }
  
  return (getImpl(__NET_HOOK_INVOKE_ONCE)->getOption(__NET_HOOK_INVOKE_MORE SocketOptions::SO_W_TIMEOUT)).sov_int_value;    
}

void Socket::setSendBufferSize(__NET_HOOK_DECLARE_MORE int size) __NET_THROW_DECLARE(SocketException)
{
  if (!(size > 0)) 
  {
    __NET_HOOK_RETURN(return,SocketException,"negative send size");       
  }
  if (isClosed())
  { 
    __NET_HOOK_RETURN(return,SocketException,"Socket is closed");   
  }
  
  SocketOptionsValue sov;
  sov.sov_type      = SocketOptionsValue::SOV_INTEGER;
  sov.sov_int_value = size;     
  getImpl(__NET_HOOK_INVOKE_ONCE)->setOption(__NET_HOOK_INVOKE_MORE SocketOptions::SO_W_SNDBUF, sov);
}

int Socket::getSendBufferSize(__NET_HOOK_DECLARE_ONCE) const  __NET_THROW_DECLARE(SocketException) 
{
  if (isClosed())
  { 
    __NET_HOOK_RETURN(return -1,SocketException,"Socket is closed");   
  }
  return (getImpl(__NET_HOOK_INVOKE_ONCE)->getOption(__NET_HOOK_INVOKE_MORE SocketOptions::SO_W_SNDBUF)).sov_int_value;
}

void Socket::setReceiveBufferSize(__NET_HOOK_DECLARE_MORE int size) __NET_THROW_DECLARE(SocketException)
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

int Socket::getReceiveBufferSize(__NET_HOOK_DECLARE_ONCE) const  __NET_THROW_DECLARE(SocketException)
{
  if (isClosed())
  { 
    __NET_HOOK_RETURN(return -1,SocketException,"Socket is closed");
  }
  return  (getImpl(__NET_HOOK_INVOKE_ONCE)->getOption(__NET_HOOK_INVOKE_MORE SocketOptions::SO_W_RCVBUF)).sov_int_value;
}

void Socket::setKeepAlive(__NET_HOOK_DECLARE_MORE bool on)  __NET_THROW_DECLARE(SocketException) 
{
  if (isClosed())
  {
    __NET_HOOK_RETURN(return,SocketException,"Socket is closed");
  }

  SocketOptionsValue sov;
  sov.sov_type      = SocketOptionsValue::SOV_BOOLEAN;
  sov.sov_bol_value = on;
      
  getImpl(__NET_HOOK_INVOKE_ONCE)->setOption(__NET_HOOK_INVOKE_MORE SocketOptions::SO_W_KEEPALIVE, sov);
}

bool Socket::getKeepAlive(__NET_HOOK_DECLARE_ONCE) const   __NET_THROW_DECLARE(SocketException) 
{
  if (isClosed())
  { 
    __NET_HOOK_RETURN(return false,SocketException,"Socket is closed");
  }
  
  return (getImpl(__NET_HOOK_INVOKE_ONCE)->getOption(__NET_HOOK_INVOKE_MORE SocketOptions::SO_W_KEEPALIVE)).sov_bol_value;
}

void Socket::setTrafficClass(__NET_HOOK_DECLARE_MORE int tc)  __NET_THROW_DECLARE(SocketException) 
{
  if (tc < 0 || tc > 255)
  { 
    __NET_HOOK_RETURN(return,SocketException,"tc is not in range 0 -- 255");    
  }
  
  if (isClosed())
  {
    __NET_HOOK_RETURN(return,SocketException,"Socket is closed");
  }    

  SocketOptionsValue sov;
  sov.sov_type      = SocketOptionsValue::SOV_INTEGER;
  sov.sov_int_value = tc;
      
  getImpl(__NET_HOOK_INVOKE_ONCE)->setOption(__NET_HOOK_INVOKE_MORE SocketOptions::SO_W_IP_TOS, sov);
}

int Socket::getTrafficClass(__NET_HOOK_DECLARE_ONCE) const   __NET_THROW_DECLARE(SocketException) 
{
  return ((getImpl(__NET_HOOK_INVOKE_ONCE)->getOption(__NET_HOOK_INVOKE_MORE SocketOptions::SO_W_IP_TOS))).sov_int_value;
}

void Socket::setReuseAddress(__NET_HOOK_DECLARE_MORE bool on)  __NET_THROW_DECLARE(SocketException) 
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

bool Socket::getReuseAddress(__NET_HOOK_DECLARE_ONCE) const  __NET_THROW_DECLARE(SocketException) 
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

void Socket::createImpl(__NET_HOOK_DECLARE_MORE bool stream)  __NET_THROW_DECLARE(SocketException) 
{
  if(p_impl == 0)
  {
    p_impl = new PlainSocketImpl();
    p_impl->setSocket(this);
    p_impl->create(__NET_HOOK_INVOKE_MORE stream);
    created = true;   
  }
}


SocketImpl* Socket::getImpl(__NET_HOOK_DECLARE_ONCE) const  __NET_THROW_DECLARE(SocketException)
{
  if(!created )
  {
    Socket* __tcp_socket = (Socket*)(this);
    __tcp_socket->createImpl(__NET_HOOK_INVOKE_MORE true);
  }

  return p_impl;
}

void Socket::postAccept() 
{ 
  connected = true;
  created = true;
  bound = true;
}

void Socket::setCreated() 
{
  created = true;
}

void Socket::setBound() 
{
  bound = true;
}

void Socket::setConnected() 
{
  connected = true;
}

__OPENCODE_END_NAMESPACE
