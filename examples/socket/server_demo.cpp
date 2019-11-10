#include "socket/inetAddress.h"
#include "socket/socket.h"
#include "socket/datagramSocket.h"
#include "socket/serverSocket.h"
#include "port/libsys.h"

#include <iostream>
#include <memory>

USING_OPENCODE_STL_NS
USING_OPENCODE_NS

//------------------------------------------------------------------
//demo function definition
//------------------------------------------------------------------

#define  __NET_HOOK_PRINTF() printf("[NET_TRACE] %s\n",__net_hook.getMessage().c_str())

void server_demo()
{
  __OPENCODE_TRY
  {
    ServerSocket server;
    __NET_HOOK_DEFINE(); 
    printf("-----------------------server.bind---...------------------------------\n");
    printf("LocalAddress : %s\n",InetAddress::getDotAddress(InetAddress::getLocalHost(__NET_HOOK_INVOKE_ONCE)).c_str());
    __NET_HOOK_CHECK(__NET_HOOK_PRINTF();return);
    
    server.bind(__NET_HOOK_INVOKE_MORE InetAddress::getLocalHost(__NET_HOOK_INVOKE_ONCE),12345);
    __NET_HOOK_CHECK(__NET_HOOK_PRINTF();return);
    printf("-----------------------server.bind---OK-----[%s]-------------------------\n",server.toString().c_str());

    while(true)
    {
      __OPENCODE_TRY
      {
        __NET_HOOK_RESET();
        Socket socket;
        server.accept(__NET_HOOK_INVOKE_MORE socket);
        __NET_HOOK_CHECK(__NET_HOOK_PRINTF();return);
        printf("-----------------------server.accept---OK------------------------------\n");
  
        printf("[R] Ip=%s,port=%d\n",InetAddress::getDotAddress(socket.getInetAddress()).c_str(),socket.getPort());
        printf("[L] Ip=%s,port=%d\n",InetAddress::getDotAddress(socket.getLocalAddress()).c_str(),socket.getLocalPort());
  
        socket.write(__NET_HOOK_INVOKE_MORE "1234",4);
        __NET_HOOK_CHECK(__NET_HOOK_PRINTF();return);
         
        socket.close();
      }
      __OPENCODE_CATCH(SocketException& ex)
      {
        printf("Exception : %s,%d\n",ex.getMessage().c_str(),ex.getErrorCode());
      }
    }
    server.close();
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
    DatagramSocket ds(__NET_HOOK_INVOKE_MORE 10002);
    __NET_HOOK_CHECK(__NET_HOOK_PRINTF();return);
    
    char msg[1024] = {0};
    strncpy(msg,"1234",4);

    SocketAddress addr(InetAddress::getLocalHost(__NET_HOOK_INVOKE_ONCE),10001);
    __NET_HOOK_CHECK(__NET_HOOK_PRINTF();return);  
    
    printf("---------------send ...---------------\n");
    ds.send(__NET_HOOK_INVOKE_MORE &addr,msg,4);
    __NET_HOOK_CHECK(__NET_HOOK_PRINTF();return);
    printf("---------------send ok---------------\n");
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
  server_demo();
  
  return 0;
}
