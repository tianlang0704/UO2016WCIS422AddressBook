#pragma once

#include "../DuiLib/UIlib.h"
#include <string>
#include <map>

using namespace std;
using namespace DuiLib;

class EditWindow : public CWindowWnd, public INotifyUI, public IMessageFilterUI
{
public:
	EditWindow(map<string, string> *res, vector<string> headers) { m_pRes = res; m_headers = headers; m_nCustom = 0; };
	LPCTSTR GetWindowClassName() const { return _T("UIPopupFrame"); };
	UINT GetClassStyle() const { return UI_CLASSSTYLE_DIALOG; };
	void OnFinalMessage(HWND /*hWnd*/)
	{
		m_pm.RemovePreMessageFilter(this);
		delete this;
	};

	void Init() {
		CEditUI* pNameEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("edit_firstname")));
		if (pNameEdit)
			pNameEdit->SetFocus();

		m_pFirstname = static_cast<CEditUI *>(m_pm.FindControl("edit_firstname"));
		m_pMiddlename = static_cast<CEditUI *>(m_pm.FindControl("edit_middlename"));
		m_pLastname = static_cast<CEditUI *>(m_pm.FindControl("edit_lastname"));

		m_pPhone = static_cast<CEditUI *>(m_pm.FindControl("edit_tel"));

		m_pAddress1 = static_cast<CEditUI *>(m_pm.FindControl("edit_address1"));
		m_pAddress2 = static_cast<CEditUI *>(m_pm.FindControl("edit_address2"));

		m_pCity = static_cast<CEditUI *>(m_pm.FindControl("edit_city"));
		m_pState = static_cast<CEditUI *>(m_pm.FindControl("edit_state"));
		m_pZip = static_cast<CEditUI *>(m_pm.FindControl("edit_zip"));


		if (m_pFirstname)
			m_pFirstname->SetText((*m_pRes)[FIRSTNAME_HEADER_NAME].c_str());
		//if (m_pMiddlename)
		//m_pMiddlename->SetText(m_pRes->at(MIDDLENAME_HEADER_NAME).c_str());
		if (m_pLastname)
			m_pLastname->SetText((*m_pRes)[LASTNAME_HEADER_NAME].c_str());
		if (m_pPhone)
			m_pPhone->SetText((*m_pRes)[PHONE_HEADER_NAME].c_str());
		if (m_pAddress1)
			m_pAddress1->SetText((*m_pRes)[ADDRESS1_HEADER_NAME].c_str());
		if (m_pAddress2)
			m_pAddress2->SetText((*m_pRes)[ADDRESS2_HEADER_NAME].c_str());
		if (m_pCity)
			m_pCity->SetText((*m_pRes)[CITY_HEADER_NAME].c_str());
		if (m_pState)
			m_pState->SetText((*m_pRes)[STATE_HEADER_NAME].c_str());
		if (m_pZip)
			m_pZip->SetText((*m_pRes)[ZIPCODE_HEADER_NAME].c_str());

		int fieldNum = m_headers.size();
		CVerticalLayoutUI *cusCon = static_cast<CVerticalLayoutUI *>(m_pm.FindControl("custom_field_container"));
		cusCon->SetFixedHeight((fieldNum - 8) * 45);
		for (int i = 8; i < fieldNum; i++)
		{
			string fieldName = m_headers[i];

			CHorizontalLayoutUI *container = new CHorizontalLayoutUI();
			container->SetAttribute("padding", "5,0,0,15");
			container->SetAttribute("height", "30");
			container->SetAttribute("childpadding", "10,0,10,0");
			container->SetManager(&m_pm, cusCon);

			CLabelUI *label = new CLabelUI();
			label->SetAttribute("height", "25");
			label->SetAttribute("width", "50");
			label->SetAttribute("text", fieldName.c_str());
			label->SetManager(&m_pm, cusCon);

			CEditUI *edit = new CEditUI();
			edit->SetAttribute("height", "25");
			edit->SetAttribute("bordercolor", "#C6CFD8");
			edit->SetAttribute("textcolor", "#FF4422A");
			edit->SetAttribute("bkimage", "file='search_bg.png' source='0,0,258,23' corner='1,1,1,1'");
			edit->SetAttribute("focusbordercolor", "#CC7878");
			edit->SetAttribute("borderround", "2,2");
			edit->SetAttribute("bordersize", "1");
			edit->SetAttribute("name", fieldName.c_str());
			edit->SetAttribute("text",m_pRes->at(fieldName).c_str());
			edit->SetManager(&m_pm, cusCon);
			
			container->Add(label);
			container->Add(edit);
			cusCon->Add(container);
		}



	}

	void Notify(TNotifyUI& msg)
	{
		if (msg.sType == _T("click")) {
			if (msg.pSender->GetName() == _T("w_close")) { Close(1); return; }
			else if (msg.pSender->GetName() == _T("btn_cancel")) { Close(1); return; }
			else if (msg.pSender->GetName() == _T("btn_ok")) 
			{
				if (m_pCity)
					(*m_pRes)[CITY_HEADER_NAME] = m_pCity->GetText().GetData();
				if (m_pState)
					(*m_pRes)[STATE_HEADER_NAME] = m_pState->GetText().GetData();
				if (m_pZip)
					(*m_pRes)[ZIPCODE_HEADER_NAME] = m_pZip->GetText().GetData();
				if (m_pAddress1)
					(*m_pRes)[ADDRESS1_HEADER_NAME] = m_pAddress1->GetText().GetData();
				if (m_pAddress2)
					(*m_pRes)[ADDRESS2_HEADER_NAME] = m_pAddress2->GetText().GetData();
				if (m_pLastname)
					(*m_pRes)[LASTNAME_HEADER_NAME] = m_pLastname->GetText().GetData();
				if (m_pFirstname)
					(*m_pRes)[FIRSTNAME_HEADER_NAME] = m_pFirstname->GetText().GetData();
				if (m_pPhone)
					(*m_pRes)[PHONE_HEADER_NAME] = m_pPhone->GetText().GetData();
				//if (m_pMiddlename)
				//(*m_pRes)[MIDDLENAME_HEADER_NAME] = m_pMiddlename->GetText().GetData();

				int fieldNum = m_headers.size();
				for (int i = 8; i < fieldNum; i++)
				{
					CEditUI *cusEdit = static_cast<CEditUI *>(m_pm.FindControl(m_headers[i].c_str()));
					if (cusEdit)
						(*m_pRes)[m_headers[i]] = cusEdit->GetText().GetData();
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
		CControlUI* pRoot = builder.Create(_T("edit.xml"), NULL, (UINT)0, &m_pm);
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
			if (wParam == VK_ESCAPE) {
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
	
private:
	map<string, string> *m_pRes;
	int m_nCustom;
	vector<string> m_headers;

	CEditUI *m_pFirstname;
	CEditUI *m_pMiddlename;
	CEditUI *m_pLastname;

	CEditUI *m_pPhone;

	CEditUI *m_pAddress1;
	CEditUI *m_pAddress2;

	CEditUI *m_pCity;
	CEditUI *m_pState;
	CEditUI *m_pZip;
};