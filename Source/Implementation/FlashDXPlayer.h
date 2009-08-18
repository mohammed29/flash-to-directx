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

//---------------------------------------------------------------------
/// Implementation of IFlashDXPlayer interface.
//---------------------------------------------------------------------
class CFlashDXPlayer : public IFlashDXPlayer
{
public:
	//---------------------------------------------------------------------
	/// Constructor.
	CFlashDXPlayer();

	//---------------------------------------------------------------------
	/// Destructor.
	~CFlashDXPlayer();

	//---------------------------------------------------------------------
	// IFlashDXPlayer implementations.
	//---------------------------------------------------------------------
	virtual void SetUserData(intptr_t data);
	virtual intptr_t GetUserData() const;
	virtual bool LoadMovie(const wchar_t* movie);
	virtual void StartPlaying();
	virtual void StopPlaying();
	virtual void Rewind();
	virtual void ResizePlayer(unsigned int newWidth, unsigned int newHeight);
	virtual bool IsNeedUpdate() const;
	virtual void DrawFrame(HDC dc);
	virtual void SetMousePos(unsigned int x, unsigned int y);
	virtual void SetMouseButtonState(EMouseButton button, bool pressed);
	virtual void SendMouseWheel(int delta);
	virtual void SendKey(bool pressed, int virtualKey, int extended);
	virtual void SendChar(int character, int extended);
	virtual void SendCut();
	virtual void SendCopy();
	virtual void SendPaste();
	virtual void SendSelectAll();
	virtual void EnableSound(bool enable);
	virtual void BeginFunctionCall(const wchar_t* functionName);
	virtual void EndFunctionCall();
	virtual void BeginReturn();
	virtual void EndReturn();
	virtual void PushArgumentString(const wchar_t* string);
	virtual void PushArgumentBool(bool boolean);
	virtual void PushArgumentNumber(float number);
	virtual void AddEventHandler(struct IFlashDXEventHandler* pHandler);
	virtual void RemoveEventHandler(struct IFlashDXEventHandler* pHandler);
	virtual struct IFlashDXEventHandler* GetEventHandlerByIndex(unsigned int index);
	virtual unsigned int GetNumEventHandlers() const;

protected:
};