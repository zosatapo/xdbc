#include "port/utilities.h"
#include "port/ioc_sbuf.h"
#include "port/ioc_smart.h"
#include "port/libmath.h" 
#include "port/libnumber.h" 
#include "port/libproc.h"
#include "port/libsys.h"
#include "port/file.h" 

__OPENCODE_BEGIN_NAMESPACE

#if !defined(OPENCODE_HAS_STRCASECMP)
/*
 * This array is designed for mapping upper and lower case letter
 * together for a case independent comparison.  The mappings are
 * based upon ascii character sequences.
 */
static const unsigned char __opencode_charmap[] = {
	'\000', '\001', '\002', '\003', '\004', '\005', '\006', '\007',
	'\010', '\011', '\012', '\013', '\014', '\015', '\016', '\017',
	'\020', '\021', '\022', '\023', '\024', '\025', '\026', '\027',
	'\030', '\031', '\032', '\033', '\034', '\035', '\036', '\037',
	'\040', '\041', '\042', '\043', '\044', '\045', '\046', '\047',
	'\050', '\051', '\052', '\053', '\054', '\055', '\056', '\057',
	'\060', '\061', '\062', '\063', '\064', '\065', '\066', '\067',
	'\070', '\071', '\072', '\073', '\074', '\075', '\076', '\077',
	'\100', '\141', '\142', '\143', '\144', '\145', '\146', '\147',
	'\150', '\151', '\152', '\153', '\154', '\155', '\156', '\157',
	'\160', '\161', '\162', '\163', '\164', '\165', '\166', '\167',
	'\170', '\171', '\172', '\133', '\134', '\135', '\136', '\137',
	'\140', '\141', '\142', '\143', '\144', '\145', '\146', '\147',
	'\150', '\151', '\152', '\153', '\154', '\155', '\156', '\157',
	'\160', '\161', '\162', '\163', '\164', '\165', '\166', '\167',
	'\170', '\171', '\172', '\173', '\174', '\175', '\176', '\177',
	'\200', '\201', '\202', '\203', '\204', '\205', '\206', '\207',
	'\210', '\211', '\212', '\213', '\214', '\215', '\216', '\217',
	'\220', '\221', '\222', '\223', '\224', '\225', '\226', '\227',
	'\230', '\231', '\232', '\233', '\234', '\235', '\236', '\237',
	'\240', '\241', '\242', '\243', '\244', '\245', '\246', '\247',
	'\250', '\251', '\252', '\253', '\254', '\255', '\256', '\257',
	'\260', '\261', '\262', '\263', '\264', '\265', '\266', '\267',
	'\270', '\271', '\272', '\273', '\274', '\275', '\276', '\277',
	'\300', '\301', '\302', '\303', '\304', '\305', '\306', '\307',
	'\310', '\311', '\312', '\313', '\314', '\315', '\316', '\317',
	'\320', '\321', '\322', '\323', '\324', '\325', '\326', '\327',
	'\330', '\331', '\332', '\333', '\334', '\335', '\336', '\337',
	'\340', '\341', '\342', '\343', '\344', '\345', '\346', '\347',
	'\350', '\351', '\352', '\353', '\354', '\355', '\356', '\357',
	'\360', '\361', '\362', '\363', '\364', '\365', '\366', '\367',
	'\370', '\371', '\372', '\373', '\374', '\375', '\376', '\377'
};

int strcasecmp(const char* s1, const char* s2)
{
	register const unsigned char* cm  = __opencode_charmap;
	register const unsigned char* us1 = (const unsigned char *)s1;
	register const unsigned char* us2 = (const unsigned char *)s2;

	while (cm[*us1] == cm[*us2++])
	{
		if (*us1++ == '\0')
		{
			return (0);
		}
	}
			
	return (cm[*us1] - cm[*--us2]);
}

int strncasecmp(const char* s1, const char* s2,size_t n)
{
	register const unsigned char* cm  = __opencode_charmap;
	register const unsigned char* us1 = (const unsigned char *)s1;
	register const unsigned char* us2 = (const unsigned char *)s2;
	
	if (n != 0) 
	{
		do 
		{
			if (cm[*us1] != cm[*us2++])
			{
				return (cm[*us1] - cm[*--us2]);
			}
			
			if (*us1++ == '\0')
			{
				break;
			}	
		} while (--n != 0);
	}
	
	return (0);		
}

#endif

bool StringsKit::equalsIgnoreCase(const char* s1, const char* s2)
{
	return (strcasecmp(s1,s2) == 0);
}

bool StringsKit::equalsIgnoreCase(const char* s1, const char* s2,size_t n)
{
	return (strncasecmp(s1,s2,n) == 0);
}

bool StringsKit::equalsIgnoreCase(const string& s1, const string& s2)
{
	size_t __n = __M_max(s1.size(),s2.size());
	return (strncasecmp(s1.c_str(),s2.c_str(),__n) == 0);
}
	
string  StringsKit::toUpperCase(const string& s)
{
	string __sx(s);
	size_t __size = __sx.size();
	char*  __s1 = (char*)__sx.data();
	for(register size_t i = 0; i < __size; ++i)
	{
		__s1[i] = toupper(__s1[i]);
	}
	
	return 	__sx;
}
	
string StringsKit::toLowerCase(const string& s)
{
	string __sx(s);
	size_t __size = __sx.size();
	char*  __s1 = (char*)__sx.data();
	for(register size_t i = 0; i < __size; ++i)
	{
		__s1[i] = tolower(__s1[i]);
	}
	
	return 	__sx;
}

string  StringsKit::trimLeft(const string& s)
{
  string::size_type pos = s.find_first_not_of(' ');
  if (pos == string::npos)
  {
    return string();
  }

  return s.substr(pos);
}

string  StringsKit::trimRight(const string& s)
{
  string::size_type pos = s.find_first_not_of(' ');
  if (pos == string::npos)
  {
    return string();
  }

  string::size_type n = s.find_last_not_of(' ') + 1;
  return s.substr(0, n);
}
  
string StringsKit::trim(const string& s)
{
  string::size_type pos = s.find_first_not_of(' ');
  if (pos == string::npos)
  {
    return string();
  }

  string::size_type n = s.find_last_not_of(' ') - pos + 1;
  return s.substr(pos, n);
}

bool StringsKit::endsWith(const string& s, const string& suffix)
{
  return (s.length() - s.rfind(suffix)) == suffix.length();
}

bool StringsKit::startsWith(const string& s, const string& prefix)
{
  return (s.find(prefix) == 0);
}

string  StringsKit::replace(const string& __s,size_t __pos, size_t __n, const string& __str)
{
  StringBuffer sb(__s);
  return sb.replace(__pos,__n,__str).str();
}

string  StringsKit::replaceAll(const string& __s,size_t __pos,const string& __src, const string& __dst)
{
  StringBuffer sb(__s);
  return sb.replaceAll(__pos,__src,__dst).str();
}

string StringsKit::replaceProperty(const string& val)
{
  StringBuffer sb;
  StringBuffer value(val);
  
  size_t prev = 0;
  size_t pos = 0;
  
  while ((pos = value.indexOf("$", prev)) != string::npos)
  {
    if (pos > 0)
    {
      sb.append(value.substring(prev, pos-prev));
    }
    
    if (pos == (value.size() - 1))
    {
      sb.append('$');
      prev = pos + 1;
    }
    else if (value[pos + 1] != '{')
    {
      sb.append(value[pos + 1]);
      prev = pos + 2; // XXX
    }
    else
    {
      size_t endName = value.indexOf('}', pos);
      if (endName == string::npos)
      {
        sb.append(value.substring(pos));
        prev = value.size();
        continue;
      }
      
      string n = value.substring(pos + 2, endName- (pos + 2)).str();
     
      string value_replaced;
      if(StringsKit::equalsIgnoreCase(n ,"process.id"))
      {
        value_replaced = Number::toString(Process::getCurrentProcessId());
      }
      else
      {
        value_replaced = System::getProperty(n); 
      }

      if(!value_replaced.empty())
      {
        sb.append(value_replaced);
      }
      else
      {
        sb.append("${");
        sb.append(n);
        sb.append("}");
      }
  
      prev = endName + 1;
    }
  }
  
  if (prev < value.size())
  {
    sb.append(value.substring(prev));
  }
  
  return sb.str();
}

#define MEMORYKIT_SEPARATOR ' '
#define MEMORYKIT_PADDING   '0'


bool  BitsKit::testBit(unsigned char* mem,XLONG offset)
{
  return ( mem[offset/8] & (1 << (offset % 8)) ) != 0;
}

unsigned char BitsKit::setBit(unsigned char* mem,XLONG offset)
{
  mem[offset/8] |= (1 << (offset % 8));
  return mem[offset/8];
}

unsigned char BitsKit::clearBit(unsigned char* mem,XLONG offset)
{
  mem[offset/8] &= ~(1 << (offset % 8));
  return mem[offset/8];
}

unsigned char BitsKit::flipBit(unsigned char* mem,XLONG offset)
{
  mem[offset/8] ^= (1 << (offset % 8));
  return mem[offset/8];
}
  
XLONG  MemoryKit::roundUp(XLONG memBytes,XLONG alignBytes)
{
  return ((memBytes + alignBytes - 1) & (~(alignBytes - 1)) );
}

void MemoryKit::memDump(string& out,const char* mem,size_t length,size_t offset )
{
  if(length <= 0 || mem == 0)
  {
    return;
  }
  
  int addr_len     = sizeof(char*)*2 + 2; //10   
  int ascii_length  = 16; //16
  int hex_len      = ascii_length * 2; //32
  int ascii_len    = (int)(ascii_length + strlen(File::lineSeparator)); //17

  char addr_format[30] = {0};
  sprintf(addr_format,"%%0%dxh: ",(addr_len-2)); 
  
  int row_size = addr_len + 1 + hex_len + ascii_length + 2 + ascii_len; //78
  int out_size = (int)(((length/16)+ 1 + (length % 16 ? 1:0)) * row_size);
  
  out.resize(out_size);

  char  c_pend = MEMORYKIT_SEPARATOR; 
  char* __p_out = (char*)out.data();
  memset(__p_out, c_pend, out_size);
  
  const char* __p_stream = mem + offset;    // ptr into source buffer
  
  char* __p_addr  = 0;  // addr ptr into destination buffer
  char* __p_hex   = 0;  // hex ptr into destination buffer
  char* __p_ascii = 0;  // ascii ptr into destination buffer
  
  int rows = 0;

  for(int i=0; i<length; ++i)
  {
    if( (i % 16 ) == 0)
    {       
      __p_addr  = __p_out + (row_size * rows);
      __p_hex   = __p_addr + addr_len + 1;
      __p_ascii = __p_hex + hex_len + ascii_length;
      
      //print address
      sprintf(__p_addr,addr_format,&__p_stream[i]);
 
      //print hex_ascii separator
      *__p_ascii ++ = ';';
      *__p_ascii ++ = c_pend;

      //print line separator   
      strncpy(__p_ascii + ascii_length,File::lineSeparator,strlen(File::lineSeparator));

      ++rows;
    }
    
    //print one byte in hex  
    *__p_hex++ = Number::digits[((__p_stream[i] >> 4 )& 0x0f)];
    *__p_hex++ = Number::digits[(__p_stream[i] & 0x0f)]; 
    
    //skip separator  
    ++__p_hex;
    
    //print one byte in ascii
    *(__p_ascii++) = ((__p_stream[i]<' ' || __p_stream[i]>'~')?'.':__p_stream[i]);   
  }
}

string MemoryKit::memDump(const char* mem,size_t length,size_t offset )
{
  string out;
  MemoryKit::memDump(out,mem,length,offset);
  return out;
}

void MemoryKit::numDump(string& out,const char* mem,size_t length,size_t offset )
{
  if(length <= 0 || mem == 0)
  {
    return;
  }

  size_t outf_size = (length*3 -1) ; 
  out.resize(outf_size);
  
  char* __p_out = (char*)out.data();
  memset(__p_out,MEMORYKIT_PADDING, outf_size);

  size_t pad_size = (length -1);
  for(size_t  i= 0;i < pad_size;++i)
  {
    __p_out[i*3 + 2] = MEMORYKIT_SEPARATOR;
  }
     
  const char* __p_stream = mem;    // ptr into source buffer
  char* p_pad = __p_out + length * 3;

  for(size_t i = 0; i < length; ++i)
  {
    //print one byte in hex  
    *__p_out++ = Number::digits[((__p_stream[i] >> 4 )& 0x0f)];
    *__p_out++ = Number::digits[(__p_stream[i] & 0x0f)]; 

    //skip separator  
    __p_out++;
  }
}

string MemoryKit::numDump(const char* mem,size_t length,size_t offset )
{
  string out;
  MemoryKit::numDump(out,mem,length,offset);
  return out;
}

void    MemoryKit::decode16(string& out,const char* data,size_t length,size_t offset )
{
  size_t outf_size = length / 2; 
  out.resize(outf_size);
  
  char* __p_out = (char*)out.data();
  memset(__p_out,0, outf_size);  

  const char* mem = data + offset;
  char temp[3] = {0};
  
  for(size_t i = 0; i < outf_size;++i)
  {   
    memcpy(temp,mem,2);
    *__p_out++ = Number::parseInt(temp,16);
    mem += 2;
  } 
}  


string  MemoryKit::decode16(const char* data,size_t length,size_t offset )
{
  string  out;
  MemoryKit::decode16(out,data,length,offset);
  return out;
}

void    MemoryKit::encode16(string& out,const char* data,size_t length,size_t offset )
{
  size_t outf_size = length * 2; 
  out.resize(outf_size);
  
  char* __p_out = (char*)out.data();
  memset(__p_out,0, outf_size);  

  const char* mem = data + offset;
  
  for(size_t i = 0; i < length; ++i)
  {
    //print one byte in hex  
    *__p_out++ = Number::digits[((mem[i] >> 4 )& 0x0f)];
    *__p_out++ = Number::digits[(mem[i] & 0x0f)]; 
  }    
}

string  MemoryKit::encode16(const char* data,size_t length,size_t offset )
{
  string  out;
  MemoryKit::encode16(out,data,length,offset);
  return out;  
}
   
__OPENCODE_END_NAMESPACE
