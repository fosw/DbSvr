#pragma once


////////////////////////
CCriticalSection G_mLock;

class CThreadSyncEx
{
public:
	CThreadSyncEx( bool bUse = true )
	{
		m_bUse = bUse;

		if( m_bUse )
			G_mLock.Lock();
	}

	~CThreadSyncEx(VOID)
	{
		if( m_bUse )
			G_mLock.Unlock();
	}

	bool m_bUse;
};


///////////////////////////////////////
typedef struct tagMFP
{
	tagMFP()
	{
		reset();
	}

	void reset()
	{
		p = NULL;
		pr = NULL;
		nx = NULL;
	}

	char* p;
	tagMFP* pr;
	tagMFP* nx;
	
}MFP, *PMFP;

static MFP* G_pMemFreePage;
static char* G_pFree;

const int MP_ALSIZE = 50;

bool G_bFreeMulti = true;	// ��Ƽ�� �ҽ����� ��Ƽ ������ ����ȭ �ʿ��ϳ� 

#define MP_MULTI		// ��Ƽ ���μ����� �ҽ��� �� �ų� 

void op_alloc( size_t lsize )
{
	G_pFree = (char*)malloc( lsize *MP_ALSIZE );

	MFP* mfp = (MFP*)malloc( sizeof( MFP) );
	mfp->reset();

	mfp->p = G_pFree;

	if( !G_pMemFreePage )
		G_pMemFreePage = mfp;
	else
	{
		G_pMemFreePage->nx = mfp;
		mfp->pr = G_pMemFreePage;
		G_pMemFreePage = mfp;
	}

	char **Current = reinterpret_cast< char** >( G_pFree );
	char *Next = G_pFree;

	for( int i = 0 ; i < MP_ALSIZE -1 ; ++i )
	{
		Next += lsize;
		*Current = Next;
		Current	= reinterpret_cast< char** >(Next);
	}

	*Current = 0;
}

void MemFreePage()
{// ���μ��� ����ø� ȣ��ǰ�		
	MFP* mfp = G_pMemFreePage;

	while( mfp )
	{
		if( mfp->p )
			free( mfp->p );
		mfp->p = NULL;

		if( !mfp->pr )
			break;

		mfp = mfp->pr;

		if( mfp->nx )
			free( mfp->nx );

		mfp->nx = NULL;
	}

	if( mfp )
		free( mfp );
	mfp = NULL;	

	G_pMemFreePage = NULL;
}

//
class cFreeMemPool
{
public:
	~cFreeMemPool()
	{
		MemFreePage();
	}
};

cFreeMemPool G_FreeAll;		// ���� �ڵ����� �޸� ������ ���ؼ�
//

void* operator new( size_t lsize )
{
#ifdef MP_MULTI
	CThreadSyncEx sync( G_bFreeMulti );
#endif

//	assert( sizeof( T ) == allocLength );
//	assert( sizeof( T ) >= sizeof( char* ) );

	if( !G_pFree )
		op_alloc( lsize );

	char *pRet = G_pFree;
	G_pFree = *reinterpret_cast< char** >( pRet );

	return pRet;
}

void operator delete( void* dP )
{
#ifdef MP_MULTI
	CThreadSyncEx sync( G_bFreeMulti );
#endif

	*reinterpret_cast< char** >( dP ) = G_pFree;
	G_pFree = static_cast< char* >( dP );
}


//////////////////////////////////////////////