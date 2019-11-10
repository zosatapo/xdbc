#ifndef __OPENCODE_PORTABLE_SOCKET_H__
#define __OPENCODE_PORTABLE_SOCKET_H__

#include "port/ioc_G_config.h"

#if defined(PORT_WINDOWS)
#  include <winsock2.h>
#  include <ws2tcpip.h>
#endif

#if !defined(PORT_WINDOWS)
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <sys/ioctl.h>

#include <netdb.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <net/if.h>
#include <net/route.h>

#endif

#if defined(PORT_SUN_SOLARIS)
#  include <sys/sockio.h>
#  include <socket/if_arp.h> 
#elif defined(PORT_LINUX)
#  include <bits/ioctls.h>
#  include <linux/sockios.h>
#endif

#ifdef OPENCODE_HAS_GETIFADDRS
# include <ifaddrs.h>
#endif

#if defined(PORT_WINDOWS)           
#  define OPENCODE_TYPE_INET4ADDRESS       unsigned long
#  define OPENCODE_TYPE_SOCKLEN            int
#elif defined(PORT_HPUX)            
#  define OPENCODE_TYPE_INET4ADDRESS       unsigned long
#  define OPENCODE_TYPE_SOCKLEN            int
#else                                      
#  define OPENCODE_TYPE_INET4ADDRESS       in_addr_t
#  define OPENCODE_TYPE_SOCKLEN            socklen_t 
#endif                                     
                                           
#if defined(PORT_WINDOWS)           
#  define OPENCODE_TYPE_SOCKET             int
#  define OPENCODE_TYPE_SOCKSOPT           const char*
#  define OPENCODE_TYPE_SOCKGOPT           char*
#  define OPENCODE_SOCKET_ERROR            SOCKET_ERROR
#  define OPENCODE_INVALID_SOCKET          ((OPENCODE_TYPE_SOCKET)-1)
#  define OPENCODE_SOCKET_GETLASTERROR     WSAGetLastError
#else                                      
#  define OPENCODE_TYPE_SOCKET             int
#  define OPENCODE_TYPE_SOCKSOPT           const void*
#  define OPENCODE_TYPE_SOCKGOPT           void*
#  define OPENCODE_SOCKET_ERROR            -1
#  define OPENCODE_INVALID_SOCKET          ((OPENCODE_TYPE_SOCKET)-1)
#  define OPENCODE_SOCKET_GETLASTERROR     System::getLastErrorNumber
#endif

#ifdef AF_INET6
#  define OPENCODE_TYPE_SOCKADDR       union { \
                                         struct sockaddr_in him4; \
                                         struct sockaddr_in6 him6; \
		                 }
#  define OPENCODE_SOCKADDR_LEN        (NetworkUtils::IPv6_available() ? (int)sizeof(struct sockaddr_in6) : \
                                                                       (int)sizeof(struct sockaddr_in))
#else
#  define OPENCODE_TYPE_SOCKADDR       union { struct sockaddr_in him4; }
#  define OPENCODE_SOCKADDR_LEN        (int)sizeof(struct sockaddr_in)
#endif

#if defined(PORT_WINDOWS)
#define SET_NONBLOCKING(fd) {   \
        u_long optval = 1;\
        int optlen = sizeof(optval);\
        ioctlsocket( fd, FIONBIO, &optval );\
  }

#define SET_BLOCKING(fd) {  \
        u_long optval = 0;\
        int optlen = sizeof(optval);\
        ioctlsocket( fd, FIONBIO, &optval );\
  }
#else
#define SET_NONBLOCKING(fd) {    \
        int flags = fcntl(fd, F_GETFL);  \
        flags |= O_NONBLOCK;     \
        fcntl(fd, F_SETFL, flags);  \
}

#define SET_BLOCKING(fd) {    \
        int flags = fcntl(fd, F_GETFL); \
  flags &= ~O_NONBLOCK;    \
        fcntl(fd, F_SETFL, flags);      \
}
#endif

#endif



