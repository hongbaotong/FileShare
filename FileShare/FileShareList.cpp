/*****************************************************************************
*Copyright(c) 2013,BaoTong All rights reserved
*创建日期:	2013/04/11	
*文件名: 	FileShareList.cpp
*作者:	BaoTong
******************************************************************************/

#include "stdafx.h"
#include "FileShare.h"
#include "FileShareList.h"
#include "ItemSelectPage.h"
#include "ShareInfo.h"
// CFileShareList 对话框

IMPLEMENT_DYNAMIC(CFileShareList, CDialog)

CFileShareList::CFileShareList(CWnd* pParent /*=NULL*/)
	: CDialog(CFileShareList::IDD, pParent)
{

}

CFileShareList::~CFileShareList()
{
}

void CFileShareList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}
BOOL CFileShareList::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_pChkSelAll=(CButton*)GetDlgItem(IDC_CHK_SELECTALL);
	m_List=(CListCtrl*)GetDlgItem(IDC_LIST1);
	m_List->SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_CHECKBOXES);

	m_List->InsertColumn(1,_T("共享名"),LVCFMT_LEFT,120,1);
	m_List->InsertColumn(2,_T("路径"),LVCFMT_CENTER,200,1);
//	m_List->InsertColumn(3,_T("类型"),LVCFMT_CENTER,120,1);
	m_List->InsertColumn(3,_T("大小"),LVCFMT_CENTER,120,1);
	m_List->InsertColumn(4,_T("MD5"),LVCFMT_CENTER,220,1);

	CArray<FILE_INFO> fileInfoList;

	if(GetSvrApp()->m_ShareInfo->GetItemList(fileInfoList))
	{
		for(int i=0;i<fileInfoList.GetSize();i++)
		{
			m_List->InsertItem(i,_T(""));
			m_List->SetItemText(i,0,fileInfoList.GetAt(i).strFileName);
			m_List->SetItemText(i,1,fileInfoList.GetAt(i).strFilePath);
			//m_List->SetItemText(i,2,Fileinfo.strFileType);
			m_List->SetItemText(i,2,fileInfoList.GetAt(i).strFileSize);
			m_List->SetItemText(i,3,fileInfoList.GetAt(i).strFileMd5);
		}
	}

	return TRUE;
}

BEGIN_MESSAGE_MAP(CFileShareList, CDialog)
	ON_BN_CLICKED(IDC_BTN_ADD, &CFileShareList::OnBnClickedBtnAdd)
	ON_BN_CLICKED(IDC_BTN_DEL, &CFileShareList::OnBnClickedBtnDel)
	ON_BN_CLICKED(IDC_BTN_ADDFOLDER, &CFileShareList::OnBnClickedBtnAddfolder)
	ON_BN_CLICKED(IDC_CHK_SELECTALL, &CFileShareList::OnBnClickedChkSelectall)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CFileShareList::OnLvnItemchangedList1)
END_MESSAGE_MAP()


// CFileShareList 消息处理程序

void CFileShareList::OnBnClickedBtnAdd()
{
	// TODO: 在此添加控件通知处理程序代码
	FILE_INFO Fileinfo;
	CFileDialog fileDlg(TRUE);
	fileDlg.m_ofn.lpstrFilter=_T("所有文件(*.*)\0*.*\0\0");
	if(IDOK==fileDlg.DoModal())
	{
		
		CFileInfoProc InfoProc;
		Fileinfo=InfoProc.GetFileInfo(fileDlg.GetFileName());

		int itemCount=m_List->GetItemCount();
		m_List->InsertItem(itemCount,_T(""));
		m_List->SetItemText(itemCount,0,Fileinfo.strFileName);
		m_List->SetItemText(itemCount,1,Fileinfo.strFilePath);
		//m_List->SetItemText(itemCount,2,Fileinfo.strFileType);
		m_List->SetItemText(itemCount,2,Fileinfo.strFileSize);
		m_List->SetItemText(itemCount,3,Fileinfo.strFileMd5);

		GetSvrApp()->m_ShareInfo->AddItem(Fileinfo.strFileName,Fileinfo.strFilePath,Fileinfo.strFileSize,Fileinfo.strFileMd5);
	}
	
}

void CFileShareList::OnBnClickedBtnAddfolder()
{
	// TODO: 在此添加控件通知处理程序代码
	//CFolderDialog folderDlg(NULL,NULL,_T("文件夹列表"),BIF_RETURNONLYFSDIRS);
	//if(IDOK==folderDlg.DoModal())
	//{
	//	int itemCount=m_List->GetItemCount();

	//	m_List->InsertItem(itemCount,_T(""));

	//	m_List->SetItemText(itemCount,0,folderDlg.GetFolderName());
	//	m_List->SetItemText(itemCount,1,folderDlg.GetPathName());
	//	m_List->SetItemText(itemCount,2,_T("文件夹"));
	//}

	//CItemSelectPage ItemSelectDlg;
	//ItemSelectDlg.DoModal();
	
}

void CFileShareList::OnBnClickedBtnDel()
{
	// TODO: 在此添加控件通知处理程序代码
	//m_List->DeleteItem(m_List->GetSelectionMark());
	CString strItemName;
	for(int i=m_List->GetItemCount()-1; i>=0; i--)
	{
		if(m_List->GetCheck(i))
		{
			strItemName=m_List->GetItemText(i,0);
			if(GetSvrApp()->m_ShareInfo->DelItem(strItemName))
			{
				m_List->DeleteItem(i);
			}
		}
	}
	m_pChkSelAll->SetCheck(0);
}

void CFileShareList::OnBnClickedChkSelectall()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_pChkSelAll->GetCheck())
	{
		m_List->SetCheck(-1,TRUE);
	}
	else
	{
		m_List->SetCheck(-1,FALSE);
	}
	
}

void CFileShareList::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码

	int nSelCount=0;
	for(int i=0; i<m_List->GetItemCount(); i++)
	{
		if(m_List->GetCheck(i))
		{
			nSelCount++;
		}
	}
	if(nSelCount==m_List->GetItemCount())
	{
		m_pChkSelAll->SetCheck(1);
	}
	else
	{
		m_pChkSelAll->SetCheck(0);
	}

	*pResult = 0;
}
