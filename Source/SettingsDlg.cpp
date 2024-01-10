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

// SettingsDlg.cpp : implementation file
//

#include "pch.h"
#include "KeyFix.h"
#include "afxdialogex.h"
#include "SettingsDlg.h"
#include "resource.h"
#include <ranges>

#include <afxcolordialog.h>

using namespace std;

// CSettingsDlg dialog

IMPLEMENT_DYNAMIC(CSettingsDlg, CDialogEx)

CSettingsDlg::CSettingsDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SETTINGS, pParent)
	, m_autoStart(FALSE)
{

}

CSettingsDlg::~CSettingsDlg()
{
}

void CSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_HOTKEY_QUICK, c_hotkeyQuick);
	DDX_Control(pDX, IDC_HOTKEY_OPEN, c_hotkeyOpen);
	DDX_Check(pDX, IDC_AUTO_START, m_autoStart);
	DDX_Control(pDX, IDC_HOTKEY_CAPS_LOCK, c_hotkeyCapsLock);
	DDX_Control(pDX, IDC_LANGUAGE, c_language);
	DDX_Control(pDX, IDC_SHOW_CARET_TIP, c_showLanguageTip);
}


BEGIN_MESSAGE_MAP(CSettingsDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CSettingsDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_SHOW_CARET_TIP, &CSettingsDlg::OnBnClickedShowCaretTip)
	ON_BN_CLICKED(IDC_TIP_BK_COLOR, &CSettingsDlg::OnBnClickedTipBkColor)
	ON_BN_CLICKED(IDC_TIP_TEXT_COLOR, &CSettingsDlg::OnBnClickedTipTextColor)
	ON_CBN_SELCHANGE(IDC_LANGUAGE, &CSettingsDlg::OnCbnSelchangeLanguage)
END_MESSAGE_MAP()


// CSettingsDlg message handlers


BOOL CSettingsDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	for (auto& action : m_actions)
	{
		switch (action.first)
		{
		case HOTKEY_OPEN:
			c_hotkeyOpen.SetHotKey(action.second.HotKeyVirtualKeyCode, action.second.HotKeyModifiers);
			break;
		case HOTKEY_QUICK_REPLACE:
			c_hotkeyQuick.SetHotKey(action.second.HotKeyVirtualKeyCode, action.second.HotKeyModifiers);
			break;
		case HOTKEY_QUICK_CAPS_LOCK:
			c_hotkeyCapsLock.SetHotKey(action.second.HotKeyVirtualKeyCode, action.second.HotKeyModifiers);
			break;
		}
	}

	for (auto& lngTip : m_caretTips)
		c_language.AddString(CString(lngTip.first.c_str()));

	c_language.SetCurSel(0);
	OnCbnSelchangeLanguage();

	return TRUE;  // return TRUE unless you set the focus to a control	
}

bool CSettingsDlg::ReadHotKey(int id, CHotKeyCtrl& ctrl)
{
	ActionOptions ao;
	ctrl.GetHotKey(ao.HotKeyVirtualKeyCode, ao.HotKeyModifiers);
	if (ao.HotkeySet())
	{
		if (ranges::any_of(m_actions, [&](auto& a) { return ao.HotKeyModifiers == a.second.HotKeyModifiers && ao.HotKeyVirtualKeyCode == a.second.HotKeyVirtualKeyCode; })
			|| !RegisterHotKey(GetSafeHwnd(), 0, ao.HotKeyModifiers, ao.HotKeyVirtualKeyCode))
		{
			CString tmp;
			VERIFY(tmp.LoadString(IDS_INVALID_HOTKEY));
			tmp.Format(tmp, ctrl.GetHotKeyName());
			AfxMessageBox(tmp, MB_ICONSTOP);

			return false;
		}

		UnregisterHotKey(GetSafeHwnd(), 0);
		m_actions[id] = ao;
	}

	return true;
}


void CSettingsDlg::OnBnClickedOk()
{
	m_actions.clear();

	if (!ReadHotKey(HOTKEY_QUICK_REPLACE, c_hotkeyQuick))
		return;

	if (!ReadHotKey(HOTKEY_OPEN, c_hotkeyOpen))
		return;

	if (!ReadHotKey(HOTKEY_QUICK_CAPS_LOCK, c_hotkeyCapsLock))
		return;


	CDialogEx::OnOK();
}


void CSettingsDlg::OnBnClickedShowCaretTip()
{
	TipOptions().ShowTip = c_showLanguageTip.GetCheck() == BST_CHECKED;
}


void CSettingsDlg::OnBnClickedTipBkColor()
{
	CMFCColorDialog dlg(TipOptions().BkColor, 0, this);
	if (IDOK == dlg.DoModal())
		TipOptions().BkColor = dlg.GetColor();
}


void CSettingsDlg::OnBnClickedTipTextColor()
{
	CMFCColorDialog dlg(TipOptions().TextColor, 0, this);
	if (IDOK == dlg.DoModal())
		TipOptions().TextColor = dlg.GetColor();
}

CaretTipOptions& CSettingsDlg::TipOptions()
{
	CString language;
	c_language.GetLBText(c_language.GetCurSel(), language);
	return m_caretTips.at((LPCSTR)CStringA(language));
}

void CSettingsDlg::OnCbnSelchangeLanguage()
{
	c_showLanguageTip.SetCheck(TipOptions().ShowTip ? BST_CHECKED : BST_UNCHECKED);
}
