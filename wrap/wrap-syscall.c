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

/* bits and pieces borrowed from lima and freedreno projects...
 * Concept is the same, wrap various syscalls and log what happens.
 */

#include "wrap.h"

static struct {
	int is_pvrsrvkm;
} file_table[1024];

int open(const char* path, int flags, ...)
{
	mode_t mode = 0;
	int ret;
	PROLOG(open);

	if (flags & O_CREAT) {
		va_list args;

		va_start(args, flags);
		mode = (mode_t) va_arg(args, int);
		va_end(args);

		ret = orig_open(path, flags, mode);
	} else {
		ret = orig_open(path, flags);
	}

	if (ret != -1) {
		assert(ret < ARRAY_SIZE(file_table));
		if (!strcmp(path, "/dev/pvrsrvkm")) {
			file_table[ret].is_pvrsrvkm = 1;
			printf("found pvrsrvkm: %d\n", ret);
		} else if (strstr(path, "/dev/")) {
			printf("#### missing device, path: %s: %d\n", path, ret);
		}
	}

	return ret;
}

int close(int fd)
{
	PROLOG(close);

	if ((fd >= 0) && (fd < ARRAY_SIZE(file_table))) {
		if (file_table[fd].is_pvrsrvkm) {
			printf("closing pvrsrvkm\n");
		}
		file_table[fd].is_pvrsrvkm = 0;
	}

	return orig_close(fd);
}
