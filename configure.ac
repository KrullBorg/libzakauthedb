#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT([libaute-db], [0.3.0], [azagli@libero.it])
AC_CONFIG_SRCDIR([src/aute_db.c])
AC_CONFIG_HEADER([config.h])

AM_INIT_AUTOMAKE(-Wall)

m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

AM_MAINTAINER_MODE

AC_CANONICAL_SYSTEM

AC_LIBTOOL_WIN32_DLL

# Checks for programs.
AC_PROG_CC
AC_PROG_LIBTOOL

# Checks for libraries.
PKG_CHECK_MODULES(LIBAUTEDB, [gtk+-2.0 >= 2.16.0
                              libaute >= 0.1.0
                              libgtkform >= 0.3.0])

AC_SUBST(LIBAUTEDB_CFLAGS)
AC_SUBST(LIBAUTEDB_LIBS)

PKG_CHECK_MODULES(LIBCONFI, [libconfi >= 0.0.2], [AC_DEFINE(HAVE_LIBCONFI, [1], [libconfi is present]), have_libconfi=yes], have_libconfi=no)

AM_CONDITIONAL(HAVE_LIBCONFI, test x"$have_libconfi" = "xyes")
AC_SUBST(LIBCONFI_CFLAGS)
AC_SUBST(LIBCONFI_LIBS)

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
  data/libaute-db/form/Makefile
  data/libaute-db/gui/Makefile
])
AC_OUTPUT
