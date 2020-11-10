/*********************************************************************
 **	Name:        main.cpp                                           **
 **	Description: UAV Overlap Sample - Application Main Entrypoint   **
 **	Author:      Geoffrey Douglas, geoffrey.douglas@intel.com       **
 **	Published:   <insert date>                                      **
 ********************************************************************/

#include "UAVOverlapSampleApp.h"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

struct SimplePerformanceTimer
{
	double invFreq;
	unsigned long long previousTime;
	unsigned long long currentTime;
	double deltaTime;
	double frameTime;
	unsigned int frameCounter;
};

void UpdatePerformanceTimer(SimplePerformanceTimer& perfTimer)
{
	perfTimer.frameCounter++;

	QueryPerformanceCounter((LARGE_INTEGER*)& perfTimer.currentTime);
	perfTimer.deltaTime += (perfTimer.currentTime - perfTimer.previousTime) * perfTimer.invFreq;
	perfTimer.previousTime = perfTimer.currentTime;

	// After 1 second real-world time, compute a new frame time and reset the frame counter.
	// This cuts down on the rapid-fire frame time updates that would otherwise happen each frame.
	if (perfTimer.deltaTime > 1.0)
	{
		perfTimer.frameTime = (perfTimer.deltaTime / perfTimer.frameCounter) * 1000;
		perfTimer.deltaTime = 0;
		perfTimer.frameCounter = 0;
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return true;

	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

HWND InitWindow( HINSTANCE hInstance, int nCmdShow )
{
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"MainWnd";

	if (!RegisterClass(&wc))
	{
		MessageBox(0, L"RegisterClass Failed.", 0, 0);
		return NULL;
	}

	// Disabled resizing, minimizing, and maximizing, as they are not needed to demonstrate the extension
	// and it makes the sample code less cumbersome.
	DWORD windowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;

	RECT R = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
	AdjustWindowRect(&R, windowStyle, false);
	int width = R.right - R.left;
	int height = R.bottom - R.top;

	HWND window;
	window = CreateWindow(L"MainWnd", L"Intel UAV Overlap D3D11 Driver Extension Sample Application", windowStyle, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, hInstance, 0);
	
	if (!window)
	{
		MessageBox(0, L"CreateWindow Failed.", 0, 0);
		return NULL;
	}

    ShowWindow(window, nCmdShow );

    return window;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	HWND window = InitWindow(hInstance, nCmdShow);
	if (window == NULL)
	{
		return 0;
	}

	UAVOverlapSampleApp app(window, WINDOW_WIDTH, WINDOW_HEIGHT);

	if (FAILED(app.Init()))
	{
		app.Cleanup();
		return 0;
	}

	SimplePerformanceTimer perfTimer = { 0 };
	{
		unsigned long long freq;
		QueryPerformanceFrequency((LARGE_INTEGER*)& freq);
		perfTimer.invFreq = 1.0f / freq;

		QueryPerformanceCounter((LARGE_INTEGER*)& perfTimer.currentTime);
		perfTimer.previousTime = perfTimer.currentTime;

		perfTimer.frameCounter = 0;
	}

	// Main message loop
	MSG msg = { 0 };
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			UpdatePerformanceTimer(perfTimer);
			app.Render(perfTimer.frameTime);
		}
	}

	app.Cleanup();

	return (int)msg.wParam;
}
