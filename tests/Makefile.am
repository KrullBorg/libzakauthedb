AM_CPPFLAGS = $(WARN_CFLAGS) \
              $(DISABLE_DEPRECATED_CFLAGS) \
              $(LIBZAKAUTHEDB_CFLAGS) \
              $(LIBZAKCONFI_CFLAGS) \
              -I../src

LIBS = $(GTK_LIBS) \
       $(LIBZAKAUTHEDB_LIBS) \
       $(LIBZAKCONFI_LIBS)

LDADD = ../src/libzakauthedb.la

noinst_PROGRAMS = test \
                  test_confi \
                  test_nogui

EXTRA_DIST = test_confi.conf
