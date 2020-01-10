NULL =
SUBDIRS =

if ENABLE_GCONF
SUBDIRS += gconf
endif
if ENABLE_GSETTINGS
SUBDIRS += gsettings mate-gsettings
endif
if ENABLE_LXDE
SUBDIRS += lxde
endif
if ENABLE_MATE
SUBDIRS += mateconf
endif
if ENABLE_QT
SUBDIRS += qt
endif
if ENABLE_XFCE
SUBDIRS += xfce
endif
if ENABLE_XIM
SUBDIRS += xim
endif

-include $(top_srcdir)/git.mk
