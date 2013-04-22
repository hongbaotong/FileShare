#include "StdAfx.h"
#include "ClientTask.h"
#include "DownLoadThread.h"


CClientTask::CClientTask(void)
: m_nCliSvrState(0)
, m_uFileLen(0)
{
	InitMember(NULL,NULL);
}

CClientTask::CClientTask(HOST_INFO *pHostInfo)
{
	InitMember(pHostInfo->strAddr,pHostInfo->uPort);
}

CClientTask::CClientTask(CString strSvrAddr,UINT uPort)
{
	InitMember(strSvrAddr,uPort);
}

void CClientTask::InitMember(CString strSvrAddr, UINT uPort)
{
	CoInitialize(NULL);
	if(AfxSocketInit())
	{
		m_ConnSocket=new CSocket();
		if(m_ConnSocket->Create(0,SOCK_STREAM))
		{
			m_pInPackage=new CPackageParser();
			m_pOutPackage=new CPackageParser();
		}
	}
	m_nCliSvrState=CLI_SVR_UNCON;
	m_strSvrAddr=strSvrAddr;
	m_uPort=uPort;
}


CClientTask::~CClientTask(void)
{
	if(!m_ConnSocket!=NULL)
	{
		m_ConnSocket->Close();
		delete m_ConnSocket;
		m_ConnSocket=NULL;
	}
	SAFE_DELETE(m_pInPackage);
	SAFE_DELETE(m_pOutPackage);
}

void CClientTask::ConnSvr(void)
{
	if(CLI_SVR_UNCON==m_nCliSvrState)
	{
		m_ConnSocket->Close();
		if(m_ConnSocket->Create(0,SOCK_STREAM))
		{
			if(m_ConnSocket->Connect(m_strSvrAddr,m_uPort))
			{
				m_nCliSvrState=CLI_SVR_CON;
			}
		}
	}
}

BOOL CClientTask::SendRequest()
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

				if(m_ConnSocket->Send(pSendData->GetBufP(),pSendData->GetDataLen())!=SOCKET_ERROR)
				{
					res=TRUE;
				}
				else
				{
					m_nCliSvrState=CLI_SVR_UNCON;
				}
			}

		}
	}
	
	return res;
}

BOOL CClientTask::RecvResponse(void)
{
	BOOL res=FALSE;
	
	CBuffer bufData(64*1024);
	CBuffer bufRecv(8*1024);

	LONG nNeedRecv = PACKAGE_HEAD_LEN;
	while(bufData.GetDataLen()<nNeedRecv)
	{
		int nRecvLen=m_ConnSocket->Receive(bufRecv.GetBufP(),bufRecv.GetBufLen());
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
	
	bufData.CutHeadData(PACKAGE_HEAD_LEN);

	//回应数据处理
	if(m_pInPackage->Load(bufData))
	{
		CString strMainProc;
		CString strSubProc;
		strMainProc=m_pInPackage->GetPackageProc(strSubProc);
		if(strSubProc==_T("Response"))
		{
			if(strMainProc==_T("GetShareList"))
			{
				HandleResponse(RCV_SVR_SHARELIST);
			}
		}
		res=TRUE;
	}

	return res;
}

BOOL CClientTask::RequestServer()
{
	BOOL res=FALSE;

	int nTry=0;

CONNECT_SVR:

	++nTry;
	ConnSvr();

REQUEST:
	if(CLI_SVR_CON==m_nCliSvrState)
	{
		if(SendRequest())
		{
			if(RecvResponse())
			{
				res=TRUE;
			}
			else if(m_nCliSvrState==CLI_SVR_UNCON && nTry<3)
			{
				goto CONNECT_SVR;
			}
			else
			{
				AfxMessageBox(_T("无法连接到对方主机"));
			}
		}
		else if(m_nCliSvrState==CLI_SVR_UNCON && nTry<3)
		{
			goto CONNECT_SVR;
		}
		else
		{
			AfxMessageBox(_T("无法连接到对方主机"));
		}
	}
	return res;
}

//处理回应
BOOL CClientTask::HandleResponse(int nRspType)
{
	BOOL res=FALSE;
	switch(nRspType)
	{
	case RCV_SVR_SHARELIST:
		{
			FILE_INFO pFileInfo;
			int nIndex=0;
			while(TRUE)
			{
				CPackageParser::RECORD_HANDLE hRec=m_pInPackage->EnumRecord(nIndex);
				if(nIndex!=-1)
				{
					pFileInfo.strFileName=m_pInPackage->GetFieldValue(hRec,_T("ShareName"));
					pFileInfo.strFileSize=m_pInPackage->GetFieldValue(hRec,_T("Size"));
					pFileInfo.strFileMd5=m_pInPackage->GetFieldValue(hRec,_T("Md5"));
					m_arrFileList.Add(pFileInfo);
				}
				else
				{
					break;
				}
			}
			res=TRUE;
		}
		break;
	}
	return res;
}

void CClientTask::GetShareList(CArray<FILE_INFO> &arrFileList)
{
	if(m_pOutPackage!=NULL)
	{
		if(m_pOutPackage->CreateNewPackage())
		{
			m_pOutPackage->SetPackageProc(_T("GetShareList"),_T("Request"));

			if(RequestServer())
			{
				for(int i=0;i<m_arrFileList.GetCount();i++)
				{
					arrFileList.Add(m_arrFileList.GetAt(i));
				}
			}
		}
	}
}

void CClientTask::GetFile(CString strFileName)
{
	CDownLoadThread *DwnFileThread=new CDownLoadThread();
	if(DwnFileThread->CreateThread())
	{
		DwnFileThread->InitMember(m_strSvrAddr,m_uPort,strFileName);
		DwnFileThread->PostThreadMessageW(UM_DOWN_FILE,NEW_TASK,NULL);
	}
	
}
