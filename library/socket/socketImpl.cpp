#include "socket/socketImpl.h"
#include "socket/socket.h"
#include "socket/serverSocket.h"
#include "port/ioc_sbuf.h"
#include "port/libsys.h" 

USING_OPENCODE_STL_NS
__OPENCODE_BEGIN_NAMESPACE

//------------------------------------------------------------
// ctor and dector
//------------------------------------------------------------
SocketImpl::~SocketImpl(){}

SocketImpl::SocketImpl()
{
  p_socket = 0;
  p_serverSocket = 0;
  
  port = 0;
  localport = 0;  
  
  address = InetAddress::nullAddress(); 
}  

SocketImpl::SocketImpl(const SocketImpl& rv)
{
  p_socket = rv.p_socket;
  p_serverSocket = rv.p_serverSocket;
 
  fd = rv.fd;

  port = rv.port;
  localport = rv.localport;

  address = rv.address; 
}

SocketImpl& SocketImpl::operator=(const SocketImpl& rv)
{
  if(&rv == this)
  {
    return *this;
  }

  p_socket = rv.p_socket;
  p_serverSocket = rv.p_serverSocket;

  fd = rv.fd;

  port = rv.port;
  localport = rv.localport;

  address = rv.address;

  return *this;
}

SocketImpl* SocketImpl::clone() const
{
  //delegate to doClone
  SocketImpl* pClone = doClone();
  return pClone;
}

void SocketImpl::shutdownInput() 
{
}

void SocketImpl::shutdownOutput()
{
}  

bool SocketImpl::supportsUrgentData () 
{
  return false; // must be overridden in sub-class
}

string SocketImpl::toString() const
{
  StringBuffer sb;
  sb.append("Socket[addr=");
  sb.append(InetAddress::getDotAddress(getInetAddress()));
  sb.append(",port=");
  sb.append(getPort());
  sb.append(",localport=");
  sb.append(getLocalPort());
  sb.append("]");

  return sb.str();
}
    
const InetAddress& SocketImpl::getInetAddress() const 
{
  return address;
}

int SocketImpl::getPort() const 
{
  return port;
}

int SocketImpl::getLocalPort() const
{
  return localport;
}

const SocketDescriptor& SocketImpl::getSocketDescriptor() const
{
  return fd;
}

void SocketImpl::reset()
{
  address = InetAddress::nullAddress();
  port = 0;
  localport = 0;
  close();
}

//------------------------------------------------------------
// public method
//------------------------------------------------------------
void SocketImpl::setSocket(Socket* psoc)  
{
  this->p_socket = psoc;
}

Socket* SocketImpl::getSocket() 
{
  return p_socket;
}

void SocketImpl::setServerSocket(ServerSocket* psoc) 
{
  this->p_serverSocket = psoc;
}

ServerSocket* SocketImpl::getServerSocket() 
{
  return p_serverSocket;
} 

__OPENCODE_END_NAMESPACE

