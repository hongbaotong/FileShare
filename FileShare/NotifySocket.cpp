// NotifySocket.cpp : 实现文件
//

#include "stdafx.h"
//#include "DirectoryServer.h"
#include "NotifySocket.h"


// CNotifySocket

CNotifySocket::CNotifySocket(ISocketEvent* pEventReceiver)
:m_pEventReceiver(pEventReceiver)
{
}

CNotifySocket::~CNotifySocket()
{
}


// CNotifySocket 成员函数

CString CNotifySocket::GetPeerAddrStr()
{
	CString res;
	CString strPeerIP;
	UINT uPeerPort;
	if(GetPeerName(strPeerIP,uPeerPort))
	{
		res.Format(_T("%s:%u"),strPeerIP,uPeerPort);
	}
	return res;

}

void CNotifySocket::OnConnect(int nErrorCode)
{
	if(m_pEventReceiver != NULL)
	{
		m_pEventReceiver->OnSocketConnect(this,nErrorCode);
	}
}

void CNotifySocket::OnAccept(int nErrorCode)
{
	if(m_pEventReceiver != NULL)
	{
		m_pEventReceiver->OnSocketAccept(this,nErrorCode);
	}
}
void CNotifySocket::OnClose(int nErrorCode)
{
	if(m_pEventReceiver != NULL)
	{
		m_pEventReceiver->OnSocketClose(this,nErrorCode);
	}
}
void CNotifySocket::OnReceive(int nErrorCode)
{
	if(m_pEventReceiver != NULL)
	{
		m_pEventReceiver->OnSocketReceive(this,nErrorCode);
	}
}

void CNotifySocket::OnSend(int nErrorCode)
{
	if(m_pEventReceiver != NULL)
	{
		m_pEventReceiver->OnSocketSend(this,nErrorCode);
	}
}

void CNotifySocket::OnOutOfBandData(int nErrorCode)
{
	if(m_pEventReceiver != NULL)
	{
		m_pEventReceiver->OnSocketOutOfBandData(this,nErrorCode);
	}
}
