#pragma once
#include "ClientSocket.h"
// CBroadCastThread

class CBroadCastThread : public CWinThread,public ClientSocketEvent
{
	DECLARE_DYNCREATE(CBroadCastThread)

public:
	CBroadCastThread();           // 动态创建所使用的受保护的构造函数
	virtual ~CBroadCastThread();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

protected:
	DECLARE_MESSAGE_MAP()
private:
	CClientSocket * m_BroadSocket;
	CClientSocket * m_RecvSocket;
	BOOL m_bBroadCast;
	BOOL m_bRecvCast;
public:
	void OnBroadCast(void);
public:
	void OnSocketAccept(CClientSocket* pSrcSocket,int nErrorCode);
	void OnSocketConnect(CClientSocket* pSrcSocket,int nErrorCode);
	void OnSocketClose(CClientSocket* pSrcSocket,int nErrorCode);
	void OnSocketSend(CClientSocket* pSrcSocket,int nErrorCode);
	void OnSocketReceive(CClientSocket* pSrcSocket,int nErrorCode);
	void OnSocketOutOfBandData(CClientSocket* pSrcSocket,int nErrorCode);
public:
	void SetHwnd(HWND hWnd);
private:
	HWND m_hParentWnd;
};


