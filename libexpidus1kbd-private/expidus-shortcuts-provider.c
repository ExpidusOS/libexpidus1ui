/* vi:set expandtab sw=2 sts=2: */
/*
 * Copyright (c) 2008 Jannis Pohlmann <jannis@xfce.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <glib.h>
#include <glib-object.h>

#include <libexpidus1util/libexpidus1util.h>
#include <esconf/esconf.h>

#include <libexpidus1kbd-private/expidus-shortcuts-provider.h>



#define EXPIDUS_SHORTCUTS_PROVIDER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
    EXPIDUS_TYPE_SHORTCUTS_PROVIDER, ExpidusShortcutsProviderPrivate))



/* Property identifiers */
enum
{
  PROP_0,
  PROP_NAME,
};



typedef struct _ExpidusShortcutsProviderContext ExpidusShortcutsProviderContext;



static void expidus_shortcuts_provider_constructed      (GObject                    *object);
static void expidus_shortcuts_provider_finalize         (GObject                    *object);
static void expidus_shortcuts_provider_get_property     (GObject                    *object,
                                                      guint                       prop_id,
                                                      GValue                     *value,
                                                      GParamSpec                 *pspec);
static void expidus_shortcuts_provider_set_property     (GObject                    *object,
                                                      guint                       prop_id,
                                                      const GValue               *value,
                                                      GParamSpec                 *pspec);
static void expidus_shortcuts_provider_register         (ExpidusShortcutsProvider      *provider);
static void expidus_shortcuts_provider_property_changed (EsconfChannel              *channel,
                                                      gchar                      *property,
                                                      GValue                     *value,
                                                      ExpidusShortcutsProvider      *provider);



struct _ExpidusShortcutsProviderPrivate
{
  EsconfChannel *channel;
  gchar         *name;
  gchar         *default_base_property;
  gchar         *custom_base_property;
};

struct _ExpidusShortcutsProviderContext
{
  ExpidusShortcutsProvider *provider;
  GList                 *list;
  const gchar           *base_property;
  GHashTable            *properties;
};



G_DEFINE_TYPE (ExpidusShortcutsProvider, expidus_shortcuts_provider, G_TYPE_OBJECT)



static void
expidus_shortcuts_provider_class_init (ExpidusShortcutsProviderClass *klass)
{
  GObjectClass *gobject_class;

  g_type_class_add_private (klass, sizeof (ExpidusShortcutsProviderPrivate));

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->constructed = expidus_shortcuts_provider_constructed;
  gobject_class->finalize = expidus_shortcuts_provider_finalize;
  gobject_class->get_property = expidus_shortcuts_provider_get_property;
  gobject_class->set_property = expidus_shortcuts_provider_set_property;

  g_object_class_install_property (gobject_class,
                                   PROP_NAME,
                                   g_param_spec_string ("name",
                                                        "name",
                                                        "name",
                                                        NULL,
                                                        G_PARAM_READWRITE
                                                        | G_PARAM_CONSTRUCT_ONLY
                                                        | G_PARAM_STATIC_STRINGS));

  g_signal_new ("shortcut-removed",
                EXPIDUS_TYPE_SHORTCUTS_PROVIDER,
                G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
                0,
                NULL,
                NULL,
                g_cclosure_marshal_VOID__STRING,
                G_TYPE_NONE,
                1,
                G_TYPE_STRING);

  g_signal_new ("shortcut-added",
                EXPIDUS_TYPE_SHORTCUTS_PROVIDER,
                G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
                0,
                NULL,
                NULL,
                g_cclosure_marshal_VOID__STRING,
                G_TYPE_NONE,
                1,
                G_TYPE_STRING);
}



static void
expidus_shortcuts_provider_init (ExpidusShortcutsProvider *provider)
{
  provider->priv = EXPIDUS_SHORTCUTS_PROVIDER_GET_PRIVATE (provider);

  provider->priv->channel = esconf_channel_new ("expidus1-keyboard-shortcuts");

  g_signal_connect (provider->priv->channel, "property-changed",
                    G_CALLBACK (expidus_shortcuts_provider_property_changed), provider);
}



static void
expidus_shortcuts_provider_constructed (GObject *object)
{
  ExpidusShortcutsProvider *provider = EXPIDUS_SHORTCUTS_PROVIDER (object);

  expidus_shortcuts_provider_register (provider);

  provider->priv->default_base_property = g_strdup_printf ("/%s/default", provider->priv->name);
  provider->priv->custom_base_property = g_strdup_printf ("/%s/custom", provider->priv->name);

  if (!expidus_shortcuts_provider_is_custom (provider))
    expidus_shortcuts_provider_reset_to_defaults (provider);
}



static void
expidus_shortcuts_provider_finalize (GObject *object)
{
  ExpidusShortcutsProvider *provider = EXPIDUS_SHORTCUTS_PROVIDER (object);

  g_free (provider->priv->name);
  g_free (provider->priv->custom_base_property);
  g_free (provider->priv->default_base_property);

  g_object_unref (provider->priv->channel);

  (*G_OBJECT_CLASS (expidus_shortcuts_provider_parent_class)->finalize) (object);
}



static void
expidus_shortcuts_provider_get_property (GObject    *object,
                                      guint       prop_id,
                                      GValue     *value,
                                      GParamSpec *pspec)
{
  ExpidusShortcutsProvider *provider = EXPIDUS_SHORTCUTS_PROVIDER (object);

  switch (prop_id)
    {
    case PROP_NAME:
      g_value_set_string (value, provider->priv->name);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}



static void
expidus_shortcuts_provider_set_property (GObject      *object,
                                      guint         prop_id,
                                      const GValue *value,
                                      GParamSpec   *pspec)
{
  ExpidusShortcutsProvider *provider = EXPIDUS_SHORTCUTS_PROVIDER (object);

  switch (prop_id)
    {
    case PROP_NAME:
      g_free (provider->priv->name);
      provider->priv->name = g_strdup (g_value_get_string (value));
      g_object_notify (object, "name");
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}



static void
expidus_shortcuts_provider_register (ExpidusShortcutsProvider *provider)
{
  gchar       **provider_names;
  gchar       **names;
  gboolean      already_registered = FALSE;
  gint          i;
  const gchar  *name;

  g_return_if_fail (EXPIDUS_IS_SHORTCUTS_PROVIDER (provider));

  name = expidus_shortcuts_provider_get_name (provider);
  if (G_UNLIKELY (name == NULL))
    return;

  provider_names = esconf_channel_get_string_list (provider->priv->channel, "/providers");
  if (provider_names != NULL)
    for (i = 0; !already_registered && provider_names[i] != NULL; i++)
      already_registered = g_str_equal (provider_names[i], name);

  if (G_UNLIKELY (!already_registered))
    {
      names = g_new0 (gchar *, (provider_names != NULL ? g_strv_length (provider_names) : 0) + 2);
      i = 0;

      if (provider_names != NULL)
        for (; provider_names[i] != NULL; i++)
          names[i] = provider_names[i];

      names[i++] = (gchar *) name;
      names[i] = NULL;

      esconf_channel_set_string_list (provider->priv->channel, "/providers",
                                      (const gchar * const *) names);

      g_free (names);
    }

  g_strfreev (provider_names);
}



static void
expidus_shortcuts_provider_property_changed (EsconfChannel         *channel,
                                          gchar                 *property,
                                          GValue                *value,
                                          ExpidusShortcutsProvider *provider)
{
  const gchar *shortcut;
  gchar       *override_property;

  g_return_if_fail (EXPIDUS_IS_SHORTCUTS_PROVIDER (provider));

  DBG ("property = %s", property);

  if (!g_str_has_prefix (property, provider->priv->custom_base_property))
    return;

  override_property = g_strconcat (provider->priv->custom_base_property, "/override", NULL);

  if (G_UNLIKELY (g_utf8_collate (property, override_property) == 0))
    {
      g_free (override_property);
      return;
    }
  g_free (override_property);

  if (g_str_has_suffix (property, "/startup-notify"))
    return;

  shortcut = property + strlen (provider->priv->custom_base_property) + strlen ("/");

  if (G_VALUE_TYPE (value) != G_TYPE_INVALID)
    g_signal_emit_by_name (provider, "shortcut-added", shortcut);
  else
    g_signal_emit_by_name (provider, "shortcut-removed", shortcut);
}



ExpidusShortcutsProvider *
expidus_shortcuts_provider_new (const gchar *name)
{
  return g_object_new (EXPIDUS_TYPE_SHORTCUTS_PROVIDER, "name", name, NULL);
}



GList *
expidus_shortcuts_provider_get_providers (void)
{
  GList         *providers = NULL;
  EsconfChannel *channel;
  gchar        **names;
  gint           i;

  channel = esconf_channel_get ("expidus1-keyboard-shortcuts");
  names = esconf_channel_get_string_list (channel, "/providers");

  if (G_LIKELY (names != NULL))
    {
      for (i = 0; names[i] != NULL; ++i)
        providers = g_list_append (providers, expidus_shortcuts_provider_new (names[i]));
      g_strfreev (names);
    }

  return providers;
}



void
expidus_shortcuts_provider_free_providers (GList *providers)
{
  GList *iter;

  for (iter = g_list_first (providers); iter != NULL; iter = g_list_next (iter))
    g_object_unref (iter->data);

  g_list_free (providers);
}



const gchar *
expidus_shortcuts_provider_get_name (ExpidusShortcutsProvider *provider)
{
  g_return_val_if_fail (EXPIDUS_IS_SHORTCUTS_PROVIDER (provider), NULL);
  return provider->priv->name;
}



gboolean
expidus_shortcuts_provider_is_custom (ExpidusShortcutsProvider *provider)
{
  gchar   *property;
  gboolean override;

  g_return_val_if_fail (EXPIDUS_IS_SHORTCUTS_PROVIDER (provider), FALSE);
  g_return_val_if_fail (ESCONF_IS_CHANNEL (provider->priv->channel), FALSE);

  property = g_strconcat (provider->priv->custom_base_property, "/override", NULL);
  override = esconf_channel_get_bool (provider->priv->channel, property, FALSE);
  g_free (property);

  return override;
}



void
expidus_shortcuts_provider_reset_to_defaults (ExpidusShortcutsProvider *provider)
{
  g_return_if_fail (EXPIDUS_IS_SHORTCUTS_PROVIDER (provider));
  g_return_if_fail (ESCONF_IS_CHANNEL (provider->priv->channel));

  DBG ("property = %s", provider->priv->custom_base_property);

  esconf_channel_reset_property (provider->priv->channel, provider->priv->custom_base_property, TRUE);
  expidus_shortcuts_provider_clone_defaults (provider);
}



static gboolean
_expidus_shortcuts_provider_clone_default (const gchar           *property,
                                        const GValue          *value,
                                        ExpidusShortcutsProvider *provider)
{
  const gchar *shortcut;
  const gchar *command;
  gchar       *custom_property;

  g_return_val_if_fail (EXPIDUS_IS_SHORTCUTS_PROVIDER (provider), TRUE);
  g_return_val_if_fail (ESCONF_IS_CHANNEL (provider->priv->channel), TRUE);

  if (G_UNLIKELY (!G_IS_VALUE (value)))
    return FALSE;

  shortcut = property + strlen (provider->priv->default_base_property) + strlen ("/");
  command = g_value_get_string (value);

  DBG ("shortcut = %s, command = %s", shortcut, command);

  custom_property = g_strconcat (provider->priv->custom_base_property, "/", shortcut, NULL);
  esconf_channel_set_property (provider->priv->channel, custom_property, value);
  g_free (custom_property);

  return FALSE;
}



void
expidus_shortcuts_provider_clone_defaults (ExpidusShortcutsProvider *provider)
{
  GHashTable *properties;
  gchar      *property;

  g_return_if_fail (EXPIDUS_IS_SHORTCUTS_PROVIDER (provider));
  g_return_if_fail (ESCONF_IS_CHANNEL (provider->priv->channel));

  /* Get default command shortcuts */
  properties = esconf_channel_get_properties (provider->priv->channel, provider->priv->default_base_property);

  if (G_LIKELY (properties != NULL))
    {
      /* Copy from /commands/default to /commands/custom property by property */
      g_hash_table_foreach (properties,
                            (GHFunc) (void (*)(void)) _expidus_shortcuts_provider_clone_default,
                            provider);

      g_hash_table_destroy (properties);
    }

  DBG ("adding override property");

  /* Add the override property */
  property = g_strconcat (provider->priv->custom_base_property, "/override", NULL);
  esconf_channel_set_bool (provider->priv->channel, property, TRUE);
  g_free (property);
}



static gboolean
_expidus_shortcuts_provider_get_shortcut (const gchar                  *property,
                                       const GValue                 *value,
                                       ExpidusShortcutsProviderContext *context)
{
  ExpidusShortcut *sc;
  const gchar  *shortcut;
  const gchar  *command;
  const GValue *snotify;
  gchar        *snotify_prop;

  g_return_val_if_fail (context != NULL, TRUE);
  g_return_val_if_fail (EXPIDUS_IS_SHORTCUTS_PROVIDER (context->provider), TRUE);

  if (G_VALUE_TYPE (value) != G_TYPE_STRING)
    return FALSE;

  if (!g_str_has_prefix (property, context->provider->priv->custom_base_property))
    return FALSE;

  shortcut = property + strlen (context->provider->priv->custom_base_property) + strlen ("/");

  command = g_value_get_string (value);

  if (G_LIKELY (shortcut != NULL
      && command != NULL
      && g_utf8_strlen (shortcut, -1) > 0
      && g_utf8_strlen (command, -1) > 0))
    {
      sc = g_slice_new0 (ExpidusShortcut);

      sc->property_name = g_strdup (property);
      sc->shortcut = g_strdup (shortcut);
      sc->command = g_strdup (command);

      /* Lookup startup notify in the hash table */
      snotify_prop = g_strconcat (property, "/startup-notify", NULL);
      snotify = g_hash_table_lookup (context->properties, snotify_prop);
      if (snotify != NULL)
        sc->snotify = g_value_get_boolean (snotify);
      else
        sc->snotify = FALSE;
      g_free (snotify_prop);
      context->list = g_list_append (context->list, sc);
    }

  return FALSE;
}



GList *
expidus_shortcuts_provider_get_shortcuts (ExpidusShortcutsProvider *provider)
{
  ExpidusShortcutsProviderContext context;
  GHashTable                  *properties;

  g_return_val_if_fail (EXPIDUS_IS_SHORTCUTS_PROVIDER (provider), NULL);
  g_return_val_if_fail (ESCONF_IS_CHANNEL (provider->priv->channel), NULL);

  properties = esconf_channel_get_properties (provider->priv->channel, provider->priv->custom_base_property);

  context.provider = provider;
  context.list = NULL;
  context.properties = properties;

  if (G_LIKELY (properties != NULL))
    g_hash_table_foreach (properties,
                          (GHFunc) (void (*)(void)) _expidus_shortcuts_provider_get_shortcut,
                          &context);

  return context.list;
}



ExpidusShortcut *
expidus_shortcuts_provider_get_shortcut (ExpidusShortcutsProvider *provider,
                                      const gchar           *shortcut)
{
  ExpidusShortcut *sc = NULL;
  gchar        *base_property;
  gchar        *property;
  gchar        *command;
  gchar        *property2;
  gboolean      snotify;

  g_return_val_if_fail (EXPIDUS_IS_SHORTCUTS_PROVIDER (provider), NULL);
  g_return_val_if_fail (ESCONF_IS_CHANNEL (provider->priv->channel), NULL);

  if (G_LIKELY (expidus_shortcuts_provider_is_custom (provider)))
    base_property = provider->priv->custom_base_property;
  else
    base_property = provider->priv->default_base_property;

  property = g_strconcat (base_property, "/", shortcut, NULL);
  command = esconf_channel_get_string (provider->priv->channel, property, NULL);

  if (G_LIKELY (command != NULL))
    {
      property2 = g_strconcat (property, "/startup-notify", NULL);
      snotify = esconf_channel_get_bool (provider->priv->channel, property2, FALSE);

      sc = g_slice_new0 (ExpidusShortcut);
      sc->command = command;
      sc->property_name = g_strdup (property);
      sc->shortcut = g_strdup (shortcut);
      sc->snotify = snotify;
    }

  g_free (property);

  return sc;
}



gboolean
expidus_shortcuts_provider_has_shortcut (ExpidusShortcutsProvider *provider,
                                      const gchar           *shortcut)
{
  gboolean has_property;
  gchar   *base_property;
  gchar   *property;

  g_return_val_if_fail (EXPIDUS_IS_SHORTCUTS_PROVIDER (provider), FALSE);
  g_return_val_if_fail (ESCONF_IS_CHANNEL (provider->priv->channel), FALSE);

  if (G_LIKELY (expidus_shortcuts_provider_is_custom (provider)))
    base_property = provider->priv->custom_base_property;
  else
    base_property = provider->priv->default_base_property;

  property = g_strconcat (base_property, "/", shortcut, NULL);
  has_property = esconf_channel_has_property (provider->priv->channel, property);
  g_free (property);

  if (!has_property && g_strrstr (shortcut, "<Primary>"))
    {
      /* We want to match a shortcut with <Primary>. Older versions of
       * GTK+ used <Control> and this might be stored in Esconf. We need
       * to check for this too. */

      const gchar *primary;
      const gchar *p, *s;
      GString     *replaced;
      gchar       *with_control_shortcut;

      replaced = g_string_sized_new (strlen (shortcut));
      primary = "Primary";

      /* Replace Primary in the string by Control using the same logic
       * as endo_str_replace. */

      while (*shortcut != '\0')
        {
          if (G_UNLIKELY (*shortcut == *primary))
            {
              /* compare the pattern to the current string */
              for (p = primary + 1, s = shortcut + 1; *p == *s; ++s, ++p)
                if (*p == '\0' || *s == '\0')
                  break;

              /* check if the pattern fully matched */
              if (G_LIKELY (*p == '\0'))
                {
                  g_string_append (replaced, "Control");
                  shortcut = s;
                  continue;
                }
            }

          g_string_append_c (replaced, *shortcut++);
        }

      with_control_shortcut = g_string_free (replaced, FALSE);

      DBG ("Looking for old GTK+ shortcut %s", with_control_shortcut);

      property =
        g_strconcat (base_property, "/", with_control_shortcut, NULL);
      has_property = esconf_channel_has_property (provider->priv->channel, property);
      g_free (property);

      g_free (with_control_shortcut);
    }

  return has_property;
}



void
expidus_shortcuts_provider_set_shortcut (ExpidusShortcutsProvider *provider,
                                      const gchar           *shortcut,
                                      const gchar           *command,
                                      gboolean               snotify)
{
  gchar *property;
  gchar *property2;

  g_return_if_fail (EXPIDUS_IS_SHORTCUTS_PROVIDER (provider));
  g_return_if_fail (ESCONF_IS_CHANNEL (provider->priv->channel));
  g_return_if_fail (shortcut != NULL && command != NULL);

  /* Only allow custom shortcuts to be changed */
  if (G_UNLIKELY (!expidus_shortcuts_provider_is_custom (provider)))
    return;

  property = g_strconcat (provider->priv->custom_base_property, "/", shortcut, NULL);

  if (esconf_channel_has_property (provider->priv->channel, property))
    esconf_channel_reset_property (provider->priv->channel, property, TRUE);

  if (snotify)
    {
      property2 = g_strconcat (property, "/startup-notify", NULL);
      esconf_channel_set_bool (provider->priv->channel, property2, snotify);
      g_free (property2);
    }

  esconf_channel_set_string (provider->priv->channel, property, command);

  g_free (property);

}



void
expidus_shortcuts_provider_reset_shortcut (ExpidusShortcutsProvider *provider,
                                        const gchar           *shortcut)
{
  gchar *property;

  g_return_if_fail (EXPIDUS_IS_SHORTCUTS_PROVIDER (provider));
  g_return_if_fail (ESCONF_IS_CHANNEL (provider->priv->channel));
  g_return_if_fail (shortcut != NULL);

  property = g_strconcat (provider->priv->custom_base_property, "/", shortcut, NULL);

  DBG ("property = %s", property);

  esconf_channel_reset_property (provider->priv->channel, property, FALSE);
  g_free (property);
}



void
expidus_shortcuts_free (GList *shortcuts)
{
  g_list_foreach (shortcuts, (GFunc) (void (*)(void)) expidus_shortcut_free, NULL);
}



void
expidus_shortcut_free (ExpidusShortcut *shortcut)
{
  if (G_UNLIKELY (shortcut == NULL))
    return;

  g_free (shortcut->property_name);
  g_free (shortcut->shortcut);
  g_free (shortcut->command);
  g_slice_free (ExpidusShortcut, shortcut);
}
