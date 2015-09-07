guidir = $(datadir)/libzakauthedb/gui
formdir = $(datadir)/libzakauthedb/form

AM_CPPFLAGS = $(LIBAUTEDB_CFLAGS) \
              $(LIBGCRYPT_CFLAGS) \
              $(GPG_ERROR_CFLAGS) \
              $(LIBCONFI_CFLAGS) \
              -DGUIDIR=\""$(guidir)"\" \
              -DFORMDIR=\""$(formdir)"\"

LIBS = $(LIBAUTEDB_LIBS) \
       $(LIBGCRYPT_LIBS) \
       $(GPG_ERROR_LIBS) \
       $(LIBCONFI_LIBS)

libzakauthe_pluginsdir = $(libdir)/libzakauthe/plugins
libzakauthe_plugins_LTLIBRARIES = libzakauthedb.la

libzakauthedb_la_SOURCES = aute_db.c \
                        aute_db.h \
                        user.c \
                        user.h

libzakauthedb_la_LDFLAGS = -no-undefined
