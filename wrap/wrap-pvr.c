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

void pvrsrv_bridge_initsrv_connect_pre(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	printf("\t\tNo param\n");
}

void pvrsrv_bridge_initsrv_connect_post(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	PVRSRV_BRIDGE_RETURN *out = param->pvParamOut;

	print_error(out->eError);
}

void pvrsrv_bridge_enum_devices_pre(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	printf("\t\tNo param\n");
}

static const char *devicetypenames[] = {
	ENUM_INFO(PVRSRV_DEVICE_TYPE_UNKNOWN),
	ENUM_INFO(PVRSRV_DEVICE_TYPE_MBX1),
	ENUM_INFO(PVRSRV_DEVICE_TYPE_MBX1_LITE),
	ENUM_INFO(PVRSRV_DEVICE_TYPE_M24VA),
	ENUM_INFO(PVRSRV_DEVICE_TYPE_MVDA2),
	ENUM_INFO(PVRSRV_DEVICE_TYPE_MVED1),
	ENUM_INFO(PVRSRV_DEVICE_TYPE_MSVDX),
	ENUM_INFO(PVRSRV_DEVICE_TYPE_SGX),
	ENUM_INFO(PVRSRV_DEVICE_TYPE_VGX),
	ENUM_INFO(PVRSRV_DEVICE_TYPE_EXT)
};

static const char *deviceclassnames[] = {
	ENUM_INFO(PVRSRV_DEVICE_CLASS_3D),
	ENUM_INFO(PVRSRV_DEVICE_CLASS_DISPLAY),
	ENUM_INFO(PVRSRV_DEVICE_CLASS_BUFFER),
	ENUM_INFO(PVRSRV_DEVICE_CLASS_VIDEO)
};

static void print_deviceidentifier(PVRSRV_DEVICE_IDENTIFIER *deviceidentifier)
{
	const char *typename;
	const char *classname;

	typename =
		(deviceidentifier->eDeviceType < ARRAY_SIZE(devicetypenames)) ?
		devicetypenames[deviceidentifier->eDeviceType] : NULL;
	printf("\t\ttype:\t\t%08x (%s)\n", deviceidentifier->eDeviceType,
		typename ? typename : "unknown");

	classname =
		(deviceidentifier->eDeviceClass < ARRAY_SIZE(deviceclassnames)) ?
		deviceclassnames[deviceidentifier->eDeviceClass] : NULL;
	printf("\t\tclass:\t\t%08x (%s)\n", deviceidentifier->eDeviceClass,
		classname ? classname : "unknown");

	printf("\t\tindex:\t\t%08x\n", deviceidentifier->ui32DeviceIndex);

	if (deviceidentifier->pszPDumpDevName) {
		printf("\t\tpdump dev:\t\t'%s'\n", deviceidentifier->pszPDumpDevName);
	}
	if (deviceidentifier->pszPDumpRegName) {
		printf("\t\tpdump reg:\t\t'%s'\n", deviceidentifier->pszPDumpRegName);
	}
}

void pvrsrv_bridge_enum_devices_post(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	PVRSRV_BRIDGE_OUT_ENUMDEVICE *out = param->pvParamOut;
	int i;

	print_error(out->eError);

	if (out->eError == PVRSRV_OK) {
		printf("\t\tnum devices:\t%u\n", out->ui32NumDevices);
		printf("\t\tmax devices:\t%u\n", PVRSRV_MAX_DEVICES);

		for (i = 0; i < out->ui32NumDevices; i++) {
			print_deviceidentifier(&out->asDeviceIdentifier[i]);
		}
	}
}

void pvrsrv_bridge_acquire_deviceinfo_pre(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	PVRSRV_BRIDGE_IN_ACQUIRE_DEVICEINFO *in = param->pvParamIn;
	const char *typename;

	printf("\t\tdev index:\t%08x\n", in->uiDevIndex);
	typename =
	        (in->eDeviceType < ARRAY_SIZE(devicetypenames)) ?
		devicetypenames[in->eDeviceType] : NULL;
	printf("\t\ttype:\t\t%08x (%s)\n", in->eDeviceType,
		typename ? typename : "unknown");
}

void pvrsrv_bridge_acquire_deviceinfo_post(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	PVRSRV_BRIDGE_OUT_ACQUIRE_DEVICEINFO *out = param->pvParamOut;

	print_error(out->eError);
	printf("\t\tdevice cookie:\t%p\n", out->hDevCookie);
}

void pvrsrv_bridge_alloc_devicemem_pre(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	PVRSRV_BRIDGE_IN_ALLOCDEVICEMEM *in = param->pvParamIn;

	printf("\t\tdevice cookie:\t%p\n", in->hDevCookie);
	printf("\t\tdevmem heap:\t%p\n", in->hDevMemHeap);
	print_attribs(in->ui32Attribs);
	printf("\t\tsize:\t\t%08x\n", in->ui32Size);
	printf("\t\talignment:\t%08x\n", in->ui32Alignment);
}

static void print_clientsyncinfo(PVRSRV_CLIENT_SYNC_INFO *clientsyncinfo)
{
	if (!clientsyncinfo) {
		return;
	}

	printf("\t\tsyncdata:\t%p\n", clientsyncinfo->psSyncData);
	printf("\t\twrops complete:\t%08x\n", clientsyncinfo->sWriteOpsCompleteDevVAddr.uiAddr);
	printf("\t\trdops complete:\t%08x\n", clientsyncinfo->sReadOpsCompleteDevVAddr.uiAddr);
	printf("\t\trdops2 complete:\t\t%08x\n", clientsyncinfo->sReadOps2CompleteDevVAddr.uiAddr);
	printf("\t\tmapping info:\t%p\n", clientsyncinfo->hMappingInfo);
	printf("\t\tkernelsyncinfo:\t%p\n", clientsyncinfo->hKernelSyncInfo);
}

static void print_clientmeminfo(PVRSRV_CLIENT_MEM_INFO *clientmeminfo)
{
	if (!clientmeminfo) {
		return;
	}

	printf("\t\tlinear addr:\t%p\n", clientmeminfo->pvLinAddr);
	printf("\t\tlinear addr KM:\t%p\n", clientmeminfo->pvLinAddrKM);
	printf("\t\tdev vaddr:\t%08x\n", clientmeminfo->sDevVAddr.uiAddr);
	printf("\t\tflags:\t\t%08x\n", clientmeminfo->ui32Flags);
	printf("\t\tclient flags:\t%08x\n", clientmeminfo->ui32ClientFlags);
	printf("\t\talloc size:\t%u\n", clientmeminfo->uAllocSize);
	printf("\t\tclientsyncinfo:\t%p\n", clientmeminfo->psClientSyncInfo);
	printf("\t\tmapping info:\t%p\n", clientmeminfo->hMappingInfo);
	printf("\t\tkernel meminfo:\t%p\n", clientmeminfo->hKernelMemInfo);
	printf("\t\tres item:\t%p\n", clientmeminfo->hResItem);
	printf("\t\tclient next:\t%p\n", clientmeminfo->psNext);
}

void pvrsrv_bridge_alloc_devicemem_post(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	PVRSRV_BRIDGE_OUT_ALLOCDEVICEMEM *out = param->pvParamOut;

	print_error(out->eError);

	if (out->eError == PVRSRV_OK) {
		PVRSRV_KERNEL_MEM_INFO  *psKernelMemInfo;
#if defined (SUPPORT_SID_INTERFACE)
	        psKernelMemInfo = out->hKernelMemInfo;
#else
		psKernelMemInfo = out->psKernelMemInfo;
#endif
		printf("\t\tkernel meminfo:\t%p (unused)\n", psKernelMemInfo);
		print_clientmeminfo(&out->sClientMemInfo);
		register_buffer(NULL, (uint64_t)(uintptr_t)out->sClientMemInfo.pvLinAddrKM,
				out->sClientMemInfo.ui32Flags,
				out->sClientMemInfo.uAllocSize,
				(unsigned long)out->sClientMemInfo.hKernelMemInfo);
		print_clientsyncinfo(&out->sClientSyncInfo);
		if (out->sClientSyncInfo.hKernelSyncInfo) {
		    register_buffer(NULL, (uint64_t)(uintptr_t)out->sClientSyncInfo.hKernelSyncInfo,
			    0, 0,
			    (unsigned long)out->sClientSyncInfo.hKernelSyncInfo);
		}
	}
}

void pvrsrv_bridge_initsrv_disconnect_pre(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	PVRSRV_BRIDGE_IN_INITSRV_DISCONNECT *in = param->pvParamIn;

	printf("\t\tinit success:\t%s\n", in->bInitSuccesful ? "true" : "false");
}

void pvrsrv_bridge_initsrv_disconnect_post(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	PVRSRV_BRIDGE_RETURN *out = param->pvParamOut;

	print_error(out->eError);
}

void pvrsrv_bridge_create_devmemcontext_pre(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	PVRSRV_BRIDGE_IN_CREATE_DEVMEMCONTEXT *in = param->pvParamIn;

	printf("\t\tdev cookie:\t%p\n", in->hDevCookie);
}

void pvrsrv_bridge_create_devmemcontext_post(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	PVRSRV_BRIDGE_OUT_CREATE_DEVMEMCONTEXT *out = param->pvParamOut;
	int i;

	print_error(out->eError);
	if (out->eError == PVRSRV_OK) {
		printf("\t\tdev mem ctxt:\t%p\n", out->hDevMemContext);
		printf("\t\tnum heap:\t%u\n", out->ui32ClientHeapCount);
		printf("\t\tmax heap:\t%u\n", PVRSRV_MAX_CLIENT_HEAPS);
		for (i = 0; i < out->ui32ClientHeapCount; i++) {
			print_heapinfo(i, &out->sHeapInfo[i]);
		}
	}
}

void pvrsrv_bridge_get_misc_info_pre(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	PVRSRV_BRIDGE_IN_GET_MISC_INFO *in = param->pvParamIn;

	print_miscinfo(&in->sMiscInfo);
}

void pvrsrv_bridge_get_misc_info_post(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	PVRSRV_BRIDGE_OUT_GET_MISC_INFO *out = param->pvParamOut;

	print_error(out->eError);
	print_miscinfo(&out->sMiscInfo);
}

void pvrsrv_bridge_get_devmem_heapinfo_pre(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	PVRSRV_BRIDGE_IN_GET_DEVMEM_HEAPINFO *in = param->pvParamIn;

	printf("\t\tdev cookie:\t%p\n", in->hDevCookie);
	printf("\t\tdevmem ctxt:\t%p\n", in->hDevMemContext);
}

void pvrsrv_bridge_get_devmem_heapinfo_post(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	PVRSRV_BRIDGE_OUT_GET_DEVMEM_HEAPINFO *out = param->pvParamOut;
	int i;

	print_error(out->eError);
	if (out->eError == PVRSRV_OK) {
		printf("\t\tclient count:\t%u\n", out->ui32ClientHeapCount);
		printf("\t\tclient max:\t%u\n", PVRSRV_MAX_CLIENT_HEAPS);
		for (i = 0; i < out->ui32ClientHeapCount; i++) {
			print_heapinfo(i, &out->sHeapInfo[i]);
		}
	}
}

void pvrsrv_bridge_getmmu_pd_devpaddr_pre(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	PVRSRV_BRIDGE_IN_GETMMU_PD_DEVPADDR *in = param->pvParamIn;

	printf("\t\tdevmem ctxt:\t%p\n", in->hDevMemContext);
}

void pvrsrv_bridge_getmmu_pd_devpaddr_post(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	PVRSRV_BRIDGE_OUT_GETMMU_PD_DEVPADDR *out = param->pvParamOut;

	print_error(out->eError);
	printf("\t\tpd paddr:\t%08x\n", out->sPDDevPAddr.uiAddr);
}

void pvrsrv_bridge_map_deviceclass_memory_pre(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	PVRSRV_BRIDGE_IN_MAP_DEVICECLASS_MEMORY *in = param->pvParamIn;

	printf("\t\tdevclass buf:\t%p\n", in->hDeviceClassBuffer);
	printf("\t\tdevmem ctxt:\t%p\n", in->hDevMemContext);
}

void pvrsrv_bridge_map_deviceclass_memory_post(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	PVRSRV_BRIDGE_OUT_MAP_DEVICECLASS_MEMORY *out = param->pvParamOut;

	print_error(out->eError);
	print_clientmeminfo(&out->sClientMemInfo);
	register_buffer(NULL, (uint64_t)(uintptr_t)out->sClientMemInfo.pvLinAddrKM,
		out->sClientMemInfo.ui32Flags,
		out->sClientMemInfo.uAllocSize,
		(unsigned long)out->sClientMemInfo.hKernelMemInfo);
	print_clientsyncinfo(&out->sClientSyncInfo);
#if defined (SUPPORT_SID_INTERFACE)
	printf("\t\tkernmeminfo:\t%p\n", out->hKernelMemInfo);
#else
	printf("\t\tkernmeminfo:\t%p\n", out->psKernelMemInfo);
#endif
	printf("\t\tmap info:\t%p\n", out->hMappingInfo);
}

void pvrsrv_bridge_free_devicemem_pre(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	PVRSRV_BRIDGE_IN_FREEDEVICEMEM *in = param->pvParamIn;
	char name[32];

	printf("\t\tdevclass buf:\t%p\n", in->hDevCookie);
#if defined (SUPPORT_SID_INTERFACE)
	printf("\t\tkernmeminfo:\t%p\n", in->hKernelMemInfo);
	snprintf(name, sizeof(name), "0x%x", in->hKernelMemInfo);
	dump_unregister_buffer((IMG_HANDLE)in->hKernelMemInfo, name);
#else
	printf("\t\tkernmeminfo:\t%p\n", in->psKernelMemInfo);
	snprintf(name, sizeof(name), "%p", in->psKernelMemInfo);
	dump_unregister_buffer((IMG_HANDLE)in->psKernelMemInfo, name);
#endif
	print_clientmeminfo(&in->sClientMemInfo);
}

void pvrsrv_bridge_free_devicemem_post(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	PVRSRV_BRIDGE_RETURN *out = param->pvParamOut;

	print_error(out->eError);
}

void pvrsrv_bridge_sync_ops_take_token_pre(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	PVRSRV_BRIDGE_IN_SYNC_OPS_TAKE_TOKEN *in = param->pvParamIn;

	printf("\t\tkernsyncinfo:\t%p\n", in->hKernelSyncInfo);
}

void pvrsrv_bridge_sync_ops_take_token_post(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	PVRSRV_BRIDGE_OUT_SYNC_OPS_TAKE_TOKEN *out = param->pvParamOut;

	print_error(out->eError);
	printf("\t\tread pending:\t%08x\n", out->ui32ReadOpsPending);
	printf("\t\twrite pending:\t%08x\n", out->ui32WriteOpsPending);
}

void pvrsrv_bridge_sync_ops_flush_to_token_pre(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	PVRSRV_BRIDGE_IN_SYNC_OPS_FLUSH_TO_TOKEN *in = param->pvParamIn;
	char name[32];

	printf("\t\tkernsyncinfo:\t%p\n", in->hKernelSyncInfo);
	snprintf(name, sizeof(name), "%p-SyncInfo", in->hKernelSyncInfo);
	dump_unregister_buffer(in->hKernelSyncInfo, name);
	printf("\t\tread pending:\t%08x\n", in->ui32ReadOpsPendingSnapshot);
	printf("\t\twrite pending:\t%08x\n", in->ui32WriteOpsPendingSnapshot);
}

void pvrsrv_bridge_sync_ops_flush_to_token_post(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	PVRSRV_BRIDGE_RETURN *out = param->pvParamOut;

	print_error(out->eError);
}

void pvrsrv_bridge_destroy_devmemcontext_pre(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	PVRSRV_BRIDGE_IN_DESTROY_DEVMEMCONTEXT *in = param->pvParamIn;

	printf("\t\tdev cookie:\t%p\n", in->hDevCookie);
	printf("\t\tdevmem ctxt:\t%p\n", in->hDevMemContext);
}

void pvrsrv_bridge_destroy_devmemcontext_post(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	PVRSRV_BRIDGE_RETURN *out = param->pvParamOut;

	print_error(out->eError);
}

void pvrsrv_bridge_unmap_deviceclass_memory_pre(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	PVRSRV_BRIDGE_IN_UNMAP_DEVICECLASS_MEMORY *in = param->pvParamIn;
	char name[32];

#if defined (SUPPORT_SID_INTERFACE)
	printf("\t\tkern meminfo:\t%p\n", in->hKernelMemInfo);
	snprintf(name, sizeof(name), "%p", in->hKernelMemInfo);
	dump_unregister_buffer((IMG_HANDLE)in->hKernelMemInfo, name);
#else
	printf("\t\tkern meminfo:\t%p\n", in->psKernelMemInfo);
	snprintf(name, sizeof(name), "%p", in->psKernelMemInfo);
	dump_unregister_buffer((IMG_HANDLE)in->psKernelMemInfo, name);
#endif
	print_clientmeminfo(&in->sClientMemInfo);
	print_clientsyncinfo(&in->sClientSyncInfo);
}

void pvrsrv_bridge_unmap_deviceclass_memory_post(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	PVRSRV_BRIDGE_RETURN *out = param->pvParamOut;

	print_error(out->eError);
}
