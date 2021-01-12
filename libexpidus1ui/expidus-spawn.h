/*
 * Copyright (c) 2007 The Expidus Development Team
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

#ifndef __EXPIDUS_SPAWN_H__
#define __EXPIDUS_SPAWN_H__

#include <gdk/gdk.h>

G_BEGIN_DECLS

gboolean expidus_spawn_on_screen_with_child_watch (GdkScreen    *screen,
                                                const gchar  *working_directory,
                                                gchar       **argv,
                                                gchar       **envp,
                                                GSpawnFlags   flags,
                                                gboolean      startup_notify,
                                                guint32       startup_timestamp,
                                                const gchar  *startup_icon_name,
                                                GClosure     *child_watch_closure,
                                                GError      **error);

gboolean expidus_spawn_on_screen                  (GdkScreen    *screen,
                                                const gchar  *working_directory,
                                                gchar       **argv,
                                                gchar       **envp,
                                                GSpawnFlags   flags,
                                                gboolean      startup_notify,
                                                guint32       startup_timestamp,
                                                const gchar  *startup_icon_name,
                                                GError      **error);

gboolean expidus_spawn                            (GdkScreen    *screen,
                                                const gchar  *working_directory,
                                                gchar       **argv,
                                                gchar       **envp,
                                                GSpawnFlags   flags,
                                                gboolean      startup_notify,
                                                guint32       startup_timestamp,
                                                const gchar  *startup_icon_name,
                                                gboolean      child_process,
                                                GError      **error);

gboolean expidus_spawn_command_line_on_screen     (GdkScreen    *screen,
                                                const gchar  *command_line,
                                                gboolean      in_terminal,
                                                gboolean      startup_notify,
                                                GError      **error);

gboolean expidus_spawn_command_line               (GdkScreen    *screen,
                                                const gchar  *command_line,
                                                gboolean      in_terminal,
                                                gboolean      startup_notify,
                                                gboolean      child_process,
                                                GError      **error);

G_END_DECLS

#endif /* !__EXPIDUS_SPAWN_H__ */
