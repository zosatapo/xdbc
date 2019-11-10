#include "port/libsys.h"
#include "port/calendar.h"
#include "port/date.h"
#include "port/timestamp.h"
#include "port/time.h"
#include "port/systemLibrary.h"
#include "port/unicode.h"
#include <iostream>
#include <memory>
#include <vector>

#if defined(PORT_WINDOWS)
#  include <mbstring.h>
#  define OPENCODE_APIHOOK_MBLEN(__api_BUF,__api_COUNT) _mbclen(__api_BUF)
#else
#  define OPENCODE_APIHOOK_MBLEN(__api_BUF,__api_COUNT) mblen(__api_BUF,__api_COUNT)
#endif

USING_OPENCODE_STL_NS
USING_OPENCODE_NS

//------------------------------------------------------------------
//demo function definition
//------------------------------------------------------------------
void system_demo()
{
  vector<string> names;
  System::propertyNames(names);
  for(int i = 0; i < names.size() ; ++i)
  {
    cout << names[i] << " = " << System::getProperty(names[i]) << endl;
  }
}

void sysfunc_demo()
{
  char __uchar[8] = {0};
  memcpy(__uchar,"ÖÐÎÄAºº",7);
  printf("__uchar : %s <%s>\n",__uchar,MemoryKit::numDump((const char*)__uchar,7).c_str()); 
  
  int __mblen = OPENCODE_APIHOOK_MBLEN((unsigned char*)__uchar,7);
  printf("__mblen : %d\n",__mblen);

  wchar_t __wchar[1024 + 1] = {0};
  int __wchar_size = 0;

  char __ansi[1024 + 1] = {0};
  int __ansi_size = 0;

  __wchar_size = mbstowcs(__wchar,__uchar,7);
  printf("mbstowcs : %03d [%s]\n",__wchar_size,MemoryKit::numDump((const char*)__wchar,__wchar_size*4).c_str()); 
  
  __ansi_size = wcstombs(__ansi,__wchar,1024);
  printf("wcstombs : %03d [%s]\n",__ansi_size,MemoryKit::numDump((const char*)__ansi,__ansi_size).c_str());
  printf("__ansi : %s\n",__ansi);  

  __wchar_size = MultiByteToWideChar (CP_ACP, 0, __uchar, 7, __wchar, 1024);
  printf("MultiByteToWideChar : %03d [%s]\n",__wchar_size,MemoryKit::numDump((const char*)__wchar,__wchar_size*4).c_str()); 

  __ansi_size = WideCharToMultiByte(CP_ACP, 0,__wchar,__wchar_size,__ansi,1024,0,0);
  printf("wcstombs : %03d [%s]\n",__ansi_size,MemoryKit::numDump((const char*)__ansi,__ansi_size).c_str());
  printf("__ansi : %s\n",__ansi);
}

void ansi_utf8_demo()
{
  char __uchar[8] = {0xD6,0xD0,0xCE,0xC4,'A',0xBA,0xBA,0};
  printf("__uchar : %s\n",__uchar);
  printf("__uchar : %03d [%s]\n",7,MemoryKit::numDump(__uchar,7).c_str());

  UTF8 __utf8[1024] = {0};

  int __utf8_size = Unicode::ansi_to_utf8(__uchar,7,__utf8,1024);  
  printf("ansi_to_utf8 : %03d [%s]\n",__utf8_size,MemoryKit::numDump((const char*)__utf8,__utf8_size).c_str());
  printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
  char __ansi[1024] = {0};

  int __ansi_size = Unicode::utf8_to_ansi(__utf8,__utf8_size,__ansi,1024);
  printf("utf8_to_ansi : %03d [%s]\n",__ansi_size,MemoryKit::numDump((const char*)__ansi,__ansi_size).c_str());
  printf("__ansi : %s\n",__ansi);
}

void wordSizeDemo()
{
  cout << "sizeof(int)   = " << sizeof(int)   << endl; 
  cout << "sizeof(long)  = " << sizeof(long)  << endl; 
  cout << "sizeof(XLONG) = " << sizeof(XLONG) << endl;
#ifdef OPENCODE_LP64
  cout << "MARCO OPENCODE_LP64  defined" << endl;
#else
  cout << "MARCO OPENCODE_LP64  undefined" << endl;
#endif  
}

void calendar_demo()
{
  auto_ptr<TimeZone> def(TimeZone::getDefault()->clone());
  cout << "------ " << def->toString() << endl;

  auto_ptr<Calendar> cal(Calendar::getInstance());
  cout << "------ " << cal->toString() << endl;

  Date date;
  XLONG times = date.getTime();
  cout << "------ " << date.toString() << " --||" << MemoryKit::encode16((const char*)&times,sizeof(times)) << endl;

  Time time;
  cout << "------ " << time.toString() << endl;

  Timestamp ts;
  cout << "------ " << ts.toString() << endl;
} 

void syslib_demo()
{
  __OPENCODE_TRY
  {
    SystemLibrary slib = SystemLibrary::getInstance("D:\\instantclient_10_2\\sqora32.dll");
    slib.close();
  }
  __OPENCODE_CATCH(BaseException& ex)
  {
    cout << ex.getMessage() << endl;
  }
}

void memUtilities_demo()
{
  char p[20]={"0123456789ABCDEF"};
  p[0] = 10;

  cout << "== MemoryKit::numDump ==" <<endl;
  cout << MemoryKit::numDump(p,strlen(p)) << endl;

  string data;
  cout << "== MemoryKit::encode16 ==" <<endl;
  MemoryKit::encode16(data,p,strlen(p));
  cout << MemoryKit::numDump(data.data(),data.size()) << endl;

  string data2;
  cout << "== MemoryKit::dencode16 ==" <<endl;
  MemoryKit::decode16(data2,data.data(),data.size());
  cout << MemoryKit::numDump(data2.data(),data2.size()) << endl;
}

//------------------------------------------------------------------
//demo entry point 
//------------------------------------------------------------------
int main(int argc,char** argv)
{
  System::initializeEnvironment(argc,argv);
  
  system_demo();

  sysfunc_demo();
  //ansi_utf8_demo();
  //syslib_demo();
  //memUtilities_demo();

  LifeTimeGlobals::__lifetime_destroy();
  System::waitAnyKey();

  return 0;
}
