/*
 * Copyright © 2024 Anthoine Bourgeois <anthoine.bourgeois@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "wrap.h"

void pvrsrv_bridge_event_object_open_pre(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	PVRSRV_BRIDGE_IN_EVENT_OBJECT_OPEN *in = param->pvParamIn;

	print_eventobject(&in->sEventObject);
}

void pvrsrv_bridge_event_object_open_post(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	PVRSRV_BRIDGE_OUT_EVENT_OBJECT_OPEN *out = param->pvParamOut;

	print_error(out->eError);
	printf("\t\tos event:\t%p\n", out->hOSEvent);
}

void pvrsrv_bridge_event_object_close_pre(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	PVRSRV_BRIDGE_IN_EVENT_OBJECT_CLOSE *in = param->pvParamIn;

	print_eventobject(&in->sEventObject);
	printf("\t\tos event km:\t%p\n", in->hOSEventKM);
}

void pvrsrv_bridge_event_object_close_post(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	PVRSRV_BRIDGE_RETURN *out = param->pvParamOut;

	print_error(out->eError);
}

void pvrsrv_bridge_event_object_wait_pre(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	PVRSRV_BRIDGE_IN_EVENT_OBJECT_WAIT *in = param->pvParamIn;

	printf("\t\tos event:\t%p\n", in->hOSEventKM);
}

void pvrsrv_bridge_event_object_wait_post(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	PVRSRV_BRIDGE_RETURN *out = param->pvParamOut;

	print_error(out->eError);
}
