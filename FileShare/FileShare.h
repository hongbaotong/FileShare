// FileShare.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������
#include "NiThread.h"
#include "ShareInfo.h"
#include "PackageParser.h"
#include "TaskThread.h"
#include "TaskQueue.h"
#include <afxmt.h>

#ifndef FS_SHAREINFO 
#define FS_SHAREINFO _T("ShareInfo.xml")
#endif

#define PACKAGE_HEAD_LEN 32
#define TASK_THREAD_NUM 1
//�Զ�����Ϣ
#define WM_NI_TASK WM_USER+1
#define UM_HOSTINFO WM_USER+2
#define UM_GET_TASK WM_USER+3
#define UM_COMMIT_TASK WM_USER+4
#define UM_DOWN_FILE WM_USER+5

//����״̬
#define CLI_SVR_UNCON	0
#define CLI_SVR_CON		1

// CFileShareApp:
// �йش����ʵ�֣������ FileShare.cpp
//


struct HOST_INFO{
		CString strAddr;
		CString strSvrName;
		UINT uPort;
		time_t TimeStamp;
};


class CFileShareApp : public CWinApp
{
public:
	CFileShareApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
private:
	CNiThread *m_NiThread;
	CTaskThread *m_TaskThread;
public:
	CShareInfo* m_ShareInfo;
	CTaskQueue m_TaskQueue;
	CEvent m_SendDataEvent;
public:
	void CommitTask(CBuffer* pDataToProc, CString strClientAddr);
public:
	virtual int ExitInstance();
public:
	void SendData(CBuffer bufToSend, CString strCliAddr);
public:
	TASK_INFO_TYPE* GetTask(void);
public:
	void SendFile(CBuffer bufToSend, CString strCliAddr);
};

extern CFileShareApp theApp;

CFileShareApp* GetSvrApp();

