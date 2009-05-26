/* libnih
 *
 * test_dbus_message.c - test suite for nih-dbus/dbus_message.c
 *
 * Copyright © 2009 Scott James Remnant <scott@netsplit.com>.
 * Copyright © 2009 Canonical Ltd.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <nih/test.h>
#include <nih-dbus/test_dbus.h>

#include <dbus/dbus.h>

#include <sys/types.h>
#include <sys/wait.h>

#include <signal.h>
#include <stdlib.h>

#include <nih/macros.h>
#include <nih/alloc.h>
#include <nih/timer.h>
#include <nih/signal.h>
#include <nih/main.h>

#include <nih-dbus/dbus_connection.h>
#include <nih-dbus/dbus_object.h>

#include <nih-dbus/dbus_message.h>


void
test_message_new (void)
{
	NihDBusMessage *msg;
	pid_t           dbus_pid;
	DBusConnection *conn;
	DBusMessage *   message;

	/* Check that we can create a new DBus message structure, and that
	 * it references the connection and message.
	 */
	TEST_FUNCTION ("nih_dbus_message_new");
	TEST_DBUS (dbus_pid);
	TEST_DBUS_OPEN (conn);

	message = dbus_message_new (DBUS_MESSAGE_TYPE_METHOD_CALL);

	TEST_ALLOC_FAIL {
		msg = nih_dbus_message_new (NULL, conn, message);

		if (test_alloc_failed) {
			TEST_EQ_P (msg, NULL);

			continue;
		}

		TEST_ALLOC_SIZE (msg, sizeof (NihDBusMessage));
		TEST_EQ_P (msg->connection, conn);
		TEST_EQ_P (msg->message, message);

		nih_free (msg);
	}

	dbus_message_unref (message);

	TEST_DBUS_CLOSE (conn);
	TEST_DBUS_END (dbus_pid);

	dbus_shutdown ();
}


void
test_message_error (void)
{
	pid_t           dbus_pid;
	DBusConnection *server_conn;
	DBusConnection *client_conn;
	DBusMessage *   method_call;
	DBusMessage *   reply;
	dbus_uint32_t   serial;
	NihDBusMessage *message;
	int             ret;
	DBusError       error;

	/* Check that an error returned outside the handler with the
	 * nih_dbus_message_error() function is returned to the sender
	 * with the right details.
	 */
	TEST_FUNCTION ("nih_dbus_message_error");
	TEST_DBUS (dbus_pid);
	TEST_DBUS_OPEN (server_conn);
	TEST_DBUS_OPEN (client_conn);

	TEST_ALLOC_FAIL {
		method_call = dbus_message_new_method_call (
			dbus_bus_get_unique_name (server_conn),
			"/com/netsplit/Nih",
			"com.netsplit.Nih.Glue",
			"ReturnError");

		dbus_connection_send (client_conn, method_call, &serial);
		dbus_connection_flush (client_conn);
		dbus_message_unref (method_call);

		TEST_DBUS_MESSAGE (server_conn, method_call);
		assert (dbus_message_get_serial (method_call) == serial);

		TEST_ALLOC_SAFE {
			message = nih_new (NULL, NihDBusMessage);
			message->connection = client_conn;
			message->message = method_call;
		}

		ret = nih_dbus_message_error (message,
					      "com.netsplit.Nih.Test.MyError",
					      "this is a %s %d", "test", 1234);

		if (test_alloc_failed) {
			TEST_LT (ret, 0);

			nih_free (message);
			dbus_message_unref (method_call);
			continue;
		}

		TEST_EQ (ret, 0);

		nih_free (message);
		dbus_message_unref (method_call);

		TEST_DBUS_MESSAGE (client_conn, reply);
		TEST_EQ (dbus_message_get_type (reply),
			 DBUS_MESSAGE_TYPE_ERROR);
		TEST_EQ (dbus_message_get_reply_serial (reply), serial);

		dbus_error_init (&error);
		dbus_set_error_from_message (&error, reply);

		TEST_EQ_STR (error.name,
			     "com.netsplit.Nih.Test.MyError");
		TEST_EQ_STR (error.message,
			     "this is a test 1234");

		dbus_error_free (&error);
		dbus_message_unref (reply);
	}


	TEST_DBUS_CLOSE (client_conn);
	TEST_DBUS_CLOSE (server_conn);
	TEST_DBUS_END (dbus_pid);

	dbus_shutdown ();
}


int
main (int   argc,
      char *argv[])
{
	test_message_new ();
	test_message_error ();

	return 0;
}
