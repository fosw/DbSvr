// DlgPw.cpp : implementation file
//

#include "stdafx.h"
#include "DbSvr.h"
#include "DlgPw.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgPw dialog


CDlgPw::CDlgPw(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgPw::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgPw)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_nCheckPW = 0;
}


void CDlgPw::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgPw)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgPw, CDialog)
	//{{AFX_MSG_MAP(CDlgPw)
	ON_BN_CLICKED(IDC_BTN_IN, OnBtnIn)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgPw message handlers

bool CDlgPw::IsCheckPwOk()
{
	if( !strcmp( m_csPw , "zkdlfxy" ) )
		return true;

	return false;
}


void CDlgPw::OnBtnIn() 
{
	// TODO: Add your control notification handler code here

	GetDlgItemText( IDC_ED_PASSWORD , m_csPw );

	CDialog::OnOK();
}


