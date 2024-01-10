#pragma once


// CTipWnd

class CTipWnd : public CWnd
{
	DECLARE_DYNAMIC(CTipWnd)

public:
	CTipWnd();
	virtual ~CTipWnd();

	void SetText(CString txt, COLORREF textColor, COLORREF bkColor);

protected:
	CString m_txt;
	COLORREF m_textColor = RGB(0, 0, 0);
	COLORREF m_bkColor = RGB(0, 0, 0);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
};


