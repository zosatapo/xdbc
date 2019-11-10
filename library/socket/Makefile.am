DEFAULT_INCLUDES       =  @opencode_DEFAULT_INCLUDES@   
INCLUDES               =  -I$(top_srcdir)/library
AM_CXXFLAGS            =  @opencode_DEFS@ @opencode_CXXFLAGS@ @opencode_OBJFLAGS@

lib_LTLIBRARIES        =  libwisocket.la
libwisocket_la_LIBADD  =  $(top_srcdir)/library/port/libwiport.la   @opencode_LIBFLAGS@ @opencode_LDFLAGS@
libwisocket_la_SOURCES =  acceptor.cpp\
		                      connector.cpp\
		                      datagramSocket.cpp\
		                      datagramSocketImpl.cpp\
		                      enumInterfaces_v4.cpp\
		                      enumInterfaces_v6.cpp\
		                      inetAddress.cpp\
		                      multicastSocket.cpp\
		                      networkInterface.cpp\
		                      networkUtils.cpp\
		                      networkUtils_win.cpp\
		                      plainDatagramSocketImpl.cpp\
		                      plainSocketImpl.cpp\
		                      serverSocket.cpp\
		                      socketAddress.cpp\
		                      socket.cpp\
		                      socketDescriptor.cpp\
		                      socketException.cpp\
		                      socketImpl.cpp\
		                      socketOptions.cpp\
		                      socketUtils.cpp\
		                      socketUtils_win.cpp

socketincdir              =  $(includedir)/opencode/net
socketinc_HEADERS         =  $(top_srcdir)/library/socket/*.h
