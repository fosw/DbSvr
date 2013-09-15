// DbSvrDlg.h : header file
//

#if !defined(AFX_DbSvrDLG_H__ADBE44C8_A610_4496_86DF_B4B772FC5FF3__INCLUDED_)
#define AFX_DbSvrDLG_H__ADBE44C8_A610_4496_86DF_B4B772FC5FF3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CDbSvrDlg dialog

#include "Listen.h"

class CDbSvrDlg : public CDialog
{
// Construction
public:
	int StartServer();

	int StartThread();
	void CloseThread();

	void ReadSet();

	CDbSvrDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CDbSvrDlg)
	enum { IDD = IDD_DbSvr_DIALOG };

	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDbSvrDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL
public:
	int ConnDB();
	CListen m_Server;
	void CloseAllClient();

	void ShowStateInfo();

//	void AddLog( int nLevel , LPSTR lpszLog );
	int m_nMaxLogLine;

	bool m_nShowLog;

	int m_nLogLevel;
	
// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CDbSvrDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBStart();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnDblclkLLog();
	afx_msg BOOL OnQueryEndSession();
	afx_msg void OnBtnShow();
	afx_msg void OnBtnFindW();
	
	afx_msg void OnBtnSaveSet();
	afx_msg void OnSelendokCmbWorkType();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DbSvrDLG_H__ADBE44C8_A610_4496_86DF_B4B772FC5FF3__INCLUDED_)
