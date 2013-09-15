#if !defined(AFX_DLGPW_H__37FD8073_58D1_42E3_8427_65780CDA129E__INCLUDED_)
#define AFX_DLGPW_H__37FD8073_58D1_42E3_8427_65780CDA129E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgPw.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgPw dialog

class CDlgPw : public CDialog
{
// Construction
public:
	CDlgPw(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgPw)
	enum { IDD = IDD_DLG_PW };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgPw)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

public:

	bool IsCheckPwOk();
	CString m_csPw;
	int m_nCheckPW;

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgPw)
	afx_msg void OnBtnIn();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGPW_H__37FD8073_58D1_42E3_8427_65780CDA129E__INCLUDED_)
