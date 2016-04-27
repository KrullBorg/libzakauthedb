guidir = $(datadir)/libzakauthedb/gui
formdir = $(datadir)/libzakauthedb/form

AM_CPPFLAGS = $(LIBZAKAUTHEDB_CFLAGS) \
              $(LIBGCRYPT_CFLAGS) \
              $(GPG_ERROR_CFLAGS) \
              $(LIBZAKCONFI_CFLAGS) \
              -DGUIDIR=\""$(guidir)"\" \
              -DFORMDIR=\""$(formdir)"\"

LIBS = $(LIBZAKAUTHEDB_LIBS) \
       $(LIBGCRYPT_LIBS) \
       $(GPG_ERROR_LIBS) \
       $(LIBZAKCONFI_LIBS)

libzakauthe_pluginsdir = $(libdir)/libzakauthe/plugins
libzakauthe_plugins_LTLIBRARIES = libzakauthedb.la

libzakauthedb_la_SOURCES = aute_db.c \
                        aute_db.h \
                        user.c \
                        user.h

libzakauthedb_la_LDFLAGS = -no-undefined
