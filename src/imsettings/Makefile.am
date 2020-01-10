-include $(INTROSPECTION_MAKEFILE)

##
# Global definitions
NULL =
AM_CPPFLAGS =							\
	-DIMSETTINGS_LOCALEDIR="\"$(datadir)/locale\""		\
	-DXINIT_PATH="\"$(XINIT_PATH)\""			\
	-DXINPUT_PATH="\"$(XINPUT_PATH)\""			\
	-DXINPUT_SUFFIX="\"$(XINPUT_SUFFIX)\""			\
	-DXINPUTINFO_PATH="\"$(libexecdir)\""			\
	-DICONDIR="\"$(datadir)/pixmaps\""			\
	-I$(top_srcdir)/imsettings/				\
	-I$(top_srcdir)						\
	$(IMSETTINGS_CFLAGS)					\
	$(NULL)
LIBS =								\
	@LDFLAGS@						\
	$(IMSETTINGS_LIBS)					\
	$(NULL)
EXTRA_DIST =							\
	$(NULL)
MAINTAINERCLEANFILES =						\
	$(imsettings_built_public_headers)			\
	$(imsettings_built_private_headers)			\
	$(imsettings_built_sources)				\
	$(stamp_files)						\
	$(NULL)
CLEANFILES =							\
	xgen-imh						\
	xgen-imc						\
	$(NULL)
BUILT_FILES =							\
	$(imsettings_built_public_headers)			\
	$(imsettings_built_private_headers)			\
	$(imsettings_built_sources)				\
	$(NULL)

INTROSPECTION_GIRS =
INTROSPECTION_SCANNER_ARGS =					\
	--add-include-path=$(srcdir)				\
	--warn-all						\
	$(NULL)
INTROSPECTION_COMPILER_ARGS =					\
	--includedir=$(srcdir)					\
	$(NULL)

##
# Local definitions
imsettings_public_headers =					\
	imsettings.h						\
	imsettings-info.h					\
	imsettings-client.h					\
	imsettings-utils.h					\
	$(NULL)	
imsettings_private_headers =					\
	imsettings-private.h					\
	$(NULL)
imsettings_built_public_headers =				\
	$(NULL)
imsettings_built_private_headers =				\
	$(NULL)
#
imsettings_built_sources =					\
	$(NULL)
imsettings_sources =						\
	imsettings-client.c					\
	imsettings-info.c					\
	imsettings-utils.c					\
	$(imsettings_built_sources)				\
	$(NULL)
#
stamp_files =							\
	$(NULL)


##
# Local Rules


##
# Target platform
lib_LTLIBRARIES = 						\
	libimsettings.la					\
	$(NULL)
#
imsettingsincdir = $(includedir)/imsettings
imsettingsinc_HEADERS =						\
	$(imsettings_public_headers)				\
	$(imsettings_built_public_headers)			\
	$(NULL)
#
noinst_HEADERS =						\
	$(imsettings_private_headers)				\
	$(imsettings_built_private_headers)			\
	$(NULL)
#
libimsettings_la_SOURCES =					\
	$(imsettings_sources)					\
	$(NULL)
libimsettings_la_CFLAGS =					\
	-DG_LOG_DOMAIN="\"IMSettings\""				\
	$(NULL)
libimsettings_la_LDFLAGS =					\
	$(LDFLAGS)						\
	-version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)	\
	$(NULL)

if HAVE_INTROSPECTION
introspection_sources =			\
	$(imsettingsinc_HEADERS)	\
	$(libimsettings_la_SOURCES)	\
	$(NULL)

IMSettings-1.3.gir: $(INTROSPECTION_SCANNER) libimsettings.la Makefile
IMSettings_1_3_gir_INCLUDES =		\
	GObject-2.0			\
	Gio-2.0				\
	$(NULL)
IMSettings_1_3_gir_SCANNERFLAGS =	\
	--symbol-prefix=imsettings	\
	$(NULL)
IMSettings_1_3_gir_CFLAGS =		\
	$(AM_CPPFLAGS)			\
	$(NULL)
IMSettings_1_3_gir_LIBS = libimsettings.la
IMSettings_1_3_gir_FILES = $(introspection_sources)
INTROSPECTION_GIRS += IMSettings-1.3.gir

girdir = $(datadir)/gir-1.0
gir_DATA = $(INTROSPECTION_GIRS)

typelibdir = $(libdir)/girepository-1.0
typelib_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

CLEANFILES += $(gir_DATA) $(typelib_DATA)
endif

-include $(top_srcdir)/git.mk
