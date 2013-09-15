#if !defined(AFX_LISTEN_H__FE5FEE1C_D4EA_4C73_9084_FE95C61D9769__INCLUDED_)
#define AFX_LISTEN_H__FE5FEE1C_D4EA_4C73_9084_FE95C61D9769__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Listen.h : header file
//



/////////////////////////////////////////////////////////////////////////////
// CListen command target

class CListen : public CAsyncSocket
{
// Attributes
public:

// Operations
public:
	CListen();
	virtual ~CListen();

// Overrides
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListen)
	public:
	virtual void OnAccept(int nErrorCode);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CListen)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

// Implementation
protected:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISTEN_H__FE5FEE1C_D4EA_4C73_9084_FE95C61D9769__INCLUDED_)
