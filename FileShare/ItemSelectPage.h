#pragma once

#include "ShellFolderTree.h"
#include "ShellFolderContentList.h"
#include "FileSelectList.h"

// CItemSelectPage �Ի���

class CItemSelectPage : public CDialog
{
	DECLARE_DYNAMIC(CItemSelectPage)

public:
	CItemSelectPage(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CItemSelectPage();

// �Ի�������
	enum { IDD = IDD_ITEMSELECTPAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
protected:
	CShellFolderTree m_ShellFolderTree;
	CShellFolderContentList m_ShellFolderContentList;
	CFileSelectList m_FileSelectList;
public:
	//afx_msg void OnDestroy();
	afx_msg LRESULT OnShellTreeSelectedChg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDbClickShellItem(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnBnClickedDelitemBut();
public:
	afx_msg void OnBnClickedClrlistBut();
};
