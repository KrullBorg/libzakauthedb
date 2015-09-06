guidir = $(datadir)/libzakauthdb/gui
formdir = $(datadir)/libzakauthdb/form

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

libzakauth_pluginsdir = $(libdir)/libzakauth/plugins
libzakauth_plugins_LTLIBRARIES = libzakauthdb.la

libzakauthdb_la_SOURCES = aute_db.c \
                        aute_db.h \
                        user.c \
                        user.h

libzakauthdb_la_LDFLAGS = -no-undefined
