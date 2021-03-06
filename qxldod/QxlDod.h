/*
 * Copyright 2013-2016 Red Hat, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 *
 * You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#pragma once
#include "baseobject.h"
#include "qxl_dev.h"
#include "qxl_windows.h"
#include "mspace.h"

#define MAX_CHILDREN               1
#define MAX_VIEWS                  1
#define BITS_PER_BYTE              8

#define POINTER_SIZE               64
#define MIN_WIDTH_SIZE             1024
#define MIN_HEIGHT_SIZE            768
#define QXL_BPP                    32
#define VGA_BPP                    24

#define QXL_NON_PAGED __declspec(code_seg(".text"))

extern BOOLEAN g_bSupportVSync;

typedef struct _QXL_FLAGS
{
    UINT DriverStarted           : 1; // ( 1) 1 after StartDevice and 0 after StopDevice
    UINT Unused                  : 31;
} QXL_FLAGS;

// For the following macros, c must be a UCHAR.
#define UPPER_6_BITS(c)   (((c) & rMaskTable[6 - 1]) >> 2)
#define UPPER_5_BITS(c)   (((c) & rMaskTable[5 - 1]) >> 3)
#define LOWER_6_BITS(c)   (((BYTE)(c)) & lMaskTable[BITS_PER_BYTE - 6])
#define LOWER_5_BITS(c)   (((BYTE)(c)) & lMaskTable[BITS_PER_BYTE - 5])


#define SHIFT_FOR_UPPER_5_IN_565   (6 + 5)
#define SHIFT_FOR_MIDDLE_6_IN_565  (5)
#define SHIFT_UPPER_5_IN_565_BACK  ((BITS_PER_BYTE * 2) + (BITS_PER_BYTE - 5))
#define SHIFT_MIDDLE_6_IN_565_BACK ((BITS_PER_BYTE * 1) + (BITS_PER_BYTE - 6))
#define SHIFT_LOWER_5_IN_565_BACK  ((BITS_PER_BYTE * 0) + (BITS_PER_BYTE - 5))

#pragma pack(push)
#pragma pack(1)

typedef struct
{
    CHAR Signature[4];
    USHORT Version;
    ULONG OemStringPtr;
    LONG Capabilities;
    ULONG VideoModePtr;
    USHORT TotalMemory;
    USHORT OemSoftwareRevision;
    ULONG OemVendorNamePtr;
    ULONG OemProductNamePtr;
    ULONG OemProductRevPtr;
    CHAR Reserved[222];
} VBE_INFO, *PVBE_INFO;

typedef struct
{
/* Mandatory information for all VBE revisions */
    USHORT ModeAttributes;
    UCHAR WinAAttributes;
    UCHAR WinBAttributes;
    USHORT WinGranularity;
    USHORT WinSize;
    USHORT WinASegment;
    USHORT WinBSegment;
    ULONG WinFuncPtr;
    USHORT BytesPerScanLine;
/* Mandatory information for VBE 1.2 and above */
    USHORT XResolution;
    USHORT YResolution;
    UCHAR XCharSize;
    UCHAR YCharSize;
    UCHAR NumberOfPlanes;
    UCHAR BitsPerPixel;
    UCHAR NumberOfBanks;
    UCHAR MemoryModel;
    UCHAR BankSize;
    UCHAR NumberOfImagePages;
    UCHAR Reserved1;
/* Direct Color fields (required for Direct/6 and YUV/7 memory models) */
    UCHAR RedMaskSize;
    UCHAR RedFieldPosition;
    UCHAR GreenMaskSize;
    UCHAR GreenFieldPosition;
    UCHAR BlueMaskSize;
    UCHAR BlueFieldPosition;
    UCHAR ReservedMaskSize;
    UCHAR ReservedFieldPosition;
    UCHAR DirectColorModeInfo;
/* Mandatory information for VBE 2.0 and above */
    ULONG PhysBasePtr;
    ULONG Reserved2;
    USHORT Reserved3;
    /* Mandatory information for VBE 3.0 and above */
    USHORT LinBytesPerScanLine;
    UCHAR BnkNumberOfImagePages;
    UCHAR LinNumberOfImagePages;
    UCHAR LinRedMaskSize;
    UCHAR LinRedFieldPosition;
    UCHAR LinGreenMaskSize;
    UCHAR LinGreenFieldPosition;
    UCHAR LinBlueMaskSize;
    UCHAR LinBlueFieldPosition;
    UCHAR LinReservedMaskSize;
    UCHAR LinReservedFieldPosition;
    ULONG MaxPixelClock;
    CHAR Reserved4[189];
} VBE_MODEINFO, *PVBE_MODEINFO;

#pragma pack(pop)

typedef struct _X86BIOS_REGISTERS    // invented names
{
    ULONG Eax;
    ULONG Ecx;
    ULONG Edx;
    ULONG Ebx;
    ULONG Ebp;
    ULONG Esi;
    ULONG Edi;
    USHORT SegDs;
    USHORT SegEs;
} X86BIOS_REGISTERS, *PX86BIOS_REGISTERS;

/*  Undocumented imports from the HAL  */

#ifdef __cplusplus
extern "C" {
#endif

NTHALAPI BOOLEAN x86BiosCall (ULONG, PX86BIOS_REGISTERS);

NTHALAPI NTSTATUS x86BiosAllocateBuffer (ULONG *, USHORT *, USHORT *);
NTHALAPI NTSTATUS x86BiosFreeBuffer (USHORT, USHORT);

NTHALAPI NTSTATUS x86BiosReadMemory (USHORT, USHORT, PVOID, ULONG);
NTHALAPI NTSTATUS x86BiosWriteMemory (USHORT, USHORT, PVOID, ULONG);

#ifdef __cplusplus
}
#endif

struct DoPresentMemory
{
    PVOID                     DstAddr;
    UINT                      DstStride;
    ULONG                     DstBitPerPixel;
    UINT                      SrcWidth;
    UINT                      SrcHeight;
    BYTE*                     SrcAddr;
    LONG                      SrcPitch;
    ULONG                     NumMoves;             // in:  Number of screen to screen moves
    D3DKMT_MOVE_RECT*         Moves;               // in:  Point to the list of moves
    ULONG                     NumDirtyRects;        // in:  Number of direct rects
    RECT*                     DirtyRect;           // in:  Point to the list of dirty rects
    D3DKMDT_VIDPN_PRESENT_PATH_ROTATION Rotation;
    D3DDDI_VIDEO_PRESENT_SOURCE_ID  SourceID;
    HANDLE                    hAdapter;
    PMDL                      Mdl;
    PVOID                     DisplaySource;
};

typedef struct _BLT_INFO
{
    PVOID pBits;
    UINT Pitch;
    UINT BitsPerPel;
    POINT Offset; // To unrotated top-left of dirty rects
    D3DKMDT_VIDPN_PRESENT_PATH_ROTATION Rotation;
    UINT Width; // For the unrotated image
    UINT Height; // For the unrotated image
} BLT_INFO;

// Represents the current mode, may not always be set (i.e. frame buffer mapped) if representing the mode passed in on single mode setups.
typedef struct _CURRENT_BDD_MODE
{
    // The source mode currently set for HW Framebuffer
    // For sample driver this info filled in StartDevice by the OS and never changed.
    DXGK_DISPLAY_INFORMATION             DispInfo;

    // The rotation of the current mode. Rotation is performed in software during Present call
    D3DKMDT_VIDPN_PRESENT_PATH_ROTATION  Rotation;

    D3DKMDT_VIDPN_PRESENT_PATH_SCALING Scaling;
    // This mode might be different from one which are supported for HW frame buffer
    // Scaling/displasment might be needed (if supported)
    UINT SrcModeWidth;
    UINT SrcModeHeight;

    // Various boolean flags the struct uses
    struct _CURRENT_BDD_MODE_FLAGS
    {
        UINT SourceNotVisible     : 1; // 0 if source is visible
        UINT FullscreenPresent    : 1; // 0 if should use dirty rects for present
        UINT FrameBufferIsActive  : 1; // 0 if not currently active (i.e. target not connected to source)
        UINT DoNotMapOrUnmap      : 1; // 1 if the FrameBuffer should not be (un)mapped during normal execution
        UINT IsInternal           : 1; // 1 if it was determined (i.e. through ACPI) that an internal panel is being driven
        UINT Unused               : 27;
    } Flags;

    // The start and end of physical memory known to be all zeroes. Used to optimize the BlackOutScreen function to not write
    // zeroes to memory already known to be zero. (Physical address is located in DispInfo)
    PHYSICAL_ADDRESS ZeroedOutStart;
    PHYSICAL_ADDRESS ZeroedOutEnd;

    // Linear frame buffer pointer
    // A union with a ULONG64 is used here to ensure this struct looks the same on 32bit and 64bit builds
    // since the size of a VOID* changes depending on the build.
    union
    {
        VOID*                            Ptr;
        ULONG64                          Force8Bytes;
    } FrameBuffer;
} CURRENT_BDD_MODE;

#if DBG
class TimeMeasurement
{
public:
    TimeMeasurement()
    {
        Start();
    }
    void Start()
    {
        KeQuerySystemTime(&li1);
    }
    void Stop()
    {
        KeQuerySystemTime(&li2);
    }
    ULONG Diff()
    {
        return (ULONG)((li2.QuadPart - li1.QuadPart) / 10000);
    }
protected:
    LARGE_INTEGER li1;
    LARGE_INTEGER li2;
};
#else
class TimeMeasurement
{
public:
    TimeMeasurement() {}
    void Start() {}
    void Stop() {}
    ULONG Diff() { return 0; }
};
#endif

struct DelayedChunk
{
    LIST_ENTRY list;
    QXLDataChunk chunk;
};

class QxlDod;

class HwDeviceInterface {
public:
    virtual ~HwDeviceInterface() {;}
    virtual NTSTATUS QueryCurrentMode(PVIDEO_MODE RequestedMode) = 0;
    virtual NTSTATUS SetCurrentMode(ULONG Mode) = 0;
    virtual NTSTATUS GetCurrentMode(ULONG* Mode) = 0;
    virtual NTSTATUS SetPowerState(DEVICE_POWER_STATE DevicePowerState, DXGK_DISPLAY_INFORMATION* pDispInfo) = 0;
    virtual NTSTATUS HWInit(PCM_RESOURCE_LIST pResList, DXGK_DISPLAY_INFORMATION* pDispInfo) = 0;
    virtual NTSTATUS HWClose(void) = 0;
    QXL_NON_PAGED virtual BOOLEAN InterruptRoutine(_In_ PDXGKRNL_INTERFACE pDxgkInterface, _In_  ULONG MessageNumber) = 0;
    QXL_NON_PAGED virtual VOID DpcRoutine(PVOID) = 0;
    QXL_NON_PAGED virtual VOID ResetDevice(void) = 0;
    QXL_NON_PAGED virtual VOID VSyncInterruptPostProcess(_In_ PDXGKRNL_INTERFACE) = 0;
    virtual NTSTATUS AcquireFrameBuffer(CURRENT_BDD_MODE* pCurrentBddMode) { return STATUS_SUCCESS; }
    virtual NTSTATUS ReleaseFrameBuffer(CURRENT_BDD_MODE* pCurrentBddMode) { return STATUS_SUCCESS; }

    ULONG GetModeCount(void) const {return m_ModeCount;}
    PVIDEO_MODE_INFORMATION GetModeInfo(UINT idx) {return &m_ModeInfo[idx];}
    USHORT GetModeNumber(USHORT idx) {return m_ModeNumbers[idx];}
    USHORT GetCurrentModeIndex(void) {return m_CurrentMode;}
    VOID SetCurrentModeIndex(USHORT idx) {m_CurrentMode = idx;}
    virtual NTSTATUS ExecutePresentDisplayOnly(_In_ BYTE*             DstAddr,
                                 _In_ UINT              DstBitPerPixel,
                                 _In_ BYTE*             SrcAddr,
                                 _In_ UINT              SrcBytesPerPixel,
                                 _In_ LONG              SrcPitch,
                                 _In_ ULONG             NumMoves,
                                 _In_ D3DKMT_MOVE_RECT* pMoves,
                                 _In_ ULONG             NumDirtyRects,
                                 _In_ RECT*             pDirtyRect,
                                 _In_ D3DKMDT_VIDPN_PRESENT_PATH_ROTATION Rotation,
                                 _In_ const CURRENT_BDD_MODE* pModeCur) = 0;

    virtual VOID BlackOutScreen(CURRENT_BDD_MODE* pCurrentBddMod) = 0;
    virtual NTSTATUS SetPointerShape(_In_ CONST DXGKARG_SETPOINTERSHAPE* pSetPointerShape) = 0;
    virtual NTSTATUS SetPointerPosition(_In_ CONST DXGKARG_SETPOINTERPOSITION* pSetPointerPosition) = 0;
    virtual NTSTATUS Escape(_In_ CONST DXGKARG_ESCAPE* pEscap) = 0;
    NTSTATUS AcquireDisplayInfo(DXGK_DISPLAY_INFORMATION& DispInfo);
    ULONG GetId(void) { return m_Id; }
    virtual BOOLEAN IsBIOSCompatible() { return TRUE; }
protected:
    virtual NTSTATUS GetModeList(DXGK_DISPLAY_INFORMATION* pDispInfo) = 0;
protected:
    QxlDod* m_pQxlDod;
    PVIDEO_MODE_INFORMATION m_ModeInfo;
    ULONG m_ModeCount;
    PUSHORT m_ModeNumbers;
    USHORT m_CurrentMode;
    ULONG  m_Id;
};

class VgaDevice  :
    public HwDeviceInterface
{
public:
    VgaDevice(_In_ QxlDod* pQxlDod);
    ~VgaDevice(void);
    NTSTATUS QueryCurrentMode(PVIDEO_MODE RequestedMode);
    NTSTATUS SetCurrentMode(ULONG Mode);
    NTSTATUS GetCurrentMode(ULONG* Mode);
    NTSTATUS SetPowerState(DEVICE_POWER_STATE DevicePowerState, DXGK_DISPLAY_INFORMATION* pDispInfo);
    NTSTATUS HWInit(PCM_RESOURCE_LIST pResList, DXGK_DISPLAY_INFORMATION* pDispInfo);
    NTSTATUS HWClose(void);
    NTSTATUS ExecutePresentDisplayOnly(_In_ BYTE*             DstAddr,
                                 _In_ UINT              DstBitPerPixel,
                                 _In_ BYTE*             SrcAddr,
                                 _In_ UINT              SrcBytesPerPixel,
                                 _In_ LONG              SrcPitch,
                                 _In_ ULONG             NumMoves,
                                 _In_ D3DKMT_MOVE_RECT* pMoves,
                                 _In_ ULONG             NumDirtyRects,
                                 _In_ RECT*             pDirtyRect,
                                 _In_ D3DKMDT_VIDPN_PRESENT_PATH_ROTATION Rotation,
                                 _In_ const CURRENT_BDD_MODE* pModeCur);
    VOID BlackOutScreen(CURRENT_BDD_MODE* pCurrentBddMod);
    QXL_NON_PAGED BOOLEAN InterruptRoutine(_In_ PDXGKRNL_INTERFACE pDxgkInterface, _In_  ULONG MessageNumber);
    QXL_NON_PAGED VOID DpcRoutine(PVOID);
    QXL_NON_PAGED VOID ResetDevice(VOID);
    QXL_NON_PAGED VOID VSyncInterruptPostProcess(_In_ PDXGKRNL_INTERFACE);
    NTSTATUS AcquireFrameBuffer(CURRENT_BDD_MODE* pCurrentBddMode);
    NTSTATUS ReleaseFrameBuffer(CURRENT_BDD_MODE* pCurrentBddMode);
    NTSTATUS SetPointerShape(_In_ CONST DXGKARG_SETPOINTERSHAPE* pSetPointerShape);
    NTSTATUS SetPointerPosition(_In_ CONST DXGKARG_SETPOINTERPOSITION* pSetPointerPosition);
    NTSTATUS Escape(_In_ CONST DXGKARG_ESCAPE* pEscap);
protected:
    NTSTATUS GetModeList(DXGK_DISPLAY_INFORMATION* pDispInfo);
private:
    BOOL SetVideoModeInfo(UINT Idx, PVBE_MODEINFO pModeInfo);
};

typedef struct _MemSlot {
    UINT64 start_phys_addr;
    UINT64 end_phys_addr;
    UINT8 *start_virt_addr;
    UINT8 *last_virt_addr;
    QXLPHYSICAL high_bits;
} MemSlot;

typedef struct MspaceInfo {
    mspace _mspace;
    UINT8 *mspace_start;
    UINT8 *mspace_end;
} MspaceInfo;

enum {
    MSPACE_TYPE_DEVRAM,
    MSPACE_TYPE_VRAM,
    NUM_MSPACES,
};

#define RELEASE_RES(res) if (!--(res)->refs) (res)->free(res);
#define GET_RES(res) (++(res)->refs)

/* Debug helpers - tag each resource with this enum */
enum {
    RESOURCE_TYPE_DRAWABLE = 1,
    RESOURCE_TYPE_SURFACE,
    RESOURCE_TYPE_PATH,
    RESOURCE_TYPE_CLIP_RECTS,
    RESOURCE_TYPE_QUIC_IMAGE,
    RESOURCE_TYPE_BITMAP_IMAGE,
    RESOURCE_TYPE_SURFACE_IMAGE,
    RESOURCE_TYPE_SRING,
    RESOURCE_TYPE_CURSOR,
    RESOURCE_TYPE_BUF,
    RESOURCE_TYPE_UPDATE,
};

#ifdef DBG
#define RESOURCE_TYPE(res, val) do { res->type = val; } while (0)
#define INCREMENT_VSYNC_COUNTER(counter) InterlockedIncrement(counter)
#else
#define RESOURCE_TYPE(res, val)
#define INCREMENT_VSYNC_COUNTER(counter)
#endif

typedef struct Resource Resource;
struct Resource {
    UINT32 refs;
    void* ptr;
#ifdef DBG
    UINT32 type;
#endif
    void (*free)(Resource *res);
    UINT8 res[0];
};

#define TIMEOUT_TO_MS               ((LONGLONG) 1 * 10 * 1000)

BOOLEAN
FORCEINLINE
DoWaitForObject(
    PVOID Object,
    PLARGE_INTEGER Timeout,
    LPCSTR name)
{
    NTSTATUS status;
    TimeMeasurement tm;
    status = KeWaitForSingleObject (
            Object,
            Executive,
            KernelMode,
            FALSE,
            Timeout);
    ASSERT(NT_SUCCESS(status));
    tm.Stop();
    if (name && tm.Diff() > 1900)
    {
        // 2 seconds in PresentDisplayOnly triggers watchdog on Win10RS1
        // when VSync control enabled. Print the exact event name.
        DbgPrint(TRACE_LEVEL_ERROR, ("Waiting %d ms for %s\n", tm.Diff(), name));
    }
    return (status == STATUS_SUCCESS);
}

#define WaitForObject(o, timeout) DoWaitForObject((o), (timeout), #o)

VOID
FORCEINLINE
ReleaseMutex(
    PKMUTEX Mutex,
    BOOLEAN locked)
{
    if (locked)
    {
        KeReleaseMutex(Mutex, FALSE);
    }
}

#define QXL_SLEEP(msec) do {                             \
    LARGE_INTEGER timeout;                               \
    timeout.QuadPart = -msec * TIMEOUT_TO_MS;            \
    KeDelayExecutionThread (KernelMode, FALSE, &timeout);\
} while (0);

#define IMAGE_HASH_INIT_VAL(width, height, format) \
    ((UINT32)((width) & 0x1FFF) | ((UINT32)((height) & 0x1FFF) << 13) |\
     ((UINT32)(format) << 26))

#define MAX_OUTPUT_RES 6

typedef struct QXLOutput {
    LIST_ENTRY list;
    UINT32 num_res;
#ifdef DBG
    UINT32 type;
#endif
    Resource *resources[MAX_OUTPUT_RES];
    UINT8 data[0];
} QXLOutput;

typedef struct Ring RingItem;
typedef struct Ring {
    RingItem *prev;
    RingItem *next;
} Ring;

typedef struct InternalImage {
    QXLImage image;
} InternalImage;

typedef struct InternalCursor {
    QXLCursor cursor;
} InternalCursor;

#define CURSOR_ALLOC_SIZE (PAGE_SIZE << 1)

typedef struct DpcCbContext {
    void* ptr;
    UINT32 data;
} DPC_CB_CONTEXT,* PDPC_CB_CONTEXT;

#define BITMAP_ALLOC_BASE (sizeof(Resource) + sizeof(InternalImage) + sizeof(QXLDataChunk))
#define BITS_BUF_MAX (64 * 1024)
#define MIN(x, y) (((x) <= (y)) ? (x) : (y))
#define MAX(x, y) (((x) >= (y)) ? (x) : (y))
#define ALIGN(a, b) (((a) + ((b) - 1)) & ~((b) - 1))

// operation to be run by the presentation thread
class QxlPresentOperation
{
public:
    QxlPresentOperation() {}
    QxlPresentOperation(const QxlPresentOperation&) = delete;
    void operator=(const QxlPresentOperation&) = delete;
    // execute the operation
    virtual void Run()=0;
    virtual ~QxlPresentOperation() {}
};

#include "start-packed.h"
SPICE_RING_DECLARE(QXLPresentOnlyRing, QxlPresentOperation*, 1024);
#include "end-packed.h"

class QxlDevice  :
    public HwDeviceInterface
{
public:
    QxlDevice(_In_ QxlDod* pQxlDod);
    ~QxlDevice(void);
    NTSTATUS QueryCurrentMode(PVIDEO_MODE RequestedMode);
    NTSTATUS SetCurrentMode(ULONG Mode);
    NTSTATUS GetCurrentMode(ULONG* Mode);
    NTSTATUS SetPowerState(DEVICE_POWER_STATE DevicePowerState, DXGK_DISPLAY_INFORMATION* pDispInfo);
    NTSTATUS HWInit(PCM_RESOURCE_LIST pResList, DXGK_DISPLAY_INFORMATION* pDispInfo);
    NTSTATUS HWClose(void);
    NTSTATUS ExecutePresentDisplayOnly(_In_ BYTE*             DstAddr,
                    _In_ UINT              DstBitPerPixel,
                    _In_ BYTE*             SrcAddr,
                    _In_ UINT              SrcBytesPerPixel,
                    _In_ LONG              SrcPitch,
                    _In_ ULONG             NumMoves,
                    _In_ D3DKMT_MOVE_RECT* pMoves,
                    _In_ ULONG             NumDirtyRects,
                    _In_ RECT*             pDirtyRect,
                    _In_ D3DKMDT_VIDPN_PRESENT_PATH_ROTATION Rotation,
                    _In_ const CURRENT_BDD_MODE* pModeCur);
    VOID BlackOutScreen(CURRENT_BDD_MODE* pCurrentBddMod);
    QXL_NON_PAGED BOOLEAN InterruptRoutine(_In_ PDXGKRNL_INTERFACE pDxgkInterface, _In_  ULONG MessageNumber);
    QXL_NON_PAGED VOID DpcRoutine(PVOID);
    QXL_NON_PAGED VOID ResetDevice(VOID);
    QXL_NON_PAGED VOID VSyncInterruptPostProcess(_In_ PDXGKRNL_INTERFACE);
    NTSTATUS SetPointerShape(_In_ CONST DXGKARG_SETPOINTERSHAPE* pSetPointerShape);
    NTSTATUS SetPointerPosition(_In_ CONST DXGKARG_SETPOINTERPOSITION* pSetPointerPosition);
    NTSTATUS Escape(_In_ CONST DXGKARG_ESCAPE* pEscap);
    BOOLEAN IsBIOSCompatible() { return FALSE; }
protected:
    NTSTATUS GetModeList(DXGK_DISPLAY_INFORMATION* pDispInfo);
    QXLDrawable *PrepareBltBits (BLT_INFO* pDst,
                    CONST BLT_INFO* pSrc,
                    UINT  NumRects,
                    _In_reads_(NumRects) CONST RECT *pRects,
                    POINT*   pSourcePoint);
    QXLDrawable *PrepareCopyBits(const RECT& rect, const POINT& sourcePoint);
    QXLDrawable *Drawable(UINT8 type,
                    CONST RECT *area,
                    CONST RECT *clip,
                    UINT32 surface_id);
    void PushDrawable(QXLDrawable *drawable);
    void PushCursorCmd(QXLCursorCmd *cursor_cmd);
    QXLDrawable *GetDrawable();
    QXLCursorCmd *CursorCmd();
    void *AllocMem(UINT32 mspace_type, size_t size, BOOL force);
    VOID SetImageId(InternalImage *internal,
                    BOOL cache_me,
                    LONG width,
                    LONG height,
                    UINT8 format, UINT32 key);
private:
    NTSTATUS QxlInit(DXGK_DISPLAY_INFORMATION* pDispInfo);
    void QxlClose(void);
    void UnmapMemory(void);
    BOOL SetVideoModeInfo(UINT Idx, QXLMode* pModeInfo);
    void UpdateVideoModeInfo(UINT Idx, UINT xres, UINT yres, UINT bpp);
    BOOL InitMemSlots(void);
    BOOL CreateMemSlots(void);
    void DestroyMemSlots(void);
    void CreatePrimarySurface(PVIDEO_MODE_INFORMATION pModeInfo);
    void DestroyPrimarySurface(void);
    void SetupHWSlot(UINT8 Idx, MemSlot *pSlot);
    void SetupMemSlot(UINT8 Idx, UINT64 pastart, UINT64 paend, UINT8 *vastart, UINT8 *valast);
    BOOL CreateEvents(void);
    BOOL CreateRings(void);
    inline UINT8 *VA(QXLPHYSICAL paddr);
    inline QXLPHYSICAL PA(PVOID virt);
    void InitDeviceMemoryResources(void);
    NTSTATUS InitMonitorConfig();
    void InitMspace(UINT32 mspace_type, UINT8 *start, size_t capacity);
    void FlushReleaseRing();
    void FreeMem(void *ptr);
    UINT64 ReleaseOutput(UINT64 output_id);
    void WaitForReleaseRing(void);
    BOOL SetClip(const RECT *clip, QXLDrawable *drawable);
    void AddRes(QXLOutput *output, Resource *res);
    void DrawableAddRes(QXLDrawable *drawable, Resource *res);
    void CursorCmdAddRes(QXLCursorCmd *cmd, Resource *res);
    void FreeClipRects(Resource *res);
    void static FreeClipRectsEx(Resource *res);
    void FreeBitmapImage(Resource *res);
    void static FreeBitmapImageEx(Resource *res);
    void static FreeCursorEx(Resource *res);
    void FreeCursor(Resource *res);
    void WaitForCmdRing(void);
    void PushCmd(void);
    void WaitForCursorRing(void);
    void PushCursor(void);
    BOOLEAN AttachNewBitmap(QXLDrawable *drawable, UINT8 *src, UINT8 *src_end, INT pitch, BOOLEAN bForce);
    void DiscardDrawable(QXLDrawable *drawable);
    BOOLEAN PutBytesAlign(QXLDataChunk **chunk_ptr, UINT8 **now_ptr,
                            UINT8 **end_ptr, UINT8 *src, int size,
                            size_t alloc_size, PLIST_ENTRY pDelayed);
    QXLDataChunk *MakeChunk(DelayedChunk *pdc);
    ULONG PrepareDrawable(QXLDrawable*& drawable);
    void AsyncIo(UCHAR  Port, UCHAR Value);
    void SyncIo(UCHAR  Port, UCHAR Value);
    NTSTATUS UpdateChildStatus(BOOLEAN connect);
    NTSTATUS SetCustomDisplay(QXLEscapeSetCustomDisplay* custom_display);
    void SetMonitorConfig(QXLHead* monitor_config);
    NTSTATUS StartPresentThread();
    void StopPresentThread();
    void PresentThreadRoutine();
    static void PresentThreadRoutineWrapper(HANDLE dev) {
        ((QxlDevice *)dev)->PresentThreadRoutine();
    }
    void PostToWorkerThread(QxlPresentOperation *operation);

    static LONG GetMaxSourceMappingHeight(RECT* DirtyRects, ULONG NumDirtyRects);

private:
    USHORT m_CustomMode;

    PUCHAR m_IoBase;
    BOOLEAN m_IoMapped;
    ULONG m_IoSize;

    PHYSICAL_ADDRESS m_RamPA;
    UINT8 *m_RamStart;
    QXLRam *m_RamHdr;
    ULONG m_RamSize;

    PHYSICAL_ADDRESS m_VRamPA;
    UINT8 *m_VRamStart;
    ULONG m_VRamSize;

    QXLRom *m_RomHdr;
    ULONG m_RomSize;

    MemSlot m_MemSlots[2];
    enum { m_MainMemSlot = 0, m_SurfaceMemSlot = 1 };
    UINT8 m_SlotIdBits;
    UINT8 m_SlotGenBits;
    QXLPHYSICAL m_VaSlotMask;

    QXLCommandRing *m_CommandRing;
    QXLCursorRing *m_CursorRing;
    QXLReleaseRing *m_ReleaseRing;

    KEVENT m_DisplayEvent;
    KEVENT m_CursorEvent;
    KEVENT m_IoCmdEvent;
    KEVENT m_PresentEvent;
    KEVENT m_PresentThreadReadyEvent;

    PUCHAR m_LogPort;
    PUCHAR m_LogBuf;

    KMUTEX m_MemLock;
    KMUTEX m_CmdLock;
    KMUTEX m_IoLock;
    KMUTEX m_CrsLock;
    MspaceInfo m_MSInfo[NUM_MSPACES];

    UINT64 m_FreeOutputs;
    LONG m_Pending;

    QXLMonitorsConfig* m_monitor_config;
    QXLPHYSICAL* m_monitor_config_pa;

    QXLPresentOnlyRing m_PresentRing[1];
    // generation, updated when resolution change
    // this is used to detect if a draw command is obsoleted
    // and should not be executed
    uint16_t m_DrawGeneration;
    HANDLE m_PresentThread;
    BOOLEAN m_bActive;
};

class QxlDod {
private:
    DEVICE_OBJECT* m_pPhysicalDevice;
    DXGKRNL_INTERFACE m_DxgkInterface;
    DXGK_DEVICE_INFO m_DeviceInfo;

    DEVICE_POWER_STATE m_MonitorPowerState;
    DEVICE_POWER_STATE m_AdapterPowerState;
    QXL_FLAGS m_Flags;

    CURRENT_BDD_MODE m_CurrentModes[MAX_VIEWS];

    D3DDDI_VIDEO_PRESENT_SOURCE_ID m_SystemDisplaySourceId;
    DXGKARG_SETPOINTERSHAPE m_PointerShape;

    HwDeviceInterface* m_pHWDevice;
    KTIMER m_VsyncTimer;
    KDPC   m_VsyncTimerDpc;
    BOOLEAN m_bVsyncEnabled;
    LONG m_VsyncFiredCounter;
public:
    QxlDod(_In_ DEVICE_OBJECT* pPhysicalDeviceObject);
    ~QxlDod(void);
#pragma code_seg(push)
#pragma code_seg()
    BOOLEAN IsDriverActive() const
    {
        return m_Flags.DriverStarted;
    }
#pragma code_seg(pop)

    NTSTATUS StartDevice(_In_  DXGK_START_INFO*   pDxgkStartInfo,
                         _In_  DXGKRNL_INTERFACE* pDxgkInterface,
                         _Out_ ULONG*             pNumberOfViews,
                         _Out_ ULONG*             pNumberOfChildren);
    NTSTATUS StopDevice(VOID);
    // Must be Non-Paged
    QXL_NON_PAGED VOID ResetDevice(VOID);

    NTSTATUS DispatchIoRequest(_In_  ULONG VidPnSourceId,
                               _In_  VIDEO_REQUEST_PACKET* pVideoRequestPacket);
    NTSTATUS SetPowerState(_In_  ULONG HardwareUid,
                               _In_  DEVICE_POWER_STATE DevicePowerState,
                               _In_  POWER_ACTION       ActionType);
    // Report back child capabilities
    NTSTATUS QueryChildRelations(_Out_writes_bytes_(ChildRelationsSize) DXGK_CHILD_DESCRIPTOR* pChildRelations,
                                 _In_                             ULONG                  ChildRelationsSize);

    NTSTATUS QueryChildStatus(_Inout_ DXGK_CHILD_STATUS* pChildStatus,
                              _In_    BOOLEAN            NonDestructiveOnly);

    // Return EDID if previously retrieved
    NTSTATUS QueryDeviceDescriptor(_In_    ULONG                   ChildUid,
                                   _Inout_ DXGK_DEVICE_DESCRIPTOR* pDeviceDescriptor);

    // Must be Non-Paged
    // BDD doesn't have interrupts, so just returns false
    QXL_NON_PAGED BOOLEAN InterruptRoutine(_In_  ULONG MessageNumber);

    QXL_NON_PAGED VOID DpcRoutine(VOID);

    // Return DriverCaps, doesn't support other queries though
    NTSTATUS QueryAdapterInfo(_In_ CONST DXGKARG_QUERYADAPTERINFO* pQueryAdapterInfo);

    NTSTATUS SetPointerPosition(_In_ CONST DXGKARG_SETPOINTERPOSITION* pSetPointerPosition);

    NTSTATUS SetPointerShape(_In_ CONST DXGKARG_SETPOINTERSHAPE* pSetPointerShape);

    NTSTATUS Escape(_In_ CONST DXGKARG_ESCAPE* pEscape);

    NTSTATUS PresentDisplayOnly(_In_ CONST DXGKARG_PRESENT_DISPLAYONLY* pPresentDisplayOnly);

    NTSTATUS QueryInterface(_In_ CONST PQUERY_INTERFACE     QueryInterface);

    NTSTATUS IsSupportedVidPn(_Inout_ DXGKARG_ISSUPPORTEDVIDPN* pIsSupportedVidPn);

    NTSTATUS RecommendFunctionalVidPn(_In_ CONST DXGKARG_RECOMMENDFUNCTIONALVIDPN* CONST pRecommendFunctionalVidPn);

    NTSTATUS RecommendVidPnTopology(_In_ CONST DXGKARG_RECOMMENDVIDPNTOPOLOGY* CONST pRecommendVidPnTopology);

    NTSTATUS RecommendMonitorModes(_In_ CONST DXGKARG_RECOMMENDMONITORMODES* CONST pRecommendMonitorModes);

    NTSTATUS EnumVidPnCofuncModality(_In_ CONST DXGKARG_ENUMVIDPNCOFUNCMODALITY* CONST pEnumCofuncModality);

    NTSTATUS SetVidPnSourceVisibility(_In_ CONST DXGKARG_SETVIDPNSOURCEVISIBILITY* pSetVidPnSourceVisibility);

    NTSTATUS CommitVidPn(_In_ CONST DXGKARG_COMMITVIDPN* CONST pCommitVidPn);

    NTSTATUS UpdateActiveVidPnPresentPath(_In_ CONST DXGKARG_UPDATEACTIVEVIDPNPRESENTPATH* CONST pUpdateActiveVidPnPresentPath);

    NTSTATUS QueryVidPnHWCapability(_Inout_ DXGKARG_QUERYVIDPNHWCAPABILITY* pVidPnHWCaps);

    // Part of PnPStop (PnP instance only), returns current mode information (which will be passed to fallback instance by dxgkrnl)
    NTSTATUS StopDeviceAndReleasePostDisplayOwnership(_In_  D3DDDI_VIDEO_PRESENT_TARGET_ID TargetId,
                                                      _Out_ DXGK_DISPLAY_INFORMATION*      pDisplayInfo);

    // Must be Non-Paged
    // Call to initialize as part of bugcheck
    QXL_NON_PAGED NTSTATUS SystemDisplayEnable(_In_  D3DDDI_VIDEO_PRESENT_TARGET_ID       TargetId,
                                 _In_  PDXGKARG_SYSTEM_DISPLAY_ENABLE_FLAGS Flags,
                                 _Out_ UINT*                                pWidth,
                                 _Out_ UINT*                                pHeight,
                                 _Out_ D3DDDIFORMAT*                        pColorFormat);

    // Must be Non-Paged
    // Write out pixels as part of bugcheck
    QXL_NON_PAGED VOID SystemDisplayWrite(_In_reads_bytes_(SourceHeight * SourceStride) VOID* pSource,
                                 _In_                                     UINT  SourceWidth,
                                 _In_                                     UINT  SourceHeight,
                                 _In_                                     UINT  SourceStride,
                                 _In_                                     INT   PositionX,
                                 _In_                                     INT   PositionY);
    PDXGKRNL_INTERFACE GetDxgkInterface(void) { return &m_DxgkInterface;}
    NTSTATUS AcquireDisplayInfo(DXGK_DISPLAY_INFORMATION& DispInfo)
    {
        return m_DxgkInterface.DxgkCbAcquirePostDisplayOwnership(m_DxgkInterface.DeviceHandle, &DispInfo);
    }
    VOID EnableVsync(BOOLEAN bEnable);
private:
    VOID CleanUp(VOID);
    NTSTATUS CheckHardware();
    NTSTATUS WriteHWInfoStr(_In_ HANDLE DevInstRegKeyHandle, _In_ PCWSTR pszwValueName, _In_ PCSTR pszValue);
    // Set the given source mode on the given path
    NTSTATUS SetSourceModeAndPath(CONST D3DKMDT_VIDPN_SOURCE_MODE* pSourceMode,
                                  CONST D3DKMDT_VIDPN_PRESENT_PATH* pPath);

    // Add the current mode to the given monitor source mode set
    NTSTATUS AddSingleMonitorMode(_In_ CONST DXGKARG_RECOMMENDMONITORMODES* CONST pRecommendMonitorModes);

    // Add the current mode to the given VidPn source mode set
    NTSTATUS AddSingleSourceMode(_In_ CONST DXGK_VIDPNSOURCEMODESET_INTERFACE* pVidPnSourceModeSetInterface,
                                 D3DKMDT_HVIDPNSOURCEMODESET hVidPnSourceModeSet,
                                 D3DDDI_VIDEO_PRESENT_SOURCE_ID SourceId);

    // Add the current mode (or the matching to pinned source mode) to the give VidPn target mode set
    NTSTATUS AddSingleTargetMode(_In_ CONST DXGK_VIDPNTARGETMODESET_INTERFACE* pVidPnTargetModeSetInterface,
                                 D3DKMDT_HVIDPNTARGETMODESET hVidPnTargetModeSet,
                                 _In_opt_ CONST D3DKMDT_VIDPN_SOURCE_MODE* pVidPnPinnedSourceModeInfo,
                                 D3DDDI_VIDEO_PRESENT_SOURCE_ID SourceId);
    QXL_NON_PAGED D3DDDI_VIDEO_PRESENT_SOURCE_ID FindSourceForTarget(D3DDDI_VIDEO_PRESENT_TARGET_ID TargetId, BOOLEAN DefaultToZero);
    NTSTATUS IsVidPnSourceModeFieldsValid(CONST D3DKMDT_VIDPN_SOURCE_MODE* pSourceMode) const;
    NTSTATUS IsVidPnPathFieldsValid(CONST D3DKMDT_VIDPN_PRESENT_PATH* pPath) const;
    NTSTATUS RegisterHWInfo(_In_ ULONG Id);
    QXL_NON_PAGED VOID VsyncTimerProc();
    static QXL_NON_PAGED VOID VsyncTimerProcGate(_In_ _KDPC *dpc, _In_ PVOID context, _In_ PVOID arg1, _In_ PVOID arg2);
    QXL_NON_PAGED VOID IndicateVSyncInterrupt();
    static QXL_NON_PAGED BOOLEAN VsyncTimerSynchRoutine(PVOID context);
};

NTSTATUS
MapFrameBuffer(
    _In_                PHYSICAL_ADDRESS    PhysicalAddress,
    _In_                ULONG               Length,
    _Outptr_result_bytebuffer_(Length) VOID**              VirtualAddress);

NTSTATUS
UnmapFrameBuffer(
    _In_reads_bytes_(Length) VOID* VirtualAddress,
    _In_                ULONG Length);

QXL_NON_PAGED UINT BPPFromPixelFormat(D3DDDIFORMAT Format);
QXL_NON_PAGED D3DDDIFORMAT PixelFormatFromBPP(UINT BPP);
UINT SpiceFromPixelFormat(D3DDDIFORMAT Format);

QXL_NON_PAGED VOID CopyBitsGeneric(
                        BLT_INFO* pDst,
                        CONST BLT_INFO* pSrc,
                        UINT  NumRects,
                        _In_reads_(NumRects) CONST RECT *pRects);

QXL_NON_PAGED VOID CopyBits32_32(
                        BLT_INFO* pDst,
                        CONST BLT_INFO* pSrc,
                        UINT  NumRects,
                        _In_reads_(NumRects) CONST RECT *pRects);
QXL_NON_PAGED VOID BltBits (
                        BLT_INFO* pDst,
                        CONST BLT_INFO* pSrc,
                        UINT  NumRects,
                        _In_reads_(NumRects) CONST RECT *pRects);

QXL_NON_PAGED BYTE* GetRowStart(_In_ CONST BLT_INFO* pBltInfo, CONST RECT* pRect);
QXL_NON_PAGED VOID GetPitches(_In_ CONST BLT_INFO* pBltInfo, _Out_ LONG* pPixelPitch, _Out_ LONG* pRowPitch);
