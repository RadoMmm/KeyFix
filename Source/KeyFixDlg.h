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

#include "KeyboardLayouts.h"
#include "ActionOptions.h"
#include "CaretTipOptions.h"
#include "TipWnd.h"

#include <nlohmann/json.hpp>

// CKeyFixDlg dialog
class CKeyFixDlg : public CDialogEx
{
	// Construction
public:
	CKeyFixDlg(CWnd* pParent = nullptr);	// standard constructor

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_KEYFIX_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


	// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnSysTray(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2);

private:
	bool m_startHidden = true;

	static void CALLBACK HandleWinEvent(HWINEVENTHOOK hook, DWORD event, HWND hwnd,
		LONG idObject, LONG idChild,
		DWORD dwEventThread, DWORD dwmsEventTime);
	HWINEVENTHOOK m_hook;	
	void OnWinEvent(HWINEVENTHOOK hook, DWORD event, HWND hwnd,
		LONG idObject, LONG idChild,
		DWORD dwEventThread, DWORD dwmsEventTime);


	KeyboardLayouts m_layouts;

	std::vector<std::pair<LayoutInfo, LayoutInfo>> m_translations;
	int m_translationIdx = 0;

	CString GetClipboardText();

	std::vector<std::pair<HKL, CString>> m_uniqueLayouts;
	std::vector<std::pair<HKL, CString>> m_allLayouts;

	std::wstring m_srcText;
	std::wstring m_lastDstText;


	NOTIFYICONDATA m_NID;
	void CreateTrayIcon();
	void DestroyTrayIcon();

	void ShowContextMenu(POINT pt);

	CFont m_fontBold;

	void InitFromClipboard();

	void StartFromClipboard();
	void StartByCopyToClipboard();
	void StartQuickReplace();
	void StartQuickCapsLock();

	bool NextTranslation(bool skipFirst);
	void Translate();

	std::map<int, ActionOptions> m_actions;
	std::map<std::string, CaretTipOptions> m_caretTips;

	bool RegisterHotKeys(const std::map<int, ActionOptions>& actions);
	void UnregisterHotKeys(const std::map<int, ActionOptions>& actions);

	std::filesystem::path m_settingsPath;
	nlohmann::json m_settings;
	void SaveSettings();

	void HideWindow();

	void ApplyAutoStart();

	CTipWnd c_caretTip;	

public:

	void SetClipboardText(CString text);
	CEdit c_preview;
	CComboBox c_layoutFrom;
	CComboBox c_layoutTo;
	afx_msg void OnBnClickedOk();
	afx_msg void OnCbnSelendokLayoutFrom();
	afx_msg void OnCbnSelendokLayoutTo();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnDestroy();
	afx_msg void OnNext();
	afx_msg void OnExit();
	afx_msg void OnOpen();
	CButton c_settings;
	CButton c_replace;
	CButton c_next;
	afx_msg void OnBnClickedSettings();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	CMFCButton c_noText;
	CStatic c_frame;
	afx_msg void OnAbout();
	afx_msg void OnBnClickedCapsLock();
	CButton c_capsLock;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	bool GetCaretPosition(HWND hwnd, RECT& rect);
	CString GetActiveLanguageName(HWND wnd);
	CString GetLanguageName(HKL layout);
	void ShowLanguageTip(CString name);
};
