ORACLE_SDK_HOME                =  @opencode_ORACLE_SDK_HOME@
DEFAULT_INCLUDES               =  @opencode_DEFAULT_INCLUDES@
INCLUDES                       =  -I$(top_srcdir)/library -I$(ORACLE_SDK_HOME)/include -I$(top_srcdir)/xdbcImpl/oracle
AM_CXXFLAGS                    =  @opencode_DEFS@ @opencode_CXXFLAGS@ @opencode_OBJFLAGS@

lib_LTLIBRARIES                =  libxdbc_oracle_oci.la
libxdbc_oracle_oci_la_LDFLAGS  =  -L$(ORACLE_SDK_HOME)/lib -lclntsh \
                                  $(top_srcdir)/library/xdbc/libwixdbc.la\
                                  $(top_srcdir)/library/port/libwiport.la   @opencode_LIBFLAGS@ @opencode_LDFLAGS@
libxdbc_oracle_oci_la_SOURCES  =  oracleArrayResultSet.cpp\
		                              oracleArrayStatement.cpp\
		                              oracleBlob.cpp\
		                              oracleCallableStatement.cpp\
		                              oracleClob.cpp\
		                              oracleConnection.cpp\
		                              oracleConversion.cpp\
		                              oracle_lib.cpp\
		                              oraclePreparedStatement.cpp\
		                              oracleResultSet.cpp\
		                              oracleResultSetMetaData.cpp\
		                              oracleSQL.cpp\
		                              oracleStatement.cpp\
		                              oracleTypes.cpp\
		                              oradefs.cpp	
                                  
xdbcincdir                     =  $(includedir)/opencode/xdbcImpl/oracle/xdbc
xdbcinc_HEADERS                =  $(top_srcdir)/xdbcImpl/oracle/xdbc/*.h
