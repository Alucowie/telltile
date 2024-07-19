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
#include <syscall.h>
#include <sys/mman.h>

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
		pvrsrv_bridge_connect_services_pre(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_INITSRV_CONNECT):
		pvrsrv_bridge_initsrv_connect_pre(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_ENUM_DEVICES):
		pvrsrv_bridge_enum_devices_pre(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_ACQUIRE_DEVICEINFO):
		pvrsrv_bridge_acquire_deviceinfo_pre(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_SGXINFO_FOR_SRVINIT):
		pvrsrv_bridge_sgxinfo_for_srvinit_pre(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_ALLOC_DEVICEMEM):
		pvrsrv_bridge_alloc_devicemem_pre(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_MHANDLE_TO_MMAP_DATA):
		pvrsrv_bridge_mhandle_to_mmap_data_pre(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_RELEASE_MMAP_DATA):
		pvrsrv_bridge_release_mmap_data_pre(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_SGX_DEVINITPART2):
		pvrsrv_bridge_sgx_devinitpart2_pre(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_INITSRV_DISCONNECT):
		pvrsrv_bridge_initsrv_disconnect_pre(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_DISCONNECT_SERVICES):
		pvrsrv_bridge_disconnect_services_pre(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_SGX_GETMISCINFO):
		pvrsrv_bridge_sgx_getmiscinfo_pre(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_CREATE_DEVMEMCONTEXT):
		pvrsrv_bridge_create_devmemcontext_pre(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_SGX_GETCLIENTINFO):
		pvrsrv_bridge_sgx_getclientinfo_pre(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_GET_MISC_INFO):
		pvrsrv_bridge_get_misc_info_pre(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_EVENT_OBJECT_OPEN):
		pvrsrv_bridge_event_object_open_pre(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_GET_DEVMEM_HEAPINFO):
		pvrsrv_bridge_get_devmem_heapinfo_pre(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_GETMMU_PD_DEVPADDR):
		pvrsrv_bridge_getmmu_pd_devpaddr_pre(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_SGX_GETINTERNALDEVINFO):
		pvrsrv_bridge_sgx_getinternaldevinfo_pre(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_SGX_REGISTER_HW_RENDER_CONTEXT):
		pvrsrv_bridge_sgx_register_hw_render_context_pre(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_SGX_REGISTER_HW_TRANSFER_CONTEXT):
		pvrsrv_bridge_sgx_register_hw_transfer_context_pre(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_ENUM_CLASS):
		pvrsrv_bridge_enum_class_pre(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_OPEN_DISPCLASS_DEVICE):
		pvrsrv_bridge_open_dispclass_device_pre(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_GET_DISPCLASS_INFO):
		pvrsrv_bridge_get_dispclass_info_pre(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_CLOSE_DISPCLASS_DEVICE):
		pvrsrv_bridge_close_dispclass_device_pre(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_ENUM_DISPCLASS_FORMATS):
		pvrsrv_bridge_enum_dispclass_formats_pre(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_ENUM_DISPCLASS_DIMS):
		pvrsrv_bridge_enum_dispclass_dims_pre(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_GET_DISPCLASS_SYSBUFFER):
		pvrsrv_bridge_get_dispclass_sysbuffer_pre(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_MAP_DEVICECLASS_MEMORY):
		pvrsrv_bridge_map_deviceclass_memory_pre(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_FREE_DEVICEMEM):
		pvrsrv_bridge_free_devicemem_pre(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_SYNC_OPS_TAKE_TOKEN):
		pvrsrv_bridge_sync_ops_take_token_pre(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_SYNC_OPS_FLUSH_TO_TOKEN):
		pvrsrv_bridge_sync_ops_flush_to_token_pre(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_SGX_FLUSH_HW_RENDER_TARGET):
		pvrsrv_bridge_sgx_flush_hw_render_target_pre(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_EVENT_OBJECT_CLOSE):
		pvrsrv_bridge_event_object_close_pre(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_DESTROY_DEVMEMCONTEXT):
		pvrsrv_bridge_destroy_devmemcontext_pre(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_SGX_UNREGISTER_HW_TRANSFER_CONTEXT):
		pvrsrv_bridge_sgx_unregister_hw_transfer_context_pre(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_SGX_UNREGISTER_HW_RENDER_CONTEXT):
		pvrsrv_bridge_sgx_unregister_hw_render_context_pre(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_SGX_RELEASECLIENTINFO):
		pvrsrv_bridge_sgx_releaseclientinfo_pre(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_SGX_SUBMITTRANSFER):
		pvrsrv_bridge_sgx_submittransfer_pre(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_SGX_DOKICK):
		pvrsrv_bridge_sgx_dokick_pre(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_EVENT_OBJECT_WAIT):
		pvrsrv_bridge_event_object_wait_pre(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_UNMAP_DEVICECLASS_MEMORY):
		pvrsrv_bridge_unmap_deviceclass_memory_pre(fd, ptr);
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
		pvrsrv_bridge_connect_services_post(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_INITSRV_CONNECT):
		pvrsrv_bridge_initsrv_connect_post(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_ENUM_DEVICES):
		pvrsrv_bridge_enum_devices_post(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_ACQUIRE_DEVICEINFO):
		pvrsrv_bridge_acquire_deviceinfo_post(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_SGXINFO_FOR_SRVINIT):
		pvrsrv_bridge_sgxinfo_for_srvinit_post(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_ALLOC_DEVICEMEM):
		pvrsrv_bridge_alloc_devicemem_post(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_MHANDLE_TO_MMAP_DATA):
		pvrsrv_bridge_mhandle_to_mmap_data_post(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_RELEASE_MMAP_DATA):
		pvrsrv_bridge_release_mmap_data_post(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_SGX_DEVINITPART2):
		pvrsrv_bridge_sgx_devinitpart2_post(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_INITSRV_DISCONNECT):
		pvrsrv_bridge_initsrv_disconnect_post(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_DISCONNECT_SERVICES):
		pvrsrv_bridge_disconnect_services_post(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_SGX_GETMISCINFO):
		pvrsrv_bridge_sgx_getmiscinfo_post(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_CREATE_DEVMEMCONTEXT):
		pvrsrv_bridge_create_devmemcontext_post(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_SGX_GETCLIENTINFO):
		pvrsrv_bridge_sgx_getclientinfo_post(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_GET_MISC_INFO):
		pvrsrv_bridge_get_misc_info_post(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_EVENT_OBJECT_OPEN):
		pvrsrv_bridge_event_object_open_post(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_GET_DEVMEM_HEAPINFO):
		pvrsrv_bridge_get_devmem_heapinfo_post(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_GETMMU_PD_DEVPADDR):
		pvrsrv_bridge_getmmu_pd_devpaddr_post(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_SGX_GETINTERNALDEVINFO):
		pvrsrv_bridge_sgx_getinternaldevinfo_post(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_SGX_REGISTER_HW_RENDER_CONTEXT):
		pvrsrv_bridge_sgx_register_hw_render_context_post(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_SGX_REGISTER_HW_TRANSFER_CONTEXT):
		pvrsrv_bridge_sgx_register_hw_transfer_context_post(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_ENUM_CLASS):
		pvrsrv_bridge_enum_class_post(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_OPEN_DISPCLASS_DEVICE):
		pvrsrv_bridge_open_dispclass_device_post(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_GET_DISPCLASS_INFO):
		pvrsrv_bridge_get_dispclass_info_post(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_CLOSE_DISPCLASS_DEVICE):
		pvrsrv_bridge_close_dispclass_device_post(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_ENUM_DISPCLASS_FORMATS):
		pvrsrv_bridge_enum_dispclass_formats_post(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_ENUM_DISPCLASS_DIMS):
		pvrsrv_bridge_enum_dispclass_dims_post(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_GET_DISPCLASS_SYSBUFFER):
		pvrsrv_bridge_get_dispclass_sysbuffer_post(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_MAP_DEVICECLASS_MEMORY):
		pvrsrv_bridge_map_deviceclass_memory_post(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_FREE_DEVICEMEM):
		pvrsrv_bridge_free_devicemem_post(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_SYNC_OPS_TAKE_TOKEN):
		pvrsrv_bridge_sync_ops_take_token_post(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_SYNC_OPS_FLUSH_TO_TOKEN):
		pvrsrv_bridge_sync_ops_flush_to_token_post(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_SGX_FLUSH_HW_RENDER_TARGET):
		pvrsrv_bridge_sgx_flush_hw_render_target_post(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_EVENT_OBJECT_CLOSE):
		pvrsrv_bridge_event_object_close_post(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_DESTROY_DEVMEMCONTEXT):
		pvrsrv_bridge_destroy_devmemcontext_post(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_SGX_UNREGISTER_HW_TRANSFER_CONTEXT):
		pvrsrv_bridge_sgx_unregister_hw_transfer_context_post(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_SGX_UNREGISTER_HW_RENDER_CONTEXT):
		pvrsrv_bridge_sgx_unregister_hw_render_context_post(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_SGX_RELEASECLIENTINFO):
		pvrsrv_bridge_sgx_releaseclientinfo_post(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_SGX_SUBMITTRANSFER):
		pvrsrv_bridge_sgx_submittransfer_post(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_SGX_DOKICK):
		pvrsrv_bridge_sgx_dokick_post(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_EVENT_OBJECT_WAIT):
		pvrsrv_bridge_event_object_wait_post(fd, ptr);
		break;
	case _IOC_NR(PVRSRV_BRIDGE_UNMAP_DEVICECLASS_MEMORY):
		pvrsrv_bridge_unmap_deviceclass_memory_post(fd, ptr);
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

void * mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset)
{
	void *ret = NULL;
	PROLOG(mmap);

	if ((fd >= 0) && get_pvrsrvkm_info(fd)) {
		struct buffer *buf = find_buffer(NULL, 0, offset, 0, 0);

		printf("< [%4d]         : mmap: addr=%p, length=%zd, prot=%x, flags=%x, offset=%ld\n",
				fd, addr, length, prot, flags, offset);

		if (buf && buf->hostptr) {
			buf->munmap = 0;
			ret = buf->hostptr;
		}
	}

	if (!ret) {
		ret = orig_mmap(addr, length, prot, flags, fd, offset);
	}

	if ((fd >= 0) && get_pvrsrvkm_info(fd)) {
		struct buffer *buf = find_buffer(NULL, 0, offset, 0, 0);
		if (buf)
			buf->hostptr = ret;
		else {
			/*
			 * when a buffer is allocated using IOCTL_KGSL_GPUMEM_ALLOC_ID
			 * it's mmapped by id, not by gpuaddr, so try to find that
			 * buffer via id now.
			 */
			buf = find_buffer(NULL, 0, 0, 0, offset >> 12);
			if (buf)
				buf->hostptr = ret;
		}
		printf("< [%4d]         : mmap: -> (%p)\n", fd, ret);
	}

	return ret;
}

int munmap(void *addr, size_t length)
{
	struct buffer *buf;
	int ret;
	PROLOG(munmap);

	buf = find_buffer(addr, 0, 0, 0, 0);
	if (buf) {
		/* we need the contents at submit ioctl: */
		printf("fake munmap: buf=%p\n", buf);
		buf->munmap = 1;
		ret = 0;
		goto out;
	}

	ret = orig_munmap(addr, length);
out:
	return ret;
}

long syscall(long sysno, ...)
{
	long error = -1;
	PROLOG(syscall);

	switch (sysno) {
#ifdef __NR_mmap2
		case __NR_mmap2: {
			void *addr, *ret = NULL;
			size_t length;
			int prot, flags, fd;
			off_t offset;
			struct buffer *buf = NULL;
			va_list args;

			va_start(args, sysno);
			addr = va_arg(args, void *);
			length = va_arg(args, size_t);
			prot = va_arg(args, int);
			flags = va_arg(args, int);
			fd = va_arg(args, int);
			offset = va_arg(args, off_t);
			va_end(args);

			if ((fd >= 0) && get_pvrsrvkm_info(fd)) {
				buf = find_buffer(NULL, 0, 0, (~0xC0000000) & offset, 0);

				printf("< [%4d]         : mmap: addr=%p, length=%zd, prot=%x, flags=%x, offset=%ld\n",
						fd, addr, length, prot, flags, offset);

				if (buf && buf->hostptr) {
					buf->munmap = 0;
					ret = buf->hostptr;
				}
			}

			if (!ret) {
				ret = (void *)orig_syscall(sysno, addr, length, prot, flags, fd, offset);
			}

			if ((fd >= 0) && get_pvrsrvkm_info(fd)) {
				if (buf) {
					buf->hostptr = ret;
					buf->flags = flags;
				}
				printf("< [%4d]         : mmap: -> (%p)\n", fd, ret);
			}

			error = (long)ret;
			break;
		}
#endif
		default:
			fprintf(stderr, "%s: unsupported syscall number %ld\n", __func__, sysno);
			break;
	}

	return error;
}

static LIST_HEAD(buffers_of_interest);

struct buffer * register_buffer(void *hostptr, uint64_t gpuaddr, uint64_t flags,
		unsigned int len, unsigned long handle)
{
	struct buffer *buf = calloc(1, sizeof *buf);
	buf->gpuaddr = gpuaddr;
	buf->hostptr = hostptr;
	buf->flags = flags;
	buf->len = len;
	buf->handle = handle;
	list_add(&buf->node, &buffers_of_interest);
	return buf;
}

void unregister_buffer(struct buffer *buf)
{
	if (buf) {
		list_del(&buf->node);
		if (buf->munmap)
			munmap(buf->hostptr, buf->len);
		free(buf);
	}
}

struct buffer * find_buffer(void *hostptr, uint64_t gpuaddr,
		uint64_t offset, unsigned long handle, unsigned id)
{
	struct buffer *buf = NULL;
	list_for_each_entry(buf, &buffers_of_interest, node) {
		if (hostptr)
			if ((buf->hostptr <= hostptr) && (hostptr < (buf->hostptr + buf->len)))
				return buf;
		if (gpuaddr)
			if ((buf->gpuaddr <= gpuaddr) && (gpuaddr < (buf->gpuaddr + buf->len)))
				return buf;
		if (offset)
			if ((buf->offset <= offset) && (offset < (buf->offset + buf->len)))
				return buf;
		if (handle)
			if (buf->handle == handle)
				return buf;
		if (id)
			if (buf->id == id)
				return buf;
	}
	return NULL;
}

void dump_buffer(struct buffer *buf, const char *buf_name)
{
	static int cnt = 0;
	if (buf && buf->hostptr && buf->len) {
		char filename[64];
		int fd;
		sprintf(filename, "%04d-%016"PRIx64"-%s.dat", cnt, buf->gpuaddr, buf_name);
		printf("\t\tdumping: %s\n", filename);
		fd = open(filename, O_WRONLY| O_TRUNC | O_CREAT, 0644);
		write(fd, buf->hostptr + buf->offset, buf->len);
		close(fd);
		cnt++;
		buf->dumped = 1;
	}
}

void dump_buffers(int fd)
{
	PROLOG(mmap);
	PROLOG(munmap);

	struct buffer *buf;
	char name[32];
	list_for_each_entry(buf, &buffers_of_interest, node) {
		if (!buf || buf->dumped)
		    continue;

		int need_unmap = 0;

		if (!buf->hostptr) {
			buf->hostptr = orig_mmap(0, buf->len, PROT_READ, MAP_SHARED, fd, buf->handle | 0x80000000);
			need_unmap = 1;
		}

		snprintf(name, sizeof(name), "%04lx-buffers", buf->handle);
		dump_buffer(buf, name);

		if (need_unmap) {
			orig_munmap(buf->hostptr, buf->len);
			buf->hostptr = NULL;
		}
	}
}

void dump_unregister_buffer(IMG_HANDLE handle, const char *buf_name)
{
	struct buffer *buf;

	buf = find_buffer(0, 0, 0, (unsigned long)handle, 0);
	if (buf) {
		dump_buffer(buf, buf_name);
		unregister_buffer(buf);
	}
}
