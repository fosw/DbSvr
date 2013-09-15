// DbSvrDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DbSvr.h"
#include "DbSvrDlg.h"

#include "MyADO.h"
#include "ClientSock.h"


#include "cLockMo.h"

//#include "cMemPool.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void g_strtrim( char* pstr )
{
	char* pPos = NULL;
	char* pData = pstr;
	int nCount = 0;
	int nPosCount = 0;

	int nLen = strlen( pData );
	
	while( *pData )
	{ 
		if( *pData & 0x80 )
		{
		}
		else
		{
			if( *pData == ' ' )
			{
				pPos = pData +1;
			}
		}

		pData++;

		nCount++;

		if( nCount > nLen )
			break;
	} 

	if( pPos )
		memmove( pstr , pPos , nCount +1 -( pPos - pData ) );
}

extern MSG_PACK* setmsg( unsigned char* pdata , int nsize , int nMsgGroup , int nType );
/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDbSvrDlg dialog
CDbSvrDlg* gpThis = NULL;
CDbSvrDlg::CDbSvrDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDbSvrDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDbSvrDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	gpThis = this;
	
}

void CDbSvrDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDbSvrDlg)

	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDbSvrDlg, CDialog)
	//{{AFX_MSG_MAP(CDbSvrDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_B_START, OnBStart)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_LBN_DBLCLK(IDC_L_LOG, OnDblclkLLog)
	ON_WM_QUERYENDSESSION()
	ON_BN_CLICKED(IDC_BTN_SHOW, OnBtnShow)
	ON_BN_CLICKED(IDC_BTN_FIND_W, OnBtnFindW)
	
	ON_BN_CLICKED(IDC_BTN_SAVE_SET, OnBtnSaveSet)
	ON_CBN_SELENDOK(IDC_CMB_WORK_TYPE, OnSelendokCmbWorkType)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int G_nReelPoint = 200;
int G_nSvrType = 0;
int G_nUsingIdx = 0;

typedef CList<CString,CString> STR_LIST;

STR_LIST g_LogErrorData;
STR_LIST g_LogFlowData;



void cLF_GetCurrentPath( char* pPath )
{// 현재 실행파일의 패스 

	DWORD dwRet = GetModuleFileName( NULL , pPath , _MAX_PATH );

	char* pchar = strrchr( pPath , '\\' );
	if( pchar )
		*pchar = '\0';
}


int cLF_LogRename( char* pFileName )
{// 로그 파일 이름에 시각 추가하기

	char szNewFileName[ _MAX_PATH ] = {0};
	char szAddTime[ 50 ] = {0};

	SYSTEMTIME stTime;
	GetLocalTime( &stTime );

	wsprintf( szAddTime , "_%04d%02d%02d_%02d%02d%02d" , // _%03d", 
				  stTime.wYear , stTime.wMonth, stTime.wDay, 
				  stTime.wHour, stTime.wMinute,
				  stTime.wSecond
				  //, stTime.wMilliseconds
				  );

	strcpy( szNewFileName , pFileName );	// 파일 이름 
	char* pchar = strrchr( szNewFileName , '.' );
	*pchar = '\0';
	strcat( szNewFileName ,  szAddTime );	// 현재 시각 추가 
	strcat( szNewFileName ,  ".log" );

	return rename( pFileName , szNewFileName );	// 파일 이름 바꾸기
}


void cLF_LogFile( char* pFile , char* pBuff )
{	// 로그 쓰기

	FILE *fp = NULL;

	fp = fopen( pFile , "ab" ); // 추가할 수 있는 바이너리 타입으로 파일을 연다.
	//fp = fopen( pFile , "ab+" );
	
	if( !fp )
		return ;

	int nlen = strlen( pBuff );

	int nRet;
	
	//nRet = fseek( fp , 0l , SEEK_END );

	nRet = 0;

	int nLimit = 0;

	do
	{
		nRet += fwrite( &pBuff[ nRet ] , 1 , nlen -nRet , fp );

		nLimit++;

		if( nLimit > 50 )
		{
			break;
		}

	}while( nRet < nlen );

	fclose( fp );

}



// 1000 , 2000 , 3000
// 100 , 90 , 80 , ...
//  0

unsigned int G_nMaxTraceTime = 0;
unsigned int G_nLastTraceTime = 0;

bool G_bCalcTrace = true;


HWND G_hwnd = NULL;

void InitTrace()
{
	G_hwnd = ::FindWindow( NULL , "LogTrace" );
}

void WriteTrace( char* pstrLog )
{
	if( !G_hwnd )
		return ;

	unsigned int nStTime = 0;
	unsigned int nEdTime = 0;

	if( G_bCalcTrace )
		nStTime = GetTickCount();

	COPYDATASTRUCT copydatastruct;

	copydatastruct.lpData = pstrLog;
	copydatastruct.cbData = strlen( pstrLog ) +1;
	copydatastruct.dwData = 0;

	int nRet = 0;

	if( G_hwnd )
		nRet = SendMessage( G_hwnd , WM_COPYDATA , (WPARAM)0, (LPARAM)&copydatastruct );
	
	if( 0 == nRet )
		G_hwnd = NULL;


	if( G_bCalcTrace )
	{
		nEdTime = GetTickCount();

		G_nLastTraceTime = nEdTime -nStTime;

		if( G_nLastTraceTime > G_nMaxTraceTime )
			G_nMaxTraceTime = G_nLastTraceTime;
	}
	
//	UINT nMsg = WM_USER +201;
//	SendMessage( hwnd , nMsg , 0 , 0 );
}


int cLF_LogErrorFileSize = 0;					// 로그 파일 사이즈 
int cLF_LogErrorFileMaxSize = 1024 *1024 *5;	// 로그 파일 최대 크기 

char cLF_szLogErrorFile[ _MAX_PATH ] = {0};		// 에러 로그 파일 
char cLF_szLogErrorBuff[ 1024 ] = {0};			// 에러 로그 버퍼 

void cLF_LogError( int nLevel , ... )
{

//cLogErrorLock sync;
cLogFlowLock sync;

	va_list marker;
	
	if( gpThis->m_nLogLevel < nLevel )	// 로그 레벨 체크 
		return ;
		
	va_start( marker, nLevel );     /* Initialize variable arguments. */
	


	SYSTEMTIME stTime;
	GetLocalTime( &stTime );

	wsprintf( cLF_szLogErrorBuff , "%04d/%02d/%02d %02d:%02d:%02d:%03d ", 
			      stTime.wYear , stTime.wMonth, stTime.wDay, 
				  stTime.wHour, stTime.wMinute,
				  stTime.wSecond, stTime.wMilliseconds);

	char* pStr = 0;
	pStr = va_arg( marker, char*);
	
	wvsprintf( &cLF_szLogErrorBuff[ 24 ] , pStr, (LPSTR)marker );	
	_tcscat( cLF_szLogErrorBuff , " \r\n" );

	WriteTrace( cLF_szLogErrorBuff );

	int nLen = _tcslen( cLF_szLogErrorBuff );	// 로그 문자열 크기 

	cLF_LogErrorFileSize += nLen;		// 로그 데이타 크기 추가 

	if( cLF_LogErrorFileSize > cLF_LogErrorFileMaxSize )	// 파일 사이즈가 최대치를 넘은 경우
	{
		if( !cLF_LogRename( cLF_szLogErrorFile ) )
			cLF_LogErrorFileSize = 0;
	}

	cLF_LogFile( cLF_szLogErrorFile , cLF_szLogErrorBuff );		// 로그 파일 쓰기 
	
	va_end( marker );              /* Reset variable arguments.      */ 	
	
}

int g_LogFlowFileSize = 0;					// 로그 파일 사이즈 
int g_LogFlowFileMaxSize = 1024 *1024 *5;	// 로그 파일 최대 크기 

char szLogFlowFile[ _MAX_PATH ] = {0};		// 에러 로그 파일 
char szLogFlowBuff[ 1024 ] = {0};			// 에러 로그 버퍼 

void cLF_LogFlow( int nLevel , ... )
{

cLogFlowLock sync;

	va_list marker;
	
	if( gpThis->m_nLogLevel < nLevel )	// 로그 레벨 체크 
		return ;
		
	va_start( marker, nLevel );     /* Initialize variable arguments. */
	


	SYSTEMTIME stTime;
	GetLocalTime( &stTime );

	wsprintf( szLogFlowBuff , "%04d/%02d/%02d %02d:%02d:%02d:%03d ", 
			      stTime.wYear , stTime.wMonth, stTime.wDay, 
				  stTime.wHour, stTime.wMinute,
				  stTime.wSecond, stTime.wMilliseconds);

	char* pStr = 0;
	pStr = va_arg( marker, char*);
	
	wvsprintf( &szLogFlowBuff[ 24 ] , pStr, (LPSTR)marker );	
	_tcscat( szLogFlowBuff , " \r\n" );

	WriteTrace( szLogFlowBuff );


	int nLen = _tcslen( szLogFlowBuff );	// 로그 문자열 크기 

	g_LogFlowFileSize += nLen;		// 로그 데이타 크기 추가 

	if( g_LogFlowFileSize > g_LogFlowFileMaxSize )	// 파일 사이즈가 최대치를 넘은 경우
	{
		if( !cLF_LogRename( szLogFlowFile ) )
			g_LogFlowFileSize = 0;
	}
	
	cLF_LogFile( szLogFlowFile , szLogFlowBuff );		// 로그 파일 쓰기 

	va_end( marker );              /* Reset variable arguments.      */ 	
	
}

int cLF_CheckLogFile( char* pFileName , int nFileMaxSize )
{// 파일 크기 

	FILE *fp = NULL;

	fp = fopen( pFileName , "rb" );
	
	if( !fp )
		return 0;

	int nRet = 0;
	nRet = fseek( fp , 0l , SEEK_END );

	int nPos = 0;
	nPos = ftell( fp );

	fclose( fp );

	if( nPos > nFileMaxSize )	// 파일 사이즈가 정해진 사이즈 넘은 경우
	{
		if(	!cLF_LogRename( pFileName ) )
			nPos = 0;
	}
	
	// 최대치를 넘지 않은 경우 라면 	
	return nPos; 
}

void cLF_InitLogFileSetting()
{	// 로그파일 내용을 설정한다.

	cLF_GetCurrentPath( cLF_szLogErrorFile );	// 현재 경로 정보를 받아온다.

	strcat( cLF_szLogErrorFile , "\\" );
	strcat( cLF_szLogErrorFile , "DbSvrError.log" );

	int nRet = cLF_CheckLogFile( cLF_szLogErrorFile , cLF_LogErrorFileMaxSize ); 
	cLF_LogErrorFileSize = nRet;


////////////////////////////

	cLF_GetCurrentPath( szLogFlowFile );	// 현재 경로 정보를 받아온다.

	strcat( szLogFlowFile , "\\" );
	strcat( szLogFlowFile , "DbSvrFlow.log" );

	nRet = cLF_CheckLogFile( szLogFlowFile , g_LogFlowFileMaxSize ); 
	g_LogFlowFileSize = nRet;
	
}


CString g_csServerState;

int g_nPort = 4000;
int g_nMaxConn = 1000;

//typedef CList<CClientSock* ,CClientSock*> CLT_LIST;
typedef CArray<CClientSock* ,CClientSock*> CLT_LIST;

CLT_LIST g_ClientList;		// 클라이언트 리스트

//typedef CList<CString ,CString*> LOG_LIST;
//LOG_LIST g_loglist;			// 로그 리스트 




void cSk_PostMsg( WPARAM wparam , LPARAM lparam )
{
	gpThis->PostMessage( UM_RECEIVE_MSG , wparam , lparam );
}

void cSk_SendResp( CClientSock* pClient , int nType , BYTE* pData , int nDataSize )
{
	if( !pClient->IsValid() )
		return ;

	MSG_PACK* p = setmsg( pData , nDataSize , 1 , nType );

	int nSendRet = pClient->Send( p , nDataSize +sizeof( MSG_HEAD ) );

	if( p )
		delete p;

}




void SetInitSockCount( int nCount )
{

	//g_ClientList.SetSize( 100 );
	CClientSock* pRetClient = NULL;

	for( int n = 0 ; n < nCount ; n++ )
	{
		pRetClient = new CClientSock;
		//g_ClientList.AddTail( pRetClient );
		g_ClientList.Add( pRetClient );
	}
}

CClientSock* cSk_GetNewClient()
{

cSockListLock lock;

	CClientSock* pRetClient = NULL;
	POSITION pos = NULL;

	//int nCount = g_ClientList.GetCount();
	int nCount = g_ClientList.GetSize();

	if( nCount >= g_nMaxConn )
	{	// 연결할 수 있는 클라이언트 최대 개수를 넘었다면 
		cLF_LogError( 0 , "[100] nCount(%d) >= g_nMaxConn(%d) " , nCount , g_nMaxConn );		

		return NULL;
	}

	int nPos = 0;
	if( 0 == nCount )
	{
		pRetClient = new CClientSock;
		
		g_ClientList.Add( pRetClient );

		return pRetClient;
	}
	else
	{
		DWORD dwCurTime = GetTickCount();

		int nUsing = 0;

		int n = 0;
		CClientSock** pList = g_ClientList.GetData();		

		for( n = 0 ; n < g_ClientList.GetSize() ; n++ )
		{			
			pRetClient = pList[n];

			if( pRetClient->IsValid() )
				nUsing++;			
		}

		
		for( n = 0 ; n < g_ClientList.GetSize() ; n++ )
		{			
			pRetClient = pList[n];

			if( !pRetClient->IsValid() )
			{
				if( pRetClient->IsUsable( dwCurTime ) )
				{
					pRetClient->m_nThread = nUsing;

					cLF_LogFlow( 100 , "[GetNewClient] 재사용 nPos(%d) GetNewClient(%d) g_nMaxConn(%d) " ,
						nPos , nCount , g_nMaxConn );
					
					return pRetClient;			
				}
			}

			nPos++;
		}

 		pRetClient = new CClientSock;
		
		g_ClientList.Add( pRetClient );

		pRetClient->m_nThread = nUsing;

		cLF_LogFlow( 100 , "[GetNewClient] 새 할당 nPos(%d) GetNewClient(%d) g_nMaxConn(%d) " ,
						nPos , nCount , g_nMaxConn );

		return pRetClient;
	}

	return NULL;
}


/*
void RemoveClient( CClientSock* pClient )
{
	CClientSock* pRetClient = NULL;
	POSITION pos = NULL , prepos = NULL;

	pos = g_ClientList.GetHeadPosition();

	while( pos )
	{
		prepos = pos;
		pRetClient = g_ClientList.GetNext( pos );

		if( pClient == pRetClient )	
		{
			g_ClientList.RemoveAt( prepos );
			break;
		}
	}
}
*/

const int MAX_THREAD = 5;
const int MAX_MSG_THREAD = 2;




CString csDbName;
CString csDbIp;
CString csDbPort;

CString csDbId;
CString csDbPw;



class cDbConF
{
public:
	static CMyADO* GetFreeDb( int nIdx = -1 );

	static void FreeDb( CMyADO* pDB );
	
	static int ConnectDB( CMyADO* pDB , LPCTSTR lpszName , LPCTSTR lpszIp , LPCTSTR lpszPort , LPCTSTR lpszId , LPCTSTR lpszPw , BOOL bClose = FALSE );
	static BOOL ReConnectDB( CMyADO* pDB , BOOL bClose = TRUE );
	static int OpenDB();
	static void CloseDB();	

};



#ifdef _DEBUG
	const int DB_CONN = 10;
#else
	//const int DB_CONN = MAX_THREAD *2;		// 1000;
	//const int DB_CONN = 50;
	const int DB_CONN = 5;
#endif

int G_nLastDbConn = 0;

CMyADO mAdoObj[ DB_CONN ];

void cDbConF::FreeDb( CMyADO* pADO )
{
	pADO->EndUsing();
}

CMyADO* cDbConF::GetFreeDb( int nIdx )
{

cGetDbLock dbSync;
/*
	if( nIdx >= 0 )
	{
		if( mAdoObj[ nIdx ].IsFree() ) 
		{	
			mAdoObj[ nIdx ].Lock();
			return (CMyADO*)&mAdoObj[ nIdx ];
		}
	}
	else
	*/
	{
		//for( int i = MAX_THREAD ; i < DB_CONN ; i++ )
		for( int i = 0 ; i < DB_CONN ; i++ )
		{
			if( mAdoObj[ i ].IsFree() )
			{			
				if( i > G_nLastDbConn )
				{
					G_nLastDbConn = i;
					
				}

				mAdoObj[ i ].Lock();
				return (CMyADO*)&mAdoObj[ i ];
			}
		}

		DWORD dwTime = GetTickCount();		// 현재 시각

		G_nLastDbConn = MAX_THREAD;

		cLF_LogError( 10000 , "[GetFreeDb] G_nLastDbConn(%d)" , G_nLastDbConn );


		//for( i = MAX_THREAD ; i < DB_CONN ; i++ )
		for( i = 0 ; i < DB_CONN ; i++ )
		{
			if( !mAdoObj[ i ].IsFree() )	// 사용 불가 
			{
				if( dwTime - mAdoObj[ i ].m_LockTime > 60000 )	// 60초 이상된 못 쓰는 거면 
				{
					if( i > G_nLastDbConn )
						G_nLastDbConn = i;

					mAdoObj[ i ].Lock();
					return (CMyADO*)&mAdoObj[ i ];
				}
			}
		}
	}

	return NULL;
}



int cDbConF::ConnectDB( CMyADO* pDB , LPCTSTR lpszName , LPCTSTR lpszIp , LPCTSTR lpszPort , LPCTSTR lpszId , LPCTSTR lpszPw , BOOL bClose )
{
	if( bClose )
		pDB->Close();
	
	CString csConn;
	csConn.Format( "Driver={SQL Server};Server=%s,%s;Net=dbmscon;Trusted_Connection=no;Database=%s;Auto Translate = false" ,
		lpszIp , lpszPort , lpszName );

	if( pDB->Open( (LPCTSTR)csConn ,
		lpszId , lpszPw ) != S_OK )	// sql logon id , pw
	{	
		if( pDB->IsErrorStr() )			
			::MessageBox( NULL , pDB->GetErrorStr() , "open" "error" , MB_OK );
		return 0;
	}
	else
		return 1;	
}


BOOL cDbConF::ReConnectDB( CMyADO* pDB ,BOOL bClose )
{
	Sleep( 10000 );

	return cDbConF::ConnectDB( pDB , csDbName , csDbIp , csDbPort , csDbId , csDbPw , bClose );	
}


int cDbConF::OpenDB()
{
	for( int i = 0 ; i < DB_CONN ; i++ )
	{
		CMyADO* pADO = &mAdoObj[ i ];
		int nRet = cDbConF::ConnectDB( pADO , csDbName , csDbIp , csDbPort , csDbId , csDbPw );

		if( 0 == nRet )
			return i;	// 0 ~ DB_CONN -1
	}

	return -1;
}

void cDbConF::CloseDB()
{
	for( int i = 0 ; i < DB_CONN ; i++ )
	{
		mAdoObj[ i ].UnLock();
		mAdoObj[ i ].Close();
	}
}



class cWF
{
public:
	static void fLogin( int nThrIdx , MSG_PACK* pPack , CClientSock* pClient );	

	static void InitPath();
	static CString m_csChannelPath;

};

CString cWF::m_csChannelPath;


/////////////////////////////////////////////////////////////////////////////
// CDbSvrDlg message handlers

BOOL CDbSvrDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here

	ReadSet();

	InitTrace();

	int nPreSock = 60;
	//int nPreSock = 60 *80;

	SetInitSockCount( nPreSock );

	if( g_nMaxConn < nPreSock )
		g_nMaxConn = nPreSock;

	cLF_InitLogFileSetting();

	SetDlgItemText( IDC_E_DB_NAME , "wang" );

#ifdef _DEBUG
	SetDlgItemText( IDC_E_DB_IP , "218.145.31.21" );
#else
	SetDlgItemText( IDC_E_DB_IP , "115.179.103.182" );
#endif
	
	SetDlgItemText( IDC_E_DB_PORT , "1433" );

	SetDlgItemText( IDC_E_PORT , "11017" );
	SetDlgItemText( IDC_E_MAX_CONN , "1000" );

	SetDlgItemText( IDC_E_DB_ID , "wang" );
	SetDlgItemText( IDC_E_DB_PW , "wang1234" );

	m_nMaxLogLine = 100;
	SetDlgItemInt( IDC_E_LOG_COUNT , m_nMaxLogLine );

	SetDlgItemInt( IDC_E_PAGE , 1 );
	

	m_nShowLog = true;

	cWF::InitPath();

	///////////////////

	
		

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDbSvrDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
//		CAboutDlg dlgAbout;
//		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDbSvrDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		if( G_bCalcTrace )
		{
			CClientDC dc(this);
			
			CString csText;

			int nX = 10;
			int nY = 180;

			csText.Format( "로그기록 최대 소요 시간 : %d " , G_nMaxTraceTime );
			dc.TextOut( nX , nY +80 , csText );

			csText.Format( "마지막 소요 시간 : %d " , G_nLastTraceTime );
			dc.TextOut( nX , nY +100 , csText );
		}

		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDbSvrDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CDbSvrDlg::OnBStart() 
{
	// TODO: Add your control notification handler code here
	StartServer();
}

int CDbSvrDlg::StartServer()
{
	if( GetDlgItem( IDC_E_PORT )->IsWindowEnabled() )
	{
		int nNum = 0;
		nNum = GetDlgItemInt( IDC_EDT_CONN_CHECK_PRD );
		
		SetTimer( 1 , nNum *1000 , NULL );	

		G_nSvrType = ((CComboBox*)GetDlgItem( IDC_CMB_WORK_TYPE ))->GetCurSel();
		

		int nRet = 0;

		g_nPort = GetDlgItemInt( IDC_E_PORT );
		g_nMaxConn = GetDlgItemInt( IDC_E_MAX_CONN );

		GetDlgItem( IDC_E_PORT )->EnableWindow( FALSE );
		GetDlgItem( IDC_E_MAX_CONN )->EnableWindow( FALSE );

		StartThread();

		if( m_Server.Create( g_nPort ) )	
		{
			if( !m_Server.Listen() )
			{				
				g_csServerState.Format( "Listen 실패 (%d)" , GetLastError() );				
			}
			else
			{	
				nRet = ConnDB();

				if( 0 == nRet )
				{
					GetDlgItem( IDC_E_PORT )->EnableWindow( TRUE );
					cDbConF::CloseDB();
					m_Server.Close();

					g_csServerState.Format( "서버 실행 실패"	);
					return 0;
				}

				g_csServerState.Format( "서버 실행"	);

				SetDlgItemText( IDC_B_START , "서버 중지" );
			}
		}
		else
		{			
			g_csServerState.Format( "Create 실패 (%d)" , GetLastError() );		
		}
	}
	else
	{		

		CloseThread();

		KillTimer( 1 );

		CloseAllClient();

		m_Server.Close();

	

		GetDlgItem( IDC_E_PORT )->EnableWindow( TRUE );
		GetDlgItem( IDC_E_MAX_CONN )->EnableWindow( TRUE );

		CDialog::OnOK();
	}


	return 1;
}

void CDbSvrDlg::ShowStateInfo()
{

	


try
{


	{
cGetDbLock dbSync;

	SetDlgItemInt( IDC_E_LAST_DB , G_nLastDbConn );
	}


cSockListLock lock;


		cLF_LogFlow( 10000 , "[ShowStateInfo] start time" );

		DWORD dwCurTime = GetTickCount();
		// 주기적으로 화면 내용 갱신 
		//int nCount = g_ClientList.GetCount();
		CWnd* pWnd = NULL;
		int nCount = 0;
		CString csText;
		
		//SetDlgItemInt( IDC_E_LAST_DB , G_nLastDbConn );

		///////////////
		pWnd = GetDlgItem( IDC_S_USER_LIST );
		CClientDC dcList( pWnd );


		//POSITION pos = g_ClientList.GetHeadPosition();
		CClientSock** pList = g_ClientList.GetData();

		nCount = 0;

		int nMaxClientBuff = 0;
		int nPage = GetDlgItemInt( IDC_E_PAGE );

		int nPingSend = 0;

		//while( pos )
		for( int n = 0 ; n < g_ClientList.GetSize() ; n++ )
		{
			//CClientSock* pRetClient = g_ClientList.GetNext( pos );
			CClientSock* pRetClient = pList[n];

			if( pRetClient && pRetClient->IsValid() )
			{			
				CString csIP;
				UINT nPort;
				pRetClient->GetPeerName( csIP , nPort );

				BOOL bConn = FALSE;
				if( pRetClient->IsValid() )
					bConn = TRUE;

				if( pRetClient->m_nRecvMaxLog > nMaxClientBuff )	// 더 큰 버퍼라면 
					nMaxClientBuff = pRetClient->m_nRecvMaxLog;		// 최대 버퍼 설정

				csText.Format( "%d[%03d] %s , %s , %d , M(%d) L(%d)" ,
					bConn , nCount +1 , pRetClient->m_szID , csIP , nPort ,
					pRetClient->m_nRecvMaxLog , pRetClient->m_nRecvLastLog );

				if( ( nPage -1 ) *20 <= nCount && nCount <= nPage *25 )
				{
					dcList.TextOut( 0 , 0 +( nCount -( nPage -1 ) *20 ) *18 , csText , csText.GetLength() );
				}

				nCount++;					

//#ifndef _DEBUG
				if( bConn )		// 연결된 것 중에서 
				{
					//if( !pRetClient->IsUsable( dwCurTime ) )	// 사용중이라 재사용이 가능한게 아닌 것 
					{
						if( dwCurTime -pRetClient->m_nLastTime > 70000 )	// 마지막 수신 내용이 70 초 넘으면 
						{
							PING ping;	
	
							strcpy( ping.id , "ping" );
							strcpy( ping.pw , "Ping" );
							strcpy( ping.key , "PING" );

							cSk_SendResp( pRetClient , 115 , (unsigned char*)&ping , sizeof( ping ) );		// 접속 연결 상태를 체크한다. // 송신 실패는 알아서 끊는다.
							nPingSend++;

							pRetClient->m_nLastTime = GetTickCount();
						}
					}
				}
//#endif
			}
		}

		SetDlgItemInt( IDC_S_MAX_BUFF , nMaxClientBuff );

		m_nMaxLogLine = GetDlgItemInt( IDC_E_LOG_COUNT );
		m_nLogLevel = GetDlgItemInt( IDC_E_LEVEL );


		pWnd = GetDlgItem( IDC_S_USER );
		CClientDC dc( pWnd );

		
		
		csText.Format( "%d" , nCount );
		dc.TextOut( 0 , 0 , csText , csText.GetLength() );



		cLF_LogFlow( 500 , "[ShowStateInfo] nCount(%d) nMaxClientBuff(%d) nPingSend(%d)" ,
						nCount , nMaxClientBuff , nPingSend );	
		
		cLF_LogFlow( 10000 , "[ShowStateInfo] end time" );

}
catch(...)
{

}

}


void CDbSvrDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default

	if( 1 == nIDEvent )		// 못 처리한 수신 내용이 있는지 체크
	{ 
		gpThis->PostMessage( UM_SHOW_AVAIL , 0 , 0 );
	}
	
	CDialog::OnTimer(nIDEvent);
}

class cDbF
{
public:

	static int LoginDB( CClientSock* pClient , UINT nThreadIdx , char* pstrId , char* pstrPw , char* pstrIp );	
	static int ReqRecentUser( CClientSock* pClient , REQ_CMD* pCmd );
	static int ReqNotice( CClientSock* pClient , REQ_CMD* pCmd );
	static int ReqNoticeContent( CClientSock* pClient , REQ_CMD* pCmd );
	static int ChargeDlg( CClientSock* pClient , REQ_CMD* pCmd );
	static int ChargeReq( CClientSock* pClient , REQ_CMD* pCmd );

	static int ExChangeDlg( CClientSock* pClient , REQ_CMD* pCmd );
	static int ExChangeReq( CClientSock* pClient , REQ_CMD* pCmd );
	static int QaListDlg( CClientSock* pClient , REQ_CMD* pCmd );
	static int ReqQaContent( CClientSock* pClient , REQ_CMD* pCmd );

	static int ReqSendQa( CClientSock* pClient , REQ_CMD* pCmd );
	static int ReqJoin( CClientSock* pClient , REQ_CMD2* pCmd );
	static int ReqRefresh( CClientSock* pClient , REQ_CMD* pCmd );
	static int LogoutDB( CClientSock* pClient , char* pUserId );
	static int ReqBoxDlg( CClientSock* pClient , REQ_CMD* pCmd );
	static int ReqBoxUse( CClientSock* pClient , REQ_CMD* pCmd );
	static int ReqBoxPwd( CClientSock* pClient , REQ_CMD* pCmd );
	static int ReqCoupon( CClientSock* pClient , REQ_CMD* pCmd );
	

};


void MakeRecentKey( char* pstr , int nCKey )
{
	unsigned int nKey = GetTickCount();
	char szNum[ 30 ] = {0};
	char szNum2[ 30 ] = {0};

	itoa( nKey , szNum , 10 );

	int nLen = strlen( szNum );

	for( int n = 0 ; n < nLen ; n++ )
		szNum[ n ] = szNum[n] -'0' +'A';
	
	szNum[ nLen ] = 0;

	///////////
	itoa( nCKey , szNum2 , 10 );

	nLen = strlen( szNum2 );

	for( n = 0 ; n < nLen ; n++ )
		szNum2[ n ] = szNum2[n] -'0' +'A';
	
	szNum2[ nLen ] = 0;

	strcpy( pstr , szNum );
	strcat( pstr , szNum2 );

}


int cDbF::ReqRecentUser( CClientSock* pClient , REQ_CMD* pCmd )
{
	RESP_RECENT_USER resp;

	CMyADO* pADO = cDbConF::GetFreeDb( 0 );

	if( !pADO )
	{
		cLF_LogError( 100 ,  "[100] !pADO ReqRecentUser" );

		return 30000;
	}

LOOP_ST:
	long lReturnValue = 0;
	HRESULT hRet = NULL;

	PADO_INIT( pADO , "USP_RECENT_USER" , goto EXIT_CLOSE_END )

	PADO_ADD_RETURN( pADO , goto EXIT_CLOSE_END )
	
	PADO_EXEC( pADO , goto EXIT_CLOSE_END )	
	
	PADO_GET_RET( pADO , &lReturnValue , goto EXIT_CLOSE_END )

	if( 1 == lReturnValue )
	{
		int nIndex = 0;
		int nSendCount = 0;

		while( !pADO->IsEOF() )
		{
			char szName[ 30 ] = {0};
			char szValue[ 100 ] = {0};

			resp.user[ nIndex ].nIdx = nIndex;

			
			HRESULT hReturn = pADO->GetFieldTextByIndex( 0 , szName , 50  , szValue , 100 );			
			strcpy( resp.user[ nIndex ].szUserID , szValue );

			hReturn = pADO->GetFieldTextByIndex( 1 , szName , 50  , szValue , 100 );			
			resp.user[ nIndex ].point = _atoi64( szValue );
			nIndex++;

			if( 10 == nIndex )	// 10 개 단위로 보낸다.
			{				
				resp.nCount = nIndex;
				resp.nAdd = nSendCount;

				cSk_SendResp( pClient , 102 , (unsigned char*)&resp , sizeof( resp ) );
				memset( &resp , 0x00 , sizeof( resp ) );
				nIndex = 0;
				nSendCount++;
			}

			pADO->MoveNext();
		}

		if( 0 < nIndex && nIndex < 10 )
		{
			resp.nCount = nIndex;
			resp.nAdd = nSendCount;

			cSk_SendResp( pClient , 102 , (unsigned char*)&resp , sizeof( resp ) );
		}
	}
	

	
	cDbConF::FreeDb( pADO );
	return lReturnValue;

EXIT_CLOSE_END:
	
	if( pADO->IsErrorStr() )
	{	
		 cDbConF::ReConnectDB( pADO );
		 goto LOOP_ST;
	}
	
	cDbConF::FreeDb( pADO );
	return -1;
}

int cDbF::ReqNotice( CClientSock* pClient , REQ_CMD* pCmd )
{
	RESP_NOTICE resp;

	CMyADO* pADO = cDbConF::GetFreeDb( 0 );

	if( !pADO )
	{
		cLF_LogError( 100 ,  "[100] !pADO ReqNotice" );

		return 30000;
	}

LOOP_ST:
	long lReturnValue = 0;
	HRESULT hRet = NULL;

	PADO_INIT( pADO , "USP_NOTICE" , goto EXIT_CLOSE_END )

	PADO_ADD_RETURN( pADO , goto EXIT_CLOSE_END )
	
	PADO_EXEC( pADO , goto EXIT_CLOSE_END )	
	
	PADO_GET_RET( pADO , &lReturnValue , goto EXIT_CLOSE_END )

	if( 1 == lReturnValue )
	{
		int nIndex = 0;
		int nSendCount = 0;

		while( !pADO->IsEOF() )
		{
			char szName[ 30 ] = {0};
			char szValue[ 100 ] = {0};			

			
			HRESULT hReturn = pADO->GetFieldTextByIndex( 0 , szName , 50  , szValue , 100 );			

			g_strtrim( szValue );
			strcpy( resp.notice[ nIndex ].szTitle , szValue );

			hReturn = pADO->GetFieldTextByIndex( 1 , szName , 50  , szValue , 100 );
			memset( &resp.notice[ nIndex ].szDate , 0x0 , sizeof( resp.notice[ nIndex ].szDate ) );
			strncpy( resp.notice[ nIndex ].szDate , &szValue[5] , 5 );

			hReturn = pADO->GetFieldTextByIndex( 2 , szName , 50  , szValue , 100 );

			resp.notice[ nIndex ].nIdx = atoi( szValue );

			nIndex++;

			if( 10 == nIndex )	// 10 개 단위로 보낸다.
			{				
				resp.nCount = nIndex;
				resp.nAdd = nSendCount;

				cSk_SendResp( pClient , 103 , (unsigned char*)&resp , sizeof( resp ) );
				memset( &resp , 0x00 , sizeof( resp ) );
				nIndex = 0;
				nSendCount++;
			}

			pADO->MoveNext();
		}

		if( 0 < nIndex && nIndex < 10 )
		{
			resp.nCount = nIndex;
			resp.nAdd = nSendCount;

			cSk_SendResp( pClient , 103 , (unsigned char*)&resp , sizeof( resp ) );
		}
	}
	
	
	cDbConF::FreeDb( pADO );
	return lReturnValue;

EXIT_CLOSE_END:
	
	if( pADO->IsErrorStr() )
	{	
		 cDbConF::ReConnectDB( pADO );
		 goto LOOP_ST;
	}
	
	cDbConF::FreeDb( pADO );
	return -1;
}



int cDbF::ReqRefresh( CClientSock* pClient , REQ_CMD* pCmd )
{
	RESP_CMD resp;

	CMyADO* pADO = cDbConF::GetFreeDb( 0 );

	if( !pADO )
	{
		cLF_LogError( 100 ,  "[100] !pADO ReqRefresh" );

		return 30000;
	}

LOOP_ST:
	long lReturnValue = 0;
	HRESULT hRet = NULL;

	PADO_INIT( pADO , "USP_REFRESH2" , goto EXIT_CLOSE_END )

	PADO_ADD_RETURN( pADO , goto EXIT_CLOSE_END )

	PADO_ADD_INPUT_TEXT( pADO , "id" , pCmd->id , goto EXIT_CLOSE_END )
	PADO_ADD_INPUT_TEXT( pADO , "webkey" , (LPSTR)(LPCTSTR)pClient->m_strWebkey , goto EXIT_CLOSE_END )
	
	PADO_EXEC( pADO , goto EXIT_CLOSE_END )	
	
	PADO_GET_RET( pADO , &lReturnValue , goto EXIT_CLOSE_END )

	if( 1 == lReturnValue )
	{
		int nIndex = 0;
		int nSendCount = 0;

		while( !pADO->IsEOF() )
		{
			char szName[ 30 ] = {0};
			char szValue[ 100 ] = {0};			

			// 보유 포인트 
			HRESULT hReturn = pADO->GetFieldTextByIndex( 0 , szName , 50  , szValue , 100 );			
			resp.nPoint = atoi( szValue );

			hReturn = pADO->GetFieldTextByIndex( 1 , szName , 50  , szValue , 100 );			
			resp.nCount = atoi( szValue );

			pADO->MoveNext();
		}		
	}

	resp.nRet = lReturnValue;
	cSk_SendResp( pClient , 113 , (unsigned char*)&resp , sizeof( resp ) );	
	
	cDbConF::FreeDb( pADO );
	return lReturnValue;

EXIT_CLOSE_END:
	
	if( pADO->IsErrorStr() )
	{	
		 cDbConF::ReConnectDB( pADO );
		 goto LOOP_ST;
	}
	
	cDbConF::FreeDb( pADO );
	return -1;
}




int cDbF::ReqBoxDlg( CClientSock* pClient , REQ_CMD* pCmd )
{
	RESP_CMD resp;

	CMyADO* pADO = cDbConF::GetFreeDb( 0 );

	if( !pADO )
	{
		cLF_LogError( 100 ,  "[100] !pADO ReqBoxDlg" );

		return 30000;
	}

LOOP_ST:
	long lReturnValue = 0;
	HRESULT hRet = NULL;

	PADO_INIT( pADO , "USP_BoxDlg2" , goto EXIT_CLOSE_END )

	PADO_ADD_RETURN( pADO , goto EXIT_CLOSE_END )

	PADO_ADD_INPUT_TEXT( pADO , "id" , pCmd->id , goto EXIT_CLOSE_END )
	PADO_ADD_INPUT_TEXT( pADO , "webkey" , (LPSTR)(LPCTSTR)pClient->m_strWebkey , goto EXIT_CLOSE_END )
	
	PADO_EXEC( pADO , goto EXIT_CLOSE_END )	
	
	PADO_GET_RET( pADO , &lReturnValue , goto EXIT_CLOSE_END )

	if( 1 == lReturnValue )
	{
		int nIndex = 0;
		int nSendCount = 0;

		while( !pADO->IsEOF() )
		{
			char szName[ 30 ] = {0};
			char szValue[ 100 ] = {0};			

			// 보유 포인트 
			HRESULT hReturn = pADO->GetFieldTextByIndex( 0 , szName , 50  , szValue , 100 );
			resp.nPoint = atoi( szValue );
			
			// 금고 포인트
			hReturn = pADO->GetFieldTextByIndex( 1 , szName , 50  , szValue , 100 );
			resp.nBoxPoint = atoi( szValue );
			
			// 딜비 포인트 
			hReturn = pADO->GetFieldTextByIndex( 2 , szName , 50  , szValue , 100 );
			resp.nPoint2 = atoi( szValue );

			pADO->MoveNext();
		}		
	}

	resp.nRet = lReturnValue;
	cSk_SendResp( pClient , 114 , (unsigned char*)&resp , sizeof( resp ) );	
	
	cDbConF::FreeDb( pADO );
	return lReturnValue;

EXIT_CLOSE_END:
	
	if( pADO->IsErrorStr() )
	{	
		 cDbConF::ReConnectDB( pADO );
		 goto LOOP_ST;
	}
	
	cDbConF::FreeDb( pADO );
	return -1;
}



int cDbF::ReqBoxUse( CClientSock* pClient , REQ_CMD* pCmd )
{
	RESP_CMD resp;

	CMyADO* pADO = cDbConF::GetFreeDb( 0 );

	if( !pADO )
	{
		cLF_LogError( 100 ,  "[100] !pADO ReqBoxDlg" );

		return 30000;
	}

LOOP_ST:
	long lReturnValue = 0;
	HRESULT hRet = NULL;

	PADO_INIT( pADO , "USP_BoxUse2" , goto EXIT_CLOSE_END )

	PADO_ADD_RETURN( pADO , goto EXIT_CLOSE_END )

	PADO_ADD_INPUT_TEXT( pADO , "id" , pCmd->id , goto EXIT_CLOSE_END )
	PADO_ADD_INPUT_LONG( pADO , "point" , pCmd->nPoint , goto EXIT_CLOSE_END )
	PADO_ADD_INPUT_LONG( pADO , "type" , pCmd->nDbIdx , goto EXIT_CLOSE_END )
	PADO_ADD_INPUT_TEXT( pADO , "webkey" , (LPSTR)(LPCTSTR)pClient->m_strWebkey , goto EXIT_CLOSE_END )
	
	PADO_EXEC( pADO , goto EXIT_CLOSE_END )	
	
	PADO_GET_RET( pADO , &lReturnValue , goto EXIT_CLOSE_END )

	if( 1 == lReturnValue )
	{
		int nIndex = 0;
		int nSendCount = 0;

		while( !pADO->IsEOF() )
		{
			char szName[ 30 ] = {0};
			char szValue[ 100 ] = {0};			

			// 보유 포인트 
			HRESULT hReturn = pADO->GetFieldTextByIndex( 0 , szName , 50  , szValue , 100 );
			resp.nPoint = atoi( szValue );
			
			// 금고 포인트
			hReturn = pADO->GetFieldTextByIndex( 1 , szName , 50  , szValue , 100 );
			resp.nBoxPoint = atoi( szValue );
			
			// 딜비 포인트 
			hReturn = pADO->GetFieldTextByIndex( 2 , szName , 50  , szValue , 100 );
			resp.nPoint2 = atoi( szValue );

			pADO->MoveNext();
		}		
	}

	resp.nRet = lReturnValue;
	cSk_SendResp( pClient , 116 , (unsigned char*)&resp , sizeof( resp ) );	
	
	cDbConF::FreeDb( pADO );
	return lReturnValue;

EXIT_CLOSE_END:
	
	if( pADO->IsErrorStr() )
	{	
		 cDbConF::ReConnectDB( pADO );
		 goto LOOP_ST;
	}
	
	cDbConF::FreeDb( pADO );
	return -1;
}



int cDbF::ReqBoxPwd( CClientSock* pClient , REQ_CMD* pCmd )
{
	RESP_CMD resp;

	CMyADO* pADO = cDbConF::GetFreeDb( 0 );

	if( !pADO )
	{
		cLF_LogError( 100 ,  "[100] !pADO ReqBoxDlg" );

		return 30000;
	}

LOOP_ST:
	long lReturnValue = 0;
	HRESULT hRet = NULL;

	PADO_INIT( pADO , "USP_CheckBoxPwd2" , goto EXIT_CLOSE_END )

	PADO_ADD_RETURN( pADO , goto EXIT_CLOSE_END )

	PADO_ADD_INPUT_TEXT( pADO , "id" , pCmd->id , goto EXIT_CLOSE_END )
	PADO_ADD_INPUT_TEXT( pADO , "boxpwd" , pCmd->expw , goto EXIT_CLOSE_END )
	PADO_ADD_INPUT_TEXT( pADO , "webkey" , (LPSTR)(LPCTSTR)pClient->m_strWebkey , goto EXIT_CLOSE_END )

	
	PADO_EXEC( pADO , goto EXIT_CLOSE_END )	
	
	PADO_GET_RET( pADO , &lReturnValue , goto EXIT_CLOSE_END )

	if( 1 == lReturnValue )
	{
		
	}

	resp.nRet = lReturnValue;
	cSk_SendResp( pClient , 117 , (unsigned char*)&resp , sizeof( resp ) );	
	
	cDbConF::FreeDb( pADO );
	return lReturnValue;

EXIT_CLOSE_END:
	
	if( pADO->IsErrorStr() )
	{	
		 cDbConF::ReConnectDB( pADO );
		 goto LOOP_ST;
	}
	
	cDbConF::FreeDb( pADO );
	return -1;
}



int cDbF::ReqCoupon( CClientSock* pClient , REQ_CMD* pCmd )
{
	RESP_CMD resp;

	CMyADO* pADO = cDbConF::GetFreeDb( 0 );

	if( !pADO )
	{
		cLF_LogError( 100 ,  "[100] !pADO ReqBoxDlg" );

		return 30000;
	}

LOOP_ST:
	long lReturnValue = 0;
	HRESULT hRet = NULL;

	PADO_INIT( pADO , "USP_USE_COUPON" , goto EXIT_CLOSE_END )

	PADO_ADD_RETURN( pADO , goto EXIT_CLOSE_END )

	PADO_ADD_INPUT_TEXT( pADO , "id" , pCmd->id , goto EXIT_CLOSE_END )
	PADO_ADD_INPUT_TEXT( pADO , "coupon" , pCmd->username , goto EXIT_CLOSE_END )
	PADO_ADD_INPUT_TEXT( pADO , "webkey" , (LPSTR)(LPCTSTR)pClient->m_strWebkey , goto EXIT_CLOSE_END )
	PADO_ADD_INPUT_TEXT( pADO , "ip" , (LPSTR)(LPCTSTR)pClient->m_szIP , goto EXIT_CLOSE_END )

	
	PADO_EXEC( pADO , goto EXIT_CLOSE_END )	
	
	PADO_GET_RET( pADO , &lReturnValue , goto EXIT_CLOSE_END )

	if( 1 == lReturnValue )
	{
		
	}

	resp.nRet = lReturnValue;
	cSk_SendResp( pClient , 118 , (unsigned char*)&resp , sizeof( resp ) );	
	
	cDbConF::FreeDb( pADO );
	return lReturnValue;

EXIT_CLOSE_END:
	
	if( pADO->IsErrorStr() )
	{	
		 cDbConF::ReConnectDB( pADO );
		 goto LOOP_ST;
	}
	
	cDbConF::FreeDb( pADO );
	return -1;
}



int cDbF::LogoutDB( CClientSock* pClient , char* pUserId )
{
	RESP_CMD resp;

	CMyADO* pADO = cDbConF::GetFreeDb( 0 );

	if( !pADO )
	{
		cLF_LogError( 100 ,  "[100] !pADO LogoutDB" );

		return 30000;
	}

LOOP_ST:
	long lReturnValue = 0;
	HRESULT hRet = NULL;

	PADO_INIT( pADO , "USP_LOGOUT" , goto EXIT_CLOSE_END )

	PADO_ADD_RETURN( pADO , goto EXIT_CLOSE_END )

	PADO_ADD_INPUT_TEXT( pADO , "id" , pUserId , goto EXIT_CLOSE_END )
	
	PADO_EXEC( pADO , goto EXIT_CLOSE_END )	
	
	PADO_GET_RET( pADO , &lReturnValue , goto EXIT_CLOSE_END )


	resp.nRet = lReturnValue;
	
	//cSk_SendResp( pClient , 114 , (unsigned char*)&resp , sizeof( resp ) );	
	
	cDbConF::FreeDb( pADO );
	return lReturnValue;

EXIT_CLOSE_END:
	
	if( pADO->IsErrorStr() )
	{	
		 cDbConF::ReConnectDB( pADO );
		 goto LOOP_ST;
	}
	
	cDbConF::FreeDb( pADO );
	return -1;
}




int cDbF::ReqNoticeContent( CClientSock* pClient , REQ_CMD* pCmd )
{
	NOTICE_CONTENT resp;

	CMyADO* pADO = cDbConF::GetFreeDb( 0 );

	if( !pADO )
	{
		cLF_LogError( 100 ,  "[100] !pADO ReqNoticeContent" );

		return 30000;
	}

LOOP_ST:
	long lReturnValue = 0;
	HRESULT hRet = NULL;

	PADO_INIT( pADO , "USP_NOTICE_CONTENT" , goto EXIT_CLOSE_END )

	PADO_ADD_RETURN( pADO , goto EXIT_CLOSE_END )

	PADO_ADD_INPUT_LONG( pADO , "dbidx" , pCmd->nDbIdx , goto EXIT_CLOSE_END )	
	
	PADO_EXEC( pADO , goto EXIT_CLOSE_END )	
	
	PADO_GET_RET( pADO , &lReturnValue , goto EXIT_CLOSE_END )

	if( 1 == lReturnValue )
	{
		int nIndex = 0;
		int nSendCount = 0;

		if( !pADO->IsEOF() )
		{
			char szName[ 30 ] = {0};
			char szValue[ 1000 ] = {0};

			resp.nRet = lReturnValue;

			// 보유 포인트 
			HRESULT hReturn = pADO->GetFieldTextByIndex( 0 , szName , 50  , szValue , 100 );
			g_strtrim( szValue );
			strcpy( resp.szTitle , szValue );

			hReturn = pADO->GetFieldTextByIndex( 1 , szName , 50  , szValue , 100 );
			memset( &resp.szDate , 0x0 , sizeof( resp.szDate ) );
			strncpy( resp.szDate , &szValue[5] , 5 );

			hReturn = pADO->GetFieldTextByIndex( 2 , szName , 50  , szValue , 1000 );			
			strcpy( resp.szContent , szValue );

			resp.szContent[ 1000 -1 ] = 0;

			cSk_SendResp( pClient , 104 , (unsigned char*)&resp , sizeof( resp ) );
		}		
	}
	else
	{
		resp.nRet = lReturnValue;
		cSk_SendResp( pClient , 104 , (unsigned char*)&resp , sizeof( resp ) );
	}
	
	
	cDbConF::FreeDb( pADO );
	return lReturnValue;

EXIT_CLOSE_END:
	
	if( pADO->IsErrorStr() )
	{	
		 cDbConF::ReConnectDB( pADO );
		 goto LOOP_ST;
	}
	
	cDbConF::FreeDb( pADO );
	return -1;

}

/////////////////////////


int cDbF::ChargeDlg( CClientSock* pClient , REQ_CMD* pCmd )
{
	CHARGE_DLG_RESP resp;

	CMyADO* pADO = cDbConF::GetFreeDb( 0 );

	if( !pADO )
	{
		cLF_LogError( 100 ,  "[100] !pADO ChargeDlg" );

		return 30000;
	}

LOOP_ST:
	long lReturnValue = 0;
	HRESULT hRet = NULL;

	PADO_INIT( pADO , "USP_CHARGE_DLG2" , goto EXIT_CLOSE_END )

	PADO_ADD_RETURN( pADO , goto EXIT_CLOSE_END )

	PADO_ADD_INPUT_TEXT( pADO , "userid" , pCmd->id , goto EXIT_CLOSE_END )
	PADO_ADD_INPUT_TEXT( pADO , "webkey" , (LPSTR)(LPCTSTR)pClient->m_strWebkey , goto EXIT_CLOSE_END )
	
	PADO_EXEC( pADO , goto EXIT_CLOSE_END )	
	
	PADO_GET_RET( pADO , &lReturnValue , goto EXIT_CLOSE_END )

	if( 1 == lReturnValue )
	{
		int nIndex = 0;

		if( !pADO->IsEOF() )
		{
			char szName[ 30 ] = {0};
			char szValue[ 100 ] = {0};

			
			HRESULT hReturn = pADO->GetFieldTextByIndex( 0 , szName , 50  , szValue , 100 );
			strcpy( resp.szBank , szValue );

			hReturn = pADO->GetFieldTextByIndex( 1 , szName , 50  , szValue , 100 );
			strcpy( resp.szBankUser , szValue );

			hReturn = pADO->GetFieldTextByIndex( 2 , szName , 50  , szValue , 100 );
			strcpy( resp.szBankNum , szValue );

			hReturn = pADO->GetFieldTextByIndex( 3 , szName , 50  , szValue , 100 );
			resp.nUserPoint = _atoi64( szValue );
		}		
	}

	resp.nRet = lReturnValue;
	cSk_SendResp( pClient , 105 , (unsigned char*)&resp , sizeof( resp ) );

	
	cDbConF::FreeDb( pADO );
	return lReturnValue;

EXIT_CLOSE_END:
	
	if( pADO->IsErrorStr() )
	{	
		 cDbConF::ReConnectDB( pADO );
		 goto LOOP_ST;
	}
	
	cDbConF::FreeDb( pADO );
	return -1;
}

////////////////////////////
int cDbF::ChargeReq( CClientSock* pClient , REQ_CMD* pCmd )
{
	RESP_CMD resp;

	char szNum[ 50 ] = {0};
	sprintf( szNum , "%I64d" , pCmd->nPoint );

	CMyADO* pADO = cDbConF::GetFreeDb( 0 );

	if( !pADO )
	{
		cLF_LogError( 100 ,  "[100] !pADO ChargeReq" );

		return 30000;
	}

LOOP_ST:
	long lReturnValue = 0;
	HRESULT hRet = NULL;

	PADO_INIT( pADO , "USP_CHARGE_REQ2" , goto EXIT_CLOSE_END )

	PADO_ADD_RETURN( pADO , goto EXIT_CLOSE_END )

	PADO_ADD_INPUT_TEXT( pADO , "userid" , pCmd->id , goto EXIT_CLOSE_END )
	PADO_ADD_INPUT_TEXT( pADO , "username" , pCmd->username , goto EXIT_CLOSE_END )
	PADO_ADD_INPUT_TEXT( pADO , "point" , szNum , goto EXIT_CLOSE_END )
	PADO_ADD_INPUT_TEXT( pADO , "expw" , pCmd->expw , goto EXIT_CLOSE_END )
	PADO_ADD_INPUT_TEXT( pADO , "tel" , pCmd->tel , goto EXIT_CLOSE_END )
	PADO_ADD_INPUT_TEXT( pADO , "webkey" , (LPSTR)(LPCTSTR)pClient->m_strWebkey , goto EXIT_CLOSE_END )
	
	PADO_EXEC( pADO , goto EXIT_CLOSE_END )	
	
	PADO_GET_RET( pADO , &lReturnValue , goto EXIT_CLOSE_END )

	resp.nRet = lReturnValue;
	cSk_SendResp( pClient , 106 , (unsigned char*)&resp , sizeof( resp ) );

	
	cDbConF::FreeDb( pADO );
	return lReturnValue;

EXIT_CLOSE_END:
	
	if( pADO->IsErrorStr() )
	{	
		 cDbConF::ReConnectDB( pADO );
		 goto LOOP_ST;
	}
	
	cDbConF::FreeDb( pADO );
	return -1;
}

////////////////////////////

int cDbF::ExChangeDlg( CClientSock* pClient , REQ_CMD* pCmd )
{
	EXCHANGE_DLG_RESP resp;

	CMyADO* pADO = cDbConF::GetFreeDb( 0 );

	if( !pADO )
	{
		cLF_LogError( 100 ,  "[100] !pADO ExChangeDlg" );

		return 30000;
	}

LOOP_ST:
	long lReturnValue = 0;
	HRESULT hRet = NULL;

	PADO_INIT( pADO , "USP_EXCHANGE_DLG2" , goto EXIT_CLOSE_END )

	PADO_ADD_RETURN( pADO , goto EXIT_CLOSE_END )

	PADO_ADD_INPUT_TEXT( pADO , "userid" , pCmd->id , goto EXIT_CLOSE_END )
	PADO_ADD_INPUT_TEXT( pADO , "webkey" , (LPSTR)(LPCTSTR)pClient->m_strWebkey , goto EXIT_CLOSE_END )
	
	PADO_EXEC( pADO , goto EXIT_CLOSE_END )	
	
	PADO_GET_RET( pADO , &lReturnValue , goto EXIT_CLOSE_END )

	if( 1 == lReturnValue )
	{
		if( !pADO->IsEOF() )
		{
			char szName[ 30 ] = {0};
			char szValue[ 100 ] = {0};

			// 보유 포인트 
			HRESULT hReturn = pADO->GetFieldTextByIndex( 0 , szName , 50  , szValue , 100 );
			resp.nUserPoint = _atoi64( szValue );

			hReturn = pADO->GetFieldTextByIndex( 1 , szName , 50  , szValue , 100 );	//bank
			strcpy( resp.bank , szValue );

			hReturn = pADO->GetFieldTextByIndex( 2 , szName , 50  , szValue , 100 );	//bank_num
			strcpy( resp.bankNum , szValue );

			hReturn = pADO->GetFieldTextByIndex( 3 , szName , 50  , szValue , 100 );	//acc_name
			strcpy( resp.accname , szValue );
		}		
	}

	resp.nRet = lReturnValue;
	cSk_SendResp( pClient , 107 , (unsigned char*)&resp , sizeof( resp ) );

	
	cDbConF::FreeDb( pADO );
	return lReturnValue;

EXIT_CLOSE_END:
	
	if( pADO->IsErrorStr() )
	{	
		 cDbConF::ReConnectDB( pADO );
		 goto LOOP_ST;
	}
	
	cDbConF::FreeDb( pADO );
	return -1;
}


////////////////////////////
int cDbF::ExChangeReq( CClientSock* pClient , REQ_CMD* pCmd )
{
	RESP_CMD resp;

	char szNum[ 50 ] = {0};
	sprintf( szNum , "%I64d" , pCmd->nPoint );

	CMyADO* pADO = cDbConF::GetFreeDb( 0 );

	if( !pADO )
	{
		cLF_LogError( 100 ,  "[100] !pADO ChargeDlg" );

		return 30000;
	}

LOOP_ST:
	long lReturnValue = 0;
	HRESULT hRet = NULL;

	PADO_INIT( pADO , "USP_EXCHANGE_REQ2" , goto EXIT_CLOSE_END )

	PADO_ADD_RETURN( pADO , goto EXIT_CLOSE_END )

	PADO_ADD_INPUT_TEXT( pADO , "userid" , pCmd->id , goto EXIT_CLOSE_END )
	PADO_ADD_INPUT_TEXT( pADO , "bankuser" , pCmd->username , goto EXIT_CLOSE_END )
	PADO_ADD_INPUT_TEXT( pADO , "banknum" , pCmd->banknum , goto EXIT_CLOSE_END )
	PADO_ADD_INPUT_TEXT( pADO , "bank" , pCmd->bank , goto EXIT_CLOSE_END )
	PADO_ADD_INPUT_TEXT( pADO , "tel" , pCmd->tel , goto EXIT_CLOSE_END )

	PADO_ADD_INPUT_TEXT( pADO , "expw" , pCmd->expw , goto EXIT_CLOSE_END )
	PADO_ADD_INPUT_TEXT( pADO , "point" , szNum , goto EXIT_CLOSE_END )
	PADO_ADD_INPUT_TEXT( pADO , "webkey" , (LPSTR)(LPCTSTR)pClient->m_strWebkey , goto EXIT_CLOSE_END )
	
	PADO_EXEC( pADO , goto EXIT_CLOSE_END )	
	
	PADO_GET_RET( pADO , &lReturnValue , goto EXIT_CLOSE_END )


	if( 1 == lReturnValue )
	{
		int nIndex = 0;
		int nSendCount = 0;

		if( !pADO->IsEOF() )
		{
			char szName[ 30 ] = {0};
			char szValue[ 1000 ] = {0};

			resp.nRet = lReturnValue;

			// 보유 포인트 
			HRESULT hReturn = pADO->GetFieldTextByIndex( 0 , szName , 50  , szValue , 100 );
			resp.nPoint = atoi( szValue );			
		}		
	}

	resp.nRet = lReturnValue;
	cSk_SendResp( pClient , 108 , (unsigned char*)&resp , sizeof( resp ) );

	
	cDbConF::FreeDb( pADO );
	return lReturnValue;

EXIT_CLOSE_END:
	
	if( pADO->IsErrorStr() )
	{	
		 cDbConF::ReConnectDB( pADO );
		 goto LOOP_ST;
	}
	
	cDbConF::FreeDb( pADO );
	return -1;
}


////////////////////////////

int cDbF::LoginDB( CClientSock* pClient , UINT nThreadIdx , char* pstrId , char* pstrPw , char* pstrIp )
{

	LOGIN_RESP login_resp;

	char szRecentKey[ 60 ] = {0};
	MakeRecentKey( szRecentKey , (int)pClient );

	CMyADO* pADO = cDbConF::GetFreeDb( 0 );

	if( !pADO )
	{
		cLF_LogError( 100 ,  "[100] !pADO LoginDB" );

		return 30000;
	}

LOOP_ST:
	long lReturnValue = 0;
	HRESULT hRet = NULL;

	PADO_INIT( pADO , "USP_LOGIN" , goto EXIT_CLOSE_END )

	PADO_ADD_RETURN( pADO , goto EXIT_CLOSE_END )

	PADO_ADD_INPUT_TEXT( pADO , "userid" , pstrId , goto EXIT_CLOSE_END )
	PADO_ADD_INPUT_TEXT( pADO , "userpw" , pstrPw , goto EXIT_CLOSE_END )
	PADO_ADD_INPUT_TEXT( pADO , "userip" , pstrIp , goto EXIT_CLOSE_END )
	PADO_ADD_INPUT_TEXT( pADO , "webKey" , szRecentKey , goto EXIT_CLOSE_END )
	
	PADO_EXEC( pADO , goto EXIT_CLOSE_END )	
	
	PADO_GET_RET( pADO , &lReturnValue , goto EXIT_CLOSE_END )	

	if( 1 == lReturnValue )
	{
		int nIndex = 0;

		if( !pADO->IsEOF() )
		{
			char szName[ 30 ] = {0};
			char szValue[ 100 ] = {0};

			// 보유 포인트 
			HRESULT hReturn = pADO->GetFieldTextByIndex( 0 , szName , 50  , szValue , 100 );
			login_resp.nPoint = _atoi64( szValue );

			hReturn = pADO->GetFieldTextByIndex( 1 , szName , 50  , szValue , 100 );
			strcpy( login_resp.nick , szValue );

			strcpy( login_resp.webkey , szRecentKey );

			pClient->m_strWebkey = szRecentKey;

			pClient->m_nState = 1;
		}		
	}

	login_resp.nRetCode = lReturnValue;
	strcpy( login_resp.id , pstrId );
	cSk_SendResp( pClient , 101 , (unsigned char*)&login_resp , sizeof( login_resp ) );

	cDbConF::FreeDb( pADO );

	return lReturnValue;

EXIT_CLOSE_END:
	
	if( pADO->IsErrorStr() )
	{	
		 cDbConF::ReConnectDB( pADO );
		 goto LOOP_ST;
	}
	
	cDbConF::FreeDb( pADO );
	return -1;
}

////////////////////////////

int cDbF::QaListDlg( CClientSock* pClient , REQ_CMD* pCmd )
{
	RESP_QA_LIST resp;

	CMyADO* pADO = cDbConF::GetFreeDb( 0 );

	if( !pADO )
	{
		cLF_LogError( 100 ,  "[100] !pADO QaListDlg" );

		return 30000;
	}

LOOP_ST:
	long lReturnValue = 0;
	HRESULT hRet = NULL;

	PADO_INIT( pADO , "USP_QALIST_DLG2" , goto EXIT_CLOSE_END )

	PADO_ADD_RETURN( pADO , goto EXIT_CLOSE_END )

	PADO_ADD_INPUT_TEXT( pADO , "userid" , pCmd->id , goto EXIT_CLOSE_END )
	PADO_ADD_INPUT_TEXT( pADO , "webkey" , (LPSTR)(LPCTSTR)pClient->m_strWebkey , goto EXIT_CLOSE_END )
	
	PADO_EXEC( pADO , goto EXIT_CLOSE_END )	
	
	PADO_GET_RET( pADO , &lReturnValue , goto EXIT_CLOSE_END )

	if( 1 == lReturnValue )
	{
		int nIndex = 0;
		int nSendCount = 0;

		while( !pADO->IsEOF() )
		{
			char szName[ 30 ] = {0};
			char szValue[ 100 ] = {0};

			
			HRESULT hReturn = pADO->GetFieldTextByIndex( 0 , szName , 50  , szValue , 100 );			
			resp.qalist[ nIndex ].nIdx = atoi( szValue );	// 번호 

			hReturn = pADO->GetFieldTextByIndex( 1 , szName , 50  , szValue , 100 );
			resp.qalist[ nIndex ].nOriIdx = atoi( szValue );	

			hReturn = pADO->GetFieldTextByIndex( 2 , szName , 50  , szValue , 100 );

			g_strtrim( szValue );
			strcpy( resp.qalist[ nIndex ].szTitle , szValue );	// 제목 

			hReturn = pADO->GetFieldTextByIndex( 3 , szName , 50  , szValue , 100 );
			memset( &resp.qalist[ nIndex ].szDate , 0x0 , sizeof( resp.qalist[ nIndex ].szDate ) );
			strncpy( resp.qalist[ nIndex ].szDate , &szValue[5] , 5 );	//날짜 		

			hReturn = pADO->GetFieldTextByIndex( 4 , szName , 50  , szValue , 100 );
			strcpy( resp.qalist[ nIndex ].szWriter , szValue );		//작성자 
		
			nIndex++;

			if( 10 == nIndex )	// 10 개 단위로 보낸다.
			{				
				resp.nCount = nIndex;
				resp.nAdd = nSendCount;

				cSk_SendResp( pClient , 109 , (unsigned char*)&resp , sizeof( resp ) );
				memset( &resp , 0x00 , sizeof( resp ) );
				nIndex = 0;
				nSendCount++;
			}

			pADO->MoveNext();
		}

		if( 0 <= nIndex && nIndex < 10 )
		{
			resp.nCount = nIndex;
			resp.nAdd = -1;

			cSk_SendResp( pClient , 109 , (unsigned char*)&resp , sizeof( resp ) );
		}		
	}
	else
	{
		resp.nCount = 0;
		resp.nAdd = lReturnValue;

		cSk_SendResp( pClient , 109 , (unsigned char*)&resp , sizeof( resp ) );
	}

	cDbConF::FreeDb( pADO );
	return lReturnValue;

EXIT_CLOSE_END:
	
	if( pADO->IsErrorStr() )
	{	
		 cDbConF::ReConnectDB( pADO );
		 goto LOOP_ST;
	}
	
	cDbConF::FreeDb( pADO );
	return -1;
}

////////////////////////////////////

int cDbF::ReqQaContent( CClientSock* pClient , REQ_CMD* pCmd )
{
	RESP_QA_CONTENT resp;

	CMyADO* pADO = cDbConF::GetFreeDb( 0 );

	if( !pADO )
	{
		cLF_LogError( 100 ,  "[100] !pADO ReqQaContent" );

		return 30000;
	}

LOOP_ST:
	long lReturnValue = 0;
	HRESULT hRet = NULL;

	PADO_INIT( pADO , "USP_QA_CONTENT" , goto EXIT_CLOSE_END )

	PADO_ADD_RETURN( pADO , goto EXIT_CLOSE_END )

	PADO_ADD_INPUT_LONG( pADO , "dbidx" , pCmd->nDbIdx , goto EXIT_CLOSE_END )
	
	PADO_EXEC( pADO , goto EXIT_CLOSE_END )	
	
	PADO_GET_RET( pADO , &lReturnValue , goto EXIT_CLOSE_END )

	resp.nRet = lReturnValue;

	if( 1 == lReturnValue )
	{
		int nIndex = 0;
		int nSendCount = 0;

		if( !pADO->IsEOF() )
		{
			char szName[ 30 ] = {0};
			char szValue[ 1000 ] = {0};

			 
			HRESULT hReturn = pADO->GetFieldTextByIndex( 0 , szName , 50  , szValue , 100 );			
			resp.nIdx = atoi( szValue );

			hReturn = pADO->GetFieldTextByIndex( 1 , szName , 50  , szValue , 100 );
			resp.nOriIdx = atoi( szValue );

			hReturn = pADO->GetFieldTextByIndex( 2 , szName , 50  , szValue , 100 );
			g_strtrim( szValue );
			strcpy( resp.szTitle , szValue );

			hReturn = pADO->GetFieldTextByIndex( 3 , szName , 50  , szValue , 100 );
			memset( &resp.szDate , 0x0 , sizeof( resp.szDate ) );
			strncpy( resp.szDate , &szValue[5] , 5 );			

			hReturn = pADO->GetFieldTextByIndex( 4 , szName , 50  , szValue , 100 );
			strcpy( resp.szWriter , szValue );	
			
			hReturn = pADO->GetFieldTextByIndex( 5 , szName , 50  , szValue , 1000 );
			strcpy( resp.szContent , szValue );

			cSk_SendResp( pClient , 110 , (unsigned char*)&resp , sizeof( resp ) );
		}		
	}
	else
	{
		
		cSk_SendResp( pClient , 110 , (unsigned char*)&resp , sizeof( resp ) );
	}
	
	
	cDbConF::FreeDb( pADO );
	return lReturnValue;

EXIT_CLOSE_END:
	
	if( pADO->IsErrorStr() )
	{	
		 cDbConF::ReConnectDB( pADO );
		 goto LOOP_ST;
	}
	
	cDbConF::FreeDb( pADO );
	return -1;

}



int cDbF::ReqSendQa( CClientSock* pClient , REQ_CMD* pCmd )
{
	RESP_CMD resp;

	CMyADO* pADO = cDbConF::GetFreeDb( 0 );

	if( !pADO )
	{
		cLF_LogError( 100 ,  "[100] !pADO ReqQaContent" );

		return 30000;
	}

LOOP_ST:
	long lReturnValue = 0;
	HRESULT hRet = NULL;

	PADO_INIT( pADO , "USP_SENDQA_CONTENT2" , goto EXIT_CLOSE_END )

	PADO_ADD_RETURN( pADO , goto EXIT_CLOSE_END )

	PADO_ADD_INPUT_TEXT( pADO , "userid" , pCmd->id , goto EXIT_CLOSE_END )	
	PADO_ADD_INPUT_TEXT( pADO , "title" , pCmd->szTitle , goto EXIT_CLOSE_END )	
	PADO_ADD_INPUT_TEXT( pADO , "content" , pCmd->szContent , goto EXIT_CLOSE_END )	
	PADO_ADD_INPUT_TEXT( pADO , "webkey" , (LPSTR)(LPCTSTR)pClient->m_strWebkey , goto EXIT_CLOSE_END )
	
	PADO_EXEC( pADO , goto EXIT_CLOSE_END )	
	
	PADO_GET_RET( pADO , &lReturnValue , goto EXIT_CLOSE_END )

	resp.nRet = lReturnValue;	
		
	cSk_SendResp( pClient , 111 , (unsigned char*)&resp , sizeof( resp ) );
	
	cDbConF::FreeDb( pADO );
	return lReturnValue;

EXIT_CLOSE_END:
	
	if( pADO->IsErrorStr() )
	{	
		 cDbConF::ReConnectDB( pADO );
		 goto LOOP_ST;
	}
	
	cDbConF::FreeDb( pADO );
	return -1;

}



int cDbF::ReqJoin( CClientSock* pClient , REQ_CMD2* pCmd )
{
	RESP_CMD resp;

	CMyADO* pADO = cDbConF::GetFreeDb( 0 );

	if( !pADO )
	{
		cLF_LogError( 100 ,  "[100] !pADO ReqQaContent" );

		return 30000;
	}

LOOP_ST:
	long lReturnValue = 0;
	HRESULT hRet = NULL;

	PADO_INIT( pADO , "USP_member_join" , goto EXIT_CLOSE_END )

	PADO_ADD_RETURN( pADO , goto EXIT_CLOSE_END )

	PADO_ADD_INPUT_TEXT( pADO , "username" , pCmd->username , goto EXIT_CLOSE_END )	
	PADO_ADD_INPUT_TEXT( pADO , "id" , pCmd->id , goto EXIT_CLOSE_END )	
	PADO_ADD_INPUT_TEXT( pADO , "pwd" , pCmd->pwd , goto EXIT_CLOSE_END )	

	PADO_ADD_INPUT_TEXT( pADO , "bank" , pCmd->bank , goto EXIT_CLOSE_END )	
	PADO_ADD_INPUT_TEXT( pADO , "banknum" , pCmd->banknum , goto EXIT_CLOSE_END )	
	PADO_ADD_INPUT_TEXT( pADO , "bankUser" , pCmd->bankUser , goto EXIT_CLOSE_END )	

	PADO_ADD_INPUT_TEXT( pADO , "ex_pwd" , pCmd->ex_pwd , goto EXIT_CLOSE_END )	

	PADO_ADD_INPUT_TEXT( pADO , "tel" , pCmd->tel , goto EXIT_CLOSE_END )	
	PADO_ADD_INPUT_TEXT( pADO , "recomid" , pCmd->recomid , goto EXIT_CLOSE_END )	

	
	PADO_EXEC( pADO , goto EXIT_CLOSE_END )	
	
	PADO_GET_RET( pADO , &lReturnValue , goto EXIT_CLOSE_END )

	resp.nRet = lReturnValue;	
		
	cSk_SendResp( pClient , 112 , (unsigned char*)&resp , sizeof( resp ) );
	
	cDbConF::FreeDb( pADO );
	return lReturnValue;

EXIT_CLOSE_END:
	
	if( pADO->IsErrorStr() )
	{	
		 cDbConF::ReConnectDB( pADO );
		 goto LOOP_ST;
	}
	
	cDbConF::FreeDb( pADO );
	return -1;

}











void cSk_SetInit( char* pstr )
{

try
{
	int nLen = strlen( pstr );
	if( 0 == nLen )
		strcpy( pstr , " " );
}
catch(...)
{
}

}



CClientSock* cSk_FindUser( char* pUserId )
{

cSockListLock lock;

try
{

	CClientSock* pRetClient = NULL;
	POSITION pos = NULL;
	
	//pos = g_ClientList.GetHeadPosition();
	CClientSock** pList = g_ClientList.GetData();

	//while( pos )
	for( int n = 0 ; n < g_ClientList.GetSize() ; n++ )
	{
		//pRetClient = g_ClientList.GetNext( pos );
		pRetClient = pList[n];

//#ifndef _DEBUG
		if( pRetClient->IsValid() )
		{
			if( !strcmp( pRetClient->m_szID , pUserId ) )	// 같다면 
//#endif
				return pRetClient;	
		}
	}

	return NULL;
}
catch(...)
{

return NULL;

} 

}




void cWF::InitPath()
{
	CString csChannelPath;
	GetModuleFileName( NULL , csChannelPath.GetBufferSetLength( _MAX_PATH ) , _MAX_PATH );
	csChannelPath.ReleaseBuffer();

	int nPos = csChannelPath.ReverseFind( '\\' );
	csChannelPath.Delete( nPos , csChannelPath.GetLength() -nPos );

	csChannelPath += "\\CHANNEL\\";

	m_csChannelPath = csChannelPath;
}









void cWF::fLogin( int nThrIdx , MSG_PACK* pPack , CClientSock* pClient )
{// 로그인 
	cLF_LogFlow( 5000 , "[cWF::fLogin] start time" );


	LOGIN login;
	memcpy( &login , &pPack->Data[0] , sizeof( LOGIN ) );

	login.id[ sizeof( login.id ) -1 ] = 0;
	login.pw[ sizeof( login.pw ) -1 ] = 0;
	login.ip[ sizeof( login.pw ) -1 ] = 0;			
	
	int nRet = cDbF::LoginDB( pClient , nThrIdx , login.id , login.pw , login.ip );

	if( 1 == nRet )	// 성공인 경우 
	{
		pClient->m_szID = login.id;
		pClient->m_szIP = login.ip;
	}

	cLF_LogFlow( 20 , "[cWF::fLogin] nRet(%d) id(%s) pw(%s) " ,
			nRet , login.id , login.pw );

	cLF_LogFlow( 5000 , "[cWF::fLogin] end time" );
}




void sWork( CClientSock* pClient , MSG_PACK* pData , int nIdx )
{

	if( 101 == pData->m_nType )	// 로그인의 경우 
		cWF::fLogin( nIdx , pData , pClient );
	else
	if( 102 == pData->m_nType )	
	{
		PREQ_CMD_HEAD pHead = (PREQ_CMD_HEAD)&pData->Data[0];

		int nRet = 0;

		if( eJoin == pHead->nCMD )
		{
			REQ_CMD2 cmd;
			memcpy( &cmd , &pData->Data[0] , sizeof( cmd ) );	

			nRet = cDbF::ReqJoin( pClient , &cmd );
		}
		else
		{
			REQ_CMD cmd;
			memcpy( &cmd , &pData->Data[0] , sizeof( cmd ) );

			if( eRecentUser == cmd.nCMD )
				nRet = cDbF::ReqRecentUser( pClient , &cmd );
			else
			if( eNotice == cmd.nCMD )
				nRet = cDbF::ReqNotice( pClient , &cmd );
			else
			if( eNoticeContent == cmd.nCMD )
				nRet = cDbF::ReqNoticeContent( pClient , &cmd );
			else
			if( eChargeDlg == cmd.nCMD )
				nRet = cDbF::ChargeDlg( pClient , &cmd );

			else
			if( eChargeReq == cmd.nCMD )
				nRet = cDbF::ChargeReq( pClient , &cmd );
			
			else
			if( eExchangeDlg == cmd.nCMD )
				nRet = cDbF::ExChangeDlg( pClient , &cmd );					

			else
			if( eExchangeReq == cmd.nCMD )
				nRet = cDbF::ExChangeReq( pClient , &cmd );	

			else
			if( eQaListDlg == cmd.nCMD )
				nRet = cDbF::QaListDlg( pClient , &cmd );	

			else
			if( eQaLine == cmd.nCMD )
				nRet = cDbF::ReqQaContent( pClient , &cmd );

			else
			if( eQaSend == cmd.nCMD )
				nRet = cDbF::ReqSendQa( pClient , &cmd );

			else
			if( eRefresh == cmd.nCMD )
				nRet = cDbF::ReqRefresh( pClient , &cmd );
			else
			if( eBoxDlg == cmd.nCMD )
				nRet = cDbF::ReqBoxDlg( pClient , &cmd );
			else
			if( eBoxUse == cmd.nCMD )
				nRet = cDbF::ReqBoxUse( pClient , &cmd );
			else
			if( eBoxPwd == cmd.nCMD )
				nRet = cDbF::ReqBoxPwd( pClient , &cmd );
			else
			if( eCoupon == cmd.nCMD )
				nRet = cDbF::ReqCoupon( pClient , &cmd );

		}
	}				
	else	// 잘못된 정보
	{
		cSk_PostMsg( (WPARAM)pClient , (LPARAM)4 );
	}

	cLF_LogFlow( 5000 , "[WorkDb]  pack.m_nType(%d) end time" , pData->m_nType );

}

void WorkDb( CClientSock* pClient , int nIdx , int nExpt = 0 );
void WorkDb( CClientSock* pClient , int nIdx , int nExpt )
{

try
{
cLogBuff buffLock;		// 2013

	if( pClient->IsValid() )	
	{
		MSG_PACK tHead;

		int nLimit = 0;

		while( pClient->GetRecvSize() >= sizeof( MSG_HEAD ) )
		{
			memcpy( &tHead , &pClient->m_buff[ pClient->m_nReadPos ] , sizeof( MSG_HEAD ) );	// 헤더  
			tHead.decode_head();
			
			// 바디
			int nBuffSize = pClient->GetRecvSize();

			if( tHead.m_nMsgSize <= 0 )
			{
				cLF_LogFlow( 1000 , "[WorkDb]tHead.m_nType(%d) tHead.m_nMsgSize(%d) m_nReadPos(%d)" ,
							tHead.m_nType , tHead.m_nMsgSize , pClient->m_nReadPos );

				break;
			}
			else
			if( nBuffSize >= tHead.m_nMsgSize )
			{
				MSG_PACK* pData = (MSG_PACK*)&pClient->m_buff[ pClient->m_nReadPos ];
				pData->decode_head();
				pData->decode_body();

				if( !pData->IsOkCheckSum() )		// 정상 패킷이 아니라면 
				{
					cLF_LogError( 10 , "[WorkDb] !pack.IsOkCheckSum() m_hSocket(%d)" , pClient->m_hSocket );
					//pClient->End();		// 소켓 종료 			
					cSk_PostMsg( (WPARAM)pClient, (LPARAM)6 );
					return ;
				}

				pClient->m_nReadPos += pData->m_nMsgSize;		// 처리한 메세지 위치 이동 	

				cLF_LogFlow( 1000 , "[WorkDb] pack.m_nType(%d) m_nMsgSize(%d) m_nReadPos(%d)" ,
							pData->m_nType , pData->m_nMsgSize , pClient->m_nReadPos );
				
				sWork( pClient , pData , nIdx );		// 2013
				
			}
			else
				break;

			nLimit++;

			if( nLimit > 50 )
			{
				cLF_LogError( 10000 , "[WorkDb] nLimit(%d) GetRecvSize(%d)" ,
					nLimit , pClient->GetRecvSize() );
				break;
			}
		}
	}
}
catch(...)
{
}

}

typedef struct tagTHEAD_INFO
{
	HANDLE hThread;
	DWORD dwThreadId;

}THEAD_INFO , *PTHEAD_INFO;



THEAD_INFO G_Thread[ MAX_THREAD ];

bool G_bRun[ MAX_THREAD ];
int G_nSleep = 100;

UINT G_nThreadMsg = WM_USER +1000;



DWORD WINAPI ThreadFuncMsg( LPVOID lpParam ) 
{ 
	int nThreadIdx = -1;

	nThreadIdx = *(int*)lpParam;

	MSG msg;

	while( GetMessage( &msg , NULL , G_nThreadMsg +nThreadIdx , G_nThreadMsg +nThreadIdx ) )
	{

		CClientSock* pClient = (CClientSock*)msg.wParam;

		WorkDb( pClient , -1 );	// 작업 처리 

		//while( ::PeekMessage( &msg , NULL , G_nThreadMsg +nThreadIdx , G_nThreadMsg +nThreadIdx , PM_REMOVE ) );
		::PeekMessage( &msg , NULL , G_nThreadMsg +nThreadIdx , G_nThreadMsg +nThreadIdx , PM_REMOVE );

		//Sleep( 0 );		
	}
    
    return 0; 
} 


int CDbSvrDlg::StartThread()
{
	int n = 0;
	
	if( 2 == G_nSvrType )
	{// 스레드당 그룹으로 할당  
		for( n = 0 ; n < MAX_MSG_THREAD ; n++ )
		{
			DWORD dwThreadId = 0 , dwThrdParam = n;

			HANDLE hThread = CreateThread(
			NULL , 
			0 , 
			ThreadFuncMsg ,
			&dwThrdParam , 
			0 , 
			&dwThreadId );
			
			G_Thread[ n ].hThread = hThread;
			G_Thread[ n ].dwThreadId = dwThreadId;

			if( !hThread )
				return n;
		}
	}
	else if( 3 == G_nSvrType )
	{// 순차적으로 스레드에 할당 
		for( n = 0 ; n < MAX_MSG_THREAD ; n++ )
		{
			DWORD dwThreadId = 0 , dwThrdParam = n;

			HANDLE hThread = CreateThread(
			NULL , 
			0 , 
			ThreadFuncMsg ,
			&dwThrdParam , 
			0 , 
			&dwThreadId );
			
			G_Thread[ n ].hThread = hThread;
			G_Thread[ n ].dwThreadId = dwThreadId;

			if( !hThread )
				return n;
		}
	}
	else if( 4 == G_nSvrType )
	{
		for( n = 0 ; n < MAX_MSG_THREAD ; n++ )
		{
			DWORD dwThreadId = 0 , dwThrdParam = n;

			HANDLE hThread = CreateThread(
			NULL , 
			0 , 
			ThreadFuncMsg ,
			&dwThrdParam , 
			0 , 
			&dwThreadId );
			
			G_Thread[ n ].hThread = hThread;
			G_Thread[ n ].dwThreadId = dwThreadId;

			if( !hThread )
				return n;
		}
	}


	return -1;
}

void CDbSvrDlg::CloseThread()
{
	int n;

	for( n = 0 ; n < MAX_MSG_THREAD ; n++ )
	{
		PostThreadMessage( G_Thread[ n ].dwThreadId , WM_QUIT , 0 , 0 );

		HANDLE hHandle = G_Thread[ n ].hThread;

		DWORD dwRet = WaitForSingleObject( hHandle , 0 );
		if( WAIT_OBJECT_0 == dwRet )
			dwRet = WaitForSingleObject( hHandle , INFINITE );
	}

	for( n = MAX_MSG_THREAD ; n < MAX_THREAD ; n++ )
	{
		G_bRun[ n ] = false;

		HANDLE hHandle = G_Thread[ n ].hThread;

		DWORD dwRet = WaitForSingleObject( hHandle , 0 );
		if( WAIT_OBJECT_0 == dwRet )
			dwRet = WaitForSingleObject( hHandle , INFINITE );
	}
}

LRESULT CDbSvrDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{// 메세지 처리를 한다.
	// TODO: Add your specialized code here and/or call the base class

	if( UM_SHOW_AVAIL == message )
		ShowStateInfo();
	else if( UM_RECEIVE_MSG == message )		// 작업 메세지인 경우 
	{
		CClientSock* pClient = (CClientSock*)wParam;	// 소켓 포인터를 가져온다.

		if( 1 == lParam )	// 작업 내용인 경우 
		{			
			if( 2 == G_nSvrType )
			{	// 스레드당 그룹으로 할당  
				int nIdx = pClient->m_hSocket %MAX_MSG_THREAD;				
				PostThreadMessage( G_Thread[ nIdx ].dwThreadId , G_nThreadMsg +nIdx , wParam , lParam );				
			}
			else if( 3 == G_nSvrType )
			{	// 순차적으로 스레드에 할당 
				G_nUsingIdx++;
				if( G_nUsingIdx >= MAX_MSG_THREAD )
					G_nUsingIdx = 0;

				int nIdx = G_nUsingIdx;
			
				PostThreadMessage( G_Thread[ nIdx ].dwThreadId , G_nThreadMsg +nIdx , wParam , lParam );
			}
			else if( 4 == G_nSvrType )
			{	// 스레드당 그룹으로 할당 
				int nIdx = pClient->m_hSocket %MAX_MSG_THREAD;			
				PostThreadMessage( G_Thread[ nIdx ].dwThreadId , G_nThreadMsg +nIdx , wParam , lParam );
			}
			else if( 1 == G_nSvrType )
			{
				WorkDb( pClient , -1 , 1007 );	// 작업 처리 
			}
			else	// 메세지큐 
				WorkDb( pClient , -1 );	// 작업 처리 
		}			
		else
		{ // 이 순서를 지켜라.

			// 통신이 끊어진 경우 
			if( pClient->m_szID.GetLength() >= 0 )
			{	// 로그인 한 경우 아웃 처리 한다.
				cDbF::LogoutDB( pClient , (LPSTR)(LPCTSTR)pClient->m_szID );
			}

			pClient->End();			// 소켓을 종료한다.	
		}
	}

	return CDialog::WindowProc(message, wParam, lParam);
}




int CDbSvrDlg::ConnDB()
{// 디비를 연결한다. 

	// 접속에 사용할 정보를 구한다.
	GetDlgItemText( IDC_E_DB_NAME , csDbName );		// 디비 이름
	GetDlgItemText( IDC_E_DB_IP , csDbIp );			// 디비 서버 아이피 
	GetDlgItemText( IDC_E_DB_PORT , csDbPort );		// 디비 서버 포트 
	GetDlgItemText( IDC_E_DB_ID , csDbId );			// 디비 접속 아이디 
	GetDlgItemText( IDC_E_DB_PW , csDbPw );			// 디비 패스워드 

	int nRet = cDbConF::OpenDB();		// 디비 연결을 연다. 

	if( -1 != nRet )
		return 0;
	else
		return 1;
}

void CDbSvrDlg::CloseAllClient()
{// 소켓들 전부 종료 

cSockListLock lock;

	CClientSock* pSock = NULL;

	CClientSock** pList = g_ClientList.GetData();
	
	//while( g_ClientList.GetCount() > 0 )	// 리스트에 개수가 있는 동안 
	for( int n = 0 ; n < g_ClientList.GetSize() ; n++ )
	{
		//pSock = g_ClientList.RemoveTail();	// 맨 끝을 가져온다.
		pSock = pList[n];
		if( pSock )		// 소켓이 있다면 
		{
			pSock->End();			// 종료 처리 

			delete pSock;			// 소켓 소멸 
		}
	}

	g_ClientList.RemoveAll();
}


void CDbSvrDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here

	CloseThread();

	cDbConF::CloseDB();		// 디비 상태를 종료 

	CloseAllClient();


}








void CDbSvrDlg::OnDblclkLLog() 
{
	// TODO: Add your control notification handler code here


}

BOOL CDbSvrDlg::OnQueryEndSession() 
{
	if (!CDialog::OnQueryEndSession())
		return FALSE;
	
	// TODO: Add your specialized query end session code here
		
	return TRUE;
}

BOOL CDbSvrDlg::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class


	
	return CDialog::PreTranslateMessage(pMsg);
}


/*
PostThreadMessage로 SendThreadMessage 구현하기
 
스레드에서 메시지 처리를 하는 경우 스레드에서 특별히 윈도우를 만들지 않는 이상
 PostThreadMessage 밖에 메시지를 보낼 수 있는 방법이 없다.

특별한 경우 SendThreadMessage 같은 걸 사용하고 싶은 경우 Event 객체를 써서 비슷하게 구현할 수 있다.
 
 
메시지 전송 부분
 
enum ThreadMessageFlag { TMF_ASYNC=0, TMF_SYNC };
HANDLE waitMsg = CreateEvent(NULL, TRUE, FALSE, NULL);

if(PostThreadMessage(threadId_, WM_THREAD_INITIALIZE, reinterpret_cast&lt;WPARAM&gt;(waitMsg), TMF_SYNC)) 
{
 if(WaitForSingleObject(waitMsg, MSG_TIMEOUT) != WAIT_OBJECT_0) 
 {
  CloseHandle(waitMsg);
  return false;
 }
}
CloseHandle(waitMsg);
return true;
 
=========================================================

메시지 받는 부분
 
// 메시지 처리
...
// 메시지 동기화
if(lParam == TMF_SYNC) 
{
 HANDLE waitMsg = reinterpret_cast<HANDLE>(wParam);
 SetEvent(waitMsg);
}
return;
 
메시지 전송 부분에서 MSG_TIMEOUT 을 INFINITE로 하면 
SendMessage와 비슷하게 되고 시간을 설정하게 되면
SendMessageTimeOut 형태로 구현할 수 있게 된다.
INFINITE 로 대기하게 되는 경우 데드락이 발생할 수 있으니
 메시지를 받는 부분에서 이벤트 처리를 확실하게 해야 한다.
*/

void CDbSvrDlg::OnBtnShow() 
{
	// TODO: Add your control notification handler code here
	gpThis->PostMessage( UM_SHOW_AVAIL , 0 , 0 );
}

void CDbSvrDlg::OnBtnFindW() 
{
	// TODO: Add your control notification handler code here
	InitTrace();
}

void CDbSvrDlg::ReadSet()
{
	char szInitFile[ 260 ];
	cLF_GetCurrentPath( (char*)&szInitFile );

	strcpy( szInitFile , "\\DbSvrset.ini" );

	CString csNum;

	int nNum = 0;


	G_nReelPoint = nNum;
	
	GetPrivateProfileString( "연결점검주기" , "SEC" , "60" , 
		csNum.GetBufferSetLength( 20 ) , 20 , szInitFile );
	csNum.ReleaseBuffer();

	nNum = atoi( csNum );
	SetDlgItemInt( IDC_EDT_CONN_CHECK_PRD , nNum );

	GetPrivateProfileString( "서버방식" , "IDX" , "0" , csNum.GetBufferSetLength( 20 ) , 20 , szInitFile );
	csNum.ReleaseBuffer();

	nNum = atoi( csNum );
	((CComboBox*)GetDlgItem( IDC_CMB_WORK_TYPE ))->SetCurSel( nNum );

	
	GetPrivateProfileString( "로그레벨" , "레벨" , "100" , csNum.GetBufferSetLength( 20 ) , 20 , szInitFile );
	csNum.ReleaseBuffer();

	nNum = atoi( csNum );
	SetDlgItemInt( IDC_E_LEVEL , nNum );
	
}

void CDbSvrDlg::OnBtnSaveSet() 
{
	// TODO: Add your control notification handler code here

	char szInitFile[ 260 ];
	cLF_GetCurrentPath( (char*)&szInitFile );

	strcpy( szInitFile , "\\DbSvrset.ini" );

	CString csNum;

	int nNum = 0;
	nNum = GetDlgItemInt( IDC_EDT_CONN_CHECK_PRD );
	csNum.Format( "%d" , nNum );

	WritePrivateProfileString( "연결점검주기" , "SEC" , csNum , szInitFile );

	nNum = ((CComboBox*)GetDlgItem( IDC_CMB_WORK_TYPE ))->GetCurSel();
	csNum.Format( "%d" , nNum );

	WritePrivateProfileString( "서버방식" , "IDX" , csNum , szInitFile );

	nNum = GetDlgItemInt( IDC_E_LEVEL );
	csNum.Format( "%d" , nNum );

	WritePrivateProfileString( "로그레벨" , "레벨" , csNum , szInitFile );

}



void CDbSvrDlg::OnSelendokCmbWorkType() 
{
	// TODO: Add your control notification handler code here
	
}
