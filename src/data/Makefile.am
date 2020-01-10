##
# Global definitions
NULL =
EXTRA_DIST =				\
	$(xinput_in_in_files)		\
	$(xinputsh_in_in)		\
	$(xinputinfosh_in)		\
	$(icon_DATA)			\
	$(imsettings_functions_in)	\
	$(imsettings_target_checker_in)	\
	$(NULL)
CLEANFILES =				\
	$(xinput_DATA)			\
	$(xinputsh_DATA)		\
	$(xinputinfosh_in:.in=)		\
	$(imsettings_functions)		\
	$(imsettings_target_checker)	\
	$(NULL)


##
# Local definitions
xinput_in_in_files =		\
	none.in.in		\
	xim.in.in		\
	xcompose.in.in		\
	$(NULL)
xinput_in_files = $(xinput_in_in_files:.in.in=.in)
#
xinputsh_in_in = xinput.sh.in.in
xinputsh_in = $(xinputsh_in_in:.in=)
#
xinputinfosh_in = xinputinfo.sh.in
#
imsettings_functions_in = imsettings-functions.in
#
imsettings_target_checker_in = imsettings-target-checker.sh.in


$(xinput_DATA): $(xinput_in_files) $(builddir)/Makefile
	$(AM_V_GEN) cp `echo $@ | sed -e 's|$(XINPUT_SUFFIX)$$||'`.in $@
#
$(xinputsh_DATA): $(xinputsh_in) $(builddir)/Makefile
	$(AM_V_GEN) sed -e 's|@libexecdir\[@\]|$(libexecdir)|g' $(xinputsh_in) > $@
#
$(imsettings_functions): $(imsettings_functions_in) $(builddir)/Makefile
	$(AM_V_GEN) sed -e 's|@libexecdir\[@\]|$(libexecdir)|g' $(srcdir)/$(imsettings_functions_in) > $@
$(imsettings_target_checker): $(imsettings_target_checker_in) $(builddir)/Makefile
	$(AM_V_GEN) sed -e 's|@libexecdir\[@\]|$(libexecdir)|g' $(srcdir)/$(imsettings_target_checker_in) > $@

##
# Target platform
xinputdir = $(XINPUT_PATH)
xinput_DATA = $(xinput_in_files:.in=$(XINPUT_SUFFIX))
#
xinputshdir = @XINIT_PATH@
xinputsh_DATA = @XINPUTSH@
#
imsettings_functions = $(imsettings_functions_in:.in=)
#
imsettings_target_checker = $(imsettings_target_checker_in:.in=)
#
helperdir = $(libexecdir)
helper_SCRIPTS = 			\
	$(xinputinfosh_in:.in=)		\
	$(imsettings_target_checker)	\
	$(imsettings_functions)		\
	$(NULL)
#
icondir = $(datadir)/pixmaps
icon_DATA = 				\
	imsettings-unknown.png		\
	$(NULL)

.PHONY: clean-local

-include $(top_srcdir)/git.mk
