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

void socket_demo()
{
  __OPENCODE_TRY
  {
    __NET_HOOK_DEFINE();  
    Socket s;
    s.connect(__NET_HOOK_INVOKE_MORE InetAddress::getLocalHost(__NET_HOOK_INVOKE_ONCE),12345);
    __NET_HOOK_CHECK(__NET_HOOK_PRINTF();return);
    printf("----1 [%s]\n",s.toString().c_str());

    char buffer[8092] = {0};

    printf("----2 reading\n");
    s.read(__NET_HOOK_INVOKE_MORE buffer,8092);
    __NET_HOOK_CHECK(__NET_HOOK_PRINTF();return);
    printf("----read : %s\n",buffer);
    s.close();

  }
  __OPENCODE_CATCH(SocketException& sex)
  {
    printf("Exception : %s,%d\n",sex.getMessage().c_str(),sex.getErrorCode());
  }
}

void datagramSocket_demo()
{
  __OPENCODE_TRY
  {
    __NET_HOOK_DEFINE(); 
    DatagramSocket ds(__NET_HOOK_INVOKE_MORE 10001,InetAddress::getLocalHost(__NET_HOOK_INVOKE_ONCE));
    __NET_HOOK_CHECK(__NET_HOOK_PRINTF();return);
    
    char msg[1024] = {0};
   
    SocketAddress addr; 

    printf("---------- receive ... ----------\n");
    ds.receive(__NET_HOOK_INVOKE_MORE &addr,msg,1024);
    __NET_HOOK_CHECK(__NET_HOOK_PRINTF();return);   
    printf("---------- receive ok ----------\n");

    cout<<addr.toString()<<" | "<<msg<<endl;; 
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
  socket_demo();

//  datagramSocket_demo();

  return 0;
}
