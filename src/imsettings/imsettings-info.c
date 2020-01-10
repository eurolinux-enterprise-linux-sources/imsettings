/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * imsettings-info.c
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

#include <ctype.h>
#include <stdio.h>
#include <sys/stat.h>
#include <glib/gi18n-lib.h>
#include "imsettings.h"
#include "imsettings-client.h"
#include "imsettings-info.h"

#define IMSETTINGS_INFO_GET_PRIVATE(_o_)	(G_TYPE_INSTANCE_GET_PRIVATE ((_o_), IMSETTINGS_TYPE_INFO, IMSettingsInfoPrivate))


/**
 * SECTION:imsettings-info
 * @Short_Description: accessor class to the Input Method information.
 * @Title: IMSettingsInfo
 *
 * IMSettingsInfo provides interfaces to access the Input Method information
 * provided by the xinput configuration file.
 *
 * Please note that this simply holds the static snapshot information when
 * one requests imsettings-daemon to obtain. that may be obsoletes if keeping
 * an instance a long time.
 */

typedef enum _IMSettingsInfoType {
	IMSETTINGS_INFO_GTK_IM_MODULE = 0,
	IMSETTINGS_INFO_QT_IM_MODULE,
	IMSETTINGS_INFO_XIM,
	IMSETTINGS_INFO_IMSETTINGS_IGNORE_ME,
	IMSETTINGS_INFO_XIM_PROGRAM,
	IMSETTINGS_INFO_XIM_ARGS,
	IMSETTINGS_INFO_PREFERENCE_PROGRAM,
	IMSETTINGS_INFO_PREFERENCE_ARGS,
	IMSETTINGS_INFO_AUXILIARY_PROGRAM,
	IMSETTINGS_INFO_AUXILIARY_ARGS,
	IMSETTINGS_INFO_SHORT_DESC,
	IMSETTINGS_INFO_LONG_DESC,
	IMSETTINGS_INFO_ICON,
	IMSETTINGS_INFO_IMSETTINGS_IS_SCRIPT,
	IMSETTINGS_INFO_LANG,
	IMSETTINGS_INFO_FILENAME,
	IMSETTINGS_INFO_IS_XIM,
	IMSETTINGS_INFO_IM_NAME,
	IMSETTINGS_INFO_SUB_IM_NAME,
	IMSETTINGS_INFO_NOT_RUN,
	LAST_IMSETTINGS_INFO
} IMSettingsInfoType;

struct _IMSettingsInfoPrivate {
	GHashTable *info_table;
};

static GQuark __xinput_tokens[LAST_IMSETTINGS_INFO] = { 0 };

G_DEFINE_TYPE (IMSettingsInfo, imsettings_info, G_TYPE_OBJECT);
G_LOCK_DEFINE_STATIC (info);

/*< private >*/
G_INLINE_FUNC const gchar *
_skip_blanks(const gchar *s)
{
	while (*s && isspace(*s))
		s++;

	return s;
}

G_INLINE_FUNC const gchar *
_skip_tokens(const gchar *s)
{
	while (*s && !isspace(*s))
		s++;

	return s;
}

G_INLINE_FUNC gchar *
_unquote_string(const gchar *str)
{
	gboolean dq = FALSE, sq = FALSE;
	const gchar *p;
	GString *retval = g_string_new(NULL);

	for (p = str; *p != 0 && (!isspace(*p) || dq || sq); p++) {
		if (*p == '"' && !sq)
			dq = !dq;
		else if (*p == '\'' && !dq)
			sq = !sq;
		else if (*p == '\\' && sq == 0) {
			switch (*(p + 1)) {
			    case '"':
			    case '\'':
			    case '\\':
				    g_string_append_c(retval, *(p + 1));
				    p++;
				    break;
			    case 'b':
				    g_string_append_c(retval, '\b');
				    p++;
				    break;
			    case 'f':
				    g_string_append_c(retval, '\f');
				    p++;
				    break;
			    case 'n':
				    g_string_append_c(retval, '\n');
				    p++;
				    break;
			    case 'r':
				    g_string_append_c(retval, '\r');
				    p++;
				    break;
			    case 't':
				    g_string_append_c(retval, '\t');
				    p++;
				    break;
			    default:
				    g_string_append_c(retval, *p);
				    break;
			}
		} else {
			g_string_append_c(retval, *p);
		}
	}

	return g_string_free(retval, FALSE);
}

G_INLINE_FUNC gboolean
_parse_param(const gchar  *s,
	     gchar       **key,
	     gchar       **val)
{
	const gchar *p;
	gsize len;

	s = p = _skip_blanks(s);
	while (*s != 0) {
		if (*s == '=') {
			len = s - p;
			*key = g_malloc(len + 1);
			memcpy(*key, p, len);
			(*key)[len] = 0;
			s++;
			break;
		} else if (isspace(*s)) {
			return FALSE;
		}
		s++;
	}
	if (*s == 0)
		return FALSE;
	*val = _unquote_string(s);

	return TRUE;
}

static void
_quark_init(void)
{
	if (__xinput_tokens[0] == 0) {
#define TOKEN2QUARK(_sym_)						\
		__xinput_tokens[IMSETTINGS_INFO_ ## _sym_] = g_quark_from_static_string(# _sym_)

		TOKEN2QUARK (GTK_IM_MODULE);
		TOKEN2QUARK (QT_IM_MODULE);
		TOKEN2QUARK (XIM);
		TOKEN2QUARK (IMSETTINGS_IGNORE_ME);
		TOKEN2QUARK (XIM_PROGRAM);
		TOKEN2QUARK (XIM_ARGS);
		TOKEN2QUARK (PREFERENCE_PROGRAM);
		TOKEN2QUARK (PREFERENCE_ARGS);
		TOKEN2QUARK (AUXILIARY_PROGRAM);
		TOKEN2QUARK (AUXILIARY_ARGS);
		TOKEN2QUARK (SHORT_DESC);
		TOKEN2QUARK (LONG_DESC);
		TOKEN2QUARK (ICON);
		TOKEN2QUARK (IMSETTINGS_IS_SCRIPT);
		TOKEN2QUARK (IS_XIM);
		TOKEN2QUARK (LANG);
		TOKEN2QUARK (FILENAME);
		TOKEN2QUARK (IM_NAME);
		TOKEN2QUARK (SUB_IM_NAME);
		TOKEN2QUARK (NOT_RUN);

#undef TOKEN2QUARK
	}
}

static void
_my_free(gpointer p)
{
	guint i = GPOINTER_TO_UINT (p);

	if ((i % 2) == 0)
		g_free(p);
}

static void
imsettings_info_finalize(GObject *object)
{
	IMSettingsInfo *info = IMSETTINGS_INFO (object);
	IMSettingsInfoPrivate *priv = info->priv;

	g_hash_table_destroy(priv->info_table);

	if (G_OBJECT_CLASS (imsettings_info_parent_class)->finalize)
		G_OBJECT_CLASS (imsettings_info_parent_class)->finalize(object);
}

static void
imsettings_info_class_init(IMSettingsInfoClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private(klass, sizeof (IMSettingsInfoPrivate));

	object_class->finalize = imsettings_info_finalize;

	/* properties */
}

static void
imsettings_info_init(IMSettingsInfo *info)
{
	info->priv = IMSETTINGS_INFO_GET_PRIVATE (info);
	info->priv->info_table = g_hash_table_new_full(g_direct_hash,
						       g_direct_equal,
						       NULL,
						       _my_free);
}

/*< public >*/

/**
 * imsettings_info_variant_new:
 * @filename: a filename to the xinput configuration file.
 * @language: a locale being desired to work on.
 *
 * Generates a #GVariant from the content of @filename.
 * This API is mainly used in imsettings-daemon and not supposed to
 * be used in any applications.
 *
 * Returns: a #GVariant that can be converted to #IMSettingsInfo with
 *          imsettings_info_new().
 */
GVariant *
imsettings_info_variant_new(const gchar *filename,
			    const gchar *language)
{
	GVariant *value = NULL;
	GVariantBuilder *vb = NULL;
	GString *cmd;
	gchar *helper_path, *xinputinfo, buffer[256];
	const gchar *p;
	gint i;
	struct stat st;
	FILE *fp;

	g_return_val_if_fail (filename != NULL, NULL);
	g_return_val_if_fail (g_file_test(filename, G_FILE_TEST_EXISTS), NULL);

	_quark_init();
	cmd = g_string_new(NULL);

	if (language) {
		gchar *cl = g_strdup(setlocale(LC_CTYPE, NULL));

		if (setlocale(LC_CTYPE, language) != NULL) {
			setlocale(LC_CTYPE, cl);
			g_string_append_printf(cmd, "LANG=%s ", language);
		}
		g_free(cl);
	} else {
	}

	p = g_getenv("IMSETTINGS_HELPER_PATH");
	if (p)
		helper_path = g_strdup(p);
	else
		helper_path = g_strdup(XINPUTINFO_PATH);
	xinputinfo = g_build_filename(helper_path, "xinputinfo.sh", NULL);
	g_string_append_printf(cmd, "%s %s", xinputinfo, filename);

	g_free(helper_path);
	g_free(xinputinfo);

	if (lstat(filename, &st) == -1)
		goto error;

	G_LOCK (info);

	if ((fp = popen(cmd->str, "r")) != NULL) {
		vb = g_variant_builder_new(G_VARIANT_TYPE ("a{sv}"));

		while (!feof(fp)) {
			if ((p = fgets(buffer, 255, fp)) != NULL) {
				gchar *key = NULL, *val = NULL;
				GVariant *v = NULL;
				GQuark q;

				if (!_parse_param(p, &key, &val)) {
					g_warning("Failed to parse a line in %s: %s",
						  filename, p);
					goto bail;
				}
				q = g_quark_try_string(key);
				if (q == 0)
					goto unknown_param;
				for (i = 0; i < LAST_IMSETTINGS_INFO; i++) {
					if (__xinput_tokens[i] == q)
						break;
				}
				switch (i) {
				    case IMSETTINGS_INFO_GTK_IM_MODULE:
				    case IMSETTINGS_INFO_QT_IM_MODULE:
				    case IMSETTINGS_INFO_XIM:
				    case IMSETTINGS_INFO_XIM_PROGRAM:
				    case IMSETTINGS_INFO_XIM_ARGS:
				    case IMSETTINGS_INFO_PREFERENCE_PROGRAM:
				    case IMSETTINGS_INFO_PREFERENCE_ARGS:
				    case IMSETTINGS_INFO_AUXILIARY_PROGRAM:
				    case IMSETTINGS_INFO_AUXILIARY_ARGS:
				    case IMSETTINGS_INFO_SHORT_DESC:
				    case IMSETTINGS_INFO_LONG_DESC:
				    case IMSETTINGS_INFO_ICON:
				    case IMSETTINGS_INFO_LANG:
				    case IMSETTINGS_INFO_FILENAME:
				    case IMSETTINGS_INFO_NOT_RUN:
					    v = g_variant_new_string(val);
					    break;
				    case IMSETTINGS_INFO_IMSETTINGS_IGNORE_ME:
				    case IMSETTINGS_INFO_IMSETTINGS_IS_SCRIPT:
				    case IMSETTINGS_INFO_IS_XIM:
					    v = g_variant_new_boolean((g_ascii_strcasecmp(val, "true") == 0 ||
								       g_ascii_strcasecmp(val, "yes") == 0 ||
								       g_ascii_strcasecmp(val, "1") == 0));
					    break;
				    default:
				    unknown_param:
					    g_warning("Unknown parameter: %s = %s",
						      key, val);
					    v = NULL;
					    break;
				}
				if (v)
					g_variant_builder_add(vb, "{sv}",
							      key, v);
			  bail:
				g_free(key);
				g_free(val);
			}
		}

		pclose(fp);
	}
	value = g_variant_builder_end(vb);

	G_UNLOCK (info);

  error:
	if (vb)
		g_variant_builder_unref(vb);
	g_string_free(cmd, TRUE);

	return value;
}

/**
 * imsettings_info_new:
 * @parameters: a #GVariant generated by imsettings_info_variant_new().
 *
 * Creates an instance of #IMSettingsInfo.
 *
 * Returns: a #IMSettingsInfo or %NULL if giving an invalid @parameters.
 */
IMSettingsInfo *
imsettings_info_new(GVariant *parameters)
{
	IMSettingsInfo *retval;
	IMSettingsInfoPrivate *priv;
	GVariantIter *iter;
	const gchar *key, *s;
	gboolean b;
	GVariant *val;
	gint i;

	g_return_val_if_fail (parameters != NULL, NULL);

	_quark_init();
	retval = IMSETTINGS_INFO (g_object_new(IMSETTINGS_TYPE_INFO, NULL));
	if (retval) {
		priv = retval->priv;

		g_variant_get(parameters, "a{sv}", &iter);
		while (g_variant_iter_next(iter, "{&sv}", &key, &val)) {
			GQuark q = g_quark_try_string(key);

			if (q == 0)
				goto unknown_param;
			for (i = 0; i < LAST_IMSETTINGS_INFO; i++) {
				if (__xinput_tokens[i] == q)
					break;
			}
			switch (i) {
			    case IMSETTINGS_INFO_GTK_IM_MODULE:
			    case IMSETTINGS_INFO_QT_IM_MODULE:
			    case IMSETTINGS_INFO_XIM:
			    case IMSETTINGS_INFO_XIM_PROGRAM:
			    case IMSETTINGS_INFO_XIM_ARGS:
			    case IMSETTINGS_INFO_PREFERENCE_PROGRAM:
			    case IMSETTINGS_INFO_PREFERENCE_ARGS:
			    case IMSETTINGS_INFO_AUXILIARY_PROGRAM:
			    case IMSETTINGS_INFO_AUXILIARY_ARGS:
			    case IMSETTINGS_INFO_SHORT_DESC:
			    case IMSETTINGS_INFO_LONG_DESC:
			    case IMSETTINGS_INFO_ICON:
			    case IMSETTINGS_INFO_LANG:
			    case IMSETTINGS_INFO_FILENAME:
			    case IMSETTINGS_INFO_NOT_RUN:
				    g_variant_get(val, "&s", &s);
				    g_hash_table_insert(priv->info_table,
							GUINT_TO_POINTER (q), g_strdup(s));
				    break;
			    case IMSETTINGS_INFO_IMSETTINGS_IGNORE_ME:
			    case IMSETTINGS_INFO_IMSETTINGS_IS_SCRIPT:
			    case IMSETTINGS_INFO_IS_XIM:
				    g_variant_get(val, "b", &b);
				    g_hash_table_insert(priv->info_table,
							GUINT_TO_POINTER (q), GINT_TO_POINTER (b));
				    break;
			    default:
			    unknown_param:
				    g_warning("Unknown parameter: %s", key);
				    break;
			}
		}
		g_variant_iter_free(iter);
	}

	return retval;
}

#define _DEFUNC_PROPERTY(_t_,_n_,_s_,_c_,_v_)				\
	_t_								\
	imsettings_info_ ## _n_(IMSettingsInfo *info)			\
	{								\
		IMSettingsInfoPrivate *priv;				\
		gpointer p = NULL;					\
		_t_ retval;						\
									\
		g_return_val_if_fail (IMSETTINGS_IS_INFO (info), (_v_)); \
									\
		priv = info->priv;					\
		if (!g_hash_table_lookup_extended(priv->info_table,	\
						  GUINT_TO_POINTER (__xinput_tokens[IMSETTINGS_INFO_ ## _s_]), \
						  NULL,			\
						  &p)) {		\
			return (_v_);					\
		}							\
		retval = _c_(p);					\
									\
		return retval;						\
	}

/**
 * imsettings_info_get_filename:
 * @info: a #IMSettingsInfo.
 *
 * Obtains the xinput configuration filename.
 *
 * Returns: a string. this shouldn't be freed.
 */
_DEFUNC_PROPERTY (const gchar *, get_filename, FILENAME, (const gchar *), NULL)
/**
 * imsettings_info_get_language:
 * @info: a #IMSettingsInfo.
 *
 * Obtains the language that @info genereated by.
 *
 * Returns: a string. this shouldn't be freed.
 */
_DEFUNC_PROPERTY (const gchar *, get_language, LANG, (const gchar *), NULL)
/**
 * imsettings_info_get_gtkimm:
 * @info: a #IMSettingsInfo.
 *
 * Obtains the GTK+ immodule name. this is same value of %GTK_IM_MODULE
 * parameter in the xinput configuration file.
 *
 * Returns: a string. this shouldn't be freed.
 */
_DEFUNC_PROPERTY (const gchar *, get_gtkimm, GTK_IM_MODULE, (const gchar*), NULL)
/**
 * imsettings_info_get_qtimm:
 * @info: a #IMSettingsInfo.
 *
 * Obtains the Qt immodule name.
 *
 * This gives you same value of %QT_IM_MODULE parameter in the xinput
 * configuration file.
 *
 * Returns: a string. this shouldn't be freed.
 */
_DEFUNC_PROPERTY (const gchar *, get_qtimm, QT_IM_MODULE, (const gchar *), NULL)
/**
 * imsettings_info_get_xim:
 * @info: a #IMSettingsInfo.
 *
 * Obtains the XIM atom that is supposed to be used with XMODIFIERS=\@im=.
 *
 * This gives you same value of %XIM parameter in the xinput
 * configuration file.
 *
 * Returns: a string. this shouldn't be freed.
 */
_DEFUNC_PROPERTY (const gchar *, get_xim, XIM, (const gchar *), NULL)
/**
 * imsettings_info_get_xim_program:
 * @info: a #IMSettingsInfo.
 *
 * Obtains the XIM program name, which will be invoked by imsettings-daemon
 * with the arguments from imsettings_info_get_xim_args().
 *
 * This gives you same value of %XIM_PROGRAM parameter in the xinput
 * configuration file.
 *
 * Returns: a string. this shouldn't be freed.
 */
_DEFUNC_PROPERTY (const gchar *, get_xim_program, XIM_PROGRAM, (const gchar *), NULL)
/**
 * imsettings_info_get_xim_args:
 * @info: a #IMSettingsInfo.
 *
 * Obtains the arguments of XIM program.
 *
 * This gives you same value of %XIM_ARGS parameter in the xinput
 * configuration file.
 *
 * Returns: a string or %NULL if it's not specified. this shouldn't be freed.
 */
_DEFUNC_PROPERTY (const gchar *, get_xim_args, XIM_ARGS, (const gchar *), NULL)
/**
 * imsettings_info_get_prefs_program:
 * @info: a #IMSettingsInfo.
 *
 * Obtains the preference program name for the Input Method.
 *
 * This gives you same value of %PREFERENCE_PROGRAM parameter in the xinput
 * configuration file.
 *
 * Returns: a string or %NULL if it's not specified. this shouldn't be freed.
 */
_DEFUNC_PROPERTY (const gchar *, get_prefs_program, PREFERENCE_PROGRAM, (const gchar *), NULL)
/**
 * imsettings_info_get_prefs_args:
 * @info: a #IMSettingsInfo.
 *
 * Obtains the arguments of the preference program.
 *
 * This gives you same value of %PREFERENCE_ARGS parameter in the xinput
 * configuration file.
 *
 * Returns: a string or %NULL if it's not specified. this shouldn't be freed.
 */
_DEFUNC_PROPERTY (const gchar *, get_prefs_args, PREFERENCE_ARGS, (const gchar *), NULL)
/**
 * imsettings_info_get_aux_program:
 * @info: a #IMSettingsInfo.
 *
 * Obtains the auxiliary program name, which will be invoked by imsettings-daemon
 * with the arguments from imsettings_info_get_aux_args().
 *
 * This gives you same value of %AUXILIARY_PROGRAM parameter in the xinput
 * configuration file.
 *
 * Returns: a string or %NULL if it's not specified. this shouldn't be freed.
 */
_DEFUNC_PROPERTY (const gchar *, get_aux_program, AUXILIARY_PROGRAM, (const gchar *), NULL)
/**
 * imsettings_info_get_aux_args:
 * @info: a #IMSettingsInfo.
 *
 * Obtains the arguments of the auxiliary program.
 *
 * This gives you same value of %AUXILIARY_ARGS parameter in the xinput
 * configuration file.
 *
 * Returns: a string or %NULL if it's not specified. this shouldn't be freed.
 */
_DEFUNC_PROPERTY (const gchar *, get_aux_args, AUXILIARY_ARGS, (const gchar *), NULL)
/**
 * imsettings_info_get_icon_file:
 * @info: a #IMSettingsInfo.
 *
 * Obtains the icon filename for the Input Method.
 *
 * This gives you same value of %ICON parameter in the xinput
 * configuration file.
 *
 * Returns: a string. this shouldn't be freed.
 */
_DEFUNC_PROPERTY (const gchar *, get_icon_file, ICON, (const gchar *), NULL)
/**
 * imsettings_info_get_non_target:
 * @info: a #IMSettingsInfo.
 *
 * Obtains non-targeted desktop session for the Input Method.
 *
 * This gives you same value of %NOT_RUN parameter in the xinput
 * configuration file.
 *
 * Returns: a string. this shouldn't be freed.
 */
_DEFUNC_PROPERTY (const gchar *, get_non_target, NOT_RUN, (const gchar *), NULL)

/**
 * imsettings_info_get_short_desc:
 * @info: a #IMSettingsInfo.
 *
 * Obtains the short description for Input Method.
 *
 * This gives you same value of %SHORT_DESC parameter in the xinput
 * configuration file.  If it doesn't contain, the result would be same
 * to what imsettings_info_get_xim() returns.
 *
 * Returns: a string. this shouldn't be freed.
 */
const gchar *
imsettings_info_get_short_desc(IMSettingsInfo *info)
{
	IMSettingsInfoPrivate *priv;
	gpointer p = NULL;
	const gchar *retval;

	g_return_val_if_fail (IMSETTINGS_IS_INFO (info), NULL);

	priv = info->priv;
	if (!g_hash_table_lookup_extended(priv->info_table,
					  GUINT_TO_POINTER (__xinput_tokens[IMSETTINGS_INFO_SHORT_DESC]),
					  NULL,
					  &p)) {
		return imsettings_info_get_xim(info);
	} else if (p == NULL || *(gchar *)p == 0) {
		return imsettings_info_get_xim(info);
	}
	retval = (const gchar *)p;

	return retval;
}

/**
 * imsettings_info_get_im_name:
 * @info: a #IMSettingsInfo.
 *
 * Obtains the Input Method name. this is same value of %SHORT_DESC
 * if it doesn't contain the sub module information that is separate with ':'
 * like:
 *
 * |[
 *   SHORT_DESC=foo-im:bar-subim
 * ]|
 *
 * Returns: a string. this shouldn't be freed.
 */
const gchar *
imsettings_info_get_im_name(IMSettingsInfo *info)
{
	IMSettingsInfoPrivate *priv;
	gpointer p = NULL;
	gchar *im, *subim;
	const gchar *retval, *sd;

	g_return_val_if_fail (IMSETTINGS_IS_INFO (info), NULL);

	priv = info->priv;
	if (!g_hash_table_lookup_extended(priv->info_table,
					  GUINT_TO_POINTER (__xinput_tokens[IMSETTINGS_INFO_IM_NAME]),
					  NULL,
					  &p)) {
	  rebuild:
		sd = imsettings_info_get_short_desc(info);
		p = strchr(sd, ':');
		if (!p) {
			im = g_strdup(sd);
			subim = NULL;
		} else {
			im = g_strndup(sd, (gulong)p - (gulong)sd);
			subim = g_strdup(&((gchar *)p)[1]);
		}

		g_hash_table_insert(priv->info_table,
				    GUINT_TO_POINTER (__xinput_tokens[IMSETTINGS_INFO_IM_NAME]),
				    im);
		g_hash_table_insert(priv->info_table,
				    GUINT_TO_POINTER (__xinput_tokens[IMSETTINGS_INFO_SUB_IM_NAME]),
				    subim);

		return (const gchar *)im;
	} else if (p == NULL || *(gchar *)p == 0) {
		goto rebuild;
	}
	retval = (const gchar *)p;

	return retval;
}

/**
 * imsettings_info_get_sub_im_name:
 * @info: a #IMSettingsInfo.
 *
 * Obtains the sub Input Method name.
 *
 * Returns: a string or %NULL if %SHORT_DESC doesn't contain any information
 *          for sub Input Method.
 */
const gchar *
imsettings_info_get_sub_im_name(IMSettingsInfo *info)
{
	IMSettingsInfoPrivate *priv;
	gpointer p = NULL;
	gchar *im, *subim;
	const gchar *retval, *sd;

	g_return_val_if_fail (IMSETTINGS_IS_INFO (info), NULL);

	priv = info->priv;
	if (!g_hash_table_lookup_extended(priv->info_table,
					  GUINT_TO_POINTER (__xinput_tokens[IMSETTINGS_INFO_SUB_IM_NAME]),
					  NULL,
					  &p)) {
	  rebuild:
		sd = imsettings_info_get_short_desc(info);
		p = strchr(sd, ':');
		if (!p) {
			im = g_strdup(sd);
			subim = NULL;
		} else {
			im = g_strndup(sd, (gulong)p - (gulong)sd);
			subim = g_strdup(&((gchar *)p)[1]);
		}

		g_hash_table_insert(priv->info_table,
				    GUINT_TO_POINTER (__xinput_tokens[IMSETTINGS_INFO_IM_NAME]),
				    im);
		g_hash_table_insert(priv->info_table,
				    GUINT_TO_POINTER (__xinput_tokens[IMSETTINGS_INFO_SUB_IM_NAME]),
				    subim);

		return (const gchar *)subim;
	} else if (p == NULL || *(gchar *)p == 0) {
		if (!g_hash_table_lookup_extended(priv->info_table,
						  GUINT_TO_POINTER (__xinput_tokens[IMSETTINGS_INFO_IM_NAME]),
						  NULL,
						  NULL))
			goto rebuild;
	}
	retval = (const gchar *)p;

	return retval;
}

/**
 * imsettings_info_get_long_desc:
 * @info: a #IMSettingsInfo.
 *
 * Obtains the long description for Input Method.
 *
 * This gives you same value of %LONG_DESC parameter in the xinput
 * configuration file.
 *
 * Returns: a string or %NULL if it's not specified. this shouldn't be freed.
 */
_DEFUNC_PROPERTY (const gchar *, get_long_desc, LONG_DESC, (const gchar *), NULL)
/**
 * imsettings_info_is_visible:
 * @info: a #IMSettingsInfo.
 *
 * Checks whether the Input Method is visible.
 *
 * The result would be same what the xinput configuration file specifies in
 * %IMSETTINGS_IGNORE_ME.
 *
 * Returns: %TRUE if it's visible, otherwise %FALSE.
 */
_DEFUNC_PROPERTY (gboolean, is_visible, IMSETTINGS_IGNORE_ME, !(gboolean)GPOINTER_TO_INT, FALSE)
/**
 * imsettings_info_is_script:
 * @info: a #IMSettingsInfo.
 *
 * Checks whether the xinput configuration file is the scripting language.
 *
 * This is useful to see if the result may be different when the condition
 * is changed.
 *
 * Returns: %TRUE if the xinput configuration file is the scripting language.
 *          otherwise %FALSE.
 */
_DEFUNC_PROPERTY (gboolean, is_script, IMSETTINGS_IS_SCRIPT, (gboolean)GPOINTER_TO_INT, FALSE)
/**
 * imsettings_info_is_xim:
 * @info: a #IMSettingsInfo.
 *
 * Checks whether the Input Method in @info is supposed to work on XIM only.
 *
 * Returns: %TRUE if it is, otherwise %FALSE.
 */
_DEFUNC_PROPERTY (gboolean, is_xim, IS_XIM, (gboolean)GPOINTER_TO_INT, FALSE);

/**
 * imsettings_info_is_system_default:
 * @info: a #IMSettingsInfo.
 *
 * Checks whether the Input Method in @info is the system default.
 *
 * Returns: %TRUE if it is. otherwise %FALSE.
 */
gboolean
imsettings_info_is_system_default(IMSettingsInfo *info)
{
	IMSettingsClient *client;
	GError *err = NULL;
	gboolean retval;

	g_return_val_if_fail (IMSETTINGS_IS_INFO (info), FALSE);

	client = imsettings_client_new(imsettings_info_get_language(info));
	g_return_val_if_fail (IMSETTINGS_IS_CLIENT (client), FALSE);

	retval = imsettings_client_im_is_system_default(client,
							imsettings_info_get_short_desc(info),
							NULL, &err);
	if (err) {
		g_warning("%s", err->message);
		g_error_free(err);
	}

	g_object_unref(client);

	return retval;
}

/**
 * imsettings_info_is_user_default:
 * @info: a #IMSettingsInfo.
 *
 * Checks whether the Input Method in @info is the user default.
 *
 * Returns: %TRUE if it is. otherwise %FALSE.
 */
gboolean
imsettings_info_is_user_default(IMSettingsInfo *info)
{
	IMSettingsClient *client;
	GError *err = NULL;
	gboolean retval;

	g_return_val_if_fail (IMSETTINGS_IS_INFO (info), FALSE);

	client = imsettings_client_new(imsettings_info_get_language(info));
	g_return_val_if_fail (IMSETTINGS_IS_CLIENT (client), FALSE);

	retval = imsettings_client_im_is_user_default(client,
						      imsettings_info_get_short_desc(info),
						      NULL, &err);
	if (err) {
		g_warning("%s", err->message);
		g_error_free(err);
	}

	g_object_unref(client);

	return retval;
}

/**
 * imsettings_info_is_immodule_only:
 * @info: a #IMSettingsInfo.
 *
 * Checks whether the Input Method in @info is supposed to work for
 * immodule only.
 *
 * Returns: %TRUE if it is. otherwise %FALSE.
 */
gboolean
imsettings_info_is_immodule_only(IMSettingsInfo *info)
{
	const gchar *xim, *gtkimm;

	g_return_val_if_fail (IMSETTINGS_IS_INFO (info), FALSE);

	xim = imsettings_info_get_xim(info);
	gtkimm = imsettings_info_get_gtkimm(info);

	return g_strcmp0(xim, IMSETTINGS_NONE_CONF) == 0 &&
	       g_strcmp0(gtkimm, "gtk-im-context-simple") != 0;
}

/**
 * imsettings_info_compare:
 * @info1: a #IMSettingsInfo to compare with @i2.
 * @info2: a #IMSettingsInfo to compare with @i1.
 *
 * Compares two #IMSettingsInfo instance.
 * This function would simply compares the result of the real value in
 * the xinput configuration file. so the result of
 * imsettings_info_is_script(), imsettings_info_get_language(),
 * imsettings_info_get_filename(), imsettings_info_is_xim()
 * imsettings_info_get_im_name() and imsettings_info_get_sub_im_name()
 * will be ignored.
 *
 * Returns: %TRUE if @i1 and @i2 match, otherwise %FALSE.
 */
gboolean
imsettings_info_compare(const IMSettingsInfo *i1,
			const IMSettingsInfo *i2)
{
	IMSettingsInfoPrivate *p1, *p2;
	GHashTableIter iter;
	gpointer key, v1, v2;

	g_return_val_if_fail (IMSETTINGS_IS_INFO (i1), FALSE);
	g_return_val_if_fail (IMSETTINGS_IS_INFO (i2), FALSE);

	if (i1 == i2)
		return TRUE;
	p1 = i1->priv;
	p2 = i2->priv;

	if (g_hash_table_size(p1->info_table) != g_hash_table_size(p2->info_table))
		return FALSE;

	g_hash_table_iter_init(&iter, p1->info_table);

	while (g_hash_table_iter_next(&iter, &key, &v1)) {
		GQuark q = (GQuark)GPOINTER_TO_INT (key);

		if (q == __xinput_tokens[IMSETTINGS_INFO_IMSETTINGS_IS_SCRIPT] ||
		    q == __xinput_tokens[IMSETTINGS_INFO_LANG] ||
		    q == __xinput_tokens[IMSETTINGS_INFO_FILENAME] ||
		    q == __xinput_tokens[IMSETTINGS_INFO_IS_XIM] ||
		    q == __xinput_tokens[IMSETTINGS_INFO_IM_NAME] ||
		    q == __xinput_tokens[IMSETTINGS_INFO_SUB_IM_NAME]) {
			/* ignore */
			continue;
		}
		if (!g_hash_table_lookup_extended(p2->info_table, key, NULL, &v2))
			return FALSE;
		if (v1 && v2 && (gulong)v1 % 2 == 0 && (gulong)v2 % 2 == 0) {
			if (g_strcmp0(v1, v2) != 0)
				return FALSE;
		} else {
			if (v1 != v2)
				return FALSE;
		}
	}

	return TRUE;
}
