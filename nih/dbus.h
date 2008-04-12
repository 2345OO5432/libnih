/* libnih
 *
 * Copyright © 2008 Scott James Remnant <scott@netsplit.com>.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#ifndef NIH_DBUS_H
#define NIH_DBUS_H

#include <nih/macros.h>

#include <dbus/dbus.h>


/**
 * NihDBusError:
 * @error: ordinary NihError,
 * @name: D-Bus name.
 *
 * This structure builds on NihError to include an additional @name field
 * required for transport across D-Bus.
 *
 * If you receive a NIH_DBUS_ERROR, the returned NihError structure is
 * actually this structure and can be cast to get the additional fields.
 **/
typedef struct nih_dbus_error {
	NihError  error;
	char     *name;
} NihDBusError;


NIH_BEGIN_EXTERN

void            nih_dbus_error_raise (const char *name, const char *message);

NIH_END_EXTERN

#endif /* NIH_DBUS_H */
