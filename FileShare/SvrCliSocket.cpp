// DirSvrCliSocket.cpp : 实现文件
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


// CSvrCliSocket 成员函数
void CSvrCliSocket::SetRecvTimeOut(DWORD nTimeOut)
{
	m_nRecvTimeOut = nTimeOut;
}

BOOL CSvrCliSocket::OnMessagePending()
{
	DWORD dwTickNow = GetTickCount();

	//检测是否超时
	if(dwTickNow - m_nTickBeignRecv>m_nRecvTimeOut)
		CancelBlockingCall();

	return CSocket::OnMessagePending();
}

int CSvrCliSocket::Receive(void* lpBuf, int nBufLen, int nFlags)
{
	//记录接受的开始时间
	m_nTickBeignRecv = GetTickCount();

	return CSocket::Receive(lpBuf, nBufLen, nFlags);
}
