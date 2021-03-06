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

#include <glib.h>
#include <glib-object.h>

#include <X11/Xlib.h>
#include <X11/XKBlib.h>

#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include <gdk/gdkkeysyms.h>

#include <gtk/gtk.h>

#include <libexpidus1util/libexpidus1util.h>

#include <libexpidus1kbd-private/expidus-shortcuts-grabber.h>
#include <libexpidus1kbd-private/expidus-shortcuts-marshal.h>



#define EXPIDUS_SHORTCUTS_GRABBER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), EXPIDUS_TYPE_SHORTCUTS_GRABBER, ExpidusShortcutsGrabberPrivate))
#define MODIFIERS_ERROR ((GdkModifierType)(-1))
#define MODIFIERS_NONE 0


typedef struct _ExpidusKey ExpidusKey;



static void            expidus_shortcuts_grabber_constructed      (GObject                   *object);
static void            expidus_shortcuts_grabber_finalize         (GObject                   *object);
static void            expidus_shortcuts_grabber_keys_changed     (GdkKeymap                 *keymap,
                                                                ExpidusShortcutsGrabber      *grabber);
static void            expidus_shortcuts_grabber_grab_all         (ExpidusShortcutsGrabber      *grabber);
static void            expidus_shortcuts_grabber_ungrab_all       (ExpidusShortcutsGrabber      *grabber);
static void            expidus_shortcuts_grabber_grab             (ExpidusShortcutsGrabber      *grabber,
                                                                ExpidusKey                   *key,
                                                                gboolean                   grab);
static GdkFilterReturn expidus_shortcuts_grabber_event_filter     (GdkXEvent                 *gdk_xevent,
                                                                GdkEvent                  *event,
                                                                ExpidusShortcutsGrabber      *grabber);



struct _ExpidusShortcutsGrabberPrivate
{
  GHashTable *keys;
};

typedef enum
{
  UNDEFINED_GRAB_STATE = 0, /* Initial value after g_new0(ExpidusKey) */
  NOT_GRABBED,
  GRABBED,
} ExpidusKeyGrabState;

struct _ExpidusKey
{
  guint            keyval;
  guint            modifiers;
  GArray          *keycodes;
  ExpidusKeyGrabState grab_state;
};



G_DEFINE_TYPE (ExpidusShortcutsGrabber, expidus_shortcuts_grabber, G_TYPE_OBJECT)



static void
expidus_shortcuts_grabber_class_init (ExpidusShortcutsGrabberClass *klass)
{
  GObjectClass *gobject_class;

  g_type_class_add_private (klass, sizeof (ExpidusShortcutsGrabberPrivate));

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->constructed = expidus_shortcuts_grabber_constructed;
  gobject_class->finalize = expidus_shortcuts_grabber_finalize;

  g_signal_new ("shortcut-activated",
                EXPIDUS_TYPE_SHORTCUTS_GRABBER,
                G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
                0,
                NULL,
                NULL,
                _expidus_shortcuts_marshal_VOID__STRING_INT,
                G_TYPE_NONE,
                2,
                G_TYPE_STRING, G_TYPE_INT);
}



static void
expidus_shortcuts_grabber_init (ExpidusShortcutsGrabber *grabber)
{
  GdkDisplay      *display;
  GdkKeymap       *keymap;

  grabber->priv = EXPIDUS_SHORTCUTS_GRABBER_GET_PRIVATE (grabber);
  grabber->priv->keys = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);

  /* Workaround: Make sure modmap is up to date
   * There is possibly a bug in GTK+ where virtual modifiers are not
   * mapped because the modmap is not updated. The following function
   * updates it.
   */
  display = gdk_display_get_default ();
  keymap = gdk_keymap_get_for_display (display);
  (void) gdk_keymap_have_bidi_layouts (keymap);
}



static void
expidus_shortcuts_grabber_constructed (GObject *object)
{
  GdkDisplay *display;
  GdkKeymap  *keymap;

  ExpidusShortcutsGrabber *grabber = EXPIDUS_SHORTCUTS_GRABBER (object);

  display = gdk_display_get_default ();
  keymap = gdk_keymap_get_for_display (display);
  g_signal_connect (keymap, "keys-changed", G_CALLBACK (expidus_shortcuts_grabber_keys_changed),
                    grabber);

  /* Flush events before adding the event filter */
  XAllowEvents (GDK_DISPLAY_XDISPLAY (display), AsyncBoth, CurrentTime);

  /* Add event filter */
  gdk_window_add_filter (NULL, (GdkFilterFunc) expidus_shortcuts_grabber_event_filter, grabber);
}



static void
expidus_shortcuts_grabber_finalize (GObject *object)
{
  ExpidusShortcutsGrabber *grabber = EXPIDUS_SHORTCUTS_GRABBER (object);

  expidus_shortcuts_grabber_ungrab_all (grabber);
  g_hash_table_unref (grabber->priv->keys);

  (*G_OBJECT_CLASS (expidus_shortcuts_grabber_parent_class)->finalize) (object);
}



static void
expidus_shortcuts_grabber_keys_changed (GdkKeymap            *keymap,
                                     ExpidusShortcutsGrabber *grabber)
{
  g_return_if_fail (EXPIDUS_IS_SHORTCUTS_GRABBER (grabber));

  TRACE ("Keys changed, regrabbing");

  expidus_shortcuts_grabber_grab_all (grabber);
}



static gboolean
grab_key (const gchar          *shortcut,
          ExpidusKey              *key,
          ExpidusShortcutsGrabber *grabber)
{
  expidus_shortcuts_grabber_grab (grabber, key, TRUE);
  return FALSE;
}



static void
expidus_shortcuts_grabber_grab_all (ExpidusShortcutsGrabber *grabber)
{
  g_return_if_fail (EXPIDUS_IS_SHORTCUTS_GRABBER (grabber));
  g_hash_table_foreach (grabber->priv->keys,
                        (GHFunc) (void (*)(void)) grab_key,
                        grabber);
}



static gboolean
ungrab_key (const gchar          *shortcut,
            ExpidusKey              *key,
            ExpidusShortcutsGrabber *grabber)
{
  expidus_shortcuts_grabber_grab (grabber, key, FALSE);
  return FALSE;
}



static void
expidus_shortcuts_grabber_ungrab_all (ExpidusShortcutsGrabber *grabber)
{
  g_return_if_fail (EXPIDUS_IS_SHORTCUTS_GRABBER (grabber));
  g_hash_table_foreach (grabber->priv->keys,
                        (GHFunc) (void (*)(void)) ungrab_key,
                        grabber);
}



static void
expidus_shortcuts_grabber_grab (ExpidusShortcutsGrabber *grabber,
                             ExpidusKey              *key,
                             gboolean              grab)
{
  GdkModifierType  numlock_modifier;
  GdkKeymapKey    *keys;
  GdkDisplay      *display;
  GdkKeymap       *keymap;
  gchar           *shortcut_name;
  guint            modifiers;
  guint            k;
  gint             i;
  gint             j;
  gint             n_keys;
  gint             screens;

  g_return_if_fail (EXPIDUS_IS_SHORTCUTS_GRABBER (grabber));
  g_return_if_fail (key != NULL);

  if (key->grab_state == (grab ? GRABBED : NOT_GRABBED)) {
    TRACE (grab ? "Key already grabbed" : "Key already ungrabbed");
    return;
  }
  key->grab_state = UNDEFINED_GRAB_STATE;

  display = gdk_display_get_default ();
  screens = 1;
  keymap = gdk_keymap_get_for_display (display);

  /* Map virtual modifiers to non-virtual modifiers */
  modifiers = key->modifiers;
  gdk_keymap_map_virtual_modifiers (keymap, &modifiers);

  /* Debugging information */
  shortcut_name = gtk_accelerator_name (key->keyval, modifiers);

  TRACE (grab ? "Grabbing %s" : "Ungrabbing %s", shortcut_name);
  TRACE ("Keyval: %d", key->keyval);
  TRACE ("Modifiers: 0x%x", modifiers);

  g_free (shortcut_name);

  if (modifiers == key->modifiers &&
      (GDK_SUPER_MASK | GDK_HYPER_MASK | GDK_META_MASK) & modifiers)
    {
      TRACE ("Failed to map virtual modifiers");
      return;
    }

  /* Get all keys generating keyval */
  if (!gdk_keymap_get_entries_for_keyval (keymap,key->keyval,
                                          &keys, &n_keys))
    {
      TRACE ("Got no keys for keyval");
      return;
    }

  if (n_keys == 0)
    {
      g_free (keys);

      TRACE ("Got 0 keys for keyval");
      return;
    }

  numlock_modifier =
    XkbKeysymToModifiers (GDK_DISPLAY_XDISPLAY (display), GDK_KEY_Num_Lock);

  key->grab_state = (grab ? GRABBED : NOT_GRABBED);
  for (i = 0; i < n_keys; i ++)
    {
      /* Grab all hardware keys generating keyval */

      TRACE ("Keycode: %d", keys[i].keycode);

      for (j = 0; j < screens; j++)
        {
          /* Do the grab on all screens */
          Window root_window;

          /* Ignorable modifiers */
          guint mod_masks [] = {
            0,
            GDK_MOD2_MASK,
            numlock_modifier | GDK_MOD2_MASK,
            GDK_LOCK_MASK,
            numlock_modifier | GDK_LOCK_MASK,
            GDK_MOD5_MASK,
            numlock_modifier | GDK_MOD5_MASK,
            GDK_MOD2_MASK | GDK_LOCK_MASK,
            numlock_modifier | GDK_MOD2_MASK | GDK_LOCK_MASK,
            GDK_MOD2_MASK | GDK_MOD5_MASK,
            numlock_modifier | GDK_MOD2_MASK | GDK_MOD5_MASK,
            GDK_LOCK_MASK | GDK_MOD5_MASK,
            numlock_modifier | GDK_LOCK_MASK | GDK_MOD5_MASK,
            GDK_MOD2_MASK | GDK_LOCK_MASK | GDK_MOD5_MASK,
            numlock_modifier | GDK_MOD2_MASK | GDK_LOCK_MASK | GDK_MOD5_MASK,
          };

          /* Retrieve the root window of the screen */
          root_window = GDK_WINDOW_XID (gdk_screen_get_root_window (gdk_display_get_default_screen (display)));
          gdk_x11_display_error_trap_push (display);

          for (k = 0; k < G_N_ELEMENTS (mod_masks); k++)
            {
              /* Take ignorable modifiers into account when grabbing */
              if (grab)
                XGrabKey (GDK_DISPLAY_XDISPLAY (display),
                          keys[i].keycode,
                          modifiers | mod_masks [k],
                          root_window,
                          False,
                          GrabModeAsync,
                          GrabModeAsync);
              else
                {
                  if (i >= (gint) key->keycodes->len)
                    break;
                  XUngrabKey (GDK_DISPLAY_XDISPLAY (display),
                              g_array_index (key->keycodes, guint, i),
                              modifiers | mod_masks [k],
                              root_window);
                }
            }

          gdk_display_flush (display);

          if (gdk_x11_display_error_trap_pop (display))
            {
              TRACE (grab ? "Failed to grab" : "Failed to ungrab");
              key->grab_state = UNDEFINED_GRAB_STATE;
            }
        }
      /* Remember the old keycode, as we need it to ungrab. */
      if (grab)
        g_array_append_val (key->keycodes, keys[i].keycode);
      else
        g_array_index (key->keycodes, guint, i) = UINT_MAX;
    }

  /* Cleanup elements containing UINT_MAX from the key->keycodes array */
  for (i = key->keycodes->len - 1; i >= 0; i --)
    {
      if (g_array_index (key->keycodes, guint, i) == UINT_MAX)
        g_array_remove_index_fast (key->keycodes, i);
    }

  g_free (keys);
}



struct EventKeyFindContext
{
  ExpidusShortcutsGrabber *grabber;
  GdkModifierType       modifiers;
  guint                 keyval;
  const gchar          *result;
};



static gboolean
find_event_key (const gchar                *shortcut,
                ExpidusKey                    *key,
                struct EventKeyFindContext *context)
{
  g_return_val_if_fail (context != NULL, FALSE);

  TRACE ("Comparing to %s", shortcut);

  if ((key->modifiers & (GDK_CONTROL_MASK | GDK_SHIFT_MASK | GDK_MOD1_MASK | GDK_SUPER_MASK))
      == (context->modifiers)
      && (key->keyval == context->keyval))
    {
      context->result = shortcut;

      TRACE ("Positive match for %s", context->result);
      return TRUE;
    }

  return FALSE;
}



static GdkFilterReturn
expidus_shortcuts_grabber_event_filter (GdkXEvent            *gdk_xevent,
                                     GdkEvent             *event,
                                     ExpidusShortcutsGrabber *grabber)
{
  struct EventKeyFindContext  context;
  GdkKeymap                  *keymap;
  GdkModifierType             consumed, modifiers;
  GdkDisplay                 *display;
  XEvent                     *xevent;
  guint                       keyval, mod_mask;
  gchar                      *raw_shortcut_name;
  gint                        timestamp;

  g_return_val_if_fail (EXPIDUS_IS_SHORTCUTS_GRABBER (grabber), GDK_FILTER_CONTINUE);

  xevent = (XEvent *) gdk_xevent;

  if (xevent->type != KeyPress)
    return GDK_FILTER_CONTINUE;

  context.grabber = grabber;
  context.result = NULL;
  timestamp = xevent->xkey.time;

  /* Get the keyboard state */
  display = gdk_display_get_default ();
  gdk_x11_display_error_trap_push (display);
  keymap = gdk_keymap_get_for_display (display);
  mod_mask = gtk_accelerator_get_default_mod_mask ();
  modifiers = xevent->xkey.state;

  gdk_keymap_translate_keyboard_state (keymap, xevent->xkey.keycode,
                                       modifiers,
                                       XkbGroupForCoreState (xevent->xkey.state),
                                       &keyval, NULL, NULL, &consumed);

  /* We want Alt + Print to be Alt + Print not SysReq. See bug #7897 */
  if (keyval == GDK_KEY_Sys_Req && (modifiers & GDK_MOD1_MASK) != 0)
    {
      consumed = 0;
      keyval = GDK_KEY_Print;
    }

  /* Get the modifiers */

  /* If Shift was used when translating the keyboard state, we remove it
   * from the consumed bit because gtk_accelerator_{name,parse} fail to
   * handle this correctly. This allows us to have shortcuts with Shift
   * as a modifier key (see bug #8744). */
  if ((modifiers & GDK_SHIFT_MASK) && (consumed & GDK_SHIFT_MASK))
    consumed &= ~GDK_SHIFT_MASK;

  /*
   * !!! FIX ME !!!
   * Turn MOD4 into SUPER key press events. Although it is not clear if
   * this is a proper solution, it fixes bug #10373 which some people
   * experience without breaking functionality for other users.
   */
  if (modifiers & GDK_MOD4_MASK)
    {
      modifiers &= ~GDK_MOD4_MASK;
      modifiers |= GDK_SUPER_MASK;
      consumed &= ~GDK_MOD4_MASK;
      consumed &= ~GDK_SUPER_MASK;
    }

  modifiers &= ~consumed;
  modifiers &= mod_mask;

  /* Use the keyval and modifiers values of gtk_accelerator_parse. We
   * will compare them with values we also get from this function and as
   * it has its own logic, it's easier and safer to do so.
   * See bug #8744 for a "live" example. */
  raw_shortcut_name = gtk_accelerator_name (keyval, modifiers);
  gtk_accelerator_parse (raw_shortcut_name, &context.keyval, &context.modifiers);

  TRACE ("Looking for %s", raw_shortcut_name);
  g_free (raw_shortcut_name);

  g_hash_table_foreach (grabber->priv->keys,
                        (GHFunc) (void (*)(void)) find_event_key,
                        &context);

  if (G_LIKELY (context.result != NULL))
    /* We had a positive match */
    g_signal_emit_by_name (grabber, "shortcut-activated",
                           context.result, timestamp);

  gdk_display_flush (display);
  gdk_x11_display_error_trap_pop_ignored (display);

  return GDK_FILTER_CONTINUE;
}



ExpidusShortcutsGrabber *
expidus_shortcuts_grabber_new (void)
{
  return g_object_new (EXPIDUS_TYPE_SHORTCUTS_GRABBER, NULL);
}



void
expidus_shortcuts_grabber_add (ExpidusShortcutsGrabber *grabber,
                            const gchar          *shortcut)
{
  ExpidusKey *key;

  g_return_if_fail (EXPIDUS_IS_SHORTCUTS_GRABBER (grabber));
  g_return_if_fail (shortcut != NULL);

  key = g_new0 (ExpidusKey, 1);
  key->keycodes = g_array_new (FALSE, TRUE, sizeof (guint));

  gtk_accelerator_parse (shortcut, &key->keyval, &key->modifiers);

  if (G_LIKELY (key->keyval != 0))
    {
      expidus_shortcuts_grabber_grab (grabber, key, TRUE);
      g_hash_table_insert (grabber->priv->keys, g_strdup (shortcut), key);
    }
  else
    {
      g_array_free (key->keycodes, TRUE);
      g_free (key);
    }
}



void
expidus_shortcuts_grabber_remove (ExpidusShortcutsGrabber *grabber,
                               const gchar          *shortcut)
{
  ExpidusKey *key;

  g_return_if_fail (EXPIDUS_IS_SHORTCUTS_GRABBER (grabber));
  g_return_if_fail (shortcut != NULL);

  key = g_hash_table_lookup (grabber->priv->keys, shortcut);

  if (G_LIKELY (key != NULL))
    {
      expidus_shortcuts_grabber_grab (grabber, key, FALSE);
      g_hash_table_remove (grabber->priv->keys, shortcut);
    }
}
