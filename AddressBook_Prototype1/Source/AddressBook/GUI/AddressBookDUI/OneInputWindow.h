#pragma once

#include "../DuiLib/UIlib.h"
#include <string>

using namespace std;
using namespace DuiLib;

class OneInputWindow : public CWindowWnd, public INotifyUI, public IMessageFilterUI
{
public:
	OneInputWindow(string *res, string msg, string title) { m_pRes = res; m_Msg = msg; m_Title = title; };
	LPCTSTR GetWindowClassName() const { return _T("UIPopupFrame"); };
	UINT GetClassStyle() const { return UI_CLASSSTYLE_DIALOG; };
	void OnFinalMessage(HWND /*hWnd*/)
	{
		m_pm.RemovePreMessageFilter(this);
		delete this;
	};

	void Init() {
		CEditUI* pInputEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("edit_input")));
		if (pInputEdit)
		{
			pInputEdit->SetText(m_pRes->c_str());
			pInputEdit->SetFocus();
		}

		CLabelUI* pTitleLabel = static_cast<CLabelUI*>(m_pm.FindControl(_T("label_title")));
		if (pTitleLabel)
			pTitleLabel->SetText(m_Title.c_str());

		CLabelUI* pMsgLabel = static_cast<CLabelUI*>(m_pm.FindControl(_T("label_msg")));
		if (pMsgLabel)
			pMsgLabel->SetText(m_Msg.c_str());

	}

	void Notify(TNotifyUI& msg)
	{
		if (msg.sType == _T("click")) {
			if (msg.pSender->GetName() == _T("w_close")) { Close(1); return; }
			else if (msg.pSender->GetName() == _T("btn_ok")) 
			{
				CEditUI* pInputEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("edit_input")));
				(*m_pRes) = pInputEdit->GetText();
				Close(0); 
				return; 
			}
		}
	}

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
		styleValue &= ~WS_CAPTION;
		::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

		m_pm.Init(m_hWnd);
		m_pm.AddPreMessageFilter(this);
		CDialogBuilder builder;
		CControlUI* pRoot = builder.Create(_T("one_input.xml"), NULL, (UINT)0, &m_pm);
		ASSERT(pRoot && "Failed to parse XML");
		m_pm.AttachDialog(pRoot);
		m_pm.AddNotifier(this);

		Init();
		return 0;
	}

	LRESULT OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if (::IsIconic(*this)) bHandled = FALSE;
		return (wParam == 0) ? TRUE : FALSE;
	}

	LRESULT OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return 0;
	}

	LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return 0;
	}

	LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		POINT pt; pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);
		::ScreenToClient(*this, &pt);

		RECT rcClient;
		::GetClientRect(*this, &rcClient);

		RECT rcCaption = m_pm.GetCaptionRect();
		if (pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right \
			&& pt.y >= rcCaption.top && pt.y < rcCaption.bottom) {
			CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(pt));
			if (pControl && _tcscmp(pControl->GetClass(), _T("ButtonUI")) != 0)
				return HTCAPTION;
		}

		return HTCLIENT;
	}

	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		SIZE szRoundCorner = m_pm.GetRoundCorner();
		if (!::IsIconic(*this) && (szRoundCorner.cx != 0 || szRoundCorner.cy != 0)) {
			CDuiRect rcWnd;
			::GetWindowRect(*this, &rcWnd);
			rcWnd.Offset(-rcWnd.left, -rcWnd.top);
			rcWnd.right++; rcWnd.bottom++;
			HRGN hRgn = ::CreateRoundRectRgn(rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom, szRoundCorner.cx, szRoundCorner.cy);
			::SetWindowRgn(*this, hRgn, TRUE);
			::DeleteObject(hRgn);
		}

		bHandled = FALSE;
		return 0;
	}

	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		LRESULT lRes = 0;
		BOOL bHandled = TRUE;
		switch (uMsg) {
		case WM_CREATE:        lRes = OnCreate(uMsg, wParam, lParam, bHandled); break;
		case WM_NCACTIVATE:    lRes = OnNcActivate(uMsg, wParam, lParam, bHandled); break;
		case WM_NCCALCSIZE:    lRes = OnNcCalcSize(uMsg, wParam, lParam, bHandled); break;
		case WM_NCPAINT:       lRes = OnNcPaint(uMsg, wParam, lParam, bHandled); break;
		case WM_NCHITTEST:     lRes = OnNcHitTest(uMsg, wParam, lParam, bHandled); break;
		case WM_SIZE:          lRes = OnSize(uMsg, wParam, lParam, bHandled); break;
		default:
			bHandled = FALSE;
		}
		if (bHandled) return lRes;
		if (m_pm.MessageHandler(uMsg, wParam, lParam, lRes)) return lRes;
		return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	}

	LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
	{
		if (uMsg == WM_KEYDOWN) {
			if (wParam == VK_RETURN)
			{
				CButtonUI *pOkbtn = static_cast<CButtonUI *>(m_pm.FindControl("btn_ok"));
				TNotifyUI msg;
				msg.sType = DUI_MSGTYPE_CLICK;
				msg.pSender = pOkbtn;
				m_pm.SendNotify(msg);

				return true;
			}
			else if (wParam == VK_ESCAPE) 
			{
				Close(1);
				return true;
			}
		}
		return false;
	}

public:
	CPaintManagerUI m_pm;
	string *m_pRes;
	string m_Msg, m_Title;
};