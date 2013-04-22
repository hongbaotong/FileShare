// ClientSocket.cpp : 实现文件
//

#include "stdafx.h"
#include "FileShare.h"
#include "ClientSocket.h"


// CClientSocket

CClientSocket::CClientSocket(ClientSocketEvent* pEventReceiver)
:m_pEventReceiver(pEventReceiver)
{
}

CClientSocket::~CClientSocket()
{
}


// CClientSocket 成员函数
void CClientSocket::OnConnect(int nErrorCode)
{
	if(m_pEventReceiver != NULL)
	{
		m_pEventReceiver->OnSocketConnect(this,nErrorCode);
	}
}

void CClientSocket::OnAccept(int nErrorCode)
{
	if(m_pEventReceiver != NULL)
	{
		m_pEventReceiver->OnSocketAccept(this,nErrorCode);
	}
}

void CClientSocket::OnReceive(int nErrorCode)
{
	if(m_pEventReceiver != NULL)
	{
		m_pEventReceiver->OnSocketReceive(this,nErrorCode);
	}
}

void CClientSocket::OnClose(int nErrorCode)
{
	if(m_pEventReceiver != NULL)
	{
		m_pEventReceiver->OnSocketClose(this,nErrorCode);
	}
}

void CClientSocket::OnSend(int nErrorCode)
{
	if(m_pEventReceiver != NULL)
	{
		m_pEventReceiver->OnSocketSend(this,nErrorCode);
	}
}

void CClientSocket::OnOutOfBandData(int nErrorCode)
{
	if(m_pEventReceiver != NULL)
	{
		m_pEventReceiver->OnSocketOutOfBandData(this,nErrorCode);
	}
}