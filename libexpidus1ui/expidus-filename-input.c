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

/**
 * SECTION:expidus-filename-input
 * @title: ExpidusFilenameInput
 * @short_description: widget for filename input
 * @stability: Stable
 * @include: libexpidus1ui/libexpidus1ui.h
 *
 * A widget to allow filename input for creating or renaming files,
 * with as-you-type checking for invalid filenames.
 **/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif
#ifdef HAVE_STDARG_H
#include <stdarg.h>
#endif
#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif

#include <gtk/gtk.h>
#include <libexpidus1util/libexpidus1util.h>

#include <libexpidus1ui/expidus-filename-input.h>
#include <libexpidus1ui/libexpidus1ui-private.h>
#include <libexpidus1ui/libexpidus1ui-alias.h>

/* Property identifiers */
enum
{
  PROP_0,
  PROP_ORIGINAL_FILENAME,
  PROP_MAX_TEXT_LENGTH,
  N_PROPERTIES
};

/* Signal identifiers */
enum
{
  SIG_TEXT_VALID = 0,
  SIG_TEXT_INVALID,
  N_SIGS
};



static void     expidus_filename_input_set_property  (GObject      *object,
                                                   guint         prop_id,
                                                   const GValue *value,
                                                   GParamSpec   *pspec);
static void     expidus_filename_input_finalize      (GObject      *object);
static void     expidus_filename_input_entry_changed (GtkEditable  *editable,
                                                   gpointer      data);
static gboolean expidus_filename_input_entry_undo    (GtkWidget    *widget,
                                                   GdkEvent     *event,
                                                   gpointer      data);



struct _ExpidusFilenameInputClass
{
  GtkBoxClass parent;

  /* signals */
  void (*text_valid)   (ExpidusFilenameInput *filename_input);
  void (*text_invalid) (ExpidusFilenameInput *filename_input);
};

struct _ExpidusFilenameInput
{
  GtkBox    parent;

  GtkEntry *entry;
  GtkLabel *label;

  GRegex   *whitespace_regex;
  GRegex   *dir_sep_regex;

  gint     max_text_length;
  gchar    *original_filename;
};

static guint signals[N_SIGS];

G_DEFINE_TYPE (ExpidusFilenameInput, expidus_filename_input, GTK_TYPE_BOX)



static void
expidus_filename_input_class_init (ExpidusFilenameInputClass *klass)
{
  GObjectClass *gobject_class = (GObjectClass *)klass;

  gobject_class->finalize = expidus_filename_input_finalize;
  gobject_class->set_property = expidus_filename_input_set_property;

  /**
   * ExpidusFilenameInput:original-filename:
   *
   * The original name of the file, to be used as the initial text
   * displayed in the GtkEntry. A NULL value indicates no original
   * filename.
   *
   * Since: 4.16
   *
   **/
  g_object_class_install_property (gobject_class,
      PROP_ORIGINAL_FILENAME,
      g_param_spec_string ("original-filename",
                           "original-filename",
                           "The original filename",
                           NULL,
                           G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS));

  /**
   * ExpidusFilenameInput:max-text-length:
   *
   * The maximum permitted length of a filename. A value of -1
   * indicates no maximum length.
   *
   * Since: 4.16
   *
   **/
  g_object_class_install_property (gobject_class,
      PROP_MAX_TEXT_LENGTH,
      g_param_spec_int ("max-text-length",
                        "max-text-length",
                        "Maximum permitted length of a filename",
                        -1, G_MAXINT, -1,
                        G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS));

  /**
   * ExpidusFilenameInput::text-valid:
   * @filename_input: An #ExpidusFilenameInput
   *
   * Signals that the current text is a valid filename. This signal is
   * emitted whenever the user changes the text and the result is a valid
   * filename.
   *
   * Since: 4.16
   *
   **/
  signals[SIG_TEXT_VALID] = g_signal_new  ("text-valid",
                                           G_TYPE_FROM_CLASS (klass),
                                           G_SIGNAL_RUN_LAST,
                                           G_STRUCT_OFFSET (ExpidusFilenameInputClass,
                                                            text_valid),
                                           NULL, NULL, NULL,
                                           G_TYPE_NONE, 0);

  /**
   * ExpidusFilenameInput::text-invalid:
   * @filename_input: An #ExpidusFilenameInput
   *
   * Signals that the current text is not a valid filename. This signal is
   * emitted whenever the user changes the text and the result is not a valid
   * filename.
   *
   * Since: 4.16
   *
   **/
  signals[SIG_TEXT_VALID] = g_signal_new  ("text-invalid",
                                           G_TYPE_FROM_CLASS (klass),
                                           G_SIGNAL_RUN_LAST,
                                           G_STRUCT_OFFSET (ExpidusFilenameInputClass,
                                                            text_invalid),
                                           NULL, NULL, NULL,
                                           G_TYPE_NONE, 0);
}

static void
expidus_filename_input_init (ExpidusFilenameInput *filename_input)
{
  GError *err = NULL;

  /* by default there is no maximum length for the filename and no original filename */
  filename_input->max_text_length = -1;
  filename_input->original_filename = NULL;

  /* compile the regular expressions used to check the input */
  /* the pattern for whitespace_regex matches if the text starts or ends with whitespace */
  filename_input->whitespace_regex = g_regex_new ("^\\s|\\s$", 0, 0, &err);
  filename_input->dir_sep_regex = g_regex_new (G_DIR_SEPARATOR_S, 0, 0, &err);

  gtk_orientable_set_orientation (GTK_ORIENTABLE (filename_input), GTK_ORIENTATION_VERTICAL);
  gtk_container_set_border_width (GTK_CONTAINER (filename_input), 2);

  /* set up the GtkEntry for the input */
  filename_input->entry = GTK_ENTRY (gtk_entry_new());
  gtk_widget_set_hexpand (GTK_WIDGET (filename_input->entry), TRUE);
  gtk_widget_set_valign (GTK_WIDGET (filename_input->entry), GTK_ALIGN_CENTER);
  gtk_box_pack_start (GTK_BOX (filename_input), GTK_WIDGET (filename_input->entry), FALSE, FALSE, 0);

  /* set up the GtkLabel to display any error or warning messages */
  filename_input->label = GTK_LABEL (gtk_label_new(""));
  gtk_label_set_xalign (filename_input->label, 0.0f);
  gtk_widget_set_hexpand (GTK_WIDGET (filename_input->label), TRUE);
  gtk_box_pack_start (GTK_BOX (filename_input), GTK_WIDGET (filename_input->label), FALSE, FALSE, 0);

  /* allow reverting the filename with ctrl + z */
  g_signal_connect (filename_input->entry, "key-press-event",
                    G_CALLBACK (expidus_filename_input_entry_undo), filename_input);

  /* set up a callback to check the input text whenever it is changed*/
  g_signal_connect (filename_input->entry, "changed",
                    G_CALLBACK (expidus_filename_input_entry_changed), filename_input);
}

static void
expidus_filename_input_set_property (GObject      *object,
                                  guint         prop_id,
                                  const GValue *value,
                                  GParamSpec   *pspec)
{
  ExpidusFilenameInput *filename_input = EXPIDUS_FILENAME_INPUT (object);
  const gchar       *filename;

  switch (prop_id)
    {
    case PROP_ORIGINAL_FILENAME:
      filename =  g_value_get_string (value);
      if (filename == NULL)
        return;

      filename_input->original_filename = g_strdup (filename);
      gtk_entry_set_text (filename_input->entry, filename_input->original_filename);

      break;
    case PROP_MAX_TEXT_LENGTH:
      filename_input->max_text_length = g_value_get_int (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
expidus_filename_input_finalize (GObject *object)
{
  ExpidusFilenameInput *filename_input = EXPIDUS_FILENAME_INPUT (object);

  g_regex_unref (filename_input->whitespace_regex);
  g_regex_unref (filename_input->dir_sep_regex);

  g_free (filename_input->original_filename);

  (*G_OBJECT_CLASS (expidus_filename_input_parent_class)->finalize) (object);
}

/**
 * expidus_filename_input_get_text:
 * @filename_input     : a #ExpidusFilenameInput instance.
 *
 * Gets the current text of the widget.
 *
 * Returns: (transfer none): the string representing the current text
 *
 * Since: 4.16
 *
 **/
const gchar*
expidus_filename_input_get_text (ExpidusFilenameInput *filename_input)
{
  g_return_val_if_fail (EXPIDUS_IS_FILENAME_INPUT (filename_input), NULL);

  /* NB the returned string must not be modified or freed,
     as it belongs to the GtkEntry */
  return gtk_entry_get_text (filename_input->entry);
}

/**
 * expidus_filename_input_check:
 * @filename_input     : a #ExpidusFilenameInput instance.
 *
 * Forces a check of the current input text even when it has not changed.
 * This is useful to force the appropriate signal to be sent to indicate
 * whether the text is a valid filename or not, so that for example any
 * GtkWidgets whose sensitivity is controlled by this can be correctly updated
 * when they are first created.
 *
 * Since: 4.16
 *
 **/
void
expidus_filename_input_check (ExpidusFilenameInput *filename_input)
{
  g_return_if_fail (EXPIDUS_IS_FILENAME_INPUT (filename_input));

  g_signal_emit_by_name (filename_input->entry, "changed", 0);
}

/**
 * expidus_filename_input_get_entry:
 * @filename_input     : a #ExpidusFilenameInput instance.
 *
 * Gets the #GtkEntry associated to filename_input
 *
 * Returns: (transfer none): A #GtkEntry
 *
 * Since: 4.16
 *
 **/
GtkEntry*
expidus_filename_input_get_entry (ExpidusFilenameInput *filename_input)
{
  g_return_val_if_fail (EXPIDUS_IS_FILENAME_INPUT (filename_input), NULL);

  return filename_input->entry;
}

/**
 * expidus_filename_input_sensitise_widget:
 * @widget     : a #GtkWidget
 *
 * A convenience function to be connected as a callback for the  "text-valid" signal
 * (for example, using g_connect_swapped) for the simple case where the desired effect
 * of this signal is to set the sensitivity of a single GtkWidget (for example, a GtkButton).
 *
 * Since: 4.16
 *
 **/
void
expidus_filename_input_sensitise_widget (GtkWidget *widget)
{
  gtk_widget_set_sensitive (widget, TRUE);
}

/**
 * expidus_filename_input_desensitise_widget:
 * @widget     : a #GtkWidget
 *
 * A convenience function to be connected as a callback for the  "text-invalid" signal
 * (for example, using g_connect_swapped) for the simple case where the desired effect
 * of this signal is to set the sensitivity of a single GtkWidget (for example, a GtkButton).
 *
 * Since: 4.16
 *
 **/
void
expidus_filename_input_desensitise_widget (GtkWidget *widget)
{
  gtk_widget_set_sensitive (widget, FALSE);
}

static void
expidus_filename_input_entry_changed (GtkEditable *editable,
                                   gpointer     data)
{
  ExpidusFilenameInput *filename_input;
  GtkEntry          *entry;
  GtkLabel          *label;

  gint               text_length;
  const gchar       *text;
  const gchar       *label_text = "";
  const gchar       *icon_name = NULL;
  gboolean           new_text_valid = TRUE;
  gboolean           match_ws, match_ds;

  g_return_if_fail (GTK_IS_ENTRY (editable));
  entry = GTK_ENTRY (editable);

  g_return_if_fail (EXPIDUS_IS_FILENAME_INPUT (data));
  filename_input = EXPIDUS_FILENAME_INPUT (data);
  label = filename_input->label;

  /* get the string representing the current text of the GtkEntry */
  text_length = gtk_entry_get_text_length (entry);
  text = gtk_entry_get_text (entry); /* NB this string must not be modified or freed,
                                        as it belongs to the GtkEntry */

  /*
   * check whether the string starts or ends with whitespace, or contains the directory
   * separator
   */
  match_ws = g_regex_match (filename_input->whitespace_regex, text, 0, NULL);
  match_ds = g_regex_match (filename_input->dir_sep_regex, text, 0, NULL);

  if (text_length == 0)
    {
      /* the string is empty */
      icon_name = NULL;
      label_text = "";
      new_text_valid = FALSE;
    }
  else if (match_ds)
    {
      /* the string contains a directory separator */
      label_text = _("Directory separator illegal in file name");
      icon_name = "dialog-error";
      new_text_valid = FALSE;
    }
  else if (filename_input->max_text_length != -1 && /* max_text_length = -1 means no maximum */
          text_length > filename_input->max_text_length)
    {
      /* the string is too long */
      label_text = _("Filename is too long");
      icon_name = "dialog-error";
      new_text_valid = FALSE;
    }
  else if (match_ws)
    {
      /* the string starts or ends with whitespace
       * this does not make the filename invalid, but we warn the user about it */
      label_text = _("Filenames should not start or end with a space");
      icon_name = "dialog-warning";
      new_text_valid = TRUE;
    }

  /* update the icon in the GtkEntry and the message in the GtkLabel */
  gtk_entry_set_icon_from_icon_name (entry,
                                     GTK_ENTRY_ICON_SECONDARY,
                                     icon_name);
  gtk_label_set_text (label, label_text);

  /* send a signal to indicate whether the filename is valid */
  gtk_entry_set_activates_default (entry, new_text_valid);
  if (new_text_valid)
    g_signal_emit_by_name (filename_input, "text-valid", 0);
  else
    g_signal_emit_by_name (filename_input, "text-invalid", 0);
}

static gboolean
expidus_filename_input_entry_undo (GtkWidget  *widget,
                                GdkEvent   *event,
                                gpointer    data)
{
  guint              keyval;
  GdkModifierType    state;
  ExpidusFilenameInput *filename_input;

  g_return_val_if_fail (EXPIDUS_IS_FILENAME_INPUT (data), GDK_EVENT_PROPAGATE);
  filename_input = EXPIDUS_FILENAME_INPUT (data);

  /* if there is no original filename to restore, we are done */
  if (filename_input->original_filename == NULL)
    return GDK_EVENT_PROPAGATE;

  /* extract the keyval and state from the event */
  if (G_UNLIKELY (!gdk_event_get_keyval (event, &keyval) ||
                  !gdk_event_get_state (event, &state)))
    return GDK_EVENT_PROPAGATE;

  /* if the user pressed ctrl + z, reset the text to the original filename */
  if ((state & GDK_CONTROL_MASK) != 0 && keyval == GDK_KEY_z)
    {
      gtk_entry_set_text (GTK_ENTRY (widget),
                          filename_input->original_filename);
      return GDK_EVENT_STOP;
    }

  return GDK_EVENT_PROPAGATE;
}

#define __EXPIDUS_FILENAME_INPUT_C__
#include <libexpidus1ui/libexpidus1ui-aliasdef.c>
