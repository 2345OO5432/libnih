/* nih-dbus-tool
 *
 * tests/method_factory.c - generate tests/method_code.c
 *
 * Copyright © 2009 Scott James Remnant <scott@netsplit.com>.
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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif /* HAVE_CONFIG_H */


#include <stdio.h>

#include <nih/macros.h>
#include <nih/alloc.h>
#include <nih/string.h>

#include "method.h"
#include "argument.h"


int
main (int   argc,
      char *argv[])
{
	NihList           prototypes;
	NihList           externs;
	nih_local Method *method = NULL;
	Argument *        arg;
	nih_local char *  code = NULL;

	nih_list_init (&prototypes);
	nih_list_init (&externs);

	printf ("#include <dbus/dbus.h>\n"
		"\n"
		"#include <nih/macros.h>\n"
		"#include <nih/alloc.h>\n"
		"#include <nih/string.h>\n"
		"#include <nih/logging.h>\n"
		"#include <nih/error.h>\n"
		"#include <nih/errors.h>\n"
		"\n"
		"#include <nih-dbus/dbus_error.h>\n"
		"#include <nih-dbus/dbus_message.h>\n"
		"#include <nih-dbus/dbus_object.h>\n"
		"\n"
		"#include \"method_code.h\"\n"
		"\n"
		"\n");

	printf ("extern int my_method_handler (void *data, "
		"NihDBusMessage *message, const char *str, "
		"int32_t flags, char ***output);\n"
		"\n");

	method = method_new (NULL, "MyMethod");
	method->symbol = nih_strdup (method, "my_method");

	arg = argument_new (method, "Str", "s", NIH_DBUS_ARG_IN);
	arg->symbol = nih_strdup (arg, "str");
	nih_list_add (&method->arguments, &arg->entry);

	arg = argument_new (method, "Flags", "i", NIH_DBUS_ARG_IN);
	arg->symbol = nih_strdup (arg, "flags");
	nih_list_add (&method->arguments, &arg->entry);

	arg = argument_new (method, "Output", "as", NIH_DBUS_ARG_OUT);
	arg->symbol = nih_strdup (arg, "output");
	nih_list_add (&method->arguments, &arg->entry);

	code = method_object_function (NULL, method,
				       "MyMethod_handle",
				       "my_method_handler",
				       &prototypes, &externs);

	printf ("%s", code);
	printf ("\n"
		"\n");

	method->async = TRUE;

	printf ("extern int my_async_method_handler (void *data, "
		"NihDBusMessage *message, const char *str, "
		"int32_t flags);\n"
		"\n");

	code = method_object_function (NULL, method,
				       "MyAsyncMethod_handle",
				       "my_async_method_handler",
				       &prototypes, &externs);

	printf ("%s", code);
	printf ("\n");

	code = method_reply_function (NULL, method,
				      "my_async_method_reply",
				      &prototypes, &externs);

	printf ("%s", code);

	return 0;
}
