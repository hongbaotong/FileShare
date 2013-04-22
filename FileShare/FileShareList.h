#pragma once
#include "LibCommon/common.h"
#include "FileInfoProc.h"
#include "FolderDialog.h"

// CFileShareList �Ի���

class CFileShareList : public CDialog
{
	DECLARE_DYNAMIC(CFileShareList)

public:
	CFileShareList(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CFileShareList();
	virtual BOOL OnInitDialog( );

// �Ի�������
	enum { IDD = IDD_FILE_LIST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl* m_List;
public:
	afx_msg void OnBnClickedBtnAdd();
	afx_msg void OnBnClickedBtnDel();
	afx_msg void OnBnClickedBtnAddfolder();
public:
	afx_msg void OnBnClickedChkSelectall();
protected:
	CButton *m_pChkSelAll;
public:
	afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
};
