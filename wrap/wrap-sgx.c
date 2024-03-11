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

void pvrsrv_bridge_sgxinfo_for_srvinit_pre(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	PVRSRV_BRIDGE_IN_SGXINFO_FOR_SRVINIT *in = param->pvParamIn;

	printf("\t\tdevice cookie:\t%p\n", in->hDevCookie);
}

static void print_infoforsrvinit(SGX_BRIDGE_INFO_FOR_SRVINIT *infoforsrvinit)
{
	int i;

	printf("\t\tdev paddr:\t%08x\n", infoforsrvinit->sPDDevPAddr.uiAddr);
	printf("\t\tmax heap:\t%u\n", PVRSRV_MAX_CLIENT_HEAPS);
	for (i = 0; i < PVRSRV_MAX_CLIENT_HEAPS; i++) {
		print_heapinfo(i, &infoforsrvinit->asHeapInfo[i]);
	}
}
void pvrsrv_bridge_sgxinfo_for_srvinit_post(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	PVRSRV_BRIDGE_OUT_SGXINFO_FOR_SRVINIT *out = param->pvParamOut;

	print_error(out->eError);

	if (out->eError == PVRSRV_OK) {
		print_infoforsrvinit(&out->sInitInfo);
	}
}

static const char *initopnames[] = {
		ENUM_INFO(SGX_INIT_OP_ILLEGAL),
		ENUM_INFO(SGX_INIT_OP_WRITE_HW_REG),
#if defined(PDUMP)
		ENUM_INFO(SGX_INIT_OP_PDUMP_HW_REG),
#endif
		ENUM_INFO(SGX_INIT_OP_HALT)
};
static void print_initcmds(int maxcmds, const char *str, SGX_INIT_COMMAND *initcmds)
{
	const char *opname;
	int i;

	if (!initcmds) {
		return;
	}

	for (i = 0; i < maxcmds; i++) {
		opname =
			(initcmds[i].eOp < ARRAY_SIZE(initopnames)) ?
			initopnames[initcmds[i].eOp] : NULL;
		printf("\t\t%s[%i]:\t%08x (%s)\n", str, i, initcmds[i].eOp,
			opname ? opname : "unknown");
		if (initcmds[i].eOp == SGX_INIT_OP_WRITE_HW_REG) {
			printf("\t\toffset:\t\t%08x\n", initcmds[i].sWriteHWReg.ui32Offset);
			printf("\t\tvalue:\t\t%08x\n", initcmds[i].sWriteHWReg.ui32Value);
		}
	}
}

static void print_script(SGX_INIT_SCRIPTS *script)
{
	if (!script) {
		return;
	}
	print_initcmds(SGX_MAX_INIT_COMMANDS, "initcmd p1", script->asInitCommandsPart1);
	print_initcmds(SGX_MAX_INIT_COMMANDS, "initcmd p2", script->asInitCommandsPart2);
	print_initcmds(SGX_MAX_DEINIT_COMMANDS, "deinit cmd", script->asDeinitCommands);
}

static void print_structsizes(SGX_MISCINFO_STRUCT_SIZES *structsizes)
{
	if (!structsizes) {
		return;
	}
	printf("\t\tcmd TA:\t\t%08x\n", structsizes->ui32Sizeof_CMDTA);
	printf("\t\tcmd TA shared:\t%08x\n", structsizes->ui32Sizeof_CMDTA_SHARED);
	printf("\t\ttransfer cmd:\t%08x\n", structsizes->ui32Sizeof_TRANSFERCMD);
	printf("\t\ttransfer shd:\t%08x\n", structsizes->ui32Sizeof_TRANSFERCMD_SHARED);
	printf("\t\t3D registers:\t%08x\n", structsizes->ui32Sizeof_3DREGISTERS);
	printf("\t\thwpbdesc:\t%08x\n", structsizes->ui32Sizeof_HWPBDESC);
	printf("\t\thwrenderctxt:\t%08x\n", structsizes->ui32Sizeof_HWRENDERCONTEXT);
	printf("\t\thwrenderdetail:\t%08x\n", structsizes->ui32Sizeof_HWRENDERDETAILS);
	printf("\t\thwrtdata:\t%08x\n", structsizes->ui32Sizeof_HWRTDATA);
	printf("\t\thwrtdataset:\t%08x\n", structsizes->ui32Sizeof_HWRTDATASET);
	printf("\t\thwtransferctxt:\t%08x\n", structsizes->ui32Sizeof_HWTRANSFERCONTEXT);
	printf("\t\thost ctl:\t%08x\n", structsizes->ui32Sizeof_HOST_CTL);
	printf("\t\tcommand:\t%08x\n", structsizes->ui32Sizeof_COMMAND);
}

static void print_sgxmkif_host_ctl(SGXMKIF_HOST_CTL *host_ctl)
{
#if defined(SGX_FEATURE_EXTENDED_PERF_COUNTERS)
	int i;
#endif

	if (!host_ctl) {
	    return;
	}
#if defined(PVRSRV_USSE_EDM_BREAKPOINTS)
	printf("\t\tbrkpoint:\t%08x\n", host_ctl->ui32BreakpointDisable);
	printf("\t\tcontinue:\t%08x\n", host_ctl->ui32Continue);
#endif
	printf("\t\tinit stat:\t%08x\n", host_ctl->ui32InitStatus);
	printf("\t\tpwr stat:\t%08x\n", host_ctl->ui32PowerStatus);
	printf("\t\tcln stat:\t%08x\n", host_ctl->ui32CleanupStatus);
#if defined(FIX_HW_BRN_28889)
	printf("\t\tinvl stat:\t%08x\n", host_ctl->ui32InvalStatus);
#endif
#if defined(SUPPORT_HW_RECOVERY)
	printf("\t\tukern lock:\t%08x\n", host_ctl->ui32uKernelDetectedLockups);
	printf("\t\thost lock:\t%08x\n", host_ctl->ui32HostDetectedLockups);
	printf("\t\thwrecov rate:\t%08x\n", host_ctl->ui32HWRecoverySampleRate);
#endif
	printf("\t\tukern clk:\t%08x\n", host_ctl->ui32uKernelTimerClock);
	printf("\t\tpowman rate:\t%08x\n", host_ctl->ui32ActivePowManSampleRate);
	printf("\t\tintr flg:\t%08x\n", host_ctl->ui32InterruptFlags);
	printf("\t\tintr clr:\t%08x\n", host_ctl->ui32InterruptClearFlags);
	printf("\t\tbp clr sig:\t%08x\n", host_ctl->ui32BPSetClearSignal);
	printf("\t\tnum powevts:\t%08x\n", host_ctl->ui32NumActivePowerEvents);
	printf("\t\ttime wraps:\t%08x\n", host_ctl->ui32TimeWraps);
	printf("\t\thost clk:\t%08x\n", host_ctl->ui32HostClock);
	printf("\t\tassert fail:\t%08x\n", host_ctl->ui32AssertFail);
#if defined(SGX_FEATURE_EXTENDED_PERF_COUNTERS)
	printf("\t\tperf grp:\t");
	for (i = 0; i < PVRSRV_SGX_HWPERF_NUM_COUNTERS; i++) {
		printf("%08x ", host_ctl->aui32PerfGroup[i]);
	}
	printf("\n");
	printf("\t\tperf bit:\t");
	for (i = 0; i < PVRSRV_SGX_HWPERF_NUM_COUNTERS; i++) {
		printf("%08x ", host_ctl->aui32PerfBit[i]);
	}
	printf("\n");
#else
	printf("\t\tperf grp:\t%08x\n", host_ctl->ui32PerfGroup);
#endif
#if defined(FIX_HW_BRN_31939)
	printf("\t\tbrn31939:\t%08x\n", host_ctl->ui32BRN31939Mem);
#endif
	printf("\t\topencl cnt:\t%08x\n", host_ctl->ui32OpenCLDelayCount);
}

static void print_bridgeinitinfo(SGX_BRIDGE_INIT_INFO *bridgeinitinfo)
{
	int i;
	struct buffer *buf;

	if (!bridgeinitinfo) {
		return;
	}
	printf("\t\tCCB:\t\t%p\n", bridgeinitinfo->hKernelCCBMemInfo);
	dump_unregister_buffer(bridgeinitinfo->hKernelCCBMemInfo, "CCB");
	printf("\t\tCCB ctl:\t%p\n", bridgeinitinfo->hKernelCCBCtlMemInfo);
	dump_unregister_buffer(bridgeinitinfo->hKernelCCBCtlMemInfo, "CCBCtl");
	printf("\t\tCCB event kick:\t%p\n", bridgeinitinfo->hKernelCCBEventKickerMemInfo);
	dump_unregister_buffer(bridgeinitinfo->hKernelCCBEventKickerMemInfo, "CCBEventKicker");
	printf("\t\tSGX host ctl:\t%p\n", bridgeinitinfo->hKernelSGXHostCtlMemInfo);
	buf = find_buffer(0, 0, 0, (unsigned long)bridgeinitinfo->hKernelSGXHostCtlMemInfo, 0);
	print_sgxmkif_host_ctl(buf->hostptr + buf->offset);
	dump_unregister_buffer(bridgeinitinfo->hKernelSGXHostCtlMemInfo, "SGXHostCtl");
	printf("\t\tSGX TA3D ctl:\t%p\n", bridgeinitinfo->hKernelSGXTA3DCtlMemInfo);
	dump_unregister_buffer(bridgeinitinfo->hKernelSGXTA3DCtlMemInfo, "SGXTA3DCtl");
#if defined(FIX_HW_BRN_31272) || defined(FIX_HW_BRN_31780) || defined(FIX_HW_BRN_33920)
	printf("\t\tSGX PTLA WB:\t%p\n", bridgeinitinfo->hKernelSGXPTLAWriteBackMemInfo);
	dump_unregister_buffer(bridgeinitinfo->hKernelSGXPTLAWriteBackMemInfo, "SGXPTLAWriteBack");
#endif
	printf("\t\tSGX misc:\t%p\n", bridgeinitinfo->hKernelSGXMiscMemInfo);
	dump_unregister_buffer(bridgeinitinfo->hKernelSGXMiscMemInfo, "SGXMisc");
	for (i = 0; i < SGXMKIF_CMD_MAX; i++) {
		printf("\t\thost kick[%i]:\t%08x\n", i, bridgeinitinfo->aui32HostKickAddr[i]);
	}
	print_script(&bridgeinitinfo->sScripts);
	printf("\t\tclientbuildopt:\t%08x\n", bridgeinitinfo->ui32ClientBuildOptions);
	print_structsizes(&bridgeinitinfo->sSGXStructSizes);
#if defined(SGX_SUPPORT_HWPROFILING)
	printf("\t\tHW Prof:\t%p\n", bridgeinitinfo->hKernelHWProfilingMemInfo);
#endif
#if defined(SUPPORT_SGX_HWPERF)
	printf("\t\tHW Perf CB:\t%p\n", bridgeinitinfo->hKernelHWPerfCBMemInfo);
	dump_unregister_buffer(bridgeinitinfo->hKernelHWPerfCBMemInfo, "HWPerfCB");
#endif
	printf("\t\tTA sig buffer:\t%p\n", bridgeinitinfo->hKernelTASigBufferMemInfo);
	dump_unregister_buffer(bridgeinitinfo->hKernelTASigBufferMemInfo, "TASigBuffer");
	printf("\t\t3D sig buffer:\t%p\n", bridgeinitinfo->hKernel3DSigBufferMemInfo);
	dump_unregister_buffer(bridgeinitinfo->hKernel3DSigBufferMemInfo, "3DSigBuffer");
#if defined(FIX_HW_BRN_29702)
	printf("\t\tCFI:\t\t%p\n", bridgeinitinfo->hKernelCFIMemInfo);
#endif
#if defined(FIX_HW_BRN_29823)
	printf("\t\tDummy term:\t%p\n", bridgeinitinfo->hKernelDummyTermStreamMemInfo);
#endif
#if defined(FIX_HW_BRN_31542)
	printf("\t\tClip WAVDM:\t%p\n", bridgeinitinfo->hKernelClearClipWAVDMStreamMemInfo);
	printf("\t\tClip WA idx:\t%p\n", bridgeinitinfo->hKernelClearClipWAIndexStreamMemInfo);
	printf("\t\tClip WAPDS:\t%p\n", bridgeinitinfo->hKernelClearClipWAPDSMemInfo);
	printf("\t\tClip WAUSE:\t%p\n", bridgeinitinfo->hKernelClearClipWAUSEMemInfo);
	printf("\t\tClip WA param:\t%p\n", bridgeinitinfo->hKernelClearClipWAParamMemInfo);
	printf("\t\tClip WAPMPT:\t%p\n", bridgeinitinfo->hKernelClearClipWAPMPTMemInfo);
	printf("\t\tClip WATPC:\t%p\n", bridgeinitinfo->hKernelClearClipWATPCMemInfo);
	printf("\t\tClip WAPSGR:\t%p\n", bridgeinitinfo->hKernelClearClipWAPSGRgnHdrMemInfo);
#endif
#if defined(SGX_FEATURE_VDM_CONTEXT_SWITCH) && defined(FIX_HW_BRN_31425)
	printf("\t\tVDM snapshot:\t%p\n", bridgeinitinfo->hKernelVDMSnapShotBufferMemInfo);
	printf("\t\tVDM ctrl:\t%p\n", bridgeinitinfo->hKernelVDMCtrlStreamBufferMemInfo);
#endif
#if defined(SGX_FEATURE_VDM_CONTEXT_SWITCH) && \
	        defined(FIX_HW_BRN_33657) && defined(SUPPORT_SECURE_33657_FIX)
	printf("\t\tVDM state:\t%p\n", bridgeinitinfo->hKernelVDMStateUpdateBufferMemInfo);
#endif
#if defined(PVRSRV_USSE_EDM_STATUS_DEBUG)
	printf("\t\tEDM status:\t%p\n", bridgeinitinfo->hKernelEDMStatusBufferMemInfo);
#endif
#if defined(SGX_FEATURE_OVERLAPPED_SPM)
	printf("\t\ttmp rgn:\t%p\n", bridgeinitinfo->hKernelTmpRgnHeaderMemInfo);
#endif
	printf("\t\tEDM task reg0:\t%08x\n", bridgeinitinfo->ui32EDMTaskReg0);
	printf("\t\tEDM task reg1:\t%08x\n", bridgeinitinfo->ui32EDMTaskReg1);
	printf("\t\tclk gate ctl:\t%08x\n", bridgeinitinfo->ui32ClkGateCtl);
	printf("\t\tclk gate ctl2:\t%08x\n", bridgeinitinfo->ui32ClkGateCtl2);
	printf("\t\tclk statusreg:\t%08x\n", bridgeinitinfo->ui32ClkGateStatusReg);
	printf("\t\tclk statusmask:\t%08x\n", bridgeinitinfo->ui32ClkGateStatusMask);
	printf("\t\tcache control:\t%08x\n", bridgeinitinfo->ui32CacheControl);
	for (i = 0; i < SGX_MAX_DEV_DATA; i++) {
		printf("\t\tdevdata[%i]:\t%08x\n", i, bridgeinitinfo->asInitDevData[i]);
	}
	for (i = 0; i < SGX_MAX_INIT_MEM_HANDLES; i++) {
		printf("\t\tinit memhd[%i]:\t%p\n", i, bridgeinitinfo->asInitMemHandles[i]);
		if (bridgeinitinfo->asInitMemHandles[i]) {
			char buf_name[32];
			snprintf(buf_name, sizeof(buf_name), "InitMem_%d", i);
			dump_unregister_buffer(bridgeinitinfo->asInitMemHandles[i], buf_name);
		}
	}
}

void pvrsrv_bridge_sgx_devinitpart2_pre(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	PVRSRV_BRIDGE_IN_SGXDEVINITPART2 *in = param->pvParamIn;

	printf("\t\tdev cookie:\t%p\n", in->hDevCookie);
	print_bridgeinitinfo(&in->sInitInfo);
}

void pvrsrv_bridge_sgx_devinitpart2_post(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	PVRSRV_BRIDGE_OUT_SGXDEVINITPART2 *out = param->pvParamOut;

	print_error(out->eError);

	if (out->eError == PVRSRV_OK) {
		printf("\t\tKM build opts:\t%08x\n", out->ui32KMBuildOptions);
	}
}

static void print_sgxfeatures(PVRSRV_SGX_MISCINFO_FEATURES *features)
{
	printf("\t\tcore rev:\t%08x\n", features->ui32CoreRev);
	printf("\t\tcore id:\t%08x\n", features->ui32CoreID);
	printf("\t\tddk version:\t%08x\n", features->ui32DDKVersion);
	printf("\t\tddk build:\t%08x\n", features->ui32DDKBuild);
	printf("\t\tcore id sw:\t%08x\n", features->ui32CoreIdSW);
	printf("\t\tcore rev sw:\t%08x\n", features->ui32CoreRevSW);
	printf("\t\tbuild opts:\t%08x\n", features->ui32BuildOptions);
#if defined(SUPPORT_SGX_EDM_MEMORY_DEBUG)
	printf("\t\tdev mem val:\t%08x\n", features->ui32DeviceMemValue);
#endif
#if defined(PVRSRV_USSE_EDM_STATUS_DEBUG)
	printf("\t\tvaedm status:\t%08x\n", features->sDevVAEDMStatusBuffer.uiAddr);
	printf("\t\tedm status:\t%p\n", features->pvEDMStatusBuffer);
#endif
}
static void print_activepower(PVRSRV_SGX_MISCINFO_ACTIVEPOWER *activepower)
{
	printf("\t\tactive power:\t%08x\n", activepower->ui32NumActivePowerEvents);
}
static void print_lockups(PVRSRV_SGX_MISCINFO_LOCKUPS *lockups)
{
	printf("\t\thost lockups:\t%08x\n", lockups->ui32HostDetectedLockups);
	printf("\t\tukernel lockups:\t%08x\n", lockups->ui32uKernelDetectedLockups);
}
static void print_spm(PVRSRV_SGX_MISCINFO_SPM *spm)
{
	printf("\t\trt data set:\t%p\n", spm->hRTDataSet);
	printf("\t\tout of mem:\t%08x\n", spm->ui32NumOutOfMemSignals);
	printf("\t\tspm renders:\t%08x\n", spm->ui32NumSPMRenders);
}
#if defined(SGX_FEATURE_DATA_BREAKPOINTS)
static void print_sgxbreakpointinfo(SGX_BREAKPOINT_INFO *breakpoint)
{
	printf("\t\tbp enable:\t%s\n", breakpoint->bBPEnable ? "true" : "false");
	printf("\t\tbp index:\t%08x\n", breakpoint->ui32BPIndex);
	printf("\t\tdata master:\t%08x\n", breakpoint->ui32DataMasterMask);
	printf("\t\tbp vaddr:\t%08x\n", breakpoint->sBPDevVAddr.uiAddr);
	printf("\t\tbp vaddr end:\t%08x\n", breakpoint->sBPDevVAddrEnd.uiAddr);
	printf("\t\ttrapped:\t%s\n", breakpoint->bTrapped ? "true" : "false");
	printf("\t\tread:\t%s\n", breakpoint->bRead ? "true" : "false");
	printf("\t\twrite:\t%s\n", breakpoint->bWrite ? "true" : "false");
	printf("\t\ttrapped bp:\t%s\n", breakpoint->bTrappedBP ? "true" : "false");
	printf("\t\tcore num:\t%08x\n", breakpoint->ui32CoreNum);
	printf("\t\ttrapped vaddr:\t%08x\n", breakpoint->sTrappedBPDevVAddr.uiAddr);
	printf("\t\ttrapped len:\t%08x\n", breakpoint->ui32TrappedBPBurstLength);
	printf("\t\ttrapped read:\t%s\n", breakpoint->bTrappedBPRead ? "true" : "false");
	printf("\t\ttrapped data:\t%08x\n", breakpoint->ui32TrappedBPDataMaster);
	printf("\t\ttrapped tag:\t%08x\n", breakpoint->ui32TrappedBPTag);
}
#endif
static void print_sethwperfstatus(PVRSRV_SGX_MISCINFO_SET_HWPERF_STATUS *perfstatus)
{
	printf("\t\tperf status:\t%08x\n", perfstatus->ui32NewHWPerfStatus);
#if defined(SGX_FEATURE_EXTENDED_PERF_COUNTERS)
	int i;
	for (i = 0; i < PVRSRV_SGX_HWPERF_NUM_COUNTERS; i++) {
		printf("\t\tperf group[%d]:\t%08x\n", i, perfstatus->aui32PerfGroup[i]);
	}
	for (i = 0; i < PVRSRV_SGX_HWPERF_NUM_COUNTERS; i++) {
		printf("\t\tperf bit[%d]:\t%08x\n", i, perfstatus->aui32PerfBit[i]);
	}
#else
	printf("\t\tperf group:\t%08x\n", perfstatus->ui32PerfGroup);
#endif
}
static void print_miscrequest(SGX_MISC_INFO *miscinfo)
{
	switch (miscinfo->eRequest) {
		case SGX_MISC_INFO_REQUEST_SGXREV:
		case SGX_MISC_INFO_REQUEST_DRIVER_SGXREV:
			print_sgxfeatures(&miscinfo->uData.sSGXFeatures);
			break;
		case SGX_MISC_INFO_REQUEST_CLOCKSPEED:
			printf("\t\tclock speed:\t%08x\n", miscinfo->uData.ui32SGXClockSpeed);
			break;
		case SGX_MISC_INFO_REQUEST_ACTIVEPOWER:
			print_activepower(&miscinfo->uData.sActivePower);
			break;
		case SGX_MISC_INFO_REQUEST_LOCKUPS:
			print_lockups(&miscinfo->uData.sLockups);
			break;
		case SGX_MISC_INFO_REQUEST_SPM:
			print_spm(&miscinfo->uData.sSPM);
			break;
#if defined(SGX_FEATURE_DATA_BREAKPOINTS)
		case SGX_MISC_INFO_REQUEST_SET_BREAKPOINT:
		case SGX_MISC_INFO_REQUEST_POLL_BREAKPOINT:
		case SGX_MISC_INFO_REQUEST_RESUME_BREAKPOINT:
			print_sgxbreakpointinfo(&miscinfo->uData.sSGXBreakpointInfo);
			break;
#endif
		case SGX_MISC_INFO_REQUEST_SET_HWPERF_STATUS:
			print_sethwperfstatus(&miscinfo->uData.sSetHWPerfStatus);
			break;
		default:
			break;
	}
}
static const char *sgxmiscinitnames[] = {
		ENUM_INFO(SGX_INIT_OP_ILLEGAL),
		ENUM_INFO(SGX_MISC_INFO_REQUEST_CLOCKSPEED),
		ENUM_INFO(SGX_MISC_INFO_REQUEST_SGXREV),
		ENUM_INFO(SGX_MISC_INFO_REQUEST_DRIVER_SGXREV),
#if defined(SUPPORT_SGX_EDM_MEMORY_DEBUG)
		ENUM_INFO(SGX_MISC_INFO_REQUEST_MEMREAD),
		ENUM_INFO(SGX_MISC_INFO_REQUEST_MEMCOPY),
#endif
		ENUM_INFO(SGX_MISC_INFO_REQUEST_SET_HWPERF_STATUS),
#if defined(SGX_FEATURE_DATA_BREAKPOINTS)
		ENUM_INFO(SGX_MISC_INFO_REQUEST_SET_BREAKPOINT),
		ENUM_INFO(SGX_MISC_INFO_REQUEST_POLL_BREAKPOINT),
		ENUM_INFO(SGX_MISC_INFO_REQUEST_RESUME_BREAKPOINT),
#endif
		ENUM_INFO(SGX_MISC_INFO_DUMP_DEBUG_INFO),
		ENUM_INFO(SGX_MISC_INFO_PANIC),
		ENUM_INFO(SGX_MISC_INFO_REQUEST_SPM),
		ENUM_INFO(SGX_MISC_INFO_REQUEST_ACTIVEPOWER),
		ENUM_INFO(SGX_MISC_INFO_REQUEST_LOCKUPS)
};
static void print_sgxmiscinfo(SGX_MISC_INFO *miscinfo)
{
	const char *miscname;

	if (!miscinfo) {
		return;
	}

	miscname =
	    (miscinfo->eRequest < ARRAY_SIZE(sgxmiscinitnames)) ?
	    sgxmiscinitnames[miscinfo->eRequest] : NULL;
	printf("\t\trequest:\t%08x (%s)\n", miscinfo->eRequest,
		miscname ? miscname : "unknown");
	printf("\t\tpadding:\t%08x\n", miscinfo->ui32Padding);
#if defined(SUPPORT_SGX_EDM_MEMORY_DEBUG)
	printf("\t\tvaddr src:\t%08x\n", miscinfo->sDevVAddrSrc.uiAddr);
	printf("\t\tvaddr dest:\t%08x\n", miscinfo->sDevVAddrDest.uiAddr);
	printf("\t\tmemctxt:\t%p\n", miscinfo->hDevMemContext);
#endif
	print_miscrequest(miscinfo);
}
void pvrsrv_bridge_sgx_getmiscinfo_pre(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	PVRSRV_BRIDGE_IN_SGXGETMISCINFO *in = param->pvParamIn;

	printf("\t\tdev cookie:\t%p\n", in->hDevCookie);
	print_sgxmiscinfo(in->psMiscInfo);
}

void pvrsrv_bridge_sgx_getmiscinfo_post(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	PVRSRV_BRIDGE_RETURN *out = param->pvParamOut;
	PVRSRV_BRIDGE_IN_SGXGETMISCINFO *in = param->pvParamIn;

	print_error(out->eError);
	print_sgxmiscinfo(in->psMiscInfo);
}

void pvrsrv_bridge_sgx_getclientinfo_pre(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	PVRSRV_BRIDGE_IN_GETCLIENTINFO *in = param->pvParamIn;

	printf("\t\tdev cookie:\t%p\n", in->hDevCookie);
}

static void print_clientinfo(SGX_CLIENT_INFO *clientinfo)
{
	int i;

	if (!clientinfo) {
		return;
	}

	printf("\t\tprocess id:\t%08x\n", clientinfo->ui32ProcessID);
	printf("\t\tprocess:\t%p\n", clientinfo->pvProcess);
	print_miscinfo(&clientinfo->sMiscInfo);
	for (i = 0; i < SGX_MAX_DEV_DATA; i++) {
		printf("\t\tdevdata[%d]:\t%08x\n", i, clientinfo->asDevData[i]);
	}
}

void pvrsrv_bridge_sgx_getclientinfo_post(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	PVRSRV_BRIDGE_OUT_GETCLIENTINFO *out = param->pvParamOut;

	print_error(out->eError);
	print_clientinfo(&out->sClientInfo);
}

void pvrsrv_bridge_sgx_getinternaldevinfo_pre(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	PVRSRV_BRIDGE_IN_GETINTERNALDEVINFO *in = param->pvParamIn;

	printf("\t\tdev cookie:\t%p\n", in->hDevCookie);
}

static void print_internaldevinfo(SGX_INTERNAL_DEVINFO *devinfo)
{
	if (!devinfo) {
		return;
	}

	printf("\t\tflags:\t%08x\n", devinfo->ui32Flags);
	printf("\t\thostctlkern:\t%p\n", devinfo->hHostCtlKernelMemInfoHandle);
	printf("\t\tforce ptoff:\t%s\n", devinfo->bForcePTOff ? "true" : "false");
}

void pvrsrv_bridge_sgx_getinternaldevinfo_post(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	PVRSRV_BRIDGE_OUT_GETINTERNALDEVINFO *out = param->pvParamOut;

	print_error(out->eError);
	print_internaldevinfo(&out->sSGXInternalDevInfo);
}

void pvrsrv_bridge_sgx_register_hw_render_context_pre(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	PVRSRV_BRIDGE_IN_SGX_REGISTER_HW_RENDER_CONTEXT *in = param->pvParamIn;

	printf("\t\tdevice cookie:\t%p\n", in->hDevCookie);
	printf("\t\thwrender ctxt:\t%08x\n", in->sHWRenderContextDevVAddr.uiAddr);
}

void pvrsrv_bridge_sgx_register_hw_render_context_post(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	PVRSRV_BRIDGE_OUT_SGX_REGISTER_HW_RENDER_CONTEXT *out = param->pvParamOut;

	print_error(out->eError);
	printf("\t\thwrender ctxt:\t%p\n", out->hHWRenderContext);
}

void pvrsrv_bridge_sgx_register_hw_transfer_context_pre(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	PVRSRV_BRIDGE_IN_SGX_REGISTER_HW_TRANSFER_CONTEXT *in = param->pvParamIn;

	printf("\t\tdevice cookie:\t%p\n", in->hDevCookie);
	printf("\t\thwtransferctxt:\t%08x\n", in->sHWTransferContextDevVAddr.uiAddr);
}

void pvrsrv_bridge_sgx_register_hw_transfer_context_post(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	PVRSRV_BRIDGE_OUT_SGX_REGISTER_HW_TRANSFER_CONTEXT *out = param->pvParamOut;

	print_error(out->eError);
	printf("\t\thwtransferctxt:\t%p\n", out->hHWTransferContext);
}

static const char *devclassnames[] = {
		ENUM_INFO(PVRSRV_DEVICE_CLASS_3D),
		ENUM_INFO(PVRSRV_DEVICE_CLASS_DISPLAY),
		ENUM_INFO(PVRSRV_DEVICE_CLASS_BUFFER),
		ENUM_INFO(PVRSRV_DEVICE_CLASS_VIDEO)
};
void pvrsrv_bridge_enum_class_pre(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	PVRSRV_BRIDGE_IN_ENUMCLASS *in = param->pvParamIn;
	const char *classname;

	classname =
		(in->sDeviceClass < ARRAY_SIZE(devclassnames)) ?
		devclassnames[in->sDeviceClass] : NULL;
	printf("\t\tdevice class:\t%08x (%s)\n", in->sDeviceClass,
		classname ? classname : "unknown");
}

void pvrsrv_bridge_enum_class_post(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	PVRSRV_BRIDGE_OUT_ENUMCLASS *out = param->pvParamOut;
	int i;

	print_error(out->eError);
	printf("\t\tnum devices:\t%08x\n", out->ui32NumDevices);
	printf("\t\tmax devices:\t%08x\n", PVRSRV_MAX_DEVICES);
	for (i = 0; i < out->ui32NumDevices; i++) {
		printf("\t\tdevid[%d]:\t%08x\n", i, out->ui32DevID[i]);
	}
}

void pvrsrv_bridge_sgx_flush_hw_render_target_pre(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	PVRSRV_BRIDGE_IN_SGX_FLUSH_HW_RENDER_TARGET *in = param->pvParamIn;

	printf("\t\tdev cookie:\t%p\n", in->hDevCookie);
	printf("\t\thw rt dataset:\t%08x\n", in->sHWRTDataSetDevVAddr.uiAddr);
}

void pvrsrv_bridge_sgx_flush_hw_render_target_post(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	PVRSRV_BRIDGE_RETURN *out = param->pvParamOut;

	print_error(out->eError);
}

void pvrsrv_bridge_sgx_unregister_hw_transfer_context_pre(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	PVRSRV_BRIDGE_IN_SGX_UNREGISTER_HW_TRANSFER_CONTEXT *in = param->pvParamIn;

	printf("\t\tforce clean:\t%s\n", in->bForceCleanup ? "true" : "false");
	printf("\t\tdev cookie:\t%p\n", in->hDevCookie);
	printf("\t\thwtransferctxt:\t%p\n", in->hHWTransferContext);
}

void pvrsrv_bridge_sgx_unregister_hw_transfer_context_post(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	PVRSRV_BRIDGE_RETURN *out = param->pvParamOut;

	print_error(out->eError);
}

void pvrsrv_bridge_sgx_unregister_hw_render_context_pre(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	PVRSRV_BRIDGE_IN_SGX_UNREGISTER_HW_RENDER_CONTEXT *in = param->pvParamIn;

	printf("\t\tforce clean:\t%s\n", in->bForceCleanup ? "true" : "false");
	printf("\t\tdev cookie:\t%p\n", in->hDevCookie);
	printf("\t\thwrender ctxt:\t%p\n", in->hHWRenderContext);
}

void pvrsrv_bridge_sgx_unregister_hw_render_context_post(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	PVRSRV_BRIDGE_RETURN *out = param->pvParamOut;

	print_error(out->eError);
}

void pvrsrv_bridge_sgx_releaseclientinfo_pre(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	PVRSRV_BRIDGE_IN_RELEASECLIENTINFO *in = param->pvParamIn;

	printf("\t\tdev cookie:\t%p\n", in->hDevCookie);
	print_clientinfo(&in->sClientInfo);
}

void pvrsrv_bridge_sgx_releaseclientinfo_post(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	PVRSRV_BRIDGE_RETURN *out = param->pvParamOut;

	print_error(out->eError);
	//printf("\t\tdata:\t%p (unused)\n", out->pvData);
}

void print_transfersgxkick(PVRSRV_TRANSFER_SGX_KICK *transfersgxkick)
{
	int i;
	char name[32];
	if (!transfersgxkick) {
		return;
	}

	printf("\t\tccb meminfo:\t%p\n", transfersgxkick->hCCBMemInfo);
	printf("\t\tcmd ccb off:\t%08x\n", transfersgxkick->ui32SharedCmdCCBOffset);
	printf("\t\thwtransferctxt:\t%08x\n", transfersgxkick->sHWTransferContextDevVAddr.uiAddr);
	printf("\t\tta syncinfo:\t%p\n", transfersgxkick->hTASyncInfo);
	printf("\t\t3d syncinfo:\t%p\n", transfersgxkick->h3DSyncInfo);
	if (transfersgxkick->h3DSyncInfo) {
		snprintf(name, sizeof(name), "%p-3DSyncInfo", transfersgxkick->h3DSyncInfo);
		dump_unregister_buffer((IMG_HANDLE)transfersgxkick->h3DSyncInfo, name);
	}
	printf("\t\tnum src sync:\t%08x\n", transfersgxkick->ui32NumSrcSync);
	printf("\t\tmax src sync:\t%08x\n", SGX_MAX_TRANSFER_SYNC_OPS);
	for (i = 0; i < transfersgxkick->ui32NumSrcSync; i++) {
		printf("\t\tsrc sync[%d]:\t%p\n", i, transfersgxkick->ahSrcSyncInfo[i]);
	}
	printf("\t\tnum dst sync:\t%08x\n", transfersgxkick->ui32NumDstSync);
	for (i = 0; i < transfersgxkick->ui32NumDstSync; i++) {
		printf("\t\tdst sync[%d]:\t%p\n", i, transfersgxkick->ahDstSyncInfo[i]);
		if (transfersgxkick->ahDstSyncInfo[i]) {
			snprintf(name, sizeof(name), "%p-DstSyncInfo%d", transfersgxkick->ahDstSyncInfo[i], i);
			dump_unregister_buffer((IMG_HANDLE)transfersgxkick->ahDstSyncInfo[i], name);
		}
	}
	printf("\t\tflags:\t\t%08x\n", transfersgxkick->ui32Flags);
	printf("\t\tpdump flags:\t%08x\n", transfersgxkick->ui32PDumpFlags);
#if defined(PDUMP)
	printf("\t\tccb dump woff:\t%08x\n", transfersgxkick->ui32CCBDumpWOff);
#endif
	printf("\t\tdevmem ctxt:\t%p\n", transfersgxkick->hDevMemContext);
}

void pvrsrv_bridge_sgx_submittransfer_pre(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	PVRSRV_BRIDGE_IN_SUBMITTRANSFER *in = param->pvParamIn;

	printf("\t\tdevice cookie:\t%p\n", in->hDevCookie);
	print_transfersgxkick(&in->sKick);
}

void pvrsrv_bridge_sgx_submittransfer_post(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	PVRSRV_BRIDGE_RETURN *out = param->pvParamOut;

	print_error(out->eError);
}

void print_command(SGXMKIF_COMMAND *command)
{
	int i;
	if (!command) {
		return;
	}

	printf("\t\tservice addr:\t%08x\n", command->ui32ServiceAddress);
	printf("\t\tcache ctrl:\t%08x\n", command->ui32CacheControl);
	for (i = 0; i < 6; i++) {
		printf("\t\tdata[%d]:\t%08x\n", i, command->ui32Data[i]);
	}
}

void print_ctlstatus(CTL_STATUS *ctlstatus)
{
	if (!ctlstatus) {
		return;
	}

	printf("\t\tstatus addr:\t%08x\n", ctlstatus->sStatusDevAddr.uiAddr);
	printf("\t\tstatus val:\t%08x\n", ctlstatus->ui32StatusValue);
}

void print_internalstatusupdate(SGX_INTERNEL_STATUS_UPDATE *statusupdate)
{
	char name[32];
	if (!statusupdate) {
		return;
	}

	print_ctlstatus(&statusupdate->sCtlStatus);
	printf("\t\tkern meminfo:\t%p\n", statusupdate->hKernelMemInfo);
	if (statusupdate->hKernelMemInfo) {
		snprintf(name, sizeof(name), "%p-kernel", statusupdate->hKernelMemInfo);
		dump_unregister_buffer((IMG_HANDLE)statusupdate->hKernelMemInfo, name);
	}
}

void print_ccbkick(SGX_CCB_KICK *ccbkick)
{
	int i;
	char name[32];
	if (!ccbkick) {
		return;
	}

	print_command(&ccbkick->sCommand);
	printf("\t\tccb kmeminfo:\t%p\n", ccbkick->hCCBKernelMemInfo);
	snprintf(name, sizeof(name), "%p-CCB", ccbkick->hCCBKernelMemInfo);
	dump_unregister_buffer((IMG_HANDLE)ccbkick->hCCBKernelMemInfo, name);
	printf("\t\tnum dst sync:\t%08x\n", ccbkick->ui32NumDstSyncObjects);
	printf("\t\tkhwsync list:\t%p\n", ccbkick->hKernelHWSyncListMemInfo);
	snprintf(name, sizeof(name), "%p-HWSyncList", ccbkick->hKernelHWSyncListMemInfo);
	dump_unregister_buffer((IMG_HANDLE)ccbkick->hKernelHWSyncListMemInfo, name);
	printf("\t\tdst sync:\t%p (pointer)\n", ccbkick->pahDstSyncHandles);
	for (i = 0; i < ccbkick->ui32NumDstSyncObjects; i++) {
	        printf("\t\tdst sync[%d]:\t%p\n", i, ccbkick->pahDstSyncHandles[i]);
		snprintf(name, sizeof(name), "%p-DstSync%d", ccbkick->pahDstSyncHandles[i], i);
		dump_unregister_buffer((IMG_HANDLE)ccbkick->pahDstSyncHandles[i], name);
	}
	printf("\t\tnum ta status:\t%08x\n", ccbkick->ui32NumTAStatusVals);
	printf("\t\tnum 3d status:\t%08x\n", ccbkick->ui32Num3DStatusVals);
#if defined(SUPPORT_SGX_NEW_STATUS_VALS)
	for (i = 0; i < ccbkick->ui32NumTAStatusVals; i++) {
	        printf("\t\tta status[%d]:\n", i);
		print_internalstatusupdate(&ccbkick->asTAStatusUpdate[i]);
	}
	for (i = 0; i < ccbkick->ui32NumTAStatusVals; i++) {
	        printf("\t\t3d status[%d]:\n", i);
		print_internalstatusupdate(&ccbkick->as3DStatusUpdate[i]);
	}
#else
	for (i = 0; i < ccbkick->ui32NumTAStatusVals; i++) {
	        printf("\t\tta status[%d]:\t%p\n", i, ccbkick->ahTAStatusSyncInfo[i]);
	}
	for (i = 0; i < ccbkick->ui32NumTAStatusVals; i++) {
	        printf("\t\t3d status[%d]:\t%p\n", i, ccbkick->ah3DStatusSyncInfo[i]);
	}
#endif
	printf("\t\tfirst kick:\t%s\n", ccbkick->bFirstKickOrResume ? "true" : "false");
#if (defined(NO_HARDWARE) || defined(PDUMP))
	printf("\t\tterminate:\t%s\n", ccbkick->bTerminateOrAbort ? "true" : "false");
#endif
	printf("\t\tlast scene:\t%s\n", ccbkick->bLastInScene ? "true" : "false");
	printf("\t\tccb offset:\t%08x\n", ccbkick->ui32CCBOffset);
#if defined(SUPPORT_SGX_GENERALISED_SYNCOBJECTS)
	printf("\t\tnum ta src:\t%08x\n", ccbkick->ui32NumTASrcSyncs);
	for (i = 0; i < ccbkick->ui32NumTASrcSyncs; i++) {
		printf("\t\tta src[%d]:\t%p\n", i, ccbkick->ahTASrcKernelSyncInfo[i]);
	}
	printf("\t\tnum ta dst:\t%08x\n", ccbkick->ui32NumTADstSyncs);
	for (i = 0; i < ccbkick->ui32NumTADstSyncs; i++) {
		printf("\t\tta dst[%d]:\t%p\n", i, ccbkick->ahTADstKernelSyncInfo[i]);
	}
	printf("\t\tnum 3d src:\t%08x\n", ccbkick->ui32Num3DSrcSyncs);
	for (i = 0; i < ccbkick->ui32Num3DSrcSyncs; i++) {
		printf("\t\t3d src[%d]:\t%p\n", i, ccbkick->ah3DSrcKernelSyncInfo[i]);
	}
#else
	printf("\t\tnum src:\t%08x\n", ccbkick->ui32NumSrcSyncs);
	for (i = 0; i < ccbkick->ui32NumSrcSyncs; i++) {
		printf("\t\tsrc[%d]:\t%p\n", i, ccbkick->ahSrcKernelSyncInfo[i]);
	}
#endif
	printf("\t\tta dep:\t\t%s\n", ccbkick->bTADependency ? "true" : "false");
	printf("\t\tta 3d sync:\t%p\n", ccbkick->hTA3DSyncInfo);
	printf("\t\tta sync:\t%p\n", ccbkick->hTASyncInfo);
	printf("\t\t3d sync:\t%p\n", ccbkick->h3DSyncInfo);
	if (ccbkick->h3DSyncInfo) {
		snprintf(name, sizeof(name), "%p-3DSyncInfo", ccbkick->h3DSyncInfo);
		dump_unregister_buffer((IMG_HANDLE)ccbkick->h3DSyncInfo, name);
	}
#if defined(PDUMP)
	printf("\t\tccb dump:\t%08x\n", ccbkick->ui32CCBDumpWOff);
#endif
#if defined(NO_HARDWARE)
	printf("\t\twrite ops:\t%08x\n", ccbkick->ui32WriteOpsPendingVal);
#endif
	printf("\t\tdevmem ctxt:\t%p\n", ccbkick->hDevMemContext);
}

void pvrsrv_bridge_sgx_dokick_pre(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	PVRSRV_BRIDGE_IN_DOKICK *in = param->pvParamIn;

	printf("\t\tdevice cookie:\t%p\n", in->hDevCookie);
	print_ccbkick(&in->sCCBKick);
}

void pvrsrv_bridge_sgx_dokick_post(int fd,
		PVRSRV_BRIDGE_PACKAGE *param)
{
	PVRSRV_BRIDGE_RETURN *out = param->pvParamOut;

	print_error(out->eError);
}
