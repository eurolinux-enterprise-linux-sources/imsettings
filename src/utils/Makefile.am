##
# Global definitions
NULL =
AM_CPPFLAGS =						\
	-DIMSETTINGS_LOCALEDIR="\"$(datadir)/locale\""	\
	$(IMSETTINGS_CFLAGS)				\
	-DXINPUT_SUFFIX="\"$(XINPUT_SUFFIX)\""		\
	-I$(top_srcdir)/imsettings			\
	-I$(top_srcdir)					\
	$(NULL)
LIBS =							\
	@LDFLAGS@					\
	$(IMSETTINGS_LIBS)				\
	$(top_builddir)/imsettings/libimsettings.la	\
	$(NULL)
DEPS =							\
	$(top_builddir)/imsettings/libimsettings.la	\
	$(NULL)
EXTRA_DIST =						\
	$(autostart_in_files)				\
	$(man_xml_files)				\
	$(man1_MANS)					\
	$(NULL)
DISTCLEANFILES =					\
	$(autostart_DATA)				\
	$(NULL)
BUILT_SOURCES =						\
	$(man1_MANS)					\
	$(NULL)
MAINTAINERCLEANFILES =					\
	$(man1_MANS)					\
	$(NULL)
SUFFIXES = .1.xml

##
# Local Rules
@INTLTOOL_DESKTOP_RULE@

##
# Target platform
bin_PROGRAMS =						\
	imsettings-info					\
	imsettings-list					\
	imsettings-reload				\
	imsettings-switch				\
	$(NULL)
libexec_PROGRAMS =					\
	imsettings-check				\
	$(NULL)
man1_MANS =						\
	imsettings-info.1				\
	imsettings-list.1				\
	imsettings-reload.1				\
	imsettings-switch.1				\
	$(NULL)
#
autostartdir = $(sysconfdir)/xdg/autostart
#
autostart_in_files =					\
	imsettings-start.desktop.in			\
	$(NULL)
autostart_DATA = $(autostart_in_files:.desktop.in=.desktop)
#
man_xml_files = $(man1_MANS:.1=.1.xml)
#
.xml.1:
	$(AM_V_GEN) $(DB2MAN) $<
.1.xml.1:
	$(AM_V_GEN) $(DB2MAN) $<
#
imsettings_check_SOURCES =				\
	imsettings-check.c				\
	$(NULL)
imsettings_check_LDADD =				\
	$(X11_LIBS)					\
	$(NULL)
imsettings_info_SOURCES =				\
	imsettings-info.c				\
	$(NULL)
imsettings_list_SOURCES =				\
	imsettings-list.c				\
	$(NULL)
imsettings_reload_SOURCES =				\
	imsettings-reload.c				\
	$(NULL)
imsettings_switch_SOURCES =				\
	imsettings-switch.c				\
	$(NULL)

-include $(top_srcdir)/git.mk
