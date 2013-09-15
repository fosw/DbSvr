// DbSvr.h : main header file for the DbSvr application
//

#if !defined(AFX_DbSvr_H__B198BC6A_7144_4849_9F90_C72F8B1700B4__INCLUDED_)
#define AFX_DbSvr_H__B198BC6A_7144_4849_9F90_C72F8B1700B4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CDbSvrApp:
// See DbSvr.cpp for the implementation of this class
//

class CDbSvrApp : public CWinApp
{
public:
	CDbSvrApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDbSvrApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CDbSvrApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DbSvr_H__B198BC6A_7144_4849_9F90_C72F8B1700B4__INCLUDED_)
