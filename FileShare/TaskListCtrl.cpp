// TaskListCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include "FileShare.h"
#include "TaskListCtrl.h"


// CTaskListCtrl

IMPLEMENT_DYNAMIC(CTaskListCtrl, CListCtrl)

CTaskListCtrl::CTaskListCtrl()
{
	m_ProgressColumn=0;
	m_Progress=new CProgressCtrl();
}

CTaskListCtrl::~CTaskListCtrl()
{
}


BEGIN_MESSAGE_MAP(CTaskListCtrl, CListCtrl)
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CTaskListCtrl 消息处理程序



void CTaskListCtrl::AddItem(void)
{
	CString strTotalLen;
	strTotalLen.Format(_T("%u"),m_uTotalLen);

	m_nItemCount=this->GetItemCount();
	this->InsertItem(m_nItemCount,_T(""));
	this->SetItemText(m_nItemCount,0,m_strTaskName);
	this->SetItemText(m_nItemCount,1,strTotalLen);

	CRect rc;
	this->GetSubItemRect(m_nItemCount,m_ProgressColumn,LVIR_LABEL,rc);
	m_Progress->Create(WS_CHILD | WS_VISIBLE | PBS_SMOOTH,rc,this,1);
}

// 设置进度条显示的列位置
void CTaskListCtrl::SetProgressColumn(int ColNum)
{
	m_ProgressColumn=ColNum;
}

// 设置任务名
void CTaskListCtrl::SetTaskName(CString strTaskName)
{
	m_strTaskName=strTaskName;
}

//设置总长度
void CTaskListCtrl::SetTotalLen(ULONGLONG uTotalLen)
{
	m_uTotalLen=uTotalLen;
	m_Progress->SetRange32(0,uTotalLen);
}

//更新进度条
void CTaskListCtrl::UpdateProgress(ULONGLONG uRecv)
{
	m_Progress->SetPos(uRecv);
}

void CTaskListCtrl::OnPaint()
{
	//CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CListCtrl::OnPaint()
	CRect rc;
	this->GetSubItemRect(m_nItemCount,m_ProgressColumn,LVIR_LABEL,rc);
	m_Progress->MoveWindow(&rc);

	CListCtrl::OnPaint();
}
