/*
 * Copyright (c) 2020 The Expidus Development Team
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

#ifndef __EXPIDUS_FILENAME_INPUT_H__
#define __EXPIDUS_FILENAME_INPUT_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

typedef struct _ExpidusFilenameInputClass ExpidusFilenameInputClass;
typedef struct _ExpidusFilenameInput      ExpidusFilenameInput;

#define EXPIDUS_TYPE_FILENAME_INPUT             (expidus_filename_input_get_type())
#define EXPIDUS_FILENAME_INPUT(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj), EXPIDUS_TYPE_FILENAME_INPUT, ExpidusFilenameInput))
#define EXPIDUS_IS_FILENAME_INPUT(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj), EXPIDUS_TYPE_FILENAME_INPUT))
#define EXPIDUS_FILENAME_INPUT_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass),  EXPIDUS_TYPE_FILENAME_INPUT, ExpidusFilenameInputClass))
#define EXPIDUS_IS_FILENAME_INPUT_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass),  EXPIDUS_TYPE_FILENAME_INPUT))
#define EXPIDUS_FILENAME_INPUT_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj),  EXPIDUS_TYPE_FILENAME_INPUT, ExpidusFilenameInputClass))

GType        expidus_filename_input_get_type              (void) G_GNUC_CONST;

const gchar *expidus_filename_input_get_text              (ExpidusFilenameInput *filename_input);

void         expidus_filename_input_check                 (ExpidusFilenameInput *filename_input);

GtkEntry    *expidus_filename_input_get_entry             (ExpidusFilenameInput *filename_input);

void         expidus_filename_input_sensitise_widget      (GtkWidget         *widget);

void         expidus_filename_input_desensitise_widget    (GtkWidget         *widget);


G_END_DECLS

#endif /* !__EXPIDUS_FILENAME_INPUT_H__ */
