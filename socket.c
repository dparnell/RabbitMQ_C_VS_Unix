/*
 * ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0
 *
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See
 * the License for the specific language governing rights and
 * limitations under the License.
 *
 * The Original Code is librabbitmq.
 *
 * The Initial Developers of the Original Code are LShift Ltd, Cohesive
 * Financial Technologies LLC, and Rabbit Technologies Ltd.  Portions
 * created before 22-Nov-2008 00:00:00 GMT by LShift Ltd, Cohesive
 * Financial Technologies LLC, or Rabbit Technologies Ltd are Copyright
 * (C) 2007-2008 LShift Ltd, Cohesive Financial Technologies LLC, and
 * Rabbit Technologies Ltd.
 *
 * Portions created by LShift Ltd are Copyright (C) 2007-2010 LShift
 * Ltd. Portions created by Cohesive Financial Technologies LLC are
 * Copyright (C) 2007-2010 Cohesive Financial Technologies
 * LLC. Portions created by Rabbit Technologies Ltd are Copyright (C)
 * 2007-2010 Rabbit Technologies Ltd.
 *
 * Portions created by Tony Garnock-Jones are Copyright (C) 2009-2010
 * LShift Ltd and Tony Garnock-Jones.
 *
 * All Rights Reserved.
 *
 * Contributor(s): ______________________________________.
 *
 * Alternatively, the contents of this file may be used under the terms
 * of the GNU General Public License Version 2 or later (the "GPL"), in
 * which case the provisions of the GPL are applicable instead of those
 * above. If you wish to allow use of your version of this file only
 * under the terms of the GPL, and not to allow others to use your
 * version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the
 * notice and other provisions required by the GPL. If you do not
 * delete the provisions above, a recipient may use your version of
 * this file under the terms of any one of the MPL or the GPL.
 *
 * ***** END LICENSE BLOCK *****
 */

#include <stdint.h>
#include <string.h>

#if defined( WIN32 )

#include <winsock2.h>
#include <windows.h>

#else

#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>

#endif

#include "amqp.h"
#include "amqp_private.h"
#include "socket.h"

#if defined( WIN32 )

/* =======================================================================
    WINDOWS SECTION
   ======================================================================== */

static int called_wsastartup = 0;

#define inline

inline int socket_read(int sock, void *buf, size_t count)
{
	return recv(sock, (char *) buf, count, 0);
}

inline int socket_write(int sock, void *buf, size_t count)
{
	return send(sock, (char *) buf, count, 0);
}

inline int socket_writev(int sock, struct iovec *iov, int nvecs)
{
	DWORD ret;
	if (WSASend(sock, (LPWSABUF)iov, nvecs, &ret, 0, NULL, NULL) == 0)
		return ret;
	else
		return -1;
}

inline int encoded_socket_errno()
{
	return WSAGetLastError() | ERROR_CATEGORY_OS;
}

int socket_init(void)
{
	if (!called_wsastartup) {
		WSADATA data;
		int res = WSAStartup(0x0202, &data);
		if (res)
			return -res;
		
		called_wsastartup = 1;
	}

	return 0;
}

const char *amqp_os_error_string(int err)
{
	char *msg, *copy;

	if (!FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM
			       | FORMAT_MESSAGE_ALLOCATE_BUFFER,
			   NULL, err, 
			   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			   (LPSTR)&msg, 0, NULL))
		return _strdup("(error retrieving Windows error message)");

	copy = _strdup(msg);
	LocalFree(msg);
	return copy;
}

#else

/* =======================================================================
    UNIX SECTION
   ======================================================================== */

int socket_socket(int domain, int type, int proto)
{
	int flags;

	int s = socket(domain, type, proto);
	if (s < 0)
		return s;

	/* Always enable CLOEXEC on the socket */
	flags = fcntl(s, F_GETFD);
	if (flags == -1
	    || fcntl(s, F_SETFD, (long)(flags | FD_CLOEXEC)) == -1) {
		int e = errno;
		close(s);
		errno = e;
		return -1;
	}

	return s;
}	

const char *amqp_os_error_string(int err)
{
	return strdup(strerror(err));
}

#endif