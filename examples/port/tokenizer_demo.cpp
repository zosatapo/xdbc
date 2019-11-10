#include "port/stringTokenizer.h"
#include "port/libsys.h"

#include <iostream>
#include <memory>

USING_OPENCODE_STL_NS
USING_OPENCODE_NS

//------------------------------------------------------------------
//demo function definition
//------------------------------------------------------------------
void tokenizer_demo()
{
  StringTokenizer tokens("123|456\t789","|\t");

  while(tokens.hasMoreTokens())
  {
    cout << "Token : |" << tokens.nextToken() << "|" << endl;
  }

  cout << "----------------------------------------" << endl;

  StringTokenizer tokens2("123|456\t789","|\t",true);

  while(tokens2.hasMoreTokens())
  {
    cout << "Token : |" << tokens2.nextToken() << "|" << endl;
  }
  
}
//------------------------------------------------------------------
//demo entry point 
//------------------------------------------------------------------
int main(int argc,char** argv)
{
  System::initializeEnvironment(argc,argv);
  
  tokenizer_demo();
    
  return 0;
}
