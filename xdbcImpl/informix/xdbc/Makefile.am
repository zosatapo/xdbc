INFORMIX_SDK_HOME = @opencode_INFORMIX_SDK_HOME@
ESQLC_EXEC        = @opencode_ESQLC_EXEC@
ESQLC_OPTS        = @opencode_ESQLC_OPTS@

DEFAULT_INCLUDES                =   @opencode_DEFAULT_INCLUDES@
INCLUDES                        =   -I$(INFORMIX_SDK_HOME)/incl -I$(INFORMIX_SDK_HOME)/incl/esql \
                                    -I$(top_srcdir)/library\
                                    -I$(top_srcdir)/xdbcImpl/informix
AM_CXXFLAGS                     =   -DIFX_THREAD -D_REENTRANT -D_H_LOCALEDEF @opencode_DEFS@ @opencode_CXXFLAGS@  @opencode_OBJFLAGS@

EXTRA_DIST                      =   *.ec
ESQLC_FILES                     =   ifxArrayResultSet_ec.cpp\
                                    ifxArrayStatement_ec.cpp\
                                    ifxBlob_ec.cpp\
                                    ifxCallableStatement_ec.cpp\
                                    ifxClob_ec.cpp\
                                    ifxConnection_ec.cpp\
                                    ifxPreparedStatement_ec.cpp\
                                    ifxResultSet_ec.cpp\
                                    ifxStatement_ec.cpp 
                                    
lib_LTLIBRARIES                 =   libxdbc_informix_esql.la
libxdbc_informix_esql_la_LIBADD =   -L$(INFORMIX_SDK_HOME)/lib -L$(INFORMIX_SDK_HOME)/lib/esql \
                                    -ltasf -lm  -lcrypt -ltsql -ltgen -ltos -lifgen  -lifgls -lifsql\
                                    $(INFORMIX_SDK_HOME)/lib/esql/checkapi.o\
                                    $(top_srcdir)/library/xdbc/libwixdbc.la\
                                    $(top_srcdir)/library/port/libwiport.la   @opencode_LIBFLAGS@ @opencode_LDFLAGS@
libxdbc_informix_esql_la_SOURCES =  ifxConversion.cpp\
                                    $(ESQLC_FILES)\
		                                ifxdefs.cpp\
		                                ifx_lib.cpp\
		                                ifxResultSetMetaData.cpp\
		                                ifxSQL.cpp\
		                                ifxTypes.cpp		
                                    
xdbcincdir                      =   $(includedir)/opencode/xdbcImpl/informix/xdbc
xdbcinc_HEADERS                 =   $(top_srcdir)/xdbcImpl/informix/xdbc/*.h

if BUILD_XDBC_INFORMIX
################################################################
ifxArrayResultSet_ec.cpp:ifxArrayResultSet.ec
	$(ESQLC_EXEC) $(ESQLC_OPTS) $?
	mv ifxArrayResultSet.c ifxArrayResultSet_ec.cpp

ifxArrayStatement_ec.cpp:ifxArrayStatement.ec
	$(ESQLC_EXEC) $(ESQLC_OPTS) $?
	mv ifxArrayStatement.c ifxArrayStatement_ec.cpp

ifxBlob_ec.cpp:ifxBlob.ec
	$(ESQLC_EXEC) $(ESQLC_OPTS) $?
	mv ifxBlob.c ifxBlob_ec.cpp

ifxCallableStatement_ec.cpp:ifxCallableStatement.ec
	$(ESQLC_EXEC) $(ESQLC_OPTS) $?
	mv ifxCallableStatement.c ifxCallableStatement_ec.cpp

ifxClob_ec.cpp:ifxClob.ec
	$(ESQLC_EXEC) $(ESQLC_OPTS) $?
	mv ifxClob.c ifxClob_ec.cpp

ifxConnection_ec.cpp:ifxConnection.ec
	$(ESQLC_EXEC) $(ESQLC_OPTS) $?
	mv ifxConnection.c ifxConnection_ec.cpp

ifxPreparedStatement_ec.cpp:ifxPreparedStatement.ec
	$(ESQLC_EXEC) $(ESQLC_OPTS) $?
	mv ifxPreparedStatement.c ifxPreparedStatement_ec.cpp

ifxResultSet_ec.cpp:ifxResultSet.ec
	$(ESQLC_EXEC) $(ESQLC_OPTS) $?
	mv ifxResultSet.c ifxResultSet_ec.cpp

ifxStatement_ec.cpp:ifxStatement.ec
	$(ESQLC_EXEC) $(ESQLC_OPTS) $?
	mv ifxStatement.c ifxStatement_ec.cpp

################################################################
endif

