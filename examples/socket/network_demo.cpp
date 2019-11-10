#include "socket/socket.h"
#include "socket/datagramSocket.h"
#include "socket/networkInterface.h"
#include "socket/networkUtils.h"
#include "port/libsys.h"

#include <iostream>
#include <memory>

USING_OPENCODE_STL_NS
USING_OPENCODE_NS

//------------------------------------------------------------------
//demo function definition
//------------------------------------------------------------------

#define  __NET_HOOK_PRINTF() printf("[NET_TRACE] %s\n",__net_hook.getMessage().c_str())

void network_demo()
{
  printf("=== network_demo ===\n");

  __OPENCODE_TRY
  {
    __NET_HOOK_DEFINE();
    vector<NetworkInterface> netifs;
    NetworkInterface::getNetworkInterfaces(__NET_HOOK_INVOKE_MORE netifs);
    __NET_HOOK_CHECK(__NET_HOOK_PRINTF();return);
    
    for(int i = 0;i < netifs.size();++i)
    {
      printf("netif name : %s , netif addr : ",netifs[i].getName().c_str());

      vector<unsigned char> netifaddr = netifs[i].getInterfaceAddress();
      for(int j = 0 ; j < netifaddr.size(); ++j)
      {
        printf(":%02x",netifaddr[j]);
      }
      printf("\n");

      printf("InetAddress List:\n");
     
      const vector<InetAddress>& addrs = netifs[i].getInetAddresses(); 
      for(int j = 0 ; j < addrs.size(); ++j) 
      {
        printf("%s\n",addrs[j].toString().c_str());
      }
    }
  }
  __OPENCODE_CATCH(SocketException& sex)
  {
    printf("Exception : %s,%d\n",sex.getMessage().c_str(),sex.getErrorCode());
  }
}

void inetadress_demo()
{
  printf("=== inetadress_demo ===\n");
  __OPENCODE_TRY
  {
    __NET_HOOK_DEFINE();
    printf(" hostname : %s\n",NetworkUtils::inetGetLocalHostName().c_str());
    printf(" address  : %s\n",InetAddress::getDotAddress(InetAddress::getLocalHost(__NET_HOOK_INVOKE_ONCE)).c_str());
    __NET_HOOK_CHECK(__NET_HOOK_PRINTF();return);
    
    printf("========================================\n");
    vector<InetAddress> addrs;
    InetAddress::getAllByName(__NET_HOOK_INVOKE_MORE addrs,NetworkUtils::inetGetLocalHostName());
    __NET_HOOK_CHECK(__NET_HOOK_PRINTF();return);
      
    for(int j = 0 ; j < addrs.size(); ++j)
    {
      printf("%s\n",addrs[j].toString().c_str());
    }
  }
  __OPENCODE_CATCH(SocketException& sex)
  {
    printf("Exception : %s,%d\n",sex.getMessage().c_str(),sex.getErrorCode());
  }
}

//------------------------------------------------------------------
//demo entry point 
//------------------------------------------------------------------
int main(int argc,char** argv)
{
  System::initializeEnvironment(argc,argv);
  printf("--XXXXXXXXXXXXXXXXXXXXXXXXXXXX--\n");
  NetworkUtilsImpl::getInstance().IPv6_available();
  
  network_demo();
  inetadress_demo();

  return 0;
}
