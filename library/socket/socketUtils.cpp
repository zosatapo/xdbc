#include "socket/socketUtils.h"
#include "socket/socketdefs.h"
#include "socket/socketDescriptor.h"
#include "socket/inetAddress.h"
#include "socket/socketException.h"
#include "socket/socketOptions.h"
#include "socket/socketImpl.h"
#include "socket/socket.h"
#include "socket/serverSocket.h"
#include "socket/networkUtils.h"

#include "port/libsys.h"

__OPENCODE_BEGIN_NAMESPACE

//------------------------------------------------------------
// ctor and dector
//------------------------------------------------------------
SocketUtils::SocketUtils(){}

//=======================================================================================
//   -- BEGIN --
//=======================================================================================
const int  SocketUtils::C_MSG_PEEK = MSG_PEEK;

#if defined(PORT_WINDOWS)
struct SocketOptionsMap SocketUtils::opts[]={
  { SocketOptions::SO_W_TCP_NODELAY,    IPPROTO_TCP,  TCP_NODELAY },
  { SocketOptions::SO_W_OOBINLINE,    SOL_SOCKET,  SO_OOBINLINE },
  { SocketOptions::SO_W_LINGER,    SOL_SOCKET,  SO_LINGER },
  { SocketOptions::SO_W_SNDBUF,    SOL_SOCKET,  SO_SNDBUF },
  { SocketOptions::SO_W_RCVBUF,    SOL_SOCKET,  SO_RCVBUF },
  { SocketOptions::SO_W_KEEPALIVE,    SOL_SOCKET,  SO_KEEPALIVE },
  { SocketOptions::SO_W_REUSEADDR,    SOL_SOCKET,  SO_REUSEADDR },
  { SocketOptions::SO_W_BROADCAST,      SOL_SOCKET,     SO_BROADCAST },
  { SocketOptions::SO_W_IP_TOS,    IPPROTO_IP,  IP_TOS },
  { SocketOptions::SO_W_IP_MULTICAST_IF,  IPPROTO_IP,   IP_MULTICAST_IF },
  { SocketOptions::SO_W_IP_MULTICAST_IF2,      IPPROTO_IP,   IP_MULTICAST_IF },
  { SocketOptions::SO_W_IP_MULTICAST_LOOP,    IPPROTO_IP,   IP_MULTICAST_LOOP },
 };
#else
struct SocketOptionsMap SocketUtils::opts[]={
  { SocketOptions::SO_W_TCP_NODELAY,    IPPROTO_TCP,  TCP_NODELAY },
  { SocketOptions::SO_W_OOBINLINE,    SOL_SOCKET,  SO_OOBINLINE },
  { SocketOptions::SO_W_LINGER,    SOL_SOCKET,  SO_LINGER },
  { SocketOptions::SO_W_SNDBUF,    SOL_SOCKET,  SO_SNDBUF },
  { SocketOptions::SO_W_RCVBUF,    SOL_SOCKET,  SO_RCVBUF },
  { SocketOptions::SO_W_KEEPALIVE,    SOL_SOCKET,  SO_KEEPALIVE },
  { SocketOptions::SO_W_REUSEADDR,    SOL_SOCKET,  SO_REUSEADDR },
  { SocketOptions::SO_W_BROADCAST,      SOL_SOCKET,     SO_BROADCAST },
  { SocketOptions::SO_W_IP_TOS,    IPPROTO_IP,  IP_TOS },
  { SocketOptions::SO_W_IP_MULTICAST_IF,  IPPROTO_IP,   IP_MULTICAST_IF },
  { SocketOptions::SO_W_IP_MULTICAST_IF2,      IPPROTO_IP,   IP_MULTICAST_IF },
  { SocketOptions::SO_W_IP_MULTICAST_LOOP,    IPPROTO_IP,   IP_MULTICAST_LOOP },
 };
#endif

//--------------------------------------------------------------------------
InetAddress   SocketUtils::socketGetLocalAddress(__NET_HOOK_DECLARE_MORE const SocketDescriptor& sd)  __NET_THROW_DECLARE(SocketException)
{
  OPENCODE_TYPE_SOCKADDR him;
  OPENCODE_TYPE_SOCKLEN len = OPENCODE_SOCKADDR_LEN;
  OPENCODE_TYPE_SOCKET  fd = sd.getDescriptor();  
  int   family = sd.getFamily(); 
    
#ifdef AF_INET6
  if (family == AF_INET6)
  {
    if (::getsockname(fd, (struct sockaddr *)&him, (OPENCODE_TYPE_SOCKLEN*)&len) != OPENCODE_SOCKET_ERROR)
    {
      return NetworkUtils::makeInetAddress((struct sockaddr *)&him,AF_INET6);
    }
  }
  else
#endif
  {
    if (::getsockname(fd, (struct sockaddr *)&him, (OPENCODE_TYPE_SOCKLEN*)&len) != OPENCODE_SOCKET_ERROR)
    {
      return NetworkUtils::makeInetAddress((struct sockaddr *)&him,AF_INET);
    }
  }
  
  __NET_HOOK_RETURN(return InetAddress::nullAddress(),SocketException,"can not get local address");
}

int   SocketUtils::socketGetLocalPort(__NET_HOOK_DECLARE_MORE const SocketDescriptor& sd)  __NET_THROW_DECLARE(SocketException)
{
  OPENCODE_TYPE_SOCKADDR him;
  OPENCODE_TYPE_SOCKLEN len = OPENCODE_SOCKADDR_LEN;
  OPENCODE_TYPE_SOCKET  fd = sd.getDescriptor();  
  int   family = sd.getFamily(); 
  
#ifdef AF_INET6
  if (family == AF_INET6)
  {
    if (::getsockname(fd, (struct sockaddr *)&him, (OPENCODE_TYPE_SOCKLEN*)&len) != OPENCODE_SOCKET_ERROR)
    {
      struct sockaddr_in6* __sockaddrP_him6 = (struct sockaddr_in6 *)&him;
      return NETWORK_NTOHS(__sockaddrP_him6->sin6_port);
    }
  }
  else
#endif
  {
    if (::getsockname(fd, (struct sockaddr *)&him, (OPENCODE_TYPE_SOCKLEN*)&len) != OPENCODE_SOCKET_ERROR)
    {
      struct sockaddr_in* __sockaddrP_him4 = (struct sockaddr_in *)&him;
      return NETWORK_NTOHS(__sockaddrP_him4->sin_port);
    }
  }

  __NET_HOOK_RETURN(return 0,SocketException,"can not get local port");  
}
  
  
//--------------------------------------------------------------------------
        
void   SocketUtils::socketCreate(__NET_HOOK_DECLARE_MORE bool stream,SocketDescriptor* psd)  __NET_THROW_DECLARE(SocketException)
{
  OPENCODE_TYPE_SOCKET st_fd = OPENCODE_INVALID_SOCKET;
  int st_family = AF_INET;
  
#ifdef AF_INET6
  if (NetworkUtils::IPv6_available())
  {
    st_family = AF_INET6;
  }
  else
#endif
  {
    st_family = AF_INET;
  }
  
  st_fd = socket(st_family, (stream ? SOCK_STREAM: SOCK_DGRAM), 0);

  if(st_fd == OPENCODE_SOCKET_ERROR)
  {
    XLONG __tmp_errnum = OPENCODE_SOCKET_GETLASTERROR();
    __NET_HOOK_RETURN(return,SocketException,System::getLastError(__tmp_errnum),__tmp_errnum); 
  } 
  
  psd->setDescriptor(st_fd,st_family);
}

void   SocketUtils::socketBind(__NET_HOOK_DECLARE_MORE const SocketDescriptor& sd,const InetAddress& address, int port) __NET_THROW_DECLARE(SocketException)
{
  OPENCODE_TYPE_SOCKET  fd = sd.getDescriptor();      
  OPENCODE_TYPE_SOCKADDR him;
  if(InetAddress::isNullAddress(address))
  {
    NetworkUtils::make_sockaddr((struct sockaddr *)&him,InetAddress::anyAddress(),port);
  }
  else
  {
    NetworkUtils::make_sockaddr((struct sockaddr *)&him,address,port);
  }

#if defined(PORT_WINDOWS)
  int bind_rv = __netwin_bind(fd,(struct sockaddr*)&him, OPENCODE_SOCKADDR_LEN);
#else 
  int bind_rv = ::bind(fd,(struct sockaddr*)&him, OPENCODE_SOCKADDR_LEN);
#endif

  if(bind_rv == OPENCODE_SOCKET_ERROR)
  {
    XLONG __tmp_errnum = OPENCODE_SOCKET_GETLASTERROR();
    __NET_HOOK_RETURN(return,SocketException,System::getLastError(__tmp_errnum),__tmp_errnum); 
  }  
}

void   SocketUtils::socketListen(__NET_HOOK_DECLARE_MORE const SocketDescriptor& sd,int count) __NET_THROW_DECLARE(SocketException)
{
  OPENCODE_TYPE_SOCKET  fd = sd.getDescriptor();  
  int   family = sd.getFamily(); 
  
  int listen_rv = ::listen(fd,count);
  
  if(listen_rv == OPENCODE_SOCKET_ERROR)
  {
    XLONG __tmp_errnum = OPENCODE_SOCKET_GETLASTERROR();
    __NET_HOOK_RETURN(return,SocketException,System::getLastError(__tmp_errnum),__tmp_errnum); 
  }  
}
 
void   SocketUtils::socketClose(const SocketDescriptor& sd)
{
  OPENCODE_TYPE_SOCKET  fd = sd.getDescriptor();  
#if defined(PORT_WINDOWS)
  __netwin_socketClose(fd);
#else
  ::close(fd);
#endif
}

void   SocketUtils::socketShutdown(const SocketDescriptor& sd,int howto)
{
  OPENCODE_TYPE_SOCKET  fd = sd.getDescriptor();   
  ::shutdown(fd,howto);
}

int   SocketUtils::socketRead(__NET_HOOK_DECLARE_MORE const SocketDescriptor& sd,char* b, int length,unsigned  int flags) __NET_THROW_DECLARE(SocketException,ConnectionResetException)
{
  return SocketUtils::socketRead(__NET_HOOK_INVOKE_MORE sd,b,length,flags,0);   
}

int   SocketUtils::socketRecvFrom(__NET_HOOK_DECLARE_MORE const SocketDescriptor& sd,SocketAddress* paddr,char* buf, int len, unsigned int flags) __NET_THROW_DECLARE(SocketException,ConnectionResetException)
{
  return SocketUtils::socketRecvFrom(__NET_HOOK_INVOKE_MORE sd,paddr,buf,len,flags,0);   
}


//------------------------------------------------------------
// static socket  Options Support
//------------------------------------------------------------
void   SocketUtils::socketSetOption(__NET_HOOK_DECLARE_MORE const SocketDescriptor& sd,int cmd, bool on, const SocketOptionsValue& value) __NET_THROW_DECLARE(SocketException)
{
  OPENCODE_TYPE_SOCKET  fd = sd.getDescriptor();  
  OPENCODE_TYPE_SOCKLEN optlen; 

  if (cmd == SOCKETOPTIONS_SO_W_TIMEOUT) 
  {
#if defined(PORT_WINDOWS)  
  /*
   * SO_RCVTIMEO is only supported on Microsoft's implementation
   * of Windows Sockets so if WSAENOPROTOOPT returned then
   * reset flag and timeout will be implemented using 
   * select() -- see socketRead.
   */
  if (NetworkUtils::isRcvTimeoutSupported()) 
  {  
      int timeout  = value.sov_int_value;
      /* 
       * Disable SO_RCVTIMEO if timeout is <= 5 second.
       */
      if (timeout <= 5000)
      {
        timeout = 0;
      }

      if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (OPENCODE_TYPE_SOCKSOPT)&timeout, sizeof(timeout)) == OPENCODE_SOCKET_ERROR) 
      {
      if (WSAGetLastError() == WSAENOPROTOOPT) 
      {
        NetworkUtils::setRcvTimeoutSupported(false);
      } 
      else 
      {
        __NET_HOOK_RETURN(return,SocketException,"setsockopt SO_RCVTIMEO",OPENCODE_SOCKET_GETLASTERROR());
      }
    }     
  }
#else
  /** SO_W_TIMEOUT is a no-op on non-windows*/
#endif  
    return;
  }  
   
  union 
  {
     int i;
     char c;
    struct linger ling;
    struct in_addr in4;
  } optval;
 
  int level, optname; 
  if (socketMapOption(fd,cmd, &level, &optname) != 0)
  {
    __NET_HOOK_RETURN(return,SocketException,"Invalid option");
  }
 
  if( (cmd == SOCKETOPTIONS_SO_W_IP_MULTICAST_IF) || (cmd == SOCKETOPTIONS_SO_W_IP_MULTICAST_IF2)) 
  {
    /** Setting the multicast interface handled seperately */
    setMulticastInterface(__NET_HOOK_INVOKE_MORE sd,cmd,level,optname,value);
    return;
  }
 
  switch (cmd) 
  {
    case SOCKETOPTIONS_SO_W_SNDBUF :
    case SOCKETOPTIONS_SO_W_RCVBUF :
    case SOCKETOPTIONS_SO_W_LINGER :
    case SOCKETOPTIONS_SO_W_IP_TOS :
    {   
      if (cmd == SOCKETOPTIONS_SO_W_LINGER) 
      {
        if (on) 
        {
          optval.ling.l_onoff = 1;
          optval.ling.l_linger = value.sov_int_value;
        } 
        else 
        {
          optval.ling.l_onoff = 0;
          optval.ling.l_linger = 0;
        }
        optlen = sizeof(optval.ling);
      }
      else 
      {
        optval.i = value.sov_int_value;
        optlen = sizeof(optval.i);
      }   
    }
    break;

//-------------------------------------------------------------------------------
// DatagramSocket SetOptions --BEGIN
//-------------------------------------------------------------------------------

    case SOCKETOPTIONS_SO_W_IP_MULTICAST_LOOP :
    {
      optval.c = (!on ? 1 : 0);
      optlen = sizeof(optval.c);  
    }
    break;     
    
//-------------------------------------------------------------------------------
// DatagramSocket SetOptions --END
//-------------------------------------------------------------------------------
  
    /* bool -> int */
    default :
    {
      optval.i = (on ? 1 : 0);
      optlen = sizeof(optval.i);
    }
  }
  
  if(::setsockopt(fd,level,optname,(OPENCODE_TYPE_SOCKSOPT)&optval, optlen)<0)
  {	
    XLONG __tmp_errnum = OPENCODE_SOCKET_GETLASTERROR();
    __NET_HOOK_RETURN(return,SocketException,System::getLastError(__tmp_errnum),__tmp_errnum); 
  } 
}

SocketOptionsValue   SocketUtils::socketGetOption(__NET_HOOK_DECLARE_MORE const SocketDescriptor& sd,int cmd) __NET_THROW_DECLARE(SocketException)
{
  OPENCODE_TYPE_SOCKET  fd = sd.getDescriptor();  
  
  int level;
  int optname;
  OPENCODE_TYPE_SOCKLEN optlen;

  union 
  {
    char c;
    int i;
    struct linger ling;
    struct in_addr in4;

  } optval;

  SocketOptionsValue value;
  
  /*
   * SO_W_BINDADDR isn't a socket option
   */
  if (cmd == SocketOptions::SO_W_BINDADDR) 
  {
    value.sov_type = SocketOptionsValue::SOV_INETADDRESS;
    value.sov_inet_value = SocketUtils::socketGetLocalAddress(__NET_HOOK_INVOKE_MORE fd);
    __NET_HOOK_CHECK(return value);
    
    return value;
  } 

  /*
   * Map the  level socket option to the platform specific
   * level and option name.
   */
  if (socketMapOption(fd,cmd, &level, &optname) != 0) 
  {
    __NET_HOOK_RETURN(return value,SocketException,"Invalid option");
  }

  /** Handle IP_MULTICAST_IF seperately */
  if (cmd == SocketOptions::SO_W_IP_MULTICAST_IF || cmd == SocketOptions::SO_W_IP_MULTICAST_IF2)
  {
    return getMulticastInterface(__NET_HOOK_INVOKE_MORE sd,cmd,level,optname);
  }

  /*
   * Args are int except for SO_W_LINGER
   */
  if (cmd == SocketOptions::SO_W_LINGER) 
  {
    optlen = sizeof(optval.ling);
  } 
  else if(cmd == SocketOptions::SO_W_IP_MULTICAST_LOOP)
  {
    optlen = sizeof(optval.c);
  } 
  else
  {
    optlen = sizeof(optval.i);
  }

  if(::getsockopt(fd, level, optname, (OPENCODE_TYPE_SOCKGOPT)&optval, (OPENCODE_TYPE_SOCKLEN*)&optlen) == OPENCODE_SOCKET_ERROR)
  {
    XLONG __tmp_errnum = OPENCODE_SOCKET_GETLASTERROR();
    __NET_HOOK_RETURN(return value,SocketException,System::getLastError(__tmp_errnum),__tmp_errnum); 
  } 

  switch (cmd) 
  {
    case SOCKETOPTIONS_SO_W_SNDBUF :
    case SOCKETOPTIONS_SO_W_RCVBUF :
    case SOCKETOPTIONS_SO_W_LINGER :
    case SOCKETOPTIONS_SO_W_IP_TOS :
    {
      if(cmd == SOCKETOPTIONS_SO_W_LINGER)
      {
        if(optval.ling.l_onoff)
        {
          value.sov_type = SocketOptionsValue::SOV_BOOLINT;
          value.sov_bol_value = true;
          value.sov_int_value = optval.ling.l_linger;           
        }
        else
        {
          value.sov_type = SocketOptionsValue::SOV_BOOLEAN;
          value.sov_bol_value = false;
        }//~end if(optval.ling.l_onoff)
        
      }
      else
      {
        value.sov_type = SocketOptionsValue::SOV_INTEGER;
        value.sov_int_value = optval.i;    
        
      }//~end if(cmd == SOCKETOPTIONS_SO_W_LINGER)
    }
    break;

//-------------------------------------------------------------------------------
// DatagramSocket GetOptions --BEGIN
//-------------------------------------------------------------------------------

    case SOCKETOPTIONS_SO_W_IP_MULTICAST_LOOP :
    {
      value.sov_type = SocketOptionsValue::SOV_BOOLEAN;      
      value.sov_bol_value = (optval.i == 0);   
    }
    break;  

//-------------------------------------------------------------------------------
// DatagramSocket GetOptions --END
//-------------------------------------------------------------------------------

    default :
    {
      value.sov_type = SocketOptionsValue::SOV_BOOLEAN;      
      value.sov_bol_value = (optval.i != 0);
    }
    break;
  }    
  
  return value;
}

void   SocketUtils::socketSetTimeToLive(__NET_HOOK_DECLARE_MORE const SocketDescriptor& sd,int ttl) __NET_THROW_DECLARE(SocketException)
{
  OPENCODE_TYPE_SOCKET  fd = sd.getDescriptor();  
  
  char ittl = (char)ttl;
  OPENCODE_TYPE_SOCKLEN len = sizeof(ittl);
  
  int level = IPPROTO_IP;
  int optname = IP_MULTICAST_TTL;
  
#ifdef AF_INET6
  if (NetworkUtils::IPv6_available())
  { 
    level = IPPROTO_IPV6;
    optname = IPV6_MULTICAST_HOPS;
  }
#endif
  
  if(::setsockopt(fd,level,optname,(OPENCODE_TYPE_SOCKSOPT)&ittl, len) == OPENCODE_SOCKET_ERROR)
  {
    XLONG __tmp_errnum = OPENCODE_SOCKET_GETLASTERROR();
    __NET_HOOK_RETURN(return,SocketException,System::getLastError(__tmp_errnum),__tmp_errnum); 
  }
}

int   SocketUtils::socketGetTimeToLive(__NET_HOOK_DECLARE_MORE const SocketDescriptor& sd) __NET_THROW_DECLARE(SocketException)
{
  OPENCODE_TYPE_SOCKET  fd = sd.getDescriptor();  
  
  unsigned char ttl = 0;
  OPENCODE_TYPE_SOCKLEN len = sizeof(ttl); 
  
  int level = IPPROTO_IP;
  int optname = IP_MULTICAST_TTL;
  
#ifdef AF_INET6
  if (NetworkUtils::IPv6_available())
  { 
    level = IPPROTO_IPV6;
    optname = IPV6_MULTICAST_HOPS;
  }
#endif

  if(::getsockopt(fd, level, optname, (OPENCODE_TYPE_SOCKGOPT)&ttl, (OPENCODE_TYPE_SOCKLEN*)&len) == OPENCODE_SOCKET_ERROR)
  {
    XLONG __tmp_errnum = OPENCODE_SOCKET_GETLASTERROR();
    __NET_HOOK_RETURN(return -1,SocketException,System::getLastError(__tmp_errnum),__tmp_errnum); 
  } 

  
  return (int)ttl;
}


int   SocketUtils::socketMapOption(const SocketDescriptor& sd,int cmd,int *level, int *optname)
{
  OPENCODE_TYPE_SOCKET  fd = sd.getDescriptor();  
  
#ifdef AF_INET6
  if (NetworkUtils::IPv6_available())
  { 
    switch (cmd) 
    {
      case SOCKETOPTIONS_SO_W_IP_MULTICAST_IF:
      case SOCKETOPTIONS_SO_W_IP_MULTICAST_IF2:
      {
        *level = IPPROTO_IPV6;
        *optname = IPV6_MULTICAST_IF;
        return 0;
      }
      
      case SOCKETOPTIONS_SO_W_IP_MULTICAST_LOOP:
      {
        *level = IPPROTO_IPV6;
        *optname = IPV6_MULTICAST_LOOP;
        return 0;
      }  
    }  
  }
#endif

  int opt_size = (int)(sizeof(SocketUtils::opts) / sizeof(SocketUtils::opts[0]));
  for (int i = 0; i < opt_size; i++) 
  {
    if (cmd == SocketUtils::opts[i].cmd) 
    {
      *level = SocketUtils::opts[i].level;
      *optname = SocketUtils::opts[i].optname;
      return 0;
    }
  }

  /* not found */
  return -1;
}

void   SocketUtils::setMulticastInterface(__NET_HOOK_DECLARE_MORE const SocketDescriptor& sd,int cmd,int level,int optname, const SocketOptionsValue& value) __NET_THROW_DECLARE(SocketException)
{
  OPENCODE_TYPE_SOCKLEN optlen;
  OPENCODE_TYPE_SOCKET  fd = sd.getDescriptor();

  union
  {
    char c;
    int i;
    struct linger ling;
    struct in_addr in4;
  } optval;


  NetworkInterface __netif;

  if (cmd == SOCKETOPTIONS_SO_W_IP_MULTICAST_IF)
  {
   /**
    * value is an InetAddress.
    * On IPv4 system use IP_MULTICAST_IF socket option
    * On IPv6 system get the NetworkInterface that this IP
    * address is bound too and use the IPV6_MULTICAST_IF 
    * option instead of IP_MULTICAST_IF
    */
   
  #ifdef AF_INET6
    if (NetworkUtils::IPv6_available())
    {
      __netif = NetworkInterface::getByInetAddress(__NET_HOOK_INVOKE_MORE value.sov_inet_value);;
      __NET_HOOK_CHECK(return);
      
      optname = SOCKETOPTIONS_SO_W_IP_MULTICAST_IF2;
    }
    else
  #endif
    {
      OPENCODE_TYPE_INET4ADDRESS  addr_t;
      memcpy(&addr_t,value.sov_inet_value.getAddress(),
             __M_min((int)sizeof(addr_t),value.sov_inet_value.getLength()));
             
      optval.in4.s_addr = addr_t;  
      optlen = sizeof(optval.in4.s_addr);
    }
  }
  
  if (cmd == SOCKETOPTIONS_SO_W_IP_MULTICAST_IF2)
  {
    /**
     * value is a NetworkInterface.
     * On IPv6 system get the index of the interface and use the IPV6_MULTICAST_IF socket option
     * On IPv4 system extract addr[0] and use the IP_MULTICAST_IF option.
     */
  #ifdef AF_INET6
    if (NetworkUtils::IPv6_available())
    {
      optval.i = __netif.getIndex();
      optlen = sizeof(optval.i);      
    }
    else
  #endif
    {
      /** Check that there is at least one address bound to this interface.  */
      if (__netif.getInetAddresses().empty())
      {
        __NET_HOOK_RETURN(return,SocketException,"bad argument for IP_MULTICAST_IF2: No IP addresses bound to interface");
      }
  
      InetAddress addr = __netif.getInetAddresses()[0];
      OPENCODE_TYPE_INET4ADDRESS  addr_t;
      memcpy(&addr_t,addr.getAddress(),__M_min((int)sizeof(addr_t),addr.getLength()));
      optval.in4.s_addr = addr_t;
      optlen = sizeof(optval.in4.s_addr);
    }
  }

  if(::setsockopt(fd,level,optname,(OPENCODE_TYPE_SOCKSOPT)&optval, optlen)<0)
  {
    XLONG __tmp_errnum = OPENCODE_SOCKET_GETLASTERROR();
    __NET_HOOK_RETURN(return,SocketException,System::getLastError(__tmp_errnum),__tmp_errnum); 
  }
}

SocketOptionsValue   SocketUtils::getMulticastInterface(__NET_HOOK_DECLARE_MORE const SocketDescriptor& sd,int cmd,int level,int optname) __NET_THROW_DECLARE(SocketException)
{
  OPENCODE_TYPE_SOCKLEN optlen;
  OPENCODE_TYPE_SOCKET  fd = sd.getDescriptor();
  SocketOptionsValue value;
  
  union
  {
    char c;
    int i;
    struct linger ling;
    struct in_addr in4;
  } optval;

  bool isIPV4 = true;

#ifdef AF_INET6
  if (NetworkUtils::IPv6_available())
  {
    isIPV4 = false;
  }
#endif
  
  if(isIPV4)
  {  
    optlen = sizeof(optval.in4);
    if(::getsockopt(fd, level, optname, (OPENCODE_TYPE_SOCKGOPT)&optval, (OPENCODE_TYPE_SOCKLEN*)&optlen) == OPENCODE_SOCKET_ERROR)
    {
	    XLONG __tmp_errnum = OPENCODE_SOCKET_GETLASTERROR();
	    __NET_HOOK_RETURN(return value,SocketException,System::getLastError(__tmp_errnum),__tmp_errnum); 
    }

    /** For IP_MULTICAST_IF return InetAddress */
    if (cmd == SOCKETOPTIONS_SO_W_IP_MULTICAST_IF)
    {
      value.sov_type = SocketOptionsValue::SOV_INETADDRESS;
      value.sov_inet_value = NetworkUtils::makeInetAddress(optval.in4);
      return value;
    } 

    /** For IP_MULTICAST_IF2 we get the NetworkInterface for this address and return it */      
    value.sov_type = SocketOptionsValue::SOV_NETWORKINTERFACE;
    value.sov_netif_value = NetworkUtils::netifGetByInetAddress(__NET_HOOK_INVOKE_MORE NetworkUtils::makeInetAddress(optval.in4));
    __NET_HOOK_CHECK(return value);
    
    return value;   
  }

#ifdef AF_INET6
    /** IPv6 implementation */
    if ((cmd == SOCKETOPTIONS_SO_W_IP_MULTICAST_IF) || 
        (cmd == SOCKETOPTIONS_SO_W_IP_MULTICAST_IF2)) 
    {
      optlen = sizeof(optval.i); 
      if(::getsockopt(fd, level, optname, (OPENCODE_TYPE_SOCKGOPT)&optval, (OPENCODE_TYPE_SOCKLEN*)&optlen) == OPENCODE_SOCKET_ERROR)
      {
        XLONG __tmp_errnum = OPENCODE_SOCKET_GETLASTERROR();
        __NET_HOOK_RETURN(return value,SocketException,System::getLastError(__tmp_errnum),__tmp_errnum);
      } 
      
      int index = optval.i;
            
      /**
       * If multicast to a specific interface then return the 
       * interface (for IF2) or the any address on that interface
       * (for IF).
       */
      if(index > 0)
      {
        value.sov_netif_value = NetworkInterface::getByIndex(__NET_HOOK_INVOKE_MORE index);  
        __NET_HOOK_CHECK(return value);
         
        /** For IP_MULTICAST_IF2 return the NetworkInterface*/
        if (cmd == SOCKETOPTIONS_SO_W_IP_MULTICAST_IF2) 
        {
          value.sov_type = SocketOptionsValue::SOV_NETWORKINTERFACE;
          return value;
        }
        
        /** For IP_MULTICAST_IF return addrs[0] */
        value.sov_type = SocketOptionsValue::SOV_INETADDRESS;
        value.sov_inet_value = value.sov_netif_value.getInetAddresses()[0];     
      }
   }  
#endif

  return value;
}

//////////////////////////////////////////////////////////
#if !defined(PORT_WINDOWS)
//////////////////////////////////////////////////////////

void   SocketUtils::socketAccept(__NET_HOOK_DECLARE_MORE const SocketDescriptor& sd,SocketDescriptor* psd,SocketAddress* paddr,XLONG timeout) __NET_THROW_DECLARE(SocketException)
{
  if(timeout <= 0)
  {
    SocketUtils::socketAccept(__NET_HOOK_INVOKE_MORE sd,psd,paddr);
    return;
  }
  
  /* 
   * accept connection but ignore ECONNABORTED indicating that
   * connection was eagerly accepted by the OS but was reset
   * before accept() was called.
   *
   * If accept timeout in place and timeout is adjusted with
   * each ECONNABORTED to ensure that semantics of timeout are
   * preserved.
   */

  OPENCODE_TYPE_SOCKADDR addr_c;  
  OPENCODE_TYPE_SOCKLEN len = OPENCODE_SOCKADDR_LEN;
  OPENCODE_TYPE_SOCKET newfd;
  XLONG prevTime = 0;

  OPENCODE_TYPE_SOCKET  fd = sd.getDescriptor();  
  int   family = sd.getFamily(); 
   
  for (;;) 
  {
    if (timeout != 0) 
    {
      int ret = 0;

      /* first usage pick up current time */
      if (prevTime == 0) 
      {
        prevTime = System::currentTimeMillis();
      }

      ret = socketTimeout(fd, timeout,TIMEOUT_TYPE_ACCEPT);
      
      if (ret == 0) 
      {
        __NET_HOOK_RETURN(return,SocketException,"Accept timed out");
      } 
      else if( ret < 0)
      {
        XLONG __tmp_errnum = OPENCODE_SOCKET_GETLASTERROR();
        __NET_HOOK_RETURN(return,SocketException,System::getLastError(__tmp_errnum),__tmp_errnum);
      }
    }

    newfd = ::accept(fd,(struct sockaddr*)&addr_c,(OPENCODE_TYPE_SOCKLEN*)&len);
 
    /* connection accepted */
    if (newfd >= 0) 
    {
      break;
    }
  
    /* non-ECONNABORTED error */
    if (errno != ECONNABORTED) 
    {
      break;
    }
  
    /* ECONNABORTED error so adjust timeout */ 
    if (timeout != 0) 
    {
      XLONG currTime = System::currentTimeMillis();
      timeout -= (currTime - prevTime);
    
      if (timeout <= 0) 
      {
        __NET_HOOK_RETURN(return,SocketException,"Accept timed out");
      }
      prevTime = currTime;
    }
  }

  if (newfd < 0) 
  {
    if (errno == EINVAL) 
    {
       errno = EBADF;
    }
    
    if(errno != 0)
    {
      __NET_HOOK_RETURN(return,SocketException,System::getLastError(errno),errno);
    } 
  }
  
  *psd = newfd;

#ifdef AF_INET6
  if (NetworkUtils::IPv6_available())
  {
    *paddr = NetworkUtils::makeSocketAddress((struct sockaddr *)&addr_c,AF_INET6);
  }
  else
#endif
  {
    *paddr = NetworkUtils::makeSocketAddress((struct sockaddr *)&addr_c,AF_INET);  
  }
}
 
void   SocketUtils::socketAccept(__NET_HOOK_DECLARE_MORE const SocketDescriptor& sd,SocketDescriptor* psd,SocketAddress* paddr) __NET_THROW_DECLARE(SocketException)
{ 
  OPENCODE_TYPE_SOCKADDR addr_c;
  OPENCODE_TYPE_SOCKLEN len = OPENCODE_SOCKADDR_LEN;
  OPENCODE_TYPE_SOCKET newfd;
  
  OPENCODE_TYPE_SOCKET  fd = sd.getDescriptor();  
  int   family = sd.getFamily(); 
  
  /* 
   * accept connection but ignore ECONNABORTED indicating that
   * connection was eagerly accepted by the OS but was reset
   * before accept() was called.
   *
   */
  for (;;) 
  {
    newfd = ::accept(fd,(struct sockaddr*)&addr_c,(OPENCODE_TYPE_SOCKLEN*)&len);

    /* connection accepted */
    if (newfd >= 0) 
    {
      break;
    }
  
    /* non-ECONNABORTED error */
    if (errno != ECONNABORTED) 
    {
      break;
    }
  }

  if (newfd < 0) 
  {
    if (errno == EINVAL) 
    {
      errno = EBADF;
    }
    
    if(errno != 0)
    {
      __NET_HOOK_RETURN(return,SocketException,System::getLastError(errno),errno);
    } 
  }
  
  psd->setDescriptor(newfd,family);
  
#ifdef AF_INET6
  if (NetworkUtils::IPv6_available())
  {
    *paddr = NetworkUtils::makeSocketAddress((struct sockaddr *)&addr_c,family);
  }
  else
#endif
  {
    *paddr = NetworkUtils::makeSocketAddress((struct sockaddr *)&addr_c,AF_INET);
  }
}

void   SocketUtils::socketConnect(__NET_HOOK_DECLARE_MORE const SocketDescriptor& sd,const InetAddress& address, int port, XLONG timeout) __NET_THROW_DECLARE(SocketException)
{
  if (timeout <= 0) 
  {
    SocketUtils::socketConnect(__NET_HOOK_INVOKE_MORE sd,address,port);
    return;
  }
  
  OPENCODE_TYPE_SOCKET  fd = sd.getDescriptor();  
  int   family = sd.getFamily(); 
  OPENCODE_TYPE_SOCKADDR him;
  NetworkUtils::make_sockaddr((struct sockaddr*)&him,address,port);

  /*
   * A timeout was specified. We put the socket into non-blocking
   * mode, connect, and then wait for the connection to be
   * established, fail, or timeout.
   */
  SET_NONBLOCKING(fd);

  int connect_rv = connect(fd, (struct sockaddr *)&him, OPENCODE_SOCKADDR_LEN);   

  /* connection not established immediately */
  if (connect_rv != 0) 
  {
    int optlen;
    XLONG prevTime = System::currentTimeMillis();

    if (errno != EINPROGRESS) 
    {
       SET_BLOCKING(fd);
       __NET_HOOK_RETURN(return,SocketException,System::getLastError(errno),errno);
    }
      
    connect_rv = socketTimeout(sd,timeout,TIMEOUT_TYPE_CONNECT);
    
    if (connect_rv == 0)
    {
      SET_BLOCKING(fd);
      __NET_HOOK_RETURN(return,SocketException,"Connect timed out");
    } 
    
  }//~end if (connect_rv != 0) 

  SET_BLOCKING(fd);
  
  if(connect_rv < 0)
  {
    XLONG __tmp_errnum = OPENCODE_SOCKET_GETLASTERROR();
    __NET_HOOK_RETURN(return,SocketException,System::getLastError(__tmp_errnum),__tmp_errnum);
  } 
}

void   SocketUtils::socketConnect(__NET_HOOK_DECLARE_MORE const SocketDescriptor& sd,const InetAddress& address, int port) __NET_THROW_DECLARE(SocketException)
{
  OPENCODE_TYPE_SOCKET  fd = sd.getDescriptor();  
  int   family = sd.getFamily(); 
  
  OPENCODE_TYPE_SOCKADDR him;
  NetworkUtils::make_sockaddr((struct sockaddr *)&him,address,port);

  int connect_rv = connect(fd, (struct sockaddr *)&him, OPENCODE_SOCKADDR_LEN);

  if(connect_rv != 0)
  {
    XLONG __tmp_errnum = OPENCODE_SOCKET_GETLASTERROR();
    __NET_HOOK_RETURN(return,SocketException,System::getLastError(__tmp_errnum),__tmp_errnum);
  }  
}

int   SocketUtils::socketRead(__NET_HOOK_DECLARE_MORE const SocketDescriptor& sd,char* b, int length,unsigned  int flags,XLONG timeout) __NET_THROW_DECLARE(SocketException,ConnectionResetException)
{
  OPENCODE_TYPE_SOCKET  fd = sd.getDescriptor();  
  
  int read_rv = 0;  
  
  while(true)
  {
    if(timeout >0 )
    {
      timeout = socketTimeout(fd, timeout,TIMEOUT_TYPE_READ);
      if (timeout == 0) 
      {
        __NET_HOOK_RETURN(return -1,SocketException,"Read timed out");
        break;
      } 
      else if( timeout < 0)
      {
        XLONG __tmp_errnum = OPENCODE_SOCKET_GETLASTERROR();
        __NET_HOOK_RETURN(return -1,SocketException,System::getLastError(__tmp_errnum),__tmp_errnum);
        break;
      }
    }
      
    read_rv = ::recv(fd,b,length,flags);

    if(read_rv > 0)
    {
      return read_rv;
    }  
  
    if(read_rv <= 0)
    {
      if(read_rv < 0)
      {
        if(errno==EINTR)
        {
          continue;
        }
        else
        { 
           switch(errno)
           {
             case ECONNRESET:
             case EPIPE:
             {
						   XLONG __tmp_errnum = OPENCODE_SOCKET_GETLASTERROR();
						   __NET_HOOK_RETURN(return -1,ConnectionResetException,System::getLastError(__tmp_errnum),__tmp_errnum);
             }
             break; 
             
             default:
             {
						   XLONG __tmp_errnum = OPENCODE_SOCKET_GETLASTERROR();
						   __NET_HOOK_RETURN(return -1,SocketException,System::getLastError(__tmp_errnum),__tmp_errnum);
             }
             break;
           }
        }//~end if(errno==EINTR)

      } //~end if(read_rv < 0)
  
    }//~end if(read_rv <= 0)
  
    break;

  }//~end while(true)

  return read_rv; 
   
}

int   SocketUtils::socketRecvFrom(__NET_HOOK_DECLARE_MORE const SocketDescriptor& sd,SocketAddress* paddr,char* buf, int len, unsigned int flags,XLONG timeout) __NET_THROW_DECLARE(SocketException,ConnectionResetException)
{
  OPENCODE_TYPE_SOCKET  fd = sd.getDescriptor();  
  
  OPENCODE_TYPE_SOCKADDR addr; 
  OPENCODE_TYPE_SOCKLEN fromlen = OPENCODE_SOCKADDR_LEN;

  int recv_rv = 0;
 
  while(true)
  {
    if(timeout > 0)
    {
      timeout = socketTimeout(fd, timeout,TIMEOUT_TYPE_READ);
      if (timeout == 0) 
      {
        __NET_HOOK_RETURN(return -1,SocketException,"Read timed out");
      } 
      else if( timeout < 0)
      {
				XLONG __tmp_errnum = OPENCODE_SOCKET_GETLASTERROR();
				__NET_HOOK_RETURN(return -1,SocketException,System::getLastError(__tmp_errnum),__tmp_errnum);
      }
    }

    if(paddr == 0)
    { 
      recv_rv = ::recvfrom(fd, buf, len, flags, (struct sockaddr *)&addr, (OPENCODE_TYPE_SOCKLEN*)&fromlen);
    }
    else
    {
      recv_rv = ::recvfrom(fd, buf, len, flags, (struct sockaddr *)&addr, (OPENCODE_TYPE_SOCKLEN*)&fromlen);
    }

    if(recv_rv > 0)
    {
      return recv_rv;
    }

    if(recv_rv <= 0)
    {
      if(recv_rv < 0)
      {
        if(errno == EINTR)
        {
          continue;
        }
        else
        {
           switch(errno)
           {
             case ECONNRESET:
             case EPIPE:
             {
						   XLONG __tmp_errnum = OPENCODE_SOCKET_GETLASTERROR();
						   __NET_HOOK_RETURN(return -1,ConnectionResetException,System::getLastError(__tmp_errnum),__tmp_errnum);               	
             }
             break;

             default:
             {
						   XLONG __tmp_errnum = OPENCODE_SOCKET_GETLASTERROR();
						   __NET_HOOK_RETURN(return -1,SocketException,System::getLastError(__tmp_errnum),__tmp_errnum); 
             }
           }
        }//~end if(errno == EINTR)

      } //~end if(recv_rv < 0)

    }//~end if(recv_rv <= 0)

    break;

  }//~end while(true)

  return recv_rv;   
}

void   SocketUtils::socketWrite(__NET_HOOK_DECLARE_MORE const SocketDescriptor& sd,const char* b, int length,unsigned  int flags) __NET_THROW_DECLARE(SocketException)
{
  OPENCODE_TYPE_SOCKET  fd = sd.getDescriptor();  
  
  int writeBytes = 0;
  
  while(writeBytes < length)
  {
    int ret_code=::send(fd,b+writeBytes,length-writeBytes,flags);
 
    if(ret_code == 0)
    {
      break;
    } 
    else if(ret_code > 0)
    {
      writeBytes += ret_code;
      continue;
    }       
    else if( (ret_code < 0) && (errno == EINTR))
    { 
      continue;
    } 

		XLONG __tmp_errnum = OPENCODE_SOCKET_GETLASTERROR();
		__NET_HOOK_RETURN(return,SocketException,System::getLastError(__tmp_errnum),__tmp_errnum);     
  }
}

void   SocketUtils::socketSendTo(__NET_HOOK_DECLARE_MORE const SocketDescriptor& sd,const SocketAddress* paddr,const char* msg, int len,  unsigned  int flags) __NET_THROW_DECLARE(SocketException)
{
  OPENCODE_TYPE_SOCKET  fd = sd.getDescriptor();  
  
  OPENCODE_TYPE_SOCKADDR addr; 
  NetworkUtils::make_sockaddr((struct sockaddr *)&addr,*paddr); 
  int tolen = OPENCODE_SOCKADDR_LEN;
  
  int writeBytes = 0;
  int send_rv = 0;
  while(writeBytes < len)
  {
    send_rv = ::sendto(fd, msg+writeBytes, len - writeBytes, flags, (struct sockaddr *)&addr, tolen);

    if(send_rv == 0)
    {
      break;
    } 
    else if(send_rv > 0)
    {
      writeBytes += send_rv;
      continue;
    }       
    else if( (send_rv < 0) && (errno == EINTR))
    { 
      continue;
    } 
    
		XLONG __tmp_errnum = OPENCODE_SOCKET_GETLASTERROR();
		__NET_HOOK_RETURN(return,SocketException,System::getLastError(__tmp_errnum),__tmp_errnum);  
  }  
}

void   SocketUtils::socketSendUrgentData(__NET_HOOK_DECLARE_MORE const SocketDescriptor& sd,int data) __NET_THROW_DECLARE(SocketException)
{
  OPENCODE_TYPE_SOCKET  fd = sd.getDescriptor();  
  
  while(true)
  {
    int send_rv = ::send(fd,(char *)&data, 1, MSG_OOB);
    
    if(send_rv >= 0)
    {
      break;
    }
    
    if(send_rv < 0 && errno == EINTR)
    { 
      continue;  
    } 
    else
    {
		  XLONG __tmp_errnum = OPENCODE_SOCKET_GETLASTERROR();
		  __NET_HOOK_RETURN(return,SocketException,System::getLastError(__tmp_errnum),__tmp_errnum);  
    }  
  }
}

int   SocketUtils::socketTimeout(const SocketDescriptor& sd, XLONG ptimeout,int type)
{
  XLONG prevtime = 0;
  XLONG newtime = 0;
  
  struct timeval t;
  
  /* 
   * Pick up current time as may need to adjust timeout
   */
  if (ptimeout > 0) 
  {    
    prevtime = System::currentTimeMillis();
  }

  OPENCODE_TYPE_SOCKET  fd = sd.getDescriptor();  
    
  for(;;) 
  {
    struct pollfd pfd;
    pfd.fd = fd;
    pfd.events = POLLIN | POLLERR;
    
    int rv = poll(&pfd, 1, ptimeout);
  
    /*
    * If interrupted then adjust timeout. If timeout
    * has expired return 0 (indicating timeout expired).
    */
    if (rv < 0 && errno == EINTR) 
    {
      if (ptimeout > 0) 
      {      
        newtime = System::currentTimeMillis();
        ptimeout -= (newtime - prevtime);
        
        if (ptimeout <= 0) 
        {
           return 0;
        }
        prevtime = newtime;
      }
    } 
    else 
    {
      return rv;
    }  
  }
}
//////////////////////////////////////////////////////////
#endif
//////////////////////////////////////////////////////////

//=======================================================================================
//   -- END --
//=======================================================================================

__OPENCODE_END_NAMESPACE
