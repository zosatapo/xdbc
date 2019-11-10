ACLOCAL           = @ACLOCAL@
ACLOCAL_AMFLAGS   = -I m4
AUTOMAKE_OPTIONS  = foreign

SUBDIRS           = library xdbcImpl examples msvc
EXTRA_DIST        = autogen.sh configure.sh 

dist-hook:
	rm -rf `find $(distdir) -name Makefile`
	rm -rf `find $(distdir) -name .deps`
	rm -rf `find $(distdir) -name _deps`
	rm -rf `find $(distdir) -name .svn`
	rm -rf `find $(distdir) -name _svn`
