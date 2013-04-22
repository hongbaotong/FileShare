// DlgAddFileItemOption.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DlgAddFileItemOption.h"


// CDlgAddFileItemOption �Ի���

IMPLEMENT_DYNAMIC(CDlgAddFileItemOption, CDialog)

CDlgAddFileItemOption::CDlgAddFileItemOption(int& nOption,const CString& strFolderName,CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAddFileItemOption::IDD, pParent),m_nOption(nOption),m_strFolderName(strFolderName)
{
	m_nOption = FOLDER_RECURSIVEG_PIG;
}

CDlgAddFileItemOption::~CDlgAddFileItemOption()
{
}

void CDlgAddFileItemOption::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	CButton* pUniFolderChk = ((CButton*)GetDlgItem(IDC_CHECK_UNI_FOLDER));
	CButton* pRecursiveChk = ((CButton*)GetDlgItem(IDC_CHECK_RECUERSIVE));

	if(pDX->m_bSaveAndValidate)
	{
		if(pUniFolderChk->GetCheck() == BST_CHECKED)
		{
			m_nOption = UNIFOLDER_PIG;
			
		}
		else if(pRecursiveChk->GetCheck() == BST_CHECKED)
		{
			m_nOption = FOLDER_RECURSIVEG_PIG;
		}
		else
		{
			m_nOption = 0;
		}
	}
	else
	{
		if(m_nOption & UNIFOLDER_PIG)
		{
			pUniFolderChk->SetCheck(BST_CHECKED);
			pRecursiveChk->SetCheck(BST_UNCHECKED);
			pRecursiveChk->EnableWindow(FALSE);
		}
		else
		{
			pUniFolderChk->SetCheck(BST_UNCHECKED);
			pRecursiveChk->EnableWindow(TRUE);

			if(m_nOption & FOLDER_RECURSIVEG_PIG)
				pRecursiveChk->SetCheck(BST_CHECKED);
			else
				pRecursiveChk->SetCheck(BST_UNCHECKED);
		}
	}

	//DDX_Check(pDX,IDC_CHECK1,m_bRecursive);
}

bool CDlgAddFileItemOption::ShowAndGetOption(int& nOption,const CString& strFolderName)
{
	bool res = false;

	CDlgAddFileItemOption dlg(nOption,strFolderName,AfxGetMainWnd());
	if(dlg.DoModal() == IDOK)
		res = true;

	return res;

}


BEGIN_MESSAGE_MAP(CDlgAddFileItemOption, CDialog)
	ON_BN_CLICKED(IDC_CHECK_UNI_FOLDER, &CDlgAddFileItemOption::OnBnClickedCheckUniFolder)
END_MESSAGE_MAP()


// CDlgAddFileItemOption ��Ϣ�������

BOOL CDlgAddFileItemOption::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowText(CString(L"�ļ��м���-")+m_strFolderName);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CDlgAddFileItemOption::OnBnClickedCheckUniFolder()
{
	CButton* pRecursiveChk = ((CButton*)GetDlgItem(IDC_CHECK_RECUERSIVE));
	if(((CButton*)GetDlgItem(IDC_CHECK_UNI_FOLDER))->GetCheck() == BST_CHECKED)
	{
		pRecursiveChk->SetCheck(BST_UNCHECKED);
		pRecursiveChk->EnableWindow(FALSE);
	}
	else
	{
		pRecursiveChk->EnableWindow(TRUE);
	}
}
