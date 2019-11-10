#include "socket/multicastSocket.h"
#include "socket/datagramSocketImpl.h"
#include "socket/socketOptions.h"
#include "socket/networkUtils.h"

__OPENCODE_BEGIN_NAMESPACE


//------------------------------------------------------------------------
//
//------------------------------------------------------------------------ 
MulticastSocket::~MulticastSocket() 
{
}

MulticastSocket::MulticastSocket() 
:DatagramSocket()
{
}
  

MulticastSocket::MulticastSocket(__NET_HOOK_DECLARE_MORE int port)  __NET_THROW_DECLARE(SocketException)
:DatagramSocket(__NET_HOOK_INVOKE_MORE port)
{
}

MulticastSocket::MulticastSocket(__NET_HOOK_DECLARE_MORE const SocketAddress& bindaddr)  __NET_THROW_DECLARE(SocketException)
:DatagramSocket(__NET_HOOK_INVOKE_MORE bindaddr)
{
}

MulticastSocket::MulticastSocket(const MulticastSocket& rv)
:DatagramSocket(rv)
{
  infAddress = rv.infAddress;
}

MulticastSocket& MulticastSocket::operator=(const MulticastSocket& rv)
{
  if(&rv == this)
  {
    return *this;
  } 

  DatagramSocket::operator=(rv);

  infAddress = rv.infAddress;

  return *this;
}

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------ 
void MulticastSocket::setTimeToLive(__NET_HOOK_DECLARE_MORE int ttl)  __NET_THROW_DECLARE(SocketException)
{
  if (isClosed())
  {
    __NET_HOOK_RETURN(return,SocketException,"Socket is closed");
  }
  
  getImpl(__NET_HOOK_INVOKE_ONCE)->setTimeToLive(__NET_HOOK_INVOKE_MORE ttl);  
}

int MulticastSocket::getTimeToLive(__NET_HOOK_DECLARE_ONCE)  __NET_THROW_DECLARE(SocketException) 
{
  if (isClosed())
  {
    __NET_HOOK_RETURN(return -1,SocketException,"Socket is closed");
  }

  return getImpl(__NET_HOOK_INVOKE_ONCE)->getTimeToLive(__NET_HOOK_INVOKE_ONCE);
}

void MulticastSocket::joinGroup(__NET_HOOK_DECLARE_MORE const InetAddress& mcastaddr)  __NET_THROW_DECLARE(SocketException) 
{
  if (isClosed()) 
  {
    __NET_HOOK_RETURN(return,SocketException,"Socket is closed");  
  }

  if (!InetAddress::isMulticastAddress(mcastaddr)) 
  {
    __NET_HOOK_RETURN(return,SocketException,"Not a multicast address");
  }

  getImpl(__NET_HOOK_INVOKE_ONCE)->join(__NET_HOOK_INVOKE_MORE mcastaddr);
}

void MulticastSocket::leaveGroup(__NET_HOOK_DECLARE_MORE const InetAddress& mcastaddr)  __NET_THROW_DECLARE(SocketException) 
{
  if (isClosed()) 
  {
    __NET_HOOK_RETURN(return,SocketException,"Socket is closed");  
  }
  
  if (InetAddress::isMulticastAddress(mcastaddr)) 
  {
    __NET_HOOK_RETURN(return,SocketException,"Not a multicast address");
  }
  
  getImpl(__NET_HOOK_INVOKE_ONCE)->leave(__NET_HOOK_INVOKE_MORE mcastaddr);
}

void MulticastSocket::joinGroup(__NET_HOOK_DECLARE_MORE const SocketAddress& mcastaddr,const NetworkInterface& netIf) __NET_THROW_DECLARE(SocketException) 
{
  if (isClosed())
  {
    __NET_HOOK_RETURN(return,SocketException,"Socket is closed");  
  }

  if (!InetAddress::isMulticastAddress(mcastaddr.getInetAddress())) 
  {
    __NET_HOOK_RETURN(return,SocketException,"Not a multicast address");
  }

  getImpl(__NET_HOOK_INVOKE_ONCE)->joinGroup(__NET_HOOK_INVOKE_MORE mcastaddr, netIf);
}

void MulticastSocket::leaveGroup(__NET_HOOK_DECLARE_MORE const SocketAddress& mcastaddr, const NetworkInterface& netIf) __NET_THROW_DECLARE(SocketException) 
{
  if (isClosed())
  {
    __NET_HOOK_RETURN(return,SocketException,"Socket is closed");  
  }

  if (!InetAddress::isMulticastAddress(mcastaddr.getInetAddress())) 
  {
    __NET_HOOK_RETURN(return,SocketException,"Not a multicast address");
  }
  
  getImpl(__NET_HOOK_INVOKE_ONCE)->leaveGroup(__NET_HOOK_INVOKE_MORE mcastaddr, netIf);
}

void MulticastSocket::setInterface(__NET_HOOK_DECLARE_MORE const InetAddress& inf)  __NET_THROW_DECLARE(SocketException) 
{
  if (isClosed())
  {
    __NET_HOOK_RETURN(return,SocketException,"Socket is closed");  
  }
  
  SocketOptionsValue value;
  value.sov_type = SocketOptionsValue::SOV_INETADDRESS;
  value.sov_inet_value = inf;

  getImpl(__NET_HOOK_INVOKE_ONCE)->setOption(__NET_HOOK_INVOKE_MORE SocketOptions::SO_W_IP_MULTICAST_IF, value);
  
  infAddress = inf;
}

InetAddress MulticastSocket::getInterface(__NET_HOOK_DECLARE_ONCE)  __NET_THROW_DECLARE(SocketException) 
{
  if (isClosed())
  {
    __NET_HOOK_RETURN(return InetAddress::nullAddress(),SocketException,"Socket is closed");  
  }
  
  InetAddress ia;
    
  SocketOptionsValue value= getImpl(__NET_HOOK_INVOKE_ONCE)->getOption(__NET_HOOK_INVOKE_MORE SocketOptions::SO_W_IP_MULTICAST_IF);
  __NET_HOOK_CHECK(return ia);
  
  /**
   * No previous setInterface or interface can be
   * set using setNetworkInterface
   */
  if (InetAddress::isNullAddress(infAddress)) 
  {
    return ia;
  }

  /**
   * Same interface set with setInterface?
   */
  if (ia.equals(infAddress)) 
  {
    return ia;
  }

  /**
   * Different InetAddress from what we set with setInterface
   * so enumerate the current interface to see if the 
   * address set by setInterface is bound to this interface.
   */
  __OPENCODE_TRY 
  {
    NetworkInterface ni = NetworkUtils::netifGetByInetAddress(__NET_HOOK_INVOKE_MORE ia);
    __NET_HOOK_CHECK(return ia);
  
    vector<InetAddress> addrs = ni.getInetAddresses();
    
    for(size_t i=0;i<addrs.size();++i)
    {
      if (addrs[i].equals(infAddress)) 
      {
         return infAddress;
       }
    }

    /**
     * No match so reset infAddress to indicate that the
     * interface has changed via means
     */
     infAddress = InetAddress::nullAddress();
     return ia;
  } 
  __OPENCODE_CATCH (SocketException& e) 
  {
    return ia;
  }  
}

void MulticastSocket::setNetworkInterface(__NET_HOOK_DECLARE_MORE const NetworkInterface& netIf) __NET_THROW_DECLARE(SocketException) 
{
  SocketOptionsValue value;
  value.sov_type = SocketOptionsValue::SOV_NETWORKINTERFACE;
  value.sov_netif_value = netIf;
  
  getImpl(__NET_HOOK_INVOKE_ONCE)->setOption(__NET_HOOK_INVOKE_MORE SocketOptions::SO_W_IP_MULTICAST_IF2, value);
  infAddress = InetAddress::nullAddress();
}

NetworkInterface MulticastSocket::getNetworkInterface(__NET_HOOK_DECLARE_ONCE)  __NET_THROW_DECLARE(SocketException) 
{
  SocketOptionsValue value = getImpl(__NET_HOOK_INVOKE_ONCE)->getOption(__NET_HOOK_INVOKE_MORE SocketOptions::SO_W_IP_MULTICAST_IF2);
  NetworkInterface ni = value.sov_netif_value;
  __NET_HOOK_CHECK(return ni); 

  if (ni.getIndex() == 0) 
  {
    vector<InetAddress> addrs;
    addrs.push_back(InetAddress::getLocalHost(__NET_HOOK_INVOKE_ONCE));
    
    return NetworkInterface(addrs[0].getHostName(), 0,ni.getInterfaceAddress(), addrs);
  } 
  else 
  {
    return ni;
  }
}


void MulticastSocket::setLoopbackMode(__NET_HOOK_DECLARE_MORE bool disable)  __NET_THROW_DECLARE(SocketException) 
{
  SocketOptionsValue value;
  value.sov_type = SocketOptionsValue::SOV_BOOLEAN;
  value.sov_bol_value = disable;
   
  getImpl(__NET_HOOK_INVOKE_ONCE)->setOption(__NET_HOOK_INVOKE_MORE SocketOptions::SO_W_IP_MULTICAST_LOOP, value);
}


bool MulticastSocket::getLoopbackMode(__NET_HOOK_DECLARE_ONCE)  __NET_THROW_DECLARE(SocketException) 
{
  return (getImpl(__NET_HOOK_INVOKE_ONCE)->getOption(__NET_HOOK_INVOKE_MORE SocketOptions::SO_W_IP_MULTICAST_LOOP)).sov_bol_value;
}

__OPENCODE_END_NAMESPACE
