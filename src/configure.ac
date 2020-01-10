dnl Process this file with autoconf to produce a configure script.
AC_PREREQ(2.62)
AC_INIT([imsettings], 1.6.3, [http://code.google.com/p/imsettings/issues/entry])

. `dirname $0`/requires

AM_INIT_AUTOMAKE([1.11 -Wno-portability dist-bzip2])
dnl silent build rules, requires at least automake-1.11.
dnl by either passing --enable-silent-rules to configure or
dnl V=0 to make.
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AM_MAINTAINER_MODE
AM_CONFIG_HEADER([config.h])
AC_CONFIG_MACRO_DIR([m4macros])

AC_PROG_LIBTOOL
AC_PROG_INTLTOOL

AC_PROG_CC
AC_PROG_CXX

GNOME_COMPILE_WARNINGS

GNOME_COMMON_INIT
GNOME_DEBUG_CHECK
GNOME_MAINTAINER_MODE_DEFINES

GTK_DOC_CHECK(1.0)

CFLAGS="$CFLAGS $WARN_CFLAGS"

dnl ======================================================================
dnl Libraries versioning
dnl ======================================================================
dnl Quote from Updating library version information at libtool.info
dnl and renumbering
dnl
dnl 1. Update the version information only immediately before a public
dnl    release of your software.  More frequent updates are unnecessary,
dnl    and only guarantee that the current interface number gets larger
dnl    faster.
dnl 2. If the library source code has changed at all since the last
dnl    update, then increment REVISION (`C:R:A' becomes `C:r+1:A')
dnl 3. If any interfaces have been added, removed, or changed since the
dnl    last update, increment CURRENT, and set REVISION to 0.
dnl 4. If any interfaces have been added since the last public release,
dnl    then increment AGE.
dnl 5. If any interfaces have been removed since the last public release,
dnl    then set AGE to 0.
dnl
LT_CURRENT=9
LT_REVISION=1
LT_AGE=4

AC_SUBST(LT_CURRENT)
AC_SUBST(LT_REVISION)
AC_SUBST(LT_AGE)

dnl ======================================================================
dnl define variables
dnl ======================================================================
dbus_prefix=`pkg-config --variable prefix dbus-1`
DBUS_SERVICE_DIR=`pkg-config --variable session_bus_services_dir dbus-1|sed -e "s,${dbus_prefix},\\$(prefix),"`
AC_SUBST(DBUS_SERVICE_DIR)
IMSETTINGS_MODULEDIR="${libdir}/imsettings"
AC_SUBST(IMSETTINGS_MODULEDIR)

AM_MISSING_PROG([GIT], [git])
AM_MISSING_PROG([DB2MAN], [db2x_docbook2man])

dnl ======================================================================
dnl functions testing
dnl ======================================================================

dnl ======================================================================
dnl gettext stuff
dnl ======================================================================
GETTEXT_PACKAGE=$PACKAGE
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE", [Gettext package])

AM_GLIB_GNU_GETTEXT

dnl ======================================================================
dnl check pkg-config stuff
dnl ======================================================================
PKG_CHECK_MODULES(IMSETTINGS, gio-2.0 >= $GIO_REQUIRED
			      gobject-2.0 >= $GOBJECT_REQUIRED
			      gmodule-2.0)
PKG_CHECK_MODULES(LIBNOTIFY, libnotify, [
	PKG_CHECK_MODULES(LIBNOTIFY_OLD, libnotify < $LIBNOTIFY_REQUIRED, [
		has_old_libnotify=yes
		PKG_CHECK_MODULES(GTK, gtk+-2.0 >= $GTK_REQUIRED)
		AC_DEFINE(HAS_OLD_LIBNOTIFY,, [Using older libnotify])
		IMDAEMON_CFLAGS="$GTK_CFLAGS $LIBNOTIFY_OLD_CFLAGS"
		IMDAEMON_LIBS="$GTK_LIBS $LIBNOTIFY_OLD_LIBS"],[
		has_old_libnotify=no
		IMDAEMON_CFLAGS="$LIBNOTIFY_CFLAGS"
		IMDAEMON_LIBS="$LIBNOTIFY_LIBS"])
],
	AC_MSG_ERROR([
*** LIBNOTIFY is required.])
)
PKG_CHECK_MODULES(X11, x11)
# for gconf backend
PKG_CHECK_MODULES(GCONF, gconf-2.0,
	[use_gconf="yes"],
	[use_gconf="no"])
# for gsettings backend
PKG_CHECK_MODULES(GSETTINGS, gio-2.0 >= $GIO_REQUIRED,
	[use_gsettings="yes"],
	[use_gsettings="no"])
# for lxde backend
PKG_CHECK_MODULES(LXDE, glib-2.0,
	[use_lxde="yes"],
	[use_lxde="no"])
# for mate backend (mateconf based)
PKG_CHECK_MODULES(MATE, mateconf-2.0,
	[use_mate="yes"],
	[use_mate="no"])
# for xfce backend
PKG_CHECK_MODULES(XFCE,	libxfconf-0,
	[use_xfce="yes"],
	[use_xfce="no"])
# for qt backend
use_qt="yes"
# for XIM backend
PKG_CHECK_MODULES(XIM, libgxim >= $LIBGXIM_REQUIRED
		       gdk-2.0 >= $GDK_REQUIRED,
	[use_xim="yes"],
	[use_xim="no"])
# for unit tests
PKG_CHECK_MODULES(GTHREAD, gthread-2.0 >= $GTHREAD_REQUIRED)
PKG_CHECK_MODULES(CHECK, check >= $CHECK_REQUIRED,
	[use_check="yes"],
	[use_check="no"])

AC_SUBST(IMDAEMON_CFLAGS)
AC_SUBST(IMDAEMON_LIBS)
AC_SUBST(IMSETTINGS_CFLAGS)
AC_SUBST(IMSETTINGS_LIBS)
AC_SUBST(X11_CFLAGS)
AC_SUBST(X11_LIBS)
AC_SUBST(GCONF_CFLAGS)
AC_SUBST(GCONF_LIBS)
AC_SUBST(GSETTINGS_CFLAGS)
AC_SUBST(GSETTINGS_LIBS)
AC_SUBST(XFCE_CFLAGS)
AC_SUBST(XFCE_LIBS)
AC_SUBST(LXDE_CFLAGS)
AC_SUBST(LXDE_LIBS)
AC_SUBST(MATE_CFLAGS)
AC_SUBST(MATE_LIBS)
AC_SUBST(XIM_CFLAGS)
AC_SUBST(XIM_LIBS)
AC_SUBST(CHECK_CFLAGS)
AC_SUBST(CHECK_LIBS)
AC_SUBST(GTHREAD_CFLAGS)
AC_SUBST(GTHREAD_LIBS)

AM_CONDITIONAL(ENABLE_UNIT_TEST, test x$use_check != xno)
AM_CONDITIONAL(ENABLE_GCONF, test x$use_gconf != xno)
AM_CONDITIONAL(ENABLE_GSETTINGS, test x$use_gsettings != xno)
AM_CONDITIONAL(ENABLE_LXDE, test x$use_lxde != xno)
AM_CONDITIONAL(ENABLE_MATE, test x$use_mate != xno)
AM_CONDITIONAL(ENABLE_QT, test x$use_qt != xno)
AM_CONDITIONAL(ENABLE_XFCE, test x$use_xfce != xno)
AM_CONDITIONAL(ENABLE_XIM, test x$use_xim != xno)

dnl ======================================================================
dnl check another libraries
dnl ======================================================================
AM_PATH_GLIB_2_0($GLIB_REQUIRED, :,
	AC_MSG_ERROR([
*** GLIB $GLIB_REQUIRED_VERSION or better is required. The latest version of
*** GLIB is always available from ftp://ftp.gtk.org/.]),
    glib)

GOBJECT_INTROSPECTION_CHECK([1.30.0])

dnl ======================================================================
dnl options
dnl ======================================================================
AC_ARG_ENABLE(fallback-im,
	[AC_HELP_STRING([--disable-fallback-im],
		[disable the fallback immodule support in GTK+])],,
	[enable_fallback_im=yes])
AC_ARG_ENABLE(rebuilds,
	[AC_HELP_STRING([--disable-rebuilds],
		[disable all source autogeneration rules])],,
	[enable_rebuilds=yes])
AC_ARG_WITH(xinputdir,
	AC_HELP_STRING([--with-xinputdir], [build with the specific xinput script directory])
	,,
	with_xinputdir=no)
AC_ARG_WITH(xinputrcdir,
	AC_HELP_STRING([--with-xinputrcdir], [build with the specific xinputrc directory])
	,,
	with_xinputrcdir=no)
AC_ARG_WITH(xinitdir,
	AC_HELP_STRING([--with-xinitdir], [build with the specific xinit script directory])
	,,
	with_xinitdir=no)
AC_ARG_WITH(xinput-suffix,
	AC_HELP_STRING([--with-xinput-suffix], [specify the xinput file suffix])
	,,
	with_xinput_suffix=".conf")
AC_ARG_WITH(xinputsh,
	AC_HELP_STRING([--with-xinputsh], [specify the xinput shell script name])
	,,
	with_xinputsh=no)

dnl ======================================================================
dnl options - fallback-im
dnl ======================================================================
fallback_immodule_gtk3=no
fallback_immodule_gtk2=no

PKG_CHECK_MODULES(GTK3, gtk+-3.0,
	[PKG_CHECK_MODULES(FALLBACK_SUPPORT_IN_GTK, gtk+-3.0 >= 3.3.3,
		[fallback_immodule_gtk3=yes],
		[enable_fallback_im=no])],
	[fallback_immodule_gtk3=no])
PKG_CHECK_MODULES(GTK2, gtk+-2.0,
	[PKG_CHECK_MODULES(FALLBACK_SUPPORT_IN_GTK2, gtk+-2.0 >= 2.24.11,
		[fallback_immodule_gtk2=yes],
		[enable_fallback_im=no])],
	[fallback_immodule_gtk2=no])
AC_MSG_CHECKING([Whether GTK+ supports the immodules' fallback])
if test "x$enable_fallback_im" = "xno"; then
	AC_MSG_RESULT([no])
else
	AC_MSG_RESULT([$enable_fallback_im (gtk3: $fallback_immodule_gtk3, gtk2: $fallback_immodule_gtk2)])
	AC_DEFINE(ENABLE_FALLBACK_IM,, [Use the fallback support in GTK+])
fi

dnl ======================================================================
dnl options - rebuilds
dnl ======================================================================
REBUILD=\#
if test "x$enable_rebuilds" = "xyes"; then
	REBUILD=
fi
AC_SUBST(REBUILD)

dnl ======================================================================
dnl options - xinputdir
dnl ======================================================================
AC_MSG_CHECKING(whether to build the specific xinput script directory)
if test "x$with_xinputdir" = xno; then
	with_xinputdir="${sysconfdir}/X11/xinit/xinput.d/"
	AC_MSG_RESULT(no)
else
	AC_MSG_RESULT(yes)
fi
XINPUT_PATH=$with_xinputdir
AC_SUBST(XINPUT_PATH)

dnl ======================================================================
dnl options - xinputrcdir
dnl ======================================================================
AC_MSG_CHECKING(whether to build the specific xinputrc directory)
if test "x$with_xinputrcdir" = xno; then
	with_xinputrcdir="${sysconfdir}/X11/xinit/"
	AC_MSG_RESULT(no)
else
	AC_MSG_RESULT(yes)
fi
XINPUTRC_PATH=$with_xinputrcdir
AC_SUBST(XINPUTRC_PATH)

dnl ======================================================================
dnl options - xinputsh
dnl ======================================================================
AC_MSG_CHECKING(whether to give the specific xinput.sh name)
if test "x$with_xinputsh" = xno; then
	with_xinputsh="xinput.sh"
	AC_MSG_RESULT(no)
else
	AC_MSG_RESULT(yes)
fi
XINPUTSH=$with_xinputsh
AC_SUBST(XINPUTSH)

dnl ======================================================================
dnl options - xinitdir
dnl ======================================================================
AC_MSG_CHECKING(whether to build the specific xinit script directory)
if test "x$with_xinitdir" = xno; then
	with_xinitdir="${sysconfdir}/X11/xinit/xinitrc.d/"
	AC_MSG_RESULT(no)
else
	AC_MSG_RESULT(yes)
fi
XINIT_PATH=$with_xinitdir
AC_SUBST(XINIT_PATH)

dnl ======================================================================
dnl options - xinit-suffix
dnl ======================================================================
AC_MSG_CHECKING(whether to build the specific xinput suffix)
if test "x$with_xinput_suffix" = xno; then
   	with_xinput_suffix=""
	AC_MSG_RESULT(no)
else
	AC_MSG_RESULT(yes)
fi
XINPUT_SUFFIX=$with_xinput_suffix
AC_SUBST(XINPUT_SUFFIX)

dnl ======================================================================
dnl output
dnl ======================================================================
AC_CONFIG_COMMANDS([chmod-scripts],
	[chmod 0755 data/xinput.sh.in
	 chmod 0755 data/xinputinfo.sh
	 chmod 0755 data/imsettings-target-checker.sh.in])
AC_CONFIG_FILES([
	Makefile
	imsettings.pc
	imsettings-uninstalled.pc
	backends/Makefile
	backends/gconf/Makefile
	backends/gsettings/Makefile
	backends/lxde/Makefile
	backends/mate-gsettings/Makefile
	backends/mateconf/Makefile
	backends/qt/Makefile
	backends/xfce/Makefile
	backends/xim/Makefile
	data/Makefile
	data/none.in
	data/xcompose.in
	data/xim.in
	data/xinput.sh.in
	data/xinputinfo.sh
	docs/Makefile
	docs/version.xml
	imsettings/Makefile
	imsettings-daemon/Makefile
	po/Makefile.in
	utils/Makefile
	tests/Makefile
	tests/testcases/Makefile
])
AC_OUTPUT

dnl ======================================================================
dnl result
dnl ======================================================================
echo ""
echo "========== Build Information =========="
echo " CFLAGS:                 $CFLAGS $IMSETTINGS_CFLAGS"
echo " LDFLAGS:                $LDFLAGS $IMSETTINGS_LIBS"
echo " GConf support:          $use_gconf"
if test "x$use_gconf" = "xyes"; then
   echo "   CFLAGS:               $GCONF_CFLAGS"
   echo "   LDFLAGS:              $GCONF_LIBS"
fi
echo " GSettings support:      $use_gsettings"
if test "x$use_gconf" = "xyes"; then
   echo "   CFLAGS:               $GSETTINGS_CFLAGS"
   echo "   LDFLAGS:              $GSETTINGS_LIBS"
fi
echo " MATE (< 1.5) support:      $use_mate"
if test "x$use_mate" = "xyes"; then
   echo "   CFLAGS:               $MATE_CFLAGS"
   echo "   LDFLAGS:              $MATE_LIBS"
fi
echo " LXDE support:           $use_lxde"
if test "x$use_lxde" = "xyes"; then
   echo "   CFLAGS:               $LXDE_CFLAGS"
   echo "   LDFLAGS:              $LXDE_LIBS"
fi
echo " Qt support:             $use_qt"
if test "x$use_qt" = "xyes"; then
   echo "   CFLAGS:               $GLIB_CFLAGS"
   echo "   LDFLAGS:              $GLIB_LIBS"
fi
echo " Xfce support:           $use_xfce"
if test "x$use_xfce" = "xyes"; then
   echo "   CFLAGS:               $XFCE_CFLAGS"
   echo "   LDFLAGS:              $XFCE_LIBS"
fi
echo " XIM support:            $use_xim"
if test "x$use_xim" = "xyes"; then
   echo "   CFLAGS:               $XIM_CFLAGS"
   echo "   LDFLAGS:              $XIM_LIBS"
fi
echo " XINPUT_PATH:            $XINPUT_PATH"
echo " XINPUTRC_PATH:          $XINPUTRC_PATH"
echo " XINIT_PATH:             $XINIT_PATH"
echo " XINPUTSH:               $XINPUTSH"
echo " XINPUT_SUFFIX:          $XINPUT_SUFFIX"
echo " Rebuilds:               $enable_rebuilds"
echo " Unit testing:           $use_check"
echo ""
