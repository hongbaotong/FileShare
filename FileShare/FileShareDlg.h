// FileShareDlg.h : 头文件
//

#pragma once
#include "FileShareList.h"
#include "HostShareList.h"
//#include "TaskListCtrl.h"


// CFileShareDlg 对话框
class CFileShareDlg : public CDialog
{
// 构造
public:
	CFileShareDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_FILESHARE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
private:
	CTabCtrl* m_pTab;
	CListCtrl m_pTaskList;
private:
	CFileShareList m_FileShareList;
	CHostShareList m_HostShareList;
	afx_msg void OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult);
private:
	//CNiThread m_SocketThread;
	afx_msg void OnBnClickedBtnSetsvrname();
protected:
	virtual void OnOK();
};
