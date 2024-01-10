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
#include "afxdialogex.h"

#include "ActionOptions.h"
#include "CaretTipOptions.h"
#include <map>


class CSettingsDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSettingsDlg)

public:
	CSettingsDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CSettingsDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SETTINGS };
#endif

protected:
	bool ReadHotKey(int id, CHotKeyCtrl& ctrl);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	CaretTipOptions& TipOptions();

	DECLARE_MESSAGE_MAP()
public:

	std::map<int, ActionOptions> m_actions;
	std::map<std::string, CaretTipOptions> m_caretTips;

	CHotKeyCtrl c_hotkeyQuick;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	CHotKeyCtrl c_hotkeyOpen;
	BOOL m_autoStart;
	CHotKeyCtrl c_hotkeyCapsLock;
	afx_msg void OnBnClickedShowCaretTip();
	CComboBox c_language;
	afx_msg void OnBnClickedTipBkColor();
	afx_msg void OnBnClickedTipTextColor();
	CButton c_showLanguageTip;
	afx_msg void OnCbnSelchangeLanguage();
};
