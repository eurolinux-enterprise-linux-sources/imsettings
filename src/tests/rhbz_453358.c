/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * rhbz_453358.c
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
#include <string.h>
#include <unistd.h>
#include "imsettings.h"
#include "imsettings-client.h"
#include "main.h"

IMSettingsClient *client;

/************************************************************/
/* common functions                                         */
/************************************************************/
void
setup(void)
{
	client = imsettings_client_new(NULL);
}

void
teardown(void)
{
	imsettings_test_reload_daemons();

	g_object_unref(client);
}

/************************************************************/
/* Test cases                                               */
/************************************************************/
TDEF (issue) {
	gchar *uim, *sim;
	gchar *p;
	GError *error = NULL;
	gchar *d, *tmpl = g_build_filename(g_get_tmp_dir(), "rhbz_453358.XXXXXX", NULL);
	gchar *dest = g_build_filename(IMSETTINGS_SRCDIR, "testcases", "rhbz_453358", "case1", "xinput.d", NULL);
	gchar *xinputd;
	gchar *conf, *rc;

	d = mkdtemp(tmpl);
	fail_unless(d != NULL, "Unable to create a temporary directory.");

	p = g_strdup_printf("cp -a %s %s", dest, d);
	if (!g_spawn_command_line_sync(p, NULL, NULL, NULL, &error))
		abort();
	g_free(p);
	p = g_strdup_printf("sh -c \"find %s -type d | xargs chmod u+w\"", d);
	if (!g_spawn_command_line_sync(p, NULL, NULL, NULL, &error))
		abort();
	g_free(p);

	xinputd = g_build_filename(d, "xinput.d", NULL);
	conf = g_build_filename(xinputd, "test-scim2.conf", NULL);
	rc = g_build_filename(d, "xinputrc", NULL);
	if (symlink(conf, rc) == -1)
		fail("Unable to create a symlink");
	
	imsettings_test_restart_daemons_full("rhbz_453358" G_DIR_SEPARATOR_S "case1",
					     xinputd, tmpl);
	uim = imsettings_client_get_user_im(client, NULL, &error);
	sim = imsettings_client_get_system_im(client, NULL, &error);
	fail_unless(strcmp(uim, "S C I M") == 0, "Failed to build the test environment.");
	fail_unless(strcmp(sim, "SCIM") == 0, "Failed to build the test environment.");

	unlink(conf);
	/* FIXME */
	sleep(5);

	g_free(uim);
	g_free(sim);

	uim = imsettings_client_get_user_im(client, NULL, &error);
	sim = imsettings_client_get_system_im(client, NULL, &error);
	fail_unless(strcmp(uim, sim) == 0, "User IM and System IM has to be the same: %s vs %s.",
		    uim, sim);

	p = g_strdup_printf("rm -rf %s", tmpl);
	if (!g_spawn_command_line_sync(p, NULL, NULL, NULL, &error))
		abort();

	g_free(xinputd);
	g_free(conf);
	g_free(rc);
	g_free(uim);
	g_free(sim);
	g_free(p);
	g_free(tmpl);
} TEND

/************************************************************/
Suite *
imsettings_suite(void)
{
	Suite *s = suite_create("Red Hat Bugzilla");
	TCase *tc = tcase_create("Bug#453358: https://bugzilla.redhat.com/show_bug.cgi?id=453358");

	tcase_add_checked_fixture(tc, setup, teardown);
	tcase_set_timeout(tc, 10);

	T (issue);

	suite_add_tcase(s, tc);

	return s;
}
