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
#include "Clipboard.h"


void Clipboard::SendClipboardHotkey(WORD k)
{
	INPUT ip;
	ip.type = INPUT_KEYBOARD;
	ip.ki.wScan = 0;
	ip.ki.time = 0;
	ip.ki.dwExtraInfo = 0;

	ip.ki.wVk = VK_CONTROL;
	ip.ki.dwFlags = 0;
	SendInput(1, &ip, sizeof(INPUT));

	ip.ki.wVk = k;
	ip.ki.dwFlags = 0;
	SendInput(1, &ip, sizeof(INPUT));

	ip.ki.wVk = k;
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));

	ip.ki.wVk = VK_CONTROL;
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));
}


std::wstring Clipboard::GetText(HWND wnd)
{
	std::wstring txt;

	if (OpenClipboard(wnd))
	{
		HANDLE hData = GetClipboardData(CF_UNICODETEXT);
		if (hData != nullptr)
		{
			// Lock the handle to get the actual text pointer
			auto text = static_cast<wchar_t*>(GlobalLock(hData));
			if (text != nullptr)
			{
				txt = text;
			}
			GlobalUnlock(hData);
		}
		CloseClipboard();
	}

    return txt;
}

void Clipboard::SetText(HWND wnd, const std::wstring& text)
{
	if (OpenClipboard(wnd))
	{
		SIZE_T size = (1 + text.length()) * sizeof(wchar_t);

		HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, size);
		if (hMem != nullptr)
		{
			auto dst = GlobalLock(hMem);
			if (dst != nullptr)
			{
				memcpy(dst, text.data(), size);
				GlobalUnlock(hMem);
				EmptyClipboard();
				SetClipboardData(CF_UNICODETEXT, hMem);
			}
		}
		CloseClipboard();
	}
}

void Clipboard::SendPasteCmd()
{
	WaitForKeyRelease();
	SendClipboardHotkey('V');

}


bool Clipboard::IsAnyKeyPressed()
{
	for (int i = 1; i < 254; i++)
		if (GetAsyncKeyState(i) < 0)
			return true;

	return false;
}


void Clipboard::WaitForKeyRelease()
{
	while (IsAnyKeyPressed())
	{
		Sleep(50);
	}
}



void Clipboard::SendCopyCmd()
{
	WaitForKeyRelease();

	SendClipboardHotkey('C');
	Sleep(100);
	SendClipboardHotkey('C');
}




//void Clipboard::SendSelectBack(int cnt)
//{
//	INPUT ip;
//	ip.type = INPUT_KEYBOARD;
//	ip.ki.wScan = 0;
//	ip.ki.time = 0;
//	ip.ki.dwExtraInfo = 0;
//
//
//
//	ip.ki.wVk = VK_LSHIFT;
//	ip.ki.dwFlags = 0; // 0 for key press
//	SendInput(1, &ip, sizeof(INPUT));
//	Sleep(10);
//
//	for (int i = 0; i < cnt; i++)
//	{
//		ip.ki.wVk = VK_LEFT;
//		ip.ki.dwFlags = 0 | KEYEVENTF_EXTENDEDKEY; // 0 for key press
//		SendInput(1, &ip, sizeof(INPUT));
//
//		ip.ki.wVk = VK_LEFT;
//		ip.ki.dwFlags = KEYEVENTF_KEYUP | KEYEVENTF_EXTENDEDKEY;
//		SendInput(1, &ip, sizeof(INPUT));
//	}
//
//	// Release the "Ctrl" key
//	ip.ki.wVk = VK_LSHIFT;
//	ip.ki.dwFlags = KEYEVENTF_KEYUP;
//	SendInput(1, &ip, sizeof(INPUT));
//}

