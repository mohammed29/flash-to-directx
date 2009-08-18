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

//---------------------------------------------------------------------
/// @brief				Returns pointer on instance of Flash-to-DirectX.
extern struct IFlashDX* GetFlashToDirectXInstance();

//---------------------------------------------------------------------
/// Flash-to-DirectX interface.
//---------------------------------------------------------------------
struct IFlashDX
{
	//---------------------------------------------------------------------
	/// @brief				Creates flash player. Can be called multiple times to create different players.
	/// @param width		Width of rendering surface of the player.
	/// @param height		Width of rendering surface of the player.
	/// @return				Player interface.
	virtual struct IFlashDXPlayer* CreatePlayer(unsigned int width, unsigned int height) = 0;

	//---------------------------------------------------------------------
	/// @brief				Destroys player by it's pointer.
	/// @param pPlayer		Player to destroy.
	virtual void DestroyPlayer(IFlashDXPlayer* pPlayer) = 0;

	//---------------------------------------------------------------------
	/// Movie properties description.
	struct SMovieProperties
	{
		unsigned int	m_width;
		unsigned int	m_height;
		unsigned int	m_fps;
		unsigned int	m_numFrames;
	};

	//---------------------------------------------------------------------
	/// @brief				Returns movie properties by path to movie file.
	/// @param movie		Path to movie file.
	/// @param props		Returned properties in case of success.
	/// @return				Success flag.
	virtual bool GetMovieProperties(const wchar_t* movie, SMovieProperties& props) = 0;

	//---------------------------------------------------------------------
	/// @brief				Returns movie properties by movie data.
	/// @param movieData	Pointer on movie data.
	/// @param movieDataSize Movie data size.
	/// @param props		Returned properties in case of success.
	/// @return				Success flag.
	virtual bool GetMovieProperties(const void* movieData, const unsigned int movieDataSize, SMovieProperties& props) = 0;
};


//---------------------------------------------------------------------
/// Flash player interface.
//---------------------------------------------------------------------
struct IFlashDXPlayer
{
	//---------------------------------------------------------------------
	/// @brief				REPLACE_ME
	/// @param data	REPLACE_ME
	/// @return				void
	virtual void SetUserData(intptr_t data) = 0;

	//---------------------------------------------------------------------
	/// @brief				REPLACE_ME
	/// @return				intptr_t
	virtual intptr_t GetUserData() const = 0;

	//---------------------------------------------------------------------
	/// @brief				REPLACE_ME
	/// @param movie	REPLACE_ME
	/// @return				bool
	virtual bool LoadMovie(const wchar_t* movie) = 0;

	//---------------------------------------------------------------------
	/// @brief				REPLACE_ME
	/// @return				void
	virtual void StartPlaying() = 0;

	//---------------------------------------------------------------------
	/// @brief				REPLACE_ME
	/// @return				void
	virtual void StopPlaying() = 0;

	//---------------------------------------------------------------------
	/// @brief				REPLACE_ME
	/// @return				void
	virtual void Rewind() = 0;

	//---------------------------------------------------------------------
	/// @brief				REPLACE_ME
	/// @param newWidth	REPLACE_ME
	/// @param newHeight	REPLACE_ME
	/// @return				void
	virtual void ResizePlayer(unsigned int newWidth, unsigned int newHeight) = 0;

	//---------------------------------------------------------------------
	/// @brief				REPLACE_ME
	/// @return				bool
	virtual bool IsNeedUpdate() const = 0;

	//---------------------------------------------------------------------
	/// @brief				REPLACE_ME
	/// @param dc	REPLACE_ME
	/// @return				void
	virtual void DrawFrame(HDC dc) = 0;

	//---------------------------------------------------------------------
	/// @brief				REPLACE_ME
	/// @param x	REPLACE_ME
	/// @param y	REPLACE_ME
	/// @return				void
	virtual void SetMousePos(unsigned int x, unsigned int y) = 0;

	//---------------------------------------------------------------------
	enum EMouseButton
	{
		eMouse1 = 0,	// left mouse button
		eMouse2,		// right mouse button
		eMouse3,		// middle (wheel) mouse button
		eMouse4,
		eMouse5,
		eMouse6,
		eMouse7,
		eMouse8
	};

	//---------------------------------------------------------------------
	/// @brief				REPLACE_ME
	/// @param button	REPLACE_ME
	/// @param pressed	REPLACE_ME
	/// @return				void
	virtual void SetMouseButtonState(EMouseButton button, bool pressed) = 0;

	//---------------------------------------------------------------------
	/// @brief				REPLACE_ME
	/// @param delta	REPLACE_ME
	/// @return				void
	virtual void SendMouseWheel(int delta) = 0;

	//---------------------------------------------------------------------
	/// @brief				REPLACE_ME
	/// @param pressed	REPLACE_ME
	/// @param virtualKey	REPLACE_ME
	/// @param extended	REPLACE_ME
	/// @return				void
	virtual void SendKey(bool pressed, int virtualKey, int extended) = 0;

	//---------------------------------------------------------------------
	/// @brief				REPLACE_ME
	/// @param character	REPLACE_ME
	/// @param extended	REPLACE_ME
	/// @return				void
	virtual void SendChar(int character, int extended) = 0;

	//---------------------------------------------------------------------
	/// @brief				REPLACE_ME
	/// @return				void
	virtual void SendCut() = 0;

	//---------------------------------------------------------------------
	/// @brief				REPLACE_ME
	/// @return				void
	virtual void SendCopy() = 0;

	//---------------------------------------------------------------------
	/// @brief				REPLACE_ME
	/// @return				void
	virtual void SendPaste() = 0;

	//---------------------------------------------------------------------
	/// @brief				REPLACE_ME
	/// @return				void
	virtual void SendSelectAll() = 0;

	//---------------------------------------------------------------------
	/// @brief				REPLACE_ME
	/// @param enable	REPLACE_ME
	/// @return				void
	virtual void EnableSound(bool enable) = 0;

	//---------------------------------------------------------------------
	/// @brief				REPLACE_ME
	/// @param functionName	REPLACE_ME
	/// @return				void
	virtual void BeginFunctionCall(const wchar_t* functionName) = 0;

	//---------------------------------------------------------------------
	/// @brief				REPLACE_ME
	/// @return				void
	virtual void EndFunctionCall() = 0;

	//---------------------------------------------------------------------
	/// @brief				REPLACE_ME
	/// @return				void
	virtual void BeginReturn() = 0;

	//---------------------------------------------------------------------
	/// @brief				REPLACE_ME
	/// @return				void
	virtual void EndReturn() = 0;

	//---------------------------------------------------------------------
	/// @brief				REPLACE_ME
	/// @param string	REPLACE_ME
	/// @return				void
	virtual void PushArgumentString(const wchar_t* string) = 0;

	//---------------------------------------------------------------------
	/// @brief				REPLACE_ME
	/// @param boolean	REPLACE_ME
	/// @return				void
	virtual void PushArgumentBool(bool boolean) = 0;

	//---------------------------------------------------------------------
	/// @brief				REPLACE_ME
	/// @param number	REPLACE_ME
	/// @return				void
	virtual void PushArgumentNumber(float number) = 0;

	//---------------------------------------------------------------------
	/// @brief				Adds flash 
	/// @param pHandler	REPLACE_ME
	/// @return				void
	virtual void AddEventHandler(struct IFlashDXEventHandler* pHandler) = 0;

	//---------------------------------------------------------------------
	/// @brief				REPLACE_ME
	/// @param pHandler	REPLACE_ME
	/// @return				void
	virtual void RemoveEventHandler(struct IFlashDXEventHandler* pHandler) = 0;

	//---------------------------------------------------------------------
	/// @brief				REPLACE_ME
	/// @param index	REPLACE_ME
	/// @return				struct IFlashDXEventHandler*
	virtual struct IFlashDXEventHandler* GetEventHandlerByIndex(unsigned int index) = 0;

	//---------------------------------------------------------------------
	/// @brief				REPLACE_ME
	/// @return				unsigned int
	virtual unsigned int GetNumEventHandlers() const = 0;
};


//---------------------------------------------------------------------
/// Flash event handler interface.
//---------------------------------------------------------------------
struct IFlashDXEventHandler
{
    enum EEventType
    {
        
    };
};