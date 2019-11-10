#include "socket/plainDatagramSocketImpl.h"

#include "socket/socketDescriptor.h"
#include "socket/inetAddress.h"
#include "socket/socketException.h"
#include "socket/socketDescriptor.h"

#include "socket/socketUtils.h"
#include "socket/networkUtils.h"
#include "port/ioc_sbuf.h"

__OPENCODE_BEGIN_NAMESPACE

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------
PlainDatagramSocketImpl::~PlainDatagramSocketImpl(){}

/**
 * Constructs an instance with the given file descriptor.
 */
PlainDatagramSocketImpl::PlainDatagramSocketImpl(const SocketDescriptor& fdValue) 
{
  this->fd = fdValue;
  
  /* timeout value for receive() */
  timeout = 0;
  trafficClass = 0;
  connected = false;
  connectedPort = -1;
  
  /* cached socket options */
  multicastInterface = 0;
  loopbackMode = true;
  ttl = -1; 
}

PlainDatagramSocketImpl::PlainDatagramSocketImpl()
{
  /* timeout value for receive() */
  timeout = 0;
  trafficClass = 0;
  connected = false;
  connectedPort = -1;

  /* cached socket options */
  multicastInterface = 0;
  loopbackMode = true;
  ttl = -1;
}

PlainDatagramSocketImpl::PlainDatagramSocketImpl(const PlainDatagramSocketImpl& rv)
:DatagramSocketImpl(rv)
{
  /* timeout value for receive() */
  timeout = rv.timeout;
  trafficClass = rv.trafficClass;
  connected = rv.connected;
  connectedPort = rv.connectedPort;

  /* cached socket options */
  multicastInterface = rv.multicastInterface;
  loopbackMode = rv.loopbackMode;
  ttl = rv.ttl;
}

PlainDatagramSocketImpl& PlainDatagramSocketImpl::operator=(const PlainDatagramSocketImpl& rv)
{
  if(&rv == this)
  {
   return *this;
  }

  DatagramSocketImpl::operator=(rv);
 
  /* timeout value for receive() */
  timeout = rv.timeout;
  trafficClass = rv.trafficClass;
  connected = rv.connected;
  connectedPort = rv.connectedPort;

  /* cached socket options */
  multicastInterface = rv.multicastInterface;
  loopbackMode = rv.loopbackMode;
  ttl = rv.ttl;

  return *this;
}

PlainDatagramSocketImpl* PlainDatagramSocketImpl::doClone() const
{
  return new PlainDatagramSocketImpl(*this);
}
   
//------------------------------------------------------------------------
//
//------------------------------------------------------------------------ 
void PlainDatagramSocketImpl::create(__NET_HOOK_DECLARE_ONCE)  __NET_THROW_DECLARE(SocketException) 
{
  SocketUtils::socketCreate(__NET_HOOK_INVOKE_MORE false,&fd);
}

void PlainDatagramSocketImpl::bind(__NET_HOOK_DECLARE_MORE const InetAddress& laddr,int lport) __NET_THROW_DECLARE(SocketException)
{
  SocketUtils::socketBind(__NET_HOOK_INVOKE_MORE fd,laddr,lport);
}

void PlainDatagramSocketImpl::connect(__NET_HOOK_DECLARE_MORE const InetAddress& address, int port)  __NET_THROW_DECLARE(SocketException) 
{
  SocketUtils::socketConnect(__NET_HOOK_INVOKE_MORE fd,address,port);
  __NET_HOOK_CHECK(return);

  connectedAddress = address;
  connectedPort = port;
  connected = true;
}

void PlainDatagramSocketImpl::close() 
{
  if (fd.isValid()) 
  {
    SocketUtils::socketClose(fd);
    fd.invalidate();
  }
}

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------ 
int PlainDatagramSocketImpl::receive(__NET_HOOK_DECLARE_MORE SocketAddress* address,char* b, int length)  __NET_THROW_DECLARE(SocketException)
{
  int nread = 0;
  bool gotReset = false;

  __OPENCODE_TRY
  {
    if(connected)
    {
      nread =  SocketUtils::socketRead(__NET_HOOK_INVOKE_MORE fd,b,length, 0);
    }
    else
    {
      return SocketUtils::socketRecvFrom(__NET_HOOK_INVOKE_MORE fd,address,b,length, 0);
    }

    if(nread > 0)
    {
      return nread;
    }
  }
  __OPENCODE_CATCH(ConnectionResetException& rstExc)
  {
    gotReset = true;
  }

  if(gotReset)
  {

    if(connected)
    {
      nread =  SocketUtils::socketRead(__NET_HOOK_INVOKE_MORE fd,b,length, 0);
    }
    else
    {
      return SocketUtils::socketRecvFrom(__NET_HOOK_INVOKE_MORE fd,address,b,length, 0);
    }

    if(nread > 0)
    {
      return nread;
    }
  }

  return -1;  
}

int PlainDatagramSocketImpl::peek(__NET_HOOK_DECLARE_MORE SocketAddress* address,char* b, int length)  __NET_THROW_DECLARE(SocketException)
{
  int nread = 0;
  bool gotReset = false;

  __OPENCODE_TRY
  {
    if(connected)
    {
      nread = SocketUtils::socketRead(__NET_HOOK_INVOKE_MORE fd,b,length, SocketUtils::C_MSG_PEEK);
    }
    else
    {
      nread = SocketUtils::socketRecvFrom(__NET_HOOK_INVOKE_MORE fd,address,b,length,SocketUtils::C_MSG_PEEK);
    }

    if(nread > 0)
    {
      return nread;
    }
  }
  __OPENCODE_CATCH(ConnectionResetException& rstExc)
  {
    gotReset = true;
  }

  if(gotReset)
  {

    if(connected)
    {
      nread = SocketUtils::socketRead(__NET_HOOK_INVOKE_MORE fd,b,length, SocketUtils::C_MSG_PEEK);
    }
    else
    {
      nread = SocketUtils::socketRecvFrom(__NET_HOOK_INVOKE_MORE fd,address,b,length,SocketUtils::C_MSG_PEEK);
    }

    if(nread > 0)
    {
      return nread;
    }
  }

  return -1;
}

void PlainDatagramSocketImpl::send(__NET_HOOK_DECLARE_MORE const SocketAddress* address,const char* b, int length)  __NET_THROW_DECLARE(SocketException)
{
  if(connected)
  {
    SocketUtils::socketWrite(__NET_HOOK_INVOKE_MORE fd,b,length, 0);
  }
  else
  {
    SocketUtils::socketSendTo(__NET_HOOK_INVOKE_MORE fd,address,b,length, 0); 
  }  
}

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------ 
void PlainDatagramSocketImpl::setTimeToLive(__NET_HOOK_DECLARE_MORE int ttlValue)  __NET_THROW_DECLARE(SocketException)
{
  SocketUtils::socketSetTimeToLive(__NET_HOOK_INVOKE_MORE fd,ttlValue); 
}

int PlainDatagramSocketImpl::getTimeToLive(__NET_HOOK_DECLARE_ONCE)  __NET_THROW_DECLARE(SocketException)
{
  return SocketUtils::socketGetTimeToLive(__NET_HOOK_INVOKE_MORE fd); 
}


//------------------------------------------------------------------------
//
//------------------------------------------------------------------------ 
void PlainDatagramSocketImpl::join(__NET_HOOK_DECLARE_MORE const InetAddress& inetaddr)  __NET_THROW_DECLARE(SocketException) 
{
  NetworkUtils::mcastJoinGroup(__NET_HOOK_INVOKE_MORE fd,inetaddr,0); 
}

void PlainDatagramSocketImpl::leave(__NET_HOOK_DECLARE_MORE const InetAddress& inetaddr)  __NET_THROW_DECLARE(SocketException) 
{
  NetworkUtils::mcastLeaveGroup(__NET_HOOK_INVOKE_MORE fd,inetaddr,0);
}

void PlainDatagramSocketImpl::joinGroup(__NET_HOOK_DECLARE_MORE const SocketAddress& mcastaddr,const NetworkInterface& netIf) __NET_THROW_DECLARE(SocketException) 
{
  NetworkUtils::mcastJoinGroup(__NET_HOOK_INVOKE_MORE fd,mcastaddr.getInetAddress(),&netIf);
}

void PlainDatagramSocketImpl::leaveGroup(__NET_HOOK_DECLARE_MORE const SocketAddress& mcastaddr,const NetworkInterface& netIf) __NET_THROW_DECLARE(SocketException) 
{
   NetworkUtils::mcastLeaveGroup(__NET_HOOK_INVOKE_MORE fd,mcastaddr.getInetAddress(),&netIf);
}

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------ 
void PlainDatagramSocketImpl::setOption(__NET_HOOK_DECLARE_MORE int optID, const SocketOptionsValue& val)  __NET_THROW_DECLARE(SocketException) 
{
  bool on = true;
  switch (optID) 
  {
    /** check type safety b4 going native. */
    case SOCKETOPTIONS_SO_W_LINGER:
    {
      if(    (SocketOptionsValue::SOV_INTEGER != val.sov_type) 
          && (SocketOptionsValue::SOV_BOOLEAN != val.sov_type))
      {
        __NET_HOOK_RETURN(return,SocketException,"Bad parameter for option SO_W_LINGER");
      }
      
      if (SocketOptionsValue::SOV_BOOLEAN == val.sov_type) 
      {
        /* true only if disabling - enabling should be Integer */
        on = false;
      }
    }
    break;
//--------------------------------------------------------------
//--------------------------------------------------------------    
    case SOCKETOPTIONS_SO_W_TIMEOUT:
    {
      if (SocketOptionsValue::SOV_INTEGER != val.sov_type)
      {
        __NET_HOOK_RETURN(return,SocketException,"Bad parameter for option SO_W_TIMEOUT");
      }
      
      int tmp = val.sov_int_value;
      if (tmp < 0)
      {
        __OPENCODE_THROW(SocketException("timeout < 0"));
      }
      timeout = tmp;
    }
    break;
//--------------------------------------------------------------
//--------------------------------------------------------------      
    case SOCKETOPTIONS_SO_W_IP_TOS:
    {
     if (SocketOptionsValue::SOV_INTEGER != val.sov_type) 
     {
       __NET_HOOK_RETURN(return,SocketException,"Bad parameter for option IP_TOS");
     }
     trafficClass = val.sov_int_value;
    }
     break;
     
//--------------------------------------------------------------
//--------------------------------------------------------------  
    case SOCKETOPTIONS_SO_W_TCP_NODELAY:
    {
      if (SocketOptionsValue::SOV_BOOLEAN != val.sov_type)
      {
        __NET_HOOK_RETURN(return,SocketException,"Bad parameter for option TCP_NODELAY");
      }
      on = val.sov_bol_value;
    }
    break;

//--------------------------------------------------------------
//--------------------------------------------------------------  
    case SOCKETOPTIONS_SO_W_SNDBUF:
    case SOCKETOPTIONS_SO_W_RCVBUF:
    {
      if ((SocketOptionsValue::SOV_INTEGER != val.sov_type) || (val.sov_int_value <= 0)) 
      {
        __NET_HOOK_RETURN(return,SocketException,"Bad parameter for option SO_W_SNDBUF or SO_W_RCVBUF");
      }
    }
    break;
//--------------------------------------------------------------
//--------------------------------------------------------------  
    case SOCKETOPTIONS_SO_W_KEEPALIVE:
    {
      if (SocketOptionsValue::SOV_BOOLEAN != val.sov_type)
      {
        __NET_HOOK_RETURN(return,SocketException,"Bad parameter for option SO_W_KEEPALIVE");
      }
      on = val.sov_bol_value;
    }
    break;
//--------------------------------------------------------------
//--------------------------------------------------------------  
    case SOCKETOPTIONS_SO_W_OOBINLINE:
    {
      if (SocketOptionsValue::SOV_BOOLEAN != val.sov_type)
      {
        __NET_HOOK_RETURN(return,SocketException,"Bad parameter for option SO_W_OOBINLINE");
      }
      on = val.sov_bol_value;
    }
    break;

//--------------------------------------------------------------
//--------------------------------------------------------------  
    case SOCKETOPTIONS_SO_W_REUSEADDR:
    {
      if (SocketOptionsValue::SOV_BOOLEAN != val.sov_type)
      {
        __NET_HOOK_RETURN(return,SocketException,"Bad parameter for option SO_W_REUSEADDR"); 
      }
      on = val.sov_bol_value;
    }
    break;
//--------------------------------------------------------------
//--------------------------------------------------------------        
    default:
    {
      StringBuffer sb;
      sb.append("unrecognized TCP option: ");
      sb.append(optID);
      __NET_HOOK_RETURN(return,SocketException,sb.str());
    }
  }
  
  SocketUtils::socketSetOption(__NET_HOOK_INVOKE_MORE fd,optID, on, val);
}

SocketOptionsValue PlainDatagramSocketImpl::getOption(__NET_HOOK_DECLARE_MORE int optID)  __NET_THROW_DECLARE(SocketException) 
{
  SocketOptionsValue sov;
  
  if (optID == SO_W_TIMEOUT) 
  {
    sov.sov_type = SocketOptionsValue::SOV_INTEGER;
    sov.sov_int_value = timeout;
    
    return sov;
  }
  
  /*
  * The native socketGetOption() knows about 3 options.
  * The 32 bit value it returns will be interpreted according
  * to what we're asking.  A return of -1 means it understands
  * the option but its turned off.  It will raise a SocketException
  * if "opt" isn't one it understands.
  */
  
  switch (optID) 
  {
    case SOCKETOPTIONS_SO_W_IP_TOS:
    {
      sov = SocketUtils::socketGetOption(__NET_HOOK_INVOKE_MORE fd,optID);
      __NET_HOOK_CHECK(return sov);
        
      if (sov.sov_int_value == -1) 
      { 
        // ipv6 tos
        sov.sov_type = SocketOptionsValue::SOV_INTEGER;
        sov.sov_int_value = trafficClass;
        
        return sov;
      } 
      else 
      {
        return sov;
      }
    }
    break;

//--------------------------------------------------------------
//--------------------------------------------------------------       
    default:
    {
      return SocketUtils::socketGetOption(__NET_HOOK_INVOKE_MORE fd,optID);
    }
  }  
}

__OPENCODE_END_NAMESPACE
