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

/**
 * SECTION:libexpidus1ui-config
 * @title: libexpidus1ui config
 * @short_description: libexpidus1ui config macros
 * @stability: Stable
 * @include: libexpidus1ui/libexpidus1ui.h
 *
 * Variables and functions to check the Libexpidus1ui version.
 **/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <libexpidus1ui/libexpidus1ui-config.h>
#include <libexpidus1ui/libexpidus1ui-alias.h>


/**
 * libexpidus1ui_major_version:
 *
 * A constat that evaluates to the major version of libexpidus1ui.
 *
 */
const guint libexpidus1ui_major_version = LIBEXPIDUS1UI_MAJOR_VERSION;

/**
 * libexpidus1ui_minor_version:
 *
 * A constat that evaluates to the minor version of libexpidus1ui.
 *
 */
const guint libexpidus1ui_minor_version = LIBEXPIDUS1UI_MINOR_VERSION;

/**
 * libexpidus1ui_micro_version:
 *
 * A constat that evaluates to the micro version of libexpidus1ui.
 *
 */
const guint libexpidus1ui_micro_version = LIBEXPIDUS1UI_MICRO_VERSION;



/**
 * libexpidus1ui_check_version:
 * @required_major: the required major version.
 * @required_minor: the required minor version.
 * @required_micro: the required micro version.
 *
 * Checks that the <systemitem class="library">libexpidus1ui</systemitem> library
 * in use is compatible with the given version. Generally you would pass in
 * the constants #LIBEXPIDUS1UI_MAJOR_VERSION, #LIBEXPIDUS1UI_MINOR_VERSION and
 * #LIBEXPIDUS1UI_MICRO_VERSION as the three arguments to this function; that produces
 * a check that the library in use is compatible with the version of
 * <systemitem class="library">libexpidus1ui</systemitem> the extension was
 * compiled against.
 *
 * <example>
 * <title>Checking the runtime version of the Libexpidus1ui library</title>
 * <programlisting>
 * const gchar *mismatch;
 * mismatch = libexpidus1ui_check_version (LIBEXPIDUS1UI_MAJOR_VERSION,
 *                                      LIBEXPIDUS1UI_MINOR_VERSION,
 *                                      LIBEXPIDUS1UI_MICRO_VERSION);
 * if (G_UNLIKELY (mismatch != NULL))
 *   g_error ("Version mismatch: %<!---->s", mismatch);
 * </programlisting>
 * </example>
 *
 * Return value: %NULL if the library is compatible with the given version,
 *               or a string describing the version mismatch. The returned
 *               string is owned by the library and must not be freed or
 *               modified by the caller.
 **/
const gchar *
libexpidus1ui_check_version (guint required_major,
                          guint required_minor,
                          guint required_micro)
{
  if (required_major > LIBEXPIDUS1UI_MAJOR_VERSION)
    return "Libexpidus1ui version too old (major mismatch)";
  if (required_major < LIBEXPIDUS1UI_MAJOR_VERSION)
    return "Libexpidus1ui version too new (major mismatch)";
  if (required_minor > LIBEXPIDUS1UI_MINOR_VERSION)
    return "Libexpidus1ui version too old (minor mismatch)";
  if (required_minor == LIBEXPIDUS1UI_MINOR_VERSION && required_micro > LIBEXPIDUS1UI_MICRO_VERSION)
    return "Libexpidus1ui version too old (micro mismatch)";
  return NULL;
}



#define __LIBEXPIDUS1UI_CONFIG_C__
#include <libexpidus1ui/libexpidus1ui-aliasdef.c>
