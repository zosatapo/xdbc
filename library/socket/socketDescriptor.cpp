#include "socket/socketDescriptor.h"
#include "socket/networkUtils.h" 

__OPENCODE_BEGIN_NAMESPACE

SocketDescriptor::SocketDescriptor()
:int_fd(OPENCODE_INVALID_SOCKET)
{
#ifdef AF_INET6
  if(NetworkUtils::IPv6_available())
  {
    int_family  = AF_INET6;     
  }
  else
#endif
  {
    int_family  = AF_INET;  
  }
}

SocketDescriptor::SocketDescriptor(OPENCODE_TYPE_SOCKET fd)
:int_fd(fd)
{
#ifdef AF_INET6
  if(NetworkUtils::IPv6_available())
  {
    int_family  = AF_INET6;     
  }
  else
#endif
  {
    int_family  = AF_INET;  
  }
}

SocketDescriptor::SocketDescriptor(OPENCODE_TYPE_SOCKET fd,int family)
:int_fd(fd),int_family(family)
{
}
  
bool SocketDescriptor::isValid() const
{
  return int_fd != OPENCODE_INVALID_SOCKET;
}

OPENCODE_TYPE_SOCKET SocketDescriptor::getDescriptor() const
{
  return int_fd;
}

int   SocketDescriptor::getFamily() const
{
  return int_family;
}

void  SocketDescriptor::invalidate()
{
  int_fd = OPENCODE_INVALID_SOCKET;
}
  
void  SocketDescriptor::setDescriptor(const OPENCODE_TYPE_SOCKET& sh)
{
  int_fd = sh;
#ifdef AF_INET6
  if(NetworkUtils::IPv6_available())
  {
    int_family  = AF_INET6;     
  }
  else
#endif
  {
    int_family  = AF_INET;  
  }
}

void  SocketDescriptor::setDescriptor(const OPENCODE_TYPE_SOCKET& sh,int family)
{
  int_fd = sh;
  int_family = family;
}

__OPENCODE_END_NAMESPACE
