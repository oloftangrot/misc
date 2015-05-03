// ControllerDlg.h : header file
//

#if !defined(AFX_CONTROLLERDLG_H__A92FC84A_D0C3_11D1_B00D_000000000000__INCLUDED_)
#define AFX_CONTROLLERDLG_H__A92FC84A_D0C3_11D1_B00D_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <RTSP.h>
#include <RTSP_Internals.h>

/////////////////////////////////////////////////////////////////////////////
// CControllerDlg dialog

class CControllerDlg : public CDialog
{
// Construction
public:
	CControllerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CControllerDlg)
	enum { IDD = IDD_CONTROLLER_DIALOG };
	CListBox	m_ListBox;
	CString	m_FederationStatusString;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CControllerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void LoadMDFFile(CString file_name);
    void CControllerDlg::UpdateListBox(RTSP::MsgType msg_type);
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CControllerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnInitialize();
	afx_msg void OnStart();
	afx_msg void OnHalt();
	afx_msg void OnPing();
	afx_msg void OnOpen();
	afx_msg void OnExit();
	afx_msg void OnAbout();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONTROLLERDLG_H__A92FC84A_D0C3_11D1_B00D_000000000000__INCLUDED_)
