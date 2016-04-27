#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT([libzakauthedb], [0.5.0], [azagli@libero.it])
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
PKG_CHECK_MODULES(LIBZAKAUTHEDB, [libzakauthe >= 0.5.0
								 libgtkform >= 0.5.0])

AC_SUBST(LIBZAKAUTHEDB_CFLAGS)
AC_SUBST(LIBZAKAUTHEDB_LIBS)

PKG_CHECK_MODULES(LIBZAKCONFI, [libzakconfi >= 0.0.2], [AC_DEFINE(HAVE_LIBZAKCONFI, [1], [libzakconfi is present]), have_libzakconfi=yes], have_libzakconfi=no)

AM_CONDITIONAL(HAVE_LIBZAKCONFI, test x"$have_libzakconfi" = "xyes")
AC_SUBST(LIBZAKCONFI_CFLAGS)
AC_SUBST(LIBZAKCONFI_LIBS)

AM_PATH_LIBGCRYPT(1.2.1, :, [AC_MSG_ERROR([libgcrypt >= 1.2.1 not found.])])

AC_SUBST(LIBGCRYPT_CFLAGS)
AC_SUBST(LIBGCRYPT_LIBS)

AM_PATH_GPG_ERROR(1.10, :, [AC_MSG_ERROR([libgpgerror >= 1.10 not found.])])

AC_SUBST(GPG_ERROR_CFLAGS)
AC_SUBST(GPG_ERROR_LIBS)

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
  data/libzakauthedb/Makefile
  data/libzakauthedb/form/Makefile
  data/libzakauthedb/gui/Makefile
])
AC_OUTPUT
