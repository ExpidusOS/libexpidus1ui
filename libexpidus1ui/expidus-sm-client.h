/*
 * Copyright (c) 2009 Brian Tarricone <brian@terricone.org>
 * Copyright (C) 1999 Olivier Fourdan <fourdan@xfce.org>
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

#ifndef __EXPIDUS_SM_CLIENT_H__
#define __EXPIDUS_SM_CLIENT_H__

#if !defined(LIBEXPIDUS1UI_INSIDE_LIBEXPIDUS1UI_H) && !defined(LIBEXPIDUS1UI_COMPILATION)
#error "Only <libexpidus1ui/libexpidus1ui.h> can be included directly, this file is not part of the public API."
#endif

#include <glib-object.h>

#define EXPIDUS_TYPE_SM_CLIENT             (expidus_sm_client_get_type())
#define EXPIDUS_SM_CLIENT(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj), EXPIDUS_TYPE_SM_CLIENT, ExpidusSMClient))
#define EXPIDUS_IS_SM_CLIENT(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj), EXPIDUS_TYPE_SM_CLIENT))
#define EXPIDUS_SM_CLIENT_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass), EXPIDUS_TYPE_SM_CLIENT, ExpidusSMClientClass))
#define EXPIDUS_IS_SM_CLIENT_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass), EXPIDUS_TYPE_SM_CLIENT))
#define EXPIDUS_SM_CLIENT_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj), EXPIDUS_TYPE_SM_CLIENT, ExpidusSMClientClass))

/**
 * EXPIDUS_SM_CLIENT_ERROR:
 *
 * Error domain for ExpidusSmCLient. Errors in this domain will be from the #ExpidusSmCLientErrorEnum enumeration.
 * See #GError for more information on error domains.
 **/
#define EXPIDUS_SM_CLIENT_ERROR expidus_sm_client_error_quark()

/**
 * ExpidusSmCLientErrorEnum:
 * @EXPIDUS_SM_CLIENT_ERROR_FAILED: Failed to connect to the session manager.
 * @EXPIDUS_SM_CLIENT_ERROR_INVALID_CLIENT: Session does not have a valid client id.
 *
 * Error codes returned by ExpidusSmCLient functions.
 **/
typedef enum {
  EXPIDUS_SM_CLIENT_ERROR_FAILED,
  EXPIDUS_SM_CLIENT_ERROR_INVALID_CLIENT
} ExpidusSmCLientErrorEnum;

G_BEGIN_DECLS

typedef enum
{
    EXPIDUS_SM_CLIENT_RESTART_NORMAL = 0,
    EXPIDUS_SM_CLIENT_RESTART_IMMEDIATELY,
} ExpidusSMClientRestartStyle;

typedef enum /*< skip >*/
{
    EXPIDUS_SM_CLIENT_PRIORITY_HIGHEST = 0,
    EXPIDUS_SM_CLIENT_PRIORITY_WM = 15,
    EXPIDUS_SM_CLIENT_PRIORITY_CORE = 25,
    EXPIDUS_SM_CLIENT_PRIORITY_DESKTOP = 35,
    EXPIDUS_SM_CLIENT_PRIORITY_DEFAULT = 50,
    EXPIDUS_SM_CLIENT_PRIORITY_LOWEST = 255,
} ExpidusSMClientPriority;

typedef enum
{
    EXPIDUS_SM_CLIENT_SHUTDOWN_HINT_ASK = 0,
    EXPIDUS_SM_CLIENT_SHUTDOWN_HINT_LOGOUT,
    EXPIDUS_SM_CLIENT_SHUTDOWN_HINT_HALT,
    EXPIDUS_SM_CLIENT_SHUTDOWN_HINT_REBOOT,
} ExpidusSMClientShutdownHint;

typedef struct _ExpidusSMClient  ExpidusSMClient;

GType expidus_sm_client_get_type(void) G_GNUC_CONST;

GQuark expidus_sm_client_error_quark (void);

GOptionGroup *expidus_sm_client_get_option_group(gint argc,
                                              gchar **argv);

ExpidusSMClient *expidus_sm_client_get(void);

ExpidusSMClient *expidus_sm_client_get_with_argv(gint argc,
                                           gchar **argv,
                                           ExpidusSMClientRestartStyle restart_style,
                                           guchar priority);

ExpidusSMClient *expidus_sm_client_get_full(ExpidusSMClientRestartStyle restart_style,
                                      guchar priority,
                                      const gchar *resumed_client_id,
                                      const gchar *current_directory,
                                      const gchar **restart_command,
                                      const gchar *desktop_file);

gboolean expidus_sm_client_connect(ExpidusSMClient *sm_client,
                                GError **error);
void expidus_sm_client_disconnect(ExpidusSMClient *sm_client);

void expidus_sm_client_request_shutdown(ExpidusSMClient *sm_client,
                                     ExpidusSMClientShutdownHint shutdown_hint);

gboolean expidus_sm_client_is_connected(ExpidusSMClient *sm_client);
gboolean expidus_sm_client_is_resumed(ExpidusSMClient *sm_client);

void expidus_sm_client_set_desktop_file(ExpidusSMClient *sm_client,
                                     const gchar *desktop_file);

const gchar *expidus_sm_client_get_client_id(ExpidusSMClient *sm_client);

const gchar *expidus_sm_client_get_state_file(ExpidusSMClient *sm_client);

void expidus_sm_client_set_restart_style(ExpidusSMClient *sm_client,
                                      ExpidusSMClientRestartStyle restart_style);
ExpidusSMClientRestartStyle expidus_sm_client_get_restart_style(ExpidusSMClient *sm_client);

void expidus_sm_client_set_priority(ExpidusSMClient *sm_client,
                                 guint8 priority);
guint8 expidus_sm_client_get_priority(ExpidusSMClient *sm_client);

void expidus_sm_client_set_current_directory(ExpidusSMClient *sm_client,
                                          const gchar *current_directory);
const gchar *expidus_sm_client_get_current_directory(ExpidusSMClient *sm_client);

void expidus_sm_client_set_restart_command(ExpidusSMClient *sm_client,
                                        gchar **restart_command);
const gchar * const *
expidus_sm_client_get_restart_command(ExpidusSMClient *sm_client);

G_END_DECLS

#endif  /* __EXPIDUS_SM_CLIENT_H__ */
