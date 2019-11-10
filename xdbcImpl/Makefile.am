SUBDIRS = mysql
EXTRA_DIST =

if BUILD_XDBC_ORACLE
SUBDIRS += oracle 
else
EXTRA_DIST += oracle
endif

if BUILD_XDBC_INFORMIX
SUBDIRS += informix 
else
EXTRA_DIST += informix 
endif

