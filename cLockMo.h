// cLockMo.h: interface for the cLockMo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CLOCKMO_H__90965F30_15F7_45D0_BE03_2F75DAC9610F__INCLUDED_)
#define AFX_CLOCKMO_H__90965F30_15F7_45D0_BE03_2F75DAC9610F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000




class cLogLock
{// 로그용 락 
public:
	cLogLock();
	~cLogLock();
};


class cSockListLock
{// 소켓처리용 락 
public:
	cSockListLock();
	~cSockListLock();
};



class cGetDbLock
{// 디비 할당용 락
public:
	cGetDbLock();
	~cGetDbLock();
};

class cLogErrorLock
{// 에러 로그용 락
public:
	cLogErrorLock();
	~cLogErrorLock();
};

class cLogFlowLock
{// 에러 로그용 락
public:
	cLogFlowLock();
	~cLogFlowLock();
};

class cLogBuff
{// 에러 로그용 락
public:
	cLogBuff();
	~cLogBuff();
};



#endif // !defined(AFX_CLOCKMO_H__90965F30_15F7_45D0_BE03_2F75DAC9610F__INCLUDED_)
