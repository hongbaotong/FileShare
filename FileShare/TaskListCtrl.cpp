// TaskListCtrl.cpp : ʵ���ļ�
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



// CTaskListCtrl ��Ϣ�������



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

// ���ý�������ʾ����λ��
void CTaskListCtrl::SetProgressColumn(int ColNum)
{
	m_ProgressColumn=ColNum;
}

// ����������
void CTaskListCtrl::SetTaskName(CString strTaskName)
{
	m_strTaskName=strTaskName;
}

//�����ܳ���
void CTaskListCtrl::SetTotalLen(ULONGLONG uTotalLen)
{
	m_uTotalLen=uTotalLen;
	m_Progress->SetRange32(0,uTotalLen);
}

//���½�����
void CTaskListCtrl::UpdateProgress(ULONGLONG uRecv)
{
	m_Progress->SetPos(uRecv);
}

void CTaskListCtrl::OnPaint()
{
	//CPaintDC dc(this); // device context for painting
	// TODO: �ڴ˴������Ϣ����������
	// ��Ϊ��ͼ��Ϣ���� CListCtrl::OnPaint()
	CRect rc;
	this->GetSubItemRect(m_nItemCount,m_ProgressColumn,LVIR_LABEL,rc);
	m_Progress->MoveWindow(&rc);

	CListCtrl::OnPaint();
}
