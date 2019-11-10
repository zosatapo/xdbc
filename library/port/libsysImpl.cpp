#include "port/libsys.h"
#include "port/ioc_sbuf.h"
#include "port/stringTokenizer.h"
#include "port/utilities.h"
#include "port/logging.h"
#include "port/libnumber.h"
#include "port/booleanComponent.h"
#include "port/libproc.h"
#include "port/file.h"
#include "port/lineparser.h" 

#include "port/ioc_time.h" 
#include "port/ioc_fdir.h" 
#include "port/ioc_sysinfo.h"

#include <algorithm>

__OPENCODE_BEGIN_NAMESPACE

/*
 * Mappings from partial locale names to full locale names
 */
 static char* opencode_locale_aliases[] = {
    "ar", "ar_EG",
    "be", "be_BY",
    "bg", "bg_BG",
    "br", "br_FR",
    "ca", "ca_ES",
    "cs", "cs_CZ",
    "cz", "cs_CZ",
    "da", "da_DK",
    "de", "de_DE",
    "el", "el_GR",
    "en", "en_US",
    "eo", "eo",    /* no country for Esperanto */
    "es", "es_ES",
    "et", "et_EE",
    "eu", "eu_ES",
    "fi", "fi_FI",
    "fr", "fr_FR",
    "ga", "ga_IE",
    "gl", "gl_ES",
    "he", "iw_IL",
    "hr", "hr_HR",
#if defined(PORT_LINUX)
    "hs", "en_US", // used on Linux, not clear what it stands for
#endif
    "hu", "hu_HU",
    "id", "in_ID",
    "in", "in_ID",
    "is", "is_IS",
    "it", "it_IT",
    "iw", "iw_IL",
    "ja", "ja_JP",
    "kl", "kl_GL",
    "ko", "ko_KR",
    "lt", "lt_LT",
    "lv", "lv_LV",
    "mk", "mk_MK",
    "nl", "nl_NL",
    "no", "no_NO",
    "pl", "pl_PL",
    "pt", "pt_PT",
    "ro", "ro_RO",
    "ru", "ru_RU",
#if defined(PORT_LINUX)
    "se", "en_US", // used on Linux, not clear what it stands for
#endif
    "sh", "sh_YU",
    "sk", "sk_SK",
    "sl", "sl_SI",
    "sq", "sq_AL",
    "sr", "sr_YU",
    "su", "fi_FI",
    "sv", "sv_SE",
    "th", "th_TH",
    "tr", "tr_TR",
#if defined(PORT_LINUX)
    "ua", "en_US", // used on Linux, not clear what it stands for
#endif
    "uk", "uk_UA",
#if defined(PORT_LINUX)
    "wa", "en_US", // used on Linux, not clear what it stands for
#endif
    "zh", "zh_CN",
#if defined(PORT_LINUX)
    "catalan", "ca_ES",
    "croatian", "hr_HR",
    "czech", "cs_CZ",
    "danish", "da_DK",
    "dansk", "da_DK",
    "deutsch", "de_DE",
    "dutch", "nl_NL",
    "finnish", "fi_FI",
    "french", "fr_FR",
    "german", "de_DE",
    "greek", "el_GR",
    "hebrew", "iw_IL",
    "hrvatski", "hr_HR",
    "hungarian", "hu_HU",
    "icelandic", "is_IS",
    "italian", "it_IT",
    "japanese", "ja_JP",
    "norwegian", "no_NO",
    "polish", "pl_PL",
    "portuguese", "pt_PT",
    "romanian", "ro_RO",
    "russian", "ru_RU",
    "slovak", "sk_SK",
    "slovene", "sl_SI",
    "slovenian", "sl_SI",
    "spanish", "es_ES",
    "swedish", "sv_SE",
    "turkish", "tr_TR",
#else
    "big5", "zh_TW.Big5",
    "chinese", "zh_CN",
    "japanese", "ja_JP",
    "tchinese", "zh_TW",
#endif
    ""
 };

/*
 * Linux/Solaris language string to ISO639 string mapping table.
 */
static char* opencode_language_names[] = {
    "C", "en",
    "POSIX", "en",
    "ar", "ar",
    "be", "be",
    "bg", "bg",
    "br", "br",
    "ca", "ca",
    "cs", "cs",
    "cz", "cs",
    "da", "da",
    "de", "de",
    "el", "el",
    "en", "en",
    "eo", "eo",
    "es", "es",
    "et", "et",
    "eu", "eu",
    "fi", "fi",
    "fo", "fo",
    "fr", "fr",
    "ga", "ga",
    "gl", "gl",
    "hi", "hi",
    "he", "iw",
    "hr", "hr",
#if defined(PORT_LINUX)
    "hs", "en", // used on Linux, not clear what it stands for
#endif
    "hu", "hu",
    "id", "in",
    "in", "in",
    "is", "is",
    "it", "it",
    "iw", "iw",
    "ja", "ja",
    "kl", "kl",
    "ko", "ko",
    "lt", "lt",
    "lv", "lv",
    "mk", "mk",
    "nl", "nl",
    "no", "no",
    "nr", "nr",
    "pl", "pl",
    "pt", "pt",
    "ro", "ro",
    "ru", "ru",
#if defined(PORT_LINUX)
    "se", "en", // used on Linux, not clear what it stands for
#endif
    "sh", "sh",
    "sk", "sk",
    "sl", "sl",
    "sq", "sq",
    "sr", "sr",
    "su", "fi",
    "sv", "sv",
    "th", "th",
    "tr", "tr",
#if defined(PORT_LINUX)
    "ua", "en", // used on Linux, not clear what it stands for
#endif
    "uk", "uk",
#if defined(PORT_LINUX)
    "wa", "en", // used on Linux, not clear what it stands for
#endif
    "zh", "zh",
#if defined(PORT_LINUX)
    "catalan", "ca",
    "croatian", "hr",
    "czech", "cs",
    "danish", "da",
    "dansk", "da",
    "deutsch", "de",
    "dutch", "nl",
    "finnish", "fi",
    "fran\xE7\x61is", "fr",
    "french", "fr",
    "german", "de",
    "greek", "el",
    "hebrew", "iw",
    "hrvatski", "hr",
    "hungarian", "hu",
    "icelandic", "is",
    "italian", "it",
    "japanese", "ja",
    "norwegian", "no",
    "polish", "pl",
    "portuguese", "pt",
    "romanian", "ro",
    "russian", "ru",
    "slovak", "sk",
    "slovene", "sl",
    "slovenian", "sl",
    "spanish", "es",
    "swedish", "sv",
    "turkish", "tr",
#else
    "chinese", "zh",
    "japanese", "ja",
    "korean", "ko",
#endif
    "",
};

/*
 * Linux/Solaris country string to ISO3166 string mapping table.
 */
static char* opencode_country_names[] = {
    "AT", "AT",
    "AU", "AU",
    "AR", "AR",
    "BE", "BE",
    "BR", "BR",
    "BO", "BO",
    "CA", "CA",
    "CH", "CH",
    "CL", "CL",
    "CN", "CN",
    "CO", "CO",
    "CR", "CR",
    "CZ", "CZ",
    "DE", "DE",
    "DK", "DK",
    "DO", "DO",
    "EC", "EC",
    "EE", "EE",
    "ES", "ES",
    "FI", "FI",
    "FO", "FO",
    "FR", "FR",
    "GB", "GB",
    "GR", "GR",
    "GT", "GT",
    "HN", "HN",
    "HR", "HR",
    "HU", "HU",
    "ID", "ID",
    "IE", "IE",
    "IL", "IL",
    "IN", "IN",
    "IS", "IS",
    "IT", "IT",
    "JP", "JP",
    "KR", "KR",
    "LT", "LT",
    "LU", "LU",
    "LV", "LV",
    "MX", "MX",
    "NI", "NI",
    "NL", "NL",
    "NO", "NO",
    "NZ", "NZ",
    "PA", "PA",
    "PE", "PE",
    "PL", "PL",
    "PT", "PT",
    "PY", "PY",
#if defined(PORT_LINUX)
    "RN", "US", // used on Linux, not clear what it stands for
#endif
    "RO", "RO",
    "RU", "RU",
    "SE", "SE",
    "SI", "SI",
    "SK", "SK",
    "SV", "SV",
    "TH", "TH",
    "TR", "TR",
    "UA", "UA",
    "UK", "GB",
    "US", "US",
    "UY", "UY",
    "VE", "VE",
    "TW", "TW",
    "YU", "YU",
    "",
};

/*
 * Linux/Solaris variant string to OPENCODE variant name mapping table.
 */
static char* opencode_variant_names[] = {
#if defined(PORT_LINUX)
    "nynorsk", "NY",
#endif
    "",
};

/**
 * Take an array of string pairs (map of key->value) and a string (key).
 * Examine each pair in the map to see if the first string (key) matches the
 * string.  If so, store the second string of the pair (value) in the value and
 * return 1.  Otherwise do nothing and return 0.  The end of the map is
 * indicated by an empty string at the start of a pair (key of "").
 */
static int opencode_mapLookup(char* map[], const char* key, char** value) 
{
  int i;
  for (i = 0; strcmp(map[i], ""); i += 2)
  {
    if (!strcmp(key, map[i]))
    {
      *value = map[i + 1];
      return 1;
    }
  }
  return 0;
}

IMPLEMENT_LIFETIME_BEGIN(SystemDoImpl)
IMPLEMENT_LIFETIME_END(SystemDoImpl)

SystemDoImpl::~SystemDoImpl() {}
	
SystemDoImpl::SystemDoImpl()
{
  __map_readonlys["os.name"] = "os.name";
  __map_readonlys["os.version"] = "os.version";
  __map_readonlys["file.separator"] = "file.separator";
  __map_readonlys["path.separator"] = "path.separator";
  __map_readonlys["line.separator"] = "line.separator";
  __map_readonlys["user.name"] = "user.name";
  __map_readonlys["user.home"] = "user.home";
  __map_readonlys["user.dir"] = "user.dir";
  __map_readonlys["class.path"] = "class.path";
  __map_readonlys["library.path"] = "library.path";
  __map_readonlys["library.ext"] = "library.ext";
  __map_readonlys["application.cmdline"] = "application.cmdline";


  /** parse process cmdline */
  string cmdline = System::getCommandLine(Process::getCurrentProcessId());
  __props.setProperty("application.cmdline",cmdline); 

  string appcmd;
  string appenv;
  parseCommandLine(appcmd,appenv,cmdline);
    
  /** parse command -D option to system property */
  if(!appenv.empty())
  {
    parseAppicationEnv(appenv);
  }
  
  /** parse and store application.name*/
  string appname = parseAppicationName(appcmd);
  
  /** parse and store application.home */
  string apphome = parseAppicationHome(appcmd); 
    
  /** opencode debug properties */
  initializeLogging();
  
  LOGGING_DEBUG("SystemDoImpl::<init>|set ${application.cmdline} to {"<< cmdline  <<"}");
  LOGGING_DEBUG("SystemDoImpl::<init>|set ${application.name} to {"<< appname<<"}"); 
  LOGGING_DEBUG("SystemDoImpl::<init>|set ${application.home} to {"<< apphome <<"}"); 

  /**
   * Determine the language, country, variant, and encoding from the host,
   * and store these in the user.language, user.country, user.variant system properties.
   */
  initializeLocale(); 
  
  /** Initialize other system properties */  
  initializeOthers();
}

#if !defined(PORT_WINDOWS)

 void SystemDoImpl::initializeLocale()
{
  char *lc;
  lc = setlocale(LC_CTYPE, "");
  if (lc == 0 || !strcmp(lc, "C") || !strcmp(lc, "POSIX")) 
  {
    lc = (char*)"en_US";
  }
  
  {  
    /**
     * locale string format is
     * <language name>_<country name>.<encoding name>@<variant name>
     * <country name>, <encoding name>, and <variant name> are optional.
     */
    char temp[64];
    char* language = 0;
    char* country = 0;
    char* variant = 0;
    char* encoding = 0;
    
    
    char* std_language = 0;
    char* std_country = 0;
    char* std_variant = 0;
    char* std_encoding = 0;
    
    char* p =0;
    char encoding_variant[64];
    
    int i = 0;
    int found = 0;
    
    strcpy(temp, lc);
    
    /* Parse the language, country, encoding, and variant from the
    * locale.  Any of the elements may be missing, but they must occur
    * in the order language_country.encoding@variant, and must be
    * preceded by their delimiter (except for language).
    *
    * If the locale name (without .encoding@variant, if any) matches
    * any of the names in the locale_aliases list, map it to the
    * corresponding full locale name.  Most of the entries in the
    * locale_aliases list are locales that include a language name but
    * no country name, and this facility is used to map each language
    * to a default country if that's possible.  It's also used to map
    * the Solaris locale aliases to their proper locale IDs.
    */
    if ((p = strchr(temp, '.')) != 0) 
    {
      strcpy(encoding_variant, p); /* Copy the leading '.' */
      *p = '\0';
    } 
    else if ((p = strchr(temp, '@')) != 0) 
    {
       strcpy(encoding_variant, p); /* Copy the leading '@' */
       *p = '\0';
    } 
    else 
    {
      *encoding_variant = '\0';
    }
    
    if (opencode_mapLookup(opencode_locale_aliases, temp, &p)) 
    {
      strcpy(temp, p);
    }
    
    language = temp;
    if ((country = strchr(temp, '_')) != 0) 
    {
      *country++ = '\0';
    }
    
    p = encoding_variant;
    if ((encoding = strchr(p, '.')) != 0) 
    {
      p[encoding++ - p] = '\0';
      p = encoding;
    }
    
    if ((variant = strchr(p, '@')) != 0) 
    {
      p[variant++ - p] = '\0';
    }
    
    /* Normalize the language name */
    std_language = (char*)"en";
    if (language != 0) 
    {
      opencode_mapLookup(opencode_language_names, language, &std_language);
    }
   
    if(__props.getProperty("user.language").empty())
    {  
      LOGGING_DEBUG("SystemDoImpl::<init>|set ${user.language} to {"<< std_language <<"}");
      __props.setProperty("user.language",std_language);    
    }

    /* Normalize the country name */
    if (country != 0) 
    {
      std_country = country;
      opencode_mapLookup(opencode_country_names, country, &std_country);
    
      if(__props.getProperty("user.country").empty())
      {  
        LOGGING_DEBUG("SystemDoImpl::<init>|set ${user.country} to {"<< std_country <<"}");
        __props.setProperty("user.country",std_country);
      } 
    }
    
    /* Normalize the variant name.  Note that we only use
    * variants listed in the mapping array; others are ignored. */
    if (variant != 0) 
    {
      opencode_mapLookup(opencode_variant_names, variant, &std_variant);
      if(__props.getProperty("user.variant").empty())
      {
        LOGGING_DEBUG("SystemDoImpl::<init>|set ${user.variant} to {"<< std_variant <<"}");
        __props.setProperty("user.variant",std_variant);
      }
    }
    
    /**
     * Normalize the encoding name.  Note that we IGNORE the string
     * 'encoding' extracted from the locale name above.  Instead, we use the
     * more reliable method of calling nl_langinfo(CODESET).  This function
     * returns an empty string if no encoding is set for the given locale
     * (e.g., the C or POSIX locales); we use the default ISO 8859-1
     * converter for such locales.
     */
    
    /** OK, not so reliable - nl_langinfo() gives wrong answers on Euro locales, in particular. */
    if (strcmp(p, "ISO8859-15") == 0)
    {
      p = (char*)"ISO8859-15";
    }
    else    
    {          
      p = nl_langinfo(CODESET);
    }
    
    /* Convert the bare "646" used on Solaris to a proper IANA name */
    if (strcmp(p, "646") == 0)
    {
      p = (char*)"ISO646-US";
    }
    
    /** return same result nl_langinfo would return for en_UK, in order to use optimizations. */
    std_encoding = (*p != '\0') ? p : (char*)"ISO8859-1";
    
    /** 
     * Remap the encoding string to a different value for japanese
     * locales on linux so that customized converters are used instead
     * of the default converter for "EUC-JP". The customized converters
     * omit support for the JIS0212 encoding which is not supported by
     * the variant of "EUC-JP" encoding used on linux
     */
    if (strcmp(p, "EUC-JP") == 0) 
    {
      std_encoding = (char*)"EUC-JP-LINUX";
    }
   
    if(__props.getProperty("file.encoding").empty())
    { 
      LOGGING_DEBUG("SystemDoImpl::<init>|set ${file.encoding} to {"<< std_encoding <<"}");  
      __props.setProperty("file.encoding",std_encoding);
    }
  }
}

void SystemDoImpl::initializeOthers()
{
  /*hardware properties */
  {
    int processors_conf = 1;
    int processors_onln = 1;

#if defined(PORT_HPUX)
    struct pst_dynamic psd;
    if(pstat_getdynamic(&psd,sizeof(psd),(size_t)1,0) == -1)
    {
      LOGGING_ERROR("SystemDoImpl::<init>|pstat_getdynamic failed : " + System::getLastError());
      processors_conf = 2;
      processors_onln = processors_conf;
    }
    else
    {
      processors_conf = (int)psd.psd_proc_cnt;
      processors_onln = (int)processors_conf;
    } 
#elif defined(PORT_AIX)
    processors_conf = sysconf(_SC_NPROCESSORS_CONF);       
    processors_onln = sysconf(_SC_NPROCESSORS_ONLN);
#elif defined(PORT_SUN_SOLARIS)
    processors_conf = (int)sysconf(_SC_NPROCESSORS_CONF);
    processors_onln = (int)sysconf(_SC_NPROCESSORS_ONLN);
#else
    processors_conf = (int)get_nprocs_conf();
    processors_onln = (int)get_nprocs();
#endif

    __props.setProperty("processors.conf",Number::toString(processors_conf));
    __props.setProperty("processors.onln",Number::toString(processors_onln));
  }

  /* os properties */
  {
    struct utsname name;
    char arch[12];
    uname(&name);    
    __props.setProperty("os.name",name.sysname);
    __props.setProperty("os.version",name.release);
    __props.setProperty("os.tmpdir","/var/tmp");
  }

  /* file system properties */
  {
    __props.setProperty("file.separator",File::separator);
    __props.setProperty("path.separator",File::pathSeparator);
    __props.setProperty("line.separator",File::lineSeparator);
  }  

  /* user properties */
  {
    struct passwd *pwent = getpwuid(getuid());
    __props.setProperty("user.name",pwent->pw_name);
    __props.setProperty("user.home",pwent->pw_dir);
  }

  /* User TIMEZONE */
  {
    //__props.setProperty("user.timezone",""); 
  }

  /* Current directory */
  {
    string curDir = File::getCurrentDirectory();
    if(!curDir.empty())
    {
      __props.setProperty("user.dir",curDir);
    }
  }

  /* binary search property */
  {
    const char* classpath = System::getSystemProperty("PATH");
    if(classpath != 0)
    {
      __props.setProperty("class.path",classpath);
      LOGGING_DEBUG("SystemDoImpl::<init>|set $(class.path) to {"<< classpath <<"}");
    }
  }

  /* library search properties */
  {
    const char* libpath = 0;
    const char* libext  = 0;

//--------------------------------------------------------------------- 
    libpath = System::getSystemProperty("LD_LIBRARY_PATH");
    libext = System::getSystemProperty("LIBRARY_EXT");

    if(libext == 0)
    {
#if defined(PORT_HPUX) && defined(PORT_HPUX_PARISC)
      libext = "sl";
#else
      libext = "so";
#endif
    }  
//---------------------------------------------------------------------
    if(libpath != 0)
    {
      __props.setProperty("library.path",libpath);
      LOGGING_DEBUG("SystemDoImpl::<init>|set $(library.path) to {"<< libpath <<"}");
    }

    __props.setProperty("library.ext",libext);
  }
}

void SystemDoImpl::parseCommandLine(string& appcmd,string& appenv,const string& cmdline)
{
	appenv.clear();
	
	size_t firstpos = cmdline.find_first_of(' ');
	if(firstpos != string::npos)
	{
	  appcmd = cmdline.substr(0,firstpos);
	  if(firstpos < cmdline.size())
	  {
	  	appenv = cmdline.substr(firstpos + 1);
	  }	
	}	
	else
	{
		appcmd = cmdline;		
  }	
}
	
int SystemDoImpl::parseAppicationEnv(const string& appenv)
{
  vector<string> env;
  StringTokenizer::parseTokens(env,appenv," ");
  return parseAppicationEnv(env);   
}

#endif

string SystemDoImpl::parseAppicationName(const string& appcmd)
{
  string __appname = appcmd;
  if(!__appname.empty())
  {
    File __app_file(__appname);
    __appname = __app_file.getName();
    __props.setProperty("application.name",__appname);
  }
  
  return __appname; 
}

string SystemDoImpl::parseAppicationHome(const string& appcmd)
{
  /** set application.home property */
  string apphome = __props.getProperty("application.home");
  bool __user_provided = true;
  
  if(apphome.empty())
  {
	  __user_provided = false;
	  
	  if(appcmd.empty())
	  {
	  	apphome  = File::getCurrentDirectory();  
	  }
	  else
	  {  	
	  	size_t lastpos = appcmd.find_last_of(File::separatorChar);
		  if(lastpos != string::npos)
		  {
		    apphome = appcmd.substr(0,lastpos);
		  }	 
		  else
		  {
		  	apphome = appcmd; 	
		  }
		  
	  }//~end if(appcmd.empty())
	  
	}//~end if(apphome.empty())
	
	{
		File __file1(apphome);
		if(!__file1.isAbsolute())
		{
			File __file2(File::getCurrentDirectory(),apphome);
			apphome = __file2.getCanonicalPath();
	  }	
  }	
  {
    File __file3(apphome);
    if(__file3.isFile())
    {
      apphome = __file3.getParent();
    }
  }

	if(!__user_provided)
	{
	  size_t lastpos = apphome.find_last_of(File::separatorChar);
	  if(lastpos != string::npos)
	  {
	    string __bin_path = apphome.substr(lastpos+1);
	    if(StringsKit::equalsIgnoreCase(__bin_path.c_str(),"bin",3) )
	    {
	      apphome = apphome.substr(0,lastpos);
	    }
	  }//~end if(lastpos != string::npos)
	  
	}//~end if(!__user_provided)	
	  	  
  __props.setProperty("application.home",apphome);
  return apphome;
}

int SystemDoImpl::parseAppicationEnv(const vector<string>& env)
{
  vector<string>::const_iterator __env_pos = env.begin();
  vector<string>::const_iterator __env_end = env.end();
  int count = 0;
  
  while(__env_pos != __env_end)
  {
  	StringTokenizer tokens(*__env_pos,"=");
  	if(tokens.countTokens() == 2)
  	{
  		string key = tokens.nextToken();
  		string value = tokens.nextToken();
  		
  		if(StringsKit::equalsIgnoreCase(key.c_str(),"-D",2)){key = key.substr(2);}	
  	  
  	  if(isReadonly(key)){continue;}
  	  
      __props.setProperty(key,value);        
      count++;
       	  
  	}//~end if(tokens.countTokens() == 2)
  	
  	__env_pos++;

	}//~end while(__env_pos != __env_end)
  
  return count;
}
  
void  SystemDoImpl::initializeLogging()
{
  string value = __props.getProperty(Logging::DEBUG_KEY);
  if(!value.empty())
  {
    Logging::setInternalDebugging(BooleanComponent::valueOf(value));
  }
  else
  {
    StringBuffer __sb_key(Logging::DEBUG_KEY);
    __sb_key.replaceAll(0,".","_");
    string key = StringsKit::toUpperCase(__sb_key.str());
    const char* c_value = System::getSystemProperty(key.c_str());
    if(c_value != 0)
    {
      Logging::setInternalDebugging(BooleanComponent::valueOf(c_value));
    }
  }

  string aFileName = __props.getProperty("logging.filename");
  Logging::setFile(aFileName,true);

  LOGGING_DEBUG("LifeTimeTrack::<init>|startCreateInstance to {" << SystemDoImpl::__getClassName() << "}");
}

string SystemDoImpl::getProperty(const string& key, const string& def)
{
  Mutex::Guard g(__syn_mutex,System::__M_multithreading);
  return __props.getProperty(key,def) ;
}

const string& SystemDoImpl::getProperty(const string& key)
{
  Mutex::Guard g(__syn_mutex,System::__M_multithreading);
  return __props.getProperty(key) ;
}


void  SystemDoImpl::setProperty(const string& key, const string& value)
{
  if(isReadonly(key))
  {
    LOGGING_WARN("SystemDoImpl::setProperty|${" << key << "} is readonly");
    return;
  }

  {
    Mutex::Guard g(__syn_mutex,System::__M_multithreading);
    __props.setProperty(key,value);
  }

  LOGGING_DEBUG("SystemDoImpl::setProperty|set ${" << key << "} to {"<< value <<"}");
}

void  SystemDoImpl::propertyNames(vector<string>& names)
{
  Mutex::Guard g(__syn_mutex,System::__M_multithreading);
  __props.propertyNames(names);
}

bool SystemDoImpl::isReadonly(const string& key)
{
  Mutex::Guard g(__syn_mutex,System::__M_multithreading);
  OPENCODE_MAP<string,string>::iterator __pos = __map_readonlys.find(key);
  if(__pos != __map_readonlys.end())
  {
    return true;
  }
 
  return false;
}

int  SystemDoImpl::setProperties(const Properties& props)
{ 
  vector<string> envnames;
  props.propertyNames(envnames);

  int envsize = (int)envnames.size();
  int count = 0;

  Mutex::Guard g(__syn_mutex,System::__M_multithreading);
    
  for(int i = 0;i < envsize; ++i)
  {
    string key = envnames[i];
    string value = props.getProperty(envnames[i]);    
    __props.setProperty(key,value);
    count++;
  }

  return count;
}
  
__OPENCODE_END_NAMESPACE
