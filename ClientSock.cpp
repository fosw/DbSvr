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

char G_FixKey[] = { "무라카미하루키잡문집』은1979년부터2010년까지써온다양한글가운데저자가직접엄선한69편의미발표에세이,미수록단편소설등을엮은책이다.저자가" };

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
{// 메세지 생성 
	if( nsize > MSG_MAX )	// 2048 로 제한 
		return NULL;

	MSG_PACK* pMsg = (MSG_PACK*)new char[ nsize +sizeof( MSG_HEAD ) ];		// 메모리 할당

	memcpy( pMsg->Data , pdata , nsize );	// 데이타 복사 

	pMsg->m_nMsgG = nMsgGroup;		// 메세지 그룹
	pMsg->m_nType = nType;			// 메세지 종류

	pMsg->m_nMsgSize = nsize +sizeof( MSG_HEAD );	// 메세지 사이즈 기록
	pMsg->Init();	// 시각 , 메세지 인증키 생성 

	pMsg->makechecksum( nsize );	// 첵섬
	pMsg->encode_body();			// 바디 
	pMsg->encode_head();			// 헤더 

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

	if( dwReceivedSize > m_nRecvMaxLog )	// 기존에 받은 최대 사이즈 기록보다 크다면
		m_nRecvMaxLog = dwReceivedSize;		// 새로 최대 수신 사이즈 기록
	
	if( dwReceivedSize > ( MAX_RECV_BUFF -m_nBuffSize ) )	// 최대 수신 버퍼 사이즈보다 크다면 
	{
		cLF_LogError( 10 , "[CClientSock::OnReceive] dwReceivedSize(%d) > MAX_RECV_BUFF(%d) -m_nBuffSize(%d)",
			dwReceivedSize , MAX_RECV_BUFF , m_nBuffSize );

		dwReceivedSize = ( MAX_RECV_BUFF -m_nBuffSize );	// 여유 공간 최대 사이즈로 제한 
	}


	m_nRecvLastLog = dwReceivedSize;		// 마지막 받은 사이즈 

	{
		cLogBuff buffLock;
		if( m_nBuffSize == m_nReadPos )	// 이미 받은 거랑 처리한 거랑 같다면
		{
			m_nBuffSize = 0;
			m_nReadPos = 0;
		}
		else
		{
			if( m_nBuffSize > m_nReadPos && m_nReadPos > 0 )	// 일부 처리 했다면 
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
	if( 0 == m_dwCloseTime )	// 사용 안 한것 , 초기화 된 것 
		return true;

	if( dwCurTime -m_dwCloseTime > 3000 )	// 오래된 것
		return true;

	return false;
}
