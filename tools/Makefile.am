AM_CPPFLAGS = $(WARN_CFLAGS) \
              $(DISABLE_DEPRECATED_CFLAGS) \
              $(LIBZAKAUTHEDB_CFLAGS) \
              $(LIBGCRYPT_CFLAGS)

LIBS = $(LIBZAKAUTHEDB_LIBS) \
       $(LIBGCRYPT_LIBS)

noinst_PROGRAMS = zakauthedb-mkpwd

zakauthedb_mkpwd_SOURCES = zakauthedb_mkpwd.c
