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

void pvrsrv_bridge_mhandle_to_mmap_data_pre(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	PVRSRV_BRIDGE_IN_MHANDLE_TO_MMAP_DATA *in = param->pvParamIn;

	printf("\t\tmhandle:\t%p\n", in->hMHandle);
}

void pvrsrv_bridge_mhandle_to_mmap_data_post(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	PVRSRV_BRIDGE_OUT_MHANDLE_TO_MMAP_DATA *out = param->pvParamOut;

	print_error(out->eError);

	if (out->eError == PVRSRV_OK) {
		printf("\t\tmmap offset:\t%08x\n", out->ui32MMapOffset);
		printf("\t\tbyte offset:\t%08x\n", out->ui32ByteOffset);
		printf("\t\treal byte size:\t%08x\n", out->ui32RealByteSize);
		printf("\t\tuser vaddr:\t%08x\n", out->ui32UserVAddr);
	}
}

void pvrsrv_bridge_release_mmap_data_pre(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	PVRSRV_BRIDGE_IN_RELEASE_MMAP_DATA *in = param->pvParamIn;

	printf("\t\tmhandle:\t%p\n", in->hMHandle);
}

void pvrsrv_bridge_release_mmap_data_post(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	PVRSRV_BRIDGE_OUT_RELEASE_MMAP_DATA *out = param->pvParamOut;

	print_error(out->eError);

	if (out->eError == PVRSRV_OK) {
		printf("\t\tmunmap:\t\t%s\n", out->bMUnmap ? "true" : "false");
		printf("\t\tuser vaddr:\t%08x\n", out->ui32UserVAddr);
		printf("\t\treal byte size:\t%08x\n", out->ui32RealByteSize);
	}
}

