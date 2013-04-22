#pragma once
#include "ClientSocketEvent.h"
// CUdpSocket ÃüÁîÄ¿±ê

class CClientSocket : public CSocket
{
public:
	CClientSocket(ClientSocketEvent* pEventReceiver=NULL);
	virtual ~CClientSocket();
public:
	void OnConnect(int nErrorCode);
	void OnAccept(int nErrorCode);
	void OnReceive(int nErrorCode);
	void OnClose(int nErrorCode);
	void OnSend(int nErrorCode);
	void OnOutOfBandData(int nErrorCode);
protected:
	ClientSocketEvent* m_pEventReceiver;
};


