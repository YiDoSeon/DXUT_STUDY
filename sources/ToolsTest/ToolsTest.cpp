//--------------------------------------------------------------------------------------
// File: Framework.cpp
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#pragma warning(disable:4996)
#include "DXUT.h"
#include "DXUTcamera.h"
#include <iostream>
#include <io.h>
#include <direct.h>
#include <string>
#include "resource.h"
#include "Tutorial_Primitive.h"

using namespace std;

Tutorial_Primitive* m_pDemo;

wstring g_AssetDir;

//--------------------------------------------------------------------------------------
// Rejects any D3D9 devices that aren't acceptable to the app by returning false
//--------------------------------------------------------------------------------------
bool CALLBACK IsD3D9DeviceAcceptable(D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat,
	bool bWindowed, void* pUserContext)
{
	// Typically want to skip back buffer formats that don't support alpha blending
	IDirect3D9* pD3D = DXUTGetD3D9Object();
	if (FAILED(pD3D->CheckDeviceFormat(pCaps->AdapterOrdinal, pCaps->DeviceType,
		AdapterFormat, D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING,
		D3DRTYPE_TEXTURE, BackBufferFormat)))
		return false;

	return true;
}


//--------------------------------------------------------------------------------------
// Before a device is created, modify the device settings as needed
//--------------------------------------------------------------------------------------
bool CALLBACK ModifyDeviceSettings(DXUTDeviceSettings* pDeviceSettings, void* pUserContext)
{
	return true;
}


//--------------------------------------------------------------------------------------
// Create any D3D9 resources that will live through a device reset (D3DPOOL_MANAGED)
// and aren't tied to the back buffer size
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D9CreateDevice(IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc,
	void* pUserContext)
{
	// 렌더링은 와이어프레임으로
	//pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	//D3DXIMAGE_INFO info;
	//D3DXCreateTextureFromFileEx(pd3dDevice, L"Sprites/Laharl.png", D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, &info, NULL, &g_pTexture);

	m_pDemo = new Tutorial_Primitive();
	m_pDemo->Init();

	return S_OK;
}


//--------------------------------------------------------------------------------------
// Create any D3D9 resources that won't live through a device reset (D3DPOOL_DEFAULT) 
// or that are tied to the back buffer size 
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D9ResetDevice(IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc,
	void* pUserContext)
{
	HRESULT hr;

	/*m_pDemo = new Tutorial_Primitive();
	m_pDemo->Init();*/

	return S_OK;
}


//--------------------------------------------------------------------------------------
// Handle updates to the scene.  This is called regardless of which D3D API is used
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameMove(double fTime, float fElapsedTime, void* pUserContext)
{
	m_pDemo->FrameMove(fElapsedTime);
}


//--------------------------------------------------------------------------------------
// Render the scene using the D3D9 device
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D9FrameRender(IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext)
{
	HRESULT hr;

	// Clear the render target and the zbuffer 
	V(pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 45, 50, 170), 1.0f, 0));

	pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// Render the scene
	if (SUCCEEDED(pd3dDevice->BeginScene()))
	{
		m_pDemo->Render(fElapsedTime);
		V(pd3dDevice->EndScene());
	}
}


//--------------------------------------------------------------------------------------
// Handle messages to the application 
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
	bool* pbNoFurtherProcessing, void* pUserContext)
{
	switch (uMsg)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_FILE_OPEN:
		{
			char str[300];
			WCHAR lpstrFile[100] = L"";
			OPENFILENAME OFN;
			memset(&OFN, 0, sizeof(OPENFILENAME));
			OFN.lStructSize = sizeof(OPENFILENAME);
			OFN.hwndOwner = hWnd;
			OFN.lpstrFilter = L"Every File(*.*)\0*.*\0Text File\0*.txt;*.doc\0";
			OFN.lpstrFile = lpstrFile;
			OFN.nMaxFile = 256;
			OFN.lpstrInitialDir = L"c:\\";
			if (GetOpenFileName(&OFN)!=0)
			{
				m_pDemo->SetTextureFromFileName(lpstrFile);
			}
			return 0;
		}
		case ID_FILE_CLOSE:
			PostQuitMessage(0);
			return 0;
		}
		return 0;
	default:
		return 0;
	}
	return 0;
	//return g_Camera.HandleMessages(hWnd, uMsg, wParam, lParam);
}


//--------------------------------------------------------------------------------------
// Release D3D9 resources created in the OnD3D9ResetDevice callback 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D9LostDevice(void* pUserContext)
{
	//SAFE_DELETE(m_pDemo);
}


//--------------------------------------------------------------------------------------
// Release D3D9 resources created in the OnD3D9CreateDevice callback 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D9DestroyDevice(void* pUserContext)
{
	SAFE_DELETE(m_pDemo);
}

wstring ExePath() {
	WCHAR buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	wstring::size_type pos = wstring(buffer).find_last_of(L"\\/");
	return wstring(buffer).substr(0, pos);
}

//--------------------------------------------------------------------------------------
// Initialize everything and go into a render loop
//--------------------------------------------------------------------------------------
INT WINAPI wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	//

#ifdef _DEBUG
	//_chdir("..\\..\\assets");
	AllocConsole();

	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);
#endif

	// 디렉토리를 assets 으로 변경해주는데 만약 성공시에는 작업 디렉토리가 변경 된다.
	// 하지만 실패시에는 작업 디렉토리가 변경되지 않는다.
	// 참고로 속성 내에서 작업 디렉토리를 설정했을 시에는 실패 했을 때 마지막 작업디렉토리에서 변경이 되지 않는 것이다.
	if (_chdir("assets") == 0)
	{
		WCHAR dir[100];
		GetCurrentDirectory(100, dir);
		printf("Success - Change Directory : ");
		printf("%ws\n", dir);
	}
	else
	{
		WCHAR dir[100];
		GetCurrentDirectory(100, dir);
		printf("Fail - Not Change Directory : ");
		printf("%ws\n", dir);
	}

	/*wstring */g_AssetDir = ExePath();
	//wcout << str << endl;
	g_AssetDir += L"\\assets\\";
	const int nFolder = 5;
	wstring folderName[nFolder] = { L"Sprites", L"Icons", L"Videos", L"Models", L"Textures"
								};
	for (int i = 0; i < nFolder; i++)
	{
		wstring path = g_AssetDir + folderName[i];
		//wcout << path << endl;
		CreateDirectory(path.c_str(), NULL);
	}
	/*WCHAR dir[100];
	GetCurrentDirectory(100, dir);
	wcscat(dir, L"\\Sprites");
	printf("%ws\n", dir);
	CreateDirectory(dir, NULL);*/

	/*WCHAR dir[100];
	GetCurrentDirectory(100, dir);
	SetCurrentDirectory(dir);
	char cDir[100];
	getcwd(cDir, 100);
	printf("%ws\n", dir);
	printf("%s\n", cDir);
	getcwd(cDir, 100);
	printf("%s\n", cDir);*/

	// Set the callback functions
	DXUTSetCallbackD3D9DeviceAcceptable(IsD3D9DeviceAcceptable);
	DXUTSetCallbackD3D9DeviceCreated(OnD3D9CreateDevice);
	DXUTSetCallbackD3D9DeviceReset(OnD3D9ResetDevice);
	DXUTSetCallbackD3D9FrameRender(OnD3D9FrameRender);
	DXUTSetCallbackD3D9DeviceLost(OnD3D9LostDevice);
	DXUTSetCallbackD3D9DeviceDestroyed(OnD3D9DestroyDevice);
	DXUTSetCallbackDeviceChanging(ModifyDeviceSettings);
	DXUTSetCallbackMsgProc(MsgProc);
	DXUTSetCallbackFrameMove(OnFrameMove);

	// TODO: Perform any application-level initialization here

	// Initialize DXUT and create the desired Win32 window and Direct3D device for the application
	DXUTInit(true, true); // Parse the command line and show msgboxes
	DXUTSetHotkeyHandling(true, true, true);  // handle the default hotkeys
	DXUTSetCursorSettings(true, true); // Show the cursor and clip it when in full screen
	HICON hIcon = (HICON)LoadImage(NULL, L"Icon/GameIcon.ico", IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
	HMENU hMenu = (HMENU)LoadMenu(NULL, MAKEINTRESOURCE(IDR_MAINMENU));

	DXUTCreateWindow(L"TEST", NULL, hIcon, hMenu);
	DXUTCreateDevice(true, 1024, 768);

	// Start the render loop
	DXUTMainLoop();
#ifdef _DEBUG
	FreeConsole();
#endif

	// TODO: Perform any application-level cleanup here

	return DXUTGetExitCode();
}