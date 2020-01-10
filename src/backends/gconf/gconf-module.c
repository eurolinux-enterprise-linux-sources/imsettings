/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * gconf-module.c
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

#include <gconf/gconf-client.h>
#include "imsettings-info.h"
#include "imsettings-utils.h"

#define GCONF_SCHEMA_KEY	"/desktop/gnome/interface/gtk-im-module"

void   module_switch_im(IMSettingsInfo *info);
gchar *module_dump_im  (void);

/*< private >*/

/*< public >*/
void
module_switch_im(IMSettingsInfo *info)
{
	GConfEngine *engine;
	GConfValue *val = NULL;
	const gchar *gtkimm = imsettings_info_get_gtkimm(info);
	GError *err = NULL;
	gchar *v = NULL;

	engine = gconf_engine_get_default();
	if (!engine) {
		g_warning("Unable to obtain GConfEngine instance.");
		return;
	}
	if (!gtkimm || gtkimm[0] == 0) {
		g_warning("Invalid gtk immodule in: %s",
			  imsettings_info_get_filename(info));
		goto finalize;
	}

#ifdef ENABLE_FALLBACK_IM
	v = g_strdup_printf("%s:xim", gtkimm);
#else
	v = g_strdup(gtkimm);
#endif
	val = gconf_value_new_from_string(GCONF_VALUE_STRING,
					  v, &err);
	if (err)
		goto error;
	g_log(G_LOG_DOMAIN, G_LOG_LEVEL_INFO,
	      "Setting up %s as gtk+ immodule",
	      v);
	gconf_engine_set(engine, GCONF_SCHEMA_KEY,
			 val, &err);
	if (err) {
	  error:
		g_warning("%s", err->message);
		g_error_free(err);
	}
  finalize:
	g_free(v);
	if (val)
		gconf_value_free(val);
	if (engine) {
		gconf_engine_unref(engine);
	}
#if 0
	g_print("%d\n", gconf_debug_shutdown());
#endif
}

gchar *
module_dump_im(void)
{
	GConfEngine *engine;
	GConfValue *val = NULL;
	GError *err = NULL;
	gchar *retval = NULL;

	engine = gconf_engine_get_default();
	if (!engine) {
		g_warning("Unable to obtain GConfEngine instance.");
		return NULL;
	}
	val = gconf_engine_get(engine, GCONF_SCHEMA_KEY, &err);
	if (err) {
		g_warning("%s", err->message);
		g_error_free(err);
	} else {
		retval = g_strdup(gconf_value_get_string(val));
	}
	if (val)
		gconf_value_free(val);
	if (engine) {
		gconf_engine_unref(engine);
	}
#if 0
	g_print("%d\n", gconf_debug_shutdown());
#endif

	return retval;
}
