#pragma once
class CClientSocket;

class ClientSocketEvent abstract
{
public:
	virtual void OnSocketAccept(CClientSocket* pSrcSocket,int nErrorCode)=0;
	virtual void OnSocketConnect(CClientSocket* pSrcSocket,int nErrorCode)=0;
	virtual void OnSocketClose(CClientSocket* pSrcSocket,int nErrorCode)=0;
	virtual void OnSocketSend(CClientSocket* pSrcSocket,int nErrorCode)=0;
	virtual void OnSocketReceive(CClientSocket* pSrcSocket,int nErrorCode)=0;
	virtual void OnSocketOutOfBandData(CClientSocket* pSrcSocket,int nErrorCode)=0;
};
