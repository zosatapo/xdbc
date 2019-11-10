#include "port/libsys.h"
#include "port/utilities.h"
#include "port/unicode.h" 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <iostream>
#include <memory>

USING_OPENCODE_STL_NS
USING_OPENCODE_NS

//------------------------------------------------------------------
//demo function definition
//------------------------------------------------------------------
void sysfunc_demo()
{
  char __uchar[8] = {0xA1,0xDE,0xA1,0xD8,'A',0xD6,0xD0,0};
  printf("__uchar : %s\n",__uchar); 

  int __mblen = mblen(__uchar,7);
  printf("__mblen : %d\n",__mblen);

  if(__mblen > 0)
  {
    wchar_t __wchar[1024] = {0};
  
    int __wchar_size = mbstowcs(__wchar,__uchar,7);
    printf("__wchar_size : %d\n",__wchar_size);
    printf("mbstowcs : %03d [%s]\n",__wchar_size,MemoryKit::numDump((const char*)__wchar,__wchar_size*4).c_str()); 
  
    char __ansi[1024] = {0};

    int __ansi_size = wcstombs(__ansi,__wchar,1024);
    printf("__ansi_size : %d\n",__ansi_size);
    printf("wcstombs : %03d [%s]\n",__ansi_size,MemoryKit::numDump((const char*)__ansi,__ansi_size).c_str());
    printf("__ansi : %s\n",__ansi);  
  }
}

void ansi_utf8_demo()
{
  char __uchar[8] = {0xA1,0xDE,0xA1,0xD8,'A',0xD6,0xD0,0};
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
  
//------------------------------------------------------------------
//demo entry point 
//------------------------------------------------------------------
int main(int argc,char** argv)
{
  System::initializeEnvironment(argc,argv);
  
  sysfunc_demo();
  ansi_utf8_demo();
    
  return 0;
}
