#include "socket/socketUtils.h"
#include "socket/socketDescriptor.h"
#include "socket/inetAddress.h"
#include "socket/socketException.h"
#include "socket/socketOptions.h"
#include "socket/socketImpl.h"
#include "socket/socket.h"
#include "socket/serverSocket.h"
#include "socket/networkUtils.h"

#include "port/libsys.h"

#if defined(PORT_WINDOWS)
#ifdef __cplusplus
extern "C" {                /* Assume C declarations for C++   */
#endif  /* __cplusplus */

/*
 * Initialize Windows Sockets API support
 */
BOOL WINAPI DllMain(HINSTANCE hinst, DWORD reason, LPVOID reserved)
{
	WSADATA wsadata;
	
	switch (reason) 
	{
		case DLL_PROCESS_ATTACH:
		{
		  if (WSAStartup(MAKEWORD(2,2), &wsadata) != 0) 
		  {
				return FALSE;
		  }
		}
		break;
		case DLL_PROCESS_DETACH:
		{
		  WSACleanup();
		  break;
		}		
		default:
		{
		}
		break;
	}
	return TRUE;
}

#ifdef __cplusplus
}                                    /* End of extern "C" { */
#endif  /* __cplusplus */

#endif

__OPENCODE_BEGIN_NAMESPACE

#if defined(PORT_WINDOWS)

#ifndef IPTOS_TOS_MASK
#define IPTOS_TOS_MASK 0x1e
#endif
#ifndef IPTOS_PREC_MASK
#define IPTOS_PREC_MASK 0xe0
#endif 

/*
 * Return the default TOS value
 */
int __netwin_getDefaultTOS() 
{
	static int default_tos = -1;
	OSVERSIONINFO ver;
	HKEY hKey;
	LONG ret;
	
	/*
	 * If default ToS already determined then return it
	 */
	if (default_tos >= 0) 
	{
	  return default_tos;
	}
	
	/*
	 * Assume default is "normal service"
	 */	
	default_tos = 0;
	
	/* 
	 * Which OS is this?
	 */
	ver.dwOSVersionInfoSize = sizeof(ver);
	GetVersionEx(&ver);
	
	/*
	 * If 2000 or greater then no default ToS in registry
	 */
	if (ver.dwPlatformId == VER_PLATFORM_WIN32_NT) 
	{
		if (ver.dwMajorVersion >= 5) 
		{
		  return default_tos;
		}
	}
	
	/*
	 * Query the registry to see if a Default ToS has been set.
	 * Different registry entry for NT vs 95/98/ME.
	 */
	if (ver.dwPlatformId == VER_PLATFORM_WIN32_NT) 
	{
		ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
	                     "SYSTEM\\CurrentControlSet\\Services\\Tcp\\Parameters",
	                     0, KEY_READ, (PHKEY)&hKey);
	} 
	else 
	{
		ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
	           					 "SYSTEM\\CurrentControlSet\\Services\\VxD\\MSTCP\\Parameters", 
	                     0, KEY_READ, (PHKEY)&hKey);
	}
	if (ret == ERROR_SUCCESS) 
	{	
		DWORD dwLen;
		DWORD dwDefaultTOS;
		ULONG ulType;
		dwLen = sizeof(dwDefaultTOS);
	
		ret = RegQueryValueEx(hKey, "DefaultTOS",  NULL, &ulType,
				     (LPBYTE)&dwDefaultTOS, &dwLen);
		RegCloseKey(hKey);
		if (ret == ERROR_SUCCESS) 
		{
		  default_tos = (int)dwDefaultTOS;
	  }
	}
	return default_tos;
}

int __netwin_socketClose(int fd) 
{
	struct linger l;
	int ret;
	int len = sizeof (l);
	if (getsockopt(fd, SOL_SOCKET, SO_LINGER, (OPENCODE_TYPE_SOCKGOPT)&l, &len) == 0) 
	{
		if (l.l_onoff == 0) 
		{
      WSASendDisconnect(fd, 0);
		}
	}
	ret = closesocket (fd);
	return ret;
}

/*
 * Wrapper for bind winsock call - transparent converts an 
 * error related to binding to a port that has exclusive access
 * into an error indicating the port is in use (facilitates
 * better error reporting).
 */
int __netwin_bind(int s, struct sockaddr *him, int len)
{
	int rv = bind(s, him, len);
	
	if (rv == SOCKET_ERROR) 
	{
		/*
		* If bind fails with WSAEACCES it means that a privileged
		* process has done an exclusive bind (NT SP4/2000/XP only).
		*/
		if (WSAGetLastError() == WSAEACCES) 
		{
		  WSASetLastError(WSAEADDRINUSE);
		}
	}
	
	return rv;
}

/*
 * Wrapper for setsockopt dealing with Windows specific issues :-
 *
 * IP_TOS and IP_MULTICAST_LOOP can't be set on some Windows
 * editions. 
 * 
 * The value for the type-of-service (TOS) needs to be masked
 * to get consistent behaviour with other operating systems.
 */
int  __netwin_setSockOpt(int s, int level, int optname, const void *optval,int optlen)
{   
	int rv;
	
	if (level == IPPROTO_IP && optname == IP_TOS) 
	{
		int *tos = (int *)optval;
		*tos &= (IPTOS_TOS_MASK | IPTOS_PREC_MASK);
	}
	
	rv = setsockopt(s, level, optname, (OPENCODE_TYPE_SOCKSOPT)optval, optlen);
	
	if (rv == SOCKET_ERROR) 
	{
		/*
		 * IP_TOS & IP_MULTICAST_LOOP can't be set on some versions
		 * of Windows.
		 */
		if (   (WSAGetLastError() == WSAENOPROTOOPT) 
		     &&(level == IPPROTO_IP) 
		     &&(optname == IP_TOS || optname == IP_MULTICAST_LOOP)) 
		{
		  rv = 0;
		}
	
		/*
		 * IP_TOS can't be set on unbound UDP sockets.
		 */
		if (    (WSAGetLastError() == WSAEINVAL) 
		    &&  (level == IPPROTO_IP) 
		    &&  (optname == IP_TOS)) 
		{
		  rv = 0;
		}
	}
	
	return rv;
}

/*
 * Wrapper for setsockopt dealing with Windows specific issues :-
 *
 * IP_TOS is not supported on some versions of Windows so 
 * instead return the default value for the OS.
 */
int __netwin_getSockOpt(int s, int level, int optname, void *optval,int *optlen)
{
	int rv;
	
	rv = getsockopt(s, level, optname, (OPENCODE_TYPE_SOCKGOPT)optval, optlen);
	
	
	/*
	 * IPPROTO_IP/IP_TOS is not supported on some Windows
	 * editions so return the default type-of-service
	 * value.
	 */
	if (rv == SOCKET_ERROR) 
	{	
	  if(WSAGetLastError() == WSAENOPROTOOPT && level == IPPROTO_IP && optname == IP_TOS) 
	  {	
		  int *tos;
		  tos = (int *)optval;
		  *tos = __netwin_getDefaultTOS();	
		  rv = 0;
		}
	}
	
	return rv;
}

/*
 * Return true if this Windows edition supports ICMP Port Unreachable
 */
bool __netwin_supportPortUnreachable() 
{
	OSVERSIONINFO ver;
	ver.dwOSVersionInfoSize = sizeof(ver);
	GetVersionEx(&ver);
	
	if (ver.dwPlatformId == VER_PLATFORM_WIN32_NT && ver.dwMajorVersion >= 5) 
	{
	  return true;
	} 
	else 
	{
	  return false;
	}
}

/*
 * This function "purges" all outstanding ICMP port unreachable packets
 * outstanding on a socket and returns JNI_TRUE if any ICMP messages
 * have been purged. The rational for purging is to emulate normal BSD 
 * behaviour whereby receiving a "connection reset" status resets the
 * socket.
 */
bool __netwin_purgeOutstandingICMP(int fd)
{
  bool got_icmp = false;
  char buf[1];
  fd_set tbl;
  struct timeval t = { 0, 0 };
  OPENCODE_TYPE_SOCKADDR rmtaddr; 
  OPENCODE_TYPE_SOCKLEN addrlen  = OPENCODE_SOCKADDR_LEN;
  
  /** A no-op if this OS doesn't support it. */
  if (!__netwin_supportPortUnreachable()) 
  {
		return false;
  }
    
  /*
   * Peek at the queue to see if there is an ICMP port unreachable. If there
   * is then receive it.
   */
  FD_ZERO(&tbl);
  FD_SET(fd, &tbl);
  while(true) 
  {
		if (select(/*ignored*/fd+1, &tbl, 0, 0, &t) <= 0) 
		{
		   break;
		}   
		
		if (recvfrom(fd, buf, 1, MSG_PEEK,(struct sockaddr *)&rmtaddr, &addrlen) != OPENCODE_SOCKET_ERROR) 
	  {	    
		  break;
		}
		
		if (WSAGetLastError() != WSAECONNRESET) 
		{
    	/* some other error - we don't care here */
    	break;
		}

		recvfrom(fd, buf, 1, 0,  (struct sockaddr *)&rmtaddr, &addrlen);
		got_icmp = true;
  } 

  return got_icmp;
}

void   SocketUtils::socketAccept(__NET_HOOK_DECLARE_MORE const SocketDescriptor& sd,SocketDescriptor* psd,SocketAddress* paddr,XLONG timeout) __NET_THROW_DECLARE(SocketException)
{
  if(timeout <= 0)
  {
    SocketUtils::socketAccept(__NET_HOOK_INVOKE_MORE sd,psd,paddr);
    return;
  }

  OPENCODE_TYPE_SOCKADDR addr_c;
  OPENCODE_TYPE_SOCKLEN len = OPENCODE_SOCKADDR_LEN;
  OPENCODE_TYPE_SOCKET newfd;
  
  OPENCODE_TYPE_SOCKET  fd = sd.getDescriptor();  
  int   family = sd.getFamily(); 

  int ret = socketTimeout (sd, timeout,TIMEOUT_TYPE_ACCEPT);

	if (ret == 0) 
	{
		__NET_HOOK_RETURN(return,SocketTimeoutException,"Accept timed out");
	} 
	else if (ret == -1) 
	{
		__NET_HOOK_RETURN(return,SocketException,"Asocket closed");
	} 
	else if (ret == -2) 
	{
		__NET_HOOK_RETURN(return,SocketException,"operation interrupted");
	}
      
  newfd = ::accept(fd,(struct sockaddr*)&addr_c,(OPENCODE_TYPE_SOCKLEN*)&len);
  if(newfd < 0)
  {
  	/* REMIND: SOCKET CLOSED PROBLEM */
  	if (newfd == -2)
  	{
  		__NET_HOOK_RETURN(return,SocketException,"operation interrupted");
    }
    else
    {
    	__NET_HOOK_RETURN(return,SocketException,"socket closed");
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

void   SocketUtils::socketAccept(__NET_HOOK_DECLARE_MORE const SocketDescriptor& sd,SocketDescriptor* psd,SocketAddress* paddr) __NET_THROW_DECLARE(SocketException)
{ 
  OPENCODE_TYPE_SOCKADDR addr_c;
  OPENCODE_TYPE_SOCKLEN len = OPENCODE_SOCKADDR_LEN;
  OPENCODE_TYPE_SOCKET newfd;
  
  OPENCODE_TYPE_SOCKET  fd = sd.getDescriptor();  
  int   family = sd.getFamily(); 
  
  newfd = ::accept(fd,(struct sockaddr*)&addr_c,(OPENCODE_TYPE_SOCKLEN*)&len);
  if(newfd < 0)
  {
  	/* REMIND: SOCKET CLOSED PROBLEM */
  	if (newfd == -2)
  	{
  		__NET_HOOK_RETURN(return,SocketException,"operation interrupted");
    }
    else
    {
    	__NET_HOOK_RETURN(return,SocketException,"socket closed");
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
  
  if (connect_rv == SOCKET_ERROR) 
  {
     if (WSAGetLastError() != WSAEWOULDBLOCK)
     {
        connect_rv = WSAGetLastError();
     }
     else
     {
        fd_set wr, ex;
        struct timeval t;
        int retry;

        FD_ZERO(&wr);
        FD_ZERO(&ex);
        FD_SET(fd, &wr);
        FD_SET(fd, &ex);
        t.tv_sec = timeout / 1000;
        t.tv_usec = (timeout % 1000) * 1000;

        /* 
	       * Wait for timout, connection established or
	       * connection failed.
	       */
        connect_rv = select(fd+1, 0, &wr, &ex, &t);

        	/* 
	         * Timeout before connection is established/failed so
	         * we throw exception and shutdown input/output to prevent
	         * socket from being used.
	         * The socket should be closed immediately by the caller.
	         */
         if (connect_rv == 0)
         {
           socketShutdown( sd, SD_BOTH );
           SET_BLOCKING(fd);
           __NET_HOOK_RETURN(return,SocketTimeoutException,"Connect timed out");
         } 

	        /*
	         * We must now determine if the connection has been established
	         * or if it has failed. The logic here is designed to work around
	         * bug on Windows NT whereby using getsockopt to obtain the 
	         * last error (SO_ERROR) indicates there is no error. The workaround
	         * on NT is to allow winsock to be scheduled and this is done by
	         * yielding and retrying. As yielding is problematic in heavy
	         * load conditions we attempt up to 3 times to get the error reason.
	         */
	        if (!FD_ISSET(fd, &ex)) 
	        {
	            connect_rv = 0;
	        } 
	        else 
	        {
	            for (int retry=0; retry<3; retry++) 
	            {
                  int optlen = sizeof(connect_rv);
					        __netwin_getSockOpt(fd, SOL_SOCKET, SO_ERROR, (char*)&connect_rv, &optlen);
					        if (connect_rv != 0) 
				          {
					            break;
					        }
					        Sleep(0);

	            }//~end for (int retry=0; retry<3; retry++) 

	            if (connect_rv == 0) 
	            {
				        __NET_HOOK_RETURN(return,SocketException,"Unable to establish connection");
	            }

	        }//~end if (!FD_ISSET(fd, &ex)) 

     }//~end if (WSAGetLastError() != WSAEWOULDBLOCK)
  }//~end if (connect_rv == SOCKET_ERROR) 

  if (connect_rv != 0) 
  {
    if (connect_rv == WSAEADDRNOTAVAIL)
    {
       __NET_HOOK_RETURN(return,SocketException,"connect: Address is invalid on local machine, or port is not valid on remote machine");
    } 
    else 
    {
       __NET_HOOK_RETURN(return,SocketException,"connect",connect_rv);
    }
  } //~end if (connect_rv != 0) 

}

void   SocketUtils::socketConnect(__NET_HOOK_DECLARE_MORE const SocketDescriptor& sd,const InetAddress& address, int port) __NET_THROW_DECLARE(SocketException)
{
  OPENCODE_TYPE_SOCKET  fd = sd.getDescriptor();  
  int   family = sd.getFamily(); 
  
  OPENCODE_TYPE_SOCKADDR him;
  NetworkUtils::make_sockaddr((struct sockaddr *)&him,address,port);

  int connect_rv = connect(fd, (struct sockaddr *)&him, OPENCODE_SOCKADDR_LEN);
	if (connect_rv == SOCKET_ERROR) 
  {
	    connect_rv = WSAGetLastError();
	}

  if (connect_rv != 0) 
  {
    if (connect_rv == WSAEADDRNOTAVAIL)
    {
      __NET_HOOK_RETURN(return,SocketException,"connect: Address is invalid on local machine, or port is not valid on remote machine",connect_rv); 
    }
    else 
    {
      __NET_HOOK_RETURN(return,SocketException,System::getLastError(connect_rv),connect_rv);   
    
    } //~end if (connect_rv == WSAEADDRNOTAVAIL)
  
  } //~end if (connect_rv != 0)

}

int   SocketUtils::socketRead(__NET_HOOK_DECLARE_MORE const SocketDescriptor& sd,char* b, int length,unsigned  int flags,XLONG timeout) __NET_THROW_DECLARE(SocketException,ConnectionResetException)
{
  OPENCODE_TYPE_SOCKET  fd = sd.getDescriptor();  

  int read_rv = 0;  

  if (timeout > 0) 
  {
    if (timeout <= 5000 || !NetworkUtils::isRcvTimeoutSupported()) 
    {
      int ret = socketTimeout (sd, timeout,TIMEOUT_TYPE_READ);

      if (ret <= 0) 
      {
        if (ret == 0) 
        {
            __NET_HOOK_RETURN(return -1,SocketException,"Read timed out");
        } 
        else if (ret == OPENCODE_SOCKET_ERROR) 
        {
            __NET_HOOK_RETURN(return -1,SocketException,"Socket closed");
        }                
        else
        {
          XLONG __err_num = OPENCODE_SOCKET_GETLASTERROR();
          __NET_HOOK_RETURN(return -1,SocketException,System::getLastError(__err_num));
        } 
      
      }//~end  if (ret <= 0) 

    }//~end if (timeout <= 5000 || !NetworkUtils::isRcvTimeoutSupported()) 

  }//~end if (timeout > 0) 

  read_rv = ::recv(fd,b,length,flags); 

  if (read_rv < 0) 
  {
    /** Recv failed. */
    switch (WSAGetLastError()) 
    {
      case WSAEINTR:
      {
        __NET_HOOK_RETURN(return -1,SocketException,"socket closed");
      }
      break;

      case WSAECONNRESET:
      case WSAESHUTDOWN:
      {
        /*
         * Connection has been reset - Windows sometimes reports
         * the reset as a shutdown error.
         */
        __NET_HOOK_RETURN(return -1,ConnectionResetException,"socket reset");
      }
      break; 

      case WSAETIMEDOUT :
      {
         /*
         * Connection has been reset - Windows sometimes reports
         * the reset as a shutdown error.
         */
        __NET_HOOK_RETURN(return -1,SocketTimeoutException,"Read timed out");
      }
      break; 
      default:
      {
        __NET_HOOK_RETURN(return -1,SocketException,"recv failed");
      }
      break;
    }//~end switch (WSAGetLastError()) 

  }//~end if (read_rv < 0) 

  return read_rv; 
}

int   SocketUtils::socketRecvFrom(__NET_HOOK_DECLARE_MORE const SocketDescriptor& sd,SocketAddress* paddr,char* buf, int len, unsigned int flags,XLONG timeout) __NET_THROW_DECLARE(SocketException,ConnectionResetException)
{
  OPENCODE_TYPE_SOCKET  fd = sd.getDescriptor();  
  
  OPENCODE_TYPE_SOCKADDR addr; 
  OPENCODE_TYPE_SOCKLEN fromlen = OPENCODE_SOCKADDR_LEN;
  
  XLONG prevTime = 0;
  int recv_rv = 0;

  if (__netwin_supportPortUnreachable() && (timeout > 0)) 
  {
    prevTime = System::currentTimeMillis();
  }

	if (timeout > 0) 
	{
		if (timeout <= 5000 || !NetworkUtils::isRcvTimeoutSupported()) 
		{
			int ret = socketTimeout (sd, timeout,TIMEOUT_TYPE_READ);
		
			if (ret <= 0) 
			{
				if (ret == 0) 
				{
				  __NET_HOOK_RETURN(return -1,SocketException,"Read timed out");
				} 
				else
				{
          XLONG __err_num = OPENCODE_SOCKET_GETLASTERROR();
          __NET_HOOK_RETURN(return -1,SocketException,System::getLastError(__err_num));
				} 
			
			}//~end  if (ret <= 0) 
		
		}//~end if (timeout <= 5000 || !NetworkUtils::isRcvTimeoutSupported()) 
	
	}//~end if (timeout > 0) 
  
///////////////////////////////
  bool retry = false;
  
	/** Loop only if we discarding ICMP port unreachable packets */
	do 
	{
		retry = FALSE;
	  
	  /* receive the packet */
	  if(paddr == 0)
	  { 
	    recv_rv = ::recvfrom(fd, buf, len, flags, (struct sockaddr *)&addr, (OPENCODE_TYPE_SOCKLEN*)&fromlen);
	  }
	  else
	  {
	    recv_rv = ::recvfrom(fd, buf, len, flags, (struct sockaddr *)&addr, (OPENCODE_TYPE_SOCKLEN*)&fromlen);
	  }
  
		if (recv_rv == OPENCODE_SOCKET_ERROR) 
		{
		  if (WSAGetLastError() == WSAECONNRESET) 
		  {
				/*
				 * An icmp port unreachable has been received - consume any other
				 * outstanding packets.
				 */
				__netwin_purgeOutstandingICMP(fd);		
		
				/*
				 * If a timeout was specified then we need to adjust it because
				 * we may have used up some of the timeout before the icmp port
				 * unreachable arrived.
				 */
				if (timeout > 0) 
				{
			    int ret;
			    XLONG newTime = System::currentTimeMillis();
			    timeout -= (newTime - prevTime);
			    prevTime = newTime;
			
			    if (timeout <= 0) 
			    {
						ret = 0;
			    } 
			    else 
			    {
						ret = socketTimeout(fd, timeout,TIMEOUT_TYPE_READ);
			    }
			
			    if (ret <= 0) 
			    {
						if (ret == 0) 
						{
						  __NET_HOOK_RETURN(return -1,SocketTimeoutException,"Receive timed out");
						    
						} 
						else if (ret == OPENCODE_SOCKET_ERROR) 
						{
							__NET_HOOK_RETURN(return -1,SocketException,"Socket closed");
						} 
						else
						{
					    XLONG __tmp_errnum = OPENCODE_SOCKET_GETLASTERROR();
					    __NET_HOOK_RETURN(return -1,SocketException,System::getLastError(__tmp_errnum),__tmp_errnum); 
						}
			    }
				}
		
				/*
				 * An ICMP port unreachable was received but we are
				 * not connected so ignore it.
				 */
				retry = true;
				
		  }//~end if (WSAGetLastError() == WSAECONNRESET) 
		  
		}//~end if (recv_rv == OPENCODE_SOCKET_ERROR) 
	
	} while (retry);

	if (recv_rv < 0) 
	{
		/* check to see if it's because the buffer was too small */
		if (WSAGetLastError() == WSAEMSGSIZE) 
		{
	    /* it is because the buffer is too small. It's UDP, it's
	     * unreliable, it's all good. discard the rest of the
	     * data..
	     */
	    recv_rv = len;
		} 
	}
	
	if (recv_rv == OPENCODE_SOCKET_ERROR) 
	{
		__NET_HOOK_RETURN(return -1,SocketException,"socket closed",OPENCODE_SOCKET_GETLASTERROR());
	} 
	else if (recv_rv < 0) 
	{
		__NET_HOOK_RETURN(return -1,SocketException,"Datagram receive failed",OPENCODE_SOCKET_GETLASTERROR()); 
	}

  return recv_rv;  
}

void   SocketUtils::socketWrite(__NET_HOOK_DECLARE_MORE const SocketDescriptor& sd,const char* b, int length,unsigned  int flags) __NET_THROW_DECLARE(SocketException)
{
  OPENCODE_TYPE_SOCKET  fd = sd.getDescriptor();  
  int __write_bytes = 0;
  
  while(__write_bytes < length)
  {
    int retry = 0;
    
    int send_n = ::send(fd,b + __write_bytes,length - __write_bytes,flags);
 	  
    if(send_n == 0)
    {
      break;
    } 
    else if (send_n > 0) 
 	  {
			__write_bytes += send_n;
      continue;
	  }
    
    __NET_HOOK_RETURN(return,SocketException,"socket write error",OPENCODE_SOCKET_GETLASTERROR());
  }
}

void   SocketUtils::socketSendTo(__NET_HOOK_DECLARE_MORE const SocketDescriptor& sd,const SocketAddress* paddr,const char* msg, int len,  unsigned  int flags) __NET_THROW_DECLARE(SocketException)
{
  OPENCODE_TYPE_SOCKET  fd = sd.getDescriptor();  
  
  OPENCODE_TYPE_SOCKADDR addr; 
  NetworkUtils::make_sockaddr((struct sockaddr *)&addr,*paddr); 
  int tolen = OPENCODE_SOCKADDR_LEN;
  
  int __write_bytes = 0;
  int send_rv = 0;
  while(__write_bytes < len)
  {
    send_rv = ::sendto(fd, msg + __write_bytes, len - __write_bytes, flags, (struct sockaddr *)&addr, tolen);

    if(send_rv == 0)
    {
      break;
    } 
    else if(send_rv > 0)
    {
      __write_bytes += send_rv;
      continue;
    }       

		__NET_HOOK_RETURN(return,SocketException,"Datagram send failed",OPENCODE_SOCKET_GETLASTERROR());  
  } 
}

void   SocketUtils::socketSendUrgentData(__NET_HOOK_DECLARE_MORE const SocketDescriptor& sd,int data) __NET_THROW_DECLARE(SocketException)
{
  OPENCODE_TYPE_SOCKET  fd = sd.getDescriptor();  
  
  int send_rv = ::send(fd,(char *)&data, 1, MSG_OOB);
  
  if(send_rv < 0)
  {
    __NET_HOOK_RETURN(return,SocketException,"send failed",OPENCODE_SOCKET_GETLASTERROR());  
  }  
}

int   SocketUtils::socketTimeout(const SocketDescriptor& sd, XLONG timeout,int type)
{
	  OPENCODE_TYPE_SOCKET  fd = sd.getDescriptor(); 
    switch(type)
	  {
		  case TIMEOUT_TYPE_ACCEPT:
		  {
          fd_set tbl; 
          struct timeval t; 
          t.tv_sec = timeout / 1000; 
          t.tv_usec = (timeout % 1000) * 1000; 
          FD_ZERO(&tbl); 
          FD_SET(fd, &tbl); 

          return select (fd + 1, &tbl, 0, 0, &t);
      }
	    break;
  //---------------------------------------
		  case TIMEOUT_TYPE_CONNECT:
		  {
          fd_set wr, ex;
          struct timeval t;
          int retry;

          FD_ZERO(&wr);
          FD_ZERO(&ex);
          FD_SET(fd, &wr);
          FD_SET(fd, &ex);
          t.tv_sec = timeout / 1000;
          t.tv_usec = (timeout % 1000) * 1000;

          /** Wait for timout, connection established or  connection failed. */
          return select(fd+1, 0, &wr, &ex, &t);
      }
	    break;
  //---------------------------------------
		  case TIMEOUT_TYPE_READ:
		  {
        if(!NetworkUtils::isRcvTimeoutSupported())
        {
            fd_set tbl; 
            struct timeval t; 
            t.tv_sec = timeout / 1000; 
            t.tv_usec = (timeout % 1000) * 1000; 
            FD_ZERO(&tbl); 
            FD_SET(fd, &tbl); 

            return select (fd + 1, &tbl, 0, 0, &t);
        }
      }
	    break;
  //---------------------------------------
      default:
      {
      }
      break;		  	
    }
    	
	  return 1;
}

#endif

__OPENCODE_END_NAMESPACE
