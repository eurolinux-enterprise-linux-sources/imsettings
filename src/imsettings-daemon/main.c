/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * main.c
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

#include <stdlib.h>
#include <glib/gi18n.h>
#ifndef HAS_OLD_LIBNOTIFY
#include <libnotify/notify.h>
#endif
#include "imsettings-server.h"
#include "imsettings-utils.h"

static void _sig_handler(int signum);

static IMSettingsServer *server = NULL;
static gboolean running = TRUE;
static GQuark loop_in_object = 0;

/*< private >*/
static gboolean
_loop_cb(gpointer data)
{
	if (!running) {
		GMainLoop *loop = data;

		g_main_loop_quit(loop);
	}

	return FALSE;
}

static int
_setup_signal(int signum)
{
	struct sigaction sa;

	sa.sa_handler = _sig_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;

	return sigaction(signum, &sa, NULL);
}

static void
_sig_handler(int signum)
{
	GMainLoop *loop;

	if (!server) {
		g_print("\nReceived a signal %d. exiting...\n", signum);
		exit(1);
	}
	switch (signum) {
	    case SIGTERM:
	    case SIGINT:
		    running = FALSE;
		    loop = g_object_get_qdata(G_OBJECT (server),
					      loop_in_object);
		    g_idle_add(_loop_cb, loop);
		    _setup_signal(signum);
		    break;
	    default:
		    g_print("\nUnknown signal (%d) received. ignoring.\n", signum);
		    break;
	}
}

static void
_disconnected_cb(IMSettingsServer *server)
{
	GMainLoop *loop = g_object_get_qdata(G_OBJECT (server),
					     loop_in_object);

	running = FALSE;
	g_idle_add(_loop_cb, loop);
}

/*< public >*/
int
main(int argc, char **argv)
{
	GError *err = NULL;
	gboolean arg_replace = FALSE, arg_no_logfile = FALSE;
	gchar *arg_xinputrcdir = NULL, *arg_xinputdir = NULL, *arg_homedir = NULL, *arg_moduledir = NULL;
	GMainLoop *loop;
	GOptionContext *ctx = g_option_context_new(NULL);
	GOptionEntry entries[] = {
		{"replace", 0, 0, G_OPTION_ARG_NONE, &arg_replace, N_("Replace the instance of the imsettings daemon."), NULL},
		{"xinputrcdir", 0, G_OPTION_FLAG_HIDDEN, G_OPTION_ARG_STRING, &arg_xinputrcdir, N_("Set the system-wide xinputrc directory (for debugging purpose)"), N_("DIR")},
		{"xinputdir", 0, G_OPTION_FLAG_HIDDEN, G_OPTION_ARG_STRING, &arg_xinputdir, N_("Set the IM configuration directory (for debugging purpose)"), N_("DIR")},
		{"homedir", 0, G_OPTION_FLAG_HIDDEN, G_OPTION_ARG_STRING, &arg_homedir, N_("Set a home directory (for debugging purpose)"), N_("DIR")},
		{"moduledir", 0, G_OPTION_FLAG_HIDDEN, G_OPTION_ARG_STRING, &arg_moduledir, N_("Set the imsettings module directory (for debugging purpose)"), N_("DIR")},
		{"no-logfile", 0, G_OPTION_FLAG_HIDDEN, G_OPTION_ARG_NONE, &arg_no_logfile, N_("Do not create a log file."), NULL},
		{NULL, 0, 0, 0, NULL, NULL, NULL}
	};
	GDBusConnection *connection;

#ifdef ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, IMSETTINGS_LOCALEDIR);
#ifdef HAVE_BIND_TEXTDOMAIN_CODESET
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
#endif /* HAVE_BIND_TEXTDOMAIN_CODESET */
	textdomain (GETTEXT_PACKAGE);
#endif /* ENABLE_NLS */

#ifndef HAS_OLD_LIBNOTIFY
	notify_init("imsettings-daemon");
#else
	g_type_init();
#endif

	/* deal with the arguments */
	g_option_context_add_main_entries(ctx, entries, GETTEXT_PACKAGE);
	if (!g_option_context_parse(ctx, &argc, &argv, &err)) {
		if (err != NULL) {
			g_print("%s\n", err->message);
		} else {
			g_warning(_("Unknown error in parsing the command lines."));
		}
		exit(1);
	}
	g_option_context_free(ctx);

	connection = g_bus_get_sync(G_BUS_TYPE_SESSION,
				    NULL,
				    &err);
	if (err) {
		g_printerr("%s\n", err->message);
		exit(1);
	}

	g_dbus_error_register_error(IMSETTINGS_GERROR, 3, "com.redhat.imsettings.Error");

	_setup_signal(SIGTERM);
	_setup_signal(SIGINT);

	server = imsettings_server_new(connection,
				       arg_homedir,
				       arg_xinputrcdir,
				       arg_xinputdir,
				       arg_moduledir);
	if (!server) {
		g_printerr("Unable to create a server instance.\n");
		exit(1);
	}
	loop = g_main_loop_new(NULL, FALSE);

	g_signal_connect(server, "disconnected",
			 G_CALLBACK (_disconnected_cb),
			 NULL);

	g_object_set(G_OBJECT (server),
		     "logging", !arg_no_logfile,
		     NULL);
	loop_in_object = g_quark_from_static_string("imsettings-daemon-loop");
	g_object_set_qdata(G_OBJECT (server), loop_in_object, loop);

	imsettings_server_start(server, arg_replace);
	g_main_loop_run(loop);

	g_print("\nExiting...\n");

	g_object_unref(server);
	g_object_unref(connection);
	g_main_loop_unref(loop);

#ifndef HAS_OLD_LIBNOTIFY
	notify_uninit();
#endif

	/* invoking _exit(2) instead of just returning or invoking exit(2)
	 * to avoid segfault in a function added by atexit(3) in GConf.
	 * Since GConf is dlopen'd, the function pointer isn't a valid
	 * at atexit(2) anymore.
	 */
	_exit(0);
}
