#pragma once
class CNotifySocket;

class ISocketEvent abstract 
{
public:
	virtual void OnSocketAccept(CNotifySocket* pSrcSocket,int nErrorCode)=0;
	virtual void OnSocketConnect(CNotifySocket* pSrcSocket,int nErrorCode)=0;
	virtual void OnSocketClose(CNotifySocket* pSrcSocket,int nErrorCode)=0;
	virtual void OnSocketSend(CNotifySocket* pSrcSocket,int nErrorCode)=0;
	virtual void OnSocketReceive(CNotifySocket* pSrcSocket,int nErrorCode)=0;
	virtual void OnSocketOutOfBandData(CNotifySocket* pSrcSocket,int nErrorCode)=0;
};
