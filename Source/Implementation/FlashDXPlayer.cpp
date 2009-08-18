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

//---------------------------------------------------------------------
CFlashDXPlayer::CFlashDXPlayer()
{

}

//---------------------------------------------------------------------
CFlashDXPlayer::~CFlashDXPlayer()
{

}

//---------------------------------------------------------------------
void CFlashDXPlayer::SetUserData(intptr_t data)
{

}

//---------------------------------------------------------------------
intptr_t CFlashDXPlayer::GetUserData() const
{
	return 0;
}

//---------------------------------------------------------------------
bool CFlashDXPlayer::LoadMovie(const wchar_t* movie)
{
	return false;
}

//---------------------------------------------------------------------
void CFlashDXPlayer::StartPlaying()
{

}

//---------------------------------------------------------------------
void CFlashDXPlayer::StopPlaying()
{

}

//---------------------------------------------------------------------
void CFlashDXPlayer::Rewind()
{

}

//---------------------------------------------------------------------
void CFlashDXPlayer::ResizePlayer(unsigned int newWidth, unsigned int newHeight)
{

}

//---------------------------------------------------------------------
void CFlashDXPlayer::DrawFrame(HDC dc)
{

}

//---------------------------------------------------------------------
void CFlashDXPlayer::SetMousePos(unsigned int x, unsigned int y)
{

}

//---------------------------------------------------------------------
void CFlashDXPlayer::SetMouseButtonState(EMouseButton button, bool pressed)
{

}

//---------------------------------------------------------------------
void CFlashDXPlayer::SendMouseWheel(int delta)
{

}

//---------------------------------------------------------------------
void CFlashDXPlayer::SendKey(bool pressed, int virtualKey, int extended)
{

}

//---------------------------------------------------------------------
void CFlashDXPlayer::SendChar(int character, int extended)
{

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