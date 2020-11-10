/********************************************************************************
 **	Name:        UAVOverlapSampleApp.cpp                                       **
 **	Description: Device Initialization, Cleanup, Rendering, and ImGui controls **
 **	Author:      Geoffrey Douglas, geoffrey.douglas@intel.com                  **
 **	Published:   <insert date>                                                 **
 *******************************************************************************/

#include "UAVOverlapSampleApp.h"

UAVOverlapSampleApp::UAVOverlapSampleApp(HWND window, uint32_t width, uint32_t height) : mWindow(window), mWidth(width), mHeight(height) 
{ 
	bUseUAVOverlapExtension = false;
	bUAVOverlapSupported = false;
	bIntelGPUPresent = false;
}

bool UAVOverlapSampleApp::InitIntelExtensions()
{
	if (SUCCEEDED(INTC_LoadExtensionsLibrary()))
	{
		INTCExtensionVersion requiredVersion = { 1, 2, 0 };

		INTCExtensionVersion* pSupportedExtVersions = nullptr;
		uint32_t supportedExtVersionCount = 0;

		INTC_D3D11_GetSupportedVersions(mDevice, pSupportedExtVersions, &supportedExtVersionCount);

		INTCExtensionInfo intcExtensionInfo = {};

		//Next, use returned value for supportedExtVersionCount to allocate space for the supported extensions
		pSupportedExtVersions = new INTCExtensionVersion[supportedExtVersionCount];
		memset(pSupportedExtVersions, 0, sizeof(INTCExtensionVersion) * supportedExtVersionCount);

		//Next populate the list of supported version and iterate until you find the needed version
		INTC_D3D11_GetSupportedVersions(mDevice, pSupportedExtVersions, &supportedExtVersionCount);

		for (uint32_t i = 0; i < supportedExtVersionCount; i++)
		{

			if ((pSupportedExtVersions[i].HWFeatureLevel >= requiredVersion.HWFeatureLevel) &&
				(pSupportedExtVersions[i].APIVersion >= requiredVersion.APIVersion) &&
				(pSupportedExtVersions[i].Revision >= requiredVersion.Revision))
			{
				intcExtensionInfo.RequestedExtensionVersion = pSupportedExtVersions[i];
				break;
			}
		}

		delete[] pSupportedExtVersions;

		if (SUCCEEDED(INTC_D3D11_CreateDeviceExtensionContext(mDevice, &mINTCExtensionContext, &intcExtensionInfo, nullptr)))
		{
			return true;
		}
		else
		{
			INTC_UnloadExtensionsLibrary();
			return false;
		}
	}
	else
	{
		return false;
	}
}

HRESULT UAVOverlapSampleApp::Init()
{
	IDXGIFactory1* factory;
	ThrowIfFailed(CreateDXGIFactory1(__uuidof(IDXGIFactory1), (LPVOID*)&factory));

	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	//    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	IDXGIAdapter1* adapter = nullptr;
	D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0 };
	D3D_FEATURE_LEVEL createdFeatureLevel;

	// Attempt to find an Intel GPU among the enumerated adapaters and create a device for it
	for (UINT32 i = 0; factory->EnumAdapters1(i, &adapter) != DXGI_ERROR_NOT_FOUND; ++i)
	{
		DXGI_ADAPTER_DESC1 desc;
		adapter->GetDesc1(&desc);

		if (desc.VendorId != 0x8086) // INTEL VendorId
		{
			continue;
		}

		ThrowIfFailed(D3D11CreateDevice(adapter, D3D_DRIVER_TYPE_UNKNOWN, NULL, createDeviceFlags, featureLevels, _countof(featureLevels), D3D11_SDK_VERSION, &mDevice, &createdFeatureLevel, &mImmediateContext));
		bIntelGPUPresent = true;
		break;
	}

	// If no Intel GPU was found, create a device with the default adapter, but warn the user that 
	// the Intel UAV Overlap extension will not be useable. 
	if (mDevice == nullptr)
	{
		MessageBox(mWindow, L"An Intel GPU is required to use the Intel D3D Extension demonstrated in this sample.", L"No Intel GPU Found", MB_ICONERROR);
		bIntelGPUPresent = false;

		for (UINT32 i = 0; factory->EnumAdapters1(i, &adapter) != DXGI_ERROR_NOT_FOUND; ++i)
		{
			DXGI_ADAPTER_DESC1 desc;
			adapter->GetDesc1(&desc);

			ThrowIfFailed(D3D11CreateDevice(adapter, D3D_DRIVER_TYPE_UNKNOWN, NULL, createDeviceFlags, featureLevels, _countof(featureLevels), D3D11_SDK_VERSION, &mDevice, &createdFeatureLevel, &mImmediateContext));

			break;
		}
	}

	// Create the swap chain
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = mWidth;
	sd.BufferDesc.Height = mHeight;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = mWindow;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	ThrowIfFailed(factory->CreateSwapChain(mDevice, &sd, &mSwapChain));

	// Create a render target view to the swap chain back buffer
	ID3D11Texture2D* backBuffer = NULL;
	ThrowIfFailed(mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer));
	ThrowIfFailed(mDevice->CreateRenderTargetView(backBuffer, NULL, &mBackBufferRTV));
	backBuffer->Release();

	// Setup the viewport
	mViewPort.Width = (FLOAT)mWidth;
	mViewPort.Height = (FLOAT)mHeight;
	mViewPort.MinDepth = 0.0f;
	mViewPort.MaxDepth = 1.0f;
	mViewPort.TopLeftX = 0;
	mViewPort.TopLeftY = 0;

	// Load the pre-compiled shader byte code
	ThrowIfFailed(D3DReadFileToBlob(L"Shaders/VertexShader.cso", &mVSBlob));
	ThrowIfFailed(D3DReadFileToBlob(L"Shaders/PixelShader.cso", &mPSBlob));
	ThrowIfFailed(D3DReadFileToBlob(L"Shaders/ComputeShader.cso", &mCSBlob));

	// Create the shaders used by this sample
	ThrowIfFailed(mDevice->CreateVertexShader(mVSBlob->GetBufferPointer(), mVSBlob->GetBufferSize(), NULL, &mVertexShader));
	ThrowIfFailed(mDevice->CreatePixelShader(mPSBlob->GetBufferPointer(), mPSBlob->GetBufferSize(), NULL, &mPixelShader));
	ThrowIfFailed(mDevice->CreateComputeShader(mCSBlob->GetBufferPointer(), mCSBlob->GetBufferSize(), NULL, &mComputeShader));

	// Define the input layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT numElements = ARRAYSIZE(layout);

	// Create the input layout
	ThrowIfFailed(mDevice->CreateInputLayout(layout, numElements, mVSBlob->GetBufferPointer(), mVSBlob->GetBufferSize(), &mVertexLayout));

	// Set the input layout
	mImmediateContext->IASetInputLayout(mVertexLayout);

	D3D11_TEXTURE2D_DESC sampleTextureDesc;
	sampleTextureDesc.Width = mWidth;
	sampleTextureDesc.Height = mHeight;
	sampleTextureDesc.MipLevels = 1;
	sampleTextureDesc.ArraySize = 1;
	sampleTextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sampleTextureDesc.SampleDesc.Count = 1;
	sampleTextureDesc.SampleDesc.Quality = 0;
	sampleTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	sampleTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	sampleTextureDesc.CPUAccessFlags = 0;
	sampleTextureDesc.MiscFlags = 0;

	D3D11_SHADER_RESOURCE_VIEW_DESC sampleSRVDesc;
	sampleSRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sampleSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	sampleSRVDesc.Texture2D.MostDetailedMip = 0;
	sampleSRVDesc.Texture2D.MipLevels = 1;

	D3D11_UNORDERED_ACCESS_VIEW_DESC sampleUAVDesc;
	sampleUAVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sampleUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	sampleUAVDesc.Texture2D.MipSlice = 0;

	ID3D11Texture2D* sampleTexture = 0;
	ThrowIfFailed(mDevice->CreateTexture2D(&sampleTextureDesc, 0, &sampleTexture));
	ThrowIfFailed(mDevice->CreateShaderResourceView(sampleTexture, &sampleSRVDesc, &mSampleSRV));
	ThrowIfFailed(mDevice->CreateUnorderedAccessView(sampleTexture, &sampleUAVDesc, &mSampleUAV));

	sampleTexture->Release();

	D3D11_BUFFER_DESC desc;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.ByteWidth = ((sizeof(ConstantBuffer) + 15) & ~15);
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	uint32_t numDispatchesX = mWidth / 16;
	uint32_t numDispatchesY = mHeight / 16;

	for (uint32_t x = 0; x < numDispatchesX; x++)
	{
		for (uint32_t y = 0; y < numDispatchesY; y++)
		{
			ConstantBuffer cbuffer = {};
			cbuffer.dispatchX = x;
			cbuffer.dispatchY = y;
			cbuffer.windowWidth = mWidth;
			cbuffer.windowHeight = mHeight;

			D3D11_SUBRESOURCE_DATA initialData = {};
			initialData.pSysMem = &cbuffer;

			ThrowIfFailed(mDevice->CreateBuffer(&desc, &initialData, &mConstantBuffer[(x * numDispatchesY) + y]));
		}
	}

	// Create vertex and index buffers for a fullscreen triangle
	SimpleVertex vertices[3];

	vertices[0].position = DirectX::XMFLOAT3(-1.0f, -3.0f, 0.0f);
	vertices[1].position = DirectX::XMFLOAT3(-1.0f, +1.0f, 0.0f);
	vertices[2].position = DirectX::XMFLOAT3(+3.0f, +1.0f, 0.0f);

	vertices[0].uv = DirectX::XMFLOAT2(0.0f, 2.0f);
	vertices[1].uv = DirectX::XMFLOAT2(0.0f, 0.0f);
	vertices[2].uv = DirectX::XMFLOAT2(2.0f, 0.0f);

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(SimpleVertex) * 3;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA initDataVB;
	ZeroMemory(&initDataVB, sizeof(initDataVB));
	initDataVB.pSysMem = vertices;

	ThrowIfFailed(mDevice->CreateBuffer(&vertexBufferDesc, &initDataVB, &mVertexBuffer));

	// Initialize IMGUI
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	ImGui_ImplWin32_Init(mWindow);
	ImGui_ImplDX11_Init(mDevice, mImmediateContext);

	// Initialize the Intel Driver Extensions Framework for use of the UAV Overlap extension
	if (bIntelGPUPresent == true)
	{
		bUAVOverlapSupported = InitIntelExtensions();
	}
	else
	{
		bUAVOverlapSupported = false;
	}

	return S_OK;
}

void UAVOverlapSampleApp::Cleanup()
{
	// Shutdown IMGUI
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	// Release the resources used by the framework
	if (!SUCCEEDED(INTC_DestroyDeviceExtensionContext(&mINTCExtensionContext)))
	{
		INTC_UnloadExtensionsLibrary();
		throw std::exception("Failed to destroy INTC_DEVICEEXTENSIONCONTEXT");
	}

	// Unload the extensions library
	INTC_UnloadExtensionsLibrary();
}

void UAVOverlapSampleApp::Render(double frameTime)
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// IMGUI Performance Window
	{
		ImGui::Begin("Performance", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
		ImGui::SetWindowPos(ImVec2(0, 0));
		ImGui::SetWindowSize(ImVec2(250, 75));
		ImGui::Text("Frame Time: %lf ms", frameTime);
		ImGui::Text("FPS       : %lf fps", 1000 / frameTime);
		ImGui::End();
	}

	// IMGUI Sample Settings Window
	{
		ImGui::Begin("Settings", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
		ImGui::SetWindowPos(ImVec2(0, 75));
		ImGui::SetWindowSize(ImVec2(250, 100));
		ImGui::Text("UAV Overlap Extension");

		int enableButtonValue = bUseUAVOverlapExtension ? 1 : 0;
		ImGui::RadioButton("Disabled", (int*)&enableButtonValue, 0);

		// If not running on an Intel GPU, disable the button that would enable use of the UAV Overlap extension
		// IMGUI does this by push an item flag and a style var, then popping after the radio button
		if (bIntelGPUPresent == false)
		{
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		}
		ImGui::RadioButton("Enabled", (int*)&enableButtonValue, 1);
		if (bIntelGPUPresent == false)
		{
			ImGui::PopItemFlag();
			ImGui::PopStyleVar();
		}

		bUseUAVOverlapExtension = (enableButtonValue != 0);

		ImGui::End();
	}

	/************************************************************************************************
	 **	Compute Pass                                                                               **
	 ** By design, each dispatch is guaranteed to write to a unique location within the bound UAV, ** 
	 **	so it is safe to disable UAV syncs between them.                                           **
	 ***********************************************************************************************/
	{
		// Bind sample compute shader
		mImmediateContext->CSSetShader(mComputeShader, NULL, 0);

		// Bind sample texture as a UAV
		mImmediateContext->CSSetUnorderedAccessViews(0, 1, &mSampleUAV, 0);

		// Disable UAV syncs until a call to D3D11EndUAVOverlap() is encountered
		if (bUseUAVOverlapExtension && bUAVOverlapSupported)
		{
			INTC_D3D11_BeginUAVOverlap(mINTCExtensionContext);
		}

		// Compute the number of dispatches needed to touch every pixel on screen, 
		// knowing that the sample compute shader runs a 16x16x1 thread group
		uint32_t numDispatchesX = mWidth / 16;
		uint32_t numDispatchesY = mHeight / 16;

		for (uint32_t x = 0; x < numDispatchesX; x++)
		{
			for (uint32_t y = 0; y < numDispatchesY; y++)
			{
				// Bind the sample constant buffer
				mImmediateContext->CSSetConstantBuffers(0, 1, &mConstantBuffer[(x * numDispatchesY) + y]);

				mImmediateContext->Dispatch(1, 1, 1);
			}
		}

		// Re-enable UAV syncs
		if (bUseUAVOverlapExtension && bUAVOverlapSupported)
		{
			INTC_D3D11_EndUAVOverlap(mINTCExtensionContext);
		}

		// Unbind the sample compute shader
		mImmediateContext->CSSetShader(NULL, NULL, 0);

		// Unbind the sample texture that was bound as a UAV
		ID3D11UnorderedAccessView* nullUAV[1] = { NULL };
		mImmediateContext->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);

		// Unbind the sample constant buffer
		ID3D11Buffer* nullBuffer[1] = { NULL };
		mImmediateContext->CSSetConstantBuffers(0, 1, nullBuffer);
	}

	/***************************************************************************************************
	 **	Render Fullscreen Triangle                                                                    **
	 **	The UAV that was written in the previous compute pass is now bound as an SRV and sampled from **
	 **	to produce the final image.                                                                   **
	 **************************************************************************************************/
	{
		// Clear the back buffer. No depth buffer is used in this sample.
		float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		mImmediateContext->ClearRenderTargetView(mBackBufferRTV, ClearColor);
		mImmediateContext->OMSetRenderTargets(1, &mBackBufferRTV, NULL);

		mImmediateContext->RSSetViewports(1, &mViewPort);

		mImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Bind the geometry buffers for the fullscreen triangle
		UINT stride = sizeof(SimpleVertex);
		UINT offset = 0;
		mImmediateContext->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);

		// Bind simple vertex and pixel shaders
		mImmediateContext->VSSetShader(mVertexShader, NULL, 0);
		mImmediateContext->PSSetShader(mPixelShader, NULL, 0);

		// Bind the sample texture (written in the previous compute pass) as an SRV
		mImmediateContext->PSSetShaderResources(0, 1, &mSampleSRV);

		// Draw the fullscreen triangle
		mImmediateContext->Draw(3, 0);

		// Unbind the simple vertex and pixel shaders
		mImmediateContext->VSSetShader(NULL, NULL, 0);
		mImmediateContext->PSSetShader(NULL, NULL, 0);

		// Unbind the sample texture as an SRV (it will get bound as a UAV again next frame)
		ID3D11ShaderResourceView* nullSRV[1] = { NULL };
		mImmediateContext->PSSetShaderResources(0, 1, nullSRV);

		// Render IMGUI
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	mSwapChain->Present(0, 0);
}
