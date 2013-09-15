// cLockMo.cpp: implementation of the cLockMo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DbSvr.h"
#include "cLockMo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


CCriticalSection g_logLock;
CCriticalSection g_Lock;

CCriticalSection g_GetDbLock;
CCriticalSection g_LogErrorLock;
CCriticalSection g_LogFlowLock;

CCriticalSection g_Buff;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


cLogLock::cLogLock()
{
	g_logLock.Lock();
}

cLogLock::~cLogLock()
{
	g_logLock.Unlock();
}

////////////////////
cSockListLock::cSockListLock()
{
	g_Lock.Lock();
}

cSockListLock::~cSockListLock()
{
	g_Lock.Unlock();
}

///////////////
cGetDbLock::cGetDbLock()
{
	g_GetDbLock.Lock();
}


cGetDbLock::~cGetDbLock()
{
	g_GetDbLock.Unlock();
}

/////////////////////////
cLogErrorLock::cLogErrorLock()
{
	g_LogErrorLock.Lock();
}


cLogErrorLock::~cLogErrorLock()
{
	g_LogErrorLock.Unlock();
}

////////////////////
cLogFlowLock::cLogFlowLock()
{
	g_LogFlowLock.Lock();
}


cLogFlowLock::~cLogFlowLock()
{
	g_LogFlowLock.Unlock();
}


//////////////////////
cLogBuff::cLogBuff()
{
	g_Buff.Lock();
}


cLogBuff::~cLogBuff()
{
	g_Buff.Unlock();
}

