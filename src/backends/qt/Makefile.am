##
# Global definitions
NULL =
AM_CPPFLAGS =						\
	-DIMSETTINGS_LOCALEDIR="\"$(datadir)/locale\""	\
	-I$(top_srcdir)/imsettings			\
	$(IMSETTINGS_CFLAGS)				\
	$(GLIB_CFLAGS)					\
	$(NULL)
LIBS =							\
	@LDFLAGS@					\
	$(top_builddir)/imsettings/libimsettings.la	\
	$(GLIB_LIBS)					\
	$(NULL)
EXTRA_DIST =						\
	$(NULL)


##
# Local definitions

##
# Local Rules

##
# Target platform
imsettingsdir = $(IMSETTINGS_MODULEDIR)
imsettings_LTLIBRARIES =				\
	libimsettings-qt.la				\
	$(NULL)
#
libimsettings_qt_la_SOURCES =				\
	qt-module.c					\
	$(NULL)
libimsettings_qt_la_CFLAGS =				\
	-DG_LOG_DOMAIN="\"IMSettings-Qt backend\""	\
	$(CFLAGS)					\
	$(NULL)
libimsettings_qt_la_LDFLAGS =				\
	-avoid-version					\
	-module						\
	$(LDFLAGS)					\
	$(NULL)
libimsettings_qt_la_LIBADD =				\
	$(LDADDS)					\
	$(NULL)

-include $(top_srcdir)/git.mk
