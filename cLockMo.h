// cLockMo.h: interface for the cLockMo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CLOCKMO_H__90965F30_15F7_45D0_BE03_2F75DAC9610F__INCLUDED_)
#define AFX_CLOCKMO_H__90965F30_15F7_45D0_BE03_2F75DAC9610F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000




class cLogLock
{// �α׿� �� 
public:
	cLogLock();
	~cLogLock();
};


class cSockListLock
{// ����ó���� �� 
public:
	cSockListLock();
	~cSockListLock();
};



class cGetDbLock
{// ��� �Ҵ�� ��
public:
	cGetDbLock();
	~cGetDbLock();
};

class cLogErrorLock
{// ���� �α׿� ��
public:
	cLogErrorLock();
	~cLogErrorLock();
};

class cLogFlowLock
{// ���� �α׿� ��
public:
	cLogFlowLock();
	~cLogFlowLock();
};

class cLogBuff
{// ���� �α׿� ��
public:
	cLogBuff();
	~cLogBuff();
};



#endif // !defined(AFX_CLOCKMO_H__90965F30_15F7_45D0_BE03_2F75DAC9610F__INCLUDED_)
