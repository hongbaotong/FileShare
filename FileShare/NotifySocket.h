#pragma once
#include <afxsock.h>
#include "ISocketEvent.h"
// CNotifySocket ÃüÁîÄ¿±ê

class CNotifySocket : public CAsyncSocket
{
public:
	CNotifySocket(ISocketEvent* pEventReceiver = NULL);
	virtual ~CNotifySocket();

	CString GetPeerAddrStr();
protected:
	void OnConnect(int nErrorCode);
	void OnAccept(int nErrorCode);
	void OnClose(int nErrorCode);
	void OnReceive(int nErrorCode);
	void OnSend(int nErrorCode);
	void OnOutOfBandData(int nErrorCode);

protected:
	ISocketEvent* m_pEventReceiver;
};


