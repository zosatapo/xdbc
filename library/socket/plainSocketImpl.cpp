#include "socket/plainSocketImpl.h"

#include "socket/socketDescriptor.h"
#include "socket/inetAddress.h"
#include "socket/socketException.h"
#include "socket/socketUtils.h"
#include "socket/socket.h"
#include "socket/serverSocket.h"

#include "port/ioc_sbuf.h"


__OPENCODE_BEGIN_NAMESPACE

const int PlainSocketImpl::CONNECTION_NOT_RESET = 0;
const int PlainSocketImpl::CONNECTION_RESET_PENDING = 1;
const int PlainSocketImpl::CONNECTION_RESET = 2;

const int PlainSocketImpl::SHUT_W_RD = 0;
const int PlainSocketImpl::SHUT_W_WR = 1;

//------------------------------------------------------------
// ctor and dector
//------------------------------------------------------------

PlainSocketImpl::~PlainSocketImpl(){}

/**
 * Constructs an instance with the given file descriptor.
 */
PlainSocketImpl::PlainSocketImpl(const SocketDescriptor& fdValue)
{
  this->fd = fdValue;

  timeout = 0;   // timeout in millisec

  trafficClass = 0; // traffic class

  shut_rd = false;
  shut_wr = false;

  /* number of threads using the SocketDescriptor */
  fdUseCount = 0;

  /* indicates a close is pending on the file descriptor */
  closePending = false;
  resetState = CONNECTION_NOT_RESET;
}

PlainSocketImpl::PlainSocketImpl()
{
  timeout = 0;   // timeout in millisec
     
  trafficClass = 0; // traffic class

  shut_rd = false;
  shut_wr = false;

  /* number of threads using the SocketDescriptor */
  fdUseCount = 0;

  /* indicates a close is pending on the file descriptor */
  closePending = false;    
  resetState = CONNECTION_NOT_RESET;
}  

PlainSocketImpl::PlainSocketImpl(const PlainSocketImpl& rv)
:SocketImpl(rv)
{
  timeout = rv.timeout;   // timeout in millisec

  trafficClass = rv.trafficClass; // traffic class

  shut_rd = rv.shut_rd;
  shut_wr = rv.shut_wr;

  /* number of threads using the SocketDescriptor */
  fdUseCount = rv.fdUseCount;

  /* indicates a close is pending on the file descriptor */
  closePending = rv.closePending;
  resetState = rv.resetState;  
}

PlainSocketImpl& PlainSocketImpl::operator=(const PlainSocketImpl& rv)
{
  if(&rv == this)
  {
    return *this;
  }

  SocketImpl::operator=(rv);

  timeout = rv.timeout;   // timeout in millisec

  trafficClass = rv.trafficClass; // traffic class

  shut_rd = rv.shut_rd;
  shut_wr = rv.shut_wr;

  /* number of threads using the SocketDescriptor */
  fdUseCount = rv.fdUseCount;

  /* indicates a close is pending on the file descriptor */
  closePending = rv.closePending;
  resetState = rv.resetState;

  return *this;
}

PlainSocketImpl* PlainSocketImpl::doClone() const
{
  return new PlainSocketImpl(*this);
}

//------------------------------------------------------------
// interface
//------------------------------------------------------------  
void PlainSocketImpl::create(__NET_HOOK_DECLARE_MORE bool stream)  __NET_THROW_DECLARE(SocketException) 
{
  SocketUtils::socketCreate(__NET_HOOK_INVOKE_MORE stream,&fd);
  __NET_HOOK_CHECK(return);
  
  if (p_socket != 0)
  {
    p_socket->setCreated();
  }
  
  if (p_serverSocket != 0)
  {
    p_serverSocket->setCreated();
  }
}


void PlainSocketImpl::connect(__NET_HOOK_DECLARE_MORE const string& host, int portValue) __NET_THROW_DECLARE(UnknownHostException, SocketException)
{
  SocketException pending;

  __OPENCODE_TRY 
  {
    InetAddress addressValue = InetAddress::getByName(__NET_HOOK_INVOKE_MORE host);

    __OPENCODE_TRY 
    {
      this->port = portValue;
      this->address = addressValue;

      connectToAddress(__NET_HOOK_INVOKE_MORE addressValue, portValue, timeout);
      return;
    } 
    __OPENCODE_CATCH (SocketException& ex) 
    {
      pending = ex;
    }
  } 
  __OPENCODE_CATCH (UnknownHostException e) 
  {
    pending = e;
  }

  // everything failed
  close();

#ifndef  OPENCODE_DISABLE_NET_SEH
  __OPENCODE_THROW(pending);
#endif
}

void PlainSocketImpl::connect(__NET_HOOK_DECLARE_MORE const InetAddress& addressValue, int portValue)  __NET_THROW_DECLARE(SocketException) 
{
  this->port = portValue;
  this->address = addressValue;
  
  __OPENCODE_TRY 
  {
    connectToAddress(__NET_HOOK_INVOKE_MORE addressValue, portValue, timeout);
    return;
  } 
  __OPENCODE_CATCH (SocketException& ex) 
  {
    // everything failed
    close();   
    __OPENCODE_RETHROW;
  }
}

void PlainSocketImpl::connect(__NET_HOOK_DECLARE_MORE const SocketAddress& addressValue, int timeoutValue)  __NET_THROW_DECLARE(SocketException) 
{
  this->port = addressValue.getPort();
  this->address = addressValue.getInetAddress();

  __OPENCODE_TRY 
  {
    connectToAddress(__NET_HOOK_INVOKE_MORE this->address, this->port, timeoutValue);
  } 
  __OPENCODE_CATCH (SocketException& ex) 
  {
    // everything failed
    close();
    __OPENCODE_RETHROW;
  }
}

void PlainSocketImpl::bind(__NET_HOOK_DECLARE_MORE const InetAddress& paddress, int lport) __NET_THROW_DECLARE(SocketException)
{
  SocketUtils::socketBind(__NET_HOOK_INVOKE_MORE fd,paddress, lport);
 
  this->localport = lport;

  if(this->localport == 0)
  {
    __OPENCODE_TRY
    {
      this->localport =  SocketUtils::socketGetLocalPort(__NET_HOOK_INVOKE_MORE fd);
    }
    __OPENCODE_CATCH(SocketException& ex){}    
  }

  if (p_socket != 0)
  {  
    p_socket->setBound();
  }
  
  if (p_serverSocket != 0)
  {
    p_serverSocket->setBound();
  }
}

void PlainSocketImpl::listen(__NET_HOOK_DECLARE_MORE int count)  __NET_THROW_DECLARE(SocketException) 
{
  SocketUtils::socketListen(__NET_HOOK_INVOKE_MORE fd,count);
}

void PlainSocketImpl::accept(__NET_HOOK_DECLARE_MORE SocketImpl* ps)  __NET_THROW_DECLARE(SocketException) 
{
  acquireFD();

  __OPENCODE_TRY 
  {
    SocketDescriptor __accept_fd;
    SocketAddress saddr;
   
    SocketUtils::socketAccept(__NET_HOOK_INVOKE_MORE fd,&__accept_fd,&saddr,timeout);
    __NET_HOOK_CHECK(return);
    
    PlainSocketImpl* ps2 = (PlainSocketImpl*)(ps);
    
    if(ps2 == 0)
    {
      __OPENCODE_THROW(SocketException("SocketImpl subclass not supported"));
    }

    ps2->fd = __accept_fd;

    ps2->localport = SocketUtils::socketGetLocalPort(__NET_HOOK_INVOKE_MORE __accept_fd);
    ps2->address   = saddr.getInetAddress();
    ps2->port      = saddr.getPort(); 
  } 
  __OPENCODE_CATCH(...) 
  {
    releaseFD();
  }
}

void PlainSocketImpl::shutdownInput() 
{
  if (fd.isValid()) 
  {
    SocketUtils::socketShutdown(fd,SHUT_W_RD);
    shut_rd = true;
  }
} 

void PlainSocketImpl::shutdownOutput() 
{
  if (fd.isValid()) 
  {
    SocketUtils::socketShutdown(fd,SHUT_W_WR);
    shut_wr = true;
  }
} 

void PlainSocketImpl::close() 
{
  if (fd.isValid())  
  {
    if (fdUseCount == 0) 
    {
      closePending = true;

      SocketUtils::socketClose(fd);
      fd.invalidate();

      return;
    } 
    else 
    {
      /*
       * If a thread has acquired the fd and a close
       * isn't pending then use a deferred close.
       * Also decrement fdUseCount to signal the last
       * thread that releases the fd to close it.
       */
      if (!closePending) 
      {
        closePending = true;
        fdUseCount--;

        SocketUtils::socketClose(fd);
      }
    }
  }
}
//------------------------------------------------------------
// interface
//------------------------------------------------------------
bool PlainSocketImpl::supportsUrgentData () 
{
  return true;
}

int PlainSocketImpl::read(__NET_HOOK_DECLARE_MORE char* b, int length)  __NET_THROW_DECLARE(SocketException)
{
  if (!fd.isValid()) 
  {
    __NET_HOOK_RETURN(return -1,SocketException,"Socket Closed");
  }
  
  if(shut_rd)
  {
    __NET_HOOK_RETURN(return -1,SocketException,"Socket input is already shutdown");
  }

  int nread = 0;
  bool gotReset = false;

  __OPENCODE_TRY
  {
    nread = SocketUtils::socketRead (__NET_HOOK_INVOKE_MORE fd,b,length,0,timeout);
  
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
    nread = SocketUtils::socketRead (__NET_HOOK_INVOKE_MORE fd,b,length,0,timeout);  
    if(nread > 0)
    {
      return nread;
    }
  }

  return -1;
}

int PlainSocketImpl::peek(__NET_HOOK_DECLARE_MORE char* b, int length)  __NET_THROW_DECLARE(SocketException)
{
  if (!fd.isValid()) 
  {
    __NET_HOOK_RETURN(return -1,SocketException,"Socket Closed");
  }
  
  if(shut_rd)
  {
    __NET_HOOK_RETURN(return -1,SocketException,"Socket input is already shutdown");
  }

  int nread = 0;
  bool gotReset = false;

  __OPENCODE_TRY
  {
    nread = SocketUtils::socketRead (__NET_HOOK_INVOKE_MORE fd,b,length,SocketUtils::C_MSG_PEEK,timeout);

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
    nread = SocketUtils::socketRead (__NET_HOOK_INVOKE_MORE fd,b,length,SocketUtils::C_MSG_PEEK,timeout);

    if(nread > 0)
    {
      return nread;
    }
  }

  return -1;
}
  
void PlainSocketImpl::write(__NET_HOOK_DECLARE_MORE const char* b, int length)  __NET_THROW_DECLARE(SocketException)
{
  if (!fd.isValid()) 
  {
    __NET_HOOK_RETURN(return,SocketException,"Socket Closed");
  }

  if(shut_wr)
  {
    __NET_HOOK_RETURN(return,SocketException,"Socket output is already shutdown");    
  }
    
  SocketUtils::socketWrite (__NET_HOOK_INVOKE_MORE fd,b,length,0);  
}

void PlainSocketImpl::sendUrgentData (__NET_HOOK_DECLARE_MORE int data)  __NET_THROW_DECLARE(SocketException) 
{
  if (!fd.isValid()) 
  {
    __NET_HOOK_RETURN(return,SocketException,"Socket Closed");
  }

  if(shut_rd)
  {
    __NET_HOOK_RETURN(return,SocketException,"Socket input is already shutdown");
  } 
     
  SocketUtils::socketSendUrgentData (__NET_HOOK_INVOKE_MORE fd,data);
}

//------------------------------------------------------------
// interface
//------------------------------------------------------------   
void PlainSocketImpl::setOption(__NET_HOOK_DECLARE_MORE int opt,const SocketOptionsValue& val)  __NET_THROW_DECLARE(SocketException) 
{
  if ((opt != SO_W_TIMEOUT) && isClosedOrPending()) 
  {
    __NET_HOOK_RETURN(return,SocketException,"Socket Closed");
  }
  
  bool on = true;
  switch (opt) 
  {
    /** check type safety b4 going native.*/
    case SOCKETOPTIONS_SO_W_LINGER:
    {
      if(    (SocketOptionsValue::SOV_INTEGER != val.sov_type) 
          && (SocketOptionsValue::SOV_BOOLEAN != val.sov_type))
      {
        __NET_HOOK_RETURN(return,SocketException,"Bad parameter for SO_W_LINGER");
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
        __NET_HOOK_RETURN(return,SocketException,"Bad parameter for SO_W_TIMEOUT");
      }
      
      int tmp = val.sov_int_value;
      if (tmp < 0)
      {
        __NET_HOOK_RETURN(return,SocketException,"timeout < 0");
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
       __NET_HOOK_RETURN(return,SocketException,"bad argument for IP_TOS");
     }
     trafficClass = val.sov_int_value;
    }
     break;

//--------------------------------------------------------------
//--------------------------------------------------------------      
    case SOCKETOPTIONS_SO_W_BINDADDR:
    {
      __NET_HOOK_RETURN(return,SocketException,"Cannot re-bind socket");
    }
    break;
         
//--------------------------------------------------------------
//--------------------------------------------------------------  
    case SOCKETOPTIONS_SO_W_TCP_NODELAY:
    {
      if (SocketOptionsValue::SOV_BOOLEAN != val.sov_type)
      {
        __NET_HOOK_RETURN(return,SocketException,"bad parameter for TCP_NODELAY");
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
        __NET_HOOK_RETURN(return,SocketException,"bad parameter for SO_W_RCVBUF");
      }
    }
    break;
//--------------------------------------------------------------
//--------------------------------------------------------------  
    case SOCKETOPTIONS_SO_W_KEEPALIVE:
    {
      if (SocketOptionsValue::SOV_BOOLEAN != val.sov_type)
      {
        __NET_HOOK_RETURN(return,SocketException,"bad parameter for SO_W_KEEPALIVE");
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
        __NET_HOOK_RETURN(return,SocketException,"bad parameter for SO_W_OOBINLINE");
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
        __NET_HOOK_RETURN(return,SocketException,"bad parameter for SO_W_REUSEADDR");
      }
      on = val.sov_bol_value;
    }
    break;


//--------------------------------------------------------------
//--------------------------------------------------------------        
    default:
    {
      StringBuffer __sb;
      __sb.append("unrecognized TCP option: "); 
      __sb.append(opt);
      __NET_HOOK_RETURN(return,SocketException,__sb.str());
    }
  }
  
  SocketUtils::socketSetOption(__NET_HOOK_INVOKE_MORE fd,opt, on, val);
}

SocketOptionsValue PlainSocketImpl::getOption(__NET_HOOK_DECLARE_MORE int opt)  __NET_THROW_DECLARE(SocketException) 
{
  if (isClosedOrPending()) 
  {
    SocketOptionsValue sov;
     __NET_HOOK_RETURN(return sov,SocketException,"Socket is closed");
  }
  
 
  if (opt == SO_W_TIMEOUT) 
  {
    SocketOptionsValue sov;
    
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
  
  switch (opt) 
  {
//--------------------------------------------------------------
//--------------------------------------------------------------     
    case SOCKETOPTIONS_SO_W_IP_TOS:
    {
      SocketOptionsValue sov = SocketUtils::socketGetOption(__NET_HOOK_INVOKE_MORE fd,opt);
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
//--------------------------------------------------------------
//--------------------------------------------------------------       
    default:
    {
      return SocketUtils::socketGetOption(__NET_HOOK_INVOKE_MORE fd,opt);
    }
  }
}

int PlainSocketImpl::getTimeout() 
{
  return timeout;
}
  
//------------------------------------------------------------
// interface
//------------------------------------------------------------
void PlainSocketImpl::connectToAddress(__NET_HOOK_DECLARE_MORE const InetAddress& addressValue, int portValue, int timeoutValue)  __NET_THROW_DECLARE(SocketException) 
{
  InetAddress addr_p = addressValue;
  if (InetAddress::isAnyAddress(addressValue)) 
  {
    addr_p = InetAddress::getLocalHost(__NET_HOOK_INVOKE_ONCE);
    __NET_HOOK_CHECK(return);  
  } 

  acquireFD();

  __OPENCODE_TRY 
  {
    SocketUtils::socketConnect(__NET_HOOK_INVOKE_MORE fd,addr_p, portValue, timeoutValue);
    __NET_HOOK_CHECK(return);
    
    if(localport<=0)
    {
      localport = SocketUtils::socketGetLocalPort(__NET_HOOK_INVOKE_MORE fd);
      __NET_HOOK_CHECK(return);  
    }

    // If we have a ref. to the Socket, then sets the flags
    // created, bound & connected to true.
    // This is normally done in Socket.connect() but some
    // subclasses of Socket may call impl.connect() directly!
    if (p_socket != 0) 
    {
      p_socket->setBound();
      p_socket->setConnected();
    }

    releaseFD();
  } 
  __OPENCODE_CATCH(SocketException& ex) 
  {
    releaseFD();
    __OPENCODE_RETHROW;
  } 
}


SocketDescriptor& PlainSocketImpl::acquireFD() 
{
  fdUseCount++;
  return fd;
}

void PlainSocketImpl::releaseFD() 
{
  fdUseCount--;
  if (fdUseCount == -1) 
  {
    if (fd.isValid()) 
    {
       SocketUtils::socketClose(fd);
       fd.invalidate();
    }
  }
}

bool PlainSocketImpl::isConnectionReset() 
{
  return (resetState == CONNECTION_RESET);
}

bool PlainSocketImpl::isConnectionResetPending() 
{
  return (resetState == CONNECTION_RESET_PENDING);
}

void PlainSocketImpl::setConnectionReset() 
{
  resetState = CONNECTION_RESET;
}

void PlainSocketImpl::setConnectionResetPending() 
{
  if (resetState == CONNECTION_NOT_RESET) 
  {
    resetState = CONNECTION_RESET_PENDING;
  }
}

bool PlainSocketImpl::isClosedOrPending() 
{
  if (closePending || !fd.isValid()) 
  {
    return true;
  } 

  return false;
}

__OPENCODE_END_NAMESPACE
