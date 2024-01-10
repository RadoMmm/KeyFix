// CaretTip.cpp : implementation file
//

#include "pch.h"
#include "TipWnd.h"


// CCaretTip

IMPLEMENT_DYNAMIC(CTipWnd, CWnd)

CTipWnd::CTipWnd()
{

}

CTipWnd::~CTipWnd()
{
}

void CTipWnd::SetText(CString txt, COLORREF textColor, COLORREF bkColor)
{
	if (m_txt != txt)
	{
		m_txt = txt;
		m_bkColor = bkColor;
		m_textColor = textColor;

		CPaintDC dc(this);
		CRect rect;
		if (dc.DrawText(txt, &rect, DT_CALCRECT | DT_SINGLELINE) > 0)
		{
			SetWindowPos(nullptr, 0, 0, rect.right, rect.bottom, SWP_NOZORDER | SWP_SHOWWINDOW | SWP_NOMOVE);
		}
	}
}


BEGIN_MESSAGE_MAP(CTipWnd, CWnd)
	ON_WM_PAINT()
END_MESSAGE_MAP()


void CTipWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CRect rect;
	GetClientRect(rect);

	dc.SetTextColor(m_textColor);
	dc.FillSolidRect(rect, m_bkColor);
	dc.DrawText(m_txt, rect, 0);
}
