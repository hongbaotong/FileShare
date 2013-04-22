#pragma once
#include "FileShare.h"

// CDlgAddFileItemOption 对话框

#define UNIFOLDER_PIG			0x1
#define FOLDER_RECURSIVEG_PIG	0x2


class CDlgAddFileItemOption : public CDialog
{
	DECLARE_DYNAMIC(CDlgAddFileItemOption)

public:
	CDlgAddFileItemOption(int& nOption,const CString& strFolderName,CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgAddFileItemOption();

// 对话框数据
	enum { IDD = IDD_ADDFILEITEM_OPTION };

public: 
	static bool ShowAndGetOption(int& nOption,const CString& strFolderName);

protected:
	int& m_nOption;
	CString m_strFolderName;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
public:
	afx_msg void OnBnClickedCheckUniFolder();
};
