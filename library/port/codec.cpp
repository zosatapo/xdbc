#include "port/codec.h"
#include "port/libmath.h" 
#include "port/ioc_sbuf.h" 
#include "port/ioc_smart.h" 

__OPENCODE_BEGIN_NAMESPACE


UXLONG  BCD::decodeUnpacked(const char* data,size_t length,size_t offset)
{
  UXLONG l = 0;
  const char* real_data = data+offset;

  for(size_t i=0;i<length;++i)
  {
    l = l*10 + real_data[i];
  }
 
  return l;
}

UXLONG  BCD::decodePacked(const char* data,size_t length,size_t offset)
{
  UXLONG l = 0;
  const char* real_data = data+offset;

  for(size_t i=0;i < length; ++i)
  {
    l = l*10 + ((real_data[i]>>4) & 0x0F);
    l = l*10 + (real_data[i] & 0x0F);
  }

  return l;
}

string  BCD::encodeUnpacked(unsigned int i)
{
  double numBits = sizeof(unsigned int) * 8;
  int numDigits  = (int)(numBits / Math::log2(10)) + 1 ;
  
  string ostr(numDigits,'\0');
  char* obuf = (char*) ostr.data();

  size_t obuflength = 0;
  do
  {
    obuf[obuflength++] = i % 10;
    i = i / 10;
  }
  while(i>0);
 
  string o_str(obuflength,'\0');
  for(size_t j = 0;j < obuflength;++j)
  {
    o_str[obuflength - j - 1] = obuf[j];
  } 

  return o_str;
}

string  BCD::encodeUnpacked(UXLONG l)
{
  double numBits = sizeof(UXLONG) * 8;
  int numDigits  = (int)(numBits / Math::log2(10)) + 1 ;

  string ostr(numDigits,'\0');
  char* obuf = (char*) ostr.data();

  int obuflength = 0;
  do
  {
    obuf[obuflength++] = (char)(l % 10);
    l = l / 10;
  }
  while(l>0);

  string o_str(obuflength,'\0');
  for(int i=0;i<obuflength;++i)
  {
    o_str[obuflength - i - 1] = obuf[i];
  }

  return o_str;
}

string BCD::encodePacked(unsigned int i)
{
  string istr = encodeUnpacked(i);
  return unpackedToPacked(istr.data(),0,istr.length());
}

string  BCD::encodePacked(UXLONG l)
{
  string istr = encodeUnpacked(l);
  return unpackedToPacked(istr.data(),0,istr.length());
}

string  BCD::packedToUnpacked(const char* data,size_t length,size_t offset)
{
  const char* real_data = data + offset;
  
  size_t numDigits = length * 2;
  numDigits -= ( ( (real_data[0]>>4 ) == 0 ) ? 0 : 1 );
 
  string ostr(numDigits,'\0');

  if((real_data[0]>>4 ) == 0)
  {
    ostr[0] = real_data[0];
  }

  for(size_t i = 1; i < length; ++i)
  {
    ostr[i] = (real_data[i] >> 4) & 0x0F;
    ostr[i+1] = real_data[i] & 0x0F; 
  } 
 
  return ostr;
}

string BCD::unpackedToPacked(const char* data,size_t length,size_t offset)
{
  const char* real_data = data + offset;

  size_t numDigits = length / 2;
  numDigits += ( (length%2 == 0) ? 0 : 1);

  string ostr(numDigits,'\0');
  
  for(size_t i = length -1; i >= 0;--i)
  {
    ostr[--numDigits] = real_data[i];

    --i;
    if( i < 0 )
    {
      break;
    }  
    
    ostr[numDigits] = ( real_data[i]<<4 ) | ostr[numDigits];
  } 

  return ostr;
}

const char Base64::S_BASE64CHAR[64]={
  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 
  'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 
  'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 
  'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 
  'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 
  'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', 
  '8', '9', '+', '/'
};
const char Base64::S_DECODETABLE[128]={
  127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
  127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
  127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
  127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
  127, 127, 127, 62 , 127, 127, 127, 63 , 52 , 53 ,
  54 , 55 , 56 , 57 , 58 , 59 , 60 , 61 , 127, 127,
  127, 127, 127, 127, 127, 0  , 1  , 2  , 3  , 4  ,
  5  , 6  , 7  , 8  , 9  , 10 , 11 , 12 , 13 , 14 ,
  15 , 16 , 17 , 18 , 19 , 20 , 21 , 22 , 23 , 24 ,
  25 , 127, 127, 127, 127, 127, 127, 26 , 27 , 28 ,
  29 , 30 , 31 , 32 , 33 , 34 , 35 , 36 , 37 , 38 ,
  39 , 40 , 41 , 42 , 43 , 44 , 45 , 46 , 47 , 48 ,
  49 , 50 , 51 , 127, 127, 127, 127, 127     
};

const char  Base64::S_BASE64PAD = '=';

int  Base64::decode0(const char* ibuf,char* obuf,size_t wp)
{
  int outlen = 3;
  if (ibuf[3] == S_BASE64PAD)
  {
    outlen = 2;
  }
  if (ibuf[2] == S_BASE64PAD)
  {
    outlen = 1;
  }
  int b0 = S_DECODETABLE[ibuf[0]];
  int b1 = S_DECODETABLE[ibuf[1]];
  int b2 = S_DECODETABLE[ibuf[2]];
  int b3 = S_DECODETABLE[ibuf[3]];
  switch (outlen)
  {
    case 1 :
    {
      obuf[wp] = (char) (b0 << 2 & 0xfc | b1 >> 4 & 0x3);
      return 1;
    }

    case 2 :
    {
      obuf[wp++] = (char) (b0 << 2 & 0xfc | b1 >> 4 & 0x3);
      obuf[wp] = (char) (b1 << 4 & 0xf0 | b2 >> 2 & 0xf);
      return 2;
    }

    case 3 :
    {
      obuf[wp++] = (char) (b0 << 2 & 0xfc | b1 >> 4 & 0x3);
      obuf[wp++] = (char) (b1 << 4 & 0xf0 | b2 >> 2 & 0xf);
      obuf[wp] = (char) (b2 << 6 & 0xc0 | b3 & 0x3f);
      return 3;
    }
 
    default :
    {
      return outlen;
    }
  }
}

string Base64::decode(const char* data,size_t length,size_t offset)
{
  string i_str(5,'\0');
  char* ibuf = (char*)i_str.data();

  size_t ibuflength = 0;
  
  size_t obuflength=(length / 4 * 3 + 3);
  string o_str(obuflength+1,'\0');
  char* obuf = (char*)o_str.data() ;
   
  size_t obufcount = 0;
  for (size_t i = offset; i < offset + length; i++)
  {
    char ch = data[i];
    if ((ch == S_BASE64PAD) || (S_DECODETABLE[ch] != 127))
    {
      ibuf[ibuflength++] = ch;

      //if (ibuflength == ibuf.length)
      if (ibuflength == 4)
      {
        obufcount += decode0(ibuf, obuf, obufcount);
        ibuflength = 0;
        memset(ibuf,0,4+1);
      }
    }
  }
  
  return string(obuf,obufcount);
}

string Base64::decode(const string& data)
{
  return decode(data.c_str(),0,data.length());
}

string Base64::encode(const char* data, size_t length,size_t offset)
{
  if (length <= 0)
  {
    return string("");
  }
  
  size_t out_size=(length / 3 * 4 + 4);
  string o_str(out_size+1,'\0');
  char* out = (char*) o_str.data();
  
  size_t rindex = offset;
  size_t windex = 0;
  size_t rest = length;
  while (rest >= 3)
  {
    int i =
      ((data[rindex] & 0xff) << 16)
        + ((data[rindex + 1] & 0xff) << 8)
        + (data[rindex + 2] & 0xff);
    out[windex++] = S_BASE64CHAR[i >> 18];
    out[windex++] = S_BASE64CHAR[(i >> 12) & 0x3f];
    out[windex++] = S_BASE64CHAR[(i >> 6) & 0x3f];
    out[windex++] = S_BASE64CHAR[i & 0x3f];
    rindex += 3;
    rest -= 3;
  }
  
  if (rest == 1)
  {
    int i = data[rindex] & 0xff;
    out[windex++] = S_BASE64CHAR[i >> 2];
    out[windex++] = S_BASE64CHAR[(i << 4) & 0x3f];
    out[windex++] = S_BASE64PAD;
    out[windex++] = S_BASE64PAD;
  }
  else if (rest == 2)
  {
    int i = ((data[rindex] & 0xff) << 8) + (data[rindex + 1] & 0xff);
    out[windex++] = S_BASE64CHAR[i >> 10];
    out[windex++] = S_BASE64CHAR[(i >> 4) & 0x3f];
    out[windex++] = S_BASE64CHAR[(i << 2) & 0x3f];
    out[windex++] = S_BASE64PAD;
  }
  
  return string(out);
}

string Base64::encode(const char* data)
{
  return encode(data,0,strlen(data));
}

__OPENCODE_END_NAMESPACE
