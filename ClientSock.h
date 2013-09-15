#if !defined(AFX_CLIENTSOCK_H__786B3DDA_7827_4C2B_808A_6E2E131DAB2C__INCLUDED_)
#define AFX_CLIENTSOCK_H__786B3DDA_7827_4C2B_808A_6E2E131DAB2C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ClientSock.h : header file
//



/////////////////////////////////////////////////////////////////////////////
// CClientSock command target

//#pragma pack(1)

typedef struct tagLOGIN
{	
	tagLOGIN()
	{
		memset( id , 0x00 , sizeof( id ) );
		memset( pw , 0x00 , sizeof( pw ) );
		memset( ip , 0x00 , sizeof( ip ) );

		m_nGameType = 0;
	}

	char id[ 24 ];
	char pw[ 52 ];
	char ip[ 20 ];

	int m_nGameType;

}LOGIN , *PLOGIN;			// 로그인 	


typedef struct tagLOGIN_RESP
{
	tagLOGIN_RESP()
	{
		nRetCode = 0;
		nPoint = 0;
		memset( id , 0x00 , sizeof( id ) );
		memset( nick , 0x00 , sizeof( nick ) );
		memset( webkey , 0x00 , sizeof( webkey ) );
	}

	int nRetCode;
	__int64 nPoint;
	char id[ 24 ];
	char nick[ 24 ];
	char webkey[ 40 ];
}LOGIN_RESP , *PLOGIN_RESP;




enum eREQ_CMD{ eCMD_NONE , eRecentUser ,
 eNotice , eNoticeContent ,
 eChargeDlg , eChargeReq ,
 eExchangeDlg , eExchangeReq ,
 eQaListDlg , eQaLine , eQaSend , eJoin , eRefresh , eBoxDlg , eBoxUse , eBoxPwd , eCoupon };


typedef struct tagREQ_CMD_HEAD
{
	tagREQ_CMD_HEAD()
	{
		nCMD = 0;
	}

	int nCMD;			//	4
}REQ_CMD_HEAD , *PREQ_CMD_HEAD;

typedef struct tagREQ_CMD : public tagREQ_CMD_HEAD
{	
	tagREQ_CMD()
	{
		//nCMD = 0;
		nCount = 0;
		nDbIdx = -1;
		nPoint = 0;

		memset( id , 0x00 , sizeof( id ) );
		memset( username , 0x00 , sizeof( username ) );

		memset( banknum , 0x00 , sizeof( banknum ) );
		memset( bank , 0x00 , sizeof( bank ) );
		memset( tel , 0x00 , sizeof( tel ) );

		memset( expw , 0x00 , sizeof( expw ) );

		memset( szTitle , 0x00 , sizeof( szTitle ) );
		memset( szContent , 0x00 , sizeof( szContent ) );
	}

	//int nCMD;
	int nCount;
	int nDbIdx;
	__int64 nPoint;
	char id[ 24 ];
	char username[ 24 ];
	char expw[ 30 ];
	char banknum[ 50 ];
	char bank[ 50 ];

	char tel[ 20 ];

	char szTitle[ 100 ];
	char szContent[ 1000 ];

}REQ_CMD , *PREQ_CMD;		

typedef struct tagREQ_CMD2 : tagREQ_CMD_HEAD
{	
	tagREQ_CMD2()
	{
		//nCMD = 0;

		memset( username , 0x00 , sizeof( username ) );
		memset( id , 0x00 , sizeof( id ) );
		memset( pwd , 0x00 , sizeof( pwd ) );

		memset( bank , 0x00 , sizeof( bank ) );
		memset( banknum , 0x00 , sizeof( banknum ) );
		memset( bankUser , 0x00 , sizeof( bankUser ) );

		memset( tel , 0x00 , sizeof( tel ) );
		memset( recomid , 0x00 , sizeof( recomid ) );
	}

	//int nCMD;
	
	char username[ 24 ];	// 24
	char id[ 24 ];			// 48
	char pwd[ 24 ];			// 72
	
	char bank[ 50 ];		// 122
	char banknum[ 50 ];		// 172
	char bankUser[ 24 ];	// 196

	char ex_pwd[ 24 ];		// 220
	
	char tel[ 20 ];			// 240
	char recomid[ 20 ];		// 260	
}REQ_CMD2 , *PREQ_CMD2;	


typedef struct tagRESP_CMD
{	
	tagRESP_CMD()
	{
		nRet = 0;
		memset( szMsg , 0x0 , sizeof( szMsg ) );
		
		nPoint = 0;
		nCount = 0;
		nPoint2 = 0;
		nBoxPoint = 0;
	}

	int nRet;
	int nPoint;
	int nCount;

	int nPoint2;
	int nBoxPoint;
	char szMsg[ 196 ];

}RESP_CMD , *PRESP_CMD;		


typedef struct tagRECENT_USER
{	
	tagRECENT_USER()
	{
		memset( szUserID , 0x00 , sizeof( szUserID ) );
		point = 0;
		nIdx = 0;
	}

	char szUserID[ 24 ];
	__int64 point;
	int nIdx;

}RECENT_USER , *PRECENT_USER;



typedef struct tagRESP_RECENT_USER
{	
	tagRESP_RECENT_USER()
	{
		nAdd = 0;
		nCount = 0;
	}

	int nAdd;
	int nCount;
	RECENT_USER user[10];
}RESP_RECENT_USER , *PRESP_RECENT_USER;


typedef struct tagPING
{	
	tagPING()
	{
		memset( id , 0x00 , sizeof( id ) );
		memset( pw , 0x00 , sizeof( pw ) );
		memset( key , 0x00 , sizeof( key ) );
	}

	char id[ 20 ];
	char pw[ 20 ];
	char key[ 20 ];
}PING , *PPING;			// 로그아웃  	



//////////////////////////////
typedef struct tagNOTICE_LINE
{	
	tagNOTICE_LINE()
	{
		memset( szTitle , 0x00 , sizeof( szTitle ) );
		memset( szDate , 0x00 , sizeof( szDate ) );
		nIdx = 0;
	}

	char szTitle[100];
	char szDate[20];
	int nIdx;

}NOTICE_LINE , *PNOTICE_LINE;


typedef struct tagRESP_NOTICE
{	
	tagRESP_NOTICE()
	{
		nAdd = 0;
		nCount = 0;
	}

	int nAdd;
	int nCount;
	NOTICE_LINE notice[10];
}RESP_NOTICE , *PRESP_NOTICE;


typedef struct tagNOTICE_CONTENT
{	
	tagNOTICE_CONTENT()
	{
		nRet = 0;

		memset( szTitle , 0x00 , sizeof( szTitle ) );
		memset( szDate , 0x00 , sizeof( szDate ) );
		memset( szContent , 0x00 , sizeof( szContent ) );		
	}

	int nRet;
	char szTitle[100];
	char szDate[20];
	char szContent[1000];

}NOTICE_CONTENT , *PNOTICE_CONTENT;





typedef struct tagCHARGE_DLG_RESP
{	
	tagCHARGE_DLG_RESP()
	{
		nRet = 0;

		memset( szBank , 0x00 , sizeof( szBank ) );
		memset( szBankUser , 0x00 , sizeof( szBankUser ) );
		memset( szBankNum , 0x00 , sizeof( szBankNum ) );
		nUserPoint = 0;		
	}

	int nRet;
	char szBank[ 50 ];
	char szBankUser[ 20 ];
	char szBankNum[ 50 ];
	__int64 nUserPoint;

}CHARGE_DLG_RESP , *PCHARGE_DLG_RESP;


/////////////////////////////

typedef struct tagEXCHANGE_DLG_RESP
{	
	tagEXCHANGE_DLG_RESP()
	{
		nRet = 0;
		nUserPoint = 0;
		
		memset( bank , 0x00 , sizeof( bank ) );
		memset( bankNum , 0x00 , sizeof( bankNum ) );
		memset( accname , 0x00 , sizeof( accname ) );
		
	}

	int nRet;
	__int64 nUserPoint;

	char bank[40];
	char bankNum[80];
	char accname[40];

}EXCHANGE_DLG_RESP , *PEXCHANGE_DLG_RESP;




//////////////////////////////
typedef struct tagQA_LIST
{	
	tagQA_LIST()
	{
		memset( szTitle , 0x00 , sizeof( szTitle ) );
		memset( szDate , 0x00 , sizeof( szDate ) );
		memset( szWriter , 0x00 , sizeof( szWriter ) );

		nIdx = 0;
		nOriIdx = 0;
	}

	char szTitle[100];
	char szDate[20];
	char szWriter[30];
	int nIdx;
	int nOriIdx;

}QA_LIST , *PQA_LIST;


typedef struct tagRESP_QA_LIST
{	
	tagRESP_QA_LIST()
	{
		nAdd = 0;
		nCount = 0;
	}

	int nAdd;
	int nCount;
	QA_LIST qalist[10];

}RESP_QA_LIST , *PRESP_QA_LIST;



typedef struct tagRESP_QA_CONTENT
{	
	tagRESP_QA_CONTENT()
	{
		memset( szTitle , 0x00 , sizeof( szTitle ) );
		memset( szDate , 0x00 , sizeof( szDate ) );
		memset( szWriter , 0x00 , sizeof( szWriter ) );
		memset( szContent , 0x00 , sizeof( szContent ) );

		nIdx = 0;
		nOriIdx = 0;
		nRet = 0;
	}

	char szTitle[100];
	char szDate[20];
	char szWriter[30];
	char szContent[1000];
	int nIdx;
	int nOriIdx;
	int nRet;

}RESP_QA_CONTENT , *PRESP_QA_CONTENT;


//////////////////////////////

#define MSG_MAX 2048



typedef struct tagMSG_HEAD
{// 메세지 헤더 
	tagMSG_HEAD()
	{
		m_nMsgSize = 0;
		m_nCheckSum = 0;
		m_nEncodeKey = 0;

		m_nKey = 0;
		m_nTime = 0;

		m_nMsgG = 0;
		m_nType = 0;
	}

	void Init()
	{// 메세지 초기화 
		m_nKey = rand() % 65536;				// 바디 키 
		m_nTime = GetTickCount();		// 시각 		
	}

	bool IsHeadOk()
	{// 헤더 정보 확인 
		if( m_nMsgSize <= 0 )
			return false;

		if( m_nMsgSize > MSG_MAX )
			return false;

		return true;
	}

	void encode_head()
	{
		unsigned char* p = (unsigned char*)this;
		unsigned char Key = m_nCheckSum;
		unsigned char backKey = m_nCheckSum;

		for( int k = 0; k < sizeof( MSG_HEAD ) ; k++ )
			p[k] ^= Key;

		m_nCheckSum = backKey;
	}

	void decode_head()
	{
		unsigned char* p = (unsigned char*)this;
		unsigned char Key = m_nCheckSum;
		unsigned char backKey = m_nCheckSum;

		for( int k = 0; k < sizeof( MSG_HEAD ) ; k++ )
			p[k] ^= Key;

		m_nCheckSum = backKey;
	}
	
	short m_nMsgSize;		// 헤더 + 바디 전체 의 길이 
	unsigned char m_nCheckSum;		// 바디 첵섬 
	unsigned char m_nEncodeKey;		// 바디 암호화 문자열 위치 키 


	unsigned char m_nMsgG;			// 메세지 종류 그룹
	unsigned char m_nType;			// 메세지 종류 
	unsigned short m_nKey;			// 메세지 송수신 확인 키

	unsigned int m_nTime;

}MSG_HEAD, *PMSG_HEAD;

typedef struct tagMSG_PACK : public MSG_HEAD
{
	tagMSG_PACK()
	{
	
	}

	int GetBodySize() 
	{
		return ( m_nMsgSize -sizeof( MSG_HEAD ) );
	}
	
	void encode_body();
	void decode_body();

	void makechecksum( int ndatasize )
	{	// m_nKey , m_nTime 를 포함해서 변동 되는 첵섬을 만든다.
		int nsize = GetBodySize();
		m_nCheckSum = 0;

		for( int n = 0 ; n < nsize ; n++ )
			m_nCheckSum ^= Data[n];
	}

	bool IsOkCheckSum()
	{// 복호화 후 첵섬 확인 
		int nsize = GetBodySize();

		unsigned char nCheckSum = 0;
		for( int n = 0 ; n < nsize ; n++ )
			nCheckSum ^= Data[n];

		return ( m_nCheckSum == nCheckSum );
	}

	BYTE Data[ 1 ];		// 바디 데이타 

}MSG_PACK , *PMSG_PACK;

//#pragma pack()

class cSockLock
{// 에러 로그용 락
public:

	cSockLock( CCriticalSection* p )
	{
		m_pLock = NULL;
		m_pLock = p;

		m_pLock->Lock();
	}

	~cSockLock()
	{
		if( m_pLock )
			m_pLock->Unlock();
	}

	CCriticalSection* m_pLock;

};


#define UM_RECEIVE_MSG	WM_USER +100
#define UM_SHOW_AVAIL	WM_USER +101


const int MAX_RECV_BUFF	= 10240;
class CClientSock : public CAsyncSocket
{
// Attributes
public:

// Operations
public:
	CClientSock();
	virtual ~CClientSock();

// Overrides
public:
	void End();
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClientSock)
	public:
	virtual void OnReceive(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	virtual int Send(const void* lpBuf, int nBufLen, int nFlags = 0);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CClientSock)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	bool IsValid( bool block = true );
	void Reset();
	bool IsUsable( DWORD dwCurTime );

	CCriticalSection m_sockLock;

	char m_buff[ MAX_RECV_BUFF ];
	DWORD GetRecvSize() { return ( m_nBuffSize -m_nReadPos ); }

	DWORD m_nBuffSize;
	DWORD m_nReadPos;
	
	CString m_szComInfo;
	CString m_szID;
	CString m_szIP;

	CString m_strWebkey;
	
	DWORD m_nStartTime;
	DWORD m_nSendReel;

	int m_nThread;		// 소속 스레드
	int m_nPingCount;

	DWORD m_dwCloseTime;

	DWORD m_nRecvMaxLog;
	DWORD m_nRecvLastLog;
	DWORD m_nLastTime;

	void SetCloseTime();

	int m_nState;

// Implementation
protected:


};




/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLIENTSOCK_H__786B3DDA_7827_4C2B_808A_6E2E131DAB2C__INCLUDED_)
