#pragma once


// CTaskListCtrl
//���н������Ϳ�ʼ��ͣ��ť���б�ؼ�

class CTaskListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CTaskListCtrl)

public:
	CTaskListCtrl();
	virtual ~CTaskListCtrl();

protected:
	DECLARE_MESSAGE_MAP()
public:
	void AddItem(void);
	// ���ý�������ʾ����λ��
	void SetProgressColumn(int ColNum);
private:
	int m_ProgressColumn;
	CProgressCtrl* m_Progress;
	int m_nItemCount;
	//CArray<CProgressCtrl*> m_ProgressList;
public:
	// ����������
	void SetTaskName(CString strTaskName);
public:
	//�����ܳ���
	void SetTotalLen(ULONGLONG uTotalLen);
public:
	//���½�����
	void UpdateProgress(ULONGLONG uRecv);
private:
	CString m_strTaskName;
	ULONGLONG m_uTotalLen;
	afx_msg void OnPaint();
};