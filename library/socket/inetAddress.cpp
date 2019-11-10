#include "socket/inetAddress.h"
#include "socket/networkUtils.h"
#include "port/ioc_sbuf.h"
#include "port/ioc_socket.h" 

__OPENCODE_BEGIN_NAMESPACE

InetAddress InetAddress::NullAddress;

const unsigned char __net_ipv4_anylocal[] = {0x00,0x00,0x00,0x00};
const unsigned char __net_ipv4_loopback[] = {0x7f,0x00,0x00,0x01};
InetAddress InetAddress::AnyAddress_IPV4(__net_ipv4_anylocal,4,"0.0.0.0");
InetAddress InetAddress::LoopbackAddress_IPV4(__net_ipv4_loopback,4,"localhost");

#ifdef AF_INET6
const unsigned char __net_ipv6_anylocal[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
const unsigned char __net_ipv6_loopback[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
InetAddress InetAddress::AnyAddress_IPV6(__net_ipv6_anylocal,16,"::");
InetAddress InetAddress::LoopbackAddress_IPV6(__net_ipv6_loopback,16,"localhost");
#endif

InetAddress::~InetAddress()
{
  if(m_address != 0)
  {
    delete []m_address; 
  }
}

InetAddress::InetAddress()
{
#ifdef AF_INET6
  sin6_flowinfo = 0;
  sin6_scope_id = 0;
#endif
  
  m_address   = 0;
  int_addrlen = 0;

  int_family  = AF_INET;
#ifdef AF_INET6
  int_family  = AF_INET6;     
#endif
}

InetAddress::InetAddress(const unsigned char* addr,int addrlen)
:m_address(0),int_addrlen(0),int_family(AF_INET)
{
#ifdef AF_INET6
  sin6_flowinfo = 0;
  sin6_scope_id = 0;
#endif

  if(addr == 0)
  {
    return;
  }

  int_addrlen = addrlen;
  m_address = new unsigned char[int_addrlen];
  memcpy(m_address,addr,int_addrlen);  
}

InetAddress::InetAddress(const unsigned char* addr,int addrlen,const string& hostname)
:m_address(0),int_addrlen(0),int_family(AF_INET),str_hostname(hostname)
{
#ifdef AF_INET6
  sin6_flowinfo = 0;
  sin6_scope_id = 0;
#endif

  if(addr == 0)
  {
    return;
  }

  int_addrlen = addrlen;
  m_address = new unsigned char[int_addrlen];
  memcpy(m_address,addr,int_addrlen);
}

InetAddress::InetAddress(const unsigned char* addr,int addrlen,int family)
:m_address(0),int_addrlen(0),int_family(family)
{
#ifdef AF_INET6
  sin6_flowinfo = 0;
  sin6_scope_id = 0;
#endif
 
  if(addr == 0)
  {
    return;
  }

  int_addrlen = addrlen;
  m_address = new unsigned char[int_addrlen];
  memcpy(m_address,addr,int_addrlen);
}

InetAddress::InetAddress(const unsigned char* addr,int addrlen,int family,const string& hostname)
:m_address(0),int_addrlen(0),int_family(family),str_hostname(hostname)
{
#ifdef AF_INET6
  sin6_flowinfo = 0;
  sin6_scope_id = 0;
#endif

  if(addr == 0)
  {
    return;
  }

  int_addrlen = addrlen;
  m_address = new unsigned char[int_addrlen];
  memcpy(m_address,addr,int_addrlen);
}
  
 
InetAddress::InetAddress(const InetAddress& rv)
{
  this->str_hostname=rv.str_hostname;
  this->int_family=rv.int_family;
  this->int_addrlen = rv.int_addrlen;
  this->m_address = 0;

#ifdef AF_INET6
  this->sin6_flowinfo = rv.sin6_flowinfo;
  this->sin6_scope_id = rv.sin6_scope_id;
#endif

  if(int_addrlen > 0)
  {
    m_address = new unsigned char[int_addrlen];
    memcpy(m_address,rv.m_address,int_addrlen); 
  }
}

InetAddress& InetAddress::operator=(const InetAddress& rv)
{
  if(&rv == this)
  {
    return *this;
  }

  this->str_hostname=rv.str_hostname;
  this->int_family=rv.int_family;
  this->int_addrlen = rv.int_addrlen;

#ifdef AF_INET6
  this->sin6_flowinfo = rv.sin6_flowinfo;
  this->sin6_scope_id = rv.sin6_scope_id;
#endif

  if(m_address != 0)
  {
    delete []m_address;
    m_address = 0;
  }

  if(int_addrlen > 0)
  {
    m_address = new unsigned char[this->int_addrlen];
    memcpy(this->m_address,rv.m_address,int_addrlen);
  }

  return *this;
}

bool InetAddress::operator==(const InetAddress& rv) const
{
  return    (int_family == rv.int_family)
         && (int_addrlen == rv.int_addrlen)
         && ( memcmp(m_address,rv.m_address,int_addrlen) == 0) ;
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------

const string& InetAddress::getHostName() const
{
  return str_hostname;
}

int InetAddress::getFamily() const
{
  return int_family;
}

const unsigned char*  InetAddress::getAddress() const
{
  return m_address;
}

int   InetAddress::getLength() const
{
  return int_addrlen;
}

bool  InetAddress::equals(const InetAddress& rv)   const
{
  if(  (int_family == rv.int_family) 
    && (int_addrlen == rv.int_addrlen)
    && (memcmp(m_address,rv.m_address,int_addrlen) == 0) )
  {
    return true;
  }
  
  return false;
}

string InetAddress::toString() const
{
  StringBuffer sb;

  sb << "InetAddress[family=";
#ifdef AF_INET6
  if(int_family == AF_INET6)
  {
    sb << "AF_INET6";
  }
  else
#endif
  {
    sb << "AF_INET";
  }
    
  sb << ",addr=" << InetAddress::getDotAddress(*this);
#ifdef AF_INET6
  if(int_family == AF_INET6)
  {
    sb << ",sin6_flowinfo=" << sin6_flowinfo << ",sin6_scope_id=" << sin6_scope_id;
  }
#endif
  sb << "]";

  return sb.str();
}

InetAddress InetAddress::getLocalHost(__NET_HOOK_DECLARE_ONCE) __NET_THROW_DECLARE(UnknownHostException)
{
  return NetworkUtils::inetGetLocalHost(__NET_HOOK_INVOKE_ONCE);
}

InetAddress InetAddress::getByName(__NET_HOOK_DECLARE_MORE const string&  host) __NET_THROW_DECLARE(UnknownHostException)
{
  return NetworkUtils::inetGetByName(__NET_HOOK_INVOKE_MORE host);
}

void InetAddress::getAllByName(__NET_HOOK_DECLARE_MORE vector<InetAddress>& addresses,const string& host) __NET_THROW_DECLARE(UnknownHostException)
{
  NetworkUtils::inetGetAllByName(__NET_HOOK_INVOKE_MORE addresses,host);
}

//--------------------------------------------------------------------
// static
//--------------------------------------------------------------------
bool InetAddress::isNullAddress(const InetAddress& address)
{
  return (address.int_addrlen == 0);
}

bool InetAddress::isAnyAddress(const InetAddress& address)
{
  if(InetAddress::isNullAddress(address))
  {
    return false;
  }

#ifdef AF_INET6
  if (address.int_family == AF_INET6) 
  {
    char test = 0x00;
    for (int i = 0; i < address.int_addrlen; i++) 
    {
      test |= address.m_address[i];
    }
  
    return (test == 0x00);
  }
  else
#endif
  {
    if(address.int_family == AF_INET)
    {
      return (address.int_addrlen != 0) && ((address.m_address[0] & 0xff) == 0x00);
    }
  }
    
  return false;
}

bool InetAddress::isLoopbackAddress(const InetAddress& address)
{
  if(InetAddress::isNullAddress(address))
  {
    return false;
  }

#ifdef AF_INET6
  if (address.int_family == AF_INET6) 
  {
    int __max_index = address.int_addrlen - 1;
    char test = 0x00;
    for (int i = 0; i < __max_index; i++)
    {
      test |= address.m_address[i];
    }
  
    return (test == 0x00) && (address.m_address[__max_index] == 0x01); 
  }
  else
#endif
  {
    if(address.int_family == AF_INET)
    {
      return ((address.m_address[0] & 0xff) == 0x7f);
    }    
  }  
  return false;
}

bool InetAddress::isMulticastAddress(const InetAddress& address)
{  
  if(InetAddress::isNullAddress(address))
  {
    return false;
  }

#ifdef AF_INET6
  if (address.int_family == AF_INET6) 
  {
    return ((address.m_address[0] & 0xff) == 0xff); 
  }
  else
#endif
  {
    if(address.int_family == AF_INET)
    {
      return ((address.m_address[0] & 0xf0) == 0xe0);
    }
  }  
    
  return false;
}


//--------------------------------------------------------------------
// static
//--------------------------------------------------------------------
string  InetAddress::getDotAddress(const InetAddress& address)
{
  return NetworkUtils::inetGetDotAddress(address); 
}

const InetAddress& InetAddress::nullAddress()
{
  return InetAddress::NullAddress;
}

const InetAddress& InetAddress::anyAddress()
{
#ifdef AF_INET6
  if(NetworkUtils::IPv6_available())
  {
    return InetAddress::AnyAddress_IPV6;
  }
  else
#endif
  {
    return InetAddress::AnyAddress_IPV4;
  }
}

__OPENCODE_END_NAMESPACE

