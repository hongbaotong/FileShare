#pragma once

// CSvrCliSocket ÃüÁîÄ¿±ê

class CSvrCliSocket : public CSocket
{
public:
	CSvrCliSocket();
	virtual ~CSvrCliSocket();

	void SetRecvTimeOut(DWORD nTimeOut);
protected:
	DWORD m_nRecvTimeOut;
	DWORD m_nTickBeignRecv;


public:
	virtual BOOL OnMessagePending();
public:
	virtual int Receive(void* lpBuf, int nBufLen, int nFlags = 0);
};


