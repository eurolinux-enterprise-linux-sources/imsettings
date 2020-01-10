/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lxde-module.c
 * Copyright (C) 2009-2012 Red Hat, Inc. All rights reserved.
 * 
 * Authors:
 *   Akira TAGOH  <tagoh@redhat.com>
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA  02110-1301  USA
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include "imsettings-info.h"

void   module_switch_im(IMSettingsInfo *info);
gchar *module_dump_im  (void);

/*< private >*/
static gboolean
_update_config(const gchar *configdir,
	       const gchar *config_file,
	       const gchar *module)
{
	GKeyFile *key = g_key_file_new();
	gchar *keydir = g_build_filename(g_get_user_config_dir(), configdir, NULL);
	gchar *keyfile = g_build_filename(keydir, config_file, NULL);
	gchar *data = NULL;
	gchar *val = NULL;
	gsize len;
	gboolean retval = FALSE;

	if (!g_key_file_load_from_file(key, keyfile, 0, NULL)) {
		if (!g_key_file_load_from_file(key, LXDE_CONFIGDIR G_DIR_SEPARATOR_S "config", 0, NULL)) {
			const gchar * const *sysconfdirs = g_get_system_config_dirs();
			int i;
			gboolean flag = FALSE;

			for (i = 0; sysconfdirs[i] != NULL; i++) {
				gchar *d = g_build_filename(sysconfdirs[i], configdir, config_file, NULL);

				/* another fallback for recent release */
				if (g_key_file_load_from_file(key, d, 0, NULL)) {
					g_free(d);
					flag = TRUE;
					break;
				}
				g_free(d);
			}
			if (!flag) {
				g_warning("Unable to read the LXDE configuration file.");
				goto bail;
			}
		}
	}

#ifdef ENABLE_FALLBACK_IM
	val = g_strdup_printf("%s:xim", module);
#else
	val = g_strdup(module);
#endif
	g_key_file_set_string(key, "GTK", "sGtk/IMModule",
			      val);

	if ((data = g_key_file_to_data(key, &len, NULL)) != NULL) {
		if (g_mkdir_with_parents(keydir, 0700) != 0) {
			int save_errno = errno;

			g_warning("Failed to create the user config dir: %s",
				  g_strerror(save_errno));
			goto bail;
		}
		if (g_file_set_contents(keyfile, data, len, NULL)) {
			retval = TRUE;
		} else {
			g_warning("Unable to store the configuration into %s", keyfile);
		}
	} else {
		g_warning("Unable to obtain the configuration from the instance.");
	}

  bail:
	g_free(val);
	g_free(data);
	g_free(keyfile);
	g_free(keydir);
	g_key_file_free(key);

	return retval;
}

/*< public >*/
void
module_switch_im(IMSettingsInfo *info)
{
	const gchar *gtkimm = imsettings_info_get_gtkimm(info);
	gboolean warn = TRUE;

	if (!gtkimm || gtkimm[0] == 0) {
		g_warning("Invalid gtk immodule in: %s",
			  imsettings_info_get_filename(info));
		return;
	}

	/* for backward compatible in lxde-settings-daemon */
	if (_update_config("lxde", "config", gtkimm)) {
		gchar *p = g_find_program_in_path("lxde-settings-daemon");
		gchar *cmdline = NULL;

		if (p) {
			cmdline = g_strdup_printf("%s reload", p);
			if (!g_spawn_command_line_sync(cmdline, NULL, NULL, NULL, NULL)) {
				g_warning("Unable to reload the LXDE settings via lxde-settings-daemon.");
			} else {
				warn = FALSE;
			}
		}
		g_free(p);
		g_free(cmdline);
	}
	/* for new xsettings manager in lxsession */
	if (_update_config("lxsession" G_DIR_SEPARATOR_S "LXDE", "desktop.conf", gtkimm)) {
		gchar *p = g_find_program_in_path("lxsession");
		gchar *cmdline = NULL;

		if (p) {
			cmdline = g_strdup_printf("%s -r", p);
			if (!g_spawn_command_line_sync(cmdline, NULL, NULL, NULL, NULL)) {
				g_warning("Unable to reload the LXDE settings via lxsession");
			} else {
				warn = FALSE;
			}
		}
		g_free(p);
		g_free(cmdline);
	}
	if (warn) {
		g_warning("the changes will not be applied until your next login.");
	} else {
		g_log(G_LOG_DOMAIN, G_LOG_LEVEL_INFO,
		      "Setting up %s as gtk+ immodule",
		      gtkimm);
	}
}

gchar *
module_dump_im(void)
{
	GKeyFile *key = g_key_file_new();
	gchar *confdir = g_build_filename(g_get_user_config_dir(), "lxde", NULL);
	gchar *conf = g_build_filename(confdir, "config", NULL);
	gchar *retval = NULL;
	GError *err = NULL;

	if (!g_key_file_load_from_file(key, conf, 0, NULL)) {
		if (!g_key_file_load_from_file(key, LXDE_CONFIGDIR G_DIR_SEPARATOR_S "config", 0, NULL)) {
			g_warning("Unable to load the lxde configuration file.");
			goto finalize;
		}
	}

	retval = g_key_file_get_string(key, "GTK", "sGtk/IMModule", &err);
	if (err) {
		g_warning(err->message);
		g_error_free(err);
	}
  finalize:
	g_free(conf);
	g_free(confdir);
	g_key_file_free(key);

	return retval;
}
