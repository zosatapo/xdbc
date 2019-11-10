#! /bin/sh
# Regenerate the files autoconf / automake

libtoolize --force --automake
autoheader
aclocal -I m4
autoconf
automake -a
