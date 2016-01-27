#pragma once

#include "../DuiLib/UIlib.h"
#include "../../Model/DB/Util.h"
#include "../../Model/AddressBookModel.h"
#include <string>
#include <map>
#include <iostream>
#include <sstream>
#include <algorithm>

using namespace std;
using namespace DuiLib;

class CustomColumnsWindow : public CWindowWnd, public INotifyUI, public IMessageFilterUI
{
public:
	CustomColumnsWindow(vector<string> *res) { m_pRes = res; };
	LPCTSTR GetWindowClassName() const { return _T("UIPopupFrame"); };
	UINT GetClassStyle() const { return UI_CLASSSTYLE_DIALOG; };
	void OnFinalMessage(HWND /*hWnd*/)
	{
		m_pm.RemovePreMessageFilter(this);
		delete this;
	};

	void Init() {
		CEditUI* pNameEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("edit_columns")));
		if (pNameEdit)
			pNameEdit->SetFocus();
	}

	void Notify(TNotifyUI& msg)
	{
		if (msg.sType == DUI_MSGTYPE_CLICK) {
			if (msg.pSender->GetName() == _T("w_close")) { Close(1); return; }
			else if (msg.pSender->GetName() == _T("btn_ok")) 
			{
				m_pRes->push_back(FIRSTNAME_HEADER_NAME);
				m_pRes->push_back(LASTNAME_HEADER_NAME);
				m_pRes->push_back(ADDRESS1_HEADER_NAME);
				m_pRes->push_back(ADDRESS2_HEADER_NAME);
				m_pRes->push_back(CITY_HEADER_NAME);
				m_pRes->push_back(STATE_HEADER_NAME);
				m_pRes->push_back(ZIPCODE_HEADER_NAME);
				m_pRes->push_back(PHONE_HEADER_NAME);

				CEditUI *columns = static_cast<CEditUI *>(m_pm.FindControl("edit_columns"));
				string buff = columns->GetText();
				replace(buff.begin(), buff.end(), ' ', ',');
				stringstream ss(buff);
				string res;
				while (getline(ss, res, ','))
				{
					if (res == "")
						continue;

					if (!Util::IsLetterNumberDash(res))
					{
						MessageBox(NULL, "Name contains invalid characters.", "Notify", MB_OK);
						return;
					}

					res = Util::ToUpper(res);
					if (find(m_pRes->begin(), m_pRes->end(), res) == m_pRes->end())
						m_pRes->push_back(res);
				}
				
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
		CControlUI* pRoot = builder.Create(_T("custom_columns.xml"), NULL, (UINT)0, &m_pm);
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
			if (wParam == VK_ESCAPE) 
			{
				Close(1);
				return true;
			}
			else if (wParam == VK_RETURN)
			{
				CButtonUI *pOkbtn = static_cast<CButtonUI *>(m_pm.FindControl("btn_ok"));
				TNotifyUI msg;
				msg.sType = DUI_MSGTYPE_CLICK;
				msg.pSender = pOkbtn;
				m_pm.SendNotify(msg);
			}
		}
		return false;
	}

public:
	CPaintManagerUI m_pm;
	vector<string> *m_pRes;
};