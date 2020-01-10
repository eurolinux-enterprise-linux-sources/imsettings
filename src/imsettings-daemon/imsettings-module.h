/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * imsettings-module.h
 * Copyright (C) 2010-2012 Red Hat, Inc. All rights reserved.
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
#ifndef __IMSETTINGS_IMSETTINGS_MODULE_H__
#define __IMSETTINGS_IMSETTINGS_MODULE_H__

#include <glib.h>
#include <glib-object.h>
#include <imsettings/imsettings-info.h>

G_BEGIN_DECLS

/**
 * IMSETTINGS_TYPE_MODULE:
 *
 * #GType for #IMSettingsModule.
 */
#define IMSETTINGS_TYPE_MODULE			(imsettings_module_get_type())
/**
 * IMSETTINGS_MODULE:
 * @object: Object which is subject to casting.
 *
 * Casts a #GObject or derived pointer into a (IMSettingsModule*) pointer.
 * Depending on the current debugging level, this function may invoke
 * certain runtime checks to identify invalid casts.
 */
#define IMSETTINGS_MODULE(_o_)			(G_TYPE_CHECK_INSTANCE_CAST ((_o_), IMSETTINGS_TYPE_MODULE, IMSettingsModule))
/**
 * IMSETTINGS_MODULE_CLASS:
 * @class: a valid #IMSettingsModuleClass
 *
 * Casts a derived #GObjectClass structure into a #IMSettingsModuleClass structure.
 */
#define IMSETTINGS_MODULE_CLASS(_c_)		(G_TYPE_CHECK_CLASS_CAST ((_c_), IMSETTINGS_TYPE_MODULE, IMSettingsModuleClass))
/**
 * IMSETTINGS_MODULE_GET_CLASS:
 * @object: a #IMSettingsModule instance.
 *
 * Get the class structure associated to a #IMSettingsModule instance.
 *
 * Returns: pointer to object class structure.
 */
#define IMSETTINGS_MODULE_GET_CLASS(_o_)	(G_TYPE_INSTANCE_GET_CLASS ((_o_), IMSETTINGS_TYPE_MODULE, IMSettingsModuleClass))
/**
 * IMSETTINGS_IS_MODULE:
 * @object: Instance to check for being a %IMSETTINGS_TYPE_MODULE.
 *
 * Checks whether a valid #GTypeInstance pointer is of type %IMSETTINGS_TYPE_MODULE.
 */
#define IMSETTINGS_IS_MODULE(_o_)		(G_TYPE_CHECK_INSTANCE_TYPE ((_o_), IMSETTINGS_TYPE_MODULE))
/**
 * IMSETTINGS_IS_MODULE_CLASS:
 * @class: a #IMSettingsModuleClass
 *
 * Checks whether @class "is a" valid #IMSettingsModuleClass structure of type
 * %IMSETTINGS_TYPE_MODULE or derived.
 */
#define IMSETTINGS_IS_MODULE_CLASS(_c_)		(G_TYPE_CHECK_CLASS_TYPE ((_c_), IMSETTINGS_TYPE_MODULE))


typedef struct _IMSettingsModuleClass		IMSettingsModuleClass;
typedef struct _IMSettingsModule		IMSettingsModule;
typedef struct _IMSettingsModulePrivate		IMSettingsModulePrivate;

typedef void    (* IMSettingsModuleSwitchFunc) (IMSettingsInfo *info);
typedef gchar * (* IMSettingsModuleDumpFunc)   (void);

/**
 * IMSettingsModuleClass:
 * @parent_class: The object class structure needs to be the first
 *   element in the imsettings module class structure in order for
 *   the class mechanism to work correctly.  This allows a
 *   IMSettingsModuleClass pointer to be cast to a GObjectClass
 *   pointer.
 */
struct _IMSettingsModuleClass {
	GObjectClass parent_class;
};
struct _IMSettingsModule {
	GObject                  parent_instance;
	IMSettingsModulePrivate *priv;
};


GType             imsettings_module_get_type  (void) G_GNUC_CONST;
IMSettingsModule *imsettings_module_new       (const gchar      *name);
const gchar      *imsettings_module_get_name  (IMSettingsModule *module);
gboolean          imsettings_module_load      (IMSettingsModule *module);
void              imsettings_module_switch_im (IMSettingsModule *module,
					       IMSettingsInfo   *info);
gchar            *imsettings_module_get_config(IMSettingsModule *module);

G_END_DECLS

#endif /* __IMSETTINGS_IMSETTINGS_MODULE_H__ */
