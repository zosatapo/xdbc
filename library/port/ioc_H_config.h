#define OPENCODE_HAS_NAMESPACES

#ifdef HAVE_ALARM
#  define OPENCODE_HAS_ALARM
#endif

#ifdef HAVE_CLOCK_GETTIME
#  define OPENCODE_HAS_CLOCK_GETTIME
#endif

#ifdef HAVE_SETENV
#  define OPENCODE_HAS_SETENV
#endif

#ifdef HAVE_STRCASESTR
#  define OPENCODE_HAS_STRCASESTR
#endif

#ifdef HAVE_STRCASECMP
#  define OPENCODE_HAS_STRCASECMP
#endif

#ifdef HAVE_STRERROR
#  define OPENCODE_HAS_STRERROR
#endif

#ifdef HAVE_STRERROR_R
#  define OPENCODE_HAS_STRERROR_R
#endif

#ifdef HAVE_STRFTIME
#  define OPENCODE_HAS_STRFTIME
#endif

#ifdef HAVE_STRTOF
#  define OPENCODE_HAS_STRTOF
#endif

#ifdef HAVE_STRTOL
#  define OPENCODE_HAS_STRTOL
#endif

#ifdef HAVE_STRTOLL
#  define OPENCODE_HAS_STRTOLL
#endif

#ifdef WORDS_BIGENDIAN
#  define OPENCODE_WORDS_BIGENDIAN
#endif

#ifdef SIZEOF_INT
#  define OPENCODE_SIZEOF_INT    SIZEOF_INT
#endif

#ifdef SIZEOF_LONG
#  define OPENCODE_SIZEOF_LONG   SIZEOF_LONG
#endif


#if defined(_DEBUG) || defined(__DEBUG)
#  ifndef DEBUG
#    define DEBUG
#  endif
#endif


/** 
 * if this is set, then use a 4 byte unicode definition, 
 * instead of the 2 bye that MS use
 */
#ifdef HAVE_TYPE_WCHAR_T
#  define OPENCODE_HAS_TYPE_WCHAR
#  define OPENCODE_SIZEOF_WCHAR  SIZEOF_WCHAR_T
#elif defined(PORT_WINDOWS)
#  define OPENCODE_HAS_TYPE_WCHAR
#  define OPENCODE_SIZEOF_WCHAR  2
#endif

#ifdef HAVE_ATEXIT
#  define OPENCODE_HAS_ATEXIT
#  define OPENCODE_HAS_FIXED_ATEXIT
#elif defined(PORT_WINDOWS)
#  define OPENCODE_HAS_ATEXIT
#  define OPENCODE_HAS_FIXED_ATEXIT
#endif

#define OPENCODE_DISABLE_ATEXIT

