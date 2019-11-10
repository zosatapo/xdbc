#include "socket/networkUtils.h"
#include "socket/socketException.h"
#include "socket/inetAddress.h"
#include "socket/socketAddress.h"
#include "socket/networkInterface.h"
#include "socket/socketOptions.h"
#include "port/utilities.h"
#include "port/libsys.h"
#include "port/booleanComponent.h"
#include "port/logging.h"
#include "port/ioc_socket.h" 

#if defined(PORT_HPUX)
#  define __INTERFACE_SIZE(ifreqP) (sizeof(struct if_laddrreq))
#  ifdef AF_INET6
#    define __INTERFACE_SIZE_IPV6(ifreqP) (sizeof(struct ifreq))
#  endif
#elif defined(PORT_AIX)
#  define __INTERFACE_SIZE(ifreqP) ( sizeof(ifreqP->ifr_name) +  __M_max( (int)(ifreqP->ifr_addr.sa_len), (int)sizeof(ifreqP->ifr_addr) ))
#else
#  define __INTERFACE_SIZE(ifreqP) (sizeof(struct  ifreq))
#endif

__OPENCODE_BEGIN_NAMESPACE

//======================================================================================
//
//======================================================================================

IMPLEMENT_LIFETIME_BEGIN(NetworkDoImpl)
IMPLEMENT_LIFETIME_END(NetworkDoImpl)

NetworkDoImpl::~NetworkDoImpl() {}
NetworkDoImpl::NetworkDoImpl()
{
  __mb_IPv6_supported = false;
  __mb_IPv6_available = false; 

#if defined(PORT_WINDOWS)  
  __mb_isRcvTimeoutSupported = true;
#else
  __mb_isRcvTimeoutSupported = false;
#endif

#ifdef  AF_INET6
  __mb_IPv6_supported = true;
#endif
  
  if(__mb_IPv6_supported)
  {
    __mb_IPv6_available = BooleanComponent::getBoolean("IPv6.available");
  }

  LOGGING_DEBUG("IPv6.supported = <" << __mb_IPv6_supported << ">");
  LOGGING_DEBUG("IPv6.available = <" << __mb_IPv6_available << ">");
}

bool   NetworkDoImpl::IPv6_supported()
{
  return __mb_IPv6_supported;
}

bool    NetworkDoImpl::IPv6_available()
{
  return __mb_IPv6_available;
}

bool NetworkDoImpl::isRcvTimeoutSupported()
{
	return __mb_isRcvTimeoutSupported;
}

void    NetworkDoImpl::setRcvTimeoutSupported(bool isRcvTimeoutSupported)
{
	__mb_isRcvTimeoutSupported = isRcvTimeoutSupported;
}

//======================================================================================
//
//======================================================================================
NetworkUtils::~NetworkUtils() {}
NetworkUtils::NetworkUtils(){}

bool     NetworkUtils::IPv6_supported()
{
  return NetworkUtilsImpl::getInstance().IPv6_supported();
}

bool    NetworkUtils::IPv6_available()
{
  return NetworkUtilsImpl::getInstance().IPv6_available();
}

bool  NetworkUtils::isRcvTimeoutSupported()
{
	return NetworkUtilsImpl::getInstance().isRcvTimeoutSupported();
}

void NetworkUtils::setRcvTimeoutSupported(bool isRcvTimeoutSupported)
{	
	NetworkUtilsImpl::getInstance().setRcvTimeoutSupported(isRcvTimeoutSupported);
}

int    NetworkUtils::inet_aton(const char *host_name, struct in_addr *addr)
{
#if defined(PORT_SUN_SOLARIS) || defined(PORT_WINDOWS)
  unsigned int ip_addr = ::inet_addr (host_name);

  // Broadcast addresses are weird...
  if (ip_addr == -1 && !StringsKit::equalsIgnoreCase(host_name, "255.255.255.255") )
  {
    return 0;
  }
  else if (addr == 0)
  {
    return 0;
  }
  else
  {
    addr->s_addr = ip_addr;  // Network byte ordered
    return 1;
  }
#else
    return ::inet_aton(host_name,addr); 
#endif
}

UXINT64 NetworkUtils::net_htonq(UXINT64 hostq)
{
#if defined(OPENCODE_WORDS_BIGENDIAN)
  return hostq;
#else
  UXINT32*i1 = (UXINT32*)&hostq;
  UXINT32 *i2 = i1 + 1;

  *i1 = net_htonl(*i1);
  *i2 = net_htonl(*i2);

  UXINT32 i3 = *i1;
  *i1 = *i2;
  *i2 = i3;

  return hostq;
#endif
}

UXINT32 NetworkUtils::net_htonl(UXINT32 hostlong)
{
#if defined(OPENCODE_WORDS_BIGENDIAN)
  return hostlong;
#else
  return (UXINT32)htonl(hostlong);
#endif
}

UXINT16  NetworkUtils::net_htons(UXINT16 hostshort)
{
#if defined(OPENCODE_WORDS_BIGENDIAN)
  return hostshort;
#else
  return (UXINT32)htons(hostshort);
#endif
}

UXINT64 NetworkUtils::net_ntohq(UXINT64 netq)
{
#if defined(OPENCODE_WORDS_BIGENDIAN)
  return netq;
#else  
  UXINT32 *i1 = (UXINT32*)&netq;
  UXINT32 *i2 = i1 + 1;

  *i1 = net_ntohl(*i1);
  *i2 = net_ntohl(*i2);

  UXINT32 i3 = *i1;
  *i1 = *i2;
  *i2 = i3;

  return netq;
#endif
}

UXINT32 NetworkUtils::net_ntohl(UXINT32 netlong)
{
#if defined(OPENCODE_WORDS_BIGENDIAN)
  return netlong;
#else  
  return (UXINT32)ntohl(netlong);
#endif
}

UXINT16  NetworkUtils::net_ntohs(UXINT16 netshort)
{
#if defined(OPENCODE_WORDS_BIGENDIAN)
  return netshort;
#else  
  return (UXINT16)ntohs(netshort);
#endif
}

//------------------------------------------------------------
// static InetAddress interface
//------------------------------------------------------------
string     NetworkUtils::inetGetLocalHostName()
{
  char host_name[1024+1] = {0};
  int ret_code=::gethostname(host_name,1024);  
  if(ret_code == OPENCODE_SOCKET_ERROR)
  {
    strcpy(host_name, "localhost");
  }
  
  return string(host_name);
}

void   NetworkUtils::inetGetAllByName(__NET_HOOK_DECLARE_MORE vector<InetAddress>& vector_addr,const string& host,int maxcount) __NET_THROW_DECLARE(UnknownHostException)
{
#ifdef AF_INET6
  bool  __may_try_ipv4 = false;

  if (NetworkUtils::IPv6_available())
  {
    int net_rv = 0;

    struct addrinfo* ai = 0;
    struct addrinfo hints ;
    memset(&hints, '\0', sizeof(hints));
    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_STREAM;

#if !defined(PORT_WINDOWS)
    hints.ai_flags = AI_V4MAPPED ;
#endif

    if((net_rv = getaddrinfo(host.c_str(),0, &hints, &ai)) != OPENCODE_SOCKET_ERROR)
    {
      struct addrinfo* runp = ai;

      if(maxcount == 1)
      {
        InetAddress __inet_addr = NetworkUtils::makeInetAddress((struct sockaddr*)runp->ai_addr,runp->ai_family);
        __inet_addr.str_hostname = host;
        vector_addr.push_back(__inet_addr);
      }
      else
      {
        while (runp != 0)
        {
          InetAddress __inet_addr = NetworkUtils::makeInetAddress((struct sockaddr*)runp->ai_addr,runp->ai_family);
          __inet_addr.str_hostname = host;
          vector_addr.push_back(__inet_addr);
          runp = runp->ai_next;
        }
      }
 
      freeaddrinfo (ai);
    }
    else
    {
      __may_try_ipv4 = true;       
    }
  }

  if((!NetworkUtils::IPv6_available()) || (NetworkUtils::IPv6_available() && __may_try_ipv4))
#endif
  {
     struct hostent* hp = 0;
     if ((hp = ::gethostbyname (host.c_str())) != 0)
     {
        struct in_addr address;
        if(NetworkUtils::inet_aton(host.c_str(),&address) != 0)
        {
          hp = ::gethostbyaddr ((const char*)&address.s_addr,sizeof(address.s_addr),AF_INET);
        }
     }

     if(hp == 0)
     {
       __NET_HOOK_RETURN(return,UnknownHostException,host);
     }

     if(maxcount == 1)
     {
       InetAddress addr((unsigned char*)hp->h_addr_list[0],hp->h_length,hp->h_addrtype,hp->h_name);
       vector_addr.push_back(addr);
     }
     else
     {
       char** pp_addr_list = hp->h_addr_list;
       while((*pp_addr_list) != 0)
       {
         OPENCODE_TYPE_INET4ADDRESS addr_t; 
         memcpy((char *)&addr_t,*pp_addr_list, __M_min((int)hp->h_length,(int)sizeof(addr_t)));             
         InetAddress addr((unsigned char*)&addr_t,4,hp->h_addrtype,hp->h_name);
         vector_addr.push_back(addr);       
         ++pp_addr_list;
         
       }//~end while((*pp_addr_list) != 0)

     }//~end if(maxcount == 1)
  }
}

InetAddress    NetworkUtils::inetGetLocalHost(__NET_HOOK_DECLARE_ONCE) __NET_THROW_DECLARE(UnknownHostException)
{
  return NetworkUtils::inetGetByName(__NET_HOOK_INVOKE_MORE NetworkUtils::inetGetLocalHostName());
}

InetAddress    NetworkUtils::inetGetByName(__NET_HOOK_DECLARE_MORE const string& host) __NET_THROW_DECLARE(UnknownHostException)
{
  vector<InetAddress> addresses;
  NetworkUtils::inetGetAllByName(__NET_HOOK_INVOKE_MORE addresses,host,1);
  __NET_HOOK_CHECK(return InetAddress::nullAddress());

  if(addresses.size() > 0)
  {
    return addresses[0];
  }

  __NET_HOOK_RETURN(return InetAddress::nullAddress(),UnknownHostException,host);
}

string    NetworkUtils::inetGetDotAddress(const InetAddress& address)
{
  if(address.getLength() > 0)
  {
#ifdef AF_INET6
    if(address.getFamily() == AF_INET6)
    {
      struct in6_addr __x_in6_addr;
      memcpy(__x_in6_addr.s6_addr,address.getAddress(),__M_min(16,address.getLength()));

      return NetworkUtils::inetGetDotAddress(__x_in6_addr);
    }
    else
#endif
    {
      if(address.getFamily() == AF_INET)
      {    
        OPENCODE_TYPE_INET4ADDRESS __x_in = 0;    
        memcpy(&__x_in,address.getAddress(),__M_min(4,address.getLength()));
      
        struct in_addr in;
        in.s_addr = __x_in;
        memcpy(&(in.s_addr),address.getAddress(),__M_min(4,address.getLength()));
        return NetworkUtils::inetGetDotAddress(in);  
      }
    }

  }//~end if(address.getLength() > 0)
   
#ifdef AF_INET6
  if(NetworkUtils::IPv6_available())
  {
    return string("::");
  }
  else
#endif
  {
    return string("0.0.0.0");
  }
}

//------------------------------------------------------------
// static NetworkInterface interface
//------------------------------------------------------------

void  NetworkUtils::netifGetAll(__NET_HOOK_DECLARE_MORE vector<NetworkInterface>& netIFArr)  __NET_THROW_DECLARE(SocketException)
{
  netif* ifs_head = enumInterfaces(__NET_HOOK_INVOKE_ONCE); 
  __NET_HOOK_CHECK(return);

  if (ifs_head == 0) 
  {
    return ;
  }
  
  /** Iterate through the interfaces, create a NetworkInterface instance */
  netif* ifs_curr = ifs_head;
  while (ifs_curr != 0) 
  { 
    NetworkInterface netif = createNetworkInterface(ifs_curr);
    netIFArr.push_back(netif); 
    
    ifs_curr = ifs_curr->next;
  }
 
  free(ifs_head);
}

NetworkInterface   NetworkUtils::netifGetByIndex(__NET_HOOK_DECLARE_MORE int index)  __NET_THROW_DECLARE(SocketException)
{
  netif *ifs, *curr;
  
  NetworkInterface netif;
  
  if (index <= 0) 
  {
    return netif;
  }
 
  ifs = enumInterfaces(__NET_HOOK_INVOKE_ONCE);
  __NET_HOOK_CHECK(return netif);
 
  if (ifs == 0) 
  {
    return netif;
  }
  
  /*
   * Search the list of interface based on index
   */
  curr = ifs;
  while (curr != 0) 
  {
    if (index == curr->index) 
    {
      break;
    }
    curr = curr->next;
  }
  
  /* if found create a NetworkInterface */
  if (curr != 0) 
  {
    netif = createNetworkInterface(curr);
  }
   
  freeif(ifs);
  
  return netif;  
}

NetworkInterface   NetworkUtils::netifGetByName(__NET_HOOK_DECLARE_MORE const string& name)  __NET_THROW_DECLARE(SocketException)
{
  netif *ifs, *curr;
  NetworkInterface netif;
  
  ifs = enumInterfaces(__NET_HOOK_INVOKE_ONCE);
  __NET_HOOK_CHECK(return netif);

  if (ifs == 0) 
  {
    return netif;
  }
  
  /** Search the list of interface based on index */
  curr = ifs;
  while (curr != 0) 
  {
    if (StringsKit::equalsIgnoreCase(name.c_str(), curr->name)) 
    {
      break;
    }
    curr = curr->next;
  }
  
  /** if found create a NetworkInterface */
  if (curr != 0) 
  {
    netif = createNetworkInterface(curr);
  }
   
  freeif(ifs);
  
  return netif;   
}

NetworkInterface   NetworkUtils::netifGetByInetAddress(__NET_HOOK_DECLARE_MORE const InetAddress& addr)  __NET_THROW_DECLARE(SocketException)
{
    netif *ifs, *curr;
    int family = AF_INET ;
    bool match = false;

    NetworkInterface netif;
    ifs = enumInterfaces(__NET_HOOK_INVOKE_ONCE);
    __NET_HOOK_CHECK(return netif);

    if (ifs == 0) 
    {
      return netif;
    }

    curr = ifs;
    while (curr != 0) 
    {
      netaddr *addrP = curr->addr;
    
      /*
       * Iterate through each address on the interface
       */
      while (addrP != 0) 
      {   
        if (family == addrP->family) 
        {
          unsigned char* __addrP = 0;
          int __addrL = 0;

        #ifdef AF_INET6
          if (NetworkUtils::IPv6_available() && (family == AF_INET6))
          {
            struct sockaddr_in6* __sockaddrP_him6 = (struct sockaddr_in6*)addrP->addr;
            __addrP = __sockaddrP_him6->sin6_addr.s6_addr; 
            __addrL = 16;
          }
          else
        #endif
          {
            struct sockaddr_in* __sockaddrP_him4 = (struct sockaddr_in*)addrP->addr;
            __addrP = (unsigned char*)&(__sockaddrP_him4->sin_addr.s_addr);
            __addrL = 4;
          }
           
          if((__addrL == addr.getLength()) && ( memcmp(__addrP,addr.getAddress(),__addrL) == 0))
          {
            match = true;
            break; 
          }      
        }
    
        if (match) 
        {
          break;
        }
        
        addrP = addrP->next;
      }
    
      if (match) 
      {
        break;
      }
      curr = curr->next;
    }

    /* if found create a NetworkInterface */
    if (match)
    {
      netif = createNetworkInterface(curr);
    }

    freeif(ifs);
    return netif;  
}

//------------------------------------------------------------
// static multicastSocket interface
//------------------------------------------------------------
void  NetworkUtils::mcastJoinGroup(__NET_HOOK_DECLARE_MORE const SocketDescriptor& sd,const InetAddress& mcastaddr, const NetworkInterface* netIf)  __NET_THROW_DECLARE(SocketException)
{
  mcast_join_leave(__NET_HOOK_INVOKE_MORE sd,mcastaddr,netIf,true);   
}

void  NetworkUtils::mcastLeaveGroup(__NET_HOOK_DECLARE_MORE const SocketDescriptor& sd,const InetAddress& mcastaddr, const NetworkInterface* netIf)  __NET_THROW_DECLARE(SocketException)
{
  mcast_join_leave(__NET_HOOK_INVOKE_MORE sd,mcastaddr,netIf,false);      
}

//------------------------------------------------------------
// static address format interface
//------------------------------------------------------------
InetAddress       NetworkUtils::makeInetAddress(struct in_addr& addr)
{  
  OPENCODE_TYPE_INET4ADDRESS __x_in_addr = addr.s_addr; 
  return InetAddress((unsigned char*)&(addr.s_addr),4,AF_INET,inet_ntoa(addr));
}

string     NetworkUtils::inetGetDotAddress(struct in_addr& addr)
{
  return string(inet_ntoa(addr));
}

#ifdef AF_INET6
InetAddress       NetworkUtils::makeInetAddress(struct in6_addr& addr)
{
  return InetAddress((unsigned char*)addr.s6_addr,16,AF_INET6,NetworkUtils::inetGetDotAddress(addr));
}

string     NetworkUtils::inetGetDotAddress(struct in6_addr& addr)
{
  char __addr_p[INET6_ADDRSTRLEN] = {0};
#if defined(PORT_WINDOWS)
#else
  inet_ntop(AF_INET6,(struct in6_addr *)&addr,__addr_p,INET6_ADDRSTRLEN);
#endif
  return string(__addr_p);  
}

void    NetworkUtils::make_sockaddr_in6(struct sockaddr_in6* him,const InetAddress& addr,int port)
{
  struct sockaddr_in6* __sockaddrP_him6 = (struct sockaddr_in6 *)him;
  __sockaddrP_him6->sin6_family   = AF_INET6;
  __sockaddrP_him6->sin6_port     = NETWORK_HTONS(port);
  __sockaddrP_him6->sin6_flowinfo = addr.sin6_flowinfo;
  __sockaddrP_him6->sin6_scope_id = addr.sin6_scope_id;
  
  if(addr.getFamily() == AF_INET6)
  {
    memcpy(__sockaddrP_him6->sin6_addr.s6_addr,addr.getAddress(),__M_min(16,addr.getLength())); 
  }
  else
  {
    unsigned char caddr[16] = {0};
    
    caddr[10] = 0xff;
    caddr[11] = 0xff;
		
    memcpy(caddr + 12,addr.getAddress(),__M_min(4,addr.getLength()));
    memcpy(__sockaddrP_him6->sin6_addr.s6_addr,caddr,16);
  }    
}
    
#endif

void  NetworkUtils::make_sockaddr(struct sockaddr* him,const InetAddress& addr,int port)
{
#ifdef AF_INET6
  if (NetworkUtils::IPv6_available())
  { 
    struct sockaddr_in6* __sockaddrP_him6 = (struct sockaddr_in6 *)him;
    make_sockaddr_in6(__sockaddrP_him6,addr,port);    
  }
  else
#endif
  {
    struct sockaddr_in* __sockaddrP_him4 = (struct sockaddr_in *)him;
    __sockaddrP_him4->sin_family = addr.getFamily();
    __sockaddrP_him4->sin_port = NETWORK_HTONS(port);
  
    memcpy(&(__sockaddrP_him4->sin_addr.s_addr),addr.getAddress(),__M_min(4,addr.getLength()));
  }  
}

void  NetworkUtils::make_sockaddr(struct sockaddr* him,const SocketAddress& addr)
{
  NetworkUtils::make_sockaddr(him,addr.getInetAddress(),addr.getPort());
}

InetAddress       NetworkUtils::makeInetAddress(struct sockaddr* addr,int family)
{
#ifdef AF_INET6
  if (family == AF_INET6)
  { 
    struct sockaddr_in6* __sockaddrP_him6 = (struct sockaddr_in6 *)addr;
    struct in6_addr __x_in6_addr;
    memcpy(__x_in6_addr.s6_addr,__sockaddrP_him6->sin6_addr.s6_addr,16);
  
    InetAddress __inet_addr = NetworkUtils::makeInetAddress(__x_in6_addr);
    __inet_addr.sin6_flowinfo = __sockaddrP_him6->sin6_flowinfo;;
    __inet_addr.sin6_scope_id = __sockaddrP_him6->sin6_scope_id;
  
    return __inet_addr;
  }
  else
#endif
  {
    struct sockaddr_in* __sockaddrP_him4 = (struct sockaddr_in *)addr;
    struct in_addr __x_in_addr;
    __x_in_addr.s_addr = __sockaddrP_him4->sin_addr.s_addr;
      
    return NetworkUtils::makeInetAddress(__x_in_addr);
  }
}

SocketAddress     NetworkUtils::makeSocketAddress(struct sockaddr* addr,int family)
{
#ifdef AF_INET6
  if (family == AF_INET6)
  { 
    struct sockaddr_in6* __sockaddrP_him6 = (struct sockaddr_in6 *)addr;
    return SocketAddress(NetworkUtils::makeInetAddress(addr,AF_INET6),NETWORK_NTOHS(__sockaddrP_him6->sin6_port));
  }
  else
#endif
  {
    struct sockaddr_in* __sockaddrP_him4 = (struct sockaddr_in *)addr;
    return SocketAddress(NetworkUtils::makeInetAddress(addr,AF_INET),NETWORK_NTOHS(__sockaddrP_him4->sin_port));
  }
}

NetworkUtils::netif*    NetworkUtils::enumInterfaces(__NET_HOOK_DECLARE_ONCE)  __NET_THROW_DECLARE(SocketException)
{
  netif *ifs = 0;
  
#ifdef AF_INET6
  /** If IPv6 is available then enumerate IPv6 addresses. */
  if (IPv6_available()) 
  {
    ifs = enumIPv6Interfaces(__NET_HOOK_INVOKE_MORE 0);
    __NET_HOOK_CHECK(return ifs);
  }
#endif

  /** Enumerate IPv4 addresses */
  ifs = enumIPv4Interfaces(__NET_HOOK_INVOKE_MORE ifs);
  __NET_HOOK_CHECK(return ifs);
  
  return ifs;
}
  
NetworkUtils::netif*   NetworkUtils::addif(netif *ifs, char* if_name, char* if_displayname,int if_dwIndex,int if_index, unsigned char* ifaddrP,int ifaddrlen,int family,struct sockaddr *new_addrP, int new_addrlen)   
{
  //printf("<<NetworkUtils::addif>> Address: %s , if_dwIndex = %d , if_index = %d\n",MemoryKit::numDump((const char*)new_addrP,new_addrlen).c_str(),if_dwIndex,if_index);

  char name[1024] = {0};
  char displayname[1024] = {0};
  char *unit;
  /*
   * If the interface name is a logical interface then we
   * remove the unit number so that we have the physical
   * interface (eg: hme0:1 -> hme0). NetworkInterface
   * currently doesn't have any concept of physical vs.
   * logical interfaces.
   */
  strcpy(name, if_name);
  unit = strchr(name, ':');
  if (unit != 0) 
  {
    *unit = '\0';
  }
  
  /*
   * Create and populate the netaddr node. If allocation fails
   * return an un-updated list.
   */
  netaddr* addrP = (netaddr *)malloc(sizeof(netaddr));
  if (addrP == 0) 
  {
    return ifs;
  }

  memset(addrP,0,sizeof(netaddr));
  addrP->addr = (struct sockaddr *)malloc(new_addrlen);
  if (addrP->addr == 0) 
  {
    free(addrP);
    return ifs;
  }

  memset(addrP->addr,0,new_addrlen);
  memcpy(addrP->addr, new_addrP, new_addrlen);
  addrP->family = family;


  netif* __ifs_head = ifs;
  netif* __currif = __ifs_head;
  /*
   * Check if this is a "new" interface. Use the interface
   * name for matching because index isn't supported on
   * Solaris 2.6 & 7.
   */
  while (__currif != 0) 
  {
    if (StringsKit::equalsIgnoreCase(name, __currif->name)) 
    {
      break;
    }
    __currif = __currif->next;
  }
 
  //printf("NetworkUtils::addif : ifs = %p , name = %s , index = %d , currif = %x\n",ifs,name,if_index,__currif);
 
  /*
   * If "new" then create an netif structure and
   * insert it onto the list.
   */
  if (__currif == 0) 
  {
    __currif = (netif *)malloc(sizeof(netif));
    if(__currif == 0)
    {
      free(addrP->addr);
      free(addrP);
      return __ifs_head; 
    }

    memset(__currif,0,sizeof(netif));
    
    int __if_name_len = strlen(name);
    __currif->name = (char*)malloc(__if_name_len + 1);
    if (__currif->name == 0) 
    {
      free(__currif);
      return __ifs_head;
    } 
    memset(__currif->name,0,__if_name_len);
    strcpy(__currif->name,name);
    
    
    char displayname[1024] = {0};
    if(if_displayname != 0)
    {
    	strcpy(displayname,if_displayname);
    }
    else
    {
    	strcpy(displayname,name);
    }
 
    int __if_displayname_len = strlen(displayname);
    __currif->displayName = (char*)malloc(__if_displayname_len + 1);
    if (__currif->displayName == 0) 
    {
      free(__currif);
      return __ifs_head;
    } 
    memset(__currif->displayName,0,__if_displayname_len);
    strcpy(__currif->displayName,displayname);   
    
    memcpy(__currif->ifaddr, ifaddrP, ifaddrlen);
   // printf("ifaddr : %s\n",MemoryKit::numDump((const char*)__currif->ifaddr,ifaddrlen).c_str());

    __currif->index = if_index;
    __currif->dwIndex = if_dwIndex;
    __currif->addr = 0;

    __currif->next = ifs;
    __ifs_head = __currif;
  }
  
  /*
   * Finally insert the address on the interface
   */
  addrP->next = __currif->addr;
  __currif->addr = addrP;

  return __ifs_head;
}

void   NetworkUtils::freeif(netif *ifs)
{
  netif *currif = ifs;
  
  while (currif != 0) 
  {
    netaddr *addrP = currif->addr;
    while (addrP != 0) 
    {
      netaddr *next = addrP->next;
      free(addrP->addr); 
      free(addrP);
      addrP = next;
    }
  
    free(currif->name);
    free(currif->displayName);
    
    ifs = currif->next;
    free(currif);
    
    currif = ifs;
  }
}

NetworkInterface  NetworkUtils::createNetworkInterface(netif *ifs)
{ 
  string name = ifs->name;
  if (name.empty()) 
  {
    return NetworkInterface();
  }
  
  string displayName = ifs->name;
  int index = ifs->index;
  
  /*
   * Create the array of InetAddresses
   */
  vector<InetAddress> addrArr;

  netaddr *addrP = ifs->addr;
  while (addrP != 0) 
  {
#ifdef AF_INET6
    if(addrP->family == AF_INET6)
    {
      addrArr.push_back(NetworkUtils::makeInetAddress((struct sockaddr*)addrP->addr,AF_INET6));
    }
    else
#endif
    if (addrP->family == AF_INET) 
    {
      addrArr.push_back(NetworkUtils::makeInetAddress((struct sockaddr*)addrP->addr,AF_INET));
    }

    addrP = addrP->next;
  }
 
  vector<unsigned char> ifaddrArr;
  for (int i = 0; i < 6; i++) 
  {
    ifaddrArr.push_back(ifs->ifaddr[i]);
  }
 
  //printf("NetworkInterface::ifaddr : %s\n",MemoryKit::numDump((const char*)ifs->ifaddr,6).c_str());
 
  /* return the NetworkInterface */
  return NetworkInterface(name,index,ifaddrArr,addrArr,displayName);  
}  

#if !defined(PORT_WINDOWS)

void  NetworkUtils::mcast_join_leave(__NET_HOOK_DECLARE_MORE const SocketDescriptor& sd,const InetAddress& mcastaddr, const NetworkInterface* netIf,bool join) __NET_THROW_DECLARE(SocketException)
{
  OPENCODE_TYPE_SOCKET  fd = sd.getDescriptor();  

  bool ipv6_join_leave = false;

#ifdef AF_INET6
  if (NetworkUtils::IPv6_available())
  { 
    ipv6_join_leave = true; 
  }
#endif
  
  if(!ipv6_join_leave)
  {
  #if defined(PORT_HPUX)
    struct ip_mreq mname;
  #elif defined(PORT_AIX)
    struct ip_mreq mname;
  #elif defined(PORT_SUN_SOLARIS)
    struct ip_mreq mname;
  #else
    struct ip_mreqn mname;
  #endif
  
    int mname_len = 0;
  
    /*
     * joinGroup(InetAddress, NetworkInterface) implementation :-
     *
     * Linux/IPv6:  use ip_mreqn structure populated with multicast
     *    address and interface index.
     *
     * IPv4:  use ip_mreq structure populated with multicast
     *    address and first address obtained from
     *    NetworkInterface
     */
    if (netIf != 0) 
    {
      if (netIf->getInetAddresses().empty()) 
      {
        __NET_HOOK_RETURN(return,SocketException,"bad argument for IP_ADD_MEMBERSHIP: No IP addresses bound to interface");
      }
      
      InetAddress addr = netIf->getInetAddresses()[0];
  
      OPENCODE_TYPE_INET4ADDRESS  addr_t;
      memcpy(&addr_t,mcastaddr.getAddress(),__M_min((int)sizeof(addr_t),mcastaddr.getLength()));     
      mname.imr_multiaddr.s_addr = addr_t;
  
      OPENCODE_TYPE_INET4ADDRESS  addr_t2;
      memcpy(&addr_t2,addr.getAddress(),__M_min((int)sizeof(addr_t2),addr.getLength()));     
            
  #if defined(PORT_HPUX)
      mname.imr_interface.s_addr = addr_t2;
  #elif defined(PORT_AIX)
      mname.imr_interface.s_addr = addr_t2;
  #elif defined(PORT_SUN_SOLARIS)
      mname.imr_interface.s_addr = addr_t2;
  #else
      mname.imr_address.s_addr = addr_t2;
  #endif
    }
  
  //-----------------------------------------------------------------------------------------
    /*
     * joinGroup(InetAddress) implementation :-
     *
     * Linux/IPv6:  use ip_mreqn structure populated with multicast
     *              address and interface index. index obtained
     *    from cached value or IPV6_MULTICAST_IF.
     *
     * IPv4:        use ip_mreq structure populated with multicast
     *              address and local address obtained from
     *              IP_MULTICAST_IF. On Linux IP_MULTICAST_IF
     *    returns different structure depending on 
     *    kernel.
     */
    if (netIf == 0) 
    {  
      struct in_addr in;
      struct in_addr *inP = &in;
      OPENCODE_TYPE_SOCKLEN len = sizeof(struct in_addr);
      if (getsockopt(fd, IPPROTO_IP, IP_MULTICAST_IF, (OPENCODE_TYPE_SOCKGOPT)inP, (OPENCODE_TYPE_SOCKLEN*)&len) < 0)
      {
        __NET_HOOK_RETURN(return,SocketException,"getsockopt IP_MULTICAST_IF failed");
      }
      
  #if defined(PORT_HPUX)
      mname.imr_interface.s_addr = in.s_addr;
  #elif defined(PORT_AIX)
      mname.imr_interface.s_addr = in.s_addr;
  #elif defined(PORT_SUN_SOLARIS)
      mname.imr_interface.s_addr = in.s_addr;
  #else
      mname.imr_address.s_addr = in.s_addr;    
  #endif
  
      OPENCODE_TYPE_INET4ADDRESS  addr_t;
      memcpy(&addr_t,mcastaddr.getAddress(),__M_min((int)sizeof(addr_t),mcastaddr.getLength()));     
      mname.imr_multiaddr.s_addr = addr_t;
    }
  
  //-------------------------------------------------------------------------------
    /* 
     * Join the multicast group.
     */
    if (::setsockopt(fd, IPPROTO_IP, (join ? IP_ADD_MEMBERSHIP:IP_DROP_MEMBERSHIP),(OPENCODE_TYPE_SOCKSOPT) &mname, sizeof(mname)) < 0) 
    {
      /*
       * If IP_ADD_MEMBERSHIP returns ENOPROTOOPT on Linux and we've got
       * IPv6 enabled then it's possible that the kernel has been fixed
       * so we switch to IPV6_ADD_MEMBERSHIP socket option.
       * As of 2.4.7 kernel IPV6_ADD_MEMERSHIP can't handle IPv4-mapped
       * addresses so we have to use IP_ADD_MEMERSHIP for IPv4 multicast
       * groups. However if the socket is an IPv6 socket then then setsockopt
       * should reurn ENOPROTOOPT. We assume this will be fixed in Linux
       * at some stage.
       */
            
      if (errno) 
      {
        if (join) 
        {
          __NET_HOOK_RETURN(return,SocketException,"setsockopt IP_ADD_MEMBERSHIP failed");
        } 
        else 
        {
          if (errno == ENOENT) 
          {
            __NET_HOOK_RETURN(return,SocketException,"Not a member of the multicast group");
          }
          else
          { 
            __NET_HOOK_RETURN(return,SocketException,"setsockopt IP_DROP_MEMBERSHIP failed");
          }
        }
      }
    
    }//~end if(setsockopt())    

  	/** If we haven't switched to IPv6 socket option then we're done.*/
  	if (!ipv6_join_leave) 
  	{
  	  return;
  	}
  }  

  /** 
   * IPv6 join. If it's an IPv4 multicast group then we use an IPv4-mapped  address.
   */
#ifdef AF_INET6
  {
    struct ipv6_mreq mname6;
    
    struct sockaddr_in6 him6;
    NetworkUtils::make_sockaddr_in6(&him6,mcastaddr,0);
    memcpy((void *)&(mname6.ipv6mr_multiaddr), him6.sin6_addr.s6_addr, sizeof(struct in6_addr));
    
    mname6.ipv6mr_interface = 0;
    if (netIf != 0) 
    {
      mname6.ipv6mr_interface =  netIf->getIndex();
    }      

  //-------------------------------------------------------------------------------
    /* 
     * Join the multicast group.
     */
    if (::setsockopt(fd, IPPROTO_IPV6, (join ? IPV6_ADD_MEMBERSHIP:IPV6_DROP_MEMBERSHIP),(OPENCODE_TYPE_SOCKSOPT) &mname6, sizeof(mname6)) < 0) 
    {
      /*
       * If IP_ADD_MEMBERSHIP returns ENOPROTOOPT on Linux and we've got
       * IPv6 enabled then it's possible that the kernel has been fixed
       * so we switch to IPV6_ADD_MEMBERSHIP socket option.
       * As of 2.4.7 kernel IPV6_ADD_MEMERSHIP can't handle IPv4-mapped
       * addresses so we have to use IP_ADD_MEMERSHIP for IPv4 multicast
       * groups. However if the socket is an IPv6 socket then then setsockopt
       * should reurn ENOPROTOOPT. We assume this will be fixed in Linux
       * at some stage.
       */
            
      if (errno) 
      {
        if (join) 
        {
          __NET_HOOK_RETURN(return,SocketException,"setsockopt IP_ADD_MEMBERSHIP failed");
        } 
        else 
        {
          if (errno == ENOENT) 
          {
            __NET_HOOK_RETURN(return,SocketException,"Not a member of the multicast group");
          }
          else
          { 
            __NET_HOOK_RETURN(return,SocketException,"setsockopt IP_DROP_MEMBERSHIP failed");
          }
        }
      }
    
    }//~end if(setsockopt())  
  } 
#endif           
}

#endif

__OPENCODE_END_NAMESPACE

