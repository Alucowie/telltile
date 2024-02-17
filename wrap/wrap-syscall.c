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

#include <sys/ioctl.h>
#include <ctype.h>

#include "wrap.h"

struct device_info {
	const char *name;
	struct {
		const char *name;
	} ioctl_info[_IOC_NR(0xffffffff)];
};

#define IOCTL_INFO(n) \
		[_IOC_NR(n)] = { .name = #n }

static struct device_info pvrsrvkm_info = {
		.name = "pvrsrvkm",
		.ioctl_info = {
				IOCTL_INFO(PVRSRV_BRIDGE_CONNECT_SERVICES),
				IOCTL_INFO(PVRSRV_BRIDGE_INITSRV_CONNECT),
				IOCTL_INFO(PVRSRV_BRIDGE_ENUM_DEVICES),
				IOCTL_INFO(PVRSRV_BRIDGE_ACQUIRE_DEVICEINFO),
				IOCTL_INFO(PVRSRV_BRIDGE_SGXINFO_FOR_SRVINIT),
				IOCTL_INFO(PVRSRV_BRIDGE_ALLOC_DEVICEMEM),
				IOCTL_INFO(PVRSRV_BRIDGE_MHANDLE_TO_MMAP_DATA),
				IOCTL_INFO(PVRSRV_BRIDGE_RELEASE_MMAP_DATA),
				IOCTL_INFO(PVRSRV_BRIDGE_SGX_DEVINITPART2),
				IOCTL_INFO(PVRSRV_BRIDGE_INITSRV_DISCONNECT),
				IOCTL_INFO(PVRSRV_BRIDGE_DISCONNECT_SERVICES),

				IOCTL_INFO(PVRSRV_BRIDGE_SGX_GETMISCINFO),
				IOCTL_INFO(PVRSRV_BRIDGE_CREATE_DEVMEMCONTEXT),
				IOCTL_INFO(PVRSRV_BRIDGE_SGX_GETCLIENTINFO),
				IOCTL_INFO(PVRSRV_BRIDGE_GET_MISC_INFO),
				IOCTL_INFO(PVRSRV_BRIDGE_EVENT_OBJECT_OPEN),
				IOCTL_INFO(PVRSRV_BRIDGE_GET_DEVMEM_HEAPINFO),
				IOCTL_INFO(PVRSRV_BRIDGE_GETMMU_PD_DEVPADDR),
				IOCTL_INFO(PVRSRV_BRIDGE_SGX_GETINTERNALDEVINFO),
				IOCTL_INFO(PVRSRV_BRIDGE_SGX_REGISTER_HW_RENDER_CONTEXT),
				IOCTL_INFO(PVRSRV_BRIDGE_SGX_REGISTER_HW_TRANSFER_CONTEXT),
				IOCTL_INFO(PVRSRV_BRIDGE_ENUM_CLASS),
				IOCTL_INFO(PVRSRV_BRIDGE_OPEN_DISPCLASS_DEVICE),
				IOCTL_INFO(PVRSRV_BRIDGE_GET_DISPCLASS_INFO),
				IOCTL_INFO(PVRSRV_BRIDGE_CLOSE_DISPCLASS_DEVICE),
				IOCTL_INFO(PVRSRV_BRIDGE_ENUM_DISPCLASS_FORMATS),
				IOCTL_INFO(PVRSRV_BRIDGE_ENUM_DISPCLASS_DIMS),
				IOCTL_INFO(PVRSRV_BRIDGE_GET_DISPCLASS_SYSBUFFER),
				IOCTL_INFO(PVRSRV_BRIDGE_MAP_DEVICECLASS_MEMORY),
				IOCTL_INFO(PVRSRV_BRIDGE_FREE_DEVICEMEM),
				IOCTL_INFO(PVRSRV_BRIDGE_SYNC_OPS_TAKE_TOKEN),
				IOCTL_INFO(PVRSRV_BRIDGE_SYNC_OPS_FLUSH_TO_TOKEN),
				IOCTL_INFO(PVRSRV_BRIDGE_SGX_FLUSH_HW_RENDER_TARGET),
				IOCTL_INFO(PVRSRV_BRIDGE_EVENT_OBJECT_CLOSE),
				IOCTL_INFO(PVRSRV_BRIDGE_DESTROY_DEVMEMCONTEXT),
				IOCTL_INFO(PVRSRV_BRIDGE_SGX_UNREGISTER_HW_TRANSFER_CONTEXT),
				IOCTL_INFO(PVRSRV_BRIDGE_SGX_UNREGISTER_HW_RENDER_CONTEXT),
				IOCTL_INFO(PVRSRV_BRIDGE_SGX_RELEASECLIENTINFO),

				IOCTL_INFO(PVRSRV_BRIDGE_SGX_SUBMITTRANSFER),
				IOCTL_INFO(PVRSRV_BRIDGE_SGX_DOKICK),
				IOCTL_INFO(PVRSRV_BRIDGE_EVENT_OBJECT_WAIT),
				IOCTL_INFO(PVRSRV_BRIDGE_UNMAP_DEVICECLASS_MEMORY),
		},
};

static struct {
	int is_pvrsrvkm;
} file_table[1024];

static struct device_info * get_pvrsrvkm_info(int fd)
{
	if (fd >= ARRAY_SIZE(file_table))
		return NULL;
	if (file_table[fd].is_pvrsrvkm)
		return &pvrsrvkm_info;
	return NULL;
}

static void
hexdump(const void *data, int size)
{
	unsigned char *buf = (void *) data;
	char alpha[17];
	int i;

	for (i = 0; i < size; i++) {
		if (!(i % 16))
			printf("\t\t\t%08X", (unsigned int) i);
		if (!(i % 4))
			printf(" ");

		if (((void *) (buf + i)) < ((void *) data)) {
			printf("   ");
			alpha[i % 16] = '.';
		} else {
			printf(" %02x", buf[i]);

			if (isprint(buf[i]) && (buf[i] < 0xA0))
				alpha[i % 16] = buf[i];
			else
				alpha[i % 16] = '.';
		}

		if ((i % 16) == 15) {
			alpha[16] = 0;
			printf("\t|%s|\n", alpha);
		}
	}

	if (i % 16) {
		for (i %= 16; i < 16; i++) {
			printf("   ");
			alpha[i] = '.';

			if (i == 15) {
				alpha[16] = 0;
				printf("\t|%s|\n", alpha);
			}
		}
	}
}

static void dump_ioctl(struct device_info *info, int dir, int fd,
		unsigned long int request, void *ptr, int ret)
{
	int nr = _IOC_NR(request);
	int sz = _IOC_SIZE(request);
	char c;
	const char *name;

	if (dir == _IOC_READ)
		c = '<';
	else
		c = '>';

	if (info->ioctl_info[nr].name)
		name = info->ioctl_info[nr].name;
	else
		name = "<unknown>";

	printf("%c [%4d] %8s: %s (%08lx)", c, fd, info->name, name, request);
	if (dir == _IOC_READ)
		printf(" => %d", ret);
	printf("\n");

	if (dir & _IOC_DIR(request))
		hexdump(ptr, sz);
}

static void pvrsrv_dump_in(void *ptr)
{
	PVRSRV_BRIDGE_PACKAGE *pkg = ptr;

	hexdump(pkg->pvParamIn, pkg->ui32InBufferSize);
}

static void pvrsrv_dump_out(void *ptr)
{
	PVRSRV_BRIDGE_PACKAGE *pkg = ptr;

	hexdump(pkg->pvParamOut, pkg->ui32OutBufferSize);
}

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

int openat(int dirfd, const char *path, int flags, ...)
{
    int ret = -1;
    mode_t mode = 0;
    PROLOG(openat);

    printf("wrap openat\n");
    if (dirfd == AT_FDCWD || path[0] == '/') {
	va_list args;
	va_start(args, flags);
	mode = (mode_t) va_arg(args, int);
	va_end(args);

	ret = open(path, flags, mode);
    } else {
	printf("#### openat not supported.\n");
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

static void pvrsrvkm_ioctl_pre(int fd, unsigned long int request, void *ptr)
{
	dump_ioctl(get_pvrsrvkm_info(fd), _IOC_WRITE, fd, request, ptr, 0);
	pvrsrv_dump_in(ptr);
	switch(_IOC_NR(request)) {
	case _IOC_NR(PVRSRV_BRIDGE_CONNECT_SERVICES):
		printf("ioctl connect_services pre\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_INITSRV_CONNECT):
		printf("ioctl initsrv_connect pre\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_ENUM_DEVICES):
		printf("ioctl enum_devices pre\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_ACQUIRE_DEVICEINFO):
		printf("ioctl acquire_deviceinfo pre\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_SGXINFO_FOR_SRVINIT):
		printf("ioctl sgxinfo_for_srvinit pre\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_ALLOC_DEVICEMEM):
		printf("ioctl alloc_devicemem pre\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_MHANDLE_TO_MMAP_DATA):
		printf("ioctl mhandle_to_mmap_data pre\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_RELEASE_MMAP_DATA):
		printf("ioctl release_mmap_data pre\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_SGX_DEVINITPART2):
		printf("ioctl sgx_devinitpart2 pre\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_INITSRV_DISCONNECT):
		printf("ioctl initsrv_disconnect pre\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_DISCONNECT_SERVICES):
		printf("ioctl disconnect_services pre\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_SGX_GETMISCINFO):
		printf("ioctl sgx_getmiscinfo pre\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_CREATE_DEVMEMCONTEXT):
		printf("ioctl create_devmemcontext pre\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_SGX_GETCLIENTINFO):
		printf("ioctl sgx_getclientinfo pre\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_GET_MISC_INFO):
		printf("ioctl get_misc_info pre\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_EVENT_OBJECT_OPEN):
		printf("ioctl event_object_open pre\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_GET_DEVMEM_HEAPINFO):
		printf("ioctl get_devmem_heapinfo pre\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_GETMMU_PD_DEVPADDR):
		printf("ioctl getmmu_pd_devpaddr pre\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_SGX_GETINTERNALDEVINFO):
		printf("ioctl sgx_getinternaldevinfo pre\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_SGX_REGISTER_HW_RENDER_CONTEXT):
		printf("ioctl sgx_register_hw_render_context pre\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_SGX_REGISTER_HW_TRANSFER_CONTEXT):
		printf("ioctl sgx_register_hw_transfer_context pre\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_ENUM_CLASS):
		printf("ioctl enum_class pre\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_OPEN_DISPCLASS_DEVICE):
		printf("ioctl open_dispclass_device pre\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_GET_DISPCLASS_INFO):
		printf("ioctl get_dispclass_info pre\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_CLOSE_DISPCLASS_DEVICE):
		printf("ioctl close_dispclass_device pre\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_ENUM_DISPCLASS_FORMATS):
		printf("ioctl enum_dispclass_formats pre\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_ENUM_DISPCLASS_DIMS):
		printf("ioctl enum_dispclass_dims pre\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_GET_DISPCLASS_SYSBUFFER):
		printf("ioctl get_dispclass_sysbuffer pre\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_MAP_DEVICECLASS_MEMORY):
		printf("ioctl map_deviceclass_memory pre\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_FREE_DEVICEMEM):
		printf("ioctl free_devicemem pre\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_SYNC_OPS_TAKE_TOKEN):
		printf("ioctl sync_ops_take_token pre\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_SYNC_OPS_FLUSH_TO_TOKEN):
		printf("ioctl sync_ops_flush_to_token pre\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_SGX_FLUSH_HW_RENDER_TARGET):
		printf("ioctl sgx_flush_hw_render_target pre\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_EVENT_OBJECT_CLOSE):
		printf("ioctl event_object_close pre\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_DESTROY_DEVMEMCONTEXT):
		printf("ioctl destroy_devmemcontext pre\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_SGX_UNREGISTER_HW_TRANSFER_CONTEXT):
		printf("ioctl sgx_unregister_hw_transfer_context pre\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_SGX_UNREGISTER_HW_RENDER_CONTEXT):
		printf("ioctl sgx_unregister_hw_render_context pre\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_SGX_RELEASECLIENTINFO):
		printf("ioctl sgx_releaseclientinfo pre\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_SGX_SUBMITTRANSFER):
		printf("ioctl sgx_submittransfer pre\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_SGX_DOKICK):
		printf("ioctl sgx_dokick pre\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_EVENT_OBJECT_WAIT):
		printf("ioctl event_object_wait pre\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_UNMAP_DEVICECLASS_MEMORY):
		printf("ioctl unmap_deviceclass_memory pre\n");
		break;
	default:
		printf("> [%4d]         : <unknown param> (%08lx)\n", fd, request);
		break;
	}
}

static void pvrsrvkm_ioctl_post(int fd, unsigned long int request, void *ptr, int ret)
{
	dump_ioctl(get_pvrsrvkm_info(fd), _IOC_READ, fd, request, ptr, ret);
	pvrsrv_dump_out(ptr);
	switch(_IOC_NR(request)) {
	case _IOC_NR(PVRSRV_BRIDGE_CONNECT_SERVICES):
		printf("ioctl connect_services post\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_INITSRV_CONNECT):
		printf("ioctl initsrv_connect post\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_ENUM_DEVICES):
		printf("ioctl enum_devices post\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_ACQUIRE_DEVICEINFO):
		printf("ioctl acquire_deviceinfo post\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_SGXINFO_FOR_SRVINIT):
		printf("ioctl sgxinfo_for_srvinit post\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_ALLOC_DEVICEMEM):
		printf("ioctl alloc_devicemem post\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_MHANDLE_TO_MMAP_DATA):
		printf("ioctl mhandle_to_mmap_data post\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_RELEASE_MMAP_DATA):
		printf("ioctl release_mmap_data post\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_SGX_DEVINITPART2):
		printf("ioctl sgx_devinitpart2 post\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_INITSRV_DISCONNECT):
		printf("ioctl initsrv_disconnect post\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_DISCONNECT_SERVICES):
		printf("ioctl disconnect_services post\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_SGX_GETMISCINFO):
		printf("ioctl sgx_getmiscinfo post\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_CREATE_DEVMEMCONTEXT):
		printf("ioctl create_devmemcontext post\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_SGX_GETCLIENTINFO):
		printf("ioctl sgx_getclientinfo post\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_GET_MISC_INFO):
		printf("ioctl get_misc_info post\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_EVENT_OBJECT_OPEN):
		printf("ioctl event_object_open post\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_GET_DEVMEM_HEAPINFO):
		printf("ioctl get_devmem_heapinfo post\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_GETMMU_PD_DEVPADDR):
		printf("ioctl getmmu_pd_devpaddr post\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_SGX_GETINTERNALDEVINFO):
		printf("ioctl sgx_getinternaldevinfo post\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_SGX_REGISTER_HW_RENDER_CONTEXT):
		printf("ioctl sgx_register_hw_render_context post\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_SGX_REGISTER_HW_TRANSFER_CONTEXT):
		printf("ioctl sgx_register_hw_transfer_context post\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_ENUM_CLASS):
		printf("ioctl enum_class post\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_OPEN_DISPCLASS_DEVICE):
		printf("ioctl open_dispclass_device post\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_GET_DISPCLASS_INFO):
		printf("ioctl get_dispclass_info post\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_CLOSE_DISPCLASS_DEVICE):
		printf("ioctl close_dispclass_device post\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_ENUM_DISPCLASS_FORMATS):
		printf("ioctl enum_dispclass_formats post\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_ENUM_DISPCLASS_DIMS):
		printf("ioctl enum_dispclass_dims post\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_GET_DISPCLASS_SYSBUFFER):
		printf("ioctl get_dispclass_sysbuffer post\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_MAP_DEVICECLASS_MEMORY):
		printf("ioctl map_deviceclass_memory post\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_FREE_DEVICEMEM):
		printf("ioctl free_devicemem post\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_SYNC_OPS_TAKE_TOKEN):
		printf("ioctl sync_ops_take_token post\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_SYNC_OPS_FLUSH_TO_TOKEN):
		printf("ioctl sync_ops_flush_to_token post\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_SGX_FLUSH_HW_RENDER_TARGET):
		printf("ioctl sgx_flush_hw_render_target post\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_EVENT_OBJECT_CLOSE):
		printf("ioctl event_object_close post\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_DESTROY_DEVMEMCONTEXT):
		printf("ioctl destroy_devmemcontext post\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_SGX_UNREGISTER_HW_TRANSFER_CONTEXT):
		printf("ioctl sgx_unregister_hw_transfer_context post\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_SGX_UNREGISTER_HW_RENDER_CONTEXT):
		printf("ioctl sgx_unregister_hw_render_context post\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_SGX_RELEASECLIENTINFO):
		printf("ioctl sgx_releaseclientinfo post\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_SGX_SUBMITTRANSFER):
		printf("ioctl sgx_submittransfer post\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_SGX_DOKICK):
		printf("ioctl sgx_dokick post\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_EVENT_OBJECT_WAIT):
		printf("ioctl event_object_wait post\n");
		break;
	case _IOC_NR(PVRSRV_BRIDGE_UNMAP_DEVICECLASS_MEMORY):
		printf("ioctl unmap_deviceclass_memory post\n");
		break;
	default:
		printf("< [%4d]         : <unknown result> (%08lx)\n", fd, request);
		break;
	}
}

int ioctl(int fd, unsigned long int request, ...)
{
	int ioc_size = _IOC_SIZE(request);
	int ret;
	PROLOG(ioctl);
	void *ptr;

	if (ioc_size) {
		va_list args;

		va_start(args, request);
		ptr = va_arg(args, void *);
		va_end(args);
	} else {
		ptr = NULL;
	}

	if (get_pvrsrvkm_info(fd))
		pvrsrvkm_ioctl_pre(fd, request, ptr);
	else
		printf("> [%4d]         : <unknown> (%08lx)\n", fd, request);

	ret = orig_ioctl(fd, request, ptr);

	if (get_pvrsrvkm_info(fd))
		pvrsrvkm_ioctl_post(fd, request, ptr, ret);
	else
		printf("< [%4d]         : <unknown> (%08lx) (%d)\n", fd, request, ret);

	return ret;
}
