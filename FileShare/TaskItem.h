#pragma once
#include "TaskListCtrl.h"

class CTaskItem
{
public:
	CTaskItem(void);
public:
	~CTaskItem(void);
public:
	// 设置任务名
	void SetTaskName(CString strTaskName);
public:
	//设置总长度
	void SetTotalLen(ULONGLONG uTotalLen);
public:
	//更新进度条
	void UpdateProgress(ULONGLONG uRecv);
	BOOL Attach(HWND hTaskList);
private:
	CString m_strTaskName;
	ULONGLONG m_uTotalLen;
	CTaskListCtrl m_TaskCtrl;
};
