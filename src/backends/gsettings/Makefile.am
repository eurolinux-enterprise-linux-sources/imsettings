##
# Global definitions
NULL =
AM_CPPFLAGS =						\
	-DIMSETTINGS_LOCALEDIR="\"$(datadir)/locale\""	\
	-I$(top_srcdir)/imsettings			\
	$(GSETTINGS_CFLAGS)				\
	$(NULL)
LIBS =							\
	@LDFLAGS@					\
	$(top_builddir)/imsettings/libimsettings.la	\
	$(GSETTINGS_LIBS)				\
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
	libimsettings-gsettings.la			\
	$(NULL)
#
libimsettings_gsettings_la_SOURCES =			\
	gsettings-module.c				\
	$(NULL)
libimsettings_gsettings_la_CFLAGS =				\
	-DG_LOG_DOMAIN="\"IMSettings-GSettings backend\""	\
	$(CFLAGS)						\
	$(NULL)
libimsettings_gsettings_la_LDFLAGS =			\
	-avoid-version					\
	-module						\
	$(LDFLAGS)					\
	$(NULL)
libimsettings_gsettings_la_LIBADD =			\
	$(LDADDS)					\
	$(NULL)

-include $(top_srcdir)/git.mk
