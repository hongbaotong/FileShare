#include "StdAfx.h"
#include "TaskItem.h"

CTaskItem::CTaskItem(void)
{
}

CTaskItem::~CTaskItem(void)
{
}

// ����������
void CTaskItem::SetTaskName(CString strTaskName)
{
	m_strTaskName=strTaskName;
}

//�����ܳ���
void CTaskItem::SetTotalLen(ULONGLONG uTotalLen)
{
	m_uTotalLen=uTotalLen;
}

//���½�����
void CTaskItem::UpdateProgress(ULONGLONG uRecv)
{
}

BOOL CTaskItem::Attach(HWND hTaskList)
{
	return m_TaskCtrl.Attach(hTaskList);
}
