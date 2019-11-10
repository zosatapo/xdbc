#include "socket/networkUtils.h"
#include "socket/networkInterface.h"
#include "socket/socketUtils.h"
#include "socket/socketException.h"
#include "socket/socketAddress.h"
#include "socket/socketOptions.h"
#include "socket/inetAddress.h"

#include "port/logging.h"
#include "port/libsys.h"
#include "port/booleanComponent.h"
#include "port/ioc_socket.h" 

__OPENCODE_BEGIN_NAMESPACE

#if defined(PORT_WINDOWS)

void  NetworkUtils::mcast_join_leave(__NET_HOOK_DECLARE_MORE const SocketDescriptor& sd,const InetAddress& mcastaddr, const NetworkInterface* netIf,bool join) __NET_THROW_DECLARE(SocketException)
{     
	OPENCODE_TYPE_SOCKET  fd = sd.getDescriptor();  
	struct ip_mreq mname;
	
	if (netIf != 0) 
	{
	  if (netIf->getInetAddresses().empty()) 
	  {
	  	if (join) 
	  	{
	    	__NET_HOOK_RETURN(return,SocketException,"bad argument for IP_ADD_MEMBERSHIP: No IP addresses bound to interface");
	  	}
	  	else
	  	{
	  		__NET_HOOK_RETURN(return,SocketException,"bad argument for IP_DROP_MEMBERSHIP: No IP addresses bound to interface");
	  	}	
	  }
	  
	  InetAddress addr = netIf->getInetAddresses()[0];
	
	  OPENCODE_TYPE_INET4ADDRESS  addr_t;
	  memcpy(&addr_t,mcastaddr.getAddress(),__M_min((int)sizeof(addr_t),mcastaddr.getLength()));     
	  mname.imr_multiaddr.s_addr = addr_t;
	
	  OPENCODE_TYPE_INET4ADDRESS  addr_t2;
	  memcpy(&addr_t2,addr.getAddress(),__M_min((int)sizeof(addr_t2),addr.getLength()));     
	  
	  mname.imr_multiaddr.s_addr = addr_t2;
	}
	
	if (netIf == 0) 
	{
	  struct in_addr in;
	  struct in_addr *inP = &in;
	  OPENCODE_TYPE_SOCKLEN len = sizeof(struct in_addr);
	  if (getsockopt(fd, IPPROTO_IP, IP_MULTICAST_IF, (OPENCODE_TYPE_SOCKGOPT)inP, (OPENCODE_TYPE_SOCKLEN*)&len) < 0)
	  {
	    __NET_HOOK_RETURN(return,SocketException,"getsockopt IP_MULTICAST_IF failed");
	  }
	  
	  mname.imr_interface.s_addr = in.s_addr;
	
	  OPENCODE_TYPE_INET4ADDRESS  addr_t;
	  memcpy(&addr_t,mcastaddr.getAddress(),__M_min((int)sizeof(addr_t),mcastaddr.getLength()));     
	  mname.imr_multiaddr.s_addr = addr_t;    	
	}
	
	if (__netwin_setSockOpt(fd, IPPROTO_IP, IP_DROP_MEMBERSHIP,(OPENCODE_TYPE_SOCKSOPT) &mname, sizeof(mname)) < 0) 
	{
    if (join) 
    {
      __NET_HOOK_RETURN(return,SocketException,"setsockopt IP_ADD_MEMBERSHIP failed");
    } 
    else 
    { 
      __NET_HOOK_RETURN(return,SocketException,"setsockopt IP_DROP_MEMBERSHIP failed");
    }
	}
}

#endif

__OPENCODE_END_NAMESPACE

