/*
 * Copyright © 2012 Rob Clark <robclark@freedesktop.org>
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

void * __rd_dlsym_helper(const char *name)
{
	static void *libc_dl;
	void *func;

	if (!libc_dl)
		libc_dl = dlopen("/lib/arm-linux-gnueabihf/libc-2.15.so", RTLD_LAZY);
	if (!libc_dl)
		libc_dl = dlopen("/lib/libc-2.16.so", RTLD_LAZY);
	if (!libc_dl)
		libc_dl = dlopen("libc.so.6", RTLD_LAZY);

	if (!libc_dl) {
		printf("Failed to dlopen libc: %s\n", dlerror());
		exit(-1);
	}

	func = dlsym(libc_dl, name);

	if (!func) {
		printf("Failed to find %s: %s\n", name, dlerror());
		exit(-1);
	}

	return func;
}
