// BroadCastThread.cpp : 实现文件
//

#include "stdafx.h"
#include "FileShare.h"
#include "BroadCastThread.h"
#include "PackageParser.h"


// CBroadCastThread

IMPLEMENT_DYNCREATE(CBroadCastThread, CWinThread)

CBroadCastThread::CBroadCastThread()
{
}

CBroadCastThread::~CBroadCastThread()
{
	
}

BOOL CBroadCastThread::InitInstance()
{
	// TODO: 在此执行任意逐线程初始化
	CoInitialize(NULL);
	if(AfxSocketInit())
	{
		m_BroadSocket=new CClientSocket(this);
		m_RecvSocket=new CClientSocket(this);
		
		m_BroadSocket->Create(0,SOCK_DGRAM);
		m_RecvSocket->Create(8021,SOCK_DGRAM);
		m_bBroadCast=TRUE;
		int nRecvBuf=32*1024;
		m_BroadSocket->SetSockOpt(SO_BROADCAST,&m_bBroadCast,sizeof(BOOL));
		m_RecvSocket->SetSockOpt(SO_RCVBUF,(const char*)&nRecvBuf,sizeof(int));
	}
	return TRUE;
}

int CBroadCastThread::ExitInstance()
{
	// TODO: 在此执行任意逐线程清理
	if(m_BroadSocket!=NULL)
	{
		m_BroadSocket->Close();
		delete m_BroadSocket;
		m_BroadSocket = NULL;
	}
	if(m_RecvSocket!=NULL)
	{
		m_RecvSocket->Close();
		delete m_RecvSocket;
		m_RecvSocket = NULL;
	}
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CBroadCastThread, CWinThread)
END_MESSAGE_MAP()


// CBroadCastThread 消息处理程序

void CBroadCastThread::OnBroadCast(void)
{
	CBuffer bufToSend;
	CString strAddr;
	CString strPort;
//	PACKAGE_HEAD *pDataHead=new PACKAGE_HEAD;

	CPackageParser* pPackageParser = new CPackageParser();
	pPackageParser->CreateNewPackage();
	pPackageParser->SetPackageProc(_T("Login"),_T("BroadCast"));
	CPackageParser::RECORD_HANDLE hRec = pPackageParser->CreateNewRecord();
	pPackageParser->SetFieldValue(hRec,_T("SvrName"),GetSvrApp()->m_ShareInfo->GetSvrName());

	strPort.Format(_T("%u"),GetSvrApp()->m_ShareInfo->GetSvrPort());
	pPackageParser->SetFieldValue(hRec,_T("Port"),strPort);

	if(pPackageParser->SaveToBuf(bufToSend))
	{
		CBuffer *pSendData = new CBuffer();
		if(pSendData!=NULL)
		{
			//加装数据包头
			if(pSendData->Alloc(bufToSend.GetDataLen()+PACKAGE_HEAD_LEN))
			{
				//填写数据包头部
				DWORD* pPackageEntityLen = (DWORD*)pSendData->GetBufP();
				*pPackageEntityLen=bufToSend.GetDataLen();
				pSendData->SetDataLen(PACKAGE_HEAD_LEN);
				pSendData->AppendData(bufToSend);
			}
			m_BroadSocket->SendTo(pSendData->GetBufP(),pSendData->GetDataLen(),8021,_T("255.255.255.255"));
		}
	}
}

void CBroadCastThread::OnSocketAccept(CClientSocket* pSrcSocket,int nErrorCode)
{
}

void CBroadCastThread::OnSocketConnect(CClientSocket* pSrcSocket,int nErrorCode)
{
}

void CBroadCastThread::OnSocketReceive(CClientSocket* pSrcSocket,int nErrorCode)
{
	
	CString strAddr;
	CString strPort;
	UINT uPort;
	CBuffer bufRecv(2000);

	int nRecvLen=m_RecvSocket->ReceiveFrom(bufRecv.GetBufP(),bufRecv.GetBufLen(),strAddr,uPort);
	bufRecv.SetDataLen(nRecvLen);
	
	if(bufRecv.GetDataLen()>PACKAGE_HEAD_LEN)
	{
		bufRecv.CutHeadData(PACKAGE_HEAD_LEN);
	}
	
	CPackageParser* pPackageParser = new CPackageParser();
	CString strSvrName;

	if(pPackageParser->Load(bufRecv))
	{
		int nIndex=0;
		
		CPackageParser::RECORD_HANDLE hRec = pPackageParser->EnumRecord(nIndex);
		strSvrName=pPackageParser->GetFieldValue(hRec,_T("SvrName"));
		strPort=pPackageParser->GetFieldValue(hRec,_T("Port"));

		CTime ct= CTime::GetTickCount();
	
		HOST_INFO *pHostInfo=new HOST_INFO;
		pHostInfo->strAddr=strAddr;
		pHostInfo->strSvrName=strSvrName;
		pHostInfo->TimeStamp=ct.GetTime();

		if(strPort.Find(_T("0x"))==0)
			_stscanf_s((LPCTSTR)strPort,_T("%x"),&pHostInfo->uPort);
		else
			_stscanf_s((LPCTSTR)strPort,_T("%d"),&pHostInfo->uPort);

		PostMessage(m_hParentWnd,UM_HOSTINFO,(WPARAM)pHostInfo,NULL);
	}
}

void CBroadCastThread::OnSocketSend(CClientSocket* pSrcSocket,int nErrorCode)
{
}

void CBroadCastThread::OnSocketOutOfBandData(CClientSocket* pSrcSocket,int nErrorCode)
{
}

void CBroadCastThread::OnSocketClose(CClientSocket* pSrcSocket,int nErrorCode)
{
}

void CBroadCastThread::SetHwnd(HWND hWnd)
{
	m_hParentWnd=hWnd;
}
