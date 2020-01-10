##
# Global definitions
NULL =
AM_CPPFLAGS =						\
	-DIMSETTINGS_LOCALEDIR="\"$(datadir)/locale\""	\
	-I$(top_srcdir)/imsettings			\
	$(MATE_CFLAGS)					\
	$(NULL)
LIBS =							\
	@LDFLAGS@					\
	$(top_builddir)/imsettings/libimsettings.la	\
	$(MATE_LIBS)					\
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
	libimsettings-mateconf.la			\
	$(NULL)
#
libimsettings_mateconf_la_SOURCES =			\
	mateconf-module.c				\
	$(NULL)
libimsettings_mateconf_la_CFLAGS =				\
	-DG_LOG_DOMAIN="\"IMSettings-MATE-Conf backend\""	\
	$(CFLAGS)						\
	$(NULL)
libimsettings_mateconf_la_LDFLAGS =			\
	-avoid-version					\
	-module						\
	$(LDFLAGS)					\
	$(NULL)
libimsettings_mateconf_la_LIBADD =			\
	$(LDADDS)					\
	$(NULL)

-include $(top_srcdir)/git.mk
