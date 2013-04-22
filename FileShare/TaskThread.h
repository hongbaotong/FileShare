#pragma once


// CTaskThread

class CTaskThread : public CWinThread
{
	DECLARE_DYNCREATE(CTaskThread)

public:
	CTaskThread();           // 动态创建所使用的受保护的构造函数
	virtual ~CTaskThread();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnGetTask(WPARAM wParam,LPARAM lParam);
public:
	BOOL GetShareList(CBuffer &bufShareList);
	/*BOOL IsIdle(void);*/
private:
	BOOL m_bIsIdle;
	virtual int Run();
	void SendSubData(CString strFileName,LONGLONG lOffset,CString strCliAddr);
	void SendFileInfo(CString strFileName,CString strCliAddr);
};


