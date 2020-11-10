/*************************************************************************************************************
 **	Name:        UAVOverlapSample.h                                                                         **
 **	Description: Intel sample code demonstrating how to use Intel's UAV Overlap driver extension for D3D11. **
 **	Author:      Geoffrey Douglas, geoffrey.douglas@intel.com                                               **
 **	Published:   <insert date>                                                                              **
 ************************************************************************************************************/

#ifndef UAVOVERLAPSAMPLEAPP_H
#define UAVOVERLAPSAMPLEAPP_H

#include <windows.h>
#include <d3d11.h>
#include <d3d12.h>
#include <d3dcompiler.h>
#include <exception>
#include "DirectXMath.h"
#include <vector>

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui_internal.h"

#include "igdext.h"

#ifndef ThrowIfFailed
#define ThrowIfFailed(x) \
{                        \
    HRESULT hr__ = (x);  \
    if(FAILED(hr__)) { throw std::exception(); return hr__; } \
}
#endif

class UAVOverlapSampleApp
{
public:
	UAVOverlapSampleApp(HWND window, uint32_t width, uint32_t height);

	HRESULT Init();
	void Cleanup();
	void Render(double frameTime);

	bool InitIntelExtensions();

	struct SimpleVertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 uv;
	};

	struct ConstantBuffer
	{
		uint32_t dispatchX;
		uint32_t dispatchY;
		uint32_t windowWidth;
		uint32_t windowHeight;
	};

private:
	HWND mWindow;
	UINT mWidth;
	UINT mHeight;

	ID3D11Device* mDevice;
	ID3D11DeviceContext* mImmediateContext;
	IDXGISwapChain* mSwapChain;

	ID3D11RenderTargetView* mBackBufferRTV;

	ID3D11VertexShader* mVertexShader;
	ID3D11PixelShader* mPixelShader;
	ID3D11ComputeShader* mComputeShader;

	ID3DBlob* mVSBlob;
	ID3DBlob* mPSBlob;
	ID3DBlob* mCSBlob;

	ID3D11InputLayout* mVertexLayout;

	ID3D11Buffer* mVertexBuffer;

	ID3D11Buffer* mConstantBuffer[3600];

	D3D11_VIEWPORT mViewPort;

	ID3D11ShaderResourceView* mSampleSRV;
	ID3D11UnorderedAccessView* mSampleUAV;

	INTCExtensionContext* mINTCExtensionContext;

	bool bUseUAVOverlapExtension;
	bool bUAVOverlapSupported;
	bool bIntelGPUPresent;
};

#endif // UAVOVERLAPSAMPLEAPP_H