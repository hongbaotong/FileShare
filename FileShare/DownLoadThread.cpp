// DownLoadThread.cpp : 实现文件
//

#include "stdafx.h"
#include "FileShare.h"
#include "DownLoadThread.h"


// CDownLoadThread

IMPLEMENT_DYNCREATE(CDownLoadThread, CWinThread)

CDownLoadThread::CDownLoadThread()
{
	//m_RecvSocket=new CSocket();
}

//CDownLoadThread::CDownLoadThread(CSocket *srcSocket,ULONGLONG uFileLen)
//{
//	m_RecvSocket=srcSocket;
//	m_uFileLen=uFileLen;
//	RecvFile();
//}

CDownLoadThread::~CDownLoadThread()
{
	m_pTaskList.Detach();
}

BOOL CDownLoadThread::InitInstance()
{
	// TODO: 在此执行任意逐线程初始化
	CoInitialize(NULL);
	if(AfxSocketInit())
	{
		m_RecvSocket=new CSocket();
		if(m_RecvSocket->Create(0,SOCK_STREAM))
		{
			m_pInPackage=new CPackageParser();
			m_pOutPackage=new CPackageParser();
			//m_RecvSocket->SetRecvTimeOut(5000);

			//m_pTaskList=new CListCtrl();
			m_pTaskList.Attach(GetDlgItem(GetSvrApp()->GetMainWnd()->m_hWnd,IDC_TASK_LIST));
		}
	}
	m_nCliSvrState=CLI_SVR_UNCON;

	return TRUE;
}

int CDownLoadThread::ExitInstance()
{
	// TODO: 在此执行任意逐线程清理
	if(!m_RecvSocket!=NULL)
	{
		m_RecvSocket->Close();
		SAFE_DELETE(m_RecvSocket);
		//m_RecvSocket=NULL;
	}
	SAFE_DELETE(m_pInPackage);
	SAFE_DELETE(m_pOutPackage);
	
	return CWinThread::ExitInstance();
}

void CDownLoadThread::InitMember(CString strSvrAddr, UINT uPort,CString strFileName)
{
	m_strSvrAddr=strSvrAddr;
	m_uPort=uPort;
	m_strFileName=strFileName;
}

void CDownLoadThread::CreateTask()
{
	//控件初始化操作
	//int itemCount=m_pTaskList.GetItemCount();
	//m_pTaskList.InsertItem(itemCount,_T(""));
	//m_pTaskList.SetItemText(itemCount,0,m_strFileName);
	//CString strSize;
	//strSize.Format(_T("%u"),m_uFileLen);
	//m_pTaskList.SetItemText(itemCount,1,strSize);
	//m_pTaskList.SetItemText(itemCount,2,_T("0.00%"));

	m_pTaskList.SetProgressColumn(2);
	m_pTaskList.SetTaskName(m_strFileName);
	
	m_pTaskList.AddItem();
	
	
	

	CString strPathName=_T("D:\\DownloadTest\\");
	strPathName+=m_strFileName;
	m_DstFile.Open(strPathName,CFile::modeCreate|CFile::modeWrite|CFile::typeBinary);
	

	m_uTotalRecvLen=0;

	int nTry=0;
CONNECT_SVR:
	nTry++;
	ConnSvr();
REQUEST:
	if(CLI_SVR_CON==m_nCliSvrState)
	{
		if(m_pOutPackage!=NULL)
		{
			if(m_pOutPackage->CreateNewPackage())
			{
				m_pOutPackage->SetPackageProc(_T("GetFileInfo"),_T("Request"));
				CPackageParser::RECORD_HANDLE hRec = m_pOutPackage->CreateNewRecord();
				m_pOutPackage->SetFieldValue(hRec,_T("FileName"),m_strFileName);
			}
		}
		
		if(SendRequest())
		{
			if(RecvResponse())
			{
				SetOffset(m_uTotalRecvLen);
				RecvData();
				m_DstFile.Close();
				PostThreadMessage(WM_QUIT,NULL,NULL);
			}
		}
	}
	else if(m_nCliSvrState==CLI_SVR_UNCON && nTry<3)
	{
		goto CONNECT_SVR;
	}
	else
	{
		AfxMessageBox(_T("无法连接"));
	}
}

void CDownLoadThread::ConnSvr(void)
{
	if(CLI_SVR_UNCON==m_nCliSvrState)
	{
		m_RecvSocket->Close();
		if(m_RecvSocket->Create(0,SOCK_STREAM))
		{
			if(m_RecvSocket->Connect(m_strSvrAddr,m_uPort))
			{
				m_nCliSvrState=CLI_SVR_CON;
			}
		}
	}
}

BOOL CDownLoadThread::SendRequest(void)
{
	BOOL res=FALSE;

	CBuffer bufToSend;

	if(m_pOutPackage->SaveToBuf(bufToSend))
	{
		CBuffer *pSendData=new CBuffer();
		if(pSendData!=NULL)
		{
			if(pSendData->Alloc(bufToSend.GetDataLen()+PACKAGE_HEAD_LEN))
			{
				DWORD *pPackageEntityLen=(DWORD*)pSendData->GetBufP();
				*pPackageEntityLen=bufToSend.GetDataLen();
				pSendData->SetDataLen(PACKAGE_HEAD_LEN);
				pSendData->AppendData(bufToSend);

				if(m_RecvSocket->Send(pSendData->GetBufP(),pSendData->GetDataLen())!=SOCKET_ERROR)
				{
					res=TRUE;
				}
				else
				{
					m_nCliSvrState=CLI_SVR_UNCON;
				}
			}
			SAFE_DELETE(pSendData);
		}
	}
	
	return res;
}

BOOL CDownLoadThread::RecvResponse(void)
{
	BOOL res=FALSE;
	
	CBuffer bufData(64*1024);
	CBuffer bufRecv(8*1024);

	LONG nNeedRecv = PACKAGE_HEAD_LEN;
	while(bufData.GetDataLen()<nNeedRecv)
	{
		int nRecvLen=m_RecvSocket->Receive(bufRecv.GetBufP(),bufRecv.GetBufLen());
		if(nRecvLen==SOCKET_ERROR)
		{
			m_nCliSvrState=CLI_SVR_UNCON;
			break;
		}

		bufRecv.SetDataLen(nRecvLen);

		bufData.AppendData(bufRecv);
		
		if(bufData.GetDataLen()>=PACKAGE_HEAD_LEN)
		{
			nNeedRecv = *((int*)bufData.GetBufP())+PACKAGE_HEAD_LEN;
		}
	}
	
	//if(bufData.GetDataLen()>nNeedRecv)
	//{
	//	int nExtraLen=bufData.GetDataLen()-nNeedRecv;

	//	CBuffer *pNextMissonHead=new CBuffer(nExtraLen);
	//	if(pNextMissonHead!=NULL)
	//	{
	//		pNextMissonHead->SetData(bufData.GetBufP()+bufData.GetDataLen()-nExtraLen,nExtraLen);
	//		m_queRecv.Add(pNextMissonHead);
	//	}
	//	bufData.CutTailData(nExtraLen);
	//}

	bufData.CutHeadData(PACKAGE_HEAD_LEN);


	//回应数据处理
	if(m_pInPackage->Load(bufData))
	{
		CString strMainProc;
		CString strSubProc;
		strMainProc=m_pInPackage->GetPackageProc(strSubProc);
		if(strSubProc==_T("Response"))
		{
			if(strMainProc==_T("GetFileInfo"))
			{
				int nIndex=0;
				CPackageParser::RECORD_HANDLE hRec=m_pInPackage->EnumRecord(nIndex);
				if(nIndex!=-1)
				{
					m_uFileLen=CStringToUINT(m_pInPackage->GetFieldValue(hRec,_T("Size")));
					m_pTaskList.SetTotalLen(m_uFileLen);
					m_strFileName=m_pInPackage->GetFieldValue(hRec,_T("FileName"));
				}
			}
		}
		res=TRUE;
	}

	return res;
}

//BOOL CDownLoadThread::RequestServer()
//{
//	BOOL res=FALSE;
//
//	int nTry=0;
//
//CONNECT_SVR:
//
//	++nTry;
//	ConnSvr();
//
//REQUEST:
//	if(CLI_SVR_CON==m_nCliSvrState)
//	{
//		if(SendRequest())
//		{
//			if(RecvResponse())
//			{
//				res=TRUE;
//			}
//			else if(m_nCliSvrState==CLI_SVR_UNCON && nTry<3)
//			{
//				goto CONNECT_SVR;
//			}
//			else
//			{
//				AfxMessageBox(_T("解析错误!"));
//			}
//		}
//		else if(m_nCliSvrState==CLI_SVR_UNCON && nTry<3)
//		{
//			goto CONNECT_SVR;
//		}
//		else
//		{
//			AfxMessageBox(_T("无法连接2"));
//		}
//	}
//	return res;
//}

void CDownLoadThread::SetOffset(UINT uOffset)
{
	if(m_RecvSocket!=NULL)
	{
		CString strOffset;
		CString strRequSize;
		strOffset.Format(_T("%u"),uOffset);
		//strRequSize.Format(_T("%u"),uSize);

		CBuffer bufToSend;

		if(m_pOutPackage!=NULL)
		{
			if(m_pOutPackage->CreateNewPackage())
			{
				m_pOutPackage->SetPackageProc(_T("GetSubData"),_T("Request"));
				CPackageParser::RECORD_HANDLE hRec = m_pOutPackage->CreateNewRecord();
				m_pOutPackage->SetFieldValue(hRec,_T("FileName"),m_strFileName);
				m_pOutPackage->SetFieldValue(hRec,_T("Offset"),strOffset);
				//m_pOutPackage->SetFieldValue(hRec,_T("Size"),strRequSize);
			}
		}
		SendRequest();
	}
}


void CDownLoadThread::RecvData()
{
	//获取Socket内部接收缓冲区长度
	int nSocketRecvBufLen = 0;
	int nSocketOptLen = sizeof(nSocketRecvBufLen);
	m_RecvSocket->GetSockOpt(SO_RCVBUF,&nSocketRecvBufLen,&nSocketOptLen);

	float fPercent;
	CString strPercent;
	while(m_uTotalRecvLen<m_uFileLen)
	{
		CBuffer *pBufRecv=new CBuffer(nSocketRecvBufLen);

		if(pBufRecv!=NULL)
		{
			int nRecvLen=m_RecvSocket->Receive(pBufRecv->GetBufP(),pBufRecv->GetBufLen());

			if(nRecvLen!=SOCKET_ERROR && nRecvLen>0)
			{
				pBufRecv->SetDataLen(nRecvLen);

				m_DstFile.Write(pBufRecv->GetBufP(),pBufRecv->GetDataLen());
				m_uTotalRecvLen+=pBufRecv->GetDataLen();
				SAFE_DELETE(pBufRecv);
			}
			else
			{
				SAFE_DELETE(pBufRecv);
				continue;
			}
		}
		else
		{
			continue;
		}

		/*fPercent=((float)m_uTotalRecvLen/m_uFileLen)*100;
		strPercent.Format(_T("%4.2f%%"),fPercent);
		m_pTaskList.SetItemText(0,2,strPercent);*/
		m_pTaskList.UpdateProgress(m_uTotalRecvLen);
	}
}


BEGIN_MESSAGE_MAP(CDownLoadThread, CWinThread)
	ON_THREAD_MESSAGE(UM_DOWN_FILE,OnDownFile)
END_MESSAGE_MAP()


// CDownLoadThread 消息处理程序
void CDownLoadThread::OnDownFile(WPARAM wParam,LPARAM lParam)
{
	m_nTaskType=(int)wParam;
	
	switch(m_nTaskType)
	{
	case NEW_TASK:
		CreateTask();
		break;
	case CONTINUE_TASK:
		break;
	}
}

