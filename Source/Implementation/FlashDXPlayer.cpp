//---------------------------------------------------------------------
// Copyright (c) 2009 Maksym Diachenko, Viktor Reutskyy, Anton Suchov
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//---------------------------------------------------------------------

#include "stdafx.h"
#include "FlashDXPlayer.h"
#include "shlwapi.h"

using namespace ShockwaveFlashObjects;

//---------------------------------------------------------------------
CFlashDXPlayer::CFlashDXPlayer(HMODULE flashDLL, unsigned int width, unsigned int height)
{
	m_userData = NULL;
	m_flashInterface = NULL;
	m_oleObject = NULL;
	m_windowlessObject = NULL;
	m_lastMouseX = 0;
	m_lastMouseY = 0;
	m_lastMouseButtons = 0;

	m_dirtyFlag = false;

	m_width = width;
	m_height = height;

	m_controlSite.Init(this);
	m_controlSite.AddRef();

	HRESULT hr;

	typedef HRESULT (__stdcall *DllGetClassObjectFunc)(REFCLSID rclsid, REFIID riid, LPVOID * ppv);

	if (flashDLL != NULL)
	{
		IClassFactory* aClassFactory = NULL;
		DllGetClassObjectFunc aDllGetClassObjectFunc = (DllGetClassObjectFunc) GetProcAddress(flashDLL, "DllGetClassObject");
		hr = aDllGetClassObjectFunc(CLSID_ShockwaveFlash, IID_IClassFactory, (void**)&aClassFactory);

		if (FAILED(hr))
			return;

		aClassFactory->CreateInstance(NULL, IID_IOleObject, (void**)&m_oleObject);
		aClassFactory->Release();	
	}
	else
	{
		hr = CoCreateInstance(CLSID_ShockwaveFlash, NULL, CLSCTX_INPROC_SERVER, IID_IOleObject, (void**)&m_oleObject);

		if (FAILED(hr))
			return;
	}

	IOleClientSite* pClientSite = NULL;
	hr = m_controlSite.QueryInterface(__uuidof(IOleClientSite), (void**)&pClientSite);
	if (FAILED(hr))
		return;

	hr = m_oleObject->SetClientSite(pClientSite);
	if (FAILED(hr))
		return;

	hr = m_oleObject->QueryInterface(__uuidof(IShockwaveFlash), (void**)&m_flashInterface);
	if (FAILED(hr))
		return;

	_bstr_t transparent = "transparent";
	hr = m_flashInterface->put_WMode(transparent);
	assert(SUCCEEDED(hr));
	SetQuality(IFlashDXPlayer::QUALITY_BEST);

	hr = m_oleObject->DoVerb(OLEIVERB_INPLACEACTIVATE, NULL, pClientSite, 0, NULL, NULL);
	assert(SUCCEEDED(hr));

	pClientSite->Release();

	hr = m_oleObject->QueryInterface(__uuidof(IOleInPlaceObjectWindowless), (void**)&m_windowlessObject);
	assert(SUCCEEDED(hr));

	m_flashSink.Init(this);
	m_flashSink.AddRef();

	// Resize player
	ResizePlayer(width, height);
}

//---------------------------------------------------------------------
CFlashDXPlayer::~CFlashDXPlayer()
{
	SAFE_RELEASE(m_windowlessObject);
	SAFE_RELEASE(m_flashInterface);

	m_flashSink.Shutdown();
	m_flashSink.Release();

	if (m_oleObject)
	{
		m_oleObject->Close(OLECLOSE_NOSAVE);
		m_oleObject->Release();	
	}

	m_controlSite.Release();
}

//---------------------------------------------------------------------
void CFlashDXPlayer::AddDirtyRect(const RECT* pRect)
{
	if (m_dirtyFlag == false)
	{
		m_dirtyRects.clear();
		m_dirtyRect.left = m_dirtyRect.top = LONG_MAX;
		m_dirtyRect.right = m_dirtyRect.bottom = -LONG_MAX;
	}

	m_dirtyFlag = true;

	if (pRect == NULL)
	{
		RECT rect = { 0, 0, m_width, m_height };
		m_dirtyRect = rect;
		m_dirtyRects.clear();
		m_dirtyRects.push_back(m_dirtyRect);
	}
	else
	{
		#define MIN_MACRO(x, y) ((x) < (y) ? (x) : (y))
		#define MAX_MACRO(x, y) ((x) > (y) ? (x) : (y))

		if (m_dirtyRect.left <= pRect->left && m_dirtyRect.top <= pRect->top &&
			m_dirtyRect.right >= pRect->right && m_dirtyRect.bottom >= pRect->bottom)
		{
			return; // already included
		}

		RECT rect = { pRect->left, pRect->top, pRect->right, pRect->bottom };
		rect.left = MAX_MACRO(rect.left, 0);
		rect.top = MAX_MACRO(rect.top, 0);
		rect.right = MIN_MACRO(rect.right, (LONG)m_width);
		rect.bottom = MIN_MACRO(rect.bottom, (LONG)m_height);

		m_dirtyRect.left = MIN_MACRO(m_dirtyRect.left, pRect->left);
		m_dirtyRect.top = MIN_MACRO(m_dirtyRect.top, pRect->top);
		m_dirtyRect.right = MAX_MACRO(m_dirtyRect.right, pRect->right);
		m_dirtyRect.bottom = MAX_MACRO(m_dirtyRect.bottom, pRect->bottom);

		m_dirtyRects.push_back(rect);
	}
}

//---------------------------------------------------------------------
void CFlashDXPlayer::SetUserData(intptr_t data)
{
	m_userData = data;
}

//---------------------------------------------------------------------
intptr_t CFlashDXPlayer::GetUserData() const
{
	return m_userData;
}

//---------------------------------------------------------------------
IFlashDXPlayer::EState CFlashDXPlayer::GetState() const
{
	if (!m_flashInterface)
		return IFlashDXPlayer::STATE_IDLE;

	if (m_flashInterface->IsPlaying())
		return IFlashDXPlayer::STATE_PLAYING;
	return IFlashDXPlayer::STATE_STOPPED;
}

//---------------------------------------------------------------------
void CFlashDXPlayer::SetQuality(EQuality quality)
{
	if (m_flashInterface)
	{
		static char* aQualityNames[6] = { "Low", "Medium", "High", "Best", "AutoLow", "AutoHigh" };

		_bstr_t newStr = aQualityNames[quality];
		m_flashInterface->PutQuality2(newStr);
	}
}

//---------------------------------------------------------------------
void CFlashDXPlayer::SetTransparencyMode(ETransparencyMode mode)
{
	if (m_flashInterface)
	{
		switch (mode)
		{
		case IFlashDXPlayer::TMODE_OPAQUE:
			m_flashInterface->PutWMode(_bstr_t("opaque"));
			break;
		case IFlashDXPlayer::TMODE_TRANSPARENT:
			m_flashInterface->PutWMode(_bstr_t("transparent"));
			break;
		default:
			break;
		}
	}
}

//---------------------------------------------------------------------
bool CFlashDXPlayer::LoadMovie(const wchar_t* movie)
{
	if (m_flashInterface)
	{
		wchar_t fullpath[MAX_PATH];

		if(!_wfullpath(fullpath, movie, MAX_PATH))
			return false;

		if (!PathFileExists(fullpath))
			return false;

		HRESULT hr = m_flashInterface->put_Movie(_bstr_t(fullpath));
		return SUCCEEDED(hr);
	}

	return false;
}

//---------------------------------------------------------------------
COLORREF CFlashDXPlayer::GetBackgroundColor()
{
	long color = 0;
	if (m_flashInterface)
		color = m_flashInterface->GetBackgroundColor();
	return (COLORREF)color;
}

//---------------------------------------------------------------------
void CFlashDXPlayer::SetBackgroundColor(COLORREF color)
{
	if (m_flashInterface)
		m_flashInterface->PutBackgroundColor((long)color);
}

//---------------------------------------------------------------------
void CFlashDXPlayer::StartPlaying()
{
	if (m_flashInterface)
		m_flashInterface->Play();
}

//---------------------------------------------------------------------
void CFlashDXPlayer::StartPlaying(const wchar_t* timelineTarget)
{
	if (m_flashInterface)
		m_flashInterface->TPlay(_bstr_t(timelineTarget));
}

//---------------------------------------------------------------------
void CFlashDXPlayer::StopPlaying()
{
	if (m_flashInterface)
		m_flashInterface->StopPlay();
}

//---------------------------------------------------------------------
void CFlashDXPlayer::StopPlaying(const wchar_t* timelineTarget)
{
	if (m_flashInterface)
		m_flashInterface->TStopPlay(_bstr_t(timelineTarget));
}

//---------------------------------------------------------------------
void CFlashDXPlayer::Rewind()
{
	if (m_flashInterface)
		m_flashInterface->Rewind();
}

//---------------------------------------------------------------------
void CFlashDXPlayer::StepForward()
{
	if (m_flashInterface)
		m_flashInterface->Forward();
}

//---------------------------------------------------------------------
void CFlashDXPlayer::StepBack()
{
	if (m_flashInterface)
		m_flashInterface->Back();
}

//---------------------------------------------------------------------
int CFlashDXPlayer::GetCurrentFrame()
{
	if (m_flashInterface)
		return m_flashInterface->CurrentFrame();
	return -1;
}

//---------------------------------------------------------------------
int CFlashDXPlayer::GetCurrentFrame(const wchar_t* timelineTarget /*= L"/"*/)
{
	if (m_flashInterface)
		return m_flashInterface->TCurrentFrame(_bstr_t(timelineTarget));
	return -1;
}

//---------------------------------------------------------------------
void CFlashDXPlayer::GotoFrame(int frame)
{
	if (m_flashInterface)
		m_flashInterface->GotoFrame((long)frame);
}

//---------------------------------------------------------------------
void CFlashDXPlayer::GotoFrame(int frame, const wchar_t* timelineTarget)
{
	if (m_flashInterface)
		m_flashInterface->TGotoFrame(_bstr_t(timelineTarget), (long)frame);
}

//---------------------------------------------------------------------
void CFlashDXPlayer::CallFrame(int frame, const wchar_t* timelineTarget /*= L"/"*/)
{
	if (m_flashInterface)
		m_flashInterface->TCallFrame(_bstr_t(timelineTarget), frame);
}

//---------------------------------------------------------------------
std::wstring CFlashDXPlayer::GetCurrentLabel(const wchar_t* timelineTarget /*= L"/"*/)
{
	if (m_flashInterface)
		return std::wstring(m_flashInterface->TCurrentLabel(_bstr_t(timelineTarget)));
	return std::wstring();
}

//---------------------------------------------------------------------
void CFlashDXPlayer::GotoLabel(const wchar_t* label, const wchar_t* timelineTarget /*= L"/"*/)
{
	if (m_flashInterface)
		m_flashInterface->TGotoLabel(_bstr_t(timelineTarget), _bstr_t(label));
}

//---------------------------------------------------------------------
void CFlashDXPlayer::CallLabel(const wchar_t* label, const wchar_t* timelineTarget /*= L"/"*/)
{
	if (m_flashInterface)
		m_flashInterface->TCallLabel(_bstr_t(timelineTarget), _bstr_t(label));
}

//---------------------------------------------------------------------
std::wstring CFlashDXPlayer::GetVariable(const wchar_t* name)
{
	if (m_flashInterface)
		std::wstring(m_flashInterface->GetVariable(_bstr_t(name)));
	return std::wstring();
}

//---------------------------------------------------------------------
void CFlashDXPlayer::SetVariable(const wchar_t* name, const wchar_t* value)
{
	if (m_flashInterface)
		m_flashInterface->SetVariable(_bstr_t(name), _bstr_t(value));
}

//---------------------------------------------------------------------
std::wstring CFlashDXPlayer::GetProperty(int iProperty, const wchar_t* timelineTarget /*= L"/"*/)
{
	if (m_flashInterface)
		return std::wstring(m_flashInterface->TGetProperty(_bstr_t(timelineTarget), iProperty));
	return std::wstring();
}

//---------------------------------------------------------------------
double CFlashDXPlayer::GetPropertyAsNumber(int iProperty, const wchar_t* timelineTarget /*= L"/"*/)
{
	if (m_flashInterface)
		return m_flashInterface->TGetPropertyNum(_bstr_t(timelineTarget), iProperty);
	return 0;
}

//---------------------------------------------------------------------
void CFlashDXPlayer::SetProperty(int iProperty, const wchar_t* value, const wchar_t* timelineTarget /*= L"/"*/)
{
	if (m_flashInterface)
		m_flashInterface->TSetProperty(_bstr_t(timelineTarget), iProperty, _bstr_t(value));
}

//---------------------------------------------------------------------
void CFlashDXPlayer::SetProperty(int iProperty, double value, const wchar_t* timelineTarget /*= L"/"*/)
{
	if (m_flashInterface)
		m_flashInterface->TSetPropertyNum(_bstr_t(timelineTarget), iProperty, value);
}

//---------------------------------------------------------------------
void CFlashDXPlayer::ResizePlayer(unsigned int newWidth, unsigned int newHeight)
{
	IOleInPlaceObject* pInPlaceObject = NULL;
	m_oleObject->QueryInterface(__uuidof(IOleInPlaceObject), (LPVOID*) &pInPlaceObject);

	if (pInPlaceObject != NULL)
	{
		RECT rect = { 0, 0, newWidth, newHeight };
		pInPlaceObject->SetObjectRects(&rect, &rect);
		pInPlaceObject->Release();

		m_width = newWidth;
		m_height = newHeight;
	}
}

//---------------------------------------------------------------------
bool CFlashDXPlayer::IsNeedUpdate() const
{
	return m_dirtyFlag;
}

//---------------------------------------------------------------------
unsigned int CFlashDXPlayer::GetNumDirtyRects() const
{
	return (unsigned int)m_dirtyRects.size();
}

//---------------------------------------------------------------------
const RECT* CFlashDXPlayer::GetDirtyRect(unsigned int index) const
{
	return &m_dirtyRects[index];
}

//---------------------------------------------------------------------
RECT CFlashDXPlayer::GetDirtyRegionBox() const
{
	return m_dirtyRect;
}

//---------------------------------------------------------------------
void CFlashDXPlayer::DrawFrame(HDC dc)
{
	if (m_dirtyFlag)
	{
		IViewObject* pViewObject = NULL;
		m_flashInterface->QueryInterface(IID_IViewObject, (LPVOID*) &pViewObject);
		if (pViewObject != NULL)
		{
			// Combine regions
			HRGN unionRgn, first, second = NULL;
			unionRgn = CreateRectRgnIndirect(&m_dirtyRects[0]);
			if (m_dirtyRects.size() >= 2)
				second = CreateRectRgn(0, 0, 1, 1);

			for (std::vector<RECT>::iterator it = m_dirtyRects.begin() + 1; it != m_dirtyRects.end(); ++it)
			{
				// Fill combined region
				first = unionRgn;
				SetRectRgn(second, it->left, it->top, it->right, it->bottom);
				unionRgn = CreateRectRgn(0, 0, 1, 1);

				CombineRgn(unionRgn, first, second, RGN_OR);
				DeleteObject(first);
			}

			if (second)
				DeleteObject(second);

			// Set clip region
			SelectClipRgn(dc, unionRgn);

			// Fill background
			COLORREF fillColor = GetBackgroundColor();
			HBRUSH fillColorBrush = CreateSolidBrush(fillColor);
			FillRgn(dc, unionRgn, fillColorBrush);
			DeleteObject(fillColorBrush);

			DeleteObject(unionRgn);

			RECTL clipRect = { 0, 0, m_width, m_height };
			pViewObject->Draw(DVASPECT_TRANSPARENT, 1, NULL, NULL, NULL, dc, &clipRect, &clipRect, NULL, 0);

			pViewObject->Release();
		}

		m_dirtyFlag = false;
	}
}

//---------------------------------------------------------------------
WPARAM CFlashDXPlayer::CreateMouseWParam(WPARAM highWord)
{
	WPARAM result = highWord;
	result |= GetAsyncKeyState(VK_CONTROL) ? MK_CONTROL : 0;
	result |= GetAsyncKeyState(VK_SHIFT) ? MK_SHIFT : 0;
	result |= m_lastMouseButtons;
	return result;
}

//---------------------------------------------------------------------
void CFlashDXPlayer::SetMousePos(unsigned int x, unsigned int y)
{
	LRESULT lr;
	m_windowlessObject->OnWindowMessage(WM_MOUSEMOVE, CreateMouseWParam(0), MAKELPARAM(x, y), &lr);
	m_lastMouseX = x;
	m_lastMouseY = y;
}

//---------------------------------------------------------------------
void CFlashDXPlayer::SetMouseButtonState(unsigned int x, unsigned int y, EMouseButton button, bool pressed)
{
	m_lastMouseX = x;
	m_lastMouseY = y;

	LRESULT lr;
	switch (button)
	{
	case IFlashDXPlayer::eMouse1:
		if (pressed)
		{
			m_lastMouseButtons |= MK_LBUTTON;
			m_windowlessObject->OnWindowMessage(WM_LBUTTONDOWN, CreateMouseWParam(0), MAKELPARAM(m_lastMouseX, m_lastMouseY), &lr);
		}
		else
		{
			m_lastMouseButtons &= ~MK_LBUTTON;
			m_windowlessObject->OnWindowMessage(WM_LBUTTONUP, CreateMouseWParam(0), MAKELPARAM(m_lastMouseX, m_lastMouseY), &lr);
		}
		break;
	case IFlashDXPlayer::eMouse2:
		if (pressed)
		{
			m_lastMouseButtons |= MK_RBUTTON;
			m_windowlessObject->OnWindowMessage(WM_RBUTTONDOWN, CreateMouseWParam(0), MAKELPARAM(m_lastMouseX, m_lastMouseY), &lr);
		}
		else
		{
			m_lastMouseButtons &= ~MK_RBUTTON;
			m_windowlessObject->OnWindowMessage(WM_RBUTTONUP, CreateMouseWParam(0), MAKELPARAM(m_lastMouseX, m_lastMouseY), &lr);
		}
		break;
	case IFlashDXPlayer::eMouse3:
		if (pressed)
		{
			m_lastMouseButtons |= MK_MBUTTON;
			m_windowlessObject->OnWindowMessage(WM_MBUTTONDOWN, CreateMouseWParam(0), MAKELPARAM(m_lastMouseX, m_lastMouseY), &lr);
		}
		else
		{
			m_lastMouseButtons &= ~MK_MBUTTON;
			m_windowlessObject->OnWindowMessage(WM_MBUTTONUP, CreateMouseWParam(0), MAKELPARAM(m_lastMouseX, m_lastMouseY), &lr);
		}
		break;
	default:
		break;
	}
}

//---------------------------------------------------------------------
void CFlashDXPlayer::SendMouseWheel(int delta)
{
	LRESULT lr;
	m_windowlessObject->OnWindowMessage(WM_MOUSEWHEEL, CreateMouseWParam(MAKEWPARAM(0, delta)), MAKELPARAM(m_lastMouseX, m_lastMouseY), &lr);
}

//---------------------------------------------------------------------
void CFlashDXPlayer::SendKey(bool pressed, int virtualKey, int extended)
{
	LRESULT lr;
	if (pressed)
		m_windowlessObject->OnWindowMessage(WM_KEYDOWN, (WPARAM)virtualKey, (LPARAM)extended, &lr);
	else
		m_windowlessObject->OnWindowMessage(WM_KEYUP, (WPARAM)virtualKey, (LPARAM)extended, &lr);
}

//---------------------------------------------------------------------
void CFlashDXPlayer::SendChar(int character, int extended)
{
	LRESULT lr;
	m_windowlessObject->OnWindowMessage(WM_CHAR, (WPARAM)character, (LPARAM)extended, &lr);
}

//---------------------------------------------------------------------
void CFlashDXPlayer::SendCut()
{

}

//---------------------------------------------------------------------
void CFlashDXPlayer::SendCopy()
{

}

//---------------------------------------------------------------------
void CFlashDXPlayer::SendPaste()
{

}

//---------------------------------------------------------------------
void CFlashDXPlayer::SendSelectAll()
{

}

//---------------------------------------------------------------------
void CFlashDXPlayer::EnableSound(bool enable)
{

}

//---------------------------------------------------------------------
void CFlashDXPlayer::BeginFunctionCall(const wchar_t* functionName)
{

}

//---------------------------------------------------------------------
void CFlashDXPlayer::EndFunctionCall()
{

}

//---------------------------------------------------------------------
void CFlashDXPlayer::BeginReturn()
{

}

//---------------------------------------------------------------------
void CFlashDXPlayer::EndReturn()
{

}

//---------------------------------------------------------------------
void CFlashDXPlayer::PushArgumentString(const wchar_t* string)
{

}

//---------------------------------------------------------------------
void CFlashDXPlayer::PushArgumentBool(bool boolean)
{

}

//---------------------------------------------------------------------
void CFlashDXPlayer::PushArgumentNumber(float number)
{

}

//---------------------------------------------------------------------
void CFlashDXPlayer::AddEventHandler(struct IFlashDXEventHandler* pHandler)
{

}

//---------------------------------------------------------------------
void CFlashDXPlayer::RemoveEventHandler(struct IFlashDXEventHandler* pHandler)
{

}

//---------------------------------------------------------------------
struct IFlashDXEventHandler* CFlashDXPlayer::GetEventHandlerByIndex(unsigned int index)
{
	return NULL;
}

//---------------------------------------------------------------------
unsigned int CFlashDXPlayer::GetNumEventHandlers() const
{
	return 0;
}