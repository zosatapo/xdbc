/** 
 *  zoneInfo.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef __OPENCODE_ZONEINFO_H__
#define __OPENCODE_ZONEINFO_H__

#include "port/libdefs.h"
#include "port/lifetime.h"
#include "port/timeZone.h"
#include "port/simpleTimeZone.h"
#include "port/ioc_map.h" 
#include "port/ioc_vector.h"
  
__OPENCODE_BEGIN_NAMESPACE

class ZoneInfo;

/** 
  * <code>ZoneInfoFileImpl</code> reads Zone information files in the 
  * <zoneinfo.home> directory and provides time zone 
  * information in the form of a {@link ZoneInfo} object. Also, it 
  * reads the ZoneInfoMappings file to obtain time zone IDs information 
  * that is used by the {@link ZoneInfo} class. The directory layout 
  * and data file formats are as follows. 
  * 
  * <p><strong>Directory layout</strong><p> 
  * 
  * All zone data files and ZoneInfoMappings are put under the 
  * <zoneinfo.home> directory. A path name for a given time 
  * zone ID is a concatenation of <zoneinfo.home> and the 
  * time zone ID. (The file separator is replaced with the platform 
  * dependent value. e.g., '\' for Win32.) An example layout will look 
  * like as follows. 
  * <blockquote> 
  * <pre> 
  * <zoneinfo.home>/Africa/Addis_Ababa 
  *                /Africa/Dakar 
  *                /America/Los_Angeles 
  *                /Asia/Singapore 
  *                /EET 
  *                /Europe/Oslo 
  *                /GMT 
  *                /Pacific/Galapagos 
  *                 ... 
  *                /ZoneInfoMappings 
  * </pre> 
  * </blockquote> 
  * 
  * A zone data file has specific information of each zone. 
  * <code>ZoneInfoMappings</code> has global information of zone IDs so 
  * that the information can be obtained without instantiating all time 
  * zones. 
  * 
  * <p><strong>File format</strong><p> 
  * 
  * Two binary-file formats based on a simple Tag-Length-Value format are used 
  * to describe TimeZone information. The generic format of a data file is: 
  * <blockquote> 
  * <pre> 
  *    DataFile { 
  *      uint8_t         magic[7]; 
  *      uint8_t         version; 
  *      data_item       data[]; 
  *    } 
  * </pre> 
  * </blockquote> 
  * where <code>magic</code> is a magic number identifying a file 
  * format, <code>version</code> is the format version number, and 
  * <code>data</code> is one or more <code>data_item</code>s. The 
  * <code>data_item</code> structure is: 
  * <blockquote> 
  * <pre> 
  *    data_item { 
  *      uint8_t        tag; 
  *      uint16_t       length; 
  *      uint8_t        value[length]; 
  *    } 
  * </pre> 
  * </blockquote> 
  * where <code>tag</code> indicates the data type of the item, 
  * <code>length</code> is a byte count of the following 
  * <code>value</code> that is the content of item data. 
  * <p> 
  * All data is stored in the big-endian order. There is no boundary 
  * alignment between date items. 
  * 
  * <p><strong>1. ZoneInfo data file</strong><p> 
  * 
  * Each ZoneInfo data file consists of the following members. 
  * <br> 
  * <blockquote> 
  * <pre> 
  *    ZoneInfoDataFile { 
  *      uint8_t              magic[7]; 
  *      uint8_t              version; 
  *      SET OF<sup>1</sup> { 
  *        transition            transitions<sup>2</sup>; 
  *        offset_table          offsets<sup>2</sup>; 
  *        simpletimezone        stzparams<sup>2</sup>; 
  *        raw_offset            rawoffset; 
  *        dstsaving             dst; 
  *        checksum              crc32; 
  *        gmtoffsetwillchange   gmtflag<sup>2</sup>; 
  *      } 
  *   } 
  *   1: an unordered collection of zero or one occurrences of each item 
  *   2: optional item 
  * </pre> 
  * </blockquote> 
  * <code>magic</code> is a byte-string constant identifying the 
  * ZoneInfo data file.  This field must be <code>"javazi\0"</code> 
  * defined as {@link #JAVAZI_LABEL}. 
  * <p> 
  * <code>version</code> is the version number of the file format. This 
  * will be used for compatibility check. This field must be 
  * <code>0x01</code> in this version. 
  * <p> 
  * <code>transition</code>, <code>offset_table</code> and 
  * <code>simpletimezone</code> have information of time transition 
  * from the past to the future.  Therefore, these structures don't 
  * exist if the zone didn't change zone names and haven't applied DST in 
  * the past, and haven't planned to apply it.  (e.g. Asia/Tokyo zone) 
  * <p> 
  * <code>raw_offset</code>, <code>dstsaving</code> and <code>checksum</code> 
  * exist in every zoneinfo file. They are used by TimeZone.class indirectly. 
 
  * 
  * <p><strong>1.1 <code>transition</code> structure</strong><p><a name="transition"></a> 
  * <blockquote> 
  * <pre> 
  *    transition { 
  *      uint8_t      tag;              // 0x04 : constant 
  *      uint16_t     length;           // byte length of whole values 
  *      int64_t      value[length/8];  // transitions in `int64_t' 
  *    } 
  * </pre> 
  * </blockquote> 
  * See {@link ZoneInfo#transitions ZoneInfo.transitions} about the value. 
  * 
  * <p><strong>1.2 <code>offset_table</code> structure</strong><p> 
  * <blockquote> 
  * <pre> 
  *    offset_table { 
  *      uint8_t      tag;              // 0x05 : constant 
  *      uint16_t     length;           // byte length of whole values 
  *      int32_t      value[length/4];  // offset values in `int' 
  *    } 
  * </pre> 
  * </blockquote> 
  * 
  * <p><strong>1.3 <code>simpletimezone</code> structure</strong><p> 
  * See {@link ZoneInfo#simpleTimeZoneParams ZoneInfo.simpleTimeZoneParams} 
  * about the value. 
  * <blockquote> 
  * <pre> 
  *    simpletimezone { 
  *      uint8_t      tag;              // 0x06 : constant 
  *      uint16_t     length;           // byte length of whole values 
  *      int32_t      value[length/4];  // SimpleTimeZone parameters 
  *    } 
  * </pre> 
  * </blockquote> 
  * See {@link ZoneInfo#offsets ZoneInfo.offsets} about the value. 
  * 
  * <p><strong>1.4 <code>raw_offset</code> structure</strong><p> 
  * <blockquote> 
  * <pre> 
  *    raw_offset { 
  *      uint8_t  tag;              // 0x01 : constant 
  *      uint16_t  length;           // must be 4. 
  *      int32_t  value;            // raw GMT offset [millisecond] 
  *    } 
  * </pre> 
  * </blockquote> 
  * See {@link ZoneInfo#rawOffset ZoneInfo.rawOffset} about the value. 
  * 
  * <p><strong>1.5 <code>dstsaving</code> structure</strong><p> 
  * Value has dstSaving in seconds.  
  * <blockquote> 
  * <pre> 
  *    dstsaving { 
  *      uint8_t  tag;              // 0x02 : constant 
  *      uint16_t  length;           // must be 2. 
  *      s2  value;            // DST save value [second] 
  *    } 
  * </pre> 
  * </blockquote> 
  * See {@link ZoneInfo#dstSavings ZoneInfo.dstSavings} about value. 
  * 
  * <p><strong>1.6 <code>checksum</code> structure</strong><p> 
  * <blockquote> 
  * <pre> 
  *    checksum { 
  *      uint8_t  tag;              // 0x03 : constant 
  *      uint16_t  length;           // must be 4. 
  *      int32_t  value;            // CRC32 value of transitions 
  *    } 
  * </pre> 
  * </blockquote> 
  * See {@link ZoneInfo#checksum ZoneInfo.checksum}. 
  * 
  * <p><strong>1.7 <code>gmtoffsetwillchange</code> structure</strong><p> 
  * This record has a flag value for {@link ZoneInfo#rawOffsetWillChange}. 
  * If this record is not present in a zoneinfo file, 0 is assumed for 
  * the value. 
  * <blockquote> 
  * <pre> 
  *    gmtoffsetwillchange { 
  *      uint8_t      tag;             // 0x07 : constant 
  *      uint16_t      length;          // must be 1. 
  *      uint8_t      value;           // 1: if the GMT raw offset will change 
  *                               // in the future, 0, otherwise. 
  *     } 
  * </pre> 
  * </blockquote> 
  * 
  * 
  * <p><strong>2. ZoneInfoMappings file</strong><p> 
  * 
  * The ZoneInfoMappings file consists of the following members. 
  * <br> 
  * <blockquote> 
  * <pre> 
  *    ZoneInfoMappings { 
  *      uint8_t      magic[7]; 
  *      uint8_t      version; 
  *      SET OF { 
  *        versionName                   version; 
  *        zone_id_table                 zoneIDs; 
  *        raw_offset_table              rawoffsets; 
  *        raw_offset_index_table        rawoffsetindices; 
  *        alias_table                   aliases; 
  *      } 
  *   } 
  * </pre> 
  * </blockquote> 
  * 
  * <code>magic</code> is a byte-string constant which has the file type.  
  * This field must be <code>"opencodezm\0"</code> defined as {@link #OPENCODEZM_LABEL}. 
  * <p> 
  * <code>version</code> is the version number of this file 
  * format. This will be used for compatibility check. This field must 
  * be <code>0x01</code> in this version. 
  * <p> 
  * <code>versionName</code> shows which version of Olson's data has been used  
  * to generate this ZoneInfoMappings. (e.g. <code>tzdata2000g</code>) <br> 
  * This field is for trouble-shooting and isn't usually used in runtime. 
  * <p> 
  * <code>zone_id_table</code, <code>raw_offset_index_table</code> and 
  * <code>alias_table</code> are general information of supported 
  * zones. 
  * 
  * <p><strong>2.1 <code>zone_id_table</code> structure</strong><p> 
  * <br> 
  * <blockquote> 
  * <pre> 
  *    zone_id_table { 
  *      uint8_t      tag;              // 0x40 : constant 
  *      uint16_t      length;           // byte length of whole values 
  *      uint16_t      zone_id_count; 
  *      zone_id value[zone_id_count]; 
  *    } 
  * 
  *    zone_id { 
  *      uint8_t      byte_length;      // byte length of id 
  *      u1      id[byte_length];  // zone name string 
  *    } 
  * </pre> 
  * </blockquote> 
  * 
  * <p><strong>2.2 <code>raw_offset_table</code> structure</strong><p> 
  * <br> 
  * <blockquote> 
  * <pre> 
  *    raw_offset_table { 
  *      uint8_t      tag;              // 0x41 : constant 
  *      uint16_t      length;           // byte length of whole values 
  *      int32_t      value[length/4];  // raw GMT offset in milliseconds 
  *   } 
  * </pre> 
  * </blockquote> 
  * 
  * <p><strong>2.3 <code>raw_offset_index_table</code> structure</strong><p> 
  * <br> 
  * <blockquote> 
  * <pre> 
  *    raw_offset_index_table { 
  *      uint8_t      tag;              // 0x42 : constant 
  *      uint16_t     length;           // byte length of whole values 
  *      uint8_t      value[length]; 
  *    } 
  * </pre> 
  * </blockquote> 
  * 
  * <p><strong>2.4 <code>alias_table</code> structure</strong><p> 
  * <br> 
  * <blockquote> 
  * <pre> 
  *   alias_table { 
  *      uint8_t       tag;              // 0x43 : constant 
  *      uint16_t      length;           // byte length of whole values 
  *      uint16_t      nentries;         // number of id-pairs 
  *      id_pair value[nentries]; 
  *   } 
  * 
  *   id_pair { 
  *      zone_id aliasname; 
  *      zone_id ID; 
  *   } 
  * </pre> 
  * </blockquote> 
  * 
  * <p><strong>2.5 <code>versionName</code> structure</strong><p> 
  * <br> 
  * <blockquote> 
  * <pre> 
  *   versionName { 
  *      uint8_t      tag;              // 0x44 : constant 
  *      uint16_t     length;           // byte length of whole values 
  *      uint8_t      value[length]; 
  *   } 
  * </pre> 
  * </blockquote> 
  * 
  */ 
class __OPENCODE_CORE_API_DECL  ZoneInfoFileImpl
{ 
  DECLARE_LIFETIME_BEGIN(ZoneInfoFileImpl)
  DECLARE_LIFETIME_END(ZoneInfoFileImpl)
  
public:
  ZoneInfoFileImpl();

private:
  ZoneInfoFileImpl(const ZoneInfoFileImpl& rv);
  ZoneInfoFileImpl& operator=(const ZoneInfoFileImpl& rv);
    
public:     
  /** 
   * Converts the given time zone ID to a platform dependent path 
   * name. For example, "America/Los_Angeles" is converted to 
   * "America\Los_Angeles" on Win32. 
   * @return a modified ID replacing '/' with {@link 
   * base.File#separatorChar File.separatorChar} if needed. 
  */ 
   string getFileName(const string& ID);
     
  /** 
   * Gets a ZoneInfo with the given GMT offset. The object 
   * has its ID in the format of GMT{+|-}hh:mm. 
   * 
   * @param originalId the given custom id (before normalized such as "GMT+9") 
   * @param gmtOffset GMT offset <em>in minutes</em> 
   * @return a ZoneInfo constructed with the given GMT offset 
  */ 
  ZoneInfo* getCustomTimeZone(const string& originalId, XLONG gmtOffset); 
     
  /** 
   * @return a ZoneInfo instance created for the specified ID or 
   * null if there is no time zone data file found for the specified 
   * ID. 
  */ 
  ZoneInfo* getZoneInfo(const string& ID);
     
  const vector<string>& getZoneIDs(); 
     
  /** 
   * @return an alias table in HashMap where a key is an alias ID 
   * (e.g., "PST") and its value is a real time zone ID (e.g., 
   * "America/Los_Angeles"). 
  */ 
  const OPENCODE_MAP<string,string>& getZoneAliases();
     
  const vector<int>& getRawOffsetIndices();
   
  const vector<XLONG>& getRawOffsets(); 

private:
  bool loadRawOffsets(const string& buf);
  
  bool loadZoneIDs(const string& buf);
  
  bool loadZoneAliases(const string& buf);
  
  bool loadRawOffsetIndices(const string& buf);
  
private:
  ZoneInfo* createZoneInfo(const string& ID); 

  /** 
   * Reads the specified file under <zoneinfo.home> into a buffer. 
   * @return the buffer, or null if any I/O error occurred. 
  */ 
  bool readZoneInfoFileImpl(const string& fileName,string& buf);

  bool getZoneInfoMappings(string& buf);
  
private:
   vector<string> zoneIDs;
   vector<XLONG> rawOffsets;
   vector<int> rawOffsetIndices; 
   OPENCODE_MAP<string,string> zoneAliases;
            
public:    
      
  /** 
   * The magic number for the ZoneInfo data file format. 
  */ 
  static const  char OPENCODEZI_LABEL[]; 
  static const  int OPENCODEZI_LABEL_LENGTH; 
   
  /** 
   * The ZoneInfo data file formation version number. 
  */ 
  static const  char OPENCODEZI_VERSION; 
   
  /** 
   * Raw offset data item tag. 
  */ 
  static const  char TAG_RawOffset; 
   
  /** 
   * Known last Daylight Saving Time save value data item tag. 
  */ 
  static const  char TAG_LastDSTSaving; 
   
  /** 
   * Checksum data item tag. 
  */ 
  static const  char TAG_CRC32; 
   
  /** 
   * Transition data item tag. 
  */ 
  static const  char TAG_Transition; 
   
  /** 
   * Offset table data item tag. 
  */ 
  static const  char TAG_Offset; 
   
  /** 
   * SimpleTimeZone parameters data item tag. 
  */ 
  static const  char TAG_SimpleTimeZone; 
   
  /** 
   * Raw GMT offset will change in the future. 
  */ 
  static const  char TAG_GMTOffsetWillChange; 
   
  /** 
   * The ZoneInfoMappings file name. 
  */ 
  static const  char OPENCODEZM_FILE_NAME[]; 
   
  /** 
   * The magic number for the ZoneInfoMappings file format. 
  */ 
  static const  char OPENCODEZM_LABEL[]; 
  static const  int OPENCODEZM_LABEL_LENGTH ; 
   
  /** 
   * The ZoneInfoMappings file format version number. 
  */ 
  static const  char OPENCODEZM_VERSION; 
   
  /** 
   * Time zone IDs data item tag. 
  */ 
  static const  char TAG_ZoneIDs; 
   
  /** 
   * Raw GMT offsets table data item tag. 
  */ 
  static const  char TAG_RawOffsets; 
   
  /** 
   * Indices to the raw GMT offset table data item tag. 
  */ 
  static const  char TAG_RawOffsetIndices; 
   
  /** 
   * Time zone aliases table data item tag. 
  */ 
  static const  char TAG_ZoneAliases; 
   
  /** 
   * Olson's zone information version tag. 
  */ 
  static const  char TAG_TZDataVersion; 
};

typedef SingletonHolder<ZoneInfoFileImpl,CreateUsingNew<ZoneInfoFileImpl>,SingletonWithLongevity<ZoneInfoFileImpl>,MultiThread<ZoneInfoFileImpl> > ZoneInfoFile;
__OPENCODE_INLINE unsigned int getLongevity(ZoneInfoFileImpl* impl){return __BASE_LONGEVITY_ZIF;}


/**
 * <code>ZoneInfo</code> is an implementation subclass of {@link
 * base.TimeZone TimeZone} that represents GMT offsets and
 * daylight saving time transitions of a time zone.
 * <p>
 * The daylight saving time transitions are described in the {@link
 * #transitions transitions} table consisting of a chronological
 * sequence of transitions of GMT offset and/or daylight saving time
 * changes. Since all transitions are represented in UTC, in theory,
 * <code>ZoneInfo</code> can be used with any calendar systems except
 * for the {@link #getOffset(int,int,int,int,int,int) getOffset}
 * method that takes Gregorian calendar date fields.
 * <p>
 * This table covers transitions from 1900 until 2037 (as of version
 * 1.4), Before 1900, it assumes that there was no daylight saving
 * time and the <code>getOffset</code> methods always return the
 * {@link #getRawOffset} value. No Local Mean Time is supported. If a
 * specified date is beyond the transition table and this time zone is
 * supposed to observe daylight saving time in 2037, it delegates
 * operations to a {@link base.SimpleTimeZone SimpleTimeZone}
 * object created using the daylight saving time schedule as of 2037.
 * <p>
 * The date items, transitions, GMT offset(s), etc. are read from a database
 * file. See {@link ZoneInfoFile} for details.
 */

class __OPENCODE_CORE_API_DECL  ZoneInfo :public TimeZone 
{
private:
  virtual TimeZone* doClone() const;

public:
  /**
   * Returns a string representation of this time zone.
   * @return the string
   */
  string toString() const ;

  /** 
   * @return a ZoneInfo instance created for the specified ID or 
   * null if there is no time zone data file found for the specified 
   * ID. 
  */ 
  static ZoneInfo* getZoneInfo(const string& ID);
  
public:
  virtual ~ZoneInfo() ;
  
  ZoneInfo(const ZoneInfo& rv);
  ZoneInfo& operator=(const ZoneInfo& rv);
  
public:
  /**
   * A Constructor for CustomID.
   */
  ZoneInfo(const string& ID, XLONG rawOffset) ;

  /**
   * Constructs a ZoneInfo instance.
   *
   * @param ID time zone name
   * @param rawOffset GMT offset in milliseconds
   * @param dstSavings daylight saving value in milliseconds or 0
   * (zero) if this time zone doesn't observe Daylight Saving Time.
   * @param checksum CRC32 value with all transitions table entry
   * values
   * @param transitions transition table
   * @param offsets offset value table
   * @param simpleTimeZoneParams parameter values for constructing
   * SimpleTimeZone
   * @param willGMTOffsetChange the value of willGMTOffsetChange
   */
  ZoneInfo(const string& ID,
     XLONG rawOffset,
     XLONG dstSavings,
     XLONG checksum,
     const vector<XLONG>& transitions,
     const vector<XLONG>& offsets,
     const vector<XLONG>& simpleTimeZoneParams,
     bool willGMTOffsetChange);

  /**
   * Returns the difference in milliseconds between local time and UTC
   * of given time, taking into account both the raw offset and the
   * effect of daylight savings.
   *
   * @param date the milliseconds in UTC
   * @return the milliseconds to add to UTC to get local wall time
   */
  XLONG getOffset(XLONG date) const ;

  XLONG getOffsets(XLONG utc, vector<XLONG>* offsets)const ;

  XLONG getOffsetsByWall(XLONG wall, vector<XLONG>* offsets)const ;


  /**
   * Returns the difference in milliseconds between local time and
   * UTC, taking into account both the raw offset and the effect of
   * daylight savings, for the specified date and time.  This method
   * assumes that the start and end month are distinct.  This method
   * assumes a Gregorian calendar for calculations.
   * <p>
   * <em>Note: In general, clients should use
   * {@link Calendar#ZONE_OFFSET Calendar.get(ZONE_OFFSET)} +
   * {@link Calendar#DST_OFFSET Calendar.get(DST_OFFSET)}
   * instead of calling this method.</em>
   *
   * @param era       The era of the given date. The value must be either
   *                  GregorianCalendar.AD or GregorianCalendar.BC.
   * @param year      The year in the given date.
   * @param month     The month in the given date. Month is 0-based. e.g.,
   *                  0 for January.
   * @param day       The day-in-month of the given date.
   * @param dayOfWeek The day-of-week of the given date.
   * @param millis    The milliseconds in day in <em>standard</em> local time.
   * @return The milliseconds to add to UTC to get local time.
   */
  XLONG getOffset(XLONG era, XLONG year, XLONG month, XLONG day,XLONG dayOfWeek, XLONG milliseconds)const ;

  /**
   * Sets the base time zone offset from GMT. This operation
   * modifies all the transitions of this ZoneInfo object, including
   * historical ones, if applicable.
   *
   * @param offsetMillis the base time zone offset to GMT.
   */
  void setRawOffset(XLONG offsetMillis) ;

  /**
   * Returns the GMT offset of the current date. This GMT offset
   * value is not modified during Daylight Saving Time.
   *
   * @return the GMT offset value in milliseconds to add to UTC time
   * to get local standard time
   */
  XLONG getRawOffset() const ;

  /**
   * Queries if this time zone uses Daylight Saving Time in the last known rule.
   */
  bool useDaylightTime()const ;

  /**
   * Queries if the specified date is in Daylight Saving Time.
   */
  bool inDaylightTime(XLONG date) const ;

  /**
   * Returns the amount of time in milliseconds that the clock is advanced
   * during daylight saving time is in effect in its last daylight saving time rule.
   *
   * @return the number of milliseconds the time is advanced with respect to
   * standard time when daylight saving time is in effect.
   */
  XLONG getDSTSavings() const ;

public:
  /**
   * Returns a SimpleTimeZone object that represents the last
   * known daylight saving time rules.
   *
   * @return a SimpleTimeZone object or null if this time zone
   * doesn't observe DST.
   */
   SimpleTimeZone* getLastRuleInstance() const;

  /**
   * Returns true if this zone has the same raw GMT offset value and
   * transition table as another zone info. If the specified
   * TimeZone object is not a ZoneInfo instance, this method returns
   * true if the specified TimeZone object has the same raw GMT
   * offset value with no daylight saving time.
   *
   * @param rv the ZoneInfo object to be compared with
   * @return true if the given <code>TimeZone</code> has the same
   * GMT offset and transition information, false, otherwise.
   */
  bool hasSameRules(const TimeZone* rv) const;

private:
  XLONG getOffsets(XLONG date, vector<XLONG>* offsets, bool isWall) const;
  
  int getTransitionIndex(XLONG date, bool isWall) const;

  XLONG getLastRawOffset() const;

  /**
   * Returns a SimpleTimeZone object representing the last GMT
   * offset and DST schedule or null if this time zone doesn't
   * observe DST.
   */
 const SimpleTimeZone* getLastRule() const;

private:
  /**
   * The raw GMT offset in milliseconds between this zone and GMT.
   * Negative offsets are to the west of Greenwich.  To obtain local
   * <em>standard</em> time, add the offset to GMT time.
   * 
   */
  XLONG rawOffset;
  
  /**
   * Difference in milliseconds from the original GMT offset in case
   * the raw offset value has been modified by calling {@link
   * #setRawOffset}. The initial value is 0.
   * 
   */
  XLONG rawOffsetDiff ;
  
  /**
   * A CRC32 value of all pairs of transition time (in milliseconds
   * in <code>XLONG</code>) in local time and its GMT offset (in
   * seconds in <code>int</code>) in the chronological order. Byte
   * values of each <code>XLONG</code> and <code>int</code> are taken
   * in the big endian order (i.e., MSB to LSB).
   * 
   */
  XLONG checksum;
  
  /**
   * The amount of time in milliseconds saved during daylight saving
   * time. If <code>useDaylight</code> is false, this value is 0.
   * 
   */
  XLONG  dstSavings;
  
  /**
   * This array describes transitions of GMT offsets of this time
   * zone, including both raw offset changes and daylight saving
   * time changes.
   * A XLONG integer consists of four bit fields.
   * <ul>
   * <li>The most significant 52-bit field represents transition
   * time in milliseconds from Gregorian January 1 1970, 00:00:00
   * GMT.</li>
   * <li>The next 4-bit field is reserved and must be 0.</li>
   * <li>The next 4-bit field is an index value to {@link #offsets
   * offsets[]} for the amount of daylight saving at the
   * transition. If this value is zero, it means that no daylight
   * saving, not the index value zero.</li>
   * <li>The least significant 4-bit field is an index value to
   * {@link #offsets offsets[]} for <em>total</em> GMT offset at the
   * transition.</li>
   * </ul>
   * If this time zone doesn't observe daylight saving time and has
   * never changed any GMT offsets in the past, this value is null.
   * 
   */
  vector<XLONG> transitions;
  
  /**
   * This array holds all unique offset values in
   * milliseconds. Index values to this array are stored in the
   * transitions array elements.
   * 
   */
  vector<XLONG> offsets;
 
  /**
   * SimpleTimeZone parameter values. It has to have either 8 for
   * {@link base.SimpleTimeZone#SimpleTimeZone(XLONG, string,
   * int, int , int , int , int , int , int , int , XLONG) the
   * 11-argument SimpleTimeZone constructor} or 10 for {@link
   * base.SimpleTimeZone#SimpleTimeZone(XLONG,string, int, int,
   * int , int , int , int , int , int , int, int, XLONG) the
   * 13-argument SimpleTimeZone constructor} parameters.
   * 
   */
  vector<XLONG> simpleTimeZoneParams;
  
  /**
   * True if the raw GMT offset value would change after the time
   * zone data has been generated; false, otherwise. The default
   * value is false.
   * 
   */
  bool willGMTOffsetChange;
  
  mutable SimpleTimeZone* lastRule;

private:
  static const XLONG OFFSET_MASK;
  static const XLONG DST_MASK;
  static const XLONG DST_NSHIFT;
  // this bit field is reserved for abbreviation support
  static const XLONG ABBR_MASK;
  static const XLONG TRANSITION_NSHIFT;
};

__OPENCODE_END_NAMESPACE

#endif
