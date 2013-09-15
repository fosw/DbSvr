// ClientSock.cpp : implementation file
//

#include "stdafx.h"
#include "DbSvr.h"
#include "ClientSock.h"

#include "cLockMo.h"

#include "cExcept.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern void cLF_LogError( int nLevel , ... );
extern void cLF_LogFlow( int nLevel , ... );

char G_FixKey[] = { "����ī���Ϸ�Ű�⹮������1979�����2010�������´پ��ѱ۰�����ڰ�����������69���ǹ̹�ǥ������,�̼��ϴ���Ҽ���������å�̴�.���ڰ�" };

void tagMSG_PACK::encode_body()
{
	m_nEncodeKey = rand() %256;

	int nKeyPos = m_nEncodeKey /2;

	int nsize = GetBodySize();	

	for( int n = 0 ; n < nsize ; n++ )
	{
		Data[n] ^= m_nCheckSum;
		Data[n] ^= G_FixKey[nKeyPos];

		nKeyPos++;

		if( nKeyPos >= 128 )
			nKeyPos = 0;
	}
}

void tagMSG_PACK::decode_body()
{
	int nKeyPos = m_nEncodeKey /2;

	int nsize = GetBodySize();

	for( int n = 0 ; n < nsize ; n++ )
	{			
		Data[n] ^= G_FixKey[nKeyPos];

		nKeyPos++;

		if( nKeyPos >= 128 )
			nKeyPos = 0;

		Data[n] ^= m_nCheckSum;
	}
}


MSG_PACK* setmsg( unsigned char* pdata , int nsize , int nMsgGroup , int nType );

MSG_PACK* setmsg( unsigned char* pdata , int nsize , int nMsgGroup , int nType )
{// �޼��� ���� 
	if( nsize > MSG_MAX )	// 2048 �� ���� 
		return NULL;

	MSG_PACK* pMsg = (MSG_PACK*)new char[ nsize +sizeof( MSG_HEAD ) ];		// �޸� �Ҵ�

	memcpy( pMsg->Data , pdata , nsize );	// ����Ÿ ���� 

	pMsg->m_nMsgG = nMsgGroup;		// �޼��� �׷�
	pMsg->m_nType = nType;			// �޼��� ����

	pMsg->m_nMsgSize = nsize +sizeof( MSG_HEAD );	// �޼��� ������ ���
	pMsg->Init();	// �ð� , �޼��� ����Ű ���� 

	pMsg->makechecksum( nsize );	// ý��
	pMsg->encode_body();			// �ٵ� 
	pMsg->encode_head();			// ��� 

	return pMsg;
}


/////////////////////////////////////////////////////////////////////////////
// CClientSock

CClientSock::CClientSock()
{
	Reset();
}

CClientSock::~CClientSock()
{
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CClientSock, CAsyncSocket)
	//{{AFX_MSG_MAP(CClientSock)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0




/////////////////////////////////////////////////////////////////////////////
// CClientSock member functions

extern void cSk_PostMsg( WPARAM wparam , LPARAM lparam );

void CClientSock::OnReceive(int nErrorCode) 
{

_INIT_STCPP
_STCPP

	// TODO: Add your specialized code here and/or call the base class
	DWORD dwReceivedSize = 0;
	IOCtl( FIONREAD , &dwReceivedSize );

	if( dwReceivedSize > m_nRecvMaxLog )	// ������ ���� �ִ� ������ ��Ϻ��� ũ�ٸ�
		m_nRecvMaxLog = dwReceivedSize;		// ���� �ִ� ���� ������ ���
	
	if( dwReceivedSize > ( MAX_RECV_BUFF -m_nBuffSize ) )	// �ִ� ���� ���� ������� ũ�ٸ� 
	{
		cLF_LogError( 10 , "[CClientSock::OnReceive] dwReceivedSize(%d) > MAX_RECV_BUFF(%d) -m_nBuffSize(%d)",
			dwReceivedSize , MAX_RECV_BUFF , m_nBuffSize );

		dwReceivedSize = ( MAX_RECV_BUFF -m_nBuffSize );	// ���� ���� �ִ� ������� ���� 
	}


	m_nRecvLastLog = dwReceivedSize;		// ������ ���� ������ 

	{
		cLogBuff buffLock;
		if( m_nBuffSize == m_nReadPos )	// �̹� ���� �Ŷ� ó���� �Ŷ� ���ٸ�
		{
			m_nBuffSize = 0;
			m_nReadPos = 0;
		}
		else
		{
			if( m_nBuffSize > m_nReadPos && m_nReadPos > 0 )	// �Ϻ� ó�� �ߴٸ� 
			{
				memmove( &m_buff[ 0 ] , &m_buff[ m_nReadPos ] , m_nBuffSize -m_nReadPos );

				m_nBuffSize -= m_nReadPos;
				m_nReadPos -= m_nReadPos;
			}
		}
	}

	m_nLastTime = GetTickCount();

	int nRet = Receive( &m_buff[ m_nBuffSize ] , dwReceivedSize );

	if( nRet > 0 )
	{
		m_nBuffSize += nRet;

		if( GetRecvSize() >= sizeof( MSG_HEAD ) )
		{
			MSG_HEAD tHead;
			memcpy( &tHead , m_buff , sizeof( MSG_HEAD ) );
			tHead.decode_head();

			if( GetRecvSize() >= tHead.m_nMsgSize )
			{
				cSk_PostMsg( (WPARAM)this, (LPARAM)1 );
			}
		}
	}

	CAsyncSocket::OnReceive(nErrorCode);

_EDCPP_ALL( CClientSock::OnReceive , ; )
}

void CClientSock::Reset()
{
	memset( m_buff , 0x00 , sizeof( m_buff ) );
	m_nBuffSize = 0;
	m_nReadPos = 0;

	m_szComInfo = "";
	m_szID = "";
	m_szIP = "";

	m_strWebkey = "";

	m_dwCloseTime = 0;

	m_nRecvMaxLog = 0;
	m_nRecvLastLog = 0;	

	m_nLastTime = 0;


	m_nThread = 0;
	m_nPingCount = 0;


	m_nState = 0;
}



void CClientSock::SetCloseTime()
{
	m_dwCloseTime = GetTickCount();
}

bool CClientSock::IsValid( bool block )
{
	if( block )
	{
cSockLock sync( &m_sockLock );

		if( INVALID_SOCKET != m_hSocket )
			return true;
		else
			return false;
	}
	else
	{
		if( INVALID_SOCKET != m_hSocket )
			return true;
		else
			return false;
	}
}

void CClientSock::OnClose(int nErrorCode) 
{
	// TODO: Add your specialized code here and/or call the base class
	cLF_LogFlow( 100 , "[CClientSock::OnClose] m_hSocket(%d) nErrorCode(%d)" , m_hSocket, nErrorCode );

	ShutDown( 2 );

	if( IsValid() )
		cSk_PostMsg( (WPARAM)this, (LPARAM)3 );	

	CAsyncSocket::OnClose(nErrorCode);
}

int CClientSock::Send(const void* lpBuf, int nBufLen, int nFlags) 
{
	// TODO: Add your specialized code here and/or call the base class
	BYTE* pData = (BYTE*)lpBuf;
	int nDataSize = nBufLen;

	int nSendByte = 0;
	int nRet = 0;
	
	int nLimit = 0;

	do
	{
		nRet = CAsyncSocket::Send( (void*)&pData[ nSendByte ] , ( nDataSize -nSendByte ) );

		if( SOCKET_ERROR == nRet )
		{
			int nErr = GetLastError();

			if( WSAEWOULDBLOCK != nErr )
			{
				cLF_LogError( 10 , "[CClientSock::Send] nErr(%d) m_hSocket(%d) " , nErr , m_hSocket );

				cSk_PostMsg( (WPARAM)this, (LPARAM)2 );
				return -1;
			}
			else
				Sleep( 20 );
		}
		else
		{
			nSendByte += nRet;
			if( nSendByte != nRet )
				nRet = nRet;
		}

		nLimit++;

		if( nLimit > 50 )
		{
			cLF_LogError( 10000 , "[CClientSock::Send] nLimit(%d) nDataSize(%d) nSendByte(%d)" ,
				nLimit , nDataSize , nSendByte );
			break;
		}

	}while( nDataSize > nSendByte );

	//return CAsyncSocket::Send(lpBuf, nBufLen, nFlags);
	return nSendByte;
}

void CClientSock::End()
{
	cLF_LogFlow( 10 , "[CClientSock::End] m_hSocket(%d)" , m_hSocket );

	SetCloseTime();
	ShutDown( 2 );

	if( IsValid() )
		Close();	
}

bool CClientSock::IsUsable( DWORD dwCurTime )
{
	if( 0 == m_dwCloseTime )	// ��� �� �Ѱ� , �ʱ�ȭ �� �� 
		return true;

	if( dwCurTime -m_dwCloseTime > 3000 )	// ������ ��
		return true;

	return false;
}
