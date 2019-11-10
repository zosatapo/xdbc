#include "socket/socketOptions.h"

__OPENCODE_BEGIN_NAMESPACE

const int  SocketOptionsValue::SOV_UNKNOWN           = 0;

const int  SocketOptionsValue::SOV_BOOLEAN           = 1;  
const int  SocketOptionsValue::SOV_INTEGER           = 2; 
const int  SocketOptionsValue::SOV_BOOLINT           = 3;
  
const int  SocketOptionsValue::SOV_INETADDRESS       = 4; 
const int  SocketOptionsValue::SOV_NETWORKINTERFACE  = 5;

const int SocketOptions::SO_W_TCP_NODELAY = SOCKETOPTIONS_SO_W_TCP_NODELAY;
const int SocketOptions::SO_W_BINDADDR = SOCKETOPTIONS_SO_W_BINDADDR;
const int SocketOptions::SO_W_REUSEADDR = SOCKETOPTIONS_SO_W_REUSEADDR;
const int SocketOptions::SO_W_BROADCAST = 0x0020;
const int SocketOptions::SO_W_IP_MULTICAST_IF = SOCKETOPTIONS_SO_W_IP_MULTICAST_IF;
const int SocketOptions::SO_W_IP_MULTICAST_IF2 = SOCKETOPTIONS_SO_W_IP_MULTICAST_IF2;
const int SocketOptions::SO_W_IP_MULTICAST_LOOP = SOCKETOPTIONS_SO_W_IP_MULTICAST_LOOP;
const int SocketOptions::SO_W_IP_TOS = SOCKETOPTIONS_SO_W_IP_TOS;
const int SocketOptions::SO_W_LINGER = SOCKETOPTIONS_SO_W_LINGER;
const int SocketOptions::SO_W_TIMEOUT = SOCKETOPTIONS_SO_W_TIMEOUT;
const int SocketOptions::SO_W_SNDBUF = SOCKETOPTIONS_SO_W_SNDBUF;
const int SocketOptions::SO_W_RCVBUF = SOCKETOPTIONS_SO_W_RCVBUF;
const int SocketOptions::SO_W_KEEPALIVE = SOCKETOPTIONS_SO_W_KEEPALIVE;
const int SocketOptions::SO_W_OOBINLINE = SOCKETOPTIONS_SO_W_OOBINLINE;

__OPENCODE_END_NAMESPACE
