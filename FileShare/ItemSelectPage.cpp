// ItemSelectPage.cpp : 实现文件
//

#include "stdafx.h"
#include "FileShare.h"
#include "ItemSelectPage.h"


// CItemSelectPage 对话框

IMPLEMENT_DYNAMIC(CItemSelectPage, CDialog)

CItemSelectPage::CItemSelectPage(CWnd* pParent /*=NULL*/)
	: CDialog(CItemSelectPage::IDD, pParent)
{

}

CItemSelectPage::~CItemSelectPage()
{
}

BOOL  CItemSelectPage::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_ShellFolderContentList.Attach(GetDlgItem(IDC_LIST_FILE)->m_hWnd);
	m_ShellFolderContentList.AdInit();

	m_ShellFolderTree.Attach(GetDlgItem(IDC_TREE_FOLDER)->m_hWnd);
	m_ShellFolderTree.AdInit();

	m_FileSelectList.SubclassWindow(GetDlgItem(IDC_LIST_ITEM)->m_hWnd);
	m_FileSelectList.AdInt();


	return TRUE;
}

void CItemSelectPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CItemSelectPage, CDialog)
	//ON_WM_DESTROY()
	ON_MESSAGE(SHELLTREE_SELECTEDCHANGE_NOTIFY,OnShellTreeSelectedChg)
	ON_MESSAGE(SHELLCONTENTLIST_ITEMDBCLICK_NOTIFY,OnDbClickShellItem)
	ON_BN_CLICKED(IDC_DELITEM_BUT, &CItemSelectPage::OnBnClickedDelitemBut)
	ON_BN_CLICKED(IDC_CLRLIST_BUT, &CItemSelectPage::OnBnClickedClrlistBut)
END_MESSAGE_MAP()


// CItemSelectPage 消息处理程序
LRESULT CItemSelectPage::OnShellTreeSelectedChg(WPARAM wParam, LPARAM lParam)
{

	m_ShellFolderContentList.ShowContent((LPITEMIDLIST)wParam,(IShellFolder*)lParam);
	return 0;
}

LRESULT CItemSelectPage::OnDbClickShellItem(WPARAM wParam, LPARAM lParam)
{
	m_FileSelectList.AddFileItem((LPITEMIDLIST)lParam,wParam);
	return 0;
}
void CItemSelectPage::OnBnClickedDelitemBut()
{
	// TODO: 在此添加控件通知处理程序代码
	m_FileSelectList.DelSelItems();
}

void CItemSelectPage::OnBnClickedClrlistBut()
{
	// TODO: 在此添加控件通知处理程序代码
	m_FileSelectList.DelAllItems();
}

//void CItemSelectPage::OnDestroy()
//{
//	m_ShellFolderContentList.Detach();
//	m_ShellFolderTree.Detach();
//	//m_FileSelectList.UnsubclassWindow();
//
//	CPropertyPage::OnDestroy();
//
//}