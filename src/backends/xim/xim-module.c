/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * xim-module.c
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

#include <gio/gio.h>
#include "imsettings.h"
#include "imsettings-info.h"

void   g_module_unload (GModule *module);
void   module_switch_im(IMSettingsInfo *info);
gchar *module_dump_im  (void);

static gchar *__client_address = NULL;

/*< private >*/

/*< public >*/
void
g_module_unload(GModule *module)
{
	GDBusConnection *connection;
	GError *err = NULL;
	GVariant *value;
	gboolean ret = FALSE;

	if (!__client_address)
		return;
	connection = g_dbus_connection_new_for_address_sync(__client_address,
							    G_DBUS_CONNECTION_FLAGS_AUTHENTICATION_CLIENT,
							    NULL,
							    NULL,
							    &err);
	if (!connection) {
		g_warning("Unable to connect to %s: %s",
			  __client_address,
			  err->message);
		g_error_free(err);
		return;
	}

	value = g_dbus_connection_call_sync(connection,
					    NULL,
					    IMSETTINGS_XIM_PATH_DBUS,
					    IMSETTINGS_XIM_INTERFACE_DBUS,
					    "StopService",
					    NULL,
					    G_VARIANT_TYPE ("(b)"),
					    G_DBUS_CALL_FLAGS_NONE,
					    -1,
					    NULL,
					    &err);
	if (value)
		g_variant_get(value, "(b)", &ret);
	if (!ret) {
		g_warning("Unable to stop imsettings-xim XIM server: %s",
			  err ? err->message : "unknown");
	} else {
		g_log(G_LOG_DOMAIN, G_LOG_LEVEL_INFO,
		      "Shut down imsettings-xim");
	}

	if (value)
		g_variant_unref(value);
	g_object_unref(connection);
}

void
module_switch_im(IMSettingsInfo *info)
{
	GDBusConnection *connection;
	GError *err = NULL;
	GVariant *value;
	gboolean ret = FALSE;
	const gchar *xim = imsettings_info_get_xim(info);

	if (!__client_address) {
		g_spawn_command_line_async("imsettings-xim --address=unix:abstract=/tmp/imsettings-xim --replace", &err);
		if (err) {
			g_warning("Unable to spawn XIM server: %s", err->message);
			g_error_free(err);
			return;
		}
		__client_address = g_strdup("unix:abstract=/tmp/imsettings-xim");
		g_usleep(3 * G_USEC_PER_SEC);
	}
	connection = g_dbus_connection_new_for_address_sync(__client_address,
							    G_DBUS_CONNECTION_FLAGS_AUTHENTICATION_CLIENT,
							    NULL,
							    NULL,
							    &err);
	if (!connection) {
		g_warning("Unable to connect to %s: %s",
			  __client_address,
			  err->message);
		g_error_free(err);
		return;
	}

	value = g_dbus_connection_call_sync(connection,
					    NULL,
					    IMSETTINGS_XIM_PATH_DBUS,
					    IMSETTINGS_XIM_INTERFACE_DBUS,
					    "SwitchXIM",
					    g_variant_new("(s)",
							  xim),
					    G_VARIANT_TYPE ("(b)"),
					    G_DBUS_CALL_FLAGS_NONE,
					    -1,
					    NULL,
					    &err);
	if (value)
		g_variant_get(value, "(b)", &ret);
	if (!ret) {
		g_warning("Unable to update XIM settings: %s",
			  err ? err->message : "unknown");
	} else {
		g_log(G_LOG_DOMAIN, G_LOG_LEVEL_INFO,
		      "Setting up %s as XIM", xim);
	}

	if (value)
		g_variant_unref(value);
	g_object_unref(connection);
}

gchar *
module_dump_im(void)
{
	GDBusConnection *connection;
	GError *err = NULL;
	GVariant *value;
	gchar *retval = NULL;

	if (!__client_address) {
		g_warning("imsettings-xim XIM server isn't running.");
		return NULL;
	}
	connection = g_dbus_connection_new_for_address_sync(__client_address,
							    G_DBUS_CONNECTION_FLAGS_AUTHENTICATION_CLIENT,
							    NULL,
							    NULL,
							    &err);
	if (!connection) {
		g_warning("Unable to connect to %s: %s",
			  __client_address,
			  err->message);
		g_error_free(err);
		return NULL;
	}

	value = g_dbus_connection_call_sync(connection,
					    NULL,
					    IMSETTINGS_XIM_PATH_DBUS,
					    IMSETTINGS_XIM_INTERFACE_DBUS,
					    "DumpXIMConfig",
					    NULL,
					    G_VARIANT_TYPE ("(s)"),
					    G_DBUS_CALL_FLAGS_NONE,
					    -1,
					    NULL,
					    &err);
	if (value)
		g_variant_get(value, "(s)", &retval);
	if (!retval) {
		g_warning("Unable to read XIM settings: %s",
			  err ? err->message : "unknown");
	}

	if (value)
		g_variant_unref(value);
	g_object_unref(connection);

	return retval;
}
