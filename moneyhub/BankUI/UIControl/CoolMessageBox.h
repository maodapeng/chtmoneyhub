
#pragma once

#include "..\Resource\Resource.h"
#include "..\Skin\SkinManager.h"
#include "AltSkinClasses.h"

#include "Util.h"
#define MH_STARTDELAYEVENT (0xfe02)

class CCoolMessageBox : public CDialogImpl<CCoolMessageBox>, public CDialogSkinMixer<CCoolMessageBox>
{
	typedef CDialogSkinMixer<CCoolMessageBox> baseClass;

public:
	CCoolMessageBox(LPCTSTR lpText, LPCTSTR lpCaption, UINT uType, bool bDisappear = false)
		: m_strText(lpText), m_strCaption(lpCaption), m_uType(uType), m_nButtonShowNum(0),m_bDisappear(bDisappear)
	{
		m_rcMsgBoxIcon.SetRect(20, 50, 20 + 36, 50 + 36);
	}

public:
	enum { IDD = IDD_DIALOG_MSGBOX };

	BEGIN_MSG_MAP(CCoolMessageBox)
		CHAIN_MSG_MAP(CDialogSkinMixer<CCoolMessageBox>)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		COMMAND_RANGE_HANDLER(IDC_BUTTON_1, IDC_BUTTON_4, OnButtonClicked)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

public:

	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		if(wParam == MH_STARTDELAYEVENT)
		{
			KillTimer(MH_STARTDELAYEVENT);
			EndDialog(m_uCancelRet);
		}
		return 0;
	}

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if(m_bDisappear == true)
		{
			RECT prect,trect;
			GetWindowRect(&trect);
			HWND hMainFrame = FindWindow(_T("MONEYHUB_MAINFRAME"), NULL);
			if(hMainFrame)
			{
				::GetWindowRect(hMainFrame, &prect);
				::SetWindowPos(m_hWnd, NULL, prect.right - (trect.right - trect.left) - 8, prect.bottom - (trect.bottom - trect.top) - 25, 0, 0,SWP_NOSIZE);
			}
			else
			{
				CenterWindow(GetDesktopWindow());
			}
			
			SetTimer(MH_STARTDELAYEVENT, 5 * 1000 ,NULL);
		}
		else
		{
			SetWindowPos(HWND_TOP, 0, 0, 0, 0,SWP_NOSIZE);
			CenterWindow(GetDesktopWindow());
		}
		SetWindowText(m_strCaption);

		return 0;
	}

	void PreCreate()
	{
		for (int i = 0; i < sizeof(m_btnButton) / sizeof(CSkinButton); i++)
		{
			m_btnButton[i] = GetDlgItem(IDC_BUTTON_1 + i);
			m_btnButton[i].ApplySkin(&m_imgDlgButton, 4);
			m_bButtonShowFlag[i] = 0;
		}

		LoadMsgBoxIcons();
		ShowOrHideButtons();
		CalcMsgBoxSize();
		CalcButtonsPos();
	}

	void CalcMsgBoxSize()
	{
		CDC dc(GetDC());
		dc.SelectFont(m_fontText);

		m_rcMsgBoxText.SetRect(0, 0, 0, 0);
		dc.DrawText(m_strText, -1, &m_rcMsgBoxText, DT_CALCRECT);

		int nWidth = m_rcMsgBoxText.Width() + 80;
		int nHeight = m_rcMsgBoxText.Height() + 120;

		CRect rcButton;
		m_btnButton[0].GetWindowRect(&rcButton);
		int nMinWidth = m_nButtonShowNum * (rcButton.Width() + 10) + 10;
		if (nWidth < nMinWidth)
			nWidth = nMinWidth;

		SetWindowPos(NULL, 0, 0, nWidth, nHeight, 0);
	}

	void CalcButtonsPos()
	{
		CRect rcButton;
		m_btnButton[0].GetWindowRect(&rcButton);
		int nButtonsWidth = m_nButtonShowNum * (rcButton.Width() + 10) - 10;

		CRect rc;
		GetClientRect(&rc);

		int x = (rc.Width() - nButtonsWidth) / 2;
		int y = rc.bottom - rcButton.Height() - 12;

		for (int i = 0; i < sizeof(m_btnButton) / sizeof(CSkinButton); i++)
		{
			if (m_bButtonShowFlag[i])
			{
				m_btnButton[i].SetWindowPos(NULL, x, y, rcButton.Width(), rcButton.Height(), 0);
				x += rcButton.Width() + 10;
			}
		}
	}

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		CPaintDC dc(m_hWnd);

		CRect rect;
		GetClientRect(&rect);

		// 图标
		int nIcon = 0;
		if ((m_uType & 0x000000F0) == MB_ICONQUESTION)
			nIcon = 1;
		else if ((m_uType & 0x000000F0) == MB_ICONWARNING)
			nIcon = 2;
		else if ((m_uType & 0x000000F0) == MB_ICONSTOP)
			nIcon = 3;
		DrawMessageBoxIcon(dc, m_rcMsgBoxIcon, nIcon);

		// 文字
		CRect rcDraw(m_rcMsgBoxIcon.right, m_rcMsgBoxIcon.top, 
			m_rcMsgBoxIcon.right + m_rcMsgBoxText.Width(),
			m_rcMsgBoxIcon.top + m_rcMsgBoxText.Height());

		rcDraw.OffsetRect(4, 0);
		if (rcDraw.Height() < m_rcMsgBoxIcon.Height())
			rcDraw.OffsetRect(0, (m_rcMsgBoxIcon.Height() - rcDraw.Height()) / 2);

		dc.SetBkMode(TRANSPARENT);
		dc.SelectFont(m_fontText);
		dc.SetTextColor(m_crTextColor);
		::DrawText(dc, m_strText, -1, &rcDraw, DT_VCENTER);

		return 0;
	}

	LRESULT OnButtonClicked(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		EndDialog(m_nRetVal[wID - IDC_BUTTON_1]);

		return 0;
	}

	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		EndDialog(m_uCancelRet);

		return 0;
	}

	bool DoSysClose()
	{
		EndDialog(m_uCancelRet);

		return true;
	}

	void ShowOrHideButtons()
	{
		CRect rcButtons[4];

		for (int i = 0; i < sizeof(m_nRetVal) / sizeof(int); i++)
		{
			m_btnButton[i].GetWindowRect(rcButtons[i]);
			ScreenToClient(rcButtons[i]);
		}

		int nHelpDiff;
		if ((m_uType & 0x0000F000) == MB_HELP)
		{
			nHelpDiff = 1;
			m_nRetVal[3] = IDHELP;
			m_btnButton[3].ShowWindow(SW_SHOW);
			m_nButtonShowNum += 1;
			m_bButtonShowFlag[3] = 1;
		}
		else
		{
			nHelpDiff = 0;
			m_btnButton[3].ShowWindow(SW_HIDE);
		}

		if ((m_uType & 0x0000000F) == MB_OK)
		{
			m_nRetVal[0] = IDOK;
			m_bButtonShowFlag[0] = 1;
			m_btnButton[0].SetWindowText(_T("确定"));
			m_btnButton[0].MoveWindow(rcButtons[3 - nHelpDiff]);

			m_btnButton[1].ShowWindow(SW_HIDE);
			m_btnButton[2].ShowWindow(SW_HIDE);

			m_uCancelRet = IDOK;
			m_nButtonShowNum += 1;
		}
		else if ((m_uType & 0x0000000F) == MB_OKCANCEL)
		{
			m_nRetVal[0] = IDOK;
			m_bButtonShowFlag[0] = 1;
			m_btnButton[0].SetWindowText(_T("确定"));
			m_btnButton[0].MoveWindow(rcButtons[2 - nHelpDiff]);

			m_nRetVal[1] = IDCANCEL;
			m_bButtonShowFlag[1] = 1;
			m_btnButton[1].SetWindowText(_T("取消"));
			m_btnButton[1].MoveWindow(rcButtons[3 - nHelpDiff]);

			m_btnButton[2].ShowWindow(SW_HIDE);

			m_uCancelRet = IDCANCEL;
			m_nButtonShowNum += 2;
		}
		else if ((m_uType & 0x0000000F) == MB_ABORTRETRYIGNORE)
		{
			m_nRetVal[0] = IDABORT;
			m_bButtonShowFlag[0] = 1;
			m_btnButton[0].SetWindowText(_T("中止"));
			m_btnButton[0].MoveWindow(rcButtons[1 - nHelpDiff]);

			m_nRetVal[1] = IDRETRY;
			m_bButtonShowFlag[1] = 1;
			m_btnButton[1].SetWindowText(_T("重试"));
			m_btnButton[1].MoveWindow(rcButtons[2 - nHelpDiff]);

			m_nRetVal[2] = IDIGNORE;
			m_bButtonShowFlag[2] = 1;
			m_btnButton[2].SetWindowText(_T("忽略"));
			m_btnButton[2].MoveWindow(rcButtons[3 - nHelpDiff]);

			m_uCancelRet = IDIGNORE;
			m_nButtonShowNum += 3;
		}
		else if ((m_uType & 0x0000000F) == MB_YESNOCANCEL)
		{
			m_nRetVal[0] = IDYES;
			m_bButtonShowFlag[0] = 1;
			m_btnButton[0].SetWindowText(_T("是"));
			m_btnButton[0].MoveWindow(rcButtons[1 - nHelpDiff]);

			m_nRetVal[1] = IDNO;
			m_bButtonShowFlag[1] = 1;
			m_btnButton[1].SetWindowText(_T("否"));
			m_btnButton[1].MoveWindow(rcButtons[2 - nHelpDiff]);

			m_nRetVal[2] = IDCANCEL;
			m_bButtonShowFlag[2] = 1;
			m_btnButton[2].SetWindowText(_T("取消"));
			m_btnButton[2].MoveWindow(rcButtons[3 - nHelpDiff]);

			m_uCancelRet = IDCANCEL;
			m_nButtonShowNum += 3;
		}
		else if ((m_uType & 0x0000000F) == MB_YESNO)
		{
			m_nRetVal[0] = IDYES;
			m_bButtonShowFlag[0] = 1;
			m_btnButton[0].SetWindowText(_T("是"));
			m_btnButton[0].MoveWindow(rcButtons[2 - nHelpDiff]);

			m_nRetVal[1] = IDNO;
			m_bButtonShowFlag[1] = 1;
			m_btnButton[1].SetWindowText(_T("否"));
			m_btnButton[1].MoveWindow(rcButtons[3 - nHelpDiff]);

			m_btnButton[2].ShowWindow(SW_HIDE);

			m_uCancelRet = IDNO;
			m_nButtonShowNum += 2;
		}
		else if ((m_uType & 0x0000000F) == MB_RETRYCANCEL)
		{
			m_nRetVal[0] = IDRETRY;
			m_bButtonShowFlag[0] = 1;
			m_btnButton[0].SetWindowText(_T("重试"));
			m_btnButton[0].MoveWindow(rcButtons[2 - nHelpDiff]);

			m_nRetVal[1] = IDCANCEL;
			m_bButtonShowFlag[1] = 1;
			m_btnButton[1].SetWindowText(_T("取消"));
			m_btnButton[1].MoveWindow(rcButtons[3 - nHelpDiff]);

			m_btnButton[2].ShowWindow(SW_HIDE);

			m_uCancelRet = IDCANCEL;
			m_nButtonShowNum += 2;
		}
		else if ((m_uType & 0x0000000F) == MB_CANCELTRYCONTINUE)
		{
			m_nRetVal[0] = IDCANCEL;
			m_bButtonShowFlag[0] = 1;
			m_btnButton[0].SetWindowText(_T("取消"));
			m_btnButton[0].MoveWindow(rcButtons[1 - nHelpDiff]);

			m_nRetVal[1] = IDRETRY;
			m_bButtonShowFlag[1] = 1;
			m_btnButton[1].SetWindowText(_T("重试"));
			m_btnButton[1].MoveWindow(rcButtons[2 - nHelpDiff]);

			m_nRetVal[2] = IDCONTINUE;
			m_bButtonShowFlag[2] = 1;
			m_btnButton[2].SetWindowText(_T("继续"));
			m_btnButton[2].MoveWindow(rcButtons[3 - nHelpDiff]);

			m_uCancelRet = IDCANCEL;
			m_nButtonShowNum += 3;
		}

		CSkinButton* pDefButton = &m_btnButton[(m_uType & 0x00000F00) >> 8];
		DWORD dwStyle = ::GetWindowLong(pDefButton->m_hWnd, GWL_STYLE) & 0xFFFF;
		::SendMessage(pDefButton->m_hWnd, BM_SETSTYLE, dwStyle | BS_DEFPUSHBUTTON, (LPARAM)FALSE);
	}


public:
	void DrawMessageBoxIcon(CDCHandle dc, const RECT &rect, UINT iCurr)
	{
		ATLASSERT(iCurr >= 0 && iCurr < 4);

		RECT rcDraw = { rect.left, rect.top, 
			rect.left + m_bmpMessageBoxIcons.GetWidth() / 4, 
			rect.top + m_bmpMessageBoxIcons.GetHeight() };

		AltDrawImagePart(dc, rcDraw, m_bmpMessageBoxIcons, iCurr, 4);
	}

	bool LoadMsgBoxIcons()
	{
		bool bRet = true;

		TCHAR szPath[MAX_PATH] = { 0 };
		::GetModuleFileName(NULL, szPath, _countof(szPath));
		::PathRemoveFileSpecW(szPath);

		CString strSkinDir(szPath);
		strSkinDir += _T("\\Skin\\");		
		bRet = bRet && m_bmpMessageBoxIcons.LoadFromFile(strSkinDir + _T("msgbox_icons.png"));

		return bRet;
	}

protected:
	CRect m_rcMsgBoxIcon;
	CRect m_rcMsgBoxText;
	CSkinButton m_btnButton[4];

	int m_bButtonShowFlag[4];
	int m_nButtonShowNum;

	CString m_strText;
	CString m_strCaption;
	UINT m_uType;

	UINT m_uCancelRet;
	int m_nRetVal[4];

	bool m_bDisappear; 
	CTuoImage m_bmpMessageBoxIcons;
};


class CShowMessageDlg : public CDialogImpl<CShowMessageDlg>
{
public:
	CShowMessageDlg(LPCTSTR lpText):m_strText(lpText){}
	~CShowMessageDlg(){}

public:
	enum { IDD = IDD_DIALOG_MSGBOX };

	BEGIN_MSG_MAP(CShowMessageDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()
private:
	CRect m_rcMsgBoxText;
	CString m_strText;

	DWORD m_crTextColor;
	DWORD m_crBackColor;

	CFont m_fontText;

public:

	void PreCreate()
	{
		CalcMsgBoxSize();
	}

	void CalcMsgBoxSize()
	{
		CDC dc(GetDC());
		dc.SelectFont(m_fontText);

		m_rcMsgBoxText.SetRect(0, 0, 0, 0);
		dc.DrawText(m_strText, -1, &m_rcMsgBoxText, DT_CALCRECT);

		int nWidth = m_rcMsgBoxText.Width() + 40;
		int nHeight = m_rcMsgBoxText.Height() + 20;

		SetWindowPos(NULL, 0, 0, nWidth, nHeight, 0);
	}


	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		if(wParam == MH_STARTDELAYEVENT)
		{
			KillTimer(MH_STARTDELAYEVENT);
			DestroyWindow();			
		}
		return 0;
	}

	void OnFinalMessage(HWND hWnd)
	{
		delete this;
		PostQuitMessage(0);
	}

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{	
		if (NULL == m_fontText)
			m_fontText.CreateFont(13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, _T("Tahoma"));
		m_crTextColor = RGB(61, 98, 123);

		PreCreate();

		RECT prect,trect;
		GetWindowRect(&trect);

		HRGN hRgn = ::CreateRoundRectRgn(0, 0, trect.right - trect.left + 1, trect.bottom - trect.top + 1, 4, 4);
		SetWindowRgn(hRgn);
		DeleteObject(hRgn);

		HWND hMainFrame = FindWindow(_T("MONEYHUB_MAINFRAME"), NULL);
		if(hMainFrame)
		{
			::GetWindowRect(hMainFrame, &prect);
			::SetWindowPos(m_hWnd, NULL, prect.right - (trect.right - trect.left) - 8, prect.bottom - (trect.bottom - trect.top) - 30, 0, 0,SWP_NOSIZE);
		}
		else
		{
			CenterWindow(GetDesktopWindow());
		}

		SetTimer(MH_STARTDELAYEVENT, 5 * 1000 ,NULL);

		return 0;
	}
	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		CPaintDC dc(m_hWnd);

		CRect rcDraw;
		GetClientRect(&rcDraw);
		dc.FillSolidRect(&rcDraw, RGB(255, 255, 194));
		rcDraw.OffsetRect(20, 10);

		dc.SetBkMode(TRANSPARENT);
		dc.SelectFont(m_fontText);
		dc.SetTextColor(m_crTextColor);
		::DrawText(dc, m_strText, -1, &rcDraw, DT_VCENTER);
		return 0;
	}

};

int WINAPI mhMessageBox(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType);

int WINAPI mhShowMessage(HWND hWnd, LPCTSTR lpText);
struct MsgData
{
	int type;
	DWORD wtype;
	wstring info;
	int res;
};

class INotifyInterface
{
public:
	INotifyInterface(){};
	// 取消导入账单
	virtual void CancelGetBill() = 0;	
	virtual void GetBillExceedTime() = 0;
	virtual void SetNotifyWnd(HWND nHwnd) = 0;
};

typedef struct ShowInfoStruct  
{
	INotifyInterface* pNotifyInterface;
	wstring info;
	int type;
}BILLSHOWINFO, *LPBILLSHOWINFO;

extern CWindow* g_hJSWndInfoDlg;
DWORD WINAPI  _threadShowWaitDLG(LPVOID lp);
DWORD WINAPI  _threadShowInfoDLG(LPVOID lp);
DWORD WINAPI  _threadShowJSInfoDLG(LPVOID lp);
void WINAPI  _endShowInfoDLGthread();
void WINAPI  _endShowJSInfoDLG();