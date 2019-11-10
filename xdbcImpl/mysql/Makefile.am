DEFAULT_INCLUDES                   =  @opencode_DEFAULT_INCLUDES@
INCLUDES                           =  -I$(top_srcdir)/library
AM_CXXFLAGS                        =  @opencode_DEFS@ @opencode_CXXFLAGS@  @opencode_OBJFLAGS@

lib_LTLIBRARIES                    =  libxdbc_mysqlOptions.la
libxdbc_mysqlOptions_la_LIBADD     =  $(top_srcdir)/library/xdbc/libwixdbc.la\
                                      $(top_srcdir)/library/port/libwiport.la\
                                      @opencode_LIBFLAGS@ @opencode_LDFLAGS@
libxdbc_mysqlOptions_la_SOURCES    =  mysqlOptions.cpp
