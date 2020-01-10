/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * gcit_11.c
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
#include "imsettings.h"
#include "imsettings-info.h"
#include "imsettings-client.h"
#include "main.h"

IMSettingsClient *client;

/************************************************************/
/* common functions                                         */
/************************************************************/
void
setup(void)
{
	imsettings_test_restart_daemons("issue_11");

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
	GVariant *v;
	GError *error = NULL;
	gchar *suffix = XINPUT_SUFFIX;

	/* XXX: testcase needs to be renamed dynamically? */
	if (suffix[0] != 0 && strcmp(suffix, ".conf_") != 0) {
		v = imsettings_client_get_info_variant(client, "SCIM", NULL, &error);
		fail_unless(v == NULL, "the info object shouldn't be get for the config without %s suffix.", suffix);
	}
} TEND

/************************************************************/
Suite *
imsettings_suite(void)
{
	Suite *s = suite_create("Google Code Issue Tracker");
	TCase *tc = tcase_create("Issue #11: http://code.google.com/p/imsettings/issues/detail?id=11");

	tcase_add_checked_fixture(tc, setup, teardown);

	T (issue);

	suite_add_tcase(s, tc);

	return s;
}
