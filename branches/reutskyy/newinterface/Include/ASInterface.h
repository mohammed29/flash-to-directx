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

struct ASValue
{
	typedef bool Boolean;
	typedef float Number;
	typedef std::wstring String;
	typedef std::vector<ASValue> Array;
	typedef std::map<std::wstring, ASValue>	Object;

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

private:
	struct _Data; _Data &data;
};

struct ASCallback
{
	inline ASCallback();
	template<typename _Function> inline ASCallback(const _Function &value);
	inline ASCallback(const ASCallback &value);
	inline ~ASCallback();
	inline ASCallback& operator = (const ASCallback &value);

	HRESULT Call(const ASValue::Array &arguments, ASValue &returnValue) const;

private:
	struct _Data; _Data &data;
};

struct ASInterface : IFlashDXEventHandler
{
	inline ASInterface(IFlashDXPlayer *pPlayer);
	inline ~ASInterface();

	inline ASValue Call(const std::wstring &functionName,
		   const ASValue &arg0 = ASValue(), const ASValue &arg1 = ASValue(), const ASValue &arg2 = ASValue(), const ASValue &arg3 = ASValue(), const ASValue &arg4 = ASValue(),
		   const ASValue &arg5 = ASValue(), const ASValue &arg6 = ASValue(), const ASValue &arg7 = ASValue(), const ASValue &arg8 = ASValue(), const ASValue &arg9 = ASValue());

	inline void AddCallback(const std::wstring &functionName, const ASCallback &callback);

private:
	IFlashDXPlayer &m_player;
	typedef std::map<std::wstring, ASCallback> Callbacks;
	Callbacks m_callbacks;
	HRESULT FlashCall(const wchar_t* request);
	HRESULT FSCommand(const wchar_t* command, const wchar_t* args);
};








// Inlines -------------------------------------------------------------------------------------------------------

#include <sstream>

// ASValue::_Data

struct ASValue::_Data
{
	int type;
	union {
		char b[sizeof(Boolean)]; char n[sizeof(Number)]; char s[sizeof(String)]; char a[sizeof(Array)]; char o[sizeof(Object)];
	} data;

	inline _Data()
	:
		type(-1)
	{}
	inline ~_Data()
	{
		Destruct();
	}

	inline void Construct(int newtype, const void* value)
	{
		Destruct();
		switch (newtype)
		{
		case 0: new (&data) Boolean(*(Boolean*)value); break;
		case 1: new (&data) Number(*(Number*)value); break;
		case 2: new (&data) String(*(String*)value); break;
		case 3: new (&data) Array(*(Array*)value); break;
		case 4: new (&data) Object(*(Object*)value); break;
		}
		type = newtype;
	}
	inline void Destruct()
	{
		switch (type)
		{
		case 0: reinterpret_cast<Boolean*>(&data)->~Boolean(); break;
		case 1: reinterpret_cast<Number*>(&data)->~Number(); break;
		case 2: reinterpret_cast<String*>(&data)->~String(); break;
		case 3: reinterpret_cast<Array*>(&data)->~Array(); break;
		case 4: reinterpret_cast<Object*>(&data)->~Object(); break;
		}
		type = -1;
	}

	inline _Data& operator = (Boolean value)
	{
		Construct(0, &value);
		return *this;
	}
	inline _Data& operator = (Number value)
	{
		Construct(1, &value);
		return *this;
	}
	inline _Data& operator = (double value)
	{
		return *this = (Number)value;
	}
	inline _Data& operator = (int value)
	{
		return *this = (Number)value;
	}
	inline _Data& operator = (unsigned int value)
	{
		return *this = (Number)value;
	}
	inline _Data& operator = (const String &value)
	{
		Construct(2, &value);
		return *this;
	}
	inline _Data& operator = (const std::string &value)
	{
		std::wstringstream s; s << value.c_str();
		return *this = s.str();
	}
	inline _Data& operator = (const wchar_t* value)
	{
		std::wstringstream s; s << value;
		return *this = s.str();
	}
	inline _Data& operator = (const char* value)
	{
		std::wstringstream s; s << value;
		return *this = s.str();
	}
	inline _Data& operator = (const Array &value)
	{
		Construct(3, &value);
		return *this;
	}
	inline _Data& operator = (const Object &value)
	{
		Construct(4, &value);
		return *this;
	}
	inline _Data& operator = (const ASValue &value)
	{
		Construct(value.data.type, &value.data.data);
		return *this;
	}

	inline operator Boolean() const
	{
		assert(type == 0);
		return (Boolean&)data;
	}
	inline operator Number() const
	{
		assert(type == 1);
		return (Number&)data;
	}
	inline operator String() const
	{
		assert(type == 2);
		return (String&)data;
	}
	inline operator Array() const
	{
		assert(type == 3);
		return (Array&)data;
	}
	inline operator Object() const
	{
		assert(type == 4);
		return (Object&)data;
	}
};

// ASValue

inline ASValue::ASValue()
:
	data(* new _Data)
{}
inline ASValue::ASValue(const ASValue &value)
:
	data(* new _Data)
{
	data = value;
}
template<typename _Type>
inline ASValue::ASValue(const _Type &value)
:
	data(* new _Data)
{
	data = value;
}
template<typename _Type>
inline ASValue& ASValue::operator = (const _Type &value)
{
	data = value;
	return *this;
}
template<typename _Type>
inline ASValue::operator _Type() const
{
	return data;
}
inline ASValue::~ASValue()
{
	delete &data;
}

inline bool ASValue::IsEmpty() const
{
	return data.type == -1;
}
inline bool ASValue::IsBoolean() const
{
	return data.type == 0;
}
inline bool ASValue::IsNumber() const
{
	return data.type == 1;
}
inline bool ASValue::IsString() const
{
	return data.type == 2;
}
inline bool ASValue::IsArray() const
{
	return data.type == 3;
}
inline bool ASValue::IsObject() const
{
	return data.type == 4;
}

inline std::wstring ASValue::ToXML() const
{
	struct _Array { static std::wstring ToXML(const Array &a)
	{
		std::wstringstream s;
		s << "<array>";
		for (unsigned int i = 0, e = a.size(); i < e; ++i)
		{
			s << L"<property id='" << i << L"'>" << a[i].ToXML() << L"</property>";
		}
		s << "</array>";
		return s.str();
	}};

	struct _Object { static std::wstring ToXML(const Object &o)
	{
		std::wstringstream s;
		s << "<object>";
		for (Object::const_iterator i = o.begin(), e = o.end(); i != e; ++i)
		{
			s << "<property id='" << i->first << "'>" << i->second.ToXML() << "</property>";
		}
		s << "</object>";
		return s.str();
	}};

	std::wstringstream s;

	switch (data.type)
	{
	case 0: s << ((Boolean&)data.data ? L"<true/>" : L"<false/>"); break;
	case 1: s << L"<number>" << (Number&)data.data << L"</number>"; break;
	case 2: s << L"<string>" << (String&)data.data << L"</string>"; break;
	case 3: s << _Array::ToXML((Array&)data.data); break;
	case 4: s << _Object::ToXML((Object&)data.data); break;
	default: s << L"<null/>"; break;
	}

	return s.str();
}
inline void ASValue::FromXML(const std::wstring &xml)
{
	struct _Props { static void parseFirst(const std::wstring &xml, std::wstring &name, std::wstring &content, size_t &length)
	{
		std::wstring propOpen = xml.substr(0, xml.find(L">") + 1);
		name = propOpen.substr(14, propOpen.length() - 14 - 2);
		const wchar_t* propClose = xml.c_str() + propOpen.length();
		int nesting = 0;
		while (propClose - xml.c_str() < (int)xml.length())
		{
			if (wcsncmp(propClose, L"<property id=", 13) == 0) ++nesting;
			else
			if (wcsncmp(propClose, L"</property>", 10) == 0)
			{
				if (nesting != 0) --nesting;
				else break;
			}
			++propClose;
		}
		content = xml.substr(propOpen.length(), propClose - xml.c_str() - propOpen.length());
		length = propOpen.length() + content.length() + 11;
	}};

	if (xml.find(L"<true/>") == 0) data = true;
	else
	if (xml.find(L"<false/>") == 0) data = false;
	else
	if (xml.find(L"<number>") == 0)
	{
		std::wstringstream s(xml.substr(8));
		Number value; s >> value;
		data = value;
	}
	else
	if (xml.find(L"<string>") == 0)
	{
		String value = xml.substr(8, xml.rfind(L"</string>") - 8);
		data = value;
	}
	else
	if (xml.find(L"<array>") == 0)
	{
		Array value;
		std::wstring propsXML = xml.substr(7, xml.rfind(L"</array>") - 7);
		while (!propsXML.empty())
		{
			std::wstring propName, propContent; size_t propLength;
			_Props::parseFirst(propsXML, propName, propContent, propLength);
			size_t propIndex; swscanf_s(propName.c_str(), L"%d", &propIndex);
			ASValue prop; prop.FromXML(propContent);
			if (propIndex >= value.size()) value.resize(propIndex + 1);
			value[propIndex] = prop;
			propsXML = propsXML.substr(propLength);
		}
		data = value;
	}
	else
	if (xml.find(L"<object>") == 0)
	{
		Object value;
		std::wstring propsXML = xml.substr(8, xml.rfind(L"</object>") - 8);
		while (!propsXML.empty())
		{
			std::wstring propName, propContent; size_t propLength;
			_Props::parseFirst(propsXML, propName, propContent, propLength);
			ASValue prop; prop.FromXML(propContent);
			value[propName] = prop;
			propsXML = propsXML.substr(propLength);
		}
		data = value;
	}
}

// ASCallback::_Data

struct ASCallback::_Data
{
};

// ASCallback

inline ASCallback::ASCallback()
:
	data(* new _Data)
{}
template<typename _Function>
inline ASCallback::ASCallback(const _Function &value)
:
	data(* new _Data)
{}
inline ASCallback::ASCallback(const ASCallback &value)
:
	data(* new _Data)
{}
inline ASCallback::~ASCallback()
{
	delete &data;
}
inline ASCallback& ASCallback::operator = (const ASCallback &value)
{
	return *this;
}
inline HRESULT ASCallback::Call(const ASValue::Array &arguments, ASValue &returnValue) const
{
	return E_NOTIMPL;
}

// ASInterface

inline ASInterface::ASInterface(IFlashDXPlayer *pPlayer)
:
	m_player(*pPlayer)
{
	m_player.AddEventHandler(this);
}

inline ASInterface::~ASInterface()
{
	m_player.RemoveEventHandler(this);
}

inline ASValue ASInterface::Call(const std::wstring &functionName, const ASValue &arg0, const ASValue &arg1, const ASValue &arg2, const ASValue &arg3, const ASValue &arg4, const ASValue &arg5, const ASValue &arg6, const ASValue &arg7, const ASValue &arg8, const ASValue &arg9)
{
	struct _Args { static void ToXML(std::wstring &arguments, const ASValue &arg0 = ASValue(), const ASValue &arg1 = ASValue(), const ASValue &arg2 = ASValue(), const ASValue &arg3 = ASValue(), const ASValue &arg4 = ASValue(), const ASValue &arg5 = ASValue(), const ASValue &arg6 = ASValue(), const ASValue &arg7 = ASValue(), const ASValue &arg8 = ASValue(), const ASValue &arg9 = ASValue())
	{
		if (arg0.IsEmpty()) return;

		arguments += arg0.ToXML();

		ToXML(arguments, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
	}};

	std::wstring arguments(L"<arguments>");
	_Args::ToXML(arguments, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
	if (arguments.length() > 11) arguments += std::wstring(L"</arguments>");
	else arguments = L"";

	std::wstring request = L"<invoke name='" + functionName + L"' returntype='xml'>" + arguments + L"</invoke>";
	const wchar_t* result = m_player.CallFunction(request.c_str());

	if (result == NULL) return ASValue();

	ASValue value; value.FromXML(result);

	return value;
}

inline void ASInterface::AddCallback(const std::wstring &functionName, const ASCallback &callback)
{
	m_callbacks[functionName] = callback;
}

HRESULT ASInterface::FlashCall(const wchar_t* request)
{
	struct _Args { static void split(const std::wstring &xml, std::vector<std::wstring> &args)
	{
		const wchar_t* argStart = xml.c_str();
		const wchar_t* argEnd = xml.c_str();
		int nesting = 0;
		bool closetag = false;
		while (argEnd - xml.c_str() < (int)xml.length())
		{
			if (wcsncmp(argEnd, L"</arguments>", 12) == 0) break;
			else
			if (wcsncmp(argEnd, L"<arguments>", 11) == 0)
			{
				nesting = 0;
				argStart += 11;
				argEnd += 10;
			}
			else
			if (wcsncmp(argEnd, L"</", 2) == 0) closetag = true;
			else
			if (wcsncmp(argEnd, L"<", 1) == 0) ++nesting;
			else
			if (wcsncmp(argEnd, L"/>", 2) == 0) closetag = true;
			else
			if (wcsncmp(argEnd, L">", 1) == 0)
			{
				if (closetag)
				{
					closetag = false;
					if (--nesting == 0)
					{
						size_t start = argStart - xml.c_str();
						size_t size = argEnd - argStart + 1;
						args.push_back(xml.substr(start, size));
						argStart = argEnd + 1;
					}
				}
			}
			++argEnd;
		}
	}};

	std::wstring xml = request;
	std::wstring functionName = xml.substr(14, xml.find(L">") - 14 - 18);
	Callbacks::iterator itCallback = m_callbacks.find(functionName);
	if (itCallback != m_callbacks.end())
	{
		std::vector<std::wstring> args;
		_Args::split(xml.substr(xml.find(L">") + 1), args);
		ASValue::Array arguments;
		for (unsigned int i = 0, s = args.size(); i < s; ++i)
		{
			ASValue arg; arg.FromXML(args[i]);
			arguments.push_back(arg);
		}
		ASValue returnValue;
		HRESULT result = itCallback->second.Call(arguments, returnValue);
		if (result == NOERROR) m_player.SetReturnValue(returnValue.ToXML().c_str());
		return result;
	}

	return E_NOTIMPL;
}

HRESULT ASInterface::FSCommand(const wchar_t* command, const wchar_t* args)
{
	return E_NOTIMPL;
}
