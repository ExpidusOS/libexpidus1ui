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

#ifndef __EXPIDUS_SHORTCUTS_PROVIDER_H__
#define __EXPIDUS_SHORTCUTS_PROVIDER_H__

#include <glib-object.h>

G_BEGIN_DECLS

typedef struct _ExpidusShortcut                 ExpidusShortcut;

typedef struct _ExpidusShortcutsProviderPrivate ExpidusShortcutsProviderPrivate;
typedef struct _ExpidusShortcutsProviderClass   ExpidusShortcutsProviderClass;
typedef struct _ExpidusShortcutsProvider        ExpidusShortcutsProvider;

#define EXPIDUS_TYPE_SHORTCUTS_PROVIDER            (expidus_shortcuts_provider_get_type ())
#define EXPIDUS_SHORTCUTS_PROVIDER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), EXPIDUS_TYPE_SHORTCUTS_PROVIDER, ExpidusShortcutsProvider))
#define EXPIDUS_SHORTCUTS_PROVIDER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), EXPIDUS_TYPE_SHORTCUTS_PROVIDER, ExpidusShortcutsProviderClass))
#define EXPIDUS_IS_SHORTCUTS_PROVIDER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), EXPIDUS_TYPE_SHORTCUTS_PROVIDER))
#define EXPIDUS_IS_SHORTCUTS_PROVIDER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), EXPIDUS_TYPE_SHORTCUTS_PROVIDER)
#define EXPIDUS_SHORTCUTS_PROVIDER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), EXPIDUS_TYPE_SHORTCUTS_PROVIDER, ExpidusShortcutsProviderClass))

GType expidus_shortcuts_provider_get_type (void) G_GNUC_CONST;

ExpidusShortcutsProvider  *expidus_shortcuts_provider_new               (const gchar           *name) G_GNUC_MALLOC;
GList                  *expidus_shortcuts_provider_get_providers     (void) G_GNUC_MALLOC;
void                    expidus_shortcuts_provider_free_providers    (GList                 *providers);
const gchar            *expidus_shortcuts_provider_get_name          (ExpidusShortcutsProvider *provider);
gboolean                expidus_shortcuts_provider_is_custom         (ExpidusShortcutsProvider *provider);
void                    expidus_shortcuts_provider_reset_to_defaults (ExpidusShortcutsProvider *provider);
void                    expidus_shortcuts_provider_clone_defaults    (ExpidusShortcutsProvider *provider);
GList                  *expidus_shortcuts_provider_get_shortcuts     (ExpidusShortcutsProvider *provider);
ExpidusShortcut           *expidus_shortcuts_provider_get_shortcut      (ExpidusShortcutsProvider *provider,
                                                                   const gchar           *shortcut);
gboolean                expidus_shortcuts_provider_has_shortcut      (ExpidusShortcutsProvider *provider,
                                                                   const gchar           *shortcut);
void                    expidus_shortcuts_provider_set_shortcut      (ExpidusShortcutsProvider *provider,
                                                                   const gchar           *shortcut,
                                                                   const gchar           *command,
                                                                   gboolean               snotify);
void                    expidus_shortcuts_provider_reset_shortcut    (ExpidusShortcutsProvider *provider,
                                                                   const gchar           *shortcut);

void                    expidus_shortcuts_free                       (GList                 *shortcuts);
void                    expidus_shortcut_free                        (ExpidusShortcut          *shortcut);



struct _ExpidusShortcutsProviderClass
{
  GObjectClass __parent__;
};

struct _ExpidusShortcutsProvider
{
  GObject __parent__;

  ExpidusShortcutsProviderPrivate *priv;
};

struct _ExpidusShortcut
{
  gchar *property_name;
  gchar *shortcut;
  gchar *command;
  guint  snotify : 1;
};

G_END_DECLS

#endif /* !__EXPIDUS_SHORTCUTS_PROVIDER_H__ */
