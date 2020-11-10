/*
 *
 * Copyright (C) 2020 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 *
 * File Name:  igdext.h
 *
 * Abstract:   Public header for Intel Extensions Framework
 *
 * Notes:      This file is intended to be included by the application to use
 *             Intel Extensions Framework
 */

#ifndef _IGDEXTAPI_H_
#define _IGDEXTAPI_H_

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

    //////////////////////////////////////////////////////////////////////////
    /// @brief Intel Extensions Framework infrastructure structures 
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    /// @brief Extension Context structure to pass to all extension calls
    struct INTCExtensionContext;

    //////////////////////////////////////////////////////////////////////////
    /// @brief Intel Graphics Device detailed information
    struct INTCDeviceInfo
    {
        uint32_t    GPUMaxFreq;
        uint32_t    GPUMinFreq;
        uint32_t    GTGeneration;
        uint32_t    EUCount;
        uint32_t    PackageTDP;
        uint32_t    MaxFillRate;
        wchar_t     GTGenerationName[ 64 ];
    };

    //////////////////////////////////////////////////////////////////////////
    /// @brief Intel Extensions Version structure
    struct INTCExtensionVersion
    {
        uint32_t    HWFeatureLevel;                         ///< HW Feature Level, based on the Intel HW Platform
        uint32_t    APIVersion;                             ///< API Version
        uint32_t    Revision;                               ///< Revision number
    };

    //////////////////////////////////////////////////////////////////////////
    /// @brief Intel Extensions detailed information structure
    struct INTCExtensionInfo
    {
        INTCExtensionVersion    RequestedExtensionVersion;  ///< [in] Intel Extension Framework interface version requested

        INTCDeviceInfo          IntelDeviceInfo;            ///< [out] Intel Graphics Device detailed information
        const wchar_t*          pDeviceDriverDesc;          ///< [out] Intel Graphics Driver description
        const wchar_t*          pDeviceDriverVersion;       ///< [out] Intel Graphics Driver version string
        uint32_t                DeviceDriverBuildNumber;    ///< [out] Intel Graphics Driver build number
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // @brief INTCExtensionAppInfo is an optional input structure; can be used for specific apps and engine code paths
    struct INTCExtensionAppInfo
    {
        const wchar_t*  pApplicationName;                   ///< [in] Application name
        uint32_t        ApplicationVersion;                 ///< [in] Application version
        const wchar_t*  pEngineName;                        ///< [in] Engine name
        uint32_t        EngineVersion;                      ///< [in] Engine version
    };

    //////////////////////////////////////////////////////////////////////////
    /// @brief D3D11 Intel Extension structure definitions
    //////////////////////////////////////////////////////////////////////////

#ifdef INTC_IGDEXT_D3D11
#endif //INTC_IGDEXT_D3D11

    //////////////////////////////////////////////////////////////////////////
    /// @brief D3D12 Intel Extension structure definitions
    //////////////////////////////////////////////////////////////////////////

#ifdef INTC_IGDEXT_D3D12

    //////////////////////////////////////////////////////////////////////////
    enum INTC_D3D12_COMMAND_QUEUE_THROTTLE_POLICY
    {
        INTC_D3D12_COMMAND_QUEUE_THROTTLE_DYNAMIC = 0,
        INTC_D3D12_COMMAND_QUEUE_THROTTLE_MAX_PERFORMANCE = 255
    };

    struct INTC_D3D12_COMMAND_QUEUE_DESC_0001
    {
        union
        {
            ::D3D12_COMMAND_QUEUE_DESC              *pD3D12Desc;
        };

        INTC_D3D12_COMMAND_QUEUE_THROTTLE_POLICY     CommandThrottlePolicy;         /// Command Queue Throttle Policy
    };
    using INTC_D3D12_COMMAND_QUEUE_DESC = INTC_D3D12_COMMAND_QUEUE_DESC_0001;

#endif //INTC_IGDEXT_D3D12

    //////////////////////////////////////////////////////////////////////////
    /// @brief D3D11 Intel Extensions Framework extension function prototypes 
    //////////////////////////////////////////////////////////////////////////

#ifdef INTC_IGDEXT_D3D11

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief BeginUAVOverlap marks the beginning point for disabling GPU synchronization between consecutive draws and 
    ///        dispatches that share UAV resources.
    /// @param pExtensionContext A pointer to the extension context associated with the current Device.
    /// @returns HRESULT Returns S_OK if it was successful.
    HRESULT INTC_D3D11_BeginUAVOverlap(
        INTCExtensionContext*   pExtensionContext );

    /// @brief EndUAVOverlap marks the end point for disabling GPU synchronization between consecutive draws and dispatches 
    ///        that share UAV resources.
    /// @param pExtensionContext A pointer to the extension context associated with the current Device.
    /// @returns HRESULT Returns S_OK if it was successful.
    HRESULT INTC_D3D11_EndUAVOverlap(
        INTCExtensionContext*   pExtensionContext );

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief MultiDrawInstancedIndirect function submits multiple DrawInstancedIndirect in one call.
    /// @param pExtensionContext A pointer to the extension context associated with the current Device.
    /// @param pDeviceContext A pointer to the device context that will be used to generate rendering commands.
    /// @param drawCount The number of draws.
    /// @param pBufferForArgs Pointer to the Arguments Buffer.
    /// @param alignedByteOffsetForArgs Offset into the Arguments Buffer.
    /// @param byteStrideForArgs The stride between elements in the Argument Buffer.
    void INTC_D3D11_MultiDrawInstancedIndirect(
        INTCExtensionContext*   pExtensionContext,
        ID3D11DeviceContext*    pDeviceContext,
        UINT                    drawCount,
        ID3D11Buffer*           pBufferForArgs,
        UINT                    alignedByteOffsetForArgs,
        UINT                    byteStrideForArgs);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief MultiDrawIndexedInstancedIndirect function submits multiple DrawIndexedInstancedIndirect in one call.
    /// @param pExtensionContext A pointer to the extension context associated with the current Device.
    /// @param pDeviceContext A pointer to the device context that will be used to generate rendering commands.
    /// @param drawCount The number of draws.
    /// @param pBufferForArgs Pointer to the Arguments Buffer.
    /// @param alignedByteOffsetForArgs Offset into the Arguments Buffer.
    /// @param byteStrideForArgs The stride between elements in the Argument Buffer.
    void INTC_D3D11_MultiDrawIndexedInstancedIndirect(
        INTCExtensionContext*   pExtensionContext,
        ID3D11DeviceContext*    pDeviceContext,
        UINT                    drawCount,
        ID3D11Buffer*           pBufferForArgs,
        UINT                    alignedByteOffsetForArgs,
        UINT                    byteStrideForArgs );

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief MultiDrawInstancedIndirect function submits multiple DrawInstancedIndirect in one call. The number of 
    ///        draws are passed using Draw Count Buffer. It must be less or equal the Max Count argument.
    /// @param pExtensionContext A pointer to the extension context associated with the current Device.
    /// @param pDeviceContext A pointer to the device context that will be used to generate rendering commands.
    /// @param pBufferForDrawCount Buffer that contains the number of draws.
    /// @param alignedByteOffsetForDrawCount Offset into the Draw Count Buffer.
    /// @param maxCount Maximum count of draws generated by this call.
    /// @param pBufferForArgs Pointer to the Arguments Buffer.
    /// @param alignedByteOffsetForArgs Offset into the Arguments Buffer.
    /// @param byteStrideForArgs The stride between elements in the Argument Buffer.
    void INTC_D3D11_MultiDrawInstancedIndirectCountIndirect(
        INTCExtensionContext*   pExtensionContext,
        ID3D11DeviceContext*    pDeviceContext,
        ID3D11Buffer*           pBufferForDrawCount,
        UINT                    alignedByteOffsetForDrawCount,
        UINT                    maxCount,
        ID3D11Buffer*           pBufferForArgs,
        UINT                    alignedByteOffsetForArgs,
        UINT                    byteStrideForArgs );

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief MultiDrawIndexedInstancedIndirect function submits multiple DrawInstancedIndirect in one call. The number of 
    ///        draws are passed using Draw Count Buffer. It must be less or equal the Max Count argument.
    /// @param pExtensionContext A pointer to the extension context associated with the current Device.
    /// @param pDeviceContext A pointer to the device context that will be used to generate rendering commands.
    /// @param pBufferForDrawCount Buffer that contains the number of draws.
    /// @param alignedByteOffsetForDrawCount Offset into the Draw Count Buffer.
    /// @param maxCount Maximum count of draws generated by this call.
    /// @param pBufferForArgs Pointer to the Arguments Buffer.
    /// @param alignedByteOffsetForArgs Offset into the Arguments Buffer.
    /// @param byteStrideForArgs The stride between elements in the Argument Buffer.
    void INTC_D3D11_MultiDrawIndexedInstancedIndirectCountIndirect(
        INTCExtensionContext*   pExtensionContext,
        ID3D11DeviceContext*    pDeviceContext,
        ID3D11Buffer*           pBufferForDrawCount,
        UINT                    alignedByteOffsetForDrawCount,
        UINT                    maxCount,
        ID3D11Buffer*           pBufferForArgs,
        UINT                    alignedByteOffsetForArgs,
        UINT                    byteStrideForArgs );

#endif //INTC_IGDEXT_D3D11

    //////////////////////////////////////////////////////////////////////////
    /// @brief D3D12 Intel Extensions Framework extension function prototypes 
    //////////////////////////////////////////////////////////////////////////

#ifdef INTC_IGDEXT_D3D12

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Creates a command queue.
    /// @param pExtensionContext A pointer to the extension context associated with the current Device.
    /// @param pDesc A pointer to a D3D12_COMPUTE_PIPELINE_STATE_DESC structure that describes compute pipeline state.
    /// @param riid The globally unique identifier (GUID) for the command queue interface.
    /// @param ppCommandQueue A pointer to a memory block that receives a pointer to the ID3D12CommandQueue interface for the command queue.
    /// @returns HRESULT Returns S_OK if it was successful.
    HRESULT INTC_D3D12_CreateCommandQueue(
        INTCExtensionContext*                       pExtensionContext,
        const INTC_D3D12_COMMAND_QUEUE_DESC*        pDesc,
        REFIID                                      riid,
        void**                                      ppCommandQueue );

#endif //INTC_IGDEXT_D3D12

    //////////////////////////////////////////////////////////////////////////
    /// @brief Intel Extensions Framework infrastructure function prototypes 
    //////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Extension library loading helper function.
    /// @details 
    ///     Function helps load Intel Extensions Framework module into the currently executing process. 
    ///     If useCurrentProcessDir is set, the function tries to load the library from the current
    ///     process directory first. If that was unsuccessful or useCurrentProcessDir was not set, 
    ///     it tries to find the full path to the Intel DX11 UMD Driver module that must be loaded
    ///     by the current process. Library is loaded from the same path (whether it is DriverStore
    ///     location or system32 folder).
    /// @param useCurrentProcessDir If true, this function attempts to load the Extensions Framework DLL
    ///        from the current process directory. If false, this fnction attempts to load the Extensions
    ///        Framework DLL from the installed graphics driver directory. 
    ///        NOTE: This function determines the path to the installed Intel graphics driver directory using 
    ///        Intel's D3D11 or D3D12 user mode driver DLL, which is expected to already be loaded by the 
    ///        current process. If this function is called before one of those DLLs is loaded (i.e. before 
    ///        the application has called CreateDevice(...)), then it will fail and return E_FAIL.
    /// @returns HRESULT Returns S_OK if it was successful.
    ////////////////////////////////////////////////////////////////////////////////////////
    HRESULT INTC_LoadExtensionsLibrary(
        bool                                        useCurrentProcessDir = false );

    ////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Extension library loading helper function.
    /// @details 
    ///     Function unloads Intel Extensions Framework from the current process.
    ////////////////////////////////////////////////////////////////////////////////////////
    void INTC_UnloadExtensionsLibrary();

#ifdef INTC_IGDEXT_D3D11

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Returns all D3D11 Intel Extensions interface versions supported on a current platform/driver/header file combination.
    ///        It is guaranteed that the application can initialize every extensions interface version returned by this call.
    ///        First call populates pSupportedExtVersionsCount, user of API allocates space for ppSupportedExtVersions then calls 
    ///        the API a second time to populate ppSupportedExtVersions. See sample for an example of API usage.
    /// @param pDevice A pointer to the current D3D11 Device.
    /// @param pSupportedExtVersions A pointer to the table of supported versions.
    /// @param pSupportedExtVersionsCount A pointer to the variable that will hold the number of supported versions.
    ///        Pointer is null if Init fails.
    /// @returns HRESULT Returns S_OK if it was successful.
    ///                  Returns invalid HRESULT if the call was unsuccessful.
    HRESULT INTC_D3D11_GetSupportedVersions(
        ID3D11Device*                               pDevice,
        INTCExtensionVersion*                       pSupportedExtVersions,
        uint32_t*                                   pSupportedExtVersionsCount );

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Creates D3D11 Intel Extensions Device Context and returns ppfnExtensionContext Extension Context object and
    ///        ppfnExtensionFuncs extension function pointers table. This function must be called prior to using extensions.
    /// @param pDevice A pointer to the current Device.
    /// @param ppExtensionContext A pointer to a pointer to the extension context associated with the current Device.
    /// @param pExtensionInfo A pointer to the ExtensionInfo structure. The requestedExtensionVersion member must be set prior to 
    ///        calling this function. The remaining members are filled in with device info about the Intel GPU and info about the graphics driver version.
    /// @param pExtensionAppInfo A pointer to the ExtensionAppInfo structure that can be optionally passed to the driver identifying application and engine.
    /// @returns HRESULT Returns S_OK if it was successful.
    ///                  Returns E_INVALIDARG if invalid arguments are passed.
    ///                  Returns E_OUTOFMEMORY if extensions are not supported by the driver.
    HRESULT INTC_D3D11_CreateDeviceExtensionContext(
        ID3D11Device*                               pDevice,
        INTCExtensionContext**                      ppExtensionContext,
        INTCExtensionInfo*                          pExtensionInfo,
        INTCExtensionAppInfo*                       pExtensionAppInfo );

#endif //INTC_IGDEXT_D3D11

#ifdef INTC_IGDEXT_D3D12

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Returns all D3D12 Intel Extensions interface versions supported on a current platform/driver/header file combination.
    ///        It is guaranteed that the application can initialize every extensions interface version returned by this call.
    ///        First call populates pSupportedExtVersionsCount, user of API allocates space for ppSupportedExtVersions then calls 
    ///        the API a second time to populate ppSupportedExtVersions. See sample for an example of API usage.
    /// @param pDevice A pointer to the current D3D11 Device.
    /// @param pSupportedExtVersions A pointer to the table of supported versions.
    /// @param pSupportedExtVersionsCount A pointer to the variable that will hold the number of supported versions.
    ///        Pointer is null if Init fails.
    /// @returns HRESULT Returns S_OK if it was successful.
    ///                  Returns invalid HRESULT if the call was unsuccessful.
    HRESULT INTC_D3D12_GetSupportedVersions(
        ID3D12Device*                               pDevice,
        INTCExtensionVersion*                       pSupportedExtVersions,
        uint32_t*                                   pSupportedExtVersionsCount );

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Creates D3D12 Intel Extensions Device Context and returns ppfnExtensionContext Extension Context object and
    ///        ppfnExtensionFuncs extension function pointers table. This function must be called prior to using extensions.
    /// @param pDevice A pointer to the current Device.
    /// @param ppExtensionContext A pointer to a pointer to the extension context associated with the current Device.
    /// @param pExtensionInfo A pointer to the ExtensionInfo structure. The requestedExtensionVersion member must be set prior to 
    ///        calling this function. The remaining members are filled in with device info about the Intel GPU and info about the graphics driver version.
    /// @param pExtensionAppInfo A pointer to the ExtensionAppInfo structure that can be optionally passed to the driver identifying application and engine.
    /// @returns HRESULT Returns S_OK if it was successful.
    ///                  Returns E_INVALIDARG if invalid arguments are passed.
    ///                  Returns E_OUTOFMEMORY if extensions are not supported by the driver.
    HRESULT INTC_D3D12_CreateDeviceExtensionContext(
        ID3D12Device*                               pDevice,
        INTCExtensionContext**                      ppExtensionContext,
        INTCExtensionInfo*                          pExtensionInfo,
        INTCExtensionAppInfo*                       pExtensionAppInfo );

#endif //INTC_IGDEXT_D3D12

#if defined(INTC_IGDEXT_D3D11) && defined(INTC_IGDEXT_D3D12)

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Creates a D3D Intel Extensions Device Context and returns ppExtensionContext Extension Context object and
    ///        This function must be called prior to using extensions when creating an extension context with a D3D11 and D3D12 device.
    /// @param pD3D11Device A pointer to the current D3D11 device
    /// @param pD3D12Device A pointer to the current D3D12 device
    /// @param pDevice A pointer to the current Device.
    /// @param ppExtensionContext A pointer to a pointer to the extension context associated with the current Device.
    /// @param pExtensionInfo A pointer to the ExtensionInfo structure. The requestedExtensionVersion member must be set prior to 
    ///        calling this function. The remaining members are filled in with device info about the Intel GPU and info about the graphics driver version.
    /// @param pExtensionAppInfo A pointer to the ExtensionAppInfo structure that can be optionally passed to the driver identifying application and engine.
    /// @returns HRESULT Returns S_OK if it was successful.
    ///                  Returns E_INVALIDARG if invalid arguments are passed.
    ///                  Returns E_OUTOFMEMORY if extensions are not supported by the driver.
    HRESULT INTC_CreateDeviceExtensionContext(
        ID3D11Device*                               pD3D11Device,
        ID3D12Device*                               pD3D12Device,
        INTCExtensionContext**                      ppExtensionContext,
        INTCExtensionInfo*                          pExtensionInfo,
        INTCExtensionAppInfo*                       pExtensionAppInfo );

#endif //INTC_IGDEXT_D3D11 & INTC_IGDEXT_D3D12

#if defined(INTC_IGDEXT_D3D11) || defined(INTC_IGDEXT_D3D12)

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Destroys D3D12 Intel Extensions Device Context and provides cleanup for the Intel Extensions Framework. 
    ///        No D3D12 extensions can be used after calling this function.
    /// @param ppExtensionContext A pointer to a pointer to the extension context associated with the current Device.
    /// @returns HRESULT Returns S_OK if it was successful.
    ///                  Returns E_INVALIDARG if invalid arguments are passed.
    HRESULT INTC_DestroyDeviceExtensionContext(
        INTCExtensionContext**                      ppExtensionContext );

#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif //_IGDEXTAPI_H_