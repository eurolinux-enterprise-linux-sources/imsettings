/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * imsettings-utils.h
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
#ifndef __IMSETTINGS_IMSETTINGS_UTILS_H__
#define __IMSETTINGS_IMSETTINGS_UTILS_H__

#include <glib.h>
#include <gio/gio.h>

G_BEGIN_DECLS

/**
 * IMSETTINGS_GERROR:
 *
 * Error domain for imsettings. Errors in this domain will be from
 * #IMSettingsError enumeration.  See #GError for information on
 * error domains.
 */
#define IMSETTINGS_GERROR	(imsettings_g_error_quark())

/**
 * IMSettingsError:
 * @IMSETTINGS_GERROR_UNKNOWN: unknown error happened.
 * @IMSETTINGS_GERROR_CONFIGURATION_ERROR: misconfigured.
 * @IMSETTINGS_GERROR_IM_NOT_FOUND: the Input Method in request doesn't found.
 * @IMSETTINGS_GERROR_UNABLE_TO_TRACK_IM: the Input Method isn't managed by imsettings.
 * @IMSETTINGS_GERROR_OOM: Out of memory occured.
 * @IMSETTINGS_GERROR_NOT_TARGETED_DESKTOP: current desktop isn't targeted by Input Method.
 *
 * Error codes used in imsettings.

 */
typedef enum {
	IMSETTINGS_GERROR_UNKNOWN,
	IMSETTINGS_GERROR_CONFIGURATION_ERROR,
	IMSETTINGS_GERROR_IM_NOT_FOUND,
	IMSETTINGS_GERROR_UNABLE_TO_TRACK_IM,
	IMSETTINGS_GERROR_OOM,
	IMSETTINGS_GERROR_NOT_TARGETED_DESKTOP,
} IMSettingsError;

GQuark              imsettings_g_error_quark     (void);
GDBusInterfaceInfo *imsettings_get_interface_info(void);
gboolean            imsettings_is_enabled        (void);

G_END_DECLS

#endif /* __IMSETTINGS_IMSETTINGS_UTILS_H__ */
