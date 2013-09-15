// Listen.cpp : implementation file
//

#include "stdafx.h"
#include "DbSvr.h"
#include "Listen.h"

#include "ClientSock.h"

#include "cLockMo.h"

extern void cLF_LogError( int nLevel , ... );
extern void cLF_LogFlow( int nLevel , ... );


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CListen

CListen::CListen()
{
}

CListen::~CListen()
{
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CListen, CAsyncSocket)
	//{{AFX_MSG_MAP(CListen)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0


extern CString g_csServerState;
extern CClientSock* cSk_GetNewClient();

/////////////////////////////////////////////////////////////////////////////
// CListen member functions



void CListen::OnAccept(int nErrorCode) 
{
	// TODO: Add your specialized code here and/or call the base class

try
{

	CClientSock* pClient = NULL;	
	pClient = cSk_GetNewClient();		// ���� ����� Ŭ���̾�Ʈ ������ �����´�.

	if( pClient )							// ������ ������ ��� 
	{
		if( !Accept( *pClient ) )			// ������ �޴´�.
		{// ���ӿ� ������ ��� 			
			g_csServerState.Format( "Accept ���� " , GetLastError() );			
		}
		else
		{// ���ӿ� ������ ��� 
			CString csAddr;
			UINT nPort;

			pClient->Reset();

			if( pClient->GetPeerName( csAddr , nPort ) )
				g_csServerState.Format( "%s %d" ,  csAddr , nPort );			
						
			g_csServerState.Format( "Accept %s " , csAddr );

			cLF_LogFlow( 10 , "[OnAccept] csAddr(%s) nPort(%d) m_hSocket(%d)" ,
				csAddr , nPort , pClient->m_hSocket );
		}
	}
	else
	{		
		g_csServerState.Format( "Ŭ���̾�Ʈ ������ �� �������� ����." );		
	}

}
catch(...)
{

}
	CAsyncSocket::OnAccept(nErrorCode);

}
