DEFAULT_INCLUDES               = @opencode_DEFAULT_INCLUDES@
INCLUDES                       = -I$(top_srcdir)/library
AM_LDFLAGS                     = $(top_srcdir)/library/xdbc/libwixdbc.la \
                                 $(top_srcdir)/library/port/libwiport.la \
                                 @opencode_LDFLAGS@
AM_CXXFLAGS                    = @opencode_DEFS@ @opencode_CXXFLAGS@


noinst_PROGRAMS                = SQLSource_demo  DataSource_demo\
                                 xdbc_stmt_demo  xdbc_pstmt_demo  xdbc_proc_demo \
                                 xdbc_arr_demo   xdbc_cursor_demo xdbc_perf_demo 

SQLSource_demo_SOURCES         = SQLSource_demo.cpp     xdbc_demo.cpp
DataSource_demo_SOURCES        = DataSource_demo.cpp    xdbc_demo.cpp 
xdbc_stmt_demo_SOURCES         = xdbc_stmt_demo.cpp     xdbc_demo.cpp
xdbc_pstmt_demo_SOURCES        = xdbc_pstmt_demo.cpp    xdbc_demo.cpp
xdbc_proc_demo_SOURCES         = xdbc_proc_demo.cpp     xdbc_demo.cpp
xdbc_arr_demo_SOURCES          = xdbc_arr_demo.cpp      xdbc_demo.cpp
xdbc_cursor_demo_SOURCES       = xdbc_cursor_demo.cpp   xdbc_demo.cpp
xdbc_perf_demo_SOURCES         = xdbc_perf_demo.cpp     xdbc_demo.cpp

EXTRA_DIST = xdbc_demo.h xdbc.ini odbc.ini xdbc.sqls.ini xdbc.sql run.sh

