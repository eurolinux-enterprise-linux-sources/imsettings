/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * imsettings-client.c
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

#include <glib/gi18n-lib.h>
#include "imsettings.h"
#include "imsettings-utils.h"
#include "imsettings-client.h"

#define IMSETTINGS_CLIENT_GET_PRIVATE(_o_)	(G_TYPE_INSTANCE_GET_PRIVATE ((_o_), IMSETTINGS_TYPE_CLIENT, IMSettingsClientPrivate))


/**
 * SECTION:imsettings-client
 * @Short_Description: convenient class for client to access imsettings-daemon.
 * @Title: IMSettingsClient
 *
 * IMSettingsClient provides interface methods to imsettings-daemon to obtain
 * a sort of informations for Input Method, operating something through
 * imsettings-daemon to Input Method.
 * This class is supposed to be the main entrance for all the client
 * applications.
 *
 * Either of functionalities in imsettings-daemon can be accessed through DBus
 * instead of this class though, results of them needs to be sorted out to
 * the appropriate types of values as needed.
 *
 * Please see documentation of each methods for more details of DBus call.
 */

G_DEFINE_TYPE (IMSettingsClient, imsettings_client, G_TYPE_OBJECT);

struct _IMSettingsClientPrivate {
	GDBusProxy *proxy;
	gchar      *locale;
};
enum {
	PROP_0,
	PROP_LOCALE,
	LAST_PROP
};

/*< private >*/
static GDBusProxy *
imsettings_client_get_proxy(IMSettingsClient *client)
{
	IMSettingsClientPrivate *priv = client->priv;
	GDBusConnection *connection;
	GError *err = NULL;

	if (priv->proxy) {
		connection = g_dbus_proxy_get_connection(priv->proxy);
		if (g_dbus_connection_is_closed(connection)) {
			g_object_unref(priv->proxy);
			goto create;
		}
	} else {
	  create:
		priv->proxy = g_dbus_proxy_new_for_bus_sync(G_BUS_TYPE_SESSION,
							    G_DBUS_PROXY_FLAGS_NONE,
							    imsettings_get_interface_info(),
							    IMSETTINGS_SERVICE_DBUS,
							    IMSETTINGS_PATH_DBUS,
							    IMSETTINGS_INTERFACE_DBUS,
							    NULL,
							    &err);
	}
	if (err) {
		g_warning("%s", err->message);
		g_error_free(err);
	}

	return priv->proxy;
}

static void
imsettings_client_set_property(GObject      *object,
			       guint         prop_id,
			       const GValue *value,
			       GParamSpec   *pspec)
{
	switch (prop_id) {
	    case PROP_LOCALE:
		    imsettings_client_set_locale(IMSETTINGS_CLIENT (object),
						 g_value_get_string(value));
		    break;
	    default:
		    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		    break;
	}
}

static void
imsettings_client_get_property(GObject    *object,
			       guint       prop_id,
			       GValue     *value,
			       GParamSpec *pspec)
{
	switch (prop_id) {
	    case PROP_LOCALE:
		    g_value_set_string(value,
				       imsettings_client_get_locale(IMSETTINGS_CLIENT (object)));
		    break;
	    default:
		    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		    break;
	}
}

static void
imsettings_client_finalize(GObject *object)
{
	IMSettingsClient *client = IMSETTINGS_CLIENT (object);
	IMSettingsClientPrivate *priv = client->priv;

	if (priv->proxy)
		g_object_unref(priv->proxy);
	g_free(priv->locale);

	if (G_OBJECT_CLASS (imsettings_client_parent_class)->finalize)
		G_OBJECT_CLASS (imsettings_client_parent_class)->finalize(object);
}

static void
imsettings_client_class_init(IMSettingsClientClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private(klass, sizeof (IMSettingsClientPrivate));

	object_class->set_property = imsettings_client_set_property;
	object_class->get_property = imsettings_client_get_property;
	object_class->finalize     = imsettings_client_finalize;

	/* properties */
	g_object_class_install_property(object_class, PROP_LOCALE,
					g_param_spec_string("locale",
							    _("Locale"),
							    _("Locale to get the imsettings information"),
							    NULL,
							    G_PARAM_READWRITE));
}

static void
imsettings_client_init(IMSettingsClient *client)
{
	IMSettingsClientPrivate *priv;

	priv = client->priv = IMSETTINGS_CLIENT_GET_PRIVATE (client);

	priv->proxy = NULL;
	priv->locale = NULL;
}

G_INLINE_FUNC gboolean
imsettings_client_async_result_boolean(IMSettingsClient  *client,
				       GAsyncResult      *result,
				       GError           **error)
{
	GDBusProxy *proxy;
	gboolean retval = FALSE;
	GVariant *value;

	g_return_val_if_fail (IMSETTINGS_IS_CLIENT (client), FALSE);
	g_return_val_if_fail (G_IS_ASYNC_RESULT (result), FALSE);

	proxy = imsettings_client_get_proxy(client);
	value = g_dbus_proxy_call_finish(proxy, result, error);
	if (value != NULL) {
		g_variant_get(value, "(b)", &retval);
		g_variant_unref(value);
	}

	return retval;
}

G_INLINE_FUNC gchar *
imsettings_client_async_result_string(IMSettingsClient  *client,
				      GAsyncResult      *result,
				      GError           **error)
{
	GDBusProxy *proxy;
	gchar *retval = NULL;
	GVariant *value;

	g_return_val_if_fail (IMSETTINGS_IS_CLIENT (client), NULL);
	g_return_val_if_fail (G_IS_ASYNC_RESULT (result), NULL);

	proxy = imsettings_client_get_proxy(client);
	value = g_dbus_proxy_call_finish(proxy, result, error);
	if (value != NULL) {
		g_variant_get(value, "(s)", &retval);
		g_variant_unref(value);
	}

	return retval;
}

G_INLINE_FUNC GVariant *
imsettings_client_async_result_variant(IMSettingsClient  *client,
				       GAsyncResult      *result,
				       GError           **error)
{
	GDBusProxy *proxy;
	GVariant *value, *retval = NULL;

	g_return_val_if_fail (IMSETTINGS_IS_CLIENT (client), NULL);
	g_return_val_if_fail (G_IS_ASYNC_RESULT (result), NULL);

	proxy = imsettings_client_get_proxy(client);
	value = g_dbus_proxy_call_finish(proxy, result, error);
	if (value != NULL) {
		retval = g_variant_get_child_value(value, 0);
		g_variant_unref(value);
	}

	return retval;
}

/*< public >*/

/**
 * imsettings_client_new:
 * @locale: a locale to obtain information for or to give it for Input Method.
 *          or %NULL if you want to pass current locale.
 *
 * Creates an instance of #IMSettingsClient.
 *
 * Returns: a #IMSettingsClient.
 */
IMSettingsClient *
imsettings_client_new(const gchar *locale)
{
	if (!imsettings_is_enabled())
		return NULL;

	return IMSETTINGS_CLIENT (g_object_new(IMSETTINGS_TYPE_CLIENT,
					       "locale", locale, NULL));
}

/**
 * imsettings_client_set_locale:
 * @client: a #IMSettingsClient.
 * @locale: a locale to obtain information for or to give it for Input Method.
 *          or %NULL if you want to pass current locale.
 *
 * Update the locale information in @client with @locale.
 *
 * Returns: %TRUE if successfully updated, otherwise %FALSE.
 */
gboolean
imsettings_client_set_locale(IMSettingsClient *client,
			     const gchar      *locale)
{
	IMSettingsClientPrivate *priv;

	g_return_val_if_fail (IMSETTINGS_IS_CLIENT (client), FALSE);

	priv = client->priv;
	if (locale) {
		gchar *cl = g_strdup(setlocale(LC_CTYPE, NULL));

		if (setlocale(LC_CTYPE, locale) == NULL) {
			g_free(cl);
			return FALSE;
		}

		setlocale(LC_CTYPE, cl);
		g_free(cl);
	}
	g_free(priv->locale);
	priv->locale = g_strdup(locale);

	g_object_notify(G_OBJECT (client), "locale");

	return TRUE;
}

/**
 * imsettings_client_get_locale:
 * @client: a #IMSettingsClient.
 *
 * Obtains current locale information in @client.
 *
 * Returns: a reference to the locale string in @client. it shouldn't be freed
 *          in applications.
 */
const gchar *
imsettings_client_get_locale(IMSettingsClient *client)
{
	IMSettingsClientPrivate *priv;

	g_return_val_if_fail (IMSETTINGS_IS_CLIENT (client), NULL);

	priv = client->priv;
	if (!priv->locale)
		return setlocale(LC_CTYPE, NULL);

	return priv->locale;
}

/**
 * imsettings_client_get_version:
 * @client: a #IMSettingsClient.
 * @cancellable: (allow-none): a #GCancellable or %NULL.
 * @error: (allow-none): a #GError to store an error if any, or %NULL.
 *
 * Obtains the version information of imsettings-daemon running.
 * This is expected to invoke at first if the return value is same to
 * %IMSETTINGS_SETTINGS_API_VERSION. otherwise it may not works as expected
 * due to the changes of the implementation between imsettings-daemon and
 * client APIs.
 *
 * You could access through DBus API instead:
 *
 * |[
 *   &lt;interface name='com.redhat.imsettings'&gt;
 *     &lt;method name='GetVersion'&gt;
 *       &lt;arg type='u' name='version' direction='out' /&gt;
 *     &lt;/method&gt;
 *   &lt;/interface&gt;
 * ]|
 *
 * Returns: a version number.
 */
guint
imsettings_client_get_version(IMSettingsClient  *client,
			      GCancellable      *cancellable,
			      GError           **error)
{
	GDBusProxy *proxy;
	guint retval = 0;
	GVariant *value;

	g_return_val_if_fail (IMSETTINGS_IS_CLIENT (client), 0);

	proxy = imsettings_client_get_proxy(client);
	value = g_dbus_proxy_call_sync(proxy,
				       "GetVersion",
				       NULL,
				       G_DBUS_CALL_FLAGS_NONE,
				       -1,
				       cancellable,
				       error);
	if (value != NULL) {
		g_variant_get(value, "(u)", &retval);
		g_variant_unref(value);
	}

	return retval;
}

/**
 * imsettings_client_get_info_variants:
 * @client: a #IMSettingsClient.
 * @cancellable: (allow-none): a #GCancellable or %NULL.
 * @error: (allow-none): a #GError to store an error if any, or %NULL.
 *
 * Obtains all of the Input Method information available on the system.
 *
 * You could access through DBus API instead:
 *
 * |[
 *   &lt;interface name='com.redhat.imsettings'&gt;
 *     &lt;method name='GetInfoVariants'&gt;
 *       &lt;arg type='s' name='lang' direction='in' /&gt;
 *       &lt;arg type='a{sv}' name='ret' direction='out' /&gt;
 *     &lt;/method&gt;
 *   &lt;/interface&gt;
 * ]|
 *
 * Returns: a #GVariant contains some pairs of an Input Method name and
 *          a #GVariant that can converts to #IMSettingsInfo through
 *          imsettings_info_new(). otherwise %NULL.
 */
GVariant *
imsettings_client_get_info_variants(IMSettingsClient  *client,
				    GCancellable      *cancellable,
				    GError           **error)
{
	GDBusProxy *proxy;
	GVariant *value, *retval = NULL;
	GError *err = NULL;

	g_return_val_if_fail (IMSETTINGS_IS_CLIENT (client), NULL);

	proxy = imsettings_client_get_proxy(client);
	value = g_dbus_proxy_call_sync(proxy,
				       "GetInfoVariants",
				       g_variant_new("(s)",
						     imsettings_client_get_locale(client)),
				       G_DBUS_CALL_FLAGS_NONE,
				       -1,
				       cancellable,
				       &err);
	if (value != NULL) {
		retval = g_variant_get_child_value(value, 0);
		g_variant_unref(value);
	}
	if (err) {
		if (error) {
			*error = g_error_copy(err);
		} else {
			g_warning("%s", err->message);
		}
		g_error_free(err);
	}

	return retval;
}

/**
 * imsettings_client_get_info_variants_start:
 * @client: a #IMSettingsClient.
 * @cancellable: (allow-none): a #GCancellable or %NULL.
 * @callback: (scope async): a #GAsyncReadyCallback.
 * @user_data: (closure): a pointer of the user data to give it to @callback.
 *
 * Request obtaining asynchronously all of the Input Method information
 * available on the system.
 */
void
imsettings_client_get_info_variants_start(IMSettingsClient    *client,
					  GCancellable        *cancellable,
					  GAsyncReadyCallback  callback,
					  gpointer             user_data)
{
	GDBusProxy *proxy;

	g_return_if_fail (IMSETTINGS_IS_CLIENT (client));

	proxy = imsettings_client_get_proxy(client);
	g_dbus_proxy_call(proxy,
			  "GetInfoVariants",
			  g_variant_new("(s)",
					imsettings_client_get_locale(client)),
			  G_DBUS_CALL_FLAGS_NONE,
			  -1,
			  cancellable,
			  callback,
			  user_data);
}

/**
 * imsettings_client_get_info_variants_finish:
 * @client: a #IMSettingsClient.
 * @result: a #GAsyncResult pushed through #GAsyncReadyCallback.
 * @error: (allow-none): a #GError to store an error if any, or %NULL.
 *
 * Obtains the result of the request from
 * imsettings_client_get_info_variants_start().
 *
 * Returns: (out) (transfer full) (allow-none): a #GVariant contains some pairs of an Input Method name and
 *          a #GVariant that can converts to #IMSettingsInfo through
 *          imsettings_info_new(). otherwise %NULL.
 */
gpointer
imsettings_client_get_info_variants_finish(IMSettingsClient  *client,
					   GAsyncResult      *result,
					   GError           **error)
{
	return imsettings_client_async_result_variant(client, result, error);
}

/**
 * imsettings_client_get_info_object:
 * @client: a #IMSettingsClient.
 * @module: an Input Method name to obtain the information.
 * @cancellable: (allow-none): a #GCancellable or %NULL.
 * @error: (allow-none): a #GError to store an error if any, or %NULL.
 *
 * A convenient function to get #IMSettingsInfo from the result of
 * imsettings_client_get_info_variant().
 *
 * Returns: (transfer full): a #IMSettingsInfo or %NULL.
 */
IMSettingsInfo *
imsettings_client_get_info_object(IMSettingsClient  *client,
				  const gchar       *module,
				  GCancellable      *cancellable,
				  GError           **error)
{
	IMSettingsInfo *retval = NULL;
	GVariant *v;

	v = imsettings_client_get_info_variant(client, module, cancellable, error);
	if (v) {
		retval = imsettings_info_new(v);
		g_variant_unref(v);
	}

	return retval;
}

/**
 * imsettings_client_get_info_variant:
 * @client: a #IMSettingsClient.
 * @module: an Input Method name to obtain the information.
 * @cancellable: (allow-none): a #GCancellable or %NULL.
 * @error: (allow-none): a #GError to store an error if any, or %NULL.
 *
 * Obtains the information for the specific Input Method in @module.
 *
 * You could access through DBus API instead:
 *
 * |[
 *   &lt;interface name='com.redhat.imsettings'&gt;
 *     &lt;method name='GetInfoVariant'&gt;
 *       &lt;arg type='s' name='lang' direction='in' /&gt;
 *       &lt;arg type='s' name='name' direction='in' /&gt;
 *       &lt;arg type='a{sv}' name='ret' direction='out' /&gt;
 *     &lt;/method&gt;
 *   &lt;/interface&gt;
 * ]|
 *
 * Returns: a #GVariant that can converts to #IMSettingsInfo through
 *          imsettings_info_new(). otherwise %NULL.
 */
GVariant *
imsettings_client_get_info_variant(IMSettingsClient  *client,
				   const gchar       *module,
				   GCancellable      *cancellable,
				   GError           **error)
{
	GDBusProxy *proxy;
	GVariant *value, *retval = NULL;
	GError *err = NULL;

	g_return_val_if_fail (IMSETTINGS_IS_CLIENT (client), NULL);

	proxy = imsettings_client_get_proxy(client);
	value = g_dbus_proxy_call_sync(proxy,
				       "GetInfoVariant",
				       g_variant_new("(ss)",
						     imsettings_client_get_locale(client),
						     module),
				       G_DBUS_CALL_FLAGS_NONE,
				       -1,
				       cancellable,
				       &err);
	if (value != NULL) {
		retval = g_variant_get_child_value(value, 0);
		g_variant_unref(value);
	}
	if (err) {
		if (error) {
			*error = g_error_copy(err);
		} else {
			g_warning("%s", err->message);
		}
		g_error_free(err);
	}

	return retval;
}

/**
 * imsettings_client_get_info_variant_start:
 * @client: a #IMSettingsClient.
 * @module: an Input Method name to obtain the information.
 * @cancellable: (allow-none): a #GCancellable or %NULL.
 * @callback: (scope async): a #GAsyncReadyCallback.
 * @user_data: (closure): a pointer of the user data to give it to @callback.
 *
 * Request obtaining asynchronously the Input Method information for @module.
 */
void
imsettings_client_get_info_variant_start(IMSettingsClient    *client,
					 const gchar         *module,
					 GCancellable        *cancellable,
					 GAsyncReadyCallback  callback,
					 gpointer             user_data)
{
	GDBusProxy *proxy;

	g_return_if_fail (IMSETTINGS_IS_CLIENT (client));

	proxy = imsettings_client_get_proxy(client);
	g_dbus_proxy_call(proxy,
			  "GetInfoVariant",
			  g_variant_new("(ss)",
					imsettings_client_get_locale(client),
					module),
			  G_DBUS_CALL_FLAGS_NONE,
			  -1,
			  cancellable,
			  callback,
			  user_data);
}

/**
 * imsettings_client_get_info_variant_finish:
 * @client: a #IMSettingsClient.
 * @result: a #GAsyncResult pushed through #GAsyncReadyCallback.
 * @error: (allow-none): a #GError to store an error if any, or %NULL.
 *
 * Obtains the result of the request from
 * imsettings_client_get_info_variant_start().
 *
 * Returns: a #GVariant that can converts to #IMSettingsInfo through
 *          imsettings_info_new(). otherwise %NULL.
 */
GVariant *
imsettings_client_get_info_variant_finish(IMSettingsClient  *client,
					  GAsyncResult      *result,
					  GError           **error)
{
	return imsettings_client_async_result_variant(client, result, error);
}

/**
 * imsettings_client_get_user_im:
 * @client: a #IMSettingsClient.
 * @cancellable: (allow-none): a #GCancellable or %NULL.
 * @error: (allow-none): a #GError to store an error if any, or %NULL.
 *
 * Obtains an Input Method name that currently is active for the user.
 * If one doesn't have the user xinputrc on their home, this simply returns the same
 * value to what imsettings_client_get_system_im() returns.
 *
 * You could access through DBus API instead:
 *
 * |[
 *   &lt;interface name='com.redhat.imsettings'&gt;
 *     &lt;method name='GetUserIM'&gt;
 *       &lt;arg type='s' name='lang' direction='in' /&gt;
 *       &lt;arg type='s' name='ret' direction='out'&gt;
 *         &lt;annotation name='org.freedesktop.DBus.GLib.Const' value='' /&gt;
 *       &lt;/arg&gt;
 *     &lt;/method&gt;
 *   &lt;/interface&gt;
 * ]|
 *
 * Returns: an Input Method name or %NULL.
 */
gchar *
imsettings_client_get_user_im(IMSettingsClient  *client,
			      GCancellable      *cancellable,
			      GError           **error)
{
	GDBusProxy *proxy;
	GVariant *value;
	gchar *retval = NULL;
	GError *err = NULL;

	g_return_val_if_fail (IMSETTINGS_IS_CLIENT (client), NULL);

	proxy = imsettings_client_get_proxy(client);
	value = g_dbus_proxy_call_sync(proxy,
				       "GetUserIM",
				       g_variant_new("(s)",
						     imsettings_client_get_locale(client)),
				       G_DBUS_CALL_FLAGS_NONE,
				       -1,
				       cancellable,
				       &err);
	if (value != NULL) {
		g_variant_get(value, "(s)", &retval);
		g_variant_unref(value);
	}
	if (err) {
		if (error) {
			*error = g_error_copy(err);
		} else {
			g_warning("%s", err->message);
		}
		g_error_free(err);
	}

	return retval;
}

/**
 * imsettings_client_get_user_im_start:
 * @client: a #IMSettingsClient.
 * @cancellable: (allow-none): a #GCancellable or %NULL.
 * @callback: (scope async): a #GAsyncReadyCallback.
 * @user_data: (closure): a pointer of the user data to give it to @callback.
 *
 * Request obtaining asynchronously an input method name that currently is
 * active for the user.
 */
void
imsettings_client_get_user_im_start(IMSettingsClient    *client,
				    GCancellable        *cancellable,
				    GAsyncReadyCallback  callback,
				    gpointer             user_data)
{
	GDBusProxy *proxy;

	g_return_if_fail (IMSETTINGS_IS_CLIENT (client));

	proxy = imsettings_client_get_proxy(client);
	g_dbus_proxy_call(proxy,
			  "GetUserIM",
			  g_variant_new("(s)",
					imsettings_client_get_locale(client)),
			  G_DBUS_CALL_FLAGS_NONE,
			  -1,
			  cancellable,
			  callback,
			  user_data);
}

/**
 * imsettings_client_get_user_im_finish:
 * @client: a #IMSettingsClient.
 * @result: a #GAsyncResult pushed through #GAsyncReadyCallback.
 * @error: (allow-none): a #GError to store an error if any, or %NULL.
 *
 * Obtains the result of the request from
 * imsettings_client_get_user_im_start().
 *
 * Returns: an Input Method name or %NULL.
 */
gchar *
imsettings_client_get_user_im_finish(IMSettingsClient  *client,
				     GAsyncResult      *result,
				     GError           **error)
{
	return imsettings_client_async_result_string(client, result, error);
}

/**
 * imsettings_client_get_system_im:
 * @client: a #IMSettingsClient.
 * @cancellable: (allow-none): a #GCancellable or %NULL.
 * @error: (allow-none): a #GError to store an error if any, or %NULL.
 *
 * Obtains an Input Method name that currently is active for the system-wide.
 *
 * You could access through DBus API instead:
 *
 * |[
 *   &lt;interface name='com.redhat.imsettings'&gt;
 *     &lt;method name='GetSystemIM'&gt;
 *       &lt;arg type='s' name='lang' direction='in' /&gt;
 *       &lt;arg type='s' name='ret' direction='out'&gt;
 *         &lt;annotation name='org.freedesktop.DBus.GLib.Const' value='' /&gt;
 *       &lt;/arg&gt;
 *     &lt;/method&gt;
 *   &lt;/interface&gt;
 * ]|
 *
 * Returns: an Input Method name or %NULL.
 */
gchar *
imsettings_client_get_system_im(IMSettingsClient  *client,
				GCancellable      *cancellable,
				GError           **error)
{
	GDBusProxy *proxy;
	GVariant *value;
	gchar *retval = NULL;

	g_return_val_if_fail (IMSETTINGS_IS_CLIENT (client), NULL);

	proxy = imsettings_client_get_proxy(client);
	value = g_dbus_proxy_call_sync(proxy,
				       "GetSystemIM",
				       g_variant_new("(s)",
						     imsettings_client_get_locale(client)),
				       G_DBUS_CALL_FLAGS_NONE,
				       -1,
				       cancellable,
				       error);
	if (value != NULL) {
		g_variant_get(value, "(s)", &retval);
		g_variant_unref(value);
	}

	return retval;
}

/**
 * imsettings_client_get_system_im_start:
 * @client: a #IMSettingsClient.
 * @cancellable: (allow-none): a #GCancellable or %NULL.
 * @callback: (scope async): a #GAsyncReadyCallback.
 * @user_data: (closure): a pointer of the user data to give it to @callback.
 *
 * Request obtaining asynchronously an input method name that currently is
 * active for the system-wide.
 */
void
imsettings_client_get_system_im_start(IMSettingsClient    *client,
				      GCancellable        *cancellable,
				      GAsyncReadyCallback  callback,
				      gpointer             user_data)
{
	GDBusProxy *proxy;

	g_return_if_fail (IMSETTINGS_IS_CLIENT (client));

	proxy = imsettings_client_get_proxy(client);
	g_dbus_proxy_call(proxy,
			  "GetSystemIM",
			  g_variant_new("(s)",
					imsettings_client_get_locale(client)),
			  G_DBUS_CALL_FLAGS_NONE,
			  -1,
			  cancellable,
			  callback,
			  user_data);
}

/**
 * imsettings_client_get_system_im_finish:
 * @client: a #IMSettingsClient.
 * @result: a #GAsyncResult pushed through #GAsyncReadyCallback.
 * @error: (allow-none): a #GError to store an error if any, or %NULL.
 *
 * Obtains the result of the request from
 * imsettings_client_get_system_im_start().
 *
 * Returns: an Input Method name or %NULL.
 */
gchar *
imsettings_client_get_system_im_finish(IMSettingsClient  *client,
				       GAsyncResult      *result,
				       GError           **error)
{
	return imsettings_client_async_result_string(client, result, error);
}

/**
 * imsettings_client_switch_im:
 * @client: a #IMSettingsClient.
 * @module: an Input Method name changing to, or %NULL to disable
 *          the Input Method.
 * @update_xinputrc: %TRUE to update the user xinputrc, otherwise %FALSE.
 * @cancellable: (allow-none): a #GCancellable or %NULL.
 * @error: (allow-none): a #GError to store an error if any, or %NULL.
 *
 * Changes the Input Method to @module.
 *
 * You could access through DBus API instead:
 *
 * |[
 *   &lt;interface name='com.redhat.imsettings'&gt;
 *     &lt;method name='SwitchIM'&gt;
 *       &lt;arg type='s' name='lang' direction='in' /&gt;
 *       &lt;arg type='s' name='module' direction='in' /&gt;
 *       &lt;arg type='b' name='update_xinputrc' direction='in' /&gt;
 *       &lt;arg type='b' name='ret' direction='out' /&gt;
 *     &lt;/method&gt;
 *   &lt;/interface&gt;
 * ]|
 *
 * Returns: if the operation is successfully done, returns %TRUE
 *          otherwise %FALSE.
 */
gboolean
imsettings_client_switch_im(IMSettingsClient  *client,
			    const gchar       *module,
			    gboolean           update_xinputrc,
			    GCancellable      *cancellable,
			    GError           **error)
{
	GDBusProxy *proxy;
	gboolean retval = FALSE;
	GVariant *value;
	gchar *m;

	g_return_val_if_fail (IMSETTINGS_IS_CLIENT (client), FALSE);

	if (module == NULL || module[0] == 0)
		m = g_strdup(IMSETTINGS_NONE_CONF);
	else
		m = g_strdup(module);

	proxy = imsettings_client_get_proxy(client);
	value = g_dbus_proxy_call_sync(proxy,
				       "SwitchIM",
				       g_variant_new("(ssb)",
						     imsettings_client_get_locale(client),
						     m, update_xinputrc),
				       G_DBUS_CALL_FLAGS_NONE,
				       -1,
				       cancellable,
				       error);
	if (value != NULL) {
		g_variant_get(value, "(b)", &retval);
		g_variant_unref(value);
	}
	g_free(m);

	return retval;

}

/**
 * imsettings_client_switch_im_start:
 * @client: a #IMSettingsClient.
 * @module: an Input Method name changing to, or %NULL to disable
 *          the Input Method.
 * @update_xinputrc: %TRUE to update the user xinputrc, otherwise %FALSE.
 * @cancellable: (allow-none): a #GCancellable or %NULL.
 * @callback: (scope async): a #GAsyncReadyCallback.
 * @user_data: (closure): a pointer of the user data to give it to @callback.
 *
 * Request changing asynchronously the Input Method to @module.
 */
void
imsettings_client_switch_im_start(IMSettingsClient    *client,
				  const gchar         *module,
				  gboolean             update_xinputrc,
				  GCancellable        *cancellable,
				  GAsyncReadyCallback  callback,
				  gpointer             user_data)
{
	GDBusProxy *proxy;
	gchar *m;

	g_return_if_fail (IMSETTINGS_IS_CLIENT (client));

	if (module == NULL || module[0] == 0)
		m = g_strdup(IMSETTINGS_NONE_CONF);
	else
		m = g_strdup(module);

	proxy = imsettings_client_get_proxy(client);
	g_dbus_proxy_call(proxy,
			  "SwitchIM",
			  g_variant_new("(ssb)",
					imsettings_client_get_locale(client),
					m, update_xinputrc),
			  G_DBUS_CALL_FLAGS_NONE,
			  -1,
			  cancellable,
			  callback,
			  user_data);
	g_free(m);
}

/**
 * imsettings_client_switch_im_finish:
 * @client: a #IMSettingsClient.
 * @result: a #GAsyncResult pushed through #GAsyncReadyCallback.
 * @error: (allow-none): a #GError to store an error if any, or %NULL.
 *
 * Obtains the result of the request from
 * imsettings_client_switch_im_start().
 *
 * Returns: if the operation is successfully done, returns %TRUE
 *          otherwise %FALSE.
 */
gboolean
imsettings_client_switch_im_finish(IMSettingsClient  *client,
				   GAsyncResult      *result,
				   GError           **error)
{
	return imsettings_client_async_result_boolean(client, result, error);
}

/**
 * imsettings_client_get_active_im_info:
 * @client: a #IMSettingsClient.
 * @cancellable: (allow-none): a #GCancellable or %NULL.
 * @error: (allow-none): a #GError to store an error if any, or %NULL.
 *
 * Obtains the Input Method information that is currently running on.
 *
 * You could access through DBus API instead:
 *
 * |[
 *   &lt;interface name='com.redhat.imsettings'&gt;
 *     &lt;method name='GetActiveVariant'&gt;
 *       &lt;arg type='a{sv}' name='ret' direction='out' /&gt;
 *     &lt;/method&gt;
 *   &lt;/interface&gt;
 * ]|
 *
 * Returns: (transfer full): a #IMSettingsInfo for active Input Method. if not, %NULL then.
 */
IMSettingsInfo *
imsettings_client_get_active_im_info(IMSettingsClient  *client,
				     GCancellable      *cancellable,
				     GError           **error)
{
	GDBusProxy *proxy;
	IMSettingsInfo *retval = NULL;
	GVariant *value, *v;

	g_return_val_if_fail (IMSETTINGS_IS_CLIENT (client), NULL);

	proxy = imsettings_client_get_proxy(client);
	value = g_dbus_proxy_call_sync(proxy,
				       "GetActiveVariant",
				       NULL,
				       G_DBUS_CALL_FLAGS_NONE,
				       -1,
				       cancellable,
				       error);
	if (value != NULL) {
		v = g_variant_get_child_value(value, 0);
		g_variant_unref(value);
		retval = imsettings_info_new(v);
	}

	return retval;
}

/**
 * imsettings_client_im_is_system_default:
 * @client: a #IMSettingsClient.
 * @module: an Input Method name to ask.
 * @cancellable: (allow-none): a #GCancellable or %NULL.
 * @error: (allow-none): a #GError to store an error if any, or %NULL.
 *
 * Checks whether @module is the system default or not.
 *
 * You could access through DBus API instead:
 *
 * |[
 *   &lt;interface name='com.redhat.imsettings'&gt;
 *     &lt;method name='IsSystemDefault'&gt;
 *       &lt;arg type='s' name='lang' direction='in' /&gt;
 *       &lt;arg type='s' name='imname' direction='in' /&gt;
 *       &lt;arg type='b' name='ret' direction='out' /&gt;
 *     &lt;/method&gt;
 *   &lt;/interface&gt;
 * ]|
 *
 * Returns: %TRUE if it is, otherwise %FALSE.
 */
gboolean
imsettings_client_im_is_system_default(IMSettingsClient  *client,
				       const gchar       *module,
				       GCancellable      *cancellable,
				       GError           **error)
{
	GDBusProxy *proxy;
	gboolean retval = FALSE;
	GVariant *value;

	g_return_val_if_fail (IMSETTINGS_IS_CLIENT (client), FALSE);
	g_return_val_if_fail (module != NULL, FALSE);

	proxy = imsettings_client_get_proxy(client);
	value = g_dbus_proxy_call_sync(proxy,
				       "IsSystemDefault",
				       g_variant_new("(ss)",
						     imsettings_client_get_locale(client),
						     module),
				       G_DBUS_CALL_FLAGS_NONE,
				       -1,
				       cancellable,
				       error);
	if (value != NULL) {
		g_variant_get(value, "(b)", &retval);
		g_variant_unref(value);
	}

	return retval;
}

/**
 * imsettings_client_im_is_user_default:
 * @client: a #IMSettingsClient.
 * @module: an Input Method name to ask.
 * @cancellable: (allow-none): a #GCancellable or %NULL.
 * @error: (allow-none): a #GError to store an error if any, or %NULL.
 *
 * Checks whether @module is the user default or not.
 *
 * You could access through DBus API instead:
 *
 * |[
 *   &lt;interface name='com.redhat.imsettings'&gt;
 *     &lt;method name='IsUserDefault'&gt;
 *       &lt;arg type='s' name='lang' direction='in' /&gt;
 *       &lt;arg type='s' name='imname' direction='in' /&gt;
 *       &lt;arg type='b' name='ret' direction='out' /&gt;
 *     &lt;/method&gt;
 *   &lt;/interface&gt;
 * ]|
 *
 * Returns: %TRUE if it is, otherwise %FALSE.
 */
gboolean
imsettings_client_im_is_user_default(IMSettingsClient  *client,
				     const gchar       *module,
				     GCancellable      *cancellable,
				     GError           **error)
{
	GDBusProxy *proxy;
	gboolean retval = FALSE;
	GVariant *value;

	g_return_val_if_fail (IMSETTINGS_IS_CLIENT (client), FALSE);
	g_return_val_if_fail (module != NULL, FALSE);

	proxy = imsettings_client_get_proxy(client);
	value = g_dbus_proxy_call_sync(proxy,
				       "IsUserDefault",
				       g_variant_new("(ss)",
						     imsettings_client_get_locale(client),
						     module),
				       G_DBUS_CALL_FLAGS_NONE,
				       -1,
				       cancellable,
				       error);
	if (value != NULL) {
		g_variant_get(value, "(b)", &retval);
		g_variant_unref(value);
	}

	return retval;
}

/**
 * imsettings_client_im_is_xim:
 * @client: a #IMSettingsClient.
 * @module: an Input Method name to ask.
 * @cancellable: (allow-none): a #GCancellable or %NULL.
 * @error: (allow-none): a #GError to store an error if any, or %NULL.
 *
 * Checks whether @module is the Input Method for XIM only or not.
 *
 * You could access through DBus API instead:
 *
 * |[
 *   &lt;interface name='com.redhat.imsettings'&gt;
 *     &lt;method name='IsXIM'&gt;
 *       &lt;arg type='s' name='lang' direction='in' /&gt;
 *       &lt;arg type='s' name='imname' direction='in' /&gt;
 *       &lt;arg type='b' name='ret' direction='out' /&gt;
 *     &lt;/method&gt;
 *   &lt;/interface&gt;
 * ]|
 *
 * Returns: %TRUE if it is, otherwise %FALSE.
 */
gboolean
imsettings_client_im_is_xim(IMSettingsClient  *client,
			    const gchar       *module,
			    GCancellable      *cancellable,
			    GError           **error)
{
	GDBusProxy *proxy;
	gboolean retval = FALSE;
	GVariant *value;

	g_return_val_if_fail (IMSETTINGS_IS_CLIENT (client), FALSE);
	g_return_val_if_fail (module != NULL, FALSE);

	proxy = imsettings_client_get_proxy(client);
	value = g_dbus_proxy_call_sync(proxy,
				       "IsXIM",
				       g_variant_new("(ss)",
						     imsettings_client_get_locale(client),
						     module),
				       G_DBUS_CALL_FLAGS_NONE,
				       -1,
				       cancellable,
				       error);
	if (value != NULL) {
		g_variant_get(value, "(b)", &retval);
		g_variant_unref(value);
	}

	return retval;
}

/**
 * imsettings_client_reload:
 * @client: a #IMSettingsClient.
 * @send_signal: %TRUE to send a signal instead of invoking a method.
 * @cancellable: (allow-none): a #GCancellable or %NULL.
 * @error: (allow-none): a #GError to store an error if any, or %NULL.
 *
 * Reloads imsettings-daemon.
 *
 * Note that @send_signal option is for the backward compatibility.
 * imsettings-daemon doesn't do anything since %IMSETTINGS_SETTINGS_API_VERSION
 * is 4. so you will get the expected behavior with:
 *
 * |[
 *   int api_version;
 *
 *   if ((api_version = imsettings_client_get_version(client)) != IMSETTINGS_SETTINGS_API_VERSION) {
 *       imsettings_client_reload(client, api_version < 4, NULL, &error);
 *   }
 * ]|
 *
 * You could access through DBus API instead:
 *
 * |[
 *   &lt;interface name='com.redhat.imsettings'&gt;
 *     &lt;signal name='Reload'&gt;
 *       &lt;arg type='b' name='ret' direction='out' /&gt;
 *     &lt;/signal&gt;
 *     &lt;method name='StopService'&gt;
 *       &lt;arg type='b' name='ret' direction='out' /&gt;
 *     &lt;/method&gt;
 *   &lt;/interface&gt;
 * ]|
 *
 * Returns: %TRUE if the operation is successfully done, otherwise %FALSE.
 */
gboolean
imsettings_client_reload(IMSettingsClient  *client,
			 gboolean           send_signal,
			 GCancellable      *cancellable,
			 GError           **error)
{
	GDBusProxy *proxy;

	g_return_val_if_fail (IMSETTINGS_IS_CLIENT (client), FALSE);

	proxy = imsettings_client_get_proxy(client);
	if (send_signal) {
		GDBusConnection *connection;

		g_clear_error(error);
		/* try to send a signal only. */
		connection = g_dbus_proxy_get_connection(proxy);
		if (!g_dbus_connection_emit_signal(connection,
						   IMSETTINGS_SERVICE_DBUS,
						   IMSETTINGS_PATH_DBUS,
						   IMSETTINGS_INTERFACE_DBUS,
						   "Reload",
						   g_variant_new("(b)", TRUE),
						   error))
			return FALSE;
		if (!g_dbus_connection_emit_signal(connection,
						   "com.redhat.imsettings.GConf",
						   "/com/redhat/imsettings/GConf",
						   "com.redhat.imsettings.GConf",
						   "Reload",
						   g_variant_new("(b)", TRUE),
						   error))
			return FALSE;
		sleep(3);
	} else {
		GVariant *value;
		gboolean retval = FALSE;

		value = g_dbus_proxy_call_sync(proxy,
					       "StopService", NULL,
					       G_DBUS_CALL_FLAGS_NONE,
					       -1,
					       cancellable,
					       error);
		if (value != NULL) {
			g_variant_get(value, "(b)", &retval);
			g_variant_unref(value);
		}
		return retval;
	}

	return TRUE;
}

/**
 * imsettings_client_get_module_settings:
 * @client: a #IMSettingsClient.
 * @cancellable: (allow-none): a #GCancellable or %NULL.
 * @error: (allow-none): a #GError to store an error, or %NULL.
 *
 * Obtains current Input Method settings in the backend modules.
 *
 * You could access through DBus API instead:
 *
 * |[
 *   &lt;interface name='com.redhat.imsettings'&gt;
 *     &lt;method name='DumpModuleSettings'&gt;
 *       &lt;arg type='a{ss}' name='ret' direction='out' /&gt;
 *     &lt;/method&gt;
 *   &lt;/interface&gt;
 * ]|
 *
 * Returns: a #GVariant that contains some pairs of the module name and
 *          the Input Method name.
 */
GVariant *
imsettings_client_get_module_settings(IMSettingsClient  *client,
				      GCancellable      *cancellable,
				      GError           **error)
{
	GDBusProxy *proxy;
	GVariant *value, *retval = NULL;
	GError *err = NULL;

	g_return_val_if_fail (IMSETTINGS_IS_CLIENT (client), NULL);

	proxy = imsettings_client_get_proxy(client);
	value = g_dbus_proxy_call_sync(proxy,
				       "DumpModuleSettings",
				       NULL,
				       G_DBUS_CALL_FLAGS_NONE,
				       -1,
				       cancellable,
				       &err);
	if (value != NULL) {
		retval = g_variant_get_child_value(value, 0);
		g_variant_unref(value);
	}
	if (err) {
		if (error) {
			*error = g_error_copy(err);
		} else {
			g_warning("%s", err->message);
		}
		g_error_free(err);
	}

	return retval;
}

/**
 * imsettings_client_ping:
 * @client: a #IMSettingsClient.
 *
 * Check whether the process is running.
 *
 * Returns: %TRUE if imsettings-daemon is running. otherwise %FALSE.
 */
gboolean
imsettings_client_ping(IMSettingsClient *client)
{
	GDBusProxy *proxy;
	GDBusConnection *connection;
	GVariant *value;
	GError *err = NULL;

	g_return_val_if_fail (IMSETTINGS_IS_CLIENT (client), FALSE);

	proxy = imsettings_client_get_proxy(client);
	connection = g_dbus_proxy_get_connection(proxy);
	value = g_dbus_connection_call_sync(connection,
					    IMSETTINGS_SERVICE_DBUS,
					    IMSETTINGS_PATH_DBUS,
					    "org.freedesktop.DBus.Peer",
					    "Ping",
					    NULL,
					    NULL,
					    G_DBUS_CALL_FLAGS_NO_AUTO_START,
					    -1,
					    NULL,
					    &err);
	if (value != NULL || !err) {
		if (value)
			g_variant_unref(value);
		return TRUE;
	}
	g_error_free(err);

	return FALSE;
}
