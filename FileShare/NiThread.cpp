// NiThread.cpp : 实现文件
//

#include "stdafx.h"
#include "FileShare.h"
#include "NiThread.h"


struct PACKAGE_NET 
{
	DWORD dwRealDataLen;//有效数据长度，不包含数据包头部
	BYTE  bTaskWrite;//客户端发给服务器时设置该字段，表示任务的读写性质
};



// CNiThread

IMPLEMENT_DYNCREATE(CNiThread, CWinThread)

CNiThread::CNiThread()
{
}

CNiThread::~CNiThread()
{
}

BOOL CNiThread::InitInstance()
{
	BOOL res = FALSE;

	m_bPreShutdown = FALSE;

	m_mapAddr2ClientInfo.InitHashTable(10*1024,TRUE);

	if(AfxSocketInit())
	{
		m_pListenSocket = new CNotifySocket(this);
		if(m_pListenSocket != NULL)
			if(m_pListenSocket->Create(GetSvrApp()->m_ShareInfo->GetSvrPort(),SOCK_STREAM))
		if(m_pListenSocket->Listen(100))
		{
			res = TRUE;
		}
	}
	
	return res;
}

int CNiThread::ExitInstance()
{
	//关闭侦听Socket
	if(m_pListenSocket!=NULL)
	{
		m_pListenSocket->Close();
		delete m_pListenSocket;
		m_pListenSocket = NULL;
	}
	
	//释放客户端信息
	POSITION pos = m_mapAddr2ClientInfo.GetStartPosition();
	while(pos != NULL)
	{
		CString strPeerAddr;
		ClientSocketInfoType* pClientInfo = NULL;
		m_mapAddr2ClientInfo.GetNextAssoc(pos,strPeerAddr,(void*&)pClientInfo);
		SAFE_DELETE(pClientInfo);
	}
	m_mapAddr2ClientInfo.RemoveAll();

	AfxSocketTerm();

	return CWinThread::ExitInstance();
}

CNiThread::ClientSocketInfoType* CNiThread::FindClientInfoByAddr(const CString& strClientAddr)
{
	ClientSocketInfoType* res = NULL;

	ClientSocketInfoType* pClientInfo = NULL;
	if(m_mapAddr2ClientInfo.Lookup(strClientAddr,(void*&)pClientInfo))
		res = pClientInfo;

	return res;
}

CNiThread::ClientSocketInfoType* CNiThread::FindClientInfoByAddr(CNotifySocket* pClientSocket)
{
	ClientSocketInfoType* res = NULL;

//	if(pClientSocket != NULL)
	{
		ClientSocketInfoType* pClientInfo = NULL;
		CString strPeerAddr = pClientSocket->GetPeerAddrStr();
		if(strPeerAddr.GetLength()>0)
		if(m_mapAddr2ClientInfo.Lookup(strPeerAddr,(void*&)pClientInfo))
			res = pClientInfo;
	}
	return res;
}

void CNiThread::OnSocketAccept(CNotifySocket* pSrcSocket,int nErrorCode)
{
	//当进入预关闭流程时,拒绝外部请求
	if(m_bPreShutdown)
		return;

	if(pSrcSocket == m_pListenSocket)
	{
		ClientSocketInfoType* pClinetInfo = new ClientSocketInfoType(this);
		
		if(pClinetInfo!= NULL && pClinetInfo->pClientSocket!=NULL)
		{
			if(m_pListenSocket->Accept(*pClinetInfo->pClientSocket) && pClinetInfo->pClientSocket->GetPeerAddrStr().GetLength()>0)
			{
				pClinetInfo->strClientAddr = pClinetInfo->pClientSocket->GetPeerAddrStr();
				m_mapAddr2ClientInfo.SetAt(pClinetInfo->pClientSocket->GetPeerAddrStr(),pClinetInfo);

				return;
			}
		}
		SAFE_DELETE(pClinetInfo);
	}
}

void CNiThread::OnSocketConnect(CNotifySocket* pSrcSocket,int nErrorCode)
{
	if(nErrorCode!=0)
	{
		//如果连接出现问题，删除相关信息

		ClientSocketInfoType* pClientInfo = FindClientInfoByAddr(pSrcSocket);
		if(pClientInfo != NULL)
		{
			m_mapAddr2ClientInfo.RemoveKey(pSrcSocket->GetPeerAddrStr());
			delete pClientInfo;
		}

	}
}

void CNiThread::OnSocketClose(CNotifySocket* pSrcSocket,int nErrorCode)
{

	ClientSocketInfoType* pClientInfo = FindClientInfoByAddr(pSrcSocket);
	if(pClientInfo!=NULL)
	{
		//通知App注销用户
		//GetSvrApp()->UnRegClient(pClientInfo->strClientAddr);

		//删除客户端数据
		m_mapAddr2ClientInfo.RemoveKey(pSrcSocket->GetPeerAddrStr());
		delete pClientInfo;
	}
	
}

void CNiThread::OnSocketSend(CNotifySocket* pSrcSocket,int nErrorCode)
{

	ClientSocketInfoType* pClientInfo = FindClientInfoByAddr(pSrcSocket);
	if(pClientInfo!=NULL)
	{
		//如果发送等待队列中有数据
		while(pClientInfo->queSend.GetCount()>0)
		{
			int nSndLen=pSrcSocket->Send(pClientInfo->queSend[0]->GetBufP(),pClientInfo->queSend[0]->GetDataLen());
			DWORD dwErr = GetLastError();
			if(nSndLen != SOCKET_ERROR && dwErr != WSAEWOULDBLOCK)
			{
				SAFE_DELETE(pClientInfo->queSend[0]);
				pClientInfo->queSend.RemoveAt(0);
			}
			else
			{
				GetSvrApp()->m_SendDataEvent.ResetEvent();
				break;
			}
		}
		if (pClientInfo->queSend.GetCount()==0)
		{
			GetSvrApp()->m_SendDataEvent.SetEvent();
		}
	}
}

void CNiThread::OnSocketReceive(CNotifySocket* pSrcSocket,int nErrorCode)
{
	//当进入预关闭流程时,拒绝外部请求
	if(m_bPreShutdown)
		return;

	ClientSocketInfoType* pClientInfo  = FindClientInfoByAddr(pSrcSocket);
	if(pClientInfo!=NULL)
	{
		//获取Socket内部接收缓冲区长度
		int nSocketRecvBufLen = 0;
		int nSocketOptLen = sizeof(nSocketRecvBufLen);
		pSrcSocket->GetSockOpt(SO_RCVBUF,&nSocketRecvBufLen,&nSocketOptLen);

		//当资源不足时关闭对应Socket
		BOOL bCloseSocket = FALSE;

		while(TRUE)
		{
			CBuffer* pBufRecv;
			pBufRecv=new CBuffer(nSocketRecvBufLen);
			//pBufRecv->Alloc(nSocketRecvBufLen);
			if(pBufRecv!=NULL)
			{
				int nRecvLen = pSrcSocket->Receive(pBufRecv->GetBufP(),nSocketRecvBufLen);
				if(nRecvLen>0)
				{
					pBufRecv->SetDataLen(nRecvLen);
				}
				else
				{
					delete pBufRecv;
					break;
				}
			}
			else
			{
				bCloseSocket = TRUE;
				break;
			}

PROCESS_RECEIVE:
			//数据入队
			pClientInfo->queRecv.Add(pBufRecv);
			pBufRecv = NULL;

			//取缓存的数据总长度
			DWORD nSumDataLen = 0;
			for(int i=0;i<pClientInfo->queRecv.GetCount();++i)
			{
				nSumDataLen +=pClientInfo->queRecv[i]->GetDataLen();
			}

			if(pClientInfo->nPackageNeedRecv == 0)//在一个数据包接收任务的开始
			{

				//如果数据总长度超过包头长度
				if(nSumDataLen>=PACKAGE_HEAD_LEN)
				{
					//合成数据
					CBuffer* pBufCompose;
					pBufCompose=new CBuffer(nSumDataLen);
					//pBufCompose->Alloc(nSumDataLen);
					if(pBufCompose != NULL)
					{
						BYTE* pOffset = pBufCompose->GetBufP();
						//合成数据并清理包头缓存
						while(pClientInfo->queRecv.GetCount()>0)
						{
							memcpy(pOffset,pClientInfo->queRecv[0]->GetBufP(),pClientInfo->queRecv[0]->GetDataLen());
							pOffset += pClientInfo->queRecv[0]->GetDataLen();

							delete pClientInfo->queRecv[0];
							pClientInfo->queRecv.RemoveAt(0);
						}
						pBufCompose->SetDataLen(nSumDataLen);

						//获得数据包有效数据长度
						DWORD nPackageHead = *((int*)pBufCompose->GetBufP());

						//根据数据长度进行不同的处理
						if( nSumDataLen<nPackageHead+PACKAGE_HEAD_LEN)//如果总长度大于或等于头部长度且没超过需接收的长度，则还有多余的数据需要入接收队列
						{
							//设置待接收数据长度
							pClientInfo->nPackageNeedRecv = nPackageHead ;

							//剪除头部数据
							pBufCompose->CutHeadData(PACKAGE_HEAD_LEN);
							if(pBufCompose->GetDataLen()>0)
								pClientInfo->queRecv.Add(pBufCompose);
							else//接收数据长度刚好等于包头
								delete pBufCompose;
							
						}
						else //if(nSumDataLen>=nPackageHead+PACKAGE_HEAD_LEN)//数据一次接够的
						{
							int nExtraLen = nSumDataLen-nPackageHead-PACKAGE_HEAD_LEN;
							CBuffer *pNextMissonHead = NULL;
							//缓存下次任务数据
							if(nExtraLen>0)
							{
								pNextMissonHead=new CBuffer(nExtraLen);
								if(pNextMissonHead != NULL)
									pNextMissonHead->SetData(pBufCompose->GetBufP()+pBufCompose->GetDataLen()-nExtraLen,nExtraLen);
							}

							//剪除尾部下次任务数据
							pBufCompose->CutTailData(nExtraLen);
							//剪除头部数据
							pBufCompose->CutHeadData(PACKAGE_HEAD_LEN);

							//提交事务数据
							
							GetSvrApp()->CommitTask(pBufCompose,pClientInfo->strClientAddr);

							//处理下次任务
							pClientInfo->nPackageNeedRecv = 0;
							if(pNextMissonHead != NULL)
							{
								pBufRecv = pNextMissonHead;
								goto PROCESS_RECEIVE;
							}

						}

					}
					else//如果没有足够内存,则关闭对应Socket
					{
						bCloseSocket = TRUE;
						break;
					}
				}
				
			}
			else//后继接收任务
			{
				int nExtraLen = nSumDataLen - pClientInfo->nPackageNeedRecv;
				if(nExtraLen>=0)
				{

					//合成数据
					CBuffer* pBufCompose;
					pBufCompose=new CBuffer(nSumDataLen);
					//pBufCompose->Alloc(nSumDataLen);
					if(pBufCompose != NULL)
					{
						BYTE* pOffeset = pBufCompose->GetBufP();

						//合成数据并清理接收缓存队列
						while(pClientInfo->queRecv.GetCount()>0)
						{
							memcpy(pOffeset,pClientInfo->queRecv[0]->GetBufP(),pClientInfo->queRecv[0]->GetDataLen());
							pOffeset += pClientInfo->queRecv[0]->GetDataLen();

							delete pClientInfo->queRecv[0];
							pClientInfo->queRecv.RemoveAt(0);
						}
						pBufCompose->SetDataLen(nSumDataLen);


						//将超过本次任务长度的部分另外缓存
						CBuffer *pNextMissonHead = NULL;
						if(nExtraLen>0)
						{
							pNextMissonHead=new CBuffer(nExtraLen);
							if(pNextMissonHead != NULL)
								pNextMissonHead->SetData(pBufCompose->GetBufP()+pClientInfo->nPackageNeedRecv,nExtraLen);
						}
						//剪除下次任务数据
						pBufCompose->CutTailData(nExtraLen);


						//提交事务数据
						GetSvrApp()->CommitTask(pBufCompose,pClientInfo->strClientAddr);
						//GetSvrApp()->PostThreadMessageW(UM_COMMIT_TASK,(WPARAM)pBufCompose,(LPARAM)pClientInfo);


						//处理下次任务
						pClientInfo->nPackageNeedRecv = 0;
						if(pNextMissonHead != NULL)
						{
							pBufRecv = pNextMissonHead;
							goto PROCESS_RECEIVE;
						}

					}
					else//资源不够时,释放资源,关闭相应Socket
					{

						bCloseSocket = TRUE;
						break;
					}

				}
			}
			
		}
		//如果没有足够内存,则关闭对应Socket
		if(bCloseSocket)
		{
			m_mapAddr2ClientInfo.RemoveKey(pSrcSocket->GetPeerAddrStr());
			delete pClientInfo;
		}
	}
}

void CNiThread::OnSocketOutOfBandData(CNotifySocket* pSrcSocket,int nErrorCode)
{
}


void CNiThread::CommitNiTask(int nFunCode,NI_TASK_PARAM_TYPE* pTaskParam)
{
	PostThreadMessage(WM_NI_TASK,nFunCode,(LPARAM)pTaskParam);
}

void CNiThread::SendFile(CBuffer& bufDataSnd,const CString& strClientAddr)
{
	if(bufDataSnd.GetDataLen()<=0 || strClientAddr.GetLength()<=0)
		return;

	NI_TASK_PARAM_TYPE* pNiTask = new  NI_TASK_PARAM_TYPE;
	if(pNiTask != NULL)
	{
		//pNiTask->pSendData = &bufDataSnd;
		pNiTask->pSendData=new CBuffer(bufDataSnd.GetDataLen());
		pNiTask->pSendData->SetData(bufDataSnd);
		//填写发送地址
		pNiTask->strClientAddr = strClientAddr;

		//提交发送任务
		CommitNiTask(NITASK_SEND,pNiTask);
	}
}

void CNiThread::SendData(CBuffer& bufDataSnd,const CString& strClientAddr)
{
	if(bufDataSnd.GetDataLen()<=0 || strClientAddr.GetLength()<=0)
		return;

	NI_TASK_PARAM_TYPE* pNiTask = new  NI_TASK_PARAM_TYPE;
	if(pNiTask != NULL)
	{
		//pNiTask->pSendData = &bufDataSnd;
		pNiTask->pSendData = new CBuffer();
		if(pNiTask->pSendData!=NULL)
		{
			//加装数据包头
			if(pNiTask->pSendData->Alloc(bufDataSnd.GetDataLen()+PACKAGE_HEAD_LEN))
			{
				//填写数据包头部
				DWORD* pPackageEntityLen = (DWORD*)pNiTask->pSendData->GetBufP();
				*pPackageEntityLen=bufDataSnd.GetDataLen();
				pNiTask->pSendData->SetDataLen(PACKAGE_HEAD_LEN);
				pNiTask->pSendData->AppendData(bufDataSnd);

				//memcpy(pNiTask->pSendData,bufDataSnd.GetBufP(),bufDataSnd.GetDataLen());
				//pNiTask->pSendData->SetDataLen(bufDataSnd.GetDataLen()+PACKAGE_HEAD_LEN);
			}
		}
		//填写发送地址
		pNiTask->strClientAddr = strClientAddr;

		//提交发送任务
		CommitNiTask(NITASK_SEND,pNiTask);
	}
}

void CNiThread::PrepareShutdown()
{
	CommitNiTask(NITASK_PRE_SHUTDOWN,NULL);
}

BEGIN_MESSAGE_MAP(CNiThread, CWinThread)
	ON_THREAD_MESSAGE(WM_NI_TASK,OnNiTask)
END_MESSAGE_MAP()


// CNiThread 消息处理程序
void CNiThread::OnNiTask(WPARAM wParam,LPARAM lParam)
{
	NI_TASK_PARAM_TYPE* pParam = (NI_TASK_PARAM_TYPE*)lParam;
	switch(wParam)
	{
	case NITASK_SEND:
		{
			ClientSocketInfoType* pClientInfo= FindClientInfoByAddr(pParam->strClientAddr);
			if(pClientInfo != NULL)
			{
				if(pClientInfo->queSend.GetCount()==0)
				{
					int nSndLen = pClientInfo->pClientSocket->Send(pParam->pSendData->GetBufP(),pParam->pSendData->GetDataLen());

					//如果当前Socket忙，不能立即发送
					DWORD dwErr = GetLastError();
					if(nSndLen == SOCKET_ERROR || dwErr == WSAEWOULDBLOCK)
					{
						pClientInfo->queSend.Add(pParam->pSendData);
						pParam->pSendData = NULL;//防止被析构
					}
				}
				else
				{
					
					GetSvrApp()->m_SendDataEvent.ResetEvent();
					
					pClientInfo->queSend.Add(pParam->pSendData);
					pParam->pSendData = NULL;//防止被析构
				}
			}
		}
		break;
	case NITASK_DENY_CLIENT:
		break;
	case NITASK_PRE_SHUTDOWN://预备关闭
		m_bPreShutdown = TRUE;
	}

	SAFE_DELETE(pParam);
}