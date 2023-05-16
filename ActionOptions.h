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
#pragma once

#include <nlohmann/json.hpp>

#define HOTKEY_OPEN 1
#define HOTKEY_QUICK_REPLACE 2
#define HOTKEY_QUICK_CAPS_LOCK 3

struct ActionOptions
{
	WORD HotKeyVirtualKeyCode;
	WORD HotKeyModifiers;

	bool HotkeySet()
	{
		return HotKeyVirtualKeyCode != 0;
	}
};

void to_json(nlohmann::json& j, const ActionOptions& p);
void from_json(const nlohmann::json& j, ActionOptions& p);
void to_json(nlohmann::json& j, const RECT& p);
void from_json(const nlohmann::json& j, RECT& p);