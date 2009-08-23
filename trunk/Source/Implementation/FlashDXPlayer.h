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

#pragma once

#include "../../Include/IFlashDX.h"
#include "ControlSite.h"
#include "FlashSink.h"

//---------------------------------------------------------------------
/// Implementation of IFlashDXPlayer interface.
//---------------------------------------------------------------------
class CFlashDXPlayer : public IFlashDXPlayer
{
public:
	//---------------------------------------------------------------------
	/// Constructor.
	CFlashDXPlayer(HMODULE flashDLL, unsigned int width, unsigned int height);

	//---------------------------------------------------------------------
	/// Destructor.
	virtual ~CFlashDXPlayer();

	//---------------------------------------------------------------------
	/// Adds dirty rectangle.
	void AddDirtyRect(const RECT* pRect);

	//---------------------------------------------------------------------
	// IFlashDXPlayer implementations.
	//---------------------------------------------------------------------
	virtual void SetUserData(intptr_t data);
	virtual intptr_t GetUserData() const;
	virtual EState GetState() const;
	virtual void SetQuality(EQuality quality);
	virtual void SetTransparencyMode(ETransparencyMode mode);
	virtual bool LoadMovie(const wchar_t* movie);
	virtual COLORREF GetBackgroundColor();
	virtual void SetBackgroundColor(COLORREF color);
	virtual void StartPlaying();
	virtual void StartPlaying(const wchar_t* timelineTarget);
	virtual void StopPlaying();
	virtual void StopPlaying(const wchar_t* timelineTarget);
	virtual void Rewind();
	virtual void StepForward();
	virtual void StepBack();
	virtual int GetCurrentFrame();
	virtual int GetCurrentFrame(const wchar_t* timelineTarget = L"/");
	virtual void GotoFrame(int frame);
	virtual void GotoFrame(int frame, const wchar_t* timelineTarget);
	virtual void CallFrame(int frame, const wchar_t* timelineTarget = L"/");
	virtual std::wstring GetCurrentLabel(const wchar_t* timelineTarget = L"/");
	virtual void GotoLabel(const wchar_t* label, const wchar_t* timelineTarget = L"/");
	virtual void CallLabel(const wchar_t* label, const wchar_t* timelineTarget = L"/");
	virtual std::wstring GetVariable(const wchar_t* name);
	virtual void SetVariable(const wchar_t* name, const wchar_t* value);
	virtual std::wstring GetProperty(int iProperty, const wchar_t* timelineTarget = L"/");
	virtual double GetPropertyAsNumber(int iProperty, const wchar_t* timelineTarget = L"/");
	virtual void SetProperty(int iProperty, const wchar_t* value, const wchar_t* timelineTarget = L"/");
	virtual void SetProperty(int iProperty, double value, const wchar_t* timelineTarget = L"/");
	virtual void ResizePlayer(unsigned int newWidth, unsigned int newHeight);
	virtual bool IsNeedUpdate() const;
	virtual unsigned int GetNumDirtyRects() const;
	virtual const RECT* GetDirtyRect(unsigned int index) const;
	virtual RECT GetDirtyRegionBox() const;
	virtual void DrawFrame(HDC dc);
	virtual void SetMousePos(unsigned int x, unsigned int y);
	virtual void SetMouseButtonState(unsigned int x, unsigned int y, EMouseButton button, bool pressed);
	virtual void SendMouseWheel(int delta);
	virtual void SendKey(bool pressed, int virtualKey, int extended);
	virtual void SendChar(int character, int extended);
	virtual void EnableSound(bool enable);
	virtual void BeginFunctionCall(const wchar_t* functionName);
	virtual std::wstring EndFunctionCall();
	virtual void BeginReturn();
	virtual void EndReturn();
	virtual void PushArgumentString(const wchar_t* string);
	virtual void PushArgumentBool(bool boolean);
	virtual void PushArgumentNumber(float number);
	virtual std::wstring CallFunction(const wchar_t* functionName,
		Arg arg0 = Arg(), Arg arg1 = Arg(), Arg arg2 = Arg(), Arg arg3 = Arg(), Arg arg4 = Arg(),
		Arg arg5 = Arg(), Arg arg6 = Arg(), Arg arg7 = Arg(), Arg arg8 = Arg(), Arg arg9 = Arg()
		);
	virtual void AddEventHandler(struct IFlashDXEventHandler* pHandler);
	virtual void RemoveEventHandler(struct IFlashDXEventHandler* pHandler);
	virtual struct IFlashDXEventHandler* GetEventHandlerByIndex(unsigned int index);
	virtual unsigned int GetNumEventHandlers() const;

protected:
	//---------------------------------------------------------------------
	WPARAM CreateMouseWParam(WPARAM highWord);

public:
	unsigned int			m_width;
	unsigned int			m_height;

	ShockwaveFlashObjects::IShockwaveFlash* m_flashInterface;

protected:
	CControlSite			m_controlSite;
	CFlashSink				m_flashSink;
	IOleObject*				m_oleObject;
	IOleInPlaceObjectWindowless* m_windowlessObject;

	RECT					m_dirtyRect;
	std::vector<RECT>		m_dirtyRects;
	bool					m_dirtyFlag;

	intptr_t				m_userData;

	unsigned int			m_lastMouseX;
	unsigned int			m_lastMouseY;
	intptr_t				m_lastMouseButtons;

	std::wstring			m_invokeString;
};