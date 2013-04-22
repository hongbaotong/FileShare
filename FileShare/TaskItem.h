#pragma once
#include "TaskListCtrl.h"

class CTaskItem
{
public:
	CTaskItem(void);
public:
	~CTaskItem(void);
public:
	// ����������
	void SetTaskName(CString strTaskName);
public:
	//�����ܳ���
	void SetTotalLen(ULONGLONG uTotalLen);
public:
	//���½�����
	void UpdateProgress(ULONGLONG uRecv);
	BOOL Attach(HWND hTaskList);
private:
	CString m_strTaskName;
	ULONGLONG m_uTotalLen;
	CTaskListCtrl m_TaskCtrl;
};
