/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * main.h
 * Copyright (C) 2007-2012 Akira TAGOH
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
#ifndef __IMSETTINGS_TEST_MAIN_H__
#define __IMSETTINGS_TEST_MAIN_H__

#include <check.h>
#include <glib.h>

G_BEGIN_DECLS

#define IMSETTINGS_TEST_ERROR	imsettings_test_get_error_quark()
#define TDEF(fn)		START_TEST (test_ ## fn)
#define TEND			END_TEST
#define T(fn)			tcase_add_test(tc, test_ ## fn)
#define TNUL(obj)		fail_unless((obj) != NULL, "Failed to create an object")


void    setup                               (void);
void    teardown                            (void);
Suite  *imsettings_suite                    (void);
GQuark  imsettings_test_get_error_quark     (void);
gchar  *imsettings_test_pop_error           (void) G_GNUC_MALLOC;
void    imsettings_test_restart_daemons     (const gchar *subdir);
void    imsettings_test_reload_daemons      (void);
void    imsettings_test_restart_daemons_full(const gchar *xinputrcdir,
                                             const gchar *xinputdir,
                                             const gchar *homedir);


G_END_DECLS

#endif /* __IMSETTINGS_TEST_MAIN_H__ */
