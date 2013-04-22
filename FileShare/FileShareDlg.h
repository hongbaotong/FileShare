// FileShareDlg.h : ͷ�ļ�
//

#pragma once
#include "FileShareList.h"
#include "HostShareList.h"
//#include "TaskListCtrl.h"


// CFileShareDlg �Ի���
class CFileShareDlg : public CDialog
{
// ����
public:
	CFileShareDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_FILESHARE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
