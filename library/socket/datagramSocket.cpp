#include "socket/datagramSocket.h"
#include "socket/socketAddress.h"
#include "socket/socketException.h"
#include "socket/plainDatagramSocketImpl.h"
#include "socket/socketUtils.h"
 
#include  "port/ioc_sbuf.h"

__OPENCODE_BEGIN_NAMESPACE

const int DatagramSocket::OPENCODE_ST_NOT_CONNECTED = 0;
const int DatagramSocket::OPENCODE_ST_CONNECTED = 1;
const int DatagramSocket::OPENCODE_ST_CONNECTED_NO_IMPL = 2;
  
//--------------------------------------------------------------------
// DatagramSocket Ctor
//--------------------------------------------------------------------
DatagramSocket::~DatagramSocket() 
{
  if(p_impl != 0)
  {
    delete p_impl;
  }
}

DatagramSocket::DatagramSocket()
{
  created = false;
  bound = false;
  closed = false;
  
  p_impl = 0; 
  connectState = OPENCODE_ST_NOT_CONNECTED;
  connectedPort = -1;
}

DatagramSocket::DatagramSocket(__NET_HOOK_DECLARE_MORE const SocketAddress& bindaddr)  __NET_THROW_DECLARE(SocketException)
{
  created = false;
  bound = false;
  closed = false;

  p_impl = 0; 
  connectState = OPENCODE_ST_NOT_CONNECTED;
  connectedPort = -1;
    
  bind(__NET_HOOK_INVOKE_MORE bindaddr);
}

DatagramSocket::DatagramSocket(__NET_HOOK_DECLARE_MORE int port)  __NET_THROW_DECLARE(SocketException)
{
  created = false;
  bound = false;
  closed = false;

  p_impl = 0; 
  connectState = OPENCODE_ST_NOT_CONNECTED;
  connectedPort = -1;
    
  bind(__NET_HOOK_INVOKE_MORE InetAddress::anyAddress(),port);
}

DatagramSocket::DatagramSocket(__NET_HOOK_DECLARE_MORE int port, const InetAddress& laddr)  __NET_THROW_DECLARE(SocketException)
{
  created = false;
  bound = false;
  closed = false;

  p_impl = 0; 
  connectState = OPENCODE_ST_NOT_CONNECTED;
  connectedPort = -1;
    
  bind(__NET_HOOK_INVOKE_MORE laddr,port);
}


//------------------------------------------------------------------------
//Copy Ctor
//------------------------------------------------------------------------ 
DatagramSocket::DatagramSocket(const DatagramSocket& rhs)
{
  created = rhs.created;
  bound = rhs.bound;
  closed = rhs.closed;

  connectState = rhs.connectState;
  connectedPort = rhs.connectedPort;
  connectedAddress = rhs.connectedAddress;

  p_impl = 0;

  if(rhs.p_impl != 0)
  {
    p_impl = rhs.p_impl->clone() ;
  }
}

DatagramSocket& DatagramSocket::operator= (const DatagramSocket& rhs)
{
  created = rhs.created;
  bound = rhs.bound;
  closed = rhs.closed;

  connectState = rhs.connectState;
  connectedPort = rhs.connectedPort;
  connectedAddress = rhs.connectedAddress;
 
  if(p_impl != 0)
  {
    delete p_impl;
    p_impl = 0;
  }

  if(rhs.p_impl != 0)
  {
    p_impl = rhs.p_impl->clone() ;
  }
  
  return *this;
}

//------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------
void DatagramSocket::bind(__NET_HOOK_DECLARE_MORE const SocketAddress& addr)  __NET_THROW_DECLARE(SocketException)
{
  getImpl(__NET_HOOK_INVOKE_ONCE)->bind(__NET_HOOK_INVOKE_MORE addr.getInetAddress(),addr.getPort());
}


void DatagramSocket::bind(__NET_HOOK_DECLARE_MORE const InetAddress& laddr,int lport)  __NET_THROW_DECLARE(SocketException)
{
  getImpl(__NET_HOOK_INVOKE_ONCE)->bind(__NET_HOOK_INVOKE_MORE laddr,lport);
}

void DatagramSocket::connect(__NET_HOOK_DECLARE_MORE const InetAddress& address, int port) __NET_THROW_DECLARE(SocketException)
{
  getImpl(__NET_HOOK_INVOKE_ONCE)->connect(__NET_HOOK_INVOKE_MORE address,port);
}

void DatagramSocket::connect(__NET_HOOK_DECLARE_MORE const SocketAddress& addr)  __NET_THROW_DECLARE(SocketException)
{
  getImpl(__NET_HOOK_INVOKE_ONCE)->connect(__NET_HOOK_INVOKE_MORE addr.getInetAddress(),addr.getPort());
}

void DatagramSocket::close()
{
  __NET_HOOK_DEFINE();
  getImpl(__NET_HOOK_INVOKE_ONCE)->close();
}

//------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------

bool DatagramSocket::isClosed() const
{
  return closed;
}

bool DatagramSocket::isBound() const
{
  return bound;
}

bool DatagramSocket::isConnected() const
{
  return connectState != OPENCODE_ST_NOT_CONNECTED;
}

//------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------


InetAddress DatagramSocket::getInetAddress() const
{
  return connectedAddress;
}


int DatagramSocket::getPort() const
{
  return connectedPort;  
}

InetAddress DatagramSocket::getLocalAddress() const
{
  if (isClosed())
  {
    return InetAddress::nullAddress();
  }

  __OPENCODE_TRY
  {
    __NET_HOOK_DEFINE();
    SocketOptionsValue sov = getImpl(__NET_HOOK_INVOKE_ONCE)->getOption(__NET_HOOK_INVOKE_MORE SocketOptions::SO_W_BINDADDR);
    return sov.sov_inet_value;
  }
  __OPENCODE_CATCH(SocketException& ex){}

  return InetAddress::anyAddress();
}

int DatagramSocket::getLocalPort() const
{
  if (isClosed())
  {
    return -1;
  }

  __OPENCODE_TRY
  {
    __NET_HOOK_DEFINE();
    return getImpl(__NET_HOOK_INVOKE_ONCE)->getLocalPort();
  }
  __OPENCODE_CATCH(SocketException& ex){}

  return 0;
}

string DatagramSocket::toString() const
{
  if (isConnected())
  {
    StringBuffer sb;
    sb.append("DatagramSocket(connected)[addr=");
    sb.append(InetAddress::getDotAddress(getInetAddress()));
    sb.append(",port=");
    sb.append(getPort());
    sb.append(",localport=");
    sb.append(getLocalPort());
    sb.append("]");

    return sb.str();
  }
  else if(isBound())
  {
    StringBuffer sb;
    sb.append("DatagramSocket(bound)[addr=");
    sb.append(InetAddress::getDotAddress(getInetAddress()));
    sb.append(",port=");
    sb.append(getPort());
    sb.append(",localaddr=");
    sb.append(InetAddress::getDotAddress(getLocalAddress()));
    sb.append(",localport=");
    sb.append(getLocalPort());
    sb.append("]");

    return sb.str();
  } 

  return "DatagramSocket[unbound]";
}
  
//------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------

int DatagramSocket::receive(__NET_HOOK_DECLARE_MORE SocketAddress* address,char* b, int length)  __NET_THROW_DECLARE(SocketException)
{
  return getImpl(__NET_HOOK_INVOKE_ONCE)->receive(__NET_HOOK_INVOKE_MORE address,b,length);
}

int DatagramSocket::peek(__NET_HOOK_DECLARE_MORE SocketAddress* address,char* b, int length)  __NET_THROW_DECLARE(SocketException)
{
  return getImpl(__NET_HOOK_INVOKE_ONCE)->peek(__NET_HOOK_INVOKE_MORE address,b,length);
}


void DatagramSocket::send(__NET_HOOK_DECLARE_MORE const SocketAddress* address,const char* b, int length)  __NET_THROW_DECLARE(SocketException)
{
  return getImpl(__NET_HOOK_INVOKE_ONCE)->send(__NET_HOOK_INVOKE_MORE address,b,length);
} 
     
//--------------------------------------------------------------------
// DatagramSocket Options Support
//--------------------------------------------------------------------

void DatagramSocket::setSoTimeout(__NET_HOOK_DECLARE_MORE int timeout)  __NET_THROW_DECLARE(SocketException) 
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

int DatagramSocket::getSoTimeout(__NET_HOOK_DECLARE_ONCE) const  __NET_THROW_DECLARE(SocketException) 
{
  if (isClosed())
  {  
    __NET_HOOK_RETURN(return -1,SocketException,"Socket is closed");
  }
  
  return (getImpl(__NET_HOOK_INVOKE_ONCE)->getOption(__NET_HOOK_INVOKE_MORE SocketOptions::SO_W_TIMEOUT)).sov_int_value;    
}

void DatagramSocket::setSendBufferSize(__NET_HOOK_DECLARE_MORE int size) __NET_THROW_DECLARE(SocketException)
{
  if (size <= 0) 
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

int DatagramSocket::getSendBufferSize(__NET_HOOK_DECLARE_ONCE) const  __NET_THROW_DECLARE(SocketException) 
{
  if (isClosed())
  { 
    __NET_HOOK_RETURN(return -1,SocketException,"Socket is closed");
  }
  return (getImpl(__NET_HOOK_INVOKE_ONCE)->getOption(__NET_HOOK_INVOKE_MORE SocketOptions::SO_W_SNDBUF)).sov_int_value;
}

void DatagramSocket::setReceiveBufferSize(__NET_HOOK_DECLARE_MORE int size) __NET_THROW_DECLARE(SocketException)
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

int DatagramSocket::getReceiveBufferSize(__NET_HOOK_DECLARE_ONCE) const  __NET_THROW_DECLARE(SocketException)
{
  if (isClosed())
  { 
    __NET_HOOK_RETURN(return -1,SocketException,"Socket is closed");
  }
  return  (getImpl(__NET_HOOK_INVOKE_ONCE)->getOption(__NET_HOOK_INVOKE_MORE SocketOptions::SO_W_RCVBUF)).sov_int_value;
}

void DatagramSocket::setReuseAddress(__NET_HOOK_DECLARE_MORE bool on)  __NET_THROW_DECLARE(SocketException) 
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

bool DatagramSocket::getReuseAddress(__NET_HOOK_DECLARE_ONCE) const  __NET_THROW_DECLARE(SocketException) 
{
  if (isClosed())
  { 
    __NET_HOOK_RETURN(return false,SocketException,"Socket is closed");
  }
  
  return (getImpl(__NET_HOOK_INVOKE_ONCE)->getOption(__NET_HOOK_INVOKE_MORE SocketOptions::SO_W_REUSEADDR)).sov_bol_value;
}
  
void DatagramSocket::setTrafficClass(__NET_HOOK_DECLARE_MORE int tc)  __NET_THROW_DECLARE(SocketException) 
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

int DatagramSocket::getTrafficClass(__NET_HOOK_DECLARE_ONCE) const  __NET_THROW_DECLARE(SocketException) 
{
  return ((getImpl(__NET_HOOK_INVOKE_ONCE)->getOption(__NET_HOOK_INVOKE_MORE SocketOptions::SO_W_IP_TOS))).sov_int_value;
}

void DatagramSocket::setBroadcast(__NET_HOOK_DECLARE_MORE bool on)   __NET_THROW_DECLARE(SocketException) 
{
  if (isClosed())
  {
    __NET_HOOK_RETURN(return,SocketException,"Socket is closed");
  }

  SocketOptionsValue sov;
  sov.sov_type      = SocketOptionsValue::SOV_BOOLEAN;
  sov.sov_bol_value = on;
      
  getImpl(__NET_HOOK_INVOKE_ONCE)->setOption(__NET_HOOK_INVOKE_MORE SocketOptions::SO_W_BROADCAST, sov);
}

bool DatagramSocket::getBroadcast(__NET_HOOK_DECLARE_ONCE) const  __NET_THROW_DECLARE(SocketException) 
{
  if (isClosed())
  { 
    __NET_HOOK_RETURN(return false,SocketException,"Socket is closed");
  }
  
  return (getImpl(__NET_HOOK_INVOKE_ONCE)->getOption(__NET_HOOK_INVOKE_MORE SocketOptions::SO_W_BROADCAST)).sov_bol_value;
}

//------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------ 
void DatagramSocket::createImpl(__NET_HOOK_DECLARE_ONCE)  __NET_THROW_DECLARE(SocketException) 
{
  if(!created)
  {
    p_impl = new PlainDatagramSocketImpl();
    p_impl->create(__NET_HOOK_INVOKE_ONCE);
    created = true;   
  }
}

DatagramSocketImpl* DatagramSocket::getImpl(__NET_HOOK_DECLARE_ONCE) const   __NET_THROW_DECLARE(SocketException) 
{
  if (!created)
  {
    DatagramSocket* __udp_socket = (DatagramSocket*)(this);
    __udp_socket->createImpl(__NET_HOOK_INVOKE_ONCE);
  }

  return p_impl;
}

__OPENCODE_END_NAMESPACE
