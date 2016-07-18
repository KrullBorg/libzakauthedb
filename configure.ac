#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT([libzakauthedb], [0.5.99], [azagli@libero.it])
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

dnl ******************************
dnl Translations
dnl ******************************
GETTEXT_PACKAGE=zakformtests
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE",
                   [The prefix for our gettext translation domains.])
AM_GLIB_GNU_GETTEXT

# Checks for libraries.
PKG_CHECK_MODULES(LIBAUTEDB, [libzakauthe >= 0.5.0
                              libzakform
                              libzakformgtk
                              libzakformgdaex])

AC_SUBST(LIBAUTEDB_CFLAGS)
AC_SUBST(LIBAUTEDB_LIBS)

PKG_CHECK_MODULES(LIBCONFI, [libconfi >= 0.0.2], [AC_DEFINE(HAVE_LIBCONFI, [1], [libconfi is present]), have_libconfi=yes], have_libconfi=no)

AM_CONDITIONAL(HAVE_LIBCONFI, test x"$have_libconfi" = "xyes")
AC_SUBST(LIBCONFI_CFLAGS)
AC_SUBST(LIBCONFI_LIBS)

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
