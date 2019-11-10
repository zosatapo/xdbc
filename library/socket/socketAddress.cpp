#include "socket/socketAddress.h"

#include "port/ioc_sbuf.h"

USING_OPENCODE_STL_NS
__OPENCODE_BEGIN_NAMESPACE

SocketAddress::SocketAddress():int_port(-1)
{}

SocketAddress::SocketAddress(const InetAddress& inetAddress,int port):inetAddress(inetAddress),int_port(port)
{}

SocketAddress::SocketAddress(const SocketAddress& rv)
{
  inetAddress = rv.inetAddress;
  int_port = rv.int_port;  
}

SocketAddress& SocketAddress::operator=(const SocketAddress& rv)
{
  if(&rv == this)
  {
    return *this;
  }

  inetAddress = rv.inetAddress;
  int_port = rv.int_port;  

  return *this;
}

bool SocketAddress::operator==(const SocketAddress& rhs) const
{
  return (int_port == rhs.int_port) && (inetAddress == rhs.inetAddress);  
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
const InetAddress& SocketAddress::getInetAddress() const
{
  return inetAddress;
}

int SocketAddress::getPort() const
{
  return int_port;
}

string SocketAddress::toString() const
{
  StringBuffer sb;
  sb.append("SocketAddress[addr=");

  sb.append(getInetAddress().toString());
  sb.append(",port=");
  sb.append(getPort());

  sb.append("]");

  return sb.str(); 
}

__OPENCODE_END_NAMESPACE
