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

#ifndef WRAP_H_
#define WRAP_H_

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>

#include "config_sgx.h"

#include "pvr_bridge.h"
#include "sgx_bridge.h"
#include "sgx_mkif_km.h"

void * __rd_dlsym_helper(const char *name);

#define PROLOG(func)					\
	static __typeof__(func) *orig_##func = NULL;	\
	if (!orig_##func)				\
		orig_##func = __rd_dlsym_helper(#func);	\

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

struct buffer {
	void *hostptr;
	unsigned int len, id;
	unsigned long handle;
	uint64_t flags;
	uint64_t gpuaddr;
	uint64_t offset;
	struct list node;
	int munmap;
	int dumped;
};

struct buffer * register_buffer(void *hostptr, uint64_t gpuaddr, uint64_t flags,
		unsigned int len, unsigned long handle);
void unregister_buffer(struct buffer *buf);
struct buffer * find_buffer(void *hostptr, uint64_t gpuaddr,
		uint64_t offset, unsigned long handle, unsigned id);
void dump_buffer(struct buffer *buf, const char *buf_name);
void dump_unregister_buffer(IMG_HANDLE handle, const char *buf_name);
void dump_buffers(int fd);

void pvrsrv_bridge_event_object_open_pre(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_event_object_open_post(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_event_object_close_pre(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_event_object_close_post(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_open_dispclass_device_pre(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_open_dispclass_device_post(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_get_dispclass_info_pre(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_get_dispclass_info_post(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_close_dispclass_device_pre(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_close_dispclass_device_post(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_enum_dispclass_formats_pre(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_enum_dispclass_formats_post(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_enum_dispclass_dims_pre(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_enum_dispclass_dims_post(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_get_dispclass_sysbuffer_pre(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_get_dispclass_sysbuffer_post(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_mhandle_to_mmap_data_pre(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_mhandle_to_mmap_data_post(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_release_mmap_data_pre(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_release_mmap_data_post(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_connect_services_pre(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_connect_services_post(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_disconnect_services_pre(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_disconnect_services_post(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_initsrv_connect_pre(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_initsrv_connect_post(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_enum_devices_pre(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_enum_devices_post(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_acquire_deviceinfo_pre(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_acquire_deviceinfo_post(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_alloc_devicemem_pre(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_alloc_devicemem_post(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_initsrv_disconnect_pre(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_initsrv_disconnect_post(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_create_devmemcontext_pre(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_create_devmemcontext_post(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_get_misc_info_pre(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_get_misc_info_post(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_get_devmem_heapinfo_pre(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_get_devmem_heapinfo_post(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_getmmu_pd_devpaddr_pre(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_getmmu_pd_devpaddr_post(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_map_deviceclass_memory_pre(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_map_deviceclass_memory_post(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_free_devicemem_pre(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_free_devicemem_post(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_sync_ops_take_token_pre(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_sync_ops_take_token_post(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_sync_ops_flush_to_token_pre(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_sync_ops_flush_to_token_post(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_destroy_devmemcontext_pre(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_destroy_devmemcontext_post(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_sgxinfo_for_srvinit_pre(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_sgxinfo_for_srvinit_post(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_sgx_devinitpart2_pre(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_sgx_devinitpart2_post(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_sgx_getmiscinfo_pre(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_sgx_getmiscinfo_post(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_sgx_getclientinfo_pre(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_sgx_getclientinfo_post(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_sgx_getinternaldevinfo_pre(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_sgx_getinternaldevinfo_post(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_sgx_register_hw_render_context_pre(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_sgx_register_hw_render_context_post(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_sgx_register_hw_transfer_context_pre(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_sgx_register_hw_transfer_context_post(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_enum_class_pre(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_enum_class_post(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_sgx_flush_hw_render_target_pre(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_sgx_flush_hw_render_target_post(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_sgx_unregister_hw_transfer_context_pre(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_sgx_unregister_hw_transfer_context_post(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_sgx_unregister_hw_render_context_pre(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_sgx_unregister_hw_render_context_post(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_sgx_releaseclientinfo_pre(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_sgx_releaseclientinfo_post(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_sgx_submittransfer_pre(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_sgx_submittransfer_post(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_sgx_dokick_pre(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_sgx_dokick_post(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_event_object_wait_pre(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_event_object_wait_post(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_unmap_deviceclass_memory_pre(int fd, PVRSRV_BRIDGE_PACKAGE *param);
void pvrsrv_bridge_unmap_deviceclass_memory_post(int fd, PVRSRV_BRIDGE_PACKAGE *param);

#define ENUM_INFO(n) [n] = #n
void print_error(PVRSRV_ERROR error);
void print_heapinfo(int i, PVRSRV_HEAP_INFO *heapinfo);
const char *heap_str(void *devmem_heap);
void print_eventobject(PVRSRV_EVENTOBJECT *eventobject);
void print_miscinfo(PVRSRV_MISC_INFO *miscinfo);
void print_attribs(IMG_UINT32 ui32Attribs);

struct head {
    bool used;
    unsigned int heap_id;
};

#endif /* WRAP_H_ */
