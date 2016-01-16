#pragma 
#define UILIB_STATIC
#include "..\DuiLib\UIlib.h"
#include "resource.h"

#ifdef _DEBUG
#   ifdef _UNICODE
#       pragma comment(lib, "..\\bin\\DuiLib_ud.lib")
#   else
#       pragma comment(lib, "..\\bin\\DuiLib_d.lib")
#   endif
#else
#   ifdef _UNICODE
#       pragma comment(lib, "..\\bin\\DuiLib_u.lib")
#   else
#       pragma comment(lib, "..\\bin\\DuiLib.lib")
#   endif
#endif

using namespace DuiLib;

#include "MenuWnd.h"

class CDuiFrameWnd : public WindowImplBase
{
public:
	virtual LPCTSTR    GetWindowClassName() const   { return _T("AddressBookDUIMainFrame"); }
	//ResZip
	virtual UILIB_RESOURCETYPE GetResourceType() const { return UILIB_ZIPRESOURCE; }
	virtual LPCTSTR GetResourceID() const			{ return (LPCTSTR)IDR_ZIPRES1; }
	virtual CDuiString GetSkinFolder()              { return _T(""); }
	virtual CDuiString GetSkinFile()                { return _T("skin.xml"); }
	//FileZip
	//virtual UILIB_RESOURCETYPE GetResourceType() const { return UILIB_ZIP; }
	//virtual CDuiString GetZIPFileName() const		{ return _T("360SafeRes.zip"); }
	//virtual CDuiString GetSkinFile()                { return _T("skin.xml"); }
	//virtual CDuiString GetSkinFolder()              { return _T("skin"); }
	//
	//virtual CDuiString GetSkinFolder()              { return _T("skin"); }
	//virtual CDuiString GetSkinFile()                { return _T("skin.xml"); }

	virtual LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		PostQuitMessage(0);
		bHandled = TRUE;
		return 1;
	};

	virtual void Notify(TNotifyUI& msg)
	{
		if (msg.sType == DUI_MSGTYPE_CLICK)
		{
			if (msg.pSender->GetName() == "w_close")
				this->Close();
		}
		else if (msg.sType == _T("menu"))
		{
			if (msg.pSender->GetName() != _T("AddressList")) return;
			CMenuWnd* pMenu = new CMenuWnd();
			if (pMenu == NULL) { return; }
			POINT pt = { msg.ptMouse.x, msg.ptMouse.y };
			::ClientToScreen(*this, &pt);
			pMenu->Init(msg.pSender, pt);
		}
	};
};

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	CPaintManagerUI::SetInstance(hInstance);

	CDuiFrameWnd duiFrame;
	duiFrame.Create(NULL, _T("DUIWnd"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	duiFrame.CenterWindow();
	duiFrame.ShowWindow();
	CPaintManagerUI::MessageLoop();
	CPaintManagerUI::Term();
	return 0;
}