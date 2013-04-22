#pragma once
#include "FileShare.h"

// CDlgAddFileItemOption �Ի���

#define UNIFOLDER_PIG			0x1
#define FOLDER_RECURSIVEG_PIG	0x2


class CDlgAddFileItemOption : public CDialog
{
	DECLARE_DYNAMIC(CDlgAddFileItemOption)

public:
	CDlgAddFileItemOption(int& nOption,const CString& strFolderName,CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgAddFileItemOption();

// �Ի�������
	enum { IDD = IDD_ADDFILEITEM_OPTION };

public: 
	static bool ShowAndGetOption(int& nOption,const CString& strFolderName);

protected:
	int& m_nOption;
	CString m_strFolderName;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
public:
	afx_msg void OnBnClickedCheckUniFolder();
};
