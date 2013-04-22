#pragma once
#include "afxsock.h"
#include "TaskListCtrl.h"
//#include "SvrCliSocket.h"

//任务类型
#define NEW_TASK 1 //新任务
#define CONTINUE_TASK 2 //断点续传


// CDownLoadThread

class CDownLoadThread : public CWinThread
{
	DECLARE_DYNCREATE(CDownLoadThread)

public:
	CDownLoadThread();           // 动态创建所使用的受保护的构造函数
	//CDownLoadThread(CSocket *srcSocket,ULONGLONG uFileLen);
	virtual ~CDownLoadThread();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnDownFile(WPARAM wParam,LPARAM lParam);
private:
	CSocket *m_RecvSocket;
	ULONGLONG m_uFileLen;
	ULONG m_uTotalRecvLen;
	CString m_strFileName;
	CString m_strSvrAddr;
	UINT m_uPort;
	int m_nTaskType;
	//CCopyArray<CBuffer*> m_queRecv;
	CTaskListCtrl m_pTaskList;
	CFile m_DstFile;
private:
	int m_nCliSvrState;// 表示客户端与服务器的连接状态
	CPackageParser* m_pInPackage;
	CPackageParser* m_pOutPackage;
public:
	void RecvData();
	void SetOffset(UINT uOffset);
	void InitMember(CString strSvrAddr, UINT uPort,CString strFileName);
public:
	void ConnSvr(void);
	BOOL SendRequest(void);
	BOOL RecvResponse(void);
	//BOOL RequestServer(void);
public:
	void CreateTask();
};


