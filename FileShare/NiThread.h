#pragma once
#include "NotifySocket.h"
#include "LibCommon/common.h"

// CNiThread
//任务功能码
#define NITASK_SEND				1
#define NITASK_DENY_CLIENT		2
#define NITASK_PRE_SHUTDOWN		3

class CNiThread : public CWinThread,public ISocketEvent
{
	DECLARE_DYNCREATE(CNiThread)

public:
	CNiThread();           // 动态创建所使用的受保护的构造函数
	virtual ~CNiThread();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//virtual int Run( );
public:
	struct NI_TASK_PARAM_TYPE
	{
		
		CString strClientAddr;
		CBuffer* pSendData;

		NI_TASK_PARAM_TYPE()
		{
			pSendData = NULL;
		}
		~NI_TASK_PARAM_TYPE()
		{
			SAFE_DELETE(pSendData);
		}

	};
	void CommitNiTask(int nFunCode,NI_TASK_PARAM_TYPE* pTaskParam);
	void SendData(CBuffer& bufDataSnd,const CString& strClientAddr);
	void SendFile(CBuffer& bufDataSnd,const CString& strClientAddr);
	void PrepareShutdown();

protected:
	virtual void OnSocketAccept(CNotifySocket* pSrcSocket,int nErrorCode);
	virtual void OnSocketConnect(CNotifySocket* pSrcSocket,int nErrorCode);
	virtual void OnSocketClose(CNotifySocket* pSrcSocket,int nErrorCode);
	virtual void OnSocketSend(CNotifySocket* pSrcSocket,int nErrorCode);
	virtual void OnSocketReceive(CNotifySocket* pSrcSocket,int nErrorCode);
	virtual void OnSocketOutOfBandData(CNotifySocket* pSrcSocket,int nErrorCode);

protected:
	CNotifySocket* m_pListenSocket;
	struct ClientSocketInfoType
	{
		CNotifySocket* pClientSocket;
		CString strClientAddr;//用来缓存客户端地址,提高效率
		CCopyArray<CBuffer*> queSend;//等待发送数据队列
		CBuffer* pBufInSend;//正在发送过程中的数据
		DWORD nPackageNeedRecv;//本次任务需接收的数据长度
		CCopyArray<CBuffer*> queRecv;
		//BYTE bTaskWrite;//标识本次任务是否为写目录任务

		ClientSocketInfoType(ISocketEvent* pSocketEveReceiver)
		{
			pClientSocket = new CNotifySocket(pSocketEveReceiver);
			pBufInSend = NULL;
			nPackageNeedRecv = 0; 
		}

		~ClientSocketInfoType()
		{
			//关闭客户端Socket
			if(pClientSocket != NULL)
			{
				pClientSocket->Close();
				delete pClientSocket;
			}
			//释放接收及发送队列
			while(queSend.GetCount()>0)
			{
				delete queSend[0];
				queSend.RemoveAt(0);
			}

			while(queRecv.GetCount()>0)
			{
				delete queRecv[0];
				queRecv.RemoveAt(0);
			}
			SAFE_DELETE(pBufInSend);
		}

	};
	//CArray<ClientSocketInfoType> m_aryClientSocket;
	CMapStringToPtr m_mapAddr2ClientInfo;
	ClientSocketInfoType* FindClientInfoByAddr(CNotifySocket* pClientSocket);
	ClientSocketInfoType* FindClientInfoByAddr(const CString& strClientAddr);
	//int FindClientInfoByAddr(CNotifySocket* pClientSocket);
	//static int CompareClinetInfoByAddr(const ClientSocketInfoType& A,const ClientSocketInfoType& B);

	BOOL m_bPreShutdown;//预关闭标志
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnNiTask(WPARAM wParam,LPARAM lParam);
};


