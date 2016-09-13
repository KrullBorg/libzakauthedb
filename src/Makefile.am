guidir = $(datadir)/$(PACKAGE)/gui
formdir = $(datadir)/$(PACKAGE)/form

AM_CPPFLAGS = $(LIBZAKAUTHEDB_CFLAGS) \
              $(LIBGCRYPT_CFLAGS) \
              $(GPG_ERROR_CFLAGS) \
              $(LIBZAKCONFI_CFLAGS) \
              -DGUIDIR=\""$(guidir)"\" \
              -DFORMDIR=\""$(formdir)"\"

libzakauthe_pluginsdir = `pkg-config --variable=pluginsdir libzakauthe`
libzakauthe_plugins_LTLIBRARIES = libzakauthedb.la
libzakauthe_plugins_DATA = zakauthedb.plugin

libzakauthedb_la_SOURCES = aute_db.c \
                        aute_db.h \
                        user.c \
                        user.h

libzakauthedb_la_LDFLAGS = -module -avoid-version -no-undefined
libzakauthedb_la_LIBADD = $(LIBZAKAUTHEDB_LIBS) \
       $(LIBGCRYPT_LIBS) \
       $(GPG_ERROR_LIBS) \
       $(LIBZAKCONFI_LIBS)
