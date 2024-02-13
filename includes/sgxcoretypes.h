
#ifndef _SGXCORETYPES_H_
#define _SGXCORETYPES_H_

typedef enum {
    SGX_CORE_ID_INVALID,
    SGX_CORE_ID_520,
    SGX_CORE_ID_530,
    SGX_CORE_ID_531,
    SGX_CORE_ID_535,
    SGX_CORE_ID_540,
    SGX_CORE_ID_541,
    SGX_CORE_ID_543,
    SGX_CORE_ID_544,
    SGX_CORE_ID_545,
    SGX_CORE_ID_554
} SGX_CORE_ID_TYPE;

typedef struct _SGX_CORE_FEATURES {
    IMG_UINT32 ui32Flags;
    IMG_UINT32 ui32Flags2;
    IMG_UINT32 ui32Flags3;
    IMG_UINT32 ui32NumMutexes;
    IMG_UINT32 ui32NumMonitors;
    IMG_UINT32 ui32TextureStateSize;
    IMG_UINT32 ui32IterationStateSize;
    IMG_UINT32 ui32NumUSEPipes;
    IMG_UINT32 ui32NumProgramCounterBits;
    IMG_UINT32 ui32NumInternalRegisters;
    const IMG_UINT32 *puInvalidSpecialRegistersForNonBitwiseCount;
    const IMG_UINT32 *auInvalidSpecialRegistersForNonBitwise;
    const IMG_CHAR *pszCoreName;
} SGX_CORE_FEATURES;

typedef struct _SGX_CORE_BUGS {
    IMG_UINT32 ui32Flags;
    IMG_UINT32 ui32Flags2;
} SGX_CORE_BUGS;

typedef struct _SGX_CORE_DESC {
    SGX_CORE_ID_TYPE eCoreType;
    IMG_UINT32 ui32CoreRevision;
    SGX_CORE_BUGS sBugs;
    const SGX_CORE_FEATURES *psFeatures;
} SGX_CORE_DESC;

typedef struct _SGX_CORE_INFO {
    SGX_CORE_ID_TYPE eID;
    IMG_UINT32 uiRev;
} SGX_CORE_INFO, *PSGX_CORE_INFO;

typedef const SGX_CORE_DESC * PCSGX_CORE_DESC;

#endif /* _SGXCORETYPES_H_ */

