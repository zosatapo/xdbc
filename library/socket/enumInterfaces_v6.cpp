#include "socket/networkUtils.h"
#include "socket/socketException.h"
#include "socket/inetAddress.h"
#include "socket/socketAddress.h"
#include "socket/networkInterface.h"
#include "socket/socketOptions.h"

#include "port/libsys.h"
#include "port/booleanComponent.h"
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

#ifdef AF_INET6

#if defined(PORT_WINDOWS)
NetworkUtils::netif*    __netwin_enumIPv6Interfaces(__NET_HOOK_DECLARE_MORE NetworkUtils::netif *ifs)  __NET_THROW_DECLARE(SocketException)
{
  return ifs;
}
#elif defined(PORT_HPUX)
NetworkUtils::netif*    __hpux_enumIPv6Interfaces(__NET_HOOK_DECLARE_MORE NetworkUtils::netif *ifs)  __NET_THROW_DECLARE(SocketException)
{
 int sockfd_ipv6 = socket(AF_INET6, SOCK_DGRAM, 0);
  if (sockfd_ipv6 < 0) 
  {
    /*
     * If EPROTONOSUPPORT is returned it means we don't have
     * IPv6 support so don't throw an exception.
     */
    if (errno != EPROTONOSUPPORT) 
    {
      __NET_HOOK_RETURN(return ifs,SocketException,"Socket creation failed");
    }
    return ifs;
  }
  
  struct if_laddrconf ifc;
  char *buf;
  unsigned int bufsize;

  int rc = 0;
  int numifs;
  if (ioctl(sockfd_ipv6, SIOCGLIFNUM, (char *)&numifs) < 0)
  {
    close(sockfd_ipv6);
    __NET_HOOK_RETURN(return ifs,SocketException,"ioctl(SIOCGIFNUM) : " + System::getLastError());
  }
  bufsize = numifs * sizeof (struct if_laddrconf);

  buf = (char *)malloc(bufsize);
  ifc.iflc_len = bufsize;
  ifc.iflc_buf = buf;
  //ifc.iflc_req->iflr_addr.sa_family = AF_UNSPEC;
  if (ioctl(sockfd_ipv6, SIOCGLIFCONF, (char *)&ifc) < 0) 
  {
    close(sockfd_ipv6);
    free(buf);
    __NET_HOOK_RETURN(return ifs,SocketException,"ioctl(SIOCGLIFCONF) : " + System::getLastError());  
  }

  /** Iterate through each interface */
  struct if_laddrreq *ifreqP = ifc.iflc_req;
  char* cp = (char *)ifc.iflc_req;
  char* cplim = cp + ifc.iflc_len;
  int index = 0;

  for(;cp < cplim; cp += __INTERFACE_SIZE_IPV6(ifreqP))
  {
    ifreqP = (struct if_laddrreq *)cp; 

    /** Try to get the interface index */
    index = (int)if_nametoindex(ifreqP->iflr_name);
    if(index == -1)
    {
      continue;
    }
    
    /** Try to get the interface MAC address */
    struct sockaddr* __sockaddrP = (struct sockaddr *)&(ifreqP->iflr_addr);
    int __sockaddrlen = sizeof(struct sockaddr_in6);

    if(__sockaddrP->sa_family == AF_INET6)
    {
      /** Try to get the interface MAC */
      unsigned char __ifaddrP[256] = {0};
      int __ifaddrlen = 0;
  
      struct arpreq __arpreq_s;
      memset((char *)&__arpreq_s, 0, sizeof(__arpreq_s));
  
      __arpreq_s.ifindex = index;
      __arpreq_s.arp_pa = ifreqP->iflr_addr;
      __arpreq_s.arp_pa.sa_family = AF_INET6; 
      __arpreq_s.arp_ha.sa_family = AF_INET6;
  
      if(ioctl(sockfd_ipv6,SIOCGARP, &__arpreq_s) >= 0)
      {
        __ifaddrlen = __arpreq_s.arp_hw_addr_len;
        memcpy(__ifaddrP,__arpreq_s.arp_ha.sa_data,__ifaddrlen);

        /* Add to the list */
        ifs = NetworkUtils::addif(ifs, ifreqP->iflr_name,0,index,index,__ifaddrP,__ifaddrlen,AF_INET6,__sockaddrP,__sockaddrlen);
      }
      else
      {
        LOGGING_ERROR("NetworkUtils::enumIPv6Interfaces|ioctl(SIOCGARP) : " << System::getLastError());
      }
      
    }//~end if(__sockaddrP->sa_family == AF_INET6)
  
  }//~end for(;cp < cplim; cp += __INTERFACE_SIZE_IPV6(ifreqP))

  /** Free socket and buffer */
  close(sockfd_ipv6);
  free(buf);   
  
  return ifs;
}
#elif defined(PORT_AIX)
NetworkUtils::netif*    __aix_enumIPv6Interfaces(__NET_HOOK_DECLARE_MORE NetworkUtils::netif *ifs)  __NET_THROW_DECLARE(SocketException)
{
  int sockfd_ipv6 = socket(AF_INET6, SOCK_DGRAM, 0);
  if (sockfd_ipv6 < 0) 
  {
    /*
     * If EPROTONOSUPPORT is returned it means we don't have
     * IPv6 support so don't throw an exception.
     */
    if (errno != EPROTONOSUPPORT) 
    {
      __NET_HOOK_RETURN(return ifs,SocketException,"Socket creation failed");  
    }
    return ifs;
  }
  
  struct ifconf ifc;
  char *buf;
  unsigned int bufsize;

  int rc = 0;
  if ( (rc = ioctl(sockfd_ipv6, SIOCGSIZIFCONF , (char *)&bufsize)) < 0)
  {
    close(sockfd_ipv6);
    __NET_HOOK_RETURN(return ifs,SocketException,"ioctl(SIOCGSIZIFCONF) : " + System::getLastError());    
  }

  buf = (char *)malloc(bufsize);
  ifc.ifc_len = bufsize;
  ifc.ifc_buf = buf;
  if (ioctl(sockfd_ipv6, SIOCGIFCONF, (char *)&ifc) < 0) 
  {
    close(sockfd_ipv6);
    free(buf);
    __NET_HOOK_RETURN(return ifs,SocketException,"ioctl(SIOCGIFCONF) : " + System::getLastError());   
  }

  /** Iterate through each interface */
  struct ifreq *ifreqP = ifc.ifc_req;
  char* cp = (char *)ifc.ifc_req;
  char* cplim = cp + ifc.ifc_len;
  int index = 0;

  for(;cp < cplim; cp += __INTERFACE_SIZE(ifreqP))
  {
    ifreqP = (struct ifreq *)cp; 

    /** Try to get the interface index */
    index = (int)if_nametoindex(ifreqP->ifr_name);
    if(index == -1)
    {
      continue;
    }
    
    /** Try to get the interface  MAC address */
    struct sockaddr* __sockaddrP = (struct sockaddr *)&(ifreqP->ifr_addr);
    int __sockaddrlen = __sockaddrP->sa_len;
    
    if(__sockaddrP->sa_family == AF_INET6)
    {
      /** Try to get the interface MAC */
      unsigned char __ifaddrP[256] = {0};
      int __ifaddrlen = 0;
  
      struct ifreq if3;
      memset((char *)&if3, 0, sizeof(if3));
      strcpy(if3.ifr_name, ifreqP->ifr_name);
      if (ioctl(sockfd_ipv6, SIOCGIFADDR , (char *)&if3) >= 0)
      {
        __ifaddrlen = 6;
        memcpy(__ifaddrP,if3.ifr_addr.sa_data,__ifaddrlen);

        /* Add to the list */
        ifs = NetworkUtils::addif(ifs, ifreqP->ifr_name,0,index,index,__ifaddrP,__ifaddrlen,AF_INET,__sockaddrP,__sockaddrlen);
      }
      else
      {
        LOGGING_ERROR("NetworkUtils::enumIPv6Interfaces|ioctl(SIOCGIFADDR) : " << System::getLastError());
      }
    
    }//~end if(__sockaddrP->sa_family == AF_INET6)
    
  }//~end for(;cp < cplim; cp += __INTERFACE_SIZE(ifreqP))

  /** Free socket and buffer */
  close(sockfd_ipv6);
  free(buf);
   
  return ifs;
}
#elif defined(PORT_SUN_SOLARIS)
NetworkUtils::netif*    __solaris_enumIPv6Interfaces(__NET_HOOK_DECLARE_MORE NetworkUtils::netif *ifs)  __NET_THROW_DECLARE(SocketException)
{
  int sockfd_ipv6 = socket(AF_INET6, SOCK_DGRAM, 0);
  if (sockfd_ipv6 < 0)
  {
    /*
     * If EPROTONOSUPPORT is returned it means we don't have
     * IPv6 support so don't throw an exception.
     */
    if (errno != EPROTONOSUPPORT)
    {
      __NET_HOOK_RETURN(return ifs,SocketException,"Socket creation failed");
    }
    return ifs;
  }

  struct lifconf ifc;
  int n;
  char *buf;
  struct lifnum numifs;
  unsigned bufsize;

  /** Get the interface count */
  numifs.lifn_family = AF_UNSPEC;
  numifs.lifn_flags = 0;
  if (ioctl(sockfd_ipv6, SIOCGIFNUM, (char *)&numifs) < 0)
  {
    close(sockfd_ipv6);
    __NET_HOOK_RETURN(return ifs,SocketException,"ioctl(SIOCGIFNUM) : " + System::getLastError());
  }

  /**  Enumerate the interface configurations */
  bufsize = numifs.lifn_count * ((int)sizeof (struct lifreq));
  buf = (char *)malloc(bufsize);
    
  ifc.lifc_family = AF_UNSPEC;
  ifc.lifc_flags = 0;
  ifc.lifc_len = bufsize;
  ifc.lifc_buf = buf;
  if (ioctl(sockfd_ipv6, SIOCGIFCONF, (char *)&ifc) < 0)
  {
    close(sockfd_ipv6);
    free(buf);
    __NET_HOOK_RETURN(return ifs,SocketException,"ioctl(SIOCGIFCONF) : " + System::getLastError());
  }

  /** Iterate through each interface */
  struct lifreq * ifreqP = ifc.lifc_req;
  for (int nx = 0; nx < numifs.lifn_count; nx++, ifreqP++) 
  {
    /** Ignore non-IPv6 addresses */
    if (ifreqP->lifr_addr.ss_family != AF_INET6) 
    {
      continue;
    }

    /** Try to get the interface index */
    struct lifreq if2;
    int index = -1;

    memset((char *)&if2, 0, sizeof(if2));
    strcpy(if2.lifr_name, ifreqP->lifr_name);

    if (ioctl(sockfd_ipv6, SIOCGIFINDEX, (char *)&if2) >= 0)
    {
      index = if2.lifr_index;
    }
    else
    {
      //index = -1;
      continue;
    }

    /** Try to get the interface InetAddress */
    struct sockaddr* __sockaddrP = (struct sockaddr *)&(ifreqP->lifr_addr);
    int __sockaddrlen = sizeof(struct sockaddr_in6);

    if(__sockaddrP->sa_family == AF_INET6)
    {
      /** Try to get the interface MAC */
      unsigned char __ifaddrP[256] = {0};
      int __ifaddrlen = 0;

      struct arpreq __arpreq_s;
      memset((char *)&__arpreq_s, 0, (int)sizeof(__arpreq_s));

      __arpreq_s.arp_pa.sa_family = AF_INET6;
      __arpreq_s.arp_ha.sa_family = AF_INET6;
      memcpy((char*)&(__arpreq_s.arp_pa),(char*)&(ifreqP->lifr_addr),sizeof(ifreqP->lifr_addr));
      if (ioctl(sockfd_ipv6, SIOCGARP, (char *)&__arpreq_s) >= 0)
      {
        __ifaddrlen = 6;
        memcpy(__ifaddrP,__arpreq_s.arp_ha.sa_data,__ifaddrlen);

        /* Add to the list */
        ifs = NetworkUtils::addif(ifs, ifreqP->lifr_name,0,index,index,__ifaddrP,__ifaddrlen,AF_INET,__sockaddrP,__sockaddrlen);
      }
      else
      {
        LOGGING_ERROR("NetworkUtils::enumIPv4Interfaces|ioctl(SIOCGARP) : " << System::getLastError());
      }

    }//~end if(__sockaddrP->sa_family == AF_INET6)

  }//~end for (int nx = 0; nx < numifs.lifn_count; nx++, ifreqP++) 

  /** Free socket and buffer*/
  close(sockfd_ipv6);
  free(buf);

  return ifs;
}
#else
NetworkUtils::netif*    __linux_enumIPv6Interfaces(__NET_HOOK_DECLARE_MORE NetworkUtils::netif *ifs)  __NET_THROW_DECLARE(SocketException)
{
  FILE* fp;
  char addr6[40], devname[20];
  char addr6p[8][5];
  int plen, scope, dad_status, if_idx;
  uint8_t ipv6addr[16];

  int sockfd_ipv6 = socket(AF_INET6, SOCK_DGRAM, 0);
  if (sockfd_ipv6 < 0) 
  {
    /*
     * If EPROTONOSUPPORT is returned it means we don't have
     * IPv6 support so don't throw an exception.
     */
    if (errno != EPROTONOSUPPORT) 
    {
      __NET_HOOK_RETURN(return ifs,SocketException,"Socket creation failed");  
    }
    return ifs;
  }
    
  if ((fp = fopen("/proc/socket/if_inet6", "r")) != 0) 
  {
    while (fscanf(fp, "%4s%4s%4s%4s%4s%4s%4s%4s %02x %02x %02x %02x %20s\n",
    addr6p[0], addr6p[1], addr6p[2], addr6p[3],
    addr6p[4], addr6p[5], addr6p[6], addr6p[7],
    &if_idx, &plen, &scope, &dad_status, devname) != EOF) 
    {
      NetworkUtils::netif *ifs_ptr = 0;
      NetworkUtils::netif *last_ptr = 0;
      struct sockaddr_in6 addr;
      
      sprintf(addr6, "%s:%s:%s:%s:%s:%s:%s:%s",
      addr6p[0], addr6p[1], addr6p[2], addr6p[3],
      addr6p[4], addr6p[5], addr6p[6], addr6p[7]);
      inet_pton(AF_INET6, addr6, ipv6addr);
    
      memset(&addr, 0, sizeof(struct sockaddr_in6));
      memcpy((void*)addr.sin6_addr.s6_addr, (const void*)ipv6addr, 16);
      
      struct sockaddr_in6*  __sockaddrP_in6 = (struct sockaddr_in6*)&addr;
      int __sockaddrlen = sizeof(struct sockaddr_in6);
      __sockaddrP_in6->sin6_scope_id = scope;
      __sockaddrP_in6->sin6_family = AF_INET6;  
      __sockaddrP_in6->sin6_flowinfo = 0;

      struct sockaddr* __sockaddrP = (struct sockaddr *)&addr;

      unsigned char __ifaddrP[256] = {0};
      int __ifaddrlen = 0;
  
      struct ifreq if3;
      memset((char *)&if3, 0, sizeof(if3));
      strcpy(if3.ifr_name, devname);
      if (ioctl(sockfd_ipv6, SIOCGIFHWADDR, (char *)&if3) >= 0)
      {
        __ifaddrlen = IFHWADDRLEN;
        memcpy(__ifaddrP,if3.ifr_hwaddr.sa_data,__ifaddrlen);          
              
        /* Add to the list */       
        ifs = NetworkUtils::addif(ifs, devname, 0,if_idx,if_idx,__ifaddrP,__ifaddrlen, AF_INET6,__sockaddrP,__sockaddrlen);       
      } 
      else
      {
        LOGGING_ERROR("NetworkUtils::enumIPv6Interfaces|ioctl(SIOCGIFHWADDR) : " << System::getLastError());
      }
      
    }//~end if ((fp = fopen("/proc/socket/if_inet6", "r")) != 0) 
    
    fclose(fp);
    close(sockfd_ipv6);
  }
  
  return ifs;
}
#endif

NetworkUtils::netif*    NetworkUtils::enumIPv6Interfaces(__NET_HOOK_DECLARE_MORE netif *ifs)  __NET_THROW_DECLARE(SocketException)
{
#if defined(PORT_WINDOWS)
  return __netwin_enumIPv6Interfaces(__NET_HOOK_INVOKE_MORE ifs);
#elif defined(PORT_HPUX) 
  return __hpux_enumIPv6Interfaces(__NET_HOOK_INVOKE_MORE ifs);
#elif defined(PORT_AIX)
  return __aix_enumIPv6Interfaces(__NET_HOOK_INVOKE_MORE ifs);
#elif defined(PORT_SUN_SOLARIS)
  return __solaris_enumIPv6Interfaces(__NET_HOOK_INVOKE_MORE ifs);
#else
  return __linux_enumIPv6Interfaces(__NET_HOOK_INVOKE_MORE ifs);
#endif 
}
#endif

__OPENCODE_END_NAMESPACE

