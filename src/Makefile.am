gladedir = $(datadir)/libaute-db/glade

AM_CPPFLAGS = $(LIBAUTEDB_CFLAGS) \
              $(LIBGCRYPT_CFLAGS) \
              $(LIBCONFI_CFLAGS) \
              -DGLADEDIR=\""$(gladedir)"\"

LIBS = $(LIBAUTEDB_LIBS) \
       $(LIBGCRYPT_LIBS) \
       $(LIBCONFI_LIBS)

libaute_pluginsdir = $(libdir)/libaute/plugins
libaute_plugins_LTLIBRARIES = libaute-db.la

libaute_db_la_SOURCES = aute_db.c
