guidir = $(datadir)/libaute-db/gui
formdir = $(datadir)/libaute-db/form

AM_CPPFLAGS = $(LIBAUTEDB_CFLAGS) \
              $(LIBGCRYPT_CFLAGS) \
              $(LIBCONFI_CFLAGS) \
              -DGUIDIR=\""$(guidir)"\" \
              -DFORMDIR=\""$(formdir)"\"

LIBS = $(LIBAUTEDB_LIBS) \
       $(LIBGCRYPT_LIBS) \
       $(LIBCONFI_LIBS)

libaute_pluginsdir = $(libdir)/libaute/plugins
libaute_plugins_LTLIBRARIES = libaute-db.la

libaute_db_la_SOURCES = aute_db.c \
                        user.c \
                        user.h

libaute_db_la_LDFLAGS = -no-undefined
