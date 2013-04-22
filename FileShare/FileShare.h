// FileShare.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号
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
//自定义消息
#define WM_NI_TASK WM_USER+1
#define UM_HOSTINFO WM_USER+2
#define UM_GET_TASK WM_USER+3
#define UM_COMMIT_TASK WM_USER+4
#define UM_DOWN_FILE WM_USER+5

//连接状态
#define CLI_SVR_UNCON	0
#define CLI_SVR_CON		1

// CFileShareApp:
// 有关此类的实现，请参阅 FileShare.cpp
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

// 重写
public:
	virtual BOOL InitInstance();

// 实现

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

