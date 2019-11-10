#include "socket/datagramSocketImpl.h"
#include "port/libsys.h" 

USING_OPENCODE_STL_NS
__OPENCODE_BEGIN_NAMESPACE

DatagramSocketImpl::~DatagramSocketImpl(){}

DatagramSocketImpl::DatagramSocketImpl()
{
 localPort = 0;
}

DatagramSocketImpl::DatagramSocketImpl(const DatagramSocketImpl& rv)
{
  localPort = rv.localPort;
  fd = rv.fd;
}

DatagramSocketImpl& DatagramSocketImpl::operator=(const DatagramSocketImpl& rv)
{
  if(&rv == this)
  {
    return *this;
  }

  localPort = rv.localPort;
  fd = rv.fd;

  return *this;
}

DatagramSocketImpl* DatagramSocketImpl::clone() const
{
  //delegate to doClone
  DatagramSocketImpl* pClone = doClone();
  return pClone;
}

 
int DatagramSocketImpl::getLocalPort() const
{
  return localPort;
}


const SocketDescriptor& DatagramSocketImpl::getSocketDescriptor() const
{
  return fd;
}


__OPENCODE_END_NAMESPACE
