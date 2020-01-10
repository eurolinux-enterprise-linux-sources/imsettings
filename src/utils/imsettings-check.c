/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * imsettings-check.c
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

#include <stdio.h>
#include <glib.h>
#include <glib/gi18n.h>
#include <gio/gio.h>
#include <X11/Xlib.h>
#include "imsettings.h"
#include "imsettings-client.h"
#include "imsettings-utils.h"

/*< private >*/
static void
_log(const gchar *message,
     gboolean     result,
     const gchar *reason)
{
	g_print("%s: ", message);
	g_print("%s\n", result ? "true" : "false");
	if (!result) {
		g_print("  reason: %s\n", reason ? _(reason) : "unknown");
	}
}

static gboolean
_check_dbus(gboolean verbose)
{
	GDBusConnection *connection;
	gboolean retval = TRUE;
	GError *error = NULL;

	connection = g_bus_get_sync(G_BUS_TYPE_SESSION,
				    NULL,
				    &error);
	if (error)
		retval = FALSE;

	if (connection)
		g_object_unref(connection);

	if (verbose) {
		_log("DBus availability",
		     retval,
		     error ? error->message : NULL);
	}
	if (error)
		g_error_free(error);

	return retval;
}

static gboolean
_check_dbus_session(const gchar *session,
		    gboolean     verbose)
{
	GDBusConnection *connection;
	gboolean retval = TRUE;
	GError *error = NULL;
	GVariant *value;

	connection = g_bus_get_sync(G_BUS_TYPE_SESSION,
				    NULL,
				    &error);
	if (connection && !error) {
		value = g_dbus_connection_call_sync(connection,
						    session,
						    "/",
						    "org.freedesktop.DBus.Peer",
						    "Ping",
						    NULL,
						    NULL,
						    G_DBUS_CALL_FLAGS_NO_AUTO_START,
						    -1,
						    NULL,
						    &error);
		if (value)
			g_variant_unref(value);
	}
	if (error)
		retval = FALSE;

	if (connection)
		g_object_unref(connection);

	if (verbose) {
		_log("DBus session availability",
		     retval,
		     error ? error->message : NULL);
	}
	if (error)
		g_error_free(error);

	return retval;
}

static gboolean
_check_xsettings(gboolean verbose)
{
	Display *dpy = XOpenDisplay(NULL);
	char buffer[256];
	Atom selection_atom;
	GError *error = NULL;
	gboolean retval = TRUE;

	if (!dpy) {
		g_set_error(&error, IMSETTINGS_GERROR, 0, N_("Unable to open X display"));
		retval = FALSE;
		goto bail;
	}
	sprintf(buffer, "_XSETTINGS_S%d", DefaultScreen(dpy));
	selection_atom = XInternAtom(dpy, buffer, False);
	if (!XGetSelectionOwner(dpy, selection_atom)) {
		g_set_error(&error, IMSETTINGS_GERROR, 0, N_("XSETTINGS manager isn't running"));
		retval = FALSE;
		goto bail;
	}
  bail:
	if (dpy)
		XCloseDisplay(dpy);
	if (verbose) {
		_log("XSETTINGS availability",
		     retval,
		     error ? error->message : NULL);
	}
	if (error)
		g_error_free(error);

	return retval;
}

static gboolean
_check_loaded_modules(gboolean verbose)
{
	gchar *locale = setlocale(LC_CTYPE, NULL);
	IMSettingsClient *client = imsettings_client_new(locale);
	GVariant *v = NULL;
	GVariantIter *iter;
	const gchar *key, *val;
	gboolean retval = TRUE;
	GError *error = NULL;

	if (!client) {
		g_set_error(&error, IMSETTINGS_GERROR, 0,
			    N_("Unable to create a client instance."));
		retval = FALSE;
		goto bail;
	}
	if (imsettings_client_get_version(client, NULL, &error) != IMSETTINGS_SETTINGS_API_VERSION) {
		retval = FALSE;
		if (!error) {
			g_set_error(&error, IMSETTINGS_GERROR, 0,
				    N_("imsettings version mismatch"));
		}
		goto bail;
	}
	v = imsettings_client_get_module_settings(client, NULL, &error);
	if (error) {
		retval = FALSE;
		goto bail;
	}
	g_variant_get(v, "a{ss}", &iter);
	if (!g_variant_iter_next(iter, "{&s&s}", &key, &val)) {
		retval = FALSE;
		g_set_error(&error, IMSETTINGS_GERROR, 0,
			    N_("No modules loaded"));
		goto bail;
	}
	g_variant_iter_free(iter);
	g_variant_unref(v);
  bail:
	if (client)
		g_object_unref(client);
	if (verbose) {
		_log("Valid modules loaded", retval,
		     error ? error->message : NULL);
	}
	if (error)
		g_error_free(error);

	return retval;
}

static gboolean
_check_modules(gboolean verbose)
{
	GString *current_settings = g_string_new(NULL);
	gchar *locale = setlocale(LC_CTYPE, NULL);
	IMSettingsClient *client = imsettings_client_new(locale);
	GVariant *v = NULL;
	GVariantIter *iter;
	const gchar *key, *val, *last_val = NULL;
	gboolean no_cmp = TRUE, retval = TRUE;
	GError *error = NULL;

	if (!client) {
		g_set_error(&error, IMSETTINGS_GERROR, 0,
			    N_("Unable to create a client instance."));
		retval = FALSE;
		goto bail;
	}
	if (imsettings_client_get_version(client, NULL, &error) != IMSETTINGS_SETTINGS_API_VERSION) {
		retval = FALSE;
		if (!error) {
			g_set_error(&error, IMSETTINGS_GERROR, 0,
				    N_("imsettings version mismatch"));
		}
		goto bail;
	}
	v = imsettings_client_get_module_settings(client, NULL, &error);
	if (error) {
		retval = FALSE;
		goto bail;
	}
	g_variant_get(v, "a{ss}", &iter);
	while (g_variant_iter_next(iter, "{&s&s}", &key, &val)) {
		if (no_cmp) {
			last_val = val;
			no_cmp = FALSE;
		} else {
			if (retval && g_strcmp0(last_val, val) != 0) {
				retval = FALSE;
				g_set_error(&error, IMSETTINGS_GERROR, 0,
					    N_("Please see .imsettings.log for more details"));
			}
			last_val = val;
		}
		g_string_append_printf(current_settings, "    %s:\t%s\n", key, val);
	}
	if (no_cmp) {
		g_set_error(&error, IMSETTINGS_GERROR, 0,
			    N_("No modules loaded"));
		retval = FALSE;
	}
	g_variant_iter_free(iter);
	g_variant_unref(v);
  bail:
	if (client)
		g_object_unref(client);
	if (verbose) {
		_log("Current settings in modules", retval,
		     error ? error->message : NULL);
		g_print("  result:\n%s", current_settings->str);
	}
	if (error)
		g_error_free(error);
	g_string_free(current_settings, TRUE);

	return retval;
}

/*< public >*/

int
main(int    argc,
     char **argv)
{
	gboolean arg_detail = FALSE, arg_check_dbus = FALSE, arg_check_modules = FALSE, arg_check_module_settings = FALSE, arg_check_xsettings = FALSE, check_all = FALSE;
	gchar *arg_session_name = NULL;
	GOptionContext *ctx = g_option_context_new(NULL);
	GOptionEntry entries[] = {
		/* For translators: this is a translation for the command-line option. */
		{"detail", 'd', 0, G_OPTION_ARG_NONE, &arg_detail, N_("Output the detail information for the result"), NULL},
		{"check-dbus", 0, 0, G_OPTION_ARG_NONE, &arg_check_dbus, N_("Check if DBus is running")},
		{"check-dbus-session", 0, 0, G_OPTION_ARG_STRING, &arg_session_name, N_("Check if SESSION is on line"), "SESSION"},
		{"check-modules", 0, 0, G_OPTION_ARG_NONE, &arg_check_modules, N_("Check if any valid modules are loaded")},
		{"check-modulesettings", 0, 0, G_OPTION_ARG_NONE, &arg_check_module_settings, N_("Check if all of the module settings has consistencies")},
		{"check-xsettings", 0, 0, G_OPTION_ARG_NONE, &arg_check_xsettings, N_("Check if XSETTINGS manager is running")},
		{NULL, 0, 0, 0, NULL, NULL, NULL}
	};
	GError *error = NULL;
	int retval = 0;

#ifdef ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, IMSETTINGS_LOCALEDIR);
#ifdef HAVE_BIND_TEXTDOMAIN_CODESET
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
#endif /* HAVE_BIND_TEXTDOMAIN_CODESET */
	textdomain (GETTEXT_PACKAGE);
#endif /* ENABLE_NLS */

	setlocale(LC_ALL, "");

	g_type_init();

	g_option_context_add_main_entries(ctx, entries, GETTEXT_PACKAGE);
	if (!g_option_context_parse(ctx, &argc, &argv, &error)) {
		if (error != NULL) {
			g_printerr("%s\n", error->message);
		} else {
			g_warning(_("Unknown error in parsing the command lines."));
		}
		return 1;
	}

	if (!arg_check_dbus &&
	    !arg_session_name &&
	    !arg_check_xsettings &&
	    !arg_check_modules &&
	    !arg_check_module_settings)
		check_all = TRUE;

	/* check if DBus is running */
	if ((arg_check_dbus || check_all) &&
	    !_check_dbus(arg_detail)) {
		retval = 1;
		goto end;
	}

	/* check if the specific dbus session is running */
	if (arg_session_name) {
		if (!arg_check_dbus && !_check_dbus(arg_detail)) {
			retval = 1;
			goto end;
		}
		if (!_check_dbus_session(arg_session_name, arg_detail)) {
			retval = 1;
			goto end;
		}
	}
	/* check if XSETTINGS manager is running */
	if ((arg_check_xsettings || check_all) &&
	    !_check_xsettings(arg_detail)) {
		retval = 1;
		goto end;
	}

	/* check if any modules loaded */
	if ((arg_check_modules || check_all) &&
	    !_check_loaded_modules(arg_detail)) {
		retval = 1;
		goto end;
	}

	/* check current settings */
	if ((arg_check_module_settings || check_all) &&
	    !_check_modules(arg_detail)) {
		retval = 1;
		goto end;
	}

  end:

	return retval;
}
