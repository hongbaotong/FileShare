// TaskThread.cpp : 实现文件
//

#include "stdafx.h"
#include "FileShare.h"
#include "TaskThread.h"


// CTaskThread

IMPLEMENT_DYNCREATE(CTaskThread, CWinThread)

CTaskThread::CTaskThread()
{
}

CTaskThread::~CTaskThread()
{
}

BOOL CTaskThread::InitInstance()
{
	// TODO: 在此执行任意逐线程初始化
	m_bIsIdle=TRUE;

	return TRUE;
}

int CTaskThread::ExitInstance()
{
	// TODO: 在此执行任意逐线程清理
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CTaskThread, CWinThread)
	ON_THREAD_MESSAGE(UM_GET_TASK,OnGetTask)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////

BOOL CTaskThread::GetShareList(CBuffer &bufShareList)
{
	BOOL res=FALSE;
	CArray<FILE_INFO> arrItemList;
	CPackageParser* pItemListPackage=new CPackageParser();

	if(pItemListPackage!=NULL)
	{
		if(pItemListPackage->CreateNewPackage())
		{
			pItemListPackage->SetPackageProc(_T("GetShareList"),_T("Response"));
			
			if(GetSvrApp()->m_ShareInfo->GetItemList(arrItemList))
			{
				for(int i=0;i<arrItemList.GetCount();i++)
				{
					CPackageParser::RECORD_HANDLE hRec=pItemListPackage->CreateNewRecord();
					pItemListPackage->SetFieldValue(hRec,_T("ShareName"),arrItemList.GetAt(i).strFileName);
					pItemListPackage->SetFieldValue(hRec,_T("Size"),arrItemList.GetAt(i).strFileSize);
					pItemListPackage->SetFieldValue(hRec,_T("Md5"),arrItemList.GetAt(i).strFileMd5);
				}
				if(pItemListPackage->SaveToBuf(bufShareList))
				{
					res=TRUE;
				}
			}
		}
	}
	return res;
}

//BOOL CTaskThread::IsIdle(void)
//{
//	return m_bIsIdle;
//}

// CTaskThread 消息处理程序

void CTaskThread::OnGetTask(WPARAM wParam,LPARAM lParam)
{

	TASK_INFO_TYPE *pTaskInfo=(TASK_INFO_TYPE *)wParam;

	if(pTaskInfo!=NULL)
	{
		CBuffer bufShareList;
		CPackageParser* pPackageParser = new CPackageParser();

		if(pPackageParser!=NULL)
		{
			if(pPackageParser->Load(*pTaskInfo->pDataToProc))
			{
				CString strMainProc;
				CString strSubProc;

				strMainProc=pPackageParser->GetPackageProc(strSubProc);
				if(strSubProc==_T("Request"))
				{
					if(strMainProc==_T("GetShareList"))
					{
						if(GetShareList(bufShareList))
						{
							GetSvrApp()->SendData(bufShareList,pTaskInfo->strCliAddr);
						}
					}
					if(strMainProc==_T("GetFileInfo"))
					{
						int nIndex=0;
						CString strFileName;
						CPackageParser::RECORD_HANDLE hRec = pPackageParser->EnumRecord(nIndex);
						if(hRec!=NULL)
						{
							if(nIndex!=-1)
							{
								CString strFileName=pPackageParser->GetFieldValue(hRec,_T("FileName"));
								SendFileInfo(strFileName,pTaskInfo->strCliAddr);
							}
						}
					}
					if(strMainProc==_T("GetSubData"))
					{
						int nIndex=0;
						CString strFileName;
						CPackageParser::RECORD_HANDLE hRec = pPackageParser->EnumRecord(nIndex);
						if(hRec!=NULL)
						{
							if(nIndex!=-1)
							{
								CString strFileName=pPackageParser->GetFieldValue(hRec,_T("FileName"));
								UINT uOffset=CStringToUINT(pPackageParser->GetFieldValue(hRec,_T("Offset")));
								//UINT uSize=CStringToUINT(pPackageParser->GetFieldValue(hRec,_T("Size")));
								SendSubData(strFileName,uOffset,pTaskInfo->strCliAddr);
							}
						}
					}
				}
			}
		}
	}
}

int CTaskThread::Run()
{
	// TODO: 在此添加专用代码和/或调用基类
	ASSERT_VALID(this);
	_AFX_THREAD_STATE* pState = AfxGetThreadState();

	// for tracking the idle time state
	BOOL bIdle = TRUE;
	LONG lIdleCount = 0;

	for(;;)
	{
		

		

		// phase1: check to see if we can do idle work
		while (bIdle &&
			!::PeekMessage(&(pState->m_msgCur), NULL, NULL, NULL, PM_NOREMOVE))
		{
			//等待、获取任务
			DWORD dRet=MsgWaitForMultipleObjects(1,&(GetSvrApp()->m_TaskQueue.m_Semaphore->m_hObject),TRUE,1500,QS_ALLINPUT);
			
			TASK_INFO_TYPE *pTaskInfo=GetSvrApp()->GetTask();

			if(pTaskInfo!=NULL)
			{
				PostThreadMessage(UM_GET_TASK,(WPARAM)pTaskInfo,NULL);
			}

			// call OnIdle while in bIdle state
			if (!OnIdle(lIdleCount++))
				bIdle = FALSE; // assume "no idle" state
		}

		// phase2: pump messages while available
		do
		{
			// pump message, but quit on WM_QUIT
			if (!PumpMessage())
				return ExitInstance();

			// reset "no idle" state after pumping "normal" message
			//if (IsIdleMessage(&m_msgCur))
			if (IsIdleMessage(&(pState->m_msgCur)))
			{
				bIdle = TRUE;
				lIdleCount = 0;
			}

		} while (::PeekMessage(&(pState->m_msgCur), NULL, NULL, NULL, PM_NOREMOVE));
	}

	//return CWinThread::Run();
}

void CTaskThread::SendSubData(CString strFileName,LONGLONG lOffset,CString strCliAddr)
{
	CString strPath=GetSvrApp()->m_ShareInfo->GetFilePath(strFileName);
	CFile srcFile;
	if(!srcFile.Open(strPath,CFile::modeRead|CFile::typeBinary|CFile::shareDenyNone))
	{
		return;
	}

	//发送文件
	UINT uReadLen;
	UINT uSend=0;
	UINT uTotalLenToSnd=srcFile.GetLength()-lOffset;
	CBuffer bufRead(BLOCK_SIZE);

	srcFile.Seek(lOffset, CFile::begin);

	while(uSend<uTotalLenToSnd)
	{
		
		//uLen=uSize>DATA_LEN?DATA_LEN:uSize;

		WaitForSingleObject(GetSvrApp()->m_SendDataEvent.m_hObject,INFINITE);
		uReadLen=srcFile.Read(bufRead.GetBufP(),BLOCK_SIZE);

		if(uReadLen==0)
		{
			break;
		}
		bufRead.SetDataLen(uReadLen);
		GetSvrApp()->SendFile(bufRead,strCliAddr);
		
		uSend+=uReadLen;

		//len1-=uReadLen;

		//uSize-=uReadLen;
		GetSvrApp()->m_SendDataEvent.SetEvent();
		
	}

	srcFile.Close();

}

void CTaskThread::SendFileInfo(CString strFileName,CString strCliAddr)
{
	CString strPath=GetSvrApp()->m_ShareInfo->GetFilePath(strFileName);
	CFile srcFile;
	if(!srcFile.Open(strPath,CFile::modeRead|CFile::typeBinary))
	{
		DWORD dwErr=GetLastError();
		return;
	}
	
	CBuffer bufFileInfo;

	//获取文件长度
	ULONGLONG uFileLen=srcFile.GetLength();
	CString strFileLen;
	strFileLen.Format(_T("%u"),uFileLen);
	srcFile.Close();

	//建立文件信息报头
	CPackageParser* pFileInfoPackage = new CPackageParser();

	if(pFileInfoPackage!=NULL)
	{
		if(pFileInfoPackage->CreateNewPackage())
		{
			pFileInfoPackage->SetPackageProc(_T("GetFileInfo"),_T("Response"));

			CPackageParser::RECORD_HANDLE hRec=pFileInfoPackage->CreateNewRecord();
			pFileInfoPackage->SetFieldValue(hRec,_T("FileName"),strFileName);
			pFileInfoPackage->SetFieldValue(hRec,_T("Size"),strFileLen);

			if(pFileInfoPackage->SaveToBuf(bufFileInfo))
			{
				GetSvrApp()->SendData(bufFileInfo,strCliAddr);
			}
		}
	}
}
