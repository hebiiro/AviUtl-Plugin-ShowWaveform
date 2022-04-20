#pragma once

//---------------------------------------------------------------------
// Get

inline HRESULT WINAPI getPrivateProfileBSTR(LPCWSTR fileName, LPCWSTR appName, LPCWSTR keyName, _bstr_t& outValue)
{
	WCHAR buffer[MAX_PATH] = {};
	DWORD result = ::GetPrivateProfileStringW(appName, keyName, L"", buffer, MAX_PATH, fileName);
	if (!result) return S_FALSE;
	outValue = buffer;
	return S_OK;
}

template<class T>
inline HRESULT WINAPI getPrivateProfileInt(LPCWSTR fileName, LPCWSTR appName, LPCWSTR keyName, T& outValue, int radix = 0)
{
	_bstr_t value = L"";
	HRESULT hr = getPrivateProfileBSTR(fileName, appName, keyName, value);
	if (hr != S_OK) return hr;
	if (!(BSTR)value) return S_FALSE;
	outValue = (T)wcstol(value, 0, radix);
	return S_OK;
}

template<class T>
inline HRESULT WINAPI getPrivateProfileReal(LPCWSTR fileName, LPCWSTR appName, LPCWSTR keyName, T& outValue)
{
	_bstr_t value = L"";
	HRESULT hr = getPrivateProfileBSTR(fileName, appName, keyName, value);
	if (hr != S_OK) return hr;
	if (!(BSTR)value) return S_FALSE;
	outValue = (T)wcstod(value, 0);
	return S_OK;
}

template<class T>
inline HRESULT WINAPI getPrivateProfileBool(LPCWSTR fileName, LPCWSTR appName, LPCWSTR keyName, T& outValue)
{
	_bstr_t value = L"";
	HRESULT hr = getPrivateProfileBSTR(fileName, appName, keyName, value);
	if (hr != S_OK) return hr;
	if (!(BSTR)value) return S_FALSE;
	if (::lstrlenW(value) == 0) return S_FALSE;

	if (::StrCmpIW(value, L"ON") == 0)
	{
		outValue = true;
	}
	else if (::StrCmpIW(value, L"OFF") == 0)
	{
		outValue = false;
	}
	else if (::StrCmpIW(value, L"YES") == 0)
	{
		outValue = true;
	}
	else if (::StrCmpIW(value, L"NO") == 0)
	{
		outValue = false;
	}
	else
	{
		outValue = !!wcstol(value, 0, 0);
	}
}

inline HRESULT WINAPI getPrivateProfileColor(LPCWSTR fileName, LPCWSTR appName, LPCWSTR keyName, COLORREF& outValue)
{
	_bstr_t value = L"";
	HRESULT hr = getPrivateProfileBSTR(fileName, appName, keyName, value);
	if (hr != S_OK) return hr;
	if (!(BSTR)value) return S_FALSE;

	int c = ::lstrlenW(value);

	if (c == 0) return S_FALSE;

	BSTR bstrValue = value;

	if (*bstrValue == L'#')
	{
		DWORD temp = wcstoul(bstrValue + 1, 0, 16);

		if (c == 4)
		{
			// #fc8

			DWORD r = (temp & 0x0F00) >> 8;
			DWORD g = (temp & 0x00F0) >> 4;
			DWORD b = (temp & 0x000F) >> 0;

			r |= r << 4;
			g |= g << 4;
			b |= b << 4;

			outValue = RGB(r, g, b);
		}
		else
		{
			// #ffcc88

			DWORD r = (temp & 0x00FF0000) >> 16;
			DWORD g = (temp & 0x0000FF00) >> 8;
			DWORD b = (temp & 0x000000FF) >> 0;

			outValue = RGB(r, g, b);
		}
	}
	else
	{
		BSTR sep1 = ::StrChrW(bstrValue, L',');

		if (!sep1)
		{
			outValue = wcstoul(bstrValue, 0, 0);
			return S_OK;
		}

		BSTR sep2 = ::StrChrW(sep1 + 1, L',');

		if (!sep2)
			return S_FALSE;

		*sep1 = L'\0';
		*sep2 = L'\0';

		DWORD r = wcstoul(bstrValue, 0, 0);
		DWORD g = wcstoul(sep1 + 1, 0, 0);
		DWORD b = wcstoul(sep2 + 1, 0, 0);

		outValue = RGB(r, g, b);
	}

	return S_OK;
}
#ifdef _GDIPLUS_H
inline HRESULT WINAPI getPrivateProfileColor(LPCWSTR fileName, LPCWSTR appName, LPCWSTR keyName, Gdiplus::Color& outValue)
{
	_bstr_t value = L"";
	HRESULT hr = getPrivateProfileBSTR(fileName, appName, keyName, value);
	if (hr != S_OK) return hr;
	if (!(BSTR)value) return S_FALSE;

	int c = ::lstrlenW(value);

	if (c == 0) return S_FALSE;

	BSTR bstrValue = value;

	if (*bstrValue == L'#')
	{
		DWORD temp = wcstoul(bstrValue + 1, 0, 16);

		if (c == 4)
		{
			// #fc8

			DWORD r = (temp & 0x0F00) >> 8;
			DWORD g = (temp & 0x00F0) >> 4;
			DWORD b = (temp & 0x000F) >> 0;

			r |= r << 4;
			g |= g << 4;
			b |= b << 4;

			outValue.SetFromCOLORREF(RGB(r, g, b));
		}
		else
		{
			// #ffcc88

			DWORD r = (temp & 0x00FF0000) >> 16;
			DWORD g = (temp & 0x0000FF00) >> 8;
			DWORD b = (temp & 0x000000FF) >> 0;

			outValue.SetFromCOLORREF(RGB(r, g, b));
		}
	}
	else
	{
		BSTR sep1 = ::StrChrW(bstrValue, L',');

		if (!sep1)
		{
			outValue.SetValue(wcstoul(bstrValue, 0, 16));
		}
		else
		{
			BSTR sep2 = ::StrChrW(sep1 + 1, L',');

			if (!sep2)
				return S_FALSE;

			BSTR sep3 = ::StrChrW(sep2 + 1, L',');

			if (!sep3)
			{
				*sep1 = L'\0';
				*sep2 = L'\0';

				DWORD r = wcstoul(bstrValue, 0, 0);
				DWORD g = wcstoul(sep1 + 1, 0, 0);
				DWORD b = wcstoul(sep2 + 1, 0, 0);

				outValue.SetFromCOLORREF(RGB(r, g, b));
			}
			else
			{
				*sep1 = L'\0';
				*sep2 = L'\0';
				*sep3 = L'\0';

				BYTE a = (BYTE)wcstoul(bstrValue, 0, 0);
				BYTE r = (BYTE)wcstoul(sep1 + 1, 0, 0);
				BYTE g = (BYTE)wcstoul(sep2 + 1, 0, 0);
				BYTE b = (BYTE)wcstoul(sep3 + 1, 0, 0);

				outValue.SetValue(outValue.MakeARGB(a, r, g, b));
			}
		}
	}

	return S_OK;
}
#endif
template<class T, class S>
inline HRESULT WINAPI getPrivateProfilePercent(LPCWSTR fileName, LPCWSTR appName, LPCWSTR keyName, T& outValue, S& outValuePercent)
{
	_bstr_t value = L"";
	HRESULT hr = getPrivateProfileBSTR(fileName, appName, keyName, value);
	if (hr != S_OK) return hr;
	if (!(BSTR)value) return S_FALSE;
	if (::lstrlenW(value) == 0) return S_FALSE;

	if (::StrChrW(value, L'%'))
		outValuePercent = wcstol(value, 0, 0);
	else
		outValue = wcstol(value, 0, 0);

	return S_OK;
}

template<class T>
inline HRESULT WINAPI getPrivateProfileTime(LPCWSTR fileName, LPCWSTR appName, LPCWSTR keyName, T& outValue)
{
	_bstr_t value = L"";
	HRESULT hr = getPrivateProfileBSTR(fileName, appName, keyName, value);
	if (hr != S_OK) return hr;
	if (!(BSTR)value) return S_FALSE;
	if (::lstrlenW(value) == 0) return S_FALSE;

	LPWSTR sep = ::StrChrW(value, L':');

	if (sep)
	{
		*sep = L'\0';
		outValue = _wtof(value) * 60 + _wtof(sep + 1);
	}
	else
	{
		outValue = _wtof(value);
	}

	return S_OK;
}

template<class T, class A>
inline HRESULT WINAPI getPrivateProfileLabel(LPCWSTR fileName, LPCWSTR appName, LPCWSTR keyName, T& outValue, const A& array)
{
	_bstr_t value = L"";
	HRESULT hr = getPrivateProfileBSTR(fileName, appName, keyName, value);
	if (hr != S_OK) return hr;
	if (!(BSTR)value) return S_FALSE;

	int c = _countof(array);
	for (int i = 0; i < c; i++)
	{
		if (::lstrcmpW(array[i].label, value) == 0)
		{
			MY_TRACE_WSTR(array[i].label);
			outValue = array[i].value;
			return S_OK;
		}
	}

	return S_FALSE;
}

//---------------------------------------------------------------------
// Set

template<class T>
inline HRESULT WINAPI setPrivateProfileBSTR(LPCWSTR fileName, LPCWSTR appName, LPCWSTR keyName, const T& value)
{
	::WritePrivateProfileStringW(appName, keyName, value, fileName);
	return S_OK;
}

template<class T>
inline HRESULT WINAPI setPrivateProfileInt(LPCWSTR fileName, LPCWSTR appName, LPCWSTR keyName, const T& value)
{
	WCHAR text[MAX_PATH] = {};
	::StringCbPrintfW(text, sizeof(text), L"%d", value);
	return setPrivateProfileBSTR(fileName, appName, keyName, text);
}

template<class T>
inline HRESULT WINAPI setPrivateProfileReal(LPCWSTR fileName, LPCWSTR appName, LPCWSTR keyName, const T& value)
{
	WCHAR text[MAX_PATH] = {};
	::StringCbPrintfW(text, sizeof(text), L"%f", value);
	return setPrivateProfileBSTR(fileName, appName, keyName, text);
}

template<class T>
inline HRESULT WINAPI setPrivateProfileBool(LPCWSTR fileName, LPCWSTR appName, LPCWSTR keyName, const T& value)
{
	return setPrivateProfileBSTR(fileName, appName, keyName, value ? L"YES" : L"NO");
}

inline HRESULT WINAPI setPrivateProfileColor(LPCWSTR fileName, LPCWSTR appName, LPCWSTR keyName, COLORREF value)
{
	BYTE r = GetRValue(value);
	BYTE g = GetGValue(value);
	BYTE b = GetBValue(value);
	WCHAR text[MAX_PATH] = {};
	::StringCbPrintfW(text, sizeof(text), L"#%02x%02x%02x", r, g, b);
	return setPrivateProfileBSTR(fileName, appName, keyName, text);
}

#ifdef _GDIPLUS_H
inline HRESULT WINAPI setPrivateProfileColor(LPCWSTR fileName, LPCWSTR appName, LPCWSTR keyName, const Gdiplus::Color& value)
{
	WCHAR text[MAX_PATH] = {};
	::StringCbPrintfW(text, sizeof(text), L"%08X", value.GetValue());
	return setPrivateProfileBSTR(fileName, appName, keyName, text);
}
#endif
template<class T, class A>
inline HRESULT WINAPI setPrivateProfileLabel(LPCWSTR fileName, LPCWSTR appName, LPCWSTR keyName, const T& value, const A& array)
{
	int c = _countof(array);
	for (int i = 0; i < c; i++)
	{
		if (array[i].value == value)
			return setPrivateProfileBSTR(fileName, appName, keyName, array[i].label);
	}

	return S_FALSE;
}

//---------------------------------------------------------------------
