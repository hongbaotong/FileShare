// DirSvrCliSocket.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "FileShare.h"
#include "SvrCliSocket.h"


// CSvrCliSocket

CSvrCliSocket::CSvrCliSocket()
{
}

CSvrCliSocket::~CSvrCliSocket()
{
}


// CSvrCliSocket ��Ա����
void CSvrCliSocket::SetRecvTimeOut(DWORD nTimeOut)
{
	m_nRecvTimeOut = nTimeOut;
}

BOOL CSvrCliSocket::OnMessagePending()
{
	DWORD dwTickNow = GetTickCount();

	//����Ƿ�ʱ
	if(dwTickNow - m_nTickBeignRecv>m_nRecvTimeOut)
		CancelBlockingCall();

	return CSocket::OnMessagePending();
}

int CSvrCliSocket::Receive(void* lpBuf, int nBufLen, int nFlags)
{
	//��¼���ܵĿ�ʼʱ��
	m_nTickBeignRecv = GetTickCount();

	return CSocket::Receive(lpBuf, nBufLen, nFlags);
}
