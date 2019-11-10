/** 
 *  asn1parser.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef __OPENCODE_ASN1PARSER_H__
#define __OPENCODE_ASN1PARSER_H__

#include "port/libdefs.h"
#include "port/timeZone.h"

#include "port/ioc_sbuf.h"
#include "port/ioc_map.h" 
#include "port/ioc_stack.h"
#include "port/ioc_vector.h"
#include "port/ioc_list.h"

__OPENCODE_BEGIN_NAMESPACE

/**
 * ====Type Tag================number_(decimal)========Tag_number_(hexadecimal)
 * ________INTEGER____________________2_________________________02________________
 * ________BIT_STRING_________________3_________________________03________________
 * ________OCTET_STRING_______________4_________________________04________________
 * ________NULL_______________________5_________________________05________________
 * ________OBJECT IDENTIFIER__________6_________________________06________________
 * ________SEQUENCE and SEQUENCE OF___16________________________10________________
 * ________SET and SET OF_____________17________________________11________________
 * ________PrintableString____________19________________________13________________
 * ________T61String__________________20________________________14________________
 * ________IA5String__________________22________________________16________________
 * ________UTCTime____________________23________________________17________________
 *  
 */
                                       
#define ASN1_DTYPE_INTEGER             (int)0x02
#define ASN1_DTYPE_BITSTRING           (int)0x03
#define ASN1_DTYPE_OCTETSTRING         (int)0x04
#define ASN1_DTYPE_NULL                (int)0x05
#define ASN1_DTYPE_OBJECTID            (int)0x06 
#define ASN1_DTYPE_SEQUENCE            (int)0x10 
#define ASN1_DTYPE_SET                 (int)0x11 
#define ASN1_DTYPE_PRINTABLESTRING     (int)0x13 
#define ASN1_DTYPE_T61STRING           (int)0x14 
#define ASN1_DTYPE_IA5STRING           (int)0x16 
#define ASN1_DTYPE_UTCTIME             (int)0x17 

                                     
#define ASN1_CLAZZ_UNIVERSAL           (int)0x00
#define ASN1_CLAZZ_APPLICATION         (int)0x40
#define ASN1_CLAZZ_CONTEXT             (int)0x80
#define ASN1_CLAZZ_PRIVATE             (int)0xC0 

#define ASN1_MASK_CLAZZ                (int)0xC0
#define ASN1_MASK_HIGH_TAG_NUMBER      (int)0x1F
#define ASN1_MASK_LONG_LENGTH_NUMBER   (int)0x7F
#define ASN1_MASK_CONSTRUCTED          (int)0x20

#define ASN1_INDEFINITE_LENGTH         (int)0x80
#define ASN1_LONG_LENGTH_BASE          (int)0x0100
#define ASN1_HIGH_TAG_NUMBER_BASE      (int)0x80
#define ASN1_INTEGER_BASE              (int)0x0100

class __OPENCODE_CORE_API_DECL Asn1Tag
{
public:
  ~Asn1Tag();
  Asn1Tag();
  Asn1Tag(int clazz,bool  constructed,int low_tag_number,const std::string& tag,bool indefinite = false);

public:
  int   clazz;
  bool  constructed; 
  int   low_tag_number; 
  bool  indefinite; 
  std::string  tag; /** hex tag including clazz,constructed and tag number*/  

public:
  static int extractTAG(const Asn1Tag& tag);
};
 
class Asn1ContainerEntry;
 
/** 
 * Asn1Container Asn1 Container Definition
 *
 * A class used for passing Asn1 data between applications and the API.
 */
class __OPENCODE_CORE_API_DECL Asn1Container
{
public:
  /**
   * destructor
   */
  ~Asn1Container();
  
  /**
   * constructor
   */
  Asn1Container();
  Asn1Container(const Asn1Tag& tag);
  
  Asn1Container(const Asn1Container& rv);
  Asn1Container& operator=(const Asn1Container& rv);
  
public:  
  /**
   * This function adds aOCPAsn1ContainerEntry pointer to the
   * container. 
   *
   * @param ace Asn1ContainerEntry to add
   */
  void add(Asn1ContainerEntry* ace);
  
  /**
   * This function removes an Asn1ContainerEntry pointer from the
   * container. 
   *
   * @param e Asn1 entry to remove
   */
  void remove(Asn1ContainerEntry* ace);
    
  /**
   * Access method to retrive the Asn1 Tag.
   * Returns Tag of the Asn1.
   */
  const Asn1Tag&  getAsn1Tag() const;
  
  /**
   * Access method to set the Asn1 Tag.
   * @param tag New Asn1 Tag
   */
  void setAsn1Tag(const Asn1Tag& tag);

public:
  vector<Asn1ContainerEntry*>* getData();
  
  /**
   * This function returns all the Asn1ContainerEntry pointers in
   * the container to the free list.  It is the responsibility of
   * applications to call this function as soon as processing of the
   * containers are completed. 
   */
  void releaseEntries();    
  
public:
  Asn1Tag   tag;
  vector<Asn1ContainerEntry*> entries;
};

/**
 * Asn1ContainerList Asn1 Container List
 *
 * A class that defines a list of Asn1Container and functions 
 * for manipulating the list.
 */
class  __OPENCODE_CORE_API_DECL Asn1Group
{
public:  
  /** destructor */
  ~Asn1Group();
 
  /** constructor*/
  Asn1Group();
  
  Asn1Group(const Asn1Group& rv);
  Asn1Group& operator=(const Asn1Group& rv);
  
public:   
  /**
   * This function appends the specified container to the internal list.
   *
   * @param c Asn1 Container to add
   */
  bool add(Asn1Container* c);
  
  /**
   * This function searches the internal list for a container
   * corresponding to the specified tag.  
   *
   * @param tag Asn1 tag to search
   */
  Asn1Container* search(const Asn1Tag& tag);   
  Asn1Container* search(const std::string& tag); 
  
  /**
   * This function removes an Asn1Container pointer from the
   * container list. 
   *
   * @param c Asn1 container to remove
   */
  void remove(Asn1Container* c);

public:
  list<Asn1Container*>* getData();

  /**
   * This function returns all the containers in the list to the free
   * list.  It is the responsibility of applications to call
   * this function as soon as processing of the containers are completed.
   */
  void releaseContainers();

  void swap(Asn1Group& __rv);
      
public:
  OPENCODE_MAP<std::string, Asn1Container*> containers_fast;
  list<Asn1Container*>             containers;
};

/**
 * ContainerEntry Container Entry
 *
 * A union structure to store a value of any type of Asn1.
 */
class __OPENCODE_CORE_API_DECL Asn1ContainerEntry
{
public:
  /** Destructor */
  ~Asn1ContainerEntry();

  /** Constuctor. */
  Asn1ContainerEntry();

  Asn1ContainerEntry(const Asn1ContainerEntry& rv);
  Asn1ContainerEntry& operator=(const Asn1ContainerEntry& rv);
  
public:
  std::string  str;
  Asn1Group    grp;
};

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
#define ASN1_ADDRESS_MAX   255

typedef struct __OPENCODE_CORE_API_DECL __asn1_address
{
public:
  __asn1_address()
  :prefix(0)
  {
    memset(address,0,ASN1_ADDRESS_MAX);
  }
  
  bool isExtension() const
  {
    return ((prefix >> 7) == 0);
  }   
  
public:
  unsigned char prefix;
  char address[ASN1_ADDRESS_MAX + 1];  
  
}ASN1_ADDRESS;

typedef struct __OPENCODE_CORE_API_DECL __asn1_timestamp
{  
public:
  __asn1_timestamp()
  :year(1970),month(1),day(1),
  hour(0),minute(0),second(0),
  tzOffset(TimeZone::getDefault()->getRawOffset())
  {}
    
public:
  int   year;
  int   month;
  int   day;
  int   hour;
  int   minute;
  int   second;
  XLONG tzOffset;
}ASN1_TIMESTAMP;

class __OPENCODE_CORE_API_DECL Asn1Codec
{
public:
  static  XLONG decode_hex(char* out,const char* buf,size_t length,size_t offset = 0);
  
  static  XLONG decode_bcd(char* out,const char* buf,size_t length,size_t offset = 0);
  static  XLONG decode_tbcd(char* out,const char* buf,size_t length,size_t offset = 0);
    
  static  XLONG decode_bit(char* out,const char* buf,size_t length,size_t offset = 0);
  static  XLONG decode_xlong(char* out,const char* buf,size_t length,size_t offset = 0);
    
  static  XLONG decode_utc(char* out,const char* buf,size_t length,size_t offset = 0);
  static  XLONG decode_timestamp(char* out,const char* buf,size_t length,size_t offset = 0);
  
  static  XLONG decode_address(char* out,const char* buf,size_t length,size_t offset = 0);
  static  XLONG decode_dot(char* out,const char* buf,size_t length,size_t offset = 0);

public:
  static  const char digitOnes[256][9];  
};

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
class __OPENCODE_CORE_API_DECL Asn1StackObject
{
public:
  Asn1StackObject();

  Asn1StackObject(const Asn1StackObject& rv);
  Asn1StackObject& operator=(const Asn1StackObject& rv);

public:
  Asn1Tag* __tag;
  int      __size;
};


class __OPENCODE_CORE_API_DECL Asn1Parser
{
public:
  static int   parse(const char* rawData,size_t size,Asn1Group& asn1Group);
};

class __OPENCODE_CORE_API_DECL Asn1GroupParser
{
public:
  static int parse(const char* rawData,size_t size,Asn1Group* acl,stack<Asn1StackObject>* ac_stack);

protected:  
  static int parseXXX(const char* rawData,size_t size,Asn1Group* acl,stack<Asn1StackObject>* ac_stack);
};

class __OPENCODE_CORE_API_DECL Asn1Format
{
public:  
  static void  toTEXT(Asn1Group* avpGroup,StringBuffer& sb,int indent = 0);
  static void  toTEXT(Asn1Container* ac,StringBuffer& sb,int indent = 0);
  static void  toTEXT(Asn1Container* ac,Asn1ContainerEntry* ace,StringBuffer& sb,int indent = 0);
};

__OPENCODE_END_NAMESPACE

#endif
