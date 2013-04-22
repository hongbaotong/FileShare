// HostShareList.cpp : 实现文件
//

#include "stdafx.h"
#include "FileShare.h"
#include "HostShareList.h"


// CHostShareList 对话框

IMPLEMENT_DYNAMIC(CHostShareList, CDialog)

CHostShareList::CHostShareList(CWnd* pParent /*=NULL*/)
	: CDialog(CHostShareList::IDD, pParent)
{

}

CHostShareList::~CHostShareList()
{
	//m_mapIpToInfo.RemoveAll();
}


BOOL CHostShareList::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_BroadThread=new CBroadCastThread();
	m_BroadThread->CreateThread();
	m_BroadThread->SetHwnd(m_hWnd);

	//初始化文件共享列表
	m_List=(CListCtrl*)GetDlgItem(IDC_FILE_LIST);
	m_List->SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_CHECKBOXES);
	m_List->InsertColumn(1,_T("文件"),LVCFMT_CENTER,120,1);
	//m_List->InsertColumn(2,_T("类型"),LVCFMT_CENTER,120,1);
	m_List->InsertColumn(2,_T("大小"),LVCFMT_CENTER,200,1);
	m_List->InsertColumn(3,_T("MD5"),LVCFMT_CENTER,220,1);

	//初始化局域网主机列表
	m_HostList=(CListCtrl*)GetDlgItem(IDC_HOST_LIST);
	m_HostList->SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	m_HostList->InsertColumn(1,_T("名称"),LVCFMT_CENTER,81,1);
	m_HostList->InsertColumn(2,_T("IP"),LVCFMT_CENTER,104,1);

	m_mapIpToInfo.InitHashTable(10*1024);

	SetTimer(1,3000,NULL);//广播定时器
	SetTimer(2,4000,NULL);//上下线监视定时器

	return TRUE;
}

void CHostShareList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CHostShareList, CDialog)
	ON_WM_TIMER()
	ON_MESSAGE(UM_HOSTINFO,OnGetHostInfo)
	ON_NOTIFY(NM_DBLCLK, IDC_HOST_LIST, &CHostShareList::OnNMDblclkHostList)
	ON_NOTIFY(NM_RCLICK, IDC_FILE_LIST, &CHostShareList::OnNMRclickFileList)
	ON_NOTIFY(NM_DBLCLK, IDC_FILE_LIST, &CHostShareList::OnNMDblclkFileList)
END_MESSAGE_MAP()


// CHostShareList 消息处理程序
void CHostShareList::OnTimer(UINT_PTR nIDEvent)
{
	switch(nIDEvent)
	{
	case 1:m_BroadThread->OnBroadCast();break;
	case 2:CheckOnline();break;
	}	
}

LRESULT CHostShareList::OnGetHostInfo(WPARAM wParam,LPARAM lParam)
{
	HOST_INFO *pHostInfo=NULL;
	pHostInfo=(HOST_INFO*)wParam;

	
	HOST_INFO *pTempInfo=NULL;
	if(!m_mapIpToInfo.Lookup(pHostInfo->strAddr,(void*&)pTempInfo))
	{
		m_mapIpToInfo.SetAt(pHostInfo->strAddr,pHostInfo);
		InsertHostItem(pHostInfo);
	}
	else
	{
		m_mapIpToInfo.SetAt(pHostInfo->strAddr,pHostInfo);
		if((pTempInfo->strSvrName)!=(pHostInfo->strSvrName))
		{
			for(int i=m_HostList->GetItemCount()-1;i>=0;i--)
			{
				CString strIP=m_HostList->GetItemText(i,1);
				if(strIP==pTempInfo->strAddr)
				{
					m_HostList->DeleteItem(i);
					break;
				}
			}
			
			InsertHostItem(pHostInfo);
		}
	}

	return 0;
}


void CHostShareList::OnNMDblclkHostList(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	CString strAddr;
	CArray<FILE_INFO> aryFileList;

	for(int i=0;i<m_HostList->GetItemCount();i++)
	{
		if(m_HostList->GetItemState(i,LVIS_SELECTED))
		{
			strAddr=m_HostList->GetItemText(i,1);
		}
	}

	HOST_INFO *pHostInfo=NULL;

	if(m_mapIpToInfo.Lookup(strAddr,(void*&)pHostInfo))
	{
		m_ClientTask=new CClientTask(pHostInfo);
		if(m_ClientTask!=NULL)
		{
			m_ClientTask->GetShareList(aryFileList);

			DelAllFileItem();
			for(int i=0;i<aryFileList.GetCount();i++)
			{
				InsertFileItem(aryFileList.GetAt(i).strFileName,aryFileList.GetAt(i).strFileSize,aryFileList.GetAt(i).strFileMd5);
			}
		}
	}
	*pResult = 0;
}

void CHostShareList::InsertHostItem(HOST_INFO* pHostInfo)
{
	int itemCount=m_HostList->GetItemCount();
	m_HostList->InsertItem(itemCount,_T(""));
	m_HostList->SetItemText(itemCount,0,pHostInfo->strSvrName);
	m_HostList->SetItemText(itemCount,1,pHostInfo->strAddr);
}

void CHostShareList::InsertFileItem(CString strName, CString strSize, CString strMd5)
{
	int itemCount=m_List->GetItemCount();
	m_List->InsertItem(itemCount,_T(""));
	m_List->SetItemText(itemCount,0,strName);
	m_List->SetItemText(itemCount,1,strSize);
	m_List->SetItemText(itemCount,2,strMd5);
}

void CHostShareList::DelAllFileItem(void)
{
	if(m_List->GetItemCount()>0)
	{
		for(int i=m_List->GetItemCount()-1;i>=0;i--)
		{
			m_List->DeleteItem(i);
		}
	}
}

void CHostShareList::OnNMRclickFileList(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_List->GetItemCount()>0)
	{
		CPoint pt;
		GetCursorPos(&pt);
		CMenu menu;
		VERIFY(menu.LoadMenuW(IDR_MENU_DOWNLOAD));

		CMenu *pPopup=menu.GetSubMenu(0);
		ASSERT(pPopup!=NULL);

		CWnd* pWndPopupOwner=(CWnd*)m_List;

		while(pWndPopupOwner->GetStyle() & WS_CHILD)
			pWndPopupOwner=pWndPopupOwner->GetParent();
		pPopup->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,pt.x,pt.y,pWndPopupOwner);
	}
	
	*pResult = 0;
}

void CHostShareList::CheckOnline(void)
{
	CTime ct=CTime::GetTickCount();
	time_t TimeStamp=ct.GetTime();

	for(int i=m_HostList->GetItemCount()-1;i>=0;i--)
	{
		CString strIP=m_HostList->GetItemText(i,1);

		HOST_INFO *pTempInfo;
		if(m_mapIpToInfo.Lookup(strIP,(void*&)pTempInfo))
		{
			if((TimeStamp-pTempInfo->TimeStamp)>4)
			{
				m_HostList->DeleteItem(i);
				m_mapIpToInfo.RemoveKey(strIP);
			}
		}
	}
}

void CHostShareList::OnNMDblclkFileList(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	CString strFileName;

	if(m_List->GetItemCount()>0)
	{
		for(int i=0;i<m_List->GetItemCount();i++)
		{
			if(m_List->GetItemState(i,LVIS_SELECTED))
			{
				strFileName=m_List->GetItemText(i,0);
				m_ClientTask->GetFile(strFileName);
			}
		}
	}
	
	*pResult = 0;
}
