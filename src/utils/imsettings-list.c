/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * imsettings-list.c
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
#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <glib/gi18n.h>
#include "imsettings.h"
#include "imsettings-info.h"
#include "imsettings-client.h"
#include "imsettings-utils.h"

int
main(int    argc,
     char **argv)
{
	IMSettingsClient *client = NULL;
	IMSettingsInfo *info, *cinfo;
	GVariant *v = NULL, *vv;
	GVariantIter *iter;
	const gchar *locale, *name, *imname, *subimname, *key;
	gchar *user_im = NULL, *system_im = NULL, *running_im = NULL;
	gchar *xinput;
	gint i;
	GError *error = NULL;
	int retval = 0;
	gsize len, slen = strlen(XINPUT_SUFFIX);

#ifdef ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, IMSETTINGS_LOCALEDIR);
#ifdef HAVE_BIND_TEXTDOMAIN_CODESET
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
#endif /* HAVE_BIND_TEXTDOMAIN_CODESET */
	textdomain (GETTEXT_PACKAGE);
#endif /* ENABLE_NLS */

	setlocale(LC_ALL, "");
	locale = setlocale(LC_CTYPE, NULL);

	g_type_init();

	if (!imsettings_is_enabled()) {
		g_printerr(_("IMSettings is disabled on the system.\n"));
		retval = 1;
		goto end;
	}
	client = imsettings_client_new(locale);
	if (!client) {
		g_printerr(_("Unable to create a client instance.\n"));
		retval = 1;
		goto end;
	}
	if (imsettings_client_get_version(client, NULL, &error) != IMSETTINGS_SETTINGS_API_VERSION) {
		if (error)
			goto error;
		g_printerr(_("Currently a different version of imsettings is running.\nRunning \"imsettings-reload\" may help but it will restart the Input Method\n"));
		retval = 1;
		goto end;
	}

	v = imsettings_client_get_info_variants(client, NULL, &error);
	if (error)
		goto error;
	user_im = imsettings_client_get_user_im(client, NULL, &error);
	if (error)
		goto error;
	system_im = imsettings_client_get_system_im(client, NULL, &error);
	if (error)
		goto error;
	cinfo = imsettings_client_get_active_im_info(client, NULL, &error);
	if (error) {
	  error:
		g_printerr("%s\n", error->message);
		retval = 1;
		g_clear_error(&error);
		goto end;
	}
	running_im = g_strdup(imsettings_info_get_im_name(cinfo));
	g_object_unref(cinfo);

	i = 0;
	g_variant_get(v, "a{sv}", &iter);
	while (g_variant_iter_next(iter, "{&sv}", &key, &vv)) {
		gchar *sub;

		len = strlen(key);
		if (len > slen &&
		    strcmp(&key[len - slen], XINPUT_SUFFIX) == 0)
			continue;
		info = imsettings_info_new(vv);
		name = imsettings_info_get_short_desc(info);
		imname = imsettings_info_get_im_name(info);
		subimname = imsettings_info_get_sub_im_name(info);
		xinput = g_path_get_basename(imsettings_info_get_filename(info));
		if (subimname) {
			sub = g_strdup_printf("(%s)", subimname);
		} else {
			sub = g_strdup("");
		}

		g_print("%s %d: %s%s[%s] %s\n",
			(strcmp(running_im, imname) == 0 ? "*" : (strcmp(user_im, name) == 0 ? "-" : " ")),
			i + 1,
			imname, sub, xinput,
			(strcmp(system_im, name) == 0 ? "(recommended)" : ""));
		g_object_unref(info);
		g_free(xinput);
		g_free(sub);
		i++;
	}
	g_variant_iter_free(iter);
  end:
	g_free(user_im);
	g_free(system_im);
	g_free(running_im);
	if (v)
		g_variant_unref(v);
	if (client)
		g_object_unref(client);

	return retval;
}
