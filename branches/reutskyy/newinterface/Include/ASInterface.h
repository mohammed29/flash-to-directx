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

#include "IFlashDX.h"

#include <string>
#include <vector>
#include <map>

//---------------------------------------------------------------------
/// Action Script value. To exchange data with AS.
//---------------------------------------------------------------------
struct ASValue
{
	inline ASValue();
	inline ASValue(const ASValue &value);
	template<typename _Type> inline ASValue(const _Type &value);
	template<typename _Type> inline ASValue& operator = (const _Type &value);
	template<typename _Type> inline operator _Type() const;
	inline ~ASValue();

	inline bool IsEmpty() const;
	inline bool IsBoolean() const;
	inline bool IsNumber() const;
	inline bool IsString() const;
	inline bool IsArray() const;
	inline bool IsObject() const;

	inline std::wstring ToXML() const;
	inline void FromXML(const std::wstring &xml);

	// AS data types
	typedef bool Boolean;
	typedef float Number;
	typedef std::wstring String;
	typedef std::vector<ASValue> Array;
	typedef std::map<std::wstring, ASValue>	Object;

private:
	struct _Data; _Data &m_data;
};

//---------------------------------------------------------------------
/// Action Script interface.
/// Allows to call AS function and register callbacks called from AS.
//---------------------------------------------------------------------
struct ASInterface
{
	inline ASInterface(IFlashDXPlayer *pPlayer);
	inline ~ASInterface();

	// Call an AS function with up to 10 arguments
	inline ASValue Call(const std::wstring &functionName,
						const ASValue &arg0 = ASValue(), const ASValue &arg1 = ASValue(),
						const ASValue &arg2 = ASValue(), const ASValue &arg3 = ASValue(),
						const ASValue &arg4 = ASValue(), const ASValue &arg5 = ASValue(),
						const ASValue &arg6 = ASValue(), const ASValue &arg7 = ASValue(),
						const ASValue &arg8 = ASValue(), const ASValue &arg9 = ASValue());

	// Register a function as an AS callback
	template<typename _Function>
	inline void AddCallback(const std::wstring &functionName, _Function function);

	// Register a method as an AS callback
	template<typename _Object, typename _Method>
	inline void AddCallback(const std::wstring &functionName, _Object &object, _Method method);

	// Register a function as a fscommand callback
	inline void AddFSCommandCallback(const std::wstring &command, void (*function)(const wchar_t* args));

	// Register a method as a fscommand callback
	template<typename _Object>
	inline void AddFSCommandCallback(const std::wstring &command, _Object &object, void (_Object::*method)(const wchar_t* args));

	// Register a function as the default fscommand callback
	inline void DefFSCommandCallback(void (*function)(const wchar_t* command, const wchar_t* args));

	// Register a method as the default fscommand callback
	template<typename _Object>
	inline void DefFSCommandCallback(_Object &object, void (_Object::*method)(const wchar_t* command, const wchar_t* args));

private:
	struct _Data; _Data &m_data;
};

#include "ASInterface.inl"