/********************************************************************
	created:	2008/08/02
	created:	2:8:2008   17:19
	filename: 	e:\Visual Studio 2005\Projects\Samples\SwfViewer\DxFlashViewer.cpp
	file path:	e:\Visual Studio 2005\Projects\Samples\SwfViewer
	file base:	DxFlashViewer
	file ext:	cpp
	author:		xoyojank
	
	purpose:	A DirectX Flash Viewer
*********************************************************************/
#include "StdAfx.h"
#include "DxFlashViewer.h"


DxFlashViewer::DxFlashViewer(void)
: mViewerWidth(0)
, mViewerHeight(0)
, mTextureWidth(0)
, mTextureHeight(0)
, mTexture(NULL)
, mDevice(NULL)
, mViewerWnd(0)
, mFlashCtrl(NULL)
, mWndlessObject(NULL)
//, mFlashWnd(NULL)
{
}

DxFlashViewer::~DxFlashViewer(void)
{
	DestroyWindow(this->mViewerWnd);
	if (NULL != this->mTexture)
	{
		this->mTexture->Release();
		this->mTexture = NULL;
	}
	if (NULL != this->mFlashCtrl)
	{
		this->mFlashCtrl->Release();
		this->mFlashCtrl = NULL;
	}
	if (NULL != this->mWndlessObject)
	{
		this->mWndlessObject->Release();
		this->mWndlessObject = NULL;
	}
}

bool DxFlashViewer::Init( IDirect3DDevice9* pDevice, HWND hWnd, int width, int height )
{
	if (NULL == pDevice || width <= 0 || height <= 0)
		return false;

	this->mDevice = pDevice;
	this->mViewerWidth = width;
	this->mViewerHeight = height;

	// calculate texture size required (next power of two above browser window size)
	for ( mTextureWidth = 1; mTextureWidth < mViewerWidth; mTextureWidth <<= 1 );
	for ( mTextureHeight = 1; mTextureHeight < mViewerHeight; mTextureHeight <<= 1 );

	///// !!!!!!!!!!!!!!!!!!!!1111111111111
	//mFlashWnd = new CFlashWnd;
	//mFlashWnd->Create(ShockwaveFlashObjects::CLSID_ShockwaveFlash,
	//	WS_EX_LAYERED, WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS,
	//	hWnd, 0/*g_hInst*/);
	///// !!!!!!!!!!!1111111111111111111


	// create the browser window in the background
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEX));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.lpfnWndProc = DefWindowProc;
	wc.lpszClassName = L"FlashViewer";
	RegisterClassEx(&wc);

	this->mViewerWnd = CreateWindowEx(0, L"FlashViewer", L"FlashViewer", WS_CHILD/*|WS_DISABLED*//*WS_POPUP*/
		, 0, 0, width, height, hWnd, NULL, NULL, 0);

	if (!this->mViewerWnd)
		return false;

	//ShowWindow(mViewerWnd, SW_SHOW); // !!!! @@@@
	//SetWindowLong(mViewerWnd, GWL_WNDPROC, (LONG)NullProc);
	//SetWindowPos(mViewerWnd, 0/*HWND_BOTTOM*/, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);

	//embed the player in the window
	HRESULT hr;
	hr = CoCreateInstance(__uuidof(ShockwaveFlash), 0, CLSCTX_ALL, __uuidof(IShockwaveFlash), (void **)&(this->mFlashCtrl)); 
	if (FAILED(hr))
		return false;
	hr = this->mFlashCtrl->put_WMode(L"transparent");
	//hr = AtlAxAttachControl(this->mFlashCtrl, this->mViewerWnd, NULL);
	if (FAILED(hr))
		return false;
	hr = this->mFlashCtrl->QueryInterface(__uuidof(IOleInPlaceObjectWindowless), (void**)&this->mWndlessObject);
	if (FAILED(hr))
		return false;

	//
	RECT crect; GetClientRect(mViewerWnd, &crect);
	mWndlessObject->SetObjectRects(&crect, &crect);

	//HRESULT hr;

	// create the render texture for Direct3D
	hr =  this->mDevice->CreateTexture(this->mTextureWidth, this->mTextureHeight
		, 0, NULL, D3DFMT_X8R8G8B8, D3DPOOL_MANAGED, &this->mTexture, NULL);

	if (FAILED(hr))
		return false;

	return true;
}

void DxFlashViewer::OpenFlash( const char* filename )
{
	//mFlashWnd->m_lpControl->put_Movie(_bstr_t(filename));
	this->mFlashCtrl->put_Movie(_bstr_t(filename));
}

IDirect3DTexture9* DxFlashViewer::GetTexturePtr()
{
	return this->mTexture;
}

int DxFlashViewer::GetViewerWidth() const
{
	return this->mViewerWidth;
}

int DxFlashViewer::GetViewerHeight() const
{
	return this->mViewerHeight;
}

int DxFlashViewer::GetTextureWidth() const
{
	return this->mTextureWidth;
}

int DxFlashViewer::GetTextureHeight() const
{
	return this->mTextureHeight;
}

extern HWND g_hWnd;

void DxFlashViewer::UpdateTexture()
{
	//if (mFlashWnd && mFlashWnd->m_lpInPlaceObjWindowless)
	//{
	//	//POINT pnt; GetCursorPos(&pnt);
	//	//SetCursorPos(pnt.x, pnt.y);
	//	//ScreenToClient(g_hWnd, &pnt);
	//	////static int shift = 1;
	//	////pnt.x += shift; shift = -shift;
	//	//LRESULT lr = 0;
	//	//LPARAM lparam = (pnt.y << 16) | (pnt.x & 0xffff);
	//	//mFlashWnd->m_lpInPlaceObjWindowless->OnWindowMessage(WM_MOUSEMOVE, 0, lparam, &lr);
	//	//SendMessage(mFlashWnd->GetHWND(), WM_ACTIVATE, 0, 0/*lparam*/);
	//	//mFlashWnd->m_lpControl->Loop = true;
	//	//mFlashWnd->InvalidateRect();
	//}
	//mFlashWnd->m_lpControl->
	//SendMessage(mFlashWnd->GetHWND(), WM_MOUSEACTIVATE, 0, 0);
	//SendMessage(mFlashWnd->GetHWND(), WM_CAPTURECHANGED, 0, 0);
	//InvalidateRect(this->mViewerWnd, 0, TRUE);

	if(NULL == this->mViewerWnd)
		return;
	RECT rect = {0, 0, this->mViewerWidth, this->mViewerHeight};
	IDirect3DSurface9* pSurface = NULL;
	this->mTexture->GetSurfaceLevel(0, &pSurface);
	if (NULL != pSurface)
	{
		HDC hdcTexture;
		HRESULT hr = pSurface->GetDC(&hdcTexture);
		if(FAILED(hr)) return;
		::SetMapMode(hdcTexture, MM_TEXT);
		::OleDraw(this->mFlashCtrl, DVASPECT_CONTENT, hdcTexture, &rect);
		//::OleDraw(mFlashWnd->m_lpControl, DVASPECT_CONTENT, hdcTexture, &rect);
		pSurface->ReleaseDC(hdcTexture);
		pSurface->Release();
	}
}

HRESULT DxFlashViewer::OnWindowMessage( UINT msg, WPARAM wParam, LPARAM lParam )
{
	//if (mFlashWnd)
	//	return SendMessage(mFlashWnd->GetHWND(), msg, wParam, lParam);
	//return SendMessage(mViewerWnd, msg, wParam, lParam);
	//mFlashCtrl->Stop();
	//mFlashCtrl->Play();
	LRESULT lr = 0;
	if (NULL != mWndlessObject)
	//if (mFlashWnd && mFlashWnd->m_lpInPlaceObjWindowless)
	{
		mWndlessObject->OnWindowMessage(msg, wParam, lParam, &lr);
		//mFlashWnd->m_lpInPlaceObjWindowless->OnWindowMessage(msg, wParam, lParam, &lr);
	}
	return 0;
}
