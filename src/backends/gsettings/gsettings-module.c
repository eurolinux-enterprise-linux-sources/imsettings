/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * gsettings-module.c
 * Copyright (C) 2011-2012 Red Hat, Inc. All rights reserved.
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

#include <gio/gio.h>
#include <glib/gi18n-lib.h>
#include "imsettings-info.h"
#include "imsettings-utils.h"

#define IMMODULE_SCHEMA		"org.gnome.desktop.interface"
#define IMMODULE_KEY		"gtk-im-module"

gboolean  _check_schema   (void);
void      module_switch_im(IMSettingsInfo *info);
gchar    *module_dump_im  (void);

/*< private >*/
gboolean
_check_schema(void)
{
	const gchar * const *schemas = g_settings_list_schemas();
	gint i;

	for (i = 0; schemas[i] != NULL; i++) {
		if (g_strcmp0(schemas[i], IMMODULE_SCHEMA) == 0)
			return TRUE;
	}
	g_warning("Settings schema '%s' is not installed.",
		  IMMODULE_SCHEMA);

	return FALSE;
}

/*< public >*/
void
module_switch_im(IMSettingsInfo *info)
{
	GSettings *settings = NULL;
	const gchar *gtkimm = imsettings_info_get_gtkimm(info);
	gchar *val = NULL;

	if (!gtkimm || gtkimm[0] == 0) {
		g_warning("Invalid gtk immodule in: %s",
			  imsettings_info_get_filename(info));
		goto finalize;
	}
	if (!_check_schema())
		goto finalize;
#ifdef ENABLE_FALLBACK_IM
	val = g_strdup_printf("%s:xim", gtkimm);
#else
	val = g_strdup(gtkimm);
#endif
	settings = g_settings_new(IMMODULE_SCHEMA);
	if (!g_settings_set_string(settings,
				   IMMODULE_KEY,
				   val)) {
		g_warning("Unable to set %s to %s", IMMODULE_KEY, IMMODULE_SCHEMA);
		goto finalize;
	}
	g_log(G_LOG_DOMAIN, G_LOG_LEVEL_INFO,
	      "Setting up %s as gtk+ immodule",
	      val);
  finalize:
	g_free(val);
	if (settings)
		g_object_unref(settings);
}

gchar *
module_dump_im(void)
{
	GSettings *settings;
	gchar *retval = NULL;

	if (_check_schema()) {
		settings = g_settings_new(IMMODULE_SCHEMA);
		retval = g_settings_get_string(settings,
					       IMMODULE_KEY);
		if (settings)
			g_object_unref(settings);
	}

	return retval;
}
