/** 
 *  codec.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef __OPENCODE_CODEC_H__
#define __OPENCODE_CODEC_H__

#include "port/libdefs.h"
#include "port/ioc_types.h"  

__OPENCODE_BEGIN_NAMESPACE

class __OPENCODE_CORE_API_DECL BCD 
{
public:
 /**
  * Decode the -UNPACKED- BCD data, The input array is
  * assumed to be in <i>big-endian</i> byte-order: the most significant
  * byte is in the zeroth element.
  *
  * @param data    The BCD encoded data to be decoded
  * @param offset  The offset within the encoded data at which to start decoding
  * @param length  The length of data to decode
  * @return The -UNPACKED- BCD decoded data
  */
  static UXLONG  decodeUnpacked(const char* data,size_t length,size_t offset = 0);

 /**
  * Decode the -PACKED- BCD data, The input array is
  * assumed to be in <i>big-endian</i> byte-order: the most significant
  * byte is in the zeroth element.
  *
  * @param data    The BCD encoded data to be decoded
  * @param offset  The offset within the encoded data at which to start decoding
  * @param length  The length of data to decode
  * @return The -PACKED- BCD decoded data
  */
  static UXLONG  decodePacked(const char* data,size_t length,size_t offset = 0);

  /**
   * Returns -UNPACKED- BCD representation of specified char array.
   * @param data The data to be encoded
   * @param off The offset within the data at which to start encoding
   * @param len The length of the data to encode
   * @return The -UNPACKED- BCD encoded data
   */
  static  string encodeUnpacked(unsigned int i);
  static  string encodeUnpacked(UXLONG l);

  /**
   * Returns -PACKED- BCD representation of specified char array.
   * @param data The data to be encoded
   * @param off The offset within the data at which to start encoding
   * @param len The length of the data to encode
   * @return The -PACKED- BCD encoded data
   */
  static  string encodePacked(unsigned int i);
  static  string encodePacked(UXLONG l);    

public:

  /**
   * Returns -UNPACKED- BCD representation of specified char array.
   * The input array is
   * assumed to be in <i>big-endian</i> byte-order: the most significant
   * byte is in the zeroth element.
   *
   * @param data The data to be encoded
   * @param off The offset within the data at which to start encoding
   * @param len The length of the data to encode
   * @param size The length of the data to return
   * @return The -UNPACKED- BCD encoded data
   */
  static string packedToUnpacked(const char* data,size_t length,size_t offset = 0);

  /**
   * Returns -PACKED- BCD representation of specified char array.
   * The input array is
   * assumed to be in <i>big-endian</i> byte-order: the most significant
   * byte is in the zeroth element.
   *
   * @param data The data to be encoded
   * @param off The offset within the data at which to start encoding
   * @param len The length of the data to encode
   * @return The -PACKED- BCD encoded data
   */  
  static string unpackedToPacked(const char* data,size_t length,size_t offset = 0); 
};

class __OPENCODE_CORE_API_DECL Base64
{
public:
 /**
  * Decode the base64 data
  * @param data    The base64 encoded data to be decoded
  * @param offset  The offset within the encoded data at which to start decoding
  * @param length  The length of data to decode
  * @return The decoded data
  */
  static string decode(const char* data,size_t length,size_t offset = 0); 

  /**
   * Decode the base64 data
   * @param data The base64 encoded data to be decoded
   * @return The decoded data
   */
  static string decode(const string& data);
 
  /**
   * Returns base64 representation of specified char array
   * @param data The data to be encoded
   * @return The base64 encoded data
   */
  static string encode(const char* data);
  
  /**
   * Returns base64 representation of specified char array.
   * @param data The data to be encoded
   * @param offset The offset within the data at which to start encoding
   * @param length The length of the data to encode
   * @return The base64 encoded data
   */
  static string encode(const char* data, size_t length,size_t offset = 0);
  
private:
  static int  decode0(const char* ibuf,char* obuf,size_t wp);

private:
  const static char S_BASE64CHAR[64];
  const static char S_DECODETABLE[128];

  const static char S_BASE64PAD;
    
};

__OPENCODE_END_NAMESPACE

#endif

