#include "socket/networkUtils.h"
#include "socket/socketException.h"
#include "socket/inetAddress.h"
#include "socket/socketAddress.h"
#include "socket/networkInterface.h"
#include "socket/socketOptions.h"

#include "port/libsys.h"
#include "port/booleanComponent.h"
#include "port/ioc_socket.h" 

#if defined(PORT_WINDOWS)
#  include <iphlpapi.h>  //for NetworkInterface
#endif

__OPENCODE_BEGIN_NAMESPACE

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

#if defined(PORT_WINDOWS)

NetworkUtils::netif* __netwin_createNetworkInterface(int& count, NetworkUtils::netif* ifs,
                                                     char *if_name,char* if_displayame,
                                                     int if_index,int if_dwIndex,
                                                     unsigned char* ifaddr,int ifaddrlen,
                                                     MIB_IFROW* ifrowP)
{
  /** Use GetIpAddrTable to enumerate the IP Addresses */
  ULONG size = sizeof(MIB_IPADDRTABLE);
  MIB_IPADDRTABLE* tableP = (MIB_IPADDRTABLE *)malloc(size);
  DWORD ret = GetIpAddrTable(tableP, &size, FALSE);
  
  if (ret == ERROR_INSUFFICIENT_BUFFER || ret == ERROR_BUFFER_OVERFLOW) 
  {
		tableP = (MIB_IPADDRTABLE *)realloc(tableP, size);
    ret = GetIpAddrTable(tableP, &size, FALSE);
  }
  
  if (ret != NO_ERROR) 
  {
		if (tableP != 0){free(tableP);}
    count = -1;
		__NET_HOOK_RETURN(return ifs,SocketException,"IP Helper Library GetIpAddrTable function failed");	
  }

  /*
   * Iterate through the table to find the addresses with the
   * matching dwIndex. Ignore 0.0.0.0 addresses.
   */
  count = 0;
  NetworkUtils::netaddr* netaddrP = NULL;
  int i = 0;
  
  while (i++ < tableP->dwNumEntries) 
  {
    struct sockaddr_in __sockaddr_in4x;
      memset(&__sockaddr_in4x,0,sizeof(__sockaddr_in4x));
      __sockaddr_in4x.sin_addr.s_addr = NETWORK_HTONL(tableP->table[i].dwAddr);
      printf("Address<%d>: %s , dwIndex = %d , ifrowP.dwIndex = %d\n",sizeof(struct sockaddr_in),MemoryKit::numDump((const char*)&(__sockaddr_in4x.sin_addr.s_addr),4).c_str(),tableP->table[i].dwIndex,ifrowP->dwIndex);

    if ( (tableP->table[i].dwIndex == ifrowP->dwIndex) && (tableP->table[i].dwAddr != 0)) 
    {                                                               
	    /** Try to get the interface  MAC address*/
	    struct sockaddr* __sockaddrP = 0;
	    int __sockaddrlen = 0;
      
      struct sockaddr_in __sockaddr_in4;
      memset(&__sockaddr_in4,0,sizeof(__sockaddr_in4));
      __sockaddr_in4.sin_addr.s_addr = (tableP->table[i].dwAddr);
      __sockaddrP = (struct sockaddr*)&__sockaddr_in4;
      __sockaddrlen = sizeof(__sockaddr_in4);
                                                     
      /* Add to the list */
      ifs = NetworkUtils::addif(ifs, if_name,if_displayame,if_index,if_dwIndex,ifaddr,ifaddrlen,AF_INET,__sockaddrP,__sockaddrlen);                    
	    count++;
		}
  }

  if (tableP != 0){free(tableP);}
  
  return ifs;
    
}

NetworkUtils::netif*    __netwin_enumIPv4Interfaces(__NET_HOOK_DECLARE_MORE NetworkUtils::netif* ifs)  __NET_THROW_DECLARE(SocketException)
{
  /* Ask the IP Helper library to enumerate the adapters */
  ULONG size = sizeof(MIB_IFTABLE);
  MIB_IFTABLE* tableP = (MIB_IFTABLE *)malloc(size);
  DWORD ret = GetIfTable(tableP, &size, TRUE);
  if (ret == ERROR_INSUFFICIENT_BUFFER || ret == ERROR_BUFFER_OVERFLOW) 
  {
	  tableP = (MIB_IFTABLE *)realloc(tableP, size);
	  ret = GetIfTable(tableP, &size, TRUE);
  }
   
  if (ret != NO_ERROR) 
  {
	  if (tableP != 0) {free(tableP); tableP = 0;}		  
 	  return ifs;
   	
  }//~end if (ret != NO_ERROR) 

  /** Iterate through the list of adapters */
  int count = 0;
  NetworkUtils::netif* netifP = 0;
  MIB_IFROW* ifrowP = tableP->table;
  int lo=0, eth=0, tr=0, fddi=0, ppp=0, sl=0, net=0;	  
   
  for (int i=0; i<tableP->dwNumEntries; i++) 
  {						
	  /** Generate a name for the device as Windows doesn't have any real concept of a device name.*/
	  char dev_name[8] = {0};	
	  switch (ifrowP->dwType) 
	  {
	    case MIB_IF_TYPE_ETHERNET:
		  {
			  sprintf(dev_name, "eth%d", eth++);
		  }
		  break;

	    case MIB_IF_TYPE_TOKENRING:
		  {
			  sprintf(dev_name, "tr%d", tr++);
		  }
		  break;

	    case MIB_IF_TYPE_FDDI:
		  {
			  sprintf(dev_name, "fddi%d", fddi++);
		  }
		  break;
  	    
	    case MIB_IF_TYPE_LOOPBACK:
		  {
			  /* There should only be only IPv4 loopback address */
		    if (lo > 0) {continue;}
		    strcpy(dev_name, "lo");
		    lo++;
		  }
		  break;

	    case MIB_IF_TYPE_PPP:
		  {
			  sprintf(dev_name, "ppp%d", ppp++);
		  }
		  break;

	    case MIB_IF_TYPE_SLIP:
		  {
			  sprintf(dev_name, "sl%d", sl++);
		  }
		  break;

	    default:
		  {
		    sprintf(dev_name, "net%d", net++);
	    }
	    break;
  	  
	  }//~end switch (ifrowP->dwType) 


	  char __displayName[1024] = {0};       
    strncpy(__displayName, (const char*)ifrowP->bDescr, ifrowP->dwDescrLen);
    __displayName[ifrowP->dwDescrLen] = '\0';
    
    int if_dwIndex = (int)ifrowP->dwIndex;
    int if_index = GetFriendlyIfIndex(ifrowP->dwIndex);

    unsigned char __ifaddrP[256] = {0};
    int __ifaddrlen = ifrowP->dwPhysAddrLen;
    memcpy(__ifaddrP,ifrowP->bPhysAddr,__ifaddrlen);
                                           				
	  int __ip_count = 0;
	  ifs = __netwin_createNetworkInterface(__ip_count,
	                                        ifs,dev_name,__displayName,
	                                        if_index,if_dwIndex,__ifaddrP,__ifaddrlen,
	                                        ifrowP);

	  /* onto the next interface */
	  ifrowP++;
  	
  }//~end for (int i=0; i<tableP->dwNumEntries; i++) 

  /** Free the interface table and return the interface list */
  if(tableP != 0) {free(tableP);tableP= 0;}
  return ifs;
}
#elif defined(PORT_HPUX)
NetworkUtils::netif*    __hpux_enumIPv4Interfaces(__NET_HOOK_DECLARE_MORE NetworkUtils::netif *ifs)  __NET_THROW_DECLARE(SocketException)
{
  int sockfd_ipv4 = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd_ipv4 < 0) 
  {
    /*
     * If EPROTONOSUPPORT is returned it means we don't have
     * IPv4 support so don't throw an exception.
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
  int numifs;

  if (ioctl(sockfd_ipv4, SIOCGIFNUM, (char *)&numifs) < 0)
  {
    close(sockfd_ipv4);
    __NET_HOOK_RETURN(return ifs,SocketException,"ioctl(SIOCGIFNUM) : " + System::getLastError());
  }
  bufsize = numifs * sizeof (struct ifreq);

  buf = (char *)malloc(bufsize);
  ifc.ifc_len = bufsize;
  ifc.ifc_buf = buf;
  if (ioctl(sockfd_ipv4, SIOCGIFCONF, (char *)&ifc) < 0) 
  {
    close(sockfd_ipv4);
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
    index = 0;

    /** Try to get the interface index */
    struct ifreq if2;
    memset((char *)&if2, 0, sizeof(if2));
    strcpy(if2.ifr_name, ifreqP->ifr_name);

    if (ioctl(sockfd_ipv4, SIOCGIFINDEX, (char *)&if2) >= 0)
    {
      index = if2.ifr_index;
    }
    else
    {
      continue;
    }
    
    /** Try to get the interface  MAC address*/
    struct sockaddr* __sockaddrP = (struct sockaddr *)&(ifreqP->ifr_addr);
    int __sockaddrlen = sizeof(struct sockaddr_in);
    
    if(__sockaddrP->sa_family == AF_INET)
    {
      /** Try to get the interface MAC */
      unsigned char __ifaddrP[256] = {0};
      int __ifaddrlen = 0;
      struct arpreq __arpreq_s;
      memset((char *)&__arpreq_s, 0, sizeof(__arpreq_s));
  
      __arpreq_s.ifindex = index;
      __arpreq_s.arp_pa = ifreqP->ifr_addr;
      __arpreq_s.arp_pa.sa_family = AF_INET; 
      __arpreq_s.arp_ha.sa_family = AF_INET;
  
      if(ioctl(sockfd_ipv4,SIOCGARP, &__arpreq_s) >= 0)
      {
        __ifaddrlen = __arpreq_s.arp_hw_addr_len;
        memcpy(__ifaddrP,__arpreq_s.arp_ha.sa_data,__ifaddrlen);
    
        /* Add to the list */
        ifs = NetworkUtils::addif(ifs, ifreqP->ifr_name,0,index,index,__ifaddrP,__ifaddrlen,AF_INET,__sockaddrP,__sockaddrlen);        
      }
      else
      {
        LOGGING_ERROR("NetworkUtils::enumIPv4Interfaces|ioctl(SIOCGARP) : " << System::getLastError());
      }
    }//~end if(__sockaddrP->sa_family == AF_INET)
    
  }//~end for(;cp < cplim; cp += __INTERFACE_SIZE(ifreqP))

  /** Free socket and buffer */
  close(sockfd_ipv4);
  free(buf);
  
  return ifs;  
}
#elif defined(PORT_AIX)
NetworkUtils::netif*    __aix_enumIPv4Interfaces(__NET_HOOK_DECLARE_MORE NetworkUtils::netif *ifs)  __NET_THROW_DECLARE(SocketException)
{
  int sockfd_ipv4 = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd_ipv4 < 0) 
  {
    /*
     * If EPROTONOSUPPORT is returned it means we don't have
     * IPv4 support so don't throw an exception.
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
  
  if ( (rc = ioctl(sockfd_ipv4, SIOCGSIZIFCONF , (char *)&bufsize)) < 0)
  {
    close(sockfd_ipv4);
    __NET_HOOK_RETURN(return ifs,SocketException,"ioctl(SIOCGSIZIFCONF) : " + System::getLastError());  
  }

  buf = (char *)malloc(bufsize);
  ifc.ifc_len = bufsize;
  ifc.ifc_buf = buf;
  if (ioctl(sockfd_ipv4, SIOCGIFCONF, (char *)&ifc) < 0) 
  {
    close(sockfd_ipv4);
    free(buf);
    __NET_HOOK_RETURN(return ifs,SocketException,"ioctl(SIOCGIFCONF) : " + System::getLastError());    
  }

  /*
   * Iterate through each interface
   */
  struct ifreq *ifreqP = ifc.ifc_req;
  char* cp = (char *)ifc.ifc_req;
  char* cplim = cp + ifc.ifc_len;
  int index = 0;

  for(;cp < cplim; cp += __INTERFACE_SIZE(ifreqP))
  {
    ifreqP = (struct ifreq *)cp; 
    index = 0;

    /** Try to get the interface index */
    index = (int)if_nametoindex(ifreqP->ifr_name);

    if(index == -1)
    {
      continue;
    }
    
    /** Try to get the interface  MAC address*/
    struct sockaddr* __sockaddrP = (struct sockaddr *)&(ifreqP->ifr_addr);
    int __sockaddrlen = __sockaddrP->sa_len;
    
    if(__sockaddrP->sa_family == AF_INET)
    {
      /** Try to get the interface MAC */
      unsigned char __ifaddrP[256] = {0};
      int __ifaddrlen = 0;
   
      struct ifreq if3;
      memset((char *)&if3, 0, sizeof(if3));
      strcpy(if3.ifr_name, ifreqP->ifr_name);
      
      if (ioctl(sockfd_ipv4, SIOCGIFADDR , (char *)&if3) >= 0)
      {
        //__ifaddrlen = if3.ifr_addr.sa_len;
        __ifaddrlen = 6;
        memcpy(__ifaddrP,if3.ifr_addr.sa_data,__ifaddrlen);

        /* Add to the list */
        ifs = NetworkUtils::addif(ifs, ifreqP->ifr_name,0,index,index,__ifaddrP,__ifaddrlen,AF_INET,__sockaddrP,__sockaddrlen);
      }
      else
      {
        LOGGING_ERROR("NetworkUtils::enumIPv4Interfaces|ioctl(SIOCGIFADDR) : " << System::getLastError());
      }
    
    }//~end if(__sockaddrP->sa_family == AF_INET)
    
  }//~end for(;cp < cplim; cp += __INTERFACE_SIZE(ifreqP))

  /** Free socket and buffer */
  close(sockfd_ipv4);
  free(buf); 
  
  return ifs;   
}    
#elif defined(PORT_SUN_SOLARIS)
NetworkUtils::netif*    __solaris_enumIPv4Interfaces(__NET_HOOK_DECLARE_MORE NetworkUtils::netif *ifs)  __NET_THROW_DECLARE(SocketException)
{
  int sockfd_ipv4 = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd_ipv4 < 0)
  {
    /*
     * If EPROTONOSUPPORT is returned it means we don't have
     * IPv4 support so don't throw an exception.
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
  int numifs;
  int rc = 0;

  if (ioctl(sockfd_ipv4, SIOCGIFNUM, (char *)&numifs) < 0) 
  {
    close(sockfd_ipv4);
    __NET_HOOK_RETURN(return ifs,SocketException,"ioctl(SIOCGIFNUM) : " + System::getLastError());
  }
  bufsize = numifs * (int)(sizeof (struct ifreq));

  buf = (char *)malloc(bufsize);
  ifc.ifc_len = bufsize;
  ifc.ifc_buf = buf;
  if (ioctl(sockfd_ipv4, SIOCGIFCONF, (char *)&ifc) < 0)
  {
    close(sockfd_ipv4);
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
    index = 0;

    /** Try to get the interface index */
    struct ifreq if2;
    memset((char *)&if2, 0, sizeof(if2));
    strcpy(if2.ifr_name, ifreqP->ifr_name);

    if (ioctl(sockfd_ipv4, SIOCGIFINDEX, (char *)&if2) >= 0)
    {
      index = if2.ifr_index;
    }
    else
    {
      //index = -1;
      continue;
    }

    /** Try to get the interface InetAddress */
    struct sockaddr* __sockaddrP = (struct sockaddr *)&(ifreqP->ifr_addr);
    int __sockaddrlen = sizeof(struct sockaddr_in);

    if(__sockaddrP->sa_family == AF_INET)
    {
      /** Try to get the interface MAC */
      unsigned char __ifaddrP[256] = {0};
      int __ifaddrlen = 0;

      struct arpreq __arpreq_s;
      memset((char *)&__arpreq_s, 0, (int)sizeof(__arpreq_s));

      __arpreq_s.arp_pa.sa_family = AF_INET; 
//      __arpreq_s.arp_ha.sa_family = AF_INET; 
      memcpy((char*)&(__arpreq_s.arp_pa),(char*)&(ifreqP->ifr_addr),sizeof(ifreqP->ifr_addr));
 
      if (ioctl(sockfd_ipv4, SIOCGARP, (char *)&__arpreq_s) >= 0)
      {
        __ifaddrlen = 6;
        memcpy(__ifaddrP,__arpreq_s.arp_ha.sa_data,__ifaddrlen);

        /* Add to the list */
        ifs = NetworkUtils::addif(ifs, ifreqP->ifr_name,0,index,index,__ifaddrP,__ifaddrlen,AF_INET,__sockaddrP,__sockaddrlen);
      }
      else
      {
        LOGGING_ERROR("NetworkUtils::enumIPv4Interfaces|ioctl(SIOCGARP) : " << System::getLastError());
      }

    }//~end if(__sockaddrP->sa_family == AF_INET)

  }//~end for(;cp < cplim; cp += __INTERFACE_SIZE(ifreqP))

  /** Free socket and buffer*/
  close(sockfd_ipv4);
  free(buf);

  return ifs;
}
#else
NetworkUtils::netif*    __linux_enumIPv4Interfaces(__NET_HOOK_DECLARE_MORE NetworkUtils::netif *ifs)  __NET_THROW_DECLARE(SocketException)
{
  int sockfd_ipv4 = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd_ipv4 < 0) 
  {
    /*
     * If EPROTONOSUPPORT is returned it means we don't have
     * IPv4 support so don't throw an exception.
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
  
  /**
   * need to do a dummy SIOCGIFCONF to determine the buffer size.
   * SIOCGIFCOUNT doesn't work
   */
  ifc.ifc_buf = 0;
  if (ioctl(sockfd_ipv4, SIOCGIFCONF, (char *)&ifc) < 0) 
  {
    close(sockfd_ipv4);
    __NET_HOOK_RETURN(return ifs,SocketException,"ioctl(SIOCGIFCONF) : " + System::getLastError());       
  }
  bufsize = ifc.ifc_len;

  buf = (char *)malloc(bufsize);
  ifc.ifc_len = bufsize;
  ifc.ifc_buf = buf;
  if (ioctl(sockfd_ipv4, SIOCGIFCONF, (char *)&ifc) < 0) 
  {
    close(sockfd_ipv4);
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
    index = 0;

    /** Try to get the interface index */
    struct ifreq if2;
    memset((char *)&if2, 0, sizeof(if2));
    strcpy(if2.ifr_name, ifreqP->ifr_name);

    if (ioctl(sockfd_ipv4, SIOCGIFINDEX, (char *)&if2) >= 0)
    {
      index = if2.ifr_ifindex;
    }
    else
    {
      //index = -1;
      continue;
    }
    
    /** Try to get the interface InetAddress */
    struct sockaddr* __sockaddrP = (struct sockaddr *)&(ifreqP->ifr_addr);
    int __sockaddrlen = sizeof(struct sockaddr_in); 
    
    if(__sockaddrP->sa_family == AF_INET)
    {
      /** Try to get the interface MAC */
      unsigned char __ifaddrP[256] = {0};
      int __ifaddrlen = 0;
   
      struct ifreq if3;
      memset((char *)&if3, 0, sizeof(if3));
      strcpy(if3.ifr_name, ifreqP->ifr_name);
      if (ioctl(sockfd_ipv4, SIOCGIFHWADDR, (char *)&if3) >= 0)
      {
        __ifaddrlen = IFHWADDRLEN;
        memcpy(__ifaddrP,if3.ifr_hwaddr.sa_data,__ifaddrlen);         

        /* Add to the list */
        ifs = NetworkUtils::addif(ifs, ifreqP->ifr_name,0,index,index,__ifaddrP,__ifaddrlen,AF_INET,__sockaddrP,__sockaddrlen);
      }
      else
      {
        LOGGING_ERROR("NetworkUtils::enumIPv4Interfaces|ioctl(SIOCGIFADDR) : " << System::getLastError());
      }  
      
    }//~end if(__sockaddrP->sa_family == AF_INET)
    
  }//~end for(;cp < cplim; cp += __INTERFACE_SIZE(ifreqP))

  /** Free socket and buffer*/
  close(sockfd_ipv4);
  free(buf); 
  
  return ifs;   
}    
#endif

NetworkUtils::netif*    NetworkUtils::enumIPv4Interfaces(__NET_HOOK_DECLARE_MORE netif *ifs)  __NET_THROW_DECLARE(SocketException)
{
#if defined(PORT_WINDOWS)
  return __netwin_enumIPv4Interfaces(__NET_HOOK_INVOKE_MORE ifs);
#elif defined(PORT_HPUX) 
  return __hpux_enumIPv4Interfaces(__NET_HOOK_INVOKE_MORE ifs);
#elif defined(PORT_AIX)
  return __aix_enumIPv4Interfaces(__NET_HOOK_INVOKE_MORE ifs);
#elif defined(PORT_SUN_SOLARIS)
  return __solaris_enumIPv4Interfaces(__NET_HOOK_INVOKE_MORE ifs);
#else
  return __linux_enumIPv4Interfaces(__NET_HOOK_INVOKE_MORE ifs);
#endif
}

__OPENCODE_END_NAMESPACE
