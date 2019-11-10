DEFAULT_INCLUDES         = @opencode_DEFAULT_INCLUDES@
INCLUDES                 = -I$(top_srcdir)/library
AM_LDFLAGS               = $(top_srcdir)/library/socket/libwisocket.la \
                           $(top_srcdir)/library/port/libwiport.la  \
                           @opencode_LDFLAGS@
AM_CXXFLAGS              = @opencode_DEFS@ @opencode_CXXFLAGS@

noinst_PROGRAMS          = network_demo  socket_demo server_demo

network_demo_SOURCES     = network_demo.cpp
socket_demo_SOURCES      = socket_demo.cpp
server_demo_SOURCES      = server_demo.cpp
