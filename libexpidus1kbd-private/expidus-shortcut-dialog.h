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

#ifndef __EXPIDUS_SHORTCUT_DIALOG_H__
#define __EXPIDUS_SHORTCUT_DIALOG_H__

#include <gtk/gtk.h>

#include <libexpidus1kbd-private/expidus-shortcuts.h>

G_BEGIN_DECLS

typedef struct _ExpidusShortcutDialogClass ExpidusShortcutDialogClass;
typedef struct _ExpidusShortcutDialog      ExpidusShortcutDialog;

#define EXPIDUS_TYPE_SHORTCUT_DIALOG            (expidus_shortcut_dialog_get_type ())
#define EXPIDUS_SHORTCUT_DIALOG(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), EXPIDUS_TYPE_SHORTCUT_DIALOG, ExpidusShortcutDialog))
#define EXPIDUS_SHORTCUT_DIALOG_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), EXPIDUS_TYPE_SHORTCUT_DIALOG, ExpidusShortcutDialogClass))
#define EXPIDUS_IS_SHORTCUT_DIALOG(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), EXPIDUS_TYPE_SHORTCUT_DIALOG))
#define EXPIDUS_IS_SHORTCUT_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), EXPIDUS_TYPE_SHORTCUT_DIALOG))
#define EXPIDUS_SHORTCUT_DIALOG_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), EXPIDUS_TYPE_SHORTCUT_DIALOG, ExpidusShortcutDialogClass))

GType        expidus_shortcut_dialog_get_type        (void) G_GNUC_CONST;

GtkWidget   *expidus_shortcut_dialog_new             (const gchar        *provider,
                                                   const gchar        *action_name,
                                                   const gchar        *action) G_GNUC_MALLOC;
gint         expidus_shortcut_dialog_run             (ExpidusShortcutDialog *dialog,
                                                   GtkWidget          *parent);
const gchar *expidus_shortcut_dialog_get_shortcut    (ExpidusShortcutDialog *dialog);
const gchar *expidus_shortcut_dialog_get_action      (ExpidusShortcutDialog *action);
const gchar *expidus_shortcut_dialog_get_action_name (ExpidusShortcutDialog *action);

G_END_DECLS

#endif /* !__EXPIDUS_SHORTCUT_DIALOG_H__ */
