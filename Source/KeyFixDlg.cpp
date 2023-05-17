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
#include "framework.h"
#include "KeyFix.h"
#include "KeyFixDlg.h"
#include "AboutDlg.h"
#include "SettingsDlg.h"
#include "afxdialogex.h"

#include <algorithm>
#include <string>
#include <ranges>
#include <set>
#include <fstream>
#include <filesystem>

#include "Clipboard.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;
using namespace nlohmann;


CKeyFixDlg::CKeyFixDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_KEYFIX_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CKeyFixDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PREVIEW, c_preview);
	DDX_Control(pDX, IDC_LAYOUT_FROM, c_layoutFrom);
	DDX_Control(pDX, IDC_LAYOUT_TO, c_layoutTo);
	DDX_Control(pDX, IDC_OPTIONS, c_settings);
	DDX_Control(pDX, IDOK, c_replace);
	DDX_Control(pDX, IDC_NEXT, c_next);
	DDX_Control(pDX, IDC_NO_TEXT, c_noText);
	DDX_Control(pDX, IDC_FRAME, c_frame);
	DDX_Control(pDX, IDC_CAPS_LOCK, c_capsLock);
}

const UINT MSG_SYS_TRAY = RegisterWindowMessage(L"SysTray");

BEGIN_MESSAGE_MAP(CKeyFixDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_HOTKEY()
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDOK, &CKeyFixDlg::OnBnClickedOk)
	ON_CBN_SELENDOK(IDC_LAYOUT_FROM, &CKeyFixDlg::OnCbnSelendokLayoutFrom)
	ON_CBN_SELENDOK(IDC_LAYOUT_TO, &CKeyFixDlg::OnCbnSelendokLayoutTo)
	ON_BN_CLICKED(IDCANCEL, &CKeyFixDlg::OnBnClickedCancel)
	ON_WM_DESTROY()
	ON_REGISTERED_MESSAGE(MSG_SYS_TRAY, &CKeyFixDlg::OnSysTray)
	ON_BN_CLICKED(IDC_NEXT, &CKeyFixDlg::OnNext)
	ON_COMMAND(IDM_EXIT, &CKeyFixDlg::OnExit)
	ON_COMMAND(ID__OPEN, &CKeyFixDlg::OnOpen)
	ON_BN_CLICKED(IDC_SETTINGS, &CKeyFixDlg::OnBnClickedSettings)
	ON_WM_GETMINMAXINFO()
	ON_WM_WINDOWPOSCHANGING()
	ON_COMMAND(IDM_ABOUT, &CKeyFixDlg::OnAbout)
	ON_BN_CLICKED(IDC_CAPS_LOCK, &CKeyFixDlg::OnBnClickedCapsLock)
END_MESSAGE_MAP()



BOOL CKeyFixDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);
	ASSERT((IDM_EXIT & 0xFFF0) == IDM_EXIT);
	ASSERT(IDM_EXIT < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}

		pSysMenu->AppendMenu(MF_SEPARATOR);
		CString tmp;
		VERIFY(tmp.LoadString(IDM_EXIT));
		pSysMenu->AppendMenu(MF_STRING, IDM_EXIT, tmp);
	}

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	m_layouts.LoadLayouts();

	CreateTrayIcon();

	wchar_t* localAppDataFolder;
	if (SHGetKnownFolderPath(FOLDERID_LocalAppData, KF_FLAG_CREATE, NULL, &localAppDataFolder) == S_OK)
	{
		m_settingsPath = localAppDataFolder;
		CoTaskMemFree(localAppDataFolder);
	}

	m_settingsPath /= "KeyFix";
	filesystem::create_directories(m_settingsPath);

	m_settingsPath /= "settings.json";

	ifstream ss(m_settingsPath.c_str());
	if (ss.is_open())
	{
		ss >> m_settings;
		ss.close();
	}
	else
	{ // first start
		m_settings = json::object();
		ApplyAutoStart();
		m_startHidden = false;
		PostMessage(WM_COMMAND, IDC_SETTINGS, 0);
	}

	m_actions = m_settings.value("actions", m_actions);
	RegisterHotKeys(m_actions);

	CRect pos;
	pos = m_settings.value("pos", pos);
	if (!pos.IsRectEmpty())
		SetWindowPos(nullptr, pos.left, pos.top, pos.Width(), pos.Height(), SWP_NOZORDER);

	HICON hIcn = (HICON)LoadImage(
		AfxGetApp()->m_hInstance,
		MAKEINTRESOURCE(IDI_SETTINGS),
		IMAGE_ICON,
		16, 16,
		LR_DEFAULTCOLOR
	);
	c_settings.SetIcon(hIcn);

	CFont* pFont = GetFont();

	LOGFONT lf;

	pFont->GetLogFont(&lf);
	lf.lfWeight = FW_BOLD;
	m_fontBold.CreateFontIndirect(&lf);
	c_replace.SetFont(&m_fontBold);

	return FALSE;  // return TRUE  unless you set the focus to a control
}

void CKeyFixDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	UINT id = nID & 0xFFF0;
	switch (id)
	{
		case IDM_ABOUTBOX:
		{
			CAboutDlg dlgAbout;
			dlgAbout.DoModal();
		}
		break;

		case IDM_EXIT:
			OnCancel();
			break;

		default:
			CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CKeyFixDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CKeyFixDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT CKeyFixDlg::OnSysTray(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(lParam))
	{
	case WM_LBUTTONUP:
		StartFromClipboard();
		break;

	case WM_CONTEXTMENU:
	{
		POINT const pt = { LOWORD(wParam), HIWORD(wParam) };
		ShowContextMenu(pt);
	}
	break;

	}

	return 0;
}

void CKeyFixDlg::OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2)
{
	switch (nHotKeyId)
	{
	case  HOTKEY_OPEN:
		StartByCopyToClipboard();
		break;

	case HOTKEY_QUICK_REPLACE:
		StartQuickReplace();
		break;

	case HOTKEY_QUICK_CAPS_LOCK:
		StartQuickCapsLock();
		break;
	}
}


void CKeyFixDlg::StartByCopyToClipboard()
{
	Clipboard::SendCopyCmd();

	StartFromClipboard();
}

void CKeyFixDlg::StartQuickReplace()
{
	Clipboard::SendCopyCmd();

	InitFromClipboard();

	if (NextTranslation(true))
	{
		Clipboard::SetText(GetSafeHwnd(), m_lastDstText);
		HideWindow();
		Clipboard::SendPasteCmd();
	}
}

void CKeyFixDlg::StartQuickCapsLock()
{
	Clipboard::SendCopyCmd();

	InitFromClipboard();

	c_capsLock.SetCheck(BST_CHECKED);
	if (NextTranslation(false))
	{
		Clipboard::SetText(GetSafeHwnd(), m_lastDstText);
		HideWindow();
		Clipboard::SendPasteCmd();
	}
}


void CKeyFixDlg::InitFromClipboard()
{
	auto txt = Clipboard::GetText(GetSafeHwnd());

	m_srcText = txt;
	m_lastDstText = txt;
	m_translations.clear();
	m_translationIdx = -1;

	c_capsLock.SetCheck(BST_UNCHECKED);

	std::vector<LayoutInfo> layouts = m_layouts.Layouts();

	CWnd* wnd = GetForegroundWindow();
	if (wnd != nullptr)
	{
		HKL activeLayout = GetKeyboardLayout(GetWindowThreadProcessId(wnd->GetSafeHwnd(), nullptr));
		auto activeLayoutInfo = m_layouts.FindLayout(activeLayout);
		if (activeLayoutInfo != nullptr)
		{
			if (0 < std::erase_if(layouts, [&](auto& x) {return x.HKL == activeLayout; }))
				layouts.emplace(layouts.begin(), *activeLayoutInfo);
		}
	}

	c_layoutFrom.ResetContent();
	c_layoutTo.ResetContent();

	ranges::stable_sort(layouts, [&](auto& a, auto& b) { return m_layouts.CanTranslateText(txt, a.HKL) > m_layouts.CanTranslateText(txt, b.HKL); });

	for (auto& layout : layouts)
		c_layoutTo.SetItemData(c_layoutTo.AddString(layout.DisplayName.c_str()), (DWORD_PTR)layout.HKL);

	vector<LayoutInfo> srcLayouts;
	//ranges::copy_if(layouts, std::back_inserter(srcLayouts), [&](auto& x) { return m_layouts.CanTranslateText(txt, x.HKL); });
	//if (srcLayouts.empty())
	srcLayouts = layouts;

	std::set<wstring> uniqueTranslations;

	for (auto& srcLayout : srcLayouts)
	{
		c_layoutFrom.SetItemData(c_layoutFrom.AddString(srcLayout.DisplayName.c_str()), (DWORD_PTR)srcLayout.HKL);

		for (auto& dstLayout : layouts)
		{
			wstring txt = m_layouts.TranslateText(m_srcText, srcLayout.HKL, dstLayout.HKL, false);
			if (uniqueTranslations.count(txt) == 0)
			{
				m_translations.emplace_back(srcLayout, dstLayout);
				uniqueTranslations.insert(txt);
			}
		}
	}

	c_next.EnableWindow(m_translations.size() > 1);

	bool hasText = !m_srcText.empty();

	c_noText.ShowWindow(hasText ? SW_HIDE : SW_SHOW);
	c_layoutFrom.EnableWindow(hasText);
	c_layoutTo.EnableWindow(hasText);
	c_replace.EnableWindow(hasText);
	c_capsLock.EnableWindow(hasText);
	c_preview.EnableWindow(hasText);
}


void CKeyFixDlg::StartFromClipboard()
{
	InitFromClipboard();

	m_startHidden = false;
	ShowWindow(SW_NORMAL);

	NextTranslation(false);
}

CString CKeyFixDlg::GetClipboardText()
{
	CString txt;

	if (OpenClipboard())
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


void CKeyFixDlg::CreateTrayIcon()
{
	memset(&m_NID, 0, sizeof(m_NID));
	m_NID.cbSize = sizeof(m_NID);

	m_NID.uID = 1;

	ASSERT(::IsWindow(GetSafeHwnd()));
	m_NID.hWnd = GetSafeHwnd();

	m_NID.uCallbackMessage = MSG_SYS_TRAY;

	m_NID.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;

	m_NID.hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));

	_tcscpy_s(m_NID.szTip, _countof(m_NID.szTip), L"KeyFix");

	m_NID.uVersion = NOTIFYICON_VERSION_4;

	if (!Shell_NotifyIcon(NIM_ADD, &m_NID))
		return;

	Shell_NotifyIcon(NIM_SETVERSION, &m_NID);
}

void CKeyFixDlg::DestroyTrayIcon()
{
	Shell_NotifyIcon(NIM_DELETE, &m_NID);
}

void CKeyFixDlg::ShowContextMenu(POINT pt)
{
	HMENU hMenu = LoadMenu(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_TRAY_MENU));
	if (hMenu)
	{
		HMENU hSubMenu = GetSubMenu(hMenu, 0);
		if (hSubMenu)
		{
			// our window must be foreground before calling TrackPopupMenu or the menu will not disappear when the user clicks away
			//SetForegroundWindow(hwnd);

			// respect menu drop alignment
			UINT uFlags = TPM_RIGHTBUTTON;
			if (GetSystemMetrics(SM_MENUDROPALIGNMENT) != 0)
			{
				uFlags |= TPM_RIGHTALIGN;
			}
			else
			{
				uFlags |= TPM_LEFTALIGN;
			}

			TrackPopupMenuEx(hSubMenu, uFlags, pt.x, pt.y, GetSafeHwnd(), NULL);
		}
		DestroyMenu(hMenu);
	}
}

void CKeyFixDlg::SetClipboardText(CString text)
{
	if (OpenClipboard())
	{
		SIZE_T size = (1 + text.GetLength()) * sizeof(wchar_t);

		HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, size);
		if (hMem != nullptr)
		{
			auto dst = GlobalLock(hMem);
			if (dst != nullptr)
			{
				memcpy(dst, (LPCWSTR)text, size);
				GlobalUnlock(hMem);
				EmptyClipboard();
				SetClipboardData(CF_UNICODETEXT, hMem);
			}
		}
		CloseClipboard();
	}
}


void CKeyFixDlg::OnBnClickedOk()
{
	Clipboard::SetText(GetSafeHwnd(), m_lastDstText);
	HideWindow();

	Clipboard::SendPasteCmd();

	//SendSelectBack(static_cast<int>(ranges::count_if(m_lastDstText, [](wchar_t x) { return x != L'\r'; })));
}


void CKeyFixDlg::OnCbnSelendokLayoutFrom()
{
	Translate();
}


void CKeyFixDlg::OnCbnSelendokLayoutTo()
{
	Translate();
}


void CKeyFixDlg::OnBnClickedCancel()
{
	HideWindow();
}


void CKeyFixDlg::HideWindow()
{
	WINDOWPLACEMENT wp;
	GetWindowPlacement(&wp);

	m_settings["pos"] = wp.rcNormalPosition;
	SaveSettings();

	ShowWindow(SW_HIDE);
}


void CKeyFixDlg::OnDestroy()
{
	HideWindow(); // save position

	CDialogEx::OnDestroy();

	DestroyTrayIcon();
}

bool CKeyFixDlg::NextTranslation(bool skipFirst)
{
	if (skipFirst && m_translationIdx < 0)
		m_translationIdx = 0;

	int previousIdx = m_translationIdx;

	while (true)
	{
		++m_translationIdx;
		if (m_translationIdx >= static_cast<int>(m_translations.size()))
			m_translationIdx = 0;

		auto& layoutFrom = m_translations[m_translationIdx].first;
		auto& layoutTo = m_translations[m_translationIdx].second;

		wstring dstTxt = m_layouts.TranslateText(m_srcText, layoutFrom.HKL, layoutTo.HKL, false);
		if (dstTxt != m_lastDstText || m_translationIdx == previousIdx || m_translationIdx == 0)
		{
			if (m_translationIdx == previousIdx)
				m_translationIdx = -1;

			c_layoutFrom.SelectString(-1, layoutFrom.DisplayName.c_str());
			c_layoutTo.SelectString(-1, layoutTo.DisplayName.c_str());

			Translate();

			return true;
		}

		if (previousIdx < 0)
			previousIdx = 0;
	}
}

void CKeyFixDlg::Translate()
{
	HKL fromHkl = (HKL)(c_layoutFrom.GetItemData(c_layoutFrom.GetCurSel()));
	HKL toHkl = (HKL)(c_layoutTo.GetItemData(c_layoutTo.GetCurSel()));

	wstring dstTxt = m_layouts.TranslateText(m_srcText, fromHkl, toHkl, c_capsLock.GetCheck() == BST_CHECKED);
	c_preview.SetWindowText(dstTxt.c_str());

	m_lastDstText = dstTxt;
}


void CKeyFixDlg::OnNext()
{
	NextTranslation(true);
}


void CKeyFixDlg::OnExit()
{
	CDialog::OnCancel();
}


void CKeyFixDlg::OnOpen()
{
	StartFromClipboard();
}


void CKeyFixDlg::OnBnClickedSettings()
{
	CSettingsDlg dlg;
	dlg.m_actions = m_actions;
	dlg.m_autoStart = m_settings.value("autoStart", true);

	UnregisterHotKeys(m_actions);

	if (dlg.DoModal() == IDOK)
	{
		m_actions = dlg.m_actions;
		m_settings["autoStart"] = dlg.m_autoStart != 0;

		SaveSettings();
		ApplyAutoStart();
	}

	RegisterHotKeys(m_actions);
}

bool CKeyFixDlg::RegisterHotKeys(const std::map<int, ActionOptions>& actions)
{
	for (auto& action : actions)
		if (!RegisterHotKey(GetSafeHwnd(), action.first, action.second.HotKeyModifiers, action.second.HotKeyVirtualKeyCode))
			return false;
	return true;
}

void CKeyFixDlg::UnregisterHotKeys(const std::map<int, ActionOptions>& actions)
{
	for (auto& action : actions)
		UnregisterHotKey(GetSafeHwnd(), action.first);
}


void CKeyFixDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	if (c_frame.GetSafeHwnd() != nullptr)
	{
		CRect r;
		c_frame.GetWindowRect(&r);
		ScreenToClient(&r);
		CalcWindowRect(&r);
		lpMMI->ptMinTrackSize.x = r.Width();
		lpMMI->ptMinTrackSize.y = r.Height();
	}
	else
		CDialogEx::OnGetMinMaxInfo(lpMMI);
}

void CKeyFixDlg::ApplyAutoStart()
{
	bool autoStart = m_settings.value("autoStart", true);

	HKEY hkey = NULL;
	RegCreateKey(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", &hkey);

	if (autoStart)
	{
		wchar_t filePath[1000];
		DWORD len = GetModuleFileName(AfxGetInstanceHandle(), filePath, _countof(filePath));
		RegSetValueEx(hkey, L"KeyFix", 0, REG_SZ, (BYTE*)filePath, sizeof(wchar_t) * (1 + len));
	}
	else
	{
		RegDeleteKeyValue(hkey, nullptr, L"KeyFix");
	}

	RegCloseKey(hkey);
}


void CKeyFixDlg::SaveSettings()
{
	m_settings["actions"] = m_actions;

	std::ofstream f(m_settingsPath.c_str());
	f << m_settings;
	f.close();
}



void CKeyFixDlg::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	if (m_startHidden) // hidden after start
	{
		lpwndpos->flags &= ~SWP_SHOWWINDOW;
	}
	CDialogEx::OnWindowPosChanging(lpwndpos);
}


void CKeyFixDlg::OnAbout()
{
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
}


void CKeyFixDlg::OnBnClickedCapsLock()
{
	Translate();
}

