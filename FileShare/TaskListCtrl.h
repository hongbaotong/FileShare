#pragma once


// CTaskListCtrl
//具有进度条和开始暂停按钮的列表控件

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
	// 设置进度条显示的列位置
	void SetProgressColumn(int ColNum);
private:
	int m_ProgressColumn;
	CProgressCtrl* m_Progress;
	int m_nItemCount;
	//CArray<CProgressCtrl*> m_ProgressList;
public:
	// 设置任务名
	void SetTaskName(CString strTaskName);
public:
	//设置总长度
	void SetTotalLen(ULONGLONG uTotalLen);
public:
	//更新进度条
	void UpdateProgress(ULONGLONG uRecv);
private:
	CString m_strTaskName;
	ULONGLONG m_uTotalLen;
	afx_msg void OnPaint();
};