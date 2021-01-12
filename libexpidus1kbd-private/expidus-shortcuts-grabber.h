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

#ifndef __EXPIDUS_SHORTCUTS_GRABBER_H__
#define __EXPIDUS_SHORTCUTS_GRABBER_H__

#include <glib-object.h>

G_BEGIN_DECLS

typedef struct _ExpidusShortcutsGrabberPrivate ExpidusShortcutsGrabberPrivate;
typedef struct _ExpidusShortcutsGrabberClass   ExpidusShortcutsGrabberClass;
typedef struct _ExpidusShortcutsGrabber        ExpidusShortcutsGrabber;

#define EXPIDUS_TYPE_SHORTCUTS_GRABBER            (expidus_shortcuts_grabber_get_type ())
#define EXPIDUS_SHORTCUTS_GRABBER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), EXPIDUS_TYPE_SHORTCUTS_GRABBER, ExpidusShortcutsGrabber))
#define EXPIDUS_SHORTCUTS_GRABBER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), EXPIDUS_TYPE_SHORTCUTS_GRABBER, ExpidusShortcutsGrabberClass))
#define EXPIDUS_IS_SHORTCUTS_GRABBER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), EXPIDUS_TYPE_SHORTCUTS_GRABBER))
#define EXPIDUS_IS_SHORTCUTS_GRABBER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), EXPIDUS_TYPE_SHORTCUTS_GRABBER)
#define EXPIDUS_SHORTCUTS_GRABBER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), EXPIDUS_TYPE_SHORTCUTS_GRABBER, ExpidusShortcutsGrabberClass))

GType                 expidus_shortcuts_grabber_get_type      (void) G_GNUC_CONST;

ExpidusShortcutsGrabber *expidus_shortcuts_grabber_new           (void) G_GNUC_MALLOC;
void                  expidus_shortcuts_grabber_add           (ExpidusShortcutsGrabber *grabber,
                                                            const gchar          *shortcut);
void                  expidus_shortcuts_grabber_remove        (ExpidusShortcutsGrabber *grabber,
                                                            const gchar          *shortcut);



struct _ExpidusShortcutsGrabberClass
{
  GObjectClass __parent__;
};

struct _ExpidusShortcutsGrabber
{
  GObject __parent__;

  ExpidusShortcutsGrabberPrivate *priv;
};

G_END_DECLS

#endif /* !__EXPIDUS_SHORTCUTS_GRABBER_H__ */
