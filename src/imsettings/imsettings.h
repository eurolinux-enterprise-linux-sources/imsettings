/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * imsettings.h
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
#ifndef __IMSETTINGS_IMSETTINGS_H__
#define __IMSETTINGS_IMSETTINGS_H__

#include <glib.h>

G_BEGIN_DECLS

/**
 * SECTION:imsettings
 * @Short_Description: Macros in imsettings
 * @Title: Macros
 *
 * This section describes some macros used in imsettings.
 */

/**
 * IMSETTINGS_SERVICE_DBUS:
 *
 * A service name for imsettings used in DBus.
 */
#define IMSETTINGS_SERVICE_DBUS		"com.redhat.imsettings"
/**
 * IMSETTINGS_PATH_DBUS:
 *
 * A path name for imsettings used in DBus.
 */
#define IMSETTINGS_PATH_DBUS		"/com/redhat/imsettings"
/**
 * IMSETTINGS_INTERFACE_DBUS:
 *
 * An interface name for imsettings used in DBus.
 */
#define IMSETTINGS_INTERFACE_DBUS	"com.redhat.imsettings"
/**
 * IMSETTINGS_XIM_SERVICE_DBUS:
 *
 * A service name for imsettings-xim XIM server used in DBus.
 */
#define IMSETTINGS_XIM_SERVICE_DBUS	"com.redhat.imsettings.xim"
/**
 * IMSETTINGS_XIM_PATH_DBUS:
 *
 * A path name for imsettings-xim XIM server used in DBus.
 */
#define IMSETTINGS_XIM_PATH_DBUS	"/com/redhat/imsettings/xim"
/**
 * IMSETTINGS_XIM_INTERFACE_DBUS:
 *
 * An interface name for imsettings-xim XIM server used in DBus.
 */
#define IMSETTINGS_XIM_INTERFACE_DBUS	"com.redhat.imsettings.xim"

/**
 * IMSETTINGS_SETTINGS_API_VERSION:
 *
 * A DBus API version in imsettings.
 */
#define IMSETTINGS_SETTINGS_API_VERSION	5

/**
 * IMSETTINGS_GLOBAL_XINPUT_CONF:
 *
 * The global configuration filename.
 */
#define IMSETTINGS_GLOBAL_XINPUT_CONF		"xinputrc"
/**
 * IMSETTINGS_USER_XINPUT_CONF:
 *
 * The user configuration filename.
 */
#define IMSETTINGS_USER_XINPUT_CONF		"xinputrc"
/**
 * IMSETTINGS_NONE_CONF:
 *
 * The configuration filename used for "disabled".
 * The real filename would be %IMSETTINGS_NONE_CONF + something specified
 * with --xinput-suffix build option that the default value is ".conf".
 */
#define IMSETTINGS_NONE_CONF			"none"
/**
 * IMSETTINGS_XIM_CONF:
 *
 * The configuration filename used for XIM.
 * The real filename would be %IMSETTINGS_XIM_CONF + something specified
 * with --xinput-suffix build option that the default value is ".conf".
 */
#define IMSETTINGS_XIM_CONF			"xim"
/**
 * IMSETTINGS_USER_SPECIFIC_SHORT_DESC:
 *
 * A short description for the user specific xinput configuration file.
 * which is a non-symlink'd user xinputrc.
 */
#define IMSETTINGS_USER_SPECIFIC_SHORT_DESC	N_("User Specific")
/**
 * IMSETTINGS_USER_SPECIFIC_LONG_DESC:
 *
 * A long description for the user specific xinput configuration file.
 * which is a non-symlink'd user xinputrc.
 */
#define IMSETTINGS_USER_SPECIFIC_LONG_DESC	N_("xinputrc was modified by the user")

G_END_DECLS

#endif /* __IMSETTINGS_IMSETTINGS_H__ */
