#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT([libaute-db], [0.0.1], [azagli@libero.it])
AC_CONFIG_SRCDIR([src/aute_db.c])
AC_CONFIG_HEADER([config.h])

AM_INIT_AUTOMAKE
AM_MAINTAINER_MODE

AC_CANONICAL_SYSTEM

AC_LIBTOOL_WIN32_DLL

# Checks for programs.
AC_PROG_CC
AC_PROG_LIBTOOL

# Checks for libraries.
PKG_CHECK_MODULES(LIBAUTEDB, [gtk+-2.0 >= 2.8.0
                              libglade-2.0 >= 2.6.0
                              libgdaobj >= 0.0.2
                              libaute >= 0.0.1])

AC_SUBST(LIBAUTEDB_CFLAGS)
AC_SUBST(LIBAUTEDB_LIBS)

AM_PATH_LIBGCRYPT(1.2.1, :, [AC_MSG_ERROR([libgcrypt >= 1.2.1 not found.])])

AC_SUBST(LIBGCRYPT_CFLAGS)
AC_SUBST(LIBGCRYPT_LIBS)

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Checks for library functions.

AC_CONFIG_FILES([
  Makefile
  src/Makefile
  tests/Makefile
  tools/Makefile
  data/Makefile
  data/libaute-db/Makefile
  data/libaute-db/glade/Makefile
])
AC_OUTPUT
