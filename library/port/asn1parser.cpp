#include "port/asn1parser.h"
#include "port/utilities.h"
#include "port/file.h"
#include "port/ioc_smart.h"
#include "port/libnumber.h"
#include "port/codec.h" 

__OPENCODE_BEGIN_NAMESPACE

Asn1Tag::~Asn1Tag(){}

Asn1Tag::Asn1Tag()
:clazz(0x00),constructed(false),low_tag_number(0x02),indefinite(false)
{}

Asn1Tag::Asn1Tag(int clazz,bool  constructed,int low_tag_number,const std::string& tag,bool indefinite)
:clazz(clazz),constructed(constructed),low_tag_number(low_tag_number),tag(tag),indefinite(indefinite)
{}

int Asn1Tag::extractTAG(const Asn1Tag& tag)
{
  if(tag.low_tag_number < ASN1_MASK_HIGH_TAG_NUMBER)
  {
    return tag.low_tag_number;
  }
 
  string __encoded_tag_number = tag.tag.substr(2);
  string __raw_tag_number;
  MemoryKit::decode16(__raw_tag_number,__encoded_tag_number.data(),__encoded_tag_number.size());
  
  int __tag_number_size  =  (int)(__raw_tag_number.size());
  int __tag_number = 0;
  
  for(int i = 0; i < __tag_number_size; ++i)
  {
    __tag_number = __tag_number * ASN1_HIGH_TAG_NUMBER_BASE + (__raw_tag_number[i] & 0x7F);
  } 
  
  return   __tag_number;
}

Asn1ContainerEntry::~Asn1ContainerEntry(){}
Asn1ContainerEntry::Asn1ContainerEntry(){}

Asn1ContainerEntry::Asn1ContainerEntry(const Asn1ContainerEntry& rv)
{
  this->str = rv.str;
  this->grp = rv.grp;
}

Asn1ContainerEntry& Asn1ContainerEntry::operator=(const Asn1ContainerEntry& rv)
{
  if(&rv == this)
  {
    return *this;
  }

  this->str = rv.str;
  this->grp = rv.grp;
    
  return *this;
}
  
Asn1Container::~Asn1Container()
{
  releaseEntries();
}

Asn1Container::Asn1Container()
{
}
Asn1Container::Asn1Container(const Asn1Tag& tag)
:tag(tag)
{
}
  
Asn1Container::Asn1Container(const Asn1Container& rv)
{
  vector<Asn1ContainerEntry*>::const_iterator __pos = rv.entries.begin();
  vector<Asn1ContainerEntry*>::const_iterator __end = rv.entries.end();
//  for(;;)
//  {
//    if(__pos == __end){break;} 
  while(__pos != __end)
  {
    add(new Asn1ContainerEntry(*(*__pos)));    
    ++__pos;
  }
  
  this->tag = rv.tag; 
}

Asn1Container& Asn1Container::operator=(const Asn1Container& rv)
{
  if(&rv == this)
  {
    return *this;
  }
  
  releaseEntries();
    
  vector<Asn1ContainerEntry*>::const_iterator __pos = rv.entries.begin();
  vector<Asn1ContainerEntry*>::const_iterator __end = rv.entries.end();
  while(__pos != __end)
  { 

    add(new Asn1ContainerEntry(*(*__pos)));    
    ++__pos;
  }
  
  this->tag = rv.tag; 
  
  return *this;
}

void Asn1Container::add(Asn1ContainerEntry* ace)
{
  entries.push_back(ace);
}

void Asn1Container::remove(Asn1ContainerEntry* ace)
{
  vector<Asn1ContainerEntry*>::iterator __pos = entries.begin();
  vector<Asn1ContainerEntry*>::iterator __end = entries.end();
  while(__pos != __end)
  { 
    if (*__pos == ace)
    {
      entries.erase(__pos);
      return;
    }
    ++__pos;
  }
}  

const Asn1Tag&  Asn1Container::getAsn1Tag() const
{
  return tag;
}

void Asn1Container::setAsn1Tag(const Asn1Tag& tagValue)
{
  this->tag = tagValue;
}

vector<Asn1ContainerEntry*>* Asn1Container::getData()
{
  return &entries;
}

void Asn1Container::releaseEntries()
{    
  vector<Asn1ContainerEntry*>::iterator __pos = entries.begin();
  vector<Asn1ContainerEntry*>::iterator __end = entries.end();

  while(__pos != __end)
  { 

    delete (*__pos);
    ++__pos;
  }
  entries.clear(); 
}

Asn1Group::~Asn1Group()
{
  releaseContainers();
}

Asn1Group::Asn1Group()
{
}
  
Asn1Group::Asn1Group(const Asn1Group& rv)
{
  list<Asn1Container*>::const_iterator __pos = rv.containers.begin();
  list<Asn1Container*>::const_iterator __end = rv.containers.end();

  while(__pos != __end)
  { 

    add(new Asn1Container(*(*__pos)));
    ++__pos;  
     
  }
}

Asn1Group& Asn1Group::operator=(const Asn1Group& rv)
{
  if(&rv == this)
  {
    return *this;
  }  

  releaseContainers();
 
  list<Asn1Container*>::const_iterator __pos = rv.containers.begin();
  list<Asn1Container*>::const_iterator __end = rv.containers.end();

  while(__pos != __end)
  { 

    add(new Asn1Container(*(*__pos)));
    ++__pos;  
     
  }
  
  return *this;
}
  
bool Asn1Group::add(Asn1Container* c)
{
  if( (c != 0) && ( search(c->getAsn1Tag().tag) == 0 ) )
  {
    containers_fast[c->getAsn1Tag().tag] = c;
    containers.push_back(c);
    
    return true;
  }
  
  return false;
}

Asn1Container* Asn1Group::search(const Asn1Tag& tag)
{
  return search(tag.tag);
}

Asn1Container* Asn1Group::search(const std::string& tag)
{
  OPENCODE_MAP<std::string, Asn1Container*>::iterator __pos = containers_fast.find(tag);
  if ( __pos != containers_fast.end() )
  {
    return __pos->second;
  }

  return 0;
}


void Asn1Group::remove(Asn1Container* c)
{
  if(c == 0)
  {
    return;
  }
  
  list<Asn1Container*>::iterator __pos = containers.begin();
  list<Asn1Container*>::iterator __end = containers.end();

  while(__pos != __end)
  { 

    if(*__pos == c)
    {
      containers.erase(__pos);
      break;
    }    
    ++__pos;
    
  }
  
  OPENCODE_MAP<std::string, Asn1Container*>::iterator __pos_fast = containers_fast.find(c->getAsn1Tag().tag);
  if ( __pos_fast != containers_fast.end() )
  {
    containers_fast.erase(__pos_fast);
  }
}

list<Asn1Container*>* Asn1Group::getData()
{
  return &containers;
}


void Asn1Group::releaseContainers()
{
  containers_fast.clear();
    
  list<Asn1Container*>::iterator __pos = containers.begin();
  list<Asn1Container*>::iterator __end = containers.end();
  Asn1Container* __p_ac = 0;
  
  while(__pos != __end)
  { 

    delete (*__pos);
    ++__pos;
    
  }

  containers.clear();
}

void Asn1Group::swap(Asn1Group& __rv)
{
  containers_fast.swap(__rv.containers_fast);
  containers.swap(__rv.containers);
}


//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------

const char Asn1Codec::digitOnes[][9] = {
  "00000000","00000001","00000010","00000011","00000100","00000101","00000110","00000111",
  "00001000","00001001","00001010","00001011","00001100","00001101","00001110","00001111",
  "00010000","00010001","00010010","00010011","00010100","00010101","00010110","00010111",
  "00011000","00011001","00011010","00011011","00011100","00011101","00011110","00011111",
  "00100000","00100001","00100010","00100011","00100100","00100101","00100110","00100111",
  "00101000","00101001","00101010","00101011","00101100","00101101","00101110","00101111",
  "00110000","00110001","00110010","00110011","00110100","00110101","00110110","00110111",
  "00111000","00111001","00111010","00111011","00111100","00111101","00111110","00111111",
  "01000000","01000001","01000010","01000011","01000100","01000101","01000110","01000111",
  "01001000","01001001","01001010","01001011","01001100","01001101","01001110","01001111",
  "01010000","01010001","01010010","01010011","01010100","01010101","01010110","01010111",
  "01011000","01011001","01011010","01011011","01011100","01011101","01011110","01011111",
  "01100000","01100001","01100010","01100011","01100100","01100101","01100110","01100111",
  "01101000","01101001","01101010","01101011","01101100","01101101","01101110","01101111",
  "01110000","01110001","01110010","01110011","01110100","01110101","01110110","01110111",
  "01111000","01111001","01111010","01111011","01111100","01111101","01111110","01111111",
  "10000000","10000001","10000010","10000011","10000100","10000101","10000110","10000111",
  "10001000","10001001","10001010","10001011","10001100","10001101","10001110","10001111",
  "10010000","10010001","10010010","10010011","10010100","10010101","10010110","10010111",
  "10011000","10011001","10011010","10011011","10011100","10011101","10011110","10011111",
  "10100000","10100001","10100010","10100011","10100100","10100101","10100110","10100111",
  "10101000","10101001","10101010","10101011","10101100","10101101","10101110","10101111",
  "10110000","10110001","10110010","10110011","10110100","10110101","10110110","10110111",
  "10111000","10111001","10111010","10111011","10111100","10111101","10111110","10111111",
  "11000000","11000001","11000010","11000011","11000100","11000101","11000110","11000111",
  "11001000","11001001","11001010","11001011","11001100","11001101","11001110","11001111",
  "11010000","11010001","11010010","11010011","11010100","11010101","11010110","11010111",
  "11011000","11011001","11011010","11011011","11011100","11011101","11011110","11011111",
  "11100000","11100001","11100010","11100011","11100100","11100101","11100110","11100111",
  "11101000","11101001","11101010","11101011","11101100","11101101","11101110","11101111",
  "11110000","11110001","11110010","11110011","11110100","11110101","11110110","11110111",
  "11111000","11111001","11111010","11111011","11111100","11111101","11111110","11111111"  
};

XLONG Asn1Codec::decode_hex(char* out,const char* buf,size_t length,size_t offset )
{
  string* __p_string = (string*)(out);
  MemoryKit::encode16(*__p_string,buf,length,offset);
  
  return 0;
}

XLONG Asn1Codec::decode_bcd(char* out,const char* buf,size_t length,size_t offset )
{
  const unsigned char* real_data = (const unsigned char*)(buf + offset); 
  string* __p_string = (string*)(out);

  size_t __digits_len =  length * 2;
  char* __p_newbuf = new char[ __digits_len + 1];
  __p_newbuf[__digits_len] = 0;
  
  
  int __count = 0;
  for(size_t __i = 0 ;__i < length; ++__i)
  {
    __p_newbuf[__count++] = Number::digits[(real_data[__i] >> 4) & 0x0F];
    __p_newbuf[__count++] = Number::digits[real_data[__i] & 0x0F];    
  }
  
  __p_string->assign(__p_newbuf,__count);
  delete []__p_newbuf;   

  return __count;
}


XLONG Asn1Codec::decode_tbcd(char* out,const char* buf,size_t length,size_t offset ) 
{ 
  const unsigned char* real_data = (const unsigned char*)(buf + offset); 
  string* __p_string = (string*)(out);

  size_t __digits_len =  length * 2;
  char* __p_newbuf = new char[ __digits_len + 1];
  __p_newbuf[__digits_len] = 0;
  

  int __count = 0;
  for(size_t __i = 0 ;__i < length; ++__i)
  {
    __p_newbuf[__count++] = Number::digits[real_data[__i] & 0x0F];
    __p_newbuf[__count++] = Number::digits[(real_data[__i] >> 4) & 0x0F];    
  }
  
  if(__count > 0)
  {
    while(__p_newbuf[__count - 1] == 'F')
    {
      __p_newbuf[--__count] = 0;
    }
  }
  
  __p_string->assign(__p_newbuf,__count);
  delete []__p_newbuf;
    
  return 0;  
}

XLONG Asn1Codec::decode_bit(char* out,const char* buf,size_t length,size_t offset )
{
  const unsigned char* real_data = (const unsigned char*)(buf + offset);
  int __pad_cnt = *(real_data++);
  string* __p_string = (string*)(out);
  
  size_t __bits_len =  length * 8;
  char* __p_newbuf = new char[ __bits_len + 1];
  __p_newbuf[__bits_len] = 0;
  
  for(size_t i = 0; i < length ;++i)
  {
    memcpy(__p_newbuf + i*8,digitOnes[*(real_data + i)],8);
  }
  
  __bits_len -= __pad_cnt;    
  __p_string->assign(__p_newbuf,__bits_len);
  delete []__p_newbuf;
  
  return 0;
}

XLONG Asn1Codec::decode_xlong(char* out,const char* buf,size_t length,size_t offset )
{
  const char* real_data = buf + offset;
  XLONG* __p_xlong = (XLONG*)(out);
  XLONG __xlong = *real_data;  
  
  for(size_t i = 1 ; i < length;++i)
  {
    __xlong = __xlong * ASN1_INTEGER_BASE +  (unsigned char)*(real_data + i);
  }
  
  *__p_xlong = __xlong;
  
  return 0;
}

XLONG Asn1Codec::decode_utc(char* out,const char* buf,size_t length,size_t offset )
{
  const char* real_data = buf + offset;
  ASN1_TIMESTAMP* __p_ts = (ASN1_TIMESTAMP*)(out); 
  char __temp[3] = {0};
  
  memcpy(__temp,real_data,2);
  __p_ts->year     = Number::parseInt(__temp);
  
  memcpy(__temp,real_data + 2,2);
  __p_ts->month    = Number::parseInt(__temp);

  memcpy(__temp,real_data + 4,2);  
  __p_ts->day      = Number::parseInt(__temp);

  memcpy(__temp,real_data + 6,2);  
  __p_ts->hour     = Number::parseInt(__temp);

  memcpy(__temp,real_data + 8,2);  
  __p_ts->minute   = Number::parseInt(__temp);

  memcpy(__temp,real_data + 10,2); 
  __p_ts->second   = Number::parseInt(__temp);
  
  __p_ts->tzOffset = 0;  
  if(*(real_data + 12) == '+')
  {
    memcpy(__temp,real_data + 13,2);
    
    char __temp_mm[3] = {0};  
    memcpy(__temp_mm,real_data + 15,2);
      
    __p_ts->tzOffset = Number::parseInt(__temp) * 60 + Number::parseInt(__temp_mm);
  }      
  else if(*(real_data + 12) == '-')
  {
    memcpy(__temp,real_data + 13,2);
    
    char __temp_mm[3] = {0};  
    memcpy(__temp_mm,real_data + 15,2);
      
    __p_ts->tzOffset = - (Number::parseInt(__temp) * 60 + Number::parseInt(__temp_mm));
  }
  
  return 0;
}

XLONG Asn1Codec::decode_timestamp(char* out,const char* buf,size_t length,size_t offset )
{
  const char* real_data = buf + offset;
  ASN1_TIMESTAMP* __p_ts = (ASN1_TIMESTAMP*)(out);
  
  __p_ts->year     = (int)BCD::decodePacked(real_data,1,0);
  __p_ts->month    = (int)BCD::decodePacked(real_data,1,1);
  __p_ts->day      = (int)(int)BCD::decodePacked(real_data,1,2);
  __p_ts->hour     = (int)BCD::decodePacked(real_data,1,3); 
  __p_ts->minute   = (int)BCD::decodePacked(real_data,1,4);
  __p_ts->second   = (int)BCD::decodePacked(real_data,1,5);
  
  __p_ts->tzOffset = 0;
  
  if(*(real_data + 6) == '+')
  {
    __p_ts->tzOffset = (int)(BCD::decodePacked(real_data,1,7) * 60 + BCD::decodePacked(real_data,1,8));
  }      
  else if(*(real_data + 6) == '-')
  {
    __p_ts->tzOffset = -((int)(BCD::decodePacked(real_data,1,7) * 60 + BCD::decodePacked(real_data,1,8)));
  }
  
  return 0;
}

XLONG Asn1Codec::decode_address(char* out,const char* buf,size_t length,size_t offset )
{
  const char* real_data = buf + offset;
  ASN1_ADDRESS* __p_addr = (ASN1_ADDRESS*)(out);
  __p_addr->prefix = *real_data;
  
  string __addr_param;
  decode_tbcd((char*)&__addr_param,buf + 1,length - 1,offset); 
  memcpy(__p_addr->address,__addr_param.data(),__addr_param.size());

  return 0;
}

XLONG Asn1Codec::decode_dot(char* out,const char* buf,size_t length,size_t offset )
{
  const char* real_data = buf + offset;
  unsigned char __seg_len = *real_data;
  
  string* __p_string = (string*)(out);
  
  if(   (('A' <= __seg_len) && (__seg_len <= 'Z')) 
     || (('a' <= __seg_len) && (__seg_len <= 'z')) 
     || (('0' <= __seg_len) && (__seg_len <= '9')))
  {
    __p_string->assign(real_data,length);
  }
  else
  {
    string* __p_new_str = new string(length - 1,0);
    smart_ptr<string> __sp_str(__p_new_str);
    char* __p_new_buf = (char*)__p_new_str->data();
    memcpy(__p_new_buf,real_data + 1,length - 1);
    
    const char* __new_buf_end = __p_new_buf + length - 1;
    __p_new_buf += (__seg_len);
    
    while(real_data < __new_buf_end)
    {
      __seg_len = *__p_new_buf;
      *__p_new_buf = '.';
      __p_new_buf += (1 + __seg_len);
    }
    
    __p_string->swap(*__p_new_str);
  }
  
  return 0;
}
  
//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------

Asn1StackObject::Asn1StackObject()
:__tag(0),__size(0){}

Asn1StackObject::Asn1StackObject(const Asn1StackObject& rv)
{
  this->__tag = rv.__tag;
  this->__size = rv.__size;
}

Asn1StackObject& Asn1StackObject::operator=(const Asn1StackObject& rv)
{
  this->__tag = rv.__tag;
  this->__size = rv.__size;  
  return *this;
}

int Asn1Parser::parse(const char* rawData,size_t size,Asn1Group& asn1Group)
{
  stack<Asn1StackObject> ac_stack;
  return Asn1GroupParser::parse(rawData,size,&asn1Group,&ac_stack);
}  

int Asn1GroupParser::parse(const char* rawData,size_t size,Asn1Group* acl,stack<Asn1StackObject>* ac_stack)
{  
  const unsigned char* __start = (unsigned char*)rawData;
  const unsigned char* __end = __start + size;
  
  int __ret_code = 0; 
  unsigned char __tmpchar = 0;
  const unsigned char* __p_firt_char = 0;
  
  int __len = 0;
  size_t __tag_len = 1;
  int __len_len = 1;
  
  for (const unsigned char* __p = __start; __p < __end;)
  {
    __p_firt_char = __p;
    __tmpchar = *__p_firt_char;    
    
    Asn1Tag __asn1_tag;  
    __asn1_tag.clazz = __tmpchar & ASN1_MASK_CLAZZ;
    __asn1_tag.constructed = ((__tmpchar & ASN1_MASK_CONSTRUCTED) == ASN1_MASK_CONSTRUCTED); //test bit 6    
    __asn1_tag.low_tag_number = __tmpchar & ASN1_MASK_HIGH_TAG_NUMBER; //test bit5-1
    
    __tag_len = 1; 
    if(__asn1_tag.low_tag_number == ASN1_MASK_HIGH_TAG_NUMBER)
    { 
      while(true)
      {
        __tmpchar = *(++__p);
        
        if((__tmpchar >> 7) == 0)
        {
          break;  
                 
        } //~end if((__tmpchar >> 7) == 0)      
           
      } //~end while(true)  
      
      __tag_len = (__p - __p_firt_char) + 1;
    }
    
    MemoryKit::encode16(__asn1_tag.tag,(const char*)__p_firt_char,__tag_len);
     
    __p = __p_firt_char + __tag_len; 
    __len = *__p;
    
    __len_len = 1;           
   
    if(__asn1_tag.constructed && (__len == ASN1_INDEFINITE_LENGTH))
    {
      __asn1_tag.indefinite = true; 
    }
    else if((__len >> 7) != 0)
    {
      __len_len = __len & ASN1_MASK_LONG_LENGTH_NUMBER;      
     
      __len = 0;     
      for(int i = 1; i <= __len_len;++i)
      {
        __len = __len * ASN1_LONG_LENGTH_BASE + *(__p + i);
      }
      __len_len += 1;
    }         
   
    __p = __p_firt_char + __tag_len + __len_len; 

#ifdef DEBUG
    printf("__tag_len = %x,clazz = %x , constructed = %d , low_tag_number = %x , indefinite = %d , tag = %s\n",
            __tag_len,
            __asn1_tag.clazz,
            __asn1_tag.constructed,
            __asn1_tag.low_tag_number,
            __asn1_tag.indefinite,
            __asn1_tag.tag.c_str());

    if(__asn1_tag.indefinite)
    {
      printf("__len_len = %x,__len = <indefinite>\n", __len_len);
    }
    else
    {
      printf("__len_len = %x,__len = %x\n", __len_len,__len);
    }

    if(__asn1_tag.constructed)
    {
      printf("mem = [<constructed>]\n");
    }
    else
    {
      printf("mem = %s\n",MemoryKit::encode16((const char*)__p,__len).c_str());
    }
#endif
    
    bool __created = false;
    Asn1Container* __p_ac = acl->search(__asn1_tag.tag);  
    Asn1ContainerEntry* __p_ace = new Asn1ContainerEntry();
    if(__p_ac == 0)
    {
      __created = true;
      __p_ac = new Asn1Container(__asn1_tag);
    }
    
      
    if(__asn1_tag.constructed)
    {        
      if(__asn1_tag.indefinite)
      {      
        Asn1StackObject __obj;
        __obj.__tag = (Asn1Tag*)&(__p_ac->getAsn1Tag());
        __obj.__size = 0;       
        ac_stack->push(__obj); 
        __ret_code = Asn1GroupParser::parse((const char*)__p,size - (__p - __p_firt_char),&(__p_ace->grp),ac_stack);  
        
        __obj = ac_stack->top();
        __len =  __obj.__size;
        ac_stack->pop();  
      }
      else
      {
        __ret_code = Asn1GroupParser::parse((const char*)__p,__len,&(__p_ace->grp),ac_stack);  
      } //~end if(__asn1_tag.indefinite)       
    }
    else
    {       
      __p_ace->str.assign((const char*)__p,__len);
      __ret_code = 0; 
    }   

    if(__ret_code < 0)
    {
      delete __p_ace;
      if(__created)
      {
        delete __p_ac;
      }
      
      return __ret_code;
    }
    
    __p +=  __len; 
     
    __p_ac->add(__p_ace);
    if(__created)
    {
      acl->add(__p_ac);
    } 
   
    if( (__p + 1) < __end)
    {
      if((*__p) == 0x00 && (*(__p + 1) == 0x00))
      {
        if(ac_stack->size() > 0)
        {
          ac_stack->top().__size = (int)(__p - __start + 2);
          break;
        }
      }  
    }
        
  }//~end for (const char* __p = __start; __p < __end;)
  
  return __ret_code; 
}  


void  Asn1Format::toTEXT(Asn1Group* __pAsn1list,StringBuffer& __sb,int __indent)
{
  list<Asn1Container*>* __avpcData = __pAsn1list->getData();
  list<Asn1Container*>::iterator __pos = __avpcData->begin();
  list<Asn1Container*>::iterator __end = __avpcData->end();  
    
  Asn1Container* __p_ac = 0;
  
  while(__pos != __end)
  { 

    __p_ac = *__pos;
    toTEXT(__p_ac,__sb,__indent);
    ++__pos;
  }
}

void  Asn1Format::toTEXT(Asn1Container* __p_ac,StringBuffer& __sb,int __indent)
{
  string indent;
  if(__indent > 0)
  {
    indent.resize(__indent*2,' ');
  }

  vector<Asn1ContainerEntry*>* __vectorData = __p_ac->getData();
  vector<Asn1ContainerEntry*>::iterator __pos = __vectorData->begin();
  vector<Asn1ContainerEntry*>::iterator __end = __vectorData->end();
  Asn1ContainerEntry* __p_ace = 0;  

  while(__pos != __end)
  {  
   
    __p_ace = *__pos;
    ++__pos;      
    
    if(__p_ac->getAsn1Tag().constructed)
    {
      __sb << indent << "[" << __p_ac->getAsn1Tag().tag << "," << Asn1Tag::extractTAG(__p_ac->getAsn1Tag()) << "]" << File::lineSeparator;
    }
    else
    {
      __sb << indent << "[" << __p_ac->getAsn1Tag().tag << "," << Asn1Tag::extractTAG(__p_ac->getAsn1Tag()) << "]";
    }
    
    toTEXT(__p_ac,__p_ace,__sb,__indent);
  }

}

void  Asn1Format::toTEXT(Asn1Container* __p_ac,Asn1ContainerEntry* __p_ace,StringBuffer& __sb,int __indent)
{
  string indent((__indent+1)*2,' ');
  
  if(__p_ac->getAsn1Tag().constructed)
  {
    toTEXT(&(__p_ace->grp),__sb,__indent + 1); 
  }
  else
  {
    //__sb << indent << "[" << __p_ace->str.size() << "]" << File::lineSeparator;
    //__sb << indent << "[" << MemoryKit::numDump(__p_ace->str.data(),__p_ace->str.size()) << "]" << File::lineSeparator;
    __sb << "-->[" << MemoryKit::numDump(__p_ace->str.data(),__p_ace->str.size()) << "]" << File::lineSeparator;
  }
}

__OPENCODE_END_NAMESPACE

