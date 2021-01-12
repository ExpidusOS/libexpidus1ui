/*
 * Copyright (c) 2006 Benedikt Meurer <benny@xfce.org>.
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
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#if !defined (LIBEXPIDUS1UI_INSIDE_LIBEXPIDUS1UI_H) && !defined (LIBEXPIDUS1UI_COMPILATION)
#error "Only <libexpidus1ui/libexpidus1ui.h> can be included directly, this file is not part of the public API."
#endif

#ifndef __EXPIDUS_TITLED_DIALOG_H__
#define __EXPIDUS_TITLED_DIALOG_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

typedef struct _ExpidusTitledDialogPrivate ExpidusTitledDialogPrivate;
typedef struct _ExpidusTitledDialogClass   ExpidusTitledDialogClass;
typedef struct _ExpidusTitledDialog        ExpidusTitledDialog;

#define EXPIDUS_TYPE_TITLED_DIALOG             (expidus_titled_dialog_get_type ())
#define EXPIDUS_TITLED_DIALOG(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), EXPIDUS_TYPE_TITLED_DIALOG, ExpidusTitledDialog))
#define EXPIDUS_TITLED_DIALOG_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), EXPIDUS_TYPE_TITLED_DIALOG, ExpidusTitledDialogClass))
#define EXPIDUS_IS_TITLED_DIALOG(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), EXPIDUS_TYPE_TITLED_DIALOG))
#define EXPIDUS_IS_TITLED_DIALOG_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), EXPIDUS_TYPE_TITLED_DIALOG))
#define EXPIDUS_TITLED_DIALOG_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), EXPIDUS_TYPE_TITLED_DIALOG, ExpidusTitledDialogClass))

struct _ExpidusTitledDialogClass
{
  /*< private >*/
  GtkDialogClass __parent__;

  /* reserved for future expansion */
  void (*reserved0) (void);
  void (*reserved1) (void);
  void (*reserved2) (void);
  void (*reserved3) (void);
  void (*reserved4) (void);
  void (*reserved5) (void);
};

/**
 * ExpidusTitledDialog:
 *
 * An opaque struct with only private fields.
 **/
struct _ExpidusTitledDialog
{
  /*< private >*/
  GtkDialog                __parent__;
  ExpidusTitledDialogPrivate *priv;
};

GType                 expidus_titled_dialog_get_type         (void) G_GNUC_CONST;

GtkWidget            *expidus_titled_dialog_new              (void) G_GNUC_MALLOC;
GtkWidget            *expidus_titled_dialog_new_with_buttons (const gchar      *title,
                                                           GtkWindow        *parent,
                                                           GtkDialogFlags    flags,
                                                           const gchar      *first_button_text,
                                                           ...) G_GNUC_MALLOC;

GtkWidget            *expidus_titled_dialog_new_with_mixed_buttons (const gchar    *title,
                                                                 GtkWindow      *parent,
                                                                 GtkDialogFlags  flags,
                                                                 const gchar    *first_button_icon_name,
                                                                 const gchar    *first_button_text,
                                                                 ...) G_GNUC_MALLOC;

void                  expidus_titled_dialog_create_action_area   (ExpidusTitledDialog *titled_dialog);
GtkWidget            *expidus_titled_dialog_add_button           (ExpidusTitledDialog *titled_dialog,
                                                               const gchar      *button_text,
                                                               gint              response_id);
void                  expidus_titled_dialog_add_action_widget    (ExpidusTitledDialog *titled_dialog,
                                                               GtkWidget        *child,
                                                               gint              response_id);
void                  expidus_titled_dialog_set_default_response (ExpidusTitledDialog *titled_dialog,
                                                               gint              response_id);
const gchar          *expidus_titled_dialog_get_subtitle         (ExpidusTitledDialog *titled_dialog);
void                  expidus_titled_dialog_set_subtitle         (ExpidusTitledDialog *titled_dialog,
                                                               const gchar      *subtitle);

G_END_DECLS

#endif /* !__EXPIDUS_TITLED_DIALOG_H__ */
