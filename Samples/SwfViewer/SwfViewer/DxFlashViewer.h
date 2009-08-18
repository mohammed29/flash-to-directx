/********************************************************************
	created:	2008/08/02
	created:	2:8:2008   17:18
	filename: 	e:\Visual Studio 2005\Projects\Samples\SwfViewer\DxFlashViewer.h
	file path:	e:\Visual Studio 2005\Projects\Samples\SwfViewer
	file base:	DxFlashViewer
	file ext:	h
	author:		xoyojank
	
	purpose:	A DirectX Flash Viewer
*********************************************************************/
#pragma once
#include <d3dx9.h>

#include <atlbase.h>
#include <atlcom.h>
#include <atlwin.h>
#pragma comment(lib,"atl")

#include "FlashWnd.h"

//#import "flash.ocx"
//#import "PROGID:ShockwaveFlash.ShockwaveFlash"
using namespace ShockwaveFlashObjects; 

class DxFlashViewer
{
public:
	DxFlashViewer(void);
	~DxFlashViewer(void);

	bool Init( IDirect3DDevice9* pDevice, HWND hWnd, int width, int height );

	void OpenFlash(const char* filename);

	void UpdateTexture();

	IDirect3DTexture9* GetTexturePtr();

	int GetViewerWidth() const;
	int GetViewerHeight() const;
	int GetTextureWidth() const;
	int GetTextureHeight() const;

	HRESULT OnWindowMessage(UINT msg, WPARAM wParam, LPARAM lParam);

	HWND ViewerWnd() { return mViewerWnd; }

private:
	int mViewerWidth;
	int mViewerHeight;
	int mTextureWidth;
	int mTextureHeight;
	IDirect3DTexture9* mTexture;
	IDirect3DDevice9* mDevice;

	HWND mViewerWnd;
	IShockwaveFlash* mFlashCtrl;
	IOleInPlaceObjectWindowless* mWndlessObject;

	CFlashWnd* mFlashWnd;
};
