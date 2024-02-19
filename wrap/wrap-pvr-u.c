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

void pvrsrv_bridge_connect_services_pre(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	PVRSRV_BRIDGE_IN_CONNECT_SERVICES *in = param->pvParamIn;

	printf("\t\tflags:\t\t%08x\n", in->ui32Flags);
}

void pvrsrv_bridge_connect_services_post(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	PVRSRV_BRIDGE_OUT_CONNECT_SERVICES *out = param->pvParamOut;

	print_error(out->eError);
	printf("\t\tkernel handler:\t%p\n", out->hKernelServices);
}

void pvrsrv_bridge_disconnect_services_pre(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	printf("\t\tNo param\n");
}

void pvrsrv_bridge_disconnect_services_post(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	PVRSRV_BRIDGE_RETURN *out = param->pvParamOut;

	print_error(out->eError);
}
