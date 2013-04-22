#include "StdAfx.h"
#include "TaskItem.h"

CTaskItem::CTaskItem(void)
{
}

CTaskItem::~CTaskItem(void)
{
}

// 设置任务名
void CTaskItem::SetTaskName(CString strTaskName)
{
	m_strTaskName=strTaskName;
}

//设置总长度
void CTaskItem::SetTotalLen(ULONGLONG uTotalLen)
{
	m_uTotalLen=uTotalLen;
}

//更新进度条
void CTaskItem::UpdateProgress(ULONGLONG uRecv)
{
}

BOOL CTaskItem::Attach(HWND hTaskList)
{
	return m_TaskCtrl.Attach(hTaskList);
}
