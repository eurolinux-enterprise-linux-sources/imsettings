/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * qt-module.c
 * Copyright (C) 2008-2012 Red Hat, Inc. All rights reserved.
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

#include <stdio.h>
#include <string.h>
#include <glib.h>
#include "imsettings-info.h"

void   module_switch_im(IMSettingsInfo *info);
gchar *module_dump_im  (void);

/*< private >*/

/*< public >*/
void
module_switch_im(IMSettingsInfo *info)
{
	GKeyFile *keyfile;
	gchar *filename, *orig_data = NULL, *data = NULL;
	const gchar *qtimm = imsettings_info_get_qtimm(info);
	gsize orig_size, size;
	GError *error = NULL;

	if (!qtimm || qtimm[0] == 0) {
		g_warning("Invalid Qt immodule in: %s",
			  imsettings_info_get_filename(info));
		return;
	}

	keyfile = g_key_file_new();
	filename = g_build_filename(g_get_user_config_dir(), "Trolltech.conf", NULL);
	if (!g_key_file_load_from_file(keyfile, filename, G_KEY_FILE_KEEP_COMMENTS, &error)) {
		if (error) {
			g_warning("Unable to read the Qt config file: %s", error->message);
		} else {
			g_warning("Unable to read the Qt config file: unknown error");
		}
		goto bail;
	}
	/* validate if the output is same to prevent breakage
	 * when any changes happens in QSettings.
	 */
	data = g_key_file_to_data(keyfile, &size, &error);
	if (error) {
		g_warning("Unable to obtain the config data: %s", error->message);
		goto bail;
	}
	g_file_get_contents(filename, &orig_data, &orig_size, &error);
	if (error) {
		g_warning("Unable to obtain the Qt config file: %s", error->message);
		goto bail;
	}
	if ((size - orig_size) > 1 ||
	    (size - orig_size) < 0) {
		/* XXX: need to check where is really different */
		g_critical("[BUG] Unable to proceed updating Qt config file due to GKeyFile's incompatibility: size mismatch: %" G_GSIZE_FORMAT " vs %" G_GSIZE_FORMAT, orig_size, size);
	}
	g_free(orig_data);
	g_free(data);
	orig_data = NULL;
	data = NULL;

	/* Update DefaultInputMethod */
	g_key_file_set_value(keyfile, "Qt", "DefaultInputMethod", qtimm);

	/* write back to the Qt config file */
	data = g_key_file_to_data(keyfile, &size, &error);
	if (error) {
		g_warning("Unable to obtain the updated config data: %s", error->message);
		goto bail;
	}
	if (!g_file_set_contents(filename, data, size, &error)) {
		if (error) {
			g_warning("Unable to update the Qt config file: %s", error->message);
		} else {
			g_warning("Unable to update the Qt config file: unknown error");
		}
		goto bail;
	}

	g_log(G_LOG_DOMAIN, G_LOG_LEVEL_INFO,
	      "Setting up %s as Qt immodule", qtimm);
  bail:
	if (error) {
		g_error_free(error);
	}

	g_free(orig_data);
	g_free(data);
	g_free(filename);
	g_key_file_free(keyfile);
}

gchar *
module_dump_im(void)
{
	GKeyFile *keyfile;
	gchar *filename, *retval = NULL;
	GError *error = NULL;

	keyfile = g_key_file_new();
	filename = g_build_filename(g_get_user_config_dir(), "Trolltech.conf", NULL);
	if (!g_key_file_load_from_file(keyfile, filename, G_KEY_FILE_KEEP_COMMENTS, &error)) {
		if (error) {
			g_warning("Unable to read the Qt config file: %s", error->message);
		} else {
			g_warning("Unable to read the Qt config file: unknown error");
		}
		goto bail;
	}
	retval = g_key_file_get_value(keyfile, "Qt", "DefaultInputMethod", &error);
	if (error) {
		g_warning("Unable to obtain the DefaultInputMethod: %s", error->message);
		goto bail;
	}

  bail:
	if (error) {
		g_error_free(error);
	}
	g_free(filename);
	g_key_file_free(keyfile);

	return retval;
}
