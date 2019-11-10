DEFAULT_INCLUDES      =  @opencode_DEFAULT_INCLUDES@
INCLUDES              =  -I$(top_srcdir)/library
AM_CXXFLAGS           =  @opencode_DEFS@ @opencode_CXXFLAGS@ @opencode_OBJFLAGS@

lib_LTLIBRARIES       =  libwiport.la
libwiport_la_LIBADD   =  @opencode_LIBFLAGS@ @opencode_LDFLAGS@
libwiport_la_SOURCES  =  asn1parser.cpp\
                				 booleanComponent.cpp\
                				 buddhistCalendar.cpp\
                				 calendar.cpp\
                				 codec.cpp\
                				 condition.cpp\
                				 date.cpp\
                				 file.cpp\
                				 fsysImpl.cpp\
                				 fsysImpl_win.cpp\
                				 gregorianCalendar.cpp\
                				 libdefs.cpp\
                				 libmath.cpp\
                				 libnumber.cpp\
                				 libproc.cpp\
                				 libproc_win.cpp\
                				 librandom.cpp\
                				 libsys.cpp\
                				 libsys_win.cpp\
                				 libsysImpl.cpp\
                				 libsysImpl_win.cpp\
                				 lifetime.cpp\
                				 lineparser.cpp\
                				 locale.cpp\
                				 logging.cpp\
                				 mutex.cpp\
                				 profile.cpp\
                				 properties.cpp\
                				 simpleTimeZone.cpp\
                				 stringTokenizer.cpp\
                				 systemLibrary.cpp\
                				 thread.cpp\
                				 time.cpp\
                				 timestamp.cpp\
                				 timeZone.cpp\
                				 timeZone_win.cpp\
                				 unicode.cpp\
                				 utilities.cpp\
                				 zoneInfo.cpp

portincdir            =  $(includedir)/opencode/port
portinc_HEADERS       =  $(top_srcdir)/library/port/*.h      
