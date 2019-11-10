#include "port/asn1parser.h"
#include "port/libsys.h"
#include "port/file.h"
#include "port/utilities.h"

#include <iostream>
USING_OPENCODE_STL_NS
USING_OPENCODE_NS

//------------------------------------------------------------------
//demo function definition
//------------------------------------------------------------------
class BERFilenameFilter : public FilenameFilter
{
public:
  ~BERFilenameFilter(){}
  BERFilenameFilter(){}

public:
  bool accept(const File& dir, const string& name) const
  {
    return StringsKit::endsWith(name,".ber");    
  }
};

int asn1parser_demo_0(const string& filename,Asn1Group& asn1Group)
{
  printf("filename <%s>",filename.c_str());
  FILE* fp = fopen(filename.c_str(),"rb");
  if( fp == 0 ) 
  {
    printf(" not found!\n");
    return -1 ;
  }
  printf(" found!\n");

  fseek(fp,0,SEEK_END);
  size_t __fileSize = ftell(fp);

  char* __pbuff = new char[__fileSize + 1];

  fseek(fp,0,SEEK_SET);
  fread(__pbuff,__fileSize,1,fp);
  __pbuff[__fileSize] = 0;
  fclose(fp);  
  
//  printf("\n%s\n",MemoryKit::memDump(__pbuff,__fileSize).c_str());
  
  int __ret_code = Asn1Parser::parse(__pbuff,__fileSize,asn1Group);
  printf("__ret_code = %d\n",__ret_code);
  
  StringBuffer __sb; 
  Asn1Format::toTEXT(&asn1Group,__sb);
  printf("%s\n",__sb.str().c_str());
 
  delete []__pbuff;
  
  return 0;
}

int asn1CDR_demo(int argc,char** argv)
{
  string __curdir("."); 
  if(argc >  1)
  {
    __curdir = argv[1]; 
  }

  File curFile(__curdir);
  vector<string> ber_files;
  BERFilenameFilter ber_fnf;
  curFile.list(ber_fnf,ber_files);

  for(int i = 0; i < ber_files.size(); ++i)
  {
    Asn1Group asn1Group;
    asn1parser_demo_0(__curdir + "/"+ber_files[i],asn1Group);
  }

  return 0;
}

int asn1parser_demo(int argc,char** argv)
{
  string filename;
  if(argc == 1)
  {
    filename = "GCDR2007080312220000.ber";
  }
  else
  {
    filename = argv[1];
  }

  Asn1Group asn1Group;
  int __ret_code = asn1parser_demo_0(filename,asn1Group);

  return  __ret_code;
}

int asn1parser_decode_demo(int argc,char** argv)
{
  string filename;
  if(argc == 1)
  {
    filename = "GCDR2007080312220000.ber";
  }
  else
  {
    filename = argv[1];
  }

  Asn1Group asn1Group;
  int __ret_code = asn1parser_demo_0(filename,asn1Group);
  if(__ret_code < 0)
  {
    return -1;
  } 

///////////////////////////////////
  Asn1Container* __p_ac = asn1Group.search("A1");
  Asn1ContainerEntry* __p_ace = __p_ac->getData()->at(0);
  Asn1Group* __p_ag = &(__p_ace->grp);

//----------------------------------

  __p_ac = __p_ag->search("83");
  if(__p_ac != 0)
  {
    __p_ace = __p_ac->getData()->at(0);

    string __hex;
    Asn1Codec::decode_hex((char*)&__hex,__p_ace->str.data(),__p_ace->str.size());
    cout << "__decode_hex[83] = [" << __hex << "]" << endl;

    string __tbcd;
    Asn1Codec::decode_tbcd((char*)&__tbcd,__p_ace->str.data(),__p_ace->str.size());
    cout << "__decode_tbcd[83] = [" << __tbcd << "]" << endl;
  }
//----------------------------------

  __p_ac = __p_ag->search("96");
  if(__p_ac != 0)
  {
    __p_ace = __p_ac->getData()->at(0);

    string __hex;
    Asn1Codec::decode_hex((char*)&__hex,__p_ace->str.data(),__p_ace->str.size());
    cout << "__decode_hex[96] = [" << __hex << "]" << endl;

    string __tbcd;
    ASN1_ADDRESS __addr;
    Asn1Codec::decode_address((char*)&__addr,__p_ace->str.data(),__p_ace->str.size());
    cout << "__decode_address[96] = [" << __addr.address << "]" << endl;
  }
//----------------------------------

  __p_ac = __p_ag->search("8D");
  if(__p_ac != 0)
  { 
    __p_ace = __p_ac->getData()->at(0);

    string __hex;
    Asn1Codec::decode_hex((char*)&__hex,__p_ace->str.data(),__p_ace->str.size());
    cout << "__decode_hex[8D] = [" << __hex << "]" << endl;

    string __tbcd;
    ASN1_TIMESTAMP __ts;
    Asn1Codec::decode_timestamp((char*)&__ts,__p_ace->str.data(),__p_ace->str.size());
    printf("__decode_timestamp[8D] = [%04d-%02d-%02d %02d:%02d:%02d <%d>]\n",
          __ts.year + 2000,
          __ts.month,
          __ts.day,
          __ts.hour,
          __ts.minute,
          __ts.second,
          (int)__ts.tzOffset);
  }
///////////////////////////////////

  return  __ret_code;
}

int asn1parser_high_tag_demo()
{
  char __pbuf[4];
  int __fileSize = 4;

  __pbuf[0] = 0x5F;
  __pbuf[1] = 0x21;
  __pbuf[2] = 0x01;
  __pbuf[3] = 0x6E;
 
  Asn1Group asn1Group;
  int __ret_code = Asn1Parser::parse(__pbuf,__fileSize,asn1Group);
  printf("__ret_code = %d\n",__ret_code);

  StringBuffer __sb;
  Asn1Format::toTEXT(&asn1Group,__sb);
  printf("%s\n",__sb.str().c_str());
   
  return 0;
}

int asn1parser_long_len_demo()
{
  char __pbuf[4];
  int __fileSize = 4;

  __pbuf[0] = 0x04;
  __pbuf[1] = 0x81;
  __pbuf[2] = 0x01;
  __pbuf[3] = 0x61;

  Asn1Group asn1Group;
  int __ret_code = Asn1Parser::parse(__pbuf,__fileSize,asn1Group);
  printf("__ret_code = %d\n",__ret_code);

  StringBuffer __sb;
  Asn1Format::toTEXT(&asn1Group,__sb);
  printf("%s\n",__sb.str().c_str());

  return 0;
}

int asn1parser_null_demo()
{
  char __pbuf[2];
  int __fileSize = 2;

  __pbuf[0] = 0x05;
  __pbuf[1] = 0x00;

  Asn1Group asn1Group;
  int __ret_code = Asn1Parser::parse(__pbuf,__fileSize,asn1Group);
  printf("__ret_code = %d\n",__ret_code);

  StringBuffer __sb;
  Asn1Format::toTEXT(&asn1Group,__sb);
  printf("%s\n",__sb.str().c_str());

  return 0;  
}
  
//------------------------------------------------------------------
//demo entry point 
//------------------------------------------------------------------
int main(int argc,char** argv)
{
  System::initializeEnvironment(argc,argv);

  asn1CDR_demo(argc,argv);    
  //asn1parser_decode_demo(argc,argv);
  
  //asn1parser_high_tag_demo();
  //asn1parser_long_len_demo();  
  //asn1parser_null_demo();

  return 0;
}
