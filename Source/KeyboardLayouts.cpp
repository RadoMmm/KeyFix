/*
MIT License
Copyright (c) 2023 Rado Manzela

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE
*/
#include "pch.h"
#include "KeyboardLayouts.h"

#include <algorithm>
#include <stdexcept>
#include <ranges>

using namespace std;

const LayoutInfo* KeyboardLayouts::FindLayout(HKL hkl)
{
	auto x = ranges::find_if(m_allLayouts, [&](auto& x) { return x.HKL == hkl; });
	if (x == m_allLayouts.end())
		return nullptr;
	return &(*x);
}

void KeyboardLayouts::LoadLayouts()
{
	m_uniqueLayouts.clear();
	m_allLayouts.clear();

	HKL hklArr[100];
	int layoutsCount = GetKeyboardLayoutList(_countof(hklArr), hklArr);
	for (int i = 0; i < layoutsCount; i++)
	{
		if (ActivateKeyboardLayout(hklArr[i], KLF_SETFORPROCESS))
		{
			WCHAR pName[KL_NAMELENGTH];
			if (GetKeyboardLayoutName(pName))
			{
				std::wstring layoutName = pName;

				std::wstring subKey(L"SYSTEM\\CurrentControlSet\\Control\\Keyboard Layouts\\");
				subKey += pName;

				auto valueName = L"Layout Text";

				DWORD dataSize{};
				LONG retCode = ::RegGetValue(
					HKEY_LOCAL_MACHINE,
					subKey.c_str(),
					valueName,
					RRF_RT_REG_SZ,
					nullptr,
					nullptr,
					&dataSize
				);

				if (retCode == ERROR_SUCCESS)
				{
					std::wstring s(dataSize / sizeof(wchar_t), L' ');

					retCode = ::RegGetValue(
						HKEY_LOCAL_MACHINE,
						subKey.c_str(),
						valueName,
						RRF_RT_REG_SZ,
						nullptr,
						&s[0],
						&dataSize
					);

					layoutName = s;
				}

				m_allLayouts.emplace_back(hklArr[i], layoutName);

				if (ranges::none_of(m_uniqueLayouts, [&](auto& x) { return x.DisplayName == layoutName; }))
					m_uniqueLayouts.emplace_back(hklArr[i], layoutName);
			}
		}
	}
}

bool KeyboardLayouts::CanTranslateText(std::wstring_view text, HKL hkl)
{
	return ranges::none_of(text, [&](wchar_t x) { return VkKeyScanEx(x, hkl) == -1; });
}

std::wstring KeyboardLayouts::TranslateText(std::wstring_view text, HKL hklFrom, HKL hklTo, bool capsLock)
{
	wstring result;

	WCHAR buff[32];
	UINT size = _countof(buff);

	BYTE state[256];
	memset(state, 0, sizeof(state));

	if (capsLock)
		state[VK_CAPITAL] = 1;

	for (size_t i = 0; i < text.length(); i++)
	{
		SHORT sc = VkKeyScanEx(text[i], hklFrom);
		if (sc == -1)
		{
			result += text[i];
			continue;
		}
		int vk = sc & 0xff;
		int shiftState = sc >> 8;

		state[VK_SHIFT] = (shiftState & 1) ? 0x80 : 0x00;
		state[VK_CONTROL] = (shiftState & 2) ? 0x80 : 0x00;
		state[VK_MENU] = (shiftState & 4) ? 0x80 : 0x00;

		int numChars = ToUnicodeEx(sc & 255, 0, state, buff, size, 0, hklTo);

		if (numChars > 0)
		{
			for (int j = 0; j < numChars; j++)
				result += buff[j];
		}
		else
		{
			result += text[i];
		}
	}

	return result;
}
