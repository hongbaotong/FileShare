// NiThread.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "FileShare.h"
#include "NiThread.h"


struct PACKAGE_NET 
{
	DWORD dwRealDataLen;//��Ч���ݳ��ȣ����������ݰ�ͷ��
	BYTE  bTaskWrite;//�ͻ��˷���������ʱ���ø��ֶΣ���ʾ����Ķ�д����
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
	//�ر�����Socket
	if(m_pListenSocket!=NULL)
	{
		m_pListenSocket->Close();
		delete m_pListenSocket;
		m_pListenSocket = NULL;
	}
	
	//�ͷſͻ�����Ϣ
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
	//������Ԥ�ر�����ʱ,�ܾ��ⲿ����
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
		//������ӳ������⣬ɾ�������Ϣ

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
		//֪ͨAppע���û�
		//GetSvrApp()->UnRegClient(pClientInfo->strClientAddr);

		//ɾ���ͻ�������
		m_mapAddr2ClientInfo.RemoveKey(pSrcSocket->GetPeerAddrStr());
		delete pClientInfo;
	}
	
}

void CNiThread::OnSocketSend(CNotifySocket* pSrcSocket,int nErrorCode)
{

	ClientSocketInfoType* pClientInfo = FindClientInfoByAddr(pSrcSocket);
	if(pClientInfo!=NULL)
	{
		//������͵ȴ�������������
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
	//������Ԥ�ر�����ʱ,�ܾ��ⲿ����
	if(m_bPreShutdown)
		return;

	ClientSocketInfoType* pClientInfo  = FindClientInfoByAddr(pSrcSocket);
	if(pClientInfo!=NULL)
	{
		//��ȡSocket�ڲ����ջ���������
		int nSocketRecvBufLen = 0;
		int nSocketOptLen = sizeof(nSocketRecvBufLen);
		pSrcSocket->GetSockOpt(SO_RCVBUF,&nSocketRecvBufLen,&nSocketOptLen);

		//����Դ����ʱ�رն�ӦSocket
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
			//�������
			pClientInfo->queRecv.Add(pBufRecv);
			pBufRecv = NULL;

			//ȡ����������ܳ���
			DWORD nSumDataLen = 0;
			for(int i=0;i<pClientInfo->queRecv.GetCount();++i)
			{
				nSumDataLen +=pClientInfo->queRecv[i]->GetDataLen();
			}

			if(pClientInfo->nPackageNeedRecv == 0)//��һ�����ݰ���������Ŀ�ʼ
			{

				//��������ܳ��ȳ�����ͷ����
				if(nSumDataLen>=PACKAGE_HEAD_LEN)
				{
					//�ϳ�����
					CBuffer* pBufCompose;
					pBufCompose=new CBuffer(nSumDataLen);
					//pBufCompose->Alloc(nSumDataLen);
					if(pBufCompose != NULL)
					{
						BYTE* pOffset = pBufCompose->GetBufP();
						//�ϳ����ݲ������ͷ����
						while(pClientInfo->queRecv.GetCount()>0)
						{
							memcpy(pOffset,pClientInfo->queRecv[0]->GetBufP(),pClientInfo->queRecv[0]->GetDataLen());
							pOffset += pClientInfo->queRecv[0]->GetDataLen();

							delete pClientInfo->queRecv[0];
							pClientInfo->queRecv.RemoveAt(0);
						}
						pBufCompose->SetDataLen(nSumDataLen);

						//������ݰ���Ч���ݳ���
						DWORD nPackageHead = *((int*)pBufCompose->GetBufP());

						//�������ݳ��Ƚ��в�ͬ�Ĵ���
						if( nSumDataLen<nPackageHead+PACKAGE_HEAD_LEN)//����ܳ��ȴ��ڻ����ͷ��������û��������յĳ��ȣ����ж����������Ҫ����ն���
						{
							//���ô��������ݳ���
							pClientInfo->nPackageNeedRecv = nPackageHead ;

							//����ͷ������
							pBufCompose->CutHeadData(PACKAGE_HEAD_LEN);
							if(pBufCompose->GetDataLen()>0)
								pClientInfo->queRecv.Add(pBufCompose);
							else//�������ݳ��ȸպõ��ڰ�ͷ
								delete pBufCompose;
							
						}
						else //if(nSumDataLen>=nPackageHead+PACKAGE_HEAD_LEN)//����һ�νӹ���
						{
							int nExtraLen = nSumDataLen-nPackageHead-PACKAGE_HEAD_LEN;
							CBuffer *pNextMissonHead = NULL;
							//�����´���������
							if(nExtraLen>0)
							{
								pNextMissonHead=new CBuffer(nExtraLen);
								if(pNextMissonHead != NULL)
									pNextMissonHead->SetData(pBufCompose->GetBufP()+pBufCompose->GetDataLen()-nExtraLen,nExtraLen);
							}

							//����β���´���������
							pBufCompose->CutTailData(nExtraLen);
							//����ͷ������
							pBufCompose->CutHeadData(PACKAGE_HEAD_LEN);

							//�ύ��������
							
							GetSvrApp()->CommitTask(pBufCompose,pClientInfo->strClientAddr);

							//�����´�����
							pClientInfo->nPackageNeedRecv = 0;
							if(pNextMissonHead != NULL)
							{
								pBufRecv = pNextMissonHead;
								goto PROCESS_RECEIVE;
							}

						}

					}
					else//���û���㹻�ڴ�,��رն�ӦSocket
					{
						bCloseSocket = TRUE;
						break;
					}
				}
				
			}
			else//��̽�������
			{
				int nExtraLen = nSumDataLen - pClientInfo->nPackageNeedRecv;
				if(nExtraLen>=0)
				{

					//�ϳ�����
					CBuffer* pBufCompose;
					pBufCompose=new CBuffer(nSumDataLen);
					//pBufCompose->Alloc(nSumDataLen);
					if(pBufCompose != NULL)
					{
						BYTE* pOffeset = pBufCompose->GetBufP();

						//�ϳ����ݲ�������ջ������
						while(pClientInfo->queRecv.GetCount()>0)
						{
							memcpy(pOffeset,pClientInfo->queRecv[0]->GetBufP(),pClientInfo->queRecv[0]->GetDataLen());
							pOffeset += pClientInfo->queRecv[0]->GetDataLen();

							delete pClientInfo->queRecv[0];
							pClientInfo->queRecv.RemoveAt(0);
						}
						pBufCompose->SetDataLen(nSumDataLen);


						//�������������񳤶ȵĲ������⻺��
						CBuffer *pNextMissonHead = NULL;
						if(nExtraLen>0)
						{
							pNextMissonHead=new CBuffer(nExtraLen);
							if(pNextMissonHead != NULL)
								pNextMissonHead->SetData(pBufCompose->GetBufP()+pClientInfo->nPackageNeedRecv,nExtraLen);
						}
						//�����´���������
						pBufCompose->CutTailData(nExtraLen);


						//�ύ��������
						GetSvrApp()->CommitTask(pBufCompose,pClientInfo->strClientAddr);
						//GetSvrApp()->PostThreadMessageW(UM_COMMIT_TASK,(WPARAM)pBufCompose,(LPARAM)pClientInfo);


						//�����´�����
						pClientInfo->nPackageNeedRecv = 0;
						if(pNextMissonHead != NULL)
						{
							pBufRecv = pNextMissonHead;
							goto PROCESS_RECEIVE;
						}

					}
					else//��Դ����ʱ,�ͷ���Դ,�ر���ӦSocket
					{

						bCloseSocket = TRUE;
						break;
					}

				}
			}
			
		}
		//���û���㹻�ڴ�,��رն�ӦSocket
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
		//��д���͵�ַ
		pNiTask->strClientAddr = strClientAddr;

		//�ύ��������
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
			//��װ���ݰ�ͷ
			if(pNiTask->pSendData->Alloc(bufDataSnd.GetDataLen()+PACKAGE_HEAD_LEN))
			{
				//��д���ݰ�ͷ��
				DWORD* pPackageEntityLen = (DWORD*)pNiTask->pSendData->GetBufP();
				*pPackageEntityLen=bufDataSnd.GetDataLen();
				pNiTask->pSendData->SetDataLen(PACKAGE_HEAD_LEN);
				pNiTask->pSendData->AppendData(bufDataSnd);

				//memcpy(pNiTask->pSendData,bufDataSnd.GetBufP(),bufDataSnd.GetDataLen());
				//pNiTask->pSendData->SetDataLen(bufDataSnd.GetDataLen()+PACKAGE_HEAD_LEN);
			}
		}
		//��д���͵�ַ
		pNiTask->strClientAddr = strClientAddr;

		//�ύ��������
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


// CNiThread ��Ϣ�������
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

					//�����ǰSocketæ��������������
					DWORD dwErr = GetLastError();
					if(nSndLen == SOCKET_ERROR || dwErr == WSAEWOULDBLOCK)
					{
						pClientInfo->queSend.Add(pParam->pSendData);
						pParam->pSendData = NULL;//��ֹ������
					}
				}
				else
				{
					
					GetSvrApp()->m_SendDataEvent.ResetEvent();
					
					pClientInfo->queSend.Add(pParam->pSendData);
					pParam->pSendData = NULL;//��ֹ������
				}
			}
		}
		break;
	case NITASK_DENY_CLIENT:
		break;
	case NITASK_PRE_SHUTDOWN://Ԥ���ر�
		m_bPreShutdown = TRUE;
	}

	SAFE_DELETE(pParam);
}