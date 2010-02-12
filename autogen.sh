#!/bin/bash
# Run this to generate all the initial makefiles, etc.

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

PKG_NAME="libaute-db"

(test -f $srcdir/configure.ac \
  && test -d $srcdir/src \
  && test -f $srcdir/src/aute_db.c) || {
    echo -n "**Error**: Directory "\`$srcdir\'" does not look like the"
    echo " top-level libaute directory"
    exit 1
}

which gnome-autogen.sh || {
    echo "You need to install gnome-common from GNOME and make"
    echo "sure the gnome-autogen.sh script is in your \$PATH."
    exit 1
}

USE_GNOME2_MACROS=1 . gnome-autogen.sh
