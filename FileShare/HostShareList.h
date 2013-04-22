#pragma once
#include "BroadCastThread.h"
#include "ClientTask.h"

// CHostShareList 对话框

class CHostShareList : public CDialog
{
	DECLARE_DYNAMIC(CHostShareList)

public:
	CHostShareList(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CHostShareList();
	virtual BOOL OnInitDialog( );

// 对话框数据
	enum { IDD = IDD_HOST_LIST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg LRESULT OnGetHostInfo(WPARAM wParam,LPARAM lParam);
private:
	CListCtrl* m_List;
	CListCtrl* m_HostList;
	CBroadCastThread *m_BroadThread;
	CClientTask *m_ClientTask;
	CMapStringToPtr m_mapIpToInfo;
public:
	void InsertHostItem(HOST_INFO* pHostInfo);
public:
	void InsertFileItem(CString strName, CString strSize, CString strMd5);
public:
	void DelAllFileItem(void);
public:
	afx_msg void OnNMDblclkHostList(NMHDR *pNMHDR, LRESULT *pResult);
public:
	afx_msg void OnNMRclickFileList(NMHDR *pNMHDR, LRESULT *pResult);
public:
	void CheckOnline(void);
public:
	afx_msg void OnNMDblclkFileList(NMHDR *pNMHDR, LRESULT *pResult);
};
