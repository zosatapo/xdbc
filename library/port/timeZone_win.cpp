#include "port/timeZone.h"
#include "port/zoneInfo.h"
#include "port/libsys.h"
#include "port/calendar.h"
#include "port/logging.h"

#include "port/ioc_time.h" 

USING_OPENCODE_STL_NS
__OPENCODE_BEGIN_NAMESPACE

#if defined(PORT_WINDOWS)

#define VALUE_UNKNOWN		0
#define VALUE_KEY		1
#define VALUE_MAPID		2
#define VALUE_GMTOFFSET		3

#define MAX_ZONE_CHAR		128
#define MAX_MAPID_LENGTH	16

/*
 * Gets the current time zone entry in the "Time Zones" registry.
 */
int __syswin_getWinTimeZone(char* winZoneName, char* winMapID)
{
  TIME_ZONE_INFORMATION  tzi;
  
  /** Get the current time zone setting of the platform. */
  if (GetTimeZoneInformation(&tzi) == TIME_ZONE_ID_INVALID)
  {
  	return VALUE_UNKNOWN;
  }
    	
	{
		int sign;
		LONG gmtOffset;
		if (tzi.Bias > 0) 
		{
			gmtOffset = tzi.Bias;
			sign = -1;
		} 
		else 
		{
			gmtOffset = -tzi.Bias;
			sign = 1;
		}
		if (gmtOffset != 0) 
		{
			sprintf(winZoneName, "GMT%c%02d:%02d",((sign >= 0) ? '+' : '-'),gmtOffset / 60,gmtOffset % 60);
		} 
		else
		{
			strcpy(winZoneName, "GMT");
		}
		return VALUE_GMTOFFSET;	
	}	
}

/** Index values for the mapping table.*/
#define TZ_WIN_NAME	0
#define TZ_MAPID	1
#define TZ_REGION	2
#define TZ_OPENCODE_NAME	3

#define TZ_NITEMS	4	/* number of items (fields) */

/** The mapping table file name.*/
#define MAPPINGS_FILE "\\tzmappings"

/*
 * Looks up the mapping table (tzmappings) and returns a OpenCode time
 * zone ID (e.g., "America/Los_Angeles") if found. Otherwise, 0 is
 * returned.
 *
 *  value_type is one of the following values:
 *	VALUE_KEY for exact key matching
 *	VALUE_MAPID for MapID and country-based mapping (this is
 *	required for the old Windows, such as NT 4.0 SP3).
 */
string __syswin_matchOpenCodeTZ(const char* zi_home_dir, int value_type, char* tzName,char* mapID, const char* country)
{
	string __opencode_tz;
	
	char mapFileName[_MAX_PATH + 1];
	strcpy(mapFileName, zi_home_dir);
	strcat(mapFileName, MAPPINGS_FILE);
	FILE *fp;		
	
	if ((fp = fopen(mapFileName, "r")) == 0) 
	{
	  return __opencode_tz;
	}
	
	int IDmatched = 0;
	char* items[TZ_NITEMS];
	int line = 0;
	char lineBuffer[MAX_ZONE_CHAR * 4];
	
	char bestMatch[MAX_ZONE_CHAR];
	bestMatch[0] = '\0';
	
	while (fgets(lineBuffer, sizeof(lineBuffer), fp) != 0) 
	{
		char *start, *idx, *endp;
		int itemIndex = 0;
		
		line++;
		start = idx = lineBuffer;
		endp = &lineBuffer[sizeof(lineBuffer)];
		
		/** Ignore comment and blank lines.*/
		if (*idx == '#' || *idx == '\n') 
		{
			continue;
		}
	
		for (itemIndex = 0; itemIndex < TZ_NITEMS; itemIndex++) 
		{
			items[itemIndex] = start;
			while (*idx && *idx != ':') 
			{
				if (++idx >= endp) 
				{
					fclose(fp);
					return __opencode_tz;
				}
			}
			
			if (*idx == '\0') 
			{
				fclose(fp);
				return __opencode_tz;
			}
			*idx++ = '\0';
			start = idx;
		}
	
		if (*idx != '\n') 
		{
			fclose(fp);
			return __opencode_tz;
		}
	
		if (strcmp(mapID, items[TZ_MAPID]) == 0) 
		{
			IDmatched = 1;
			if (strcmp(items[TZ_WIN_NAME], tzName) == 0) 
			{
				/** Found the time zone in the mapping table.*/
				__opencode_tz = items[TZ_OPENCODE_NAME];
				break;
			}
			
			/** Try to find the most likely time zone.*/
			if (*items[TZ_REGION] == '\0') 
			{
				strncpy(bestMatch, items[TZ_OPENCODE_NAME], MAX_ZONE_CHAR);
			} 
			else if (country != 0 && strcmp(items[TZ_REGION], country) == 0) 
			{
				if (value_type == VALUE_MAPID) 
				{
					__opencode_tz = items[TZ_OPENCODE_NAME];
					break;
				}
				strncpy(bestMatch, items[TZ_OPENCODE_NAME], MAX_ZONE_CHAR);
			}
		} 
		else 
		{
			if (IDmatched == 1) 
			{
				/** No need to look up the mapping table further. */
				break;
			}
		}
	}
	
	fclose(fp);
	return __opencode_tz;
}
	                         
string TimeZone::getSystemTimeZoneID(const string& ziHome, const string& country)
{
	string std_timezone;
	char winZoneName[MAX_ZONE_CHAR];
	char winMapID[MAX_MAPID_LENGTH];
	int  result;
		
	result = __syswin_getWinTimeZone(winZoneName, winMapID);
	
	if (result != VALUE_UNKNOWN) 
	{
		if (result == VALUE_GMTOFFSET) 
		{
		  std_timezone = winZoneName;
		} 
		else 
		{
      std_timezone = __syswin_matchOpenCodeTZ(ziHome.c_str(), result, winZoneName, winMapID, country.c_str());
		}
	}
	
	return std_timezone;
}

string TimeZone::getSystemGMTOffsetID()
{
  return string();
}

#endif

__OPENCODE_END_NAMESPACE  
