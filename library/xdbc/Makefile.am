DEFAULT_INCLUDES     = @opencode_DEFAULT_INCLUDES@
INCLUDES             = -I$(top_srcdir)/library
AM_CXXFLAGS          = @opencode_DEFS@ @opencode_CXXFLAGS@ @opencode_OBJFLAGS@

lib_LTLIBRARIES      =  libwixdbc.la
libwixdbc_la_LIBADD  =  $(top_srcdir)/library/port/libwiport.la   @opencode_LIBFLAGS@ @opencode_LDFLAGS@
libwixdbc_la_SOURCES =  basicDataSource.cpp\
		                    connection.cpp\
		                    dataTable.cpp\
		                    driverManager.cpp\
		                    odbc.cpp\
		                    odbcdfn.cpp\
		                    odbcdfnW.cpp\
		                    odbcArrayStatement.cpp\
		                    odbcBlob.cpp\
		                    odbcCallableStatement.cpp\
		                    odbcClob.cpp\
		                    odbcConnection.cpp\
		                    odbcPreparedStatement.cpp\
		                    odbcResultSet.cpp\
		                    odbcResultSetMetaData.cpp\
		                    odbcStatement.cpp\
		                    odbcTypes.cpp\
		                    pooledConnection.cpp\
		                    pooledPreparedStatement.cpp\
		                    resultset.cpp\
		                    resultsetMetaData.cpp\
		                    SQLSource.cpp\
		                    XDBCUtilities.cpp\
		                    xdefine.cpp\
		                    xtypes.cpp

xdbcincdir           =  $(includedir)/opencode/xdbc
xdbcinc_HEADERS      =  $(top_srcdir)/library/xdbc/*.h
