/**********************************************************************
 *
 * Copyright (C) Imagination Technologies Ltd. All rights reserved.
 * 
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 * 
 * This program is distributed in the hope it will be useful but, except 
 * as otherwise stated in writing, without any warranty; without even the 
 * implied warranty of merchantability or fitness for a particular purpose. 
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 * 
 * The full GNU General Public License is included in this distribution in
 * the file called "COPYING".
 *
 * Contact Information:
 * Imagination Technologies Ltd. <gpl-support@imgtec.com>
 * Home Park Estate, Kings Langley, Herts, WD4 8LZ, UK 
 *
 ******************************************************************************/

#if !defined (__SGXINFO_H__)
#define __SGXINFO_H__

#include "sgxscript.h"
#include "servicesint.h"
#include "services.h"
#if !defined (SUPPORT_SID_INTERFACE)
#include "sgxapi_km.h"
#endif
#include "sgx_mkif_km.h"


#define SGX_MAX_DEV_DATA			24
#define	SGX_MAX_INIT_MEM_HANDLES	18


typedef struct _SGX_BRIDGE_INFO_FOR_SRVINIT
{
	IMG_DEV_PHYADDR sPDDevPAddr;
	PVRSRV_HEAP_INFO asHeapInfo[PVRSRV_MAX_CLIENT_HEAPS];
} SGX_BRIDGE_INFO_FOR_SRVINIT;


typedef enum _SGXMKIF_CMD_TYPE_
{
	SGXMKIF_CMD_TA				= 0,
	SGXMKIF_CMD_TRANSFER		= 1,
	SGXMKIF_CMD_2D				= 2,
	SGXMKIF_CMD_POWER			= 3,
	SGXMKIF_CMD_CONTEXTSUSPEND	= 4,
	SGXMKIF_CMD_CLEANUP			= 5,
	SGXMKIF_CMD_GETMISCINFO		= 6,
	SGXMKIF_CMD_PROCESS_QUEUES	= 7,
	SGXMKIF_CMD_DATABREAKPOINT	= 8,
	SGXMKIF_CMD_SETHWPERFSTATUS	= 9,
 	SGXMKIF_CMD_FLUSHPDCACHE	= 10,
 	SGXMKIF_CMD_MAX				= 11,

	SGXMKIF_CMD_FORCE_I32   	= -1,

} SGXMKIF_CMD_TYPE;


typedef struct _SGX_BRIDGE_INIT_INFO_
{
#if defined (SUPPORT_SID_INTERFACE)
	IMG_SID		hKernelCCBMemInfo;
	IMG_SID		hKernelCCBCtlMemInfo;
	IMG_SID		hKernelCCBEventKickerMemInfo;
	IMG_SID		hKernelSGXHostCtlMemInfo;
	IMG_SID		hKernelSGXTA3DCtlMemInfo;
#if defined(FIX_HW_BRN_31272) || defined(FIX_HW_BRN_31780) || defined(FIX_HW_BRN_33920)
	IMG_SID		hKernelSGXPTLAWriteBackMemInfo;
#endif
	IMG_SID		hKernelSGXMiscMemInfo;
#else
	IMG_HANDLE	hKernelCCBMemInfo;
	IMG_HANDLE	hKernelCCBCtlMemInfo;
	IMG_HANDLE	hKernelCCBEventKickerMemInfo;
	IMG_HANDLE	hKernelSGXHostCtlMemInfo;
	IMG_HANDLE	hKernelSGXTA3DCtlMemInfo;
#if defined(FIX_HW_BRN_31272) || defined(FIX_HW_BRN_31780) || defined(FIX_HW_BRN_33920)
	IMG_HANDLE	hKernelSGXPTLAWriteBackMemInfo;
#endif
	IMG_HANDLE	hKernelSGXMiscMemInfo;
#endif

	IMG_UINT32	aui32HostKickAddr[SGXMKIF_CMD_MAX];

	SGX_INIT_SCRIPTS sScripts;

	IMG_UINT32	ui32ClientBuildOptions;
	SGX_MISCINFO_STRUCT_SIZES	sSGXStructSizes;

#if defined(SGX_SUPPORT_HWPROFILING)
#if defined (SUPPORT_SID_INTERFACE)
	IMG_SID		hKernelHWProfilingMemInfo;
#else
	IMG_HANDLE	hKernelHWProfilingMemInfo;
#endif
#endif
#if defined(SUPPORT_SGX_HWPERF)
#if defined (SUPPORT_SID_INTERFACE)
	IMG_SID		hKernelHWPerfCBMemInfo;
#else
	IMG_HANDLE	hKernelHWPerfCBMemInfo;
#endif
#endif
#if defined (SUPPORT_SID_INTERFACE)
	IMG_SID		hKernelTASigBufferMemInfo;
	IMG_SID		hKernel3DSigBufferMemInfo;
#else
	IMG_HANDLE	hKernelTASigBufferMemInfo;
	IMG_HANDLE	hKernel3DSigBufferMemInfo;
#endif

#if defined(FIX_HW_BRN_29702)
#if defined (SUPPORT_SID_INTERFACE)
	IMG_SID		hKernelCFIMemInfo;
#else
	IMG_HANDLE	hKernelCFIMemInfo;
#endif
#endif
#if defined(FIX_HW_BRN_29823)
#if defined (SUPPORT_SID_INTERFACE)
	IMG_SID		hKernelDummyTermStreamMemInfo;
#else
	IMG_HANDLE	hKernelDummyTermStreamMemInfo;
#endif
#endif

#if defined(FIX_HW_BRN_31542)
#if defined (SUPPORT_SID_INTERFACE)
	IMG_SID hKernelClearClipWAVDMStreamMemInfo;
	IMG_SID hKernelClearClipWAIndexStreamMemInfo;
	IMG_SID hKernelClearClipWAPDSMemInfo;
	IMG_SID hKernelClearClipWAUSEMemInfo;
	IMG_SID hKernelClearClipWAParamMemInfo;
	IMG_SID hKernelClearClipWAPMPTMemInfo;
	IMG_SID hKernelClearClipWATPCMemInfo;
	IMG_SID hKernelClearClipWAPSGRgnHdrMemInfo;
#else
	IMG_HANDLE hKernelClearClipWAVDMStreamMemInfo;
	IMG_HANDLE hKernelClearClipWAIndexStreamMemInfo;
	IMG_HANDLE hKernelClearClipWAPDSMemInfo;
	IMG_HANDLE hKernelClearClipWAUSEMemInfo;
	IMG_HANDLE hKernelClearClipWAParamMemInfo;
	IMG_HANDLE hKernelClearClipWAPMPTMemInfo;
	IMG_HANDLE hKernelClearClipWATPCMemInfo;
	IMG_HANDLE hKernelClearClipWAPSGRgnHdrMemInfo;
#endif
#endif

#if defined(SGX_FEATURE_VDM_CONTEXT_SWITCH) && defined(FIX_HW_BRN_31425)
	IMG_HANDLE	hKernelVDMSnapShotBufferMemInfo;
	IMG_HANDLE	hKernelVDMCtrlStreamBufferMemInfo;
#endif
#if defined(SGX_FEATURE_VDM_CONTEXT_SWITCH) && \
	defined(FIX_HW_BRN_33657) && defined(SUPPORT_SECURE_33657_FIX)
	IMG_HANDLE	hKernelVDMStateUpdateBufferMemInfo;
#endif
#if defined(PVRSRV_USSE_EDM_STATUS_DEBUG)
#if defined (SUPPORT_SID_INTERFACE)
	IMG_SID		hKernelEDMStatusBufferMemInfo;
#else
	IMG_HANDLE	hKernelEDMStatusBufferMemInfo;
#endif
#endif
#if defined(SGX_FEATURE_OVERLAPPED_SPM)
#if defined (SUPPORT_SID_INTERFACE)
	IMG_SID		hKernelTmpRgnHeaderMemInfo;
#else
	IMG_HANDLE hKernelTmpRgnHeaderMemInfo;
#endif
#endif

	IMG_UINT32 ui32EDMTaskReg0;
	IMG_UINT32 ui32EDMTaskReg1;

	IMG_UINT32 ui32ClkGateCtl;
	IMG_UINT32 ui32ClkGateCtl2;
	IMG_UINT32 ui32ClkGateStatusReg;
	IMG_UINT32 ui32ClkGateStatusMask;
#if defined(SGX_FEATURE_MP)
	IMG_UINT32 ui32MasterClkGateStatusReg;
	IMG_UINT32 ui32MasterClkGateStatusMask;
	IMG_UINT32 ui32MasterClkGateStatus2Reg;
	IMG_UINT32 ui32MasterClkGateStatus2Mask;
#endif 

	IMG_UINT32 ui32CacheControl;

	IMG_UINT32	asInitDevData[SGX_MAX_DEV_DATA];
#if defined (SUPPORT_SID_INTERFACE)
	IMG_SID		asInitMemHandles[SGX_MAX_INIT_MEM_HANDLES];
#else
	IMG_HANDLE	asInitMemHandles[SGX_MAX_INIT_MEM_HANDLES];
#endif

} SGX_BRIDGE_INIT_INFO;


typedef struct _SGX_DEVICE_SYNC_LIST_
{
	PSGXMKIF_HWDEVICE_SYNC_LIST	psHWDeviceSyncList;

#if defined (SUPPORT_SID_INTERFACE)
	IMG_SID					hKernelHWSyncListMemInfo;
#else
	IMG_HANDLE				hKernelHWSyncListMemInfo;
#endif
	PVRSRV_CLIENT_MEM_INFO	*psHWDeviceSyncListClientMemInfo;
	PVRSRV_CLIENT_MEM_INFO	*psAccessResourceClientMemInfo;

	volatile IMG_UINT32		*pui32Lock;

	struct _SGX_DEVICE_SYNC_LIST_	*psNext;

	
	IMG_UINT32			ui32NumSyncObjects;
#if defined (SUPPORT_SID_INTERFACE)
	IMG_SID				ahSyncHandles[1];
#else
	IMG_HANDLE			ahSyncHandles[1];
#endif
} SGX_DEVICE_SYNC_LIST, *PSGX_DEVICE_SYNC_LIST;


typedef struct _SGX_INTERNEL_STATUS_UPDATE_
{
	CTL_STATUS				sCtlStatus;
#if defined (SUPPORT_SID_INTERFACE)
	IMG_SID					hKernelMemInfo;
#else
	IMG_HANDLE				hKernelMemInfo;
#endif
} SGX_INTERNEL_STATUS_UPDATE;


typedef struct _SGX_CCB_KICK_
{
	SGXMKIF_COMMAND		sCommand;
#if defined (SUPPORT_SID_INTERFACE)
	IMG_SID		hCCBKernelMemInfo;
#else
	IMG_HANDLE	hCCBKernelMemInfo;
#endif

	IMG_UINT32	ui32NumDstSyncObjects;
#if defined (SUPPORT_SID_INTERFACE)
	IMG_SID		hKernelHWSyncListMemInfo;
#else
	IMG_HANDLE	hKernelHWSyncListMemInfo;
#endif

	
#if defined (SUPPORT_SID_INTERFACE)
	IMG_SID		*pahDstSyncHandles;
#else
	IMG_HANDLE	*pahDstSyncHandles;
#endif

	IMG_UINT32	ui32NumTAStatusVals;
	IMG_UINT32	ui32Num3DStatusVals;

#if defined(SUPPORT_SGX_NEW_STATUS_VALS)
	SGX_INTERNEL_STATUS_UPDATE	asTAStatusUpdate[SGX_MAX_TA_STATUS_VALS];
	SGX_INTERNEL_STATUS_UPDATE	as3DStatusUpdate[SGX_MAX_3D_STATUS_VALS];
#else
#if defined (SUPPORT_SID_INTERFACE)
	IMG_SID		ahTAStatusSyncInfo[SGX_MAX_TA_STATUS_VALS];
	IMG_SID		ah3DStatusSyncInfo[SGX_MAX_3D_STATUS_VALS];
#else
	IMG_HANDLE	ahTAStatusSyncInfo[SGX_MAX_TA_STATUS_VALS];
	IMG_HANDLE	ah3DStatusSyncInfo[SGX_MAX_3D_STATUS_VALS];
#endif
#endif

	IMG_BOOL	bFirstKickOrResume;
#if (defined(NO_HARDWARE) || defined(PDUMP))
	IMG_BOOL	bTerminateOrAbort;
#endif
	IMG_BOOL	bLastInScene;

	
	IMG_UINT32	ui32CCBOffset;

#if defined(SUPPORT_SGX_GENERALISED_SYNCOBJECTS)
	
	IMG_UINT32	ui32NumTASrcSyncs;
#if defined (SUPPORT_SID_INTERFACE)
	IMG_SID		ahTASrcKernelSyncInfo[SGX_MAX_TA_SRC_SYNCS];
#else
	IMG_HANDLE	ahTASrcKernelSyncInfo[SGX_MAX_TA_SRC_SYNCS];
#endif
	IMG_UINT32	ui32NumTADstSyncs;
#if defined (SUPPORT_SID_INTERFACE)
	IMG_SID		ahTADstKernelSyncInfo[SGX_MAX_TA_DST_SYNCS];
#else
	IMG_HANDLE	ahTADstKernelSyncInfo[SGX_MAX_TA_DST_SYNCS];
#endif
	IMG_UINT32	ui32Num3DSrcSyncs;
#if defined (SUPPORT_SID_INTERFACE)
	IMG_SID		ah3DSrcKernelSyncInfo[SGX_MAX_3D_SRC_SYNCS];
#else
	IMG_HANDLE	ah3DSrcKernelSyncInfo[SGX_MAX_3D_SRC_SYNCS];
#endif
#else
	
	IMG_UINT32	ui32NumSrcSyncs;
#if defined (SUPPORT_SID_INTERFACE)
	IMG_SID		ahSrcKernelSyncInfo[SGX_MAX_SRC_SYNCS];
#else
	IMG_HANDLE	ahSrcKernelSyncInfo[SGX_MAX_SRC_SYNCS];
#endif
#endif

	
	IMG_BOOL	bTADependency;
#if defined (SUPPORT_SID_INTERFACE)
	IMG_SID		hTA3DSyncInfo;

	IMG_SID		hTASyncInfo;
	IMG_SID		h3DSyncInfo;
#else
	IMG_HANDLE	hTA3DSyncInfo;

	IMG_HANDLE	hTASyncInfo;
	IMG_HANDLE	h3DSyncInfo;
#endif
#if defined(PDUMP)
	IMG_UINT32	ui32CCBDumpWOff;
#endif
#if defined(NO_HARDWARE)
	IMG_UINT32	ui32WriteOpsPendingVal;
#endif
	IMG_HANDLE	hDevMemContext;
} SGX_CCB_KICK;


#define SGX_KERNEL_USE_CODE_BASE_INDEX		15


typedef struct _SGX_CLIENT_INFO_
{
	IMG_UINT32					ui32ProcessID;			
	IMG_VOID					*pvProcess;				
	PVRSRV_MISC_INFO			sMiscInfo;				

	IMG_UINT32					asDevData[SGX_MAX_DEV_DATA]; /* 144 */

} SGX_CLIENT_INFO;

typedef struct _SGX_INTERNAL_DEVINFO_
{
	IMG_UINT32			ui32Flags;
#if defined (SUPPORT_SID_INTERFACE)
	IMG_SID				hHostCtlKernelMemInfoHandle;
#else
	IMG_HANDLE			hHostCtlKernelMemInfoHandle;
#endif
	IMG_BOOL			bForcePTOff;
} SGX_INTERNAL_DEVINFO;


typedef struct _SGX_INTERNAL_DEVINFO_KM_
{
	IMG_UINT32			ui32Flags;
	IMG_HANDLE			hHostCtlKernelMemInfoHandle;
	IMG_BOOL			bForcePTOff;
} SGX_INTERNAL_DEVINFO_KM;


#if defined(TRANSFER_QUEUE)
typedef struct _PVRSRV_TRANSFER_SGX_KICK_
{
#if defined (SUPPORT_SID_INTERFACE)
	IMG_SID			hCCBMemInfo;
#else
	IMG_HANDLE		hCCBMemInfo;
#endif
	IMG_UINT32		ui32SharedCmdCCBOffset;

	IMG_DEV_VIRTADDR 	sHWTransferContextDevVAddr;

#if defined (SUPPORT_SID_INTERFACE)
	IMG_SID			hTASyncInfo;
	IMG_SID			h3DSyncInfo;
#else
	IMG_HANDLE		hTASyncInfo;
	IMG_HANDLE		h3DSyncInfo;
#endif

	IMG_UINT32		ui32NumSrcSync;
#if defined (SUPPORT_SID_INTERFACE)
	IMG_SID			ahSrcSyncInfo[SGX_MAX_TRANSFER_SYNC_OPS];
#else
	IMG_HANDLE		ahSrcSyncInfo[SGX_MAX_TRANSFER_SYNC_OPS];
#endif

	IMG_UINT32		ui32NumDstSync;
#if defined (SUPPORT_SID_INTERFACE)
	IMG_SID			ahDstSyncInfo[SGX_MAX_TRANSFER_SYNC_OPS];
#else
	IMG_HANDLE		ahDstSyncInfo[SGX_MAX_TRANSFER_SYNC_OPS];
#endif

	IMG_UINT32		ui32Flags;

	IMG_UINT32		ui32PDumpFlags;
#if defined(PDUMP)
	IMG_UINT32		ui32CCBDumpWOff;
#endif
	IMG_HANDLE		hDevMemContext;
} PVRSRV_TRANSFER_SGX_KICK, *PPVRSRV_TRANSFER_SGX_KICK;

#if defined(SGX_FEATURE_2D_HARDWARE)
typedef struct _PVRSRV_2D_SGX_KICK_
{
#if defined (SUPPORT_SID_INTERFACE)
	IMG_SID			hCCBMemInfo;
#else
	IMG_HANDLE		hCCBMemInfo;
#endif
	IMG_UINT32		ui32SharedCmdCCBOffset;

	IMG_DEV_VIRTADDR 	sHW2DContextDevVAddr;

	IMG_UINT32		ui32NumSrcSync;
#if defined (SUPPORT_SID_INTERFACE)
	IMG_SID			ahSrcSyncInfo[SGX_MAX_2D_SRC_SYNC_OPS];

	
	IMG_SID	 		hDstSyncInfo;

	
	IMG_SID			hTASyncInfo;

	
	IMG_SID			h3DSyncInfo;
#else
	IMG_HANDLE		ahSrcSyncInfo[SGX_MAX_2D_SRC_SYNC_OPS];

	
	IMG_HANDLE 		hDstSyncInfo;

	
	IMG_HANDLE		hTASyncInfo;

	
	IMG_HANDLE		h3DSyncInfo;
#endif

	IMG_UINT32		ui32PDumpFlags;
#if defined(PDUMP)
	IMG_UINT32		ui32CCBDumpWOff;
#endif
	IMG_HANDLE		hDevMemContext;
} PVRSRV_2D_SGX_KICK, *PPVRSRV_2D_SGX_KICK;
#endif	
#endif	


#endif 
