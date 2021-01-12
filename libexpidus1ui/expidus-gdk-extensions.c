/*
 * Copyright (c) 2003-2007 Benedikt Meurer <benny@xfce.org>
 * Copyright (c) 2007      The Expidus Development Team
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
 * SECTION:expidus-gdk-extensions
 * @title: Gdk Extensions
 * @short_description: various extensions to Gdk
 * @stability: Stable
 * @include: libexpidus1ui/libexpidus1ui.h
 *
 * Common used functions that are not provided by the Gdk or Gdk-pixbuf library.
 **/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <libexpidus1ui/expidus-gdk-extensions.h>
#include <libexpidus1ui/libexpidus1ui-private.h>
#include <libexpidus1ui/libexpidus1ui-alias.h>

#ifdef GDK_WINDOWING_X11
#include <gdk/gdkx.h>
#endif



/**
 * expidus_gdk_screen_get_active:
 * @monitor_return : (out) (allow-none): Address to store the monitor number to or %NULL.
 *                   Under gtk3 this will always be set to '0'.
 *
 * Returns the currently active #GdkScreen, that is, the screen which
 * currently contains the pointer. If no active screen was found, the
 * default #GdkScreen is returned.
 *
 * Return value: (transfer full): the currently active #GdkScreen.
 **/
GdkScreen *
expidus_gdk_screen_get_active (gint *monitor_return)
{
  GdkDisplay       *display;
  gint              rootx, rooty;
  GdkScreen        *screen;

  GdkSeat *seat;

  display = gdk_display_get_default ();
  seat = gdk_display_get_default_seat (display);
  gdk_device_get_position (gdk_seat_get_pointer (seat), &screen, &rootx, &rooty);

  if (G_UNLIKELY (screen == NULL))
    {
      screen = gdk_screen_get_default ();
      if (monitor_return != NULL)
        *monitor_return = 0;
    }
  else
    {
      /* return the monitor number */
      if (monitor_return != NULL)
        *monitor_return = 0;
    }

  return screen;
}


/**
 * expidus_gdk_screen_get_geometry:
 *
 * Returns the width and height of the default #GdkScreen.
 * This is a replacement for gdk_screen_width/gdk_screen_height.
 *
 * Return value: (transfer full): a newly created #GdkRectangle
 * containing the width and height of the screen.
 *
 * Since: 4.14
 **/
GdkRectangle *
expidus_gdk_screen_get_geometry (void)
{
  GdkRectangle *geometry;
  gint x, y, w, h;
  int num_monitors;
  GdkDisplay *display;
  GdkRectangle rect;
  GdkMonitor *monitor;

  geometry = g_new0 (GdkRectangle, 1);

  display = gdk_display_get_default ();
  num_monitors = gdk_display_get_n_monitors (display);

  x = y = G_MAXINT;
  w = h = G_MININT;

  for (int i = 0; i < num_monitors; i++)
    {
      monitor = gdk_display_get_monitor (display, i);
      gdk_monitor_get_geometry (monitor, &rect);

      x = MIN (x, rect.x);
      y = MIN (y, rect.y);
      w = MAX (w, rect.x + rect.width);
      h = MAX (h, rect.y + rect.height);
    }

  geometry->width = w - x;
  geometry->height = h - y;

  return geometry;
}


#define __EXPIDUS_GDK_EXTENSIONS_C__
#include <libexpidus1ui/libexpidus1ui-aliasdef.c>
