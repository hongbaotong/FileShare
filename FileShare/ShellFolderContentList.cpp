// ShellFolderContentList.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "FileShare.h"
#include "ShellFolderContentList.h"
#define ID_SHELLFOLDERLIST_ADD          32806
#include "DlgAddFileItemOption.h"

// CShellFolderContentList

IMPLEMENT_DYNAMIC(CShellFolderContentList, CListCtrl)

CShellFolderContentList::CShellFolderContentList()
{
	m_pFolderIDLS = NULL;
	m_pIShellFolder = NULL;
	m_pIContextMenuOfItem= NULL;

}

CShellFolderContentList::~CShellFolderContentList()
{
	if(m_pIContextMenuOfItem != NULL)
	{
		m_pIContextMenuOfItem->Release();
		m_pIContextMenuOfItem = NULL;
	}
	m_hWnd=NULL;
}

HWND CShellFolderContentList::Detach( )
{
	ClearContent();
	return CListCtrl::Detach();
}

bool CShellFolderContentList::AdInit()
{
	bool res = false;

	m_ImageListNormal.Create(16,16,ILC_COLORDDB,10,1);
	m_ImageListNormal.SetBkColor(RGB(255,255,255));
	SetImageList(&m_ImageListNormal,LVSIL_SMALL);

	res =true;
	return res;
}

void CShellFolderContentList::ClearContent()
{
	while(GetItemCount()>0)
	{
		//ILFree((LPITEMIDLIST)GetItemData(0));
		CoTaskMemFree((LPITEMIDLIST)GetItemData(0));
		DeleteItem(0);
	}
}

void CShellFolderContentList::ShowContent(LPITEMIDLIST pIDLS,IShellFolder* pIFolder)
{

	ClearContent();

	IEnumIDList* pIEnum;
	if(S_OK == pIFolder->EnumObjects(0,SHCONTF_FOLDERS| SHCONTF_NONFOLDERS,&pIEnum))
	{
		LPITEMIDLIST pLocalIDLS;
		int nFileStart = 0;//��ͨ�ļ���ʼλ��
		for(HRESULT call_res = pIEnum->Next(1,&pLocalIDLS,NULL);call_res == S_OK;call_res = pIEnum->Next(1,&pLocalIDLS,NULL))
		{
			LPITEMIDLIST pFullIDLS = ILCombine(pIDLS,pLocalIDLS);
			SHFILEINFO fileInfo={0};
			if(SHGetFileInfo((LPCTSTR)pFullIDLS,0,&fileInfo,sizeof(fileInfo),SHGFI_PIDL |SHGFI_DISPLAYNAME|SHGFI_ATTRIBUTES|SHGFI_ICON|SHGFI_SMALLICON) != 0)
			{
				int iImg;
				if(!m_iSys2Self.Lookup(fileInfo.iIcon,iImg))
				{
					iImg = GetImageList(LVSIL_SMALL)->Add(fileInfo.hIcon);
					m_iSys2Self.SetAt(fileInfo.iIcon,iImg);
				}
				DestroyIcon(fileInfo.hIcon);

				LVITEM lsItem={0};
				//����,�ļ��з���ǰ��
				bool bIsFile = false;
				if(fileInfo.dwAttributes & SFGAO_FOLDER)//�ж��Ƿ����ļ�ϵͳ�����ļ���,�ų�zip�ļ�
				{
					if(fileInfo.dwAttributes & SFGAO_STREAM) 
					{
						int nSysID = GetWindowsSystemID();
						if(nSysID == WINDOWS_XP || nSysID == WINDOWS_VISTA || nSysID == WINDOWS_2003)
						{
								bIsFile = true;//zip�ļ�
						}
					}
				}
				else
				{
					bIsFile = true;
				}
				
				if(!bIsFile)//���ļ���
				{
					int i;
					for(i=0;i<nFileStart;++i)
					{
						if(Compare_Str(fileInfo.szDisplayName,GetItemText(i,0))<0)
							break;
					}
					lsItem.iItem = i;

					++nFileStart;

				}
				else//���ļ�
				{
					int i;
					for(i=nFileStart;i<GetItemCount();++i)
					{
						if(Compare_Str(fileInfo.szDisplayName,GetItemText(i,0))<0)
							break;
					}
					lsItem.iItem = i;

				}
				
				lsItem.mask = LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM;
				lsItem.pszText = fileInfo.szDisplayName;
				lsItem.cchTextMax = _tcslen(fileInfo.szDisplayName);
				lsItem.iImage = iImg;
				lsItem.lParam = (DWORD_PTR)pLocalIDLS;
				InsertItem(&lsItem);
				

			}
			ILFree(pFullIDLS);

			//CoTaskMemFree(pLocalIDLS);
		}

		pIEnum->Release();

		m_pIShellFolder = pIFolder;
		m_pFolderIDLS = pIDLS;
	}

}

void CShellFolderContentList::SendAddItemMsg(const CArray<int>& aryItemIndex)
{
	for(int i=0;i<aryItemIndex.GetCount();++i)
	{
		if(aryItemIndex[i]>=0)
		{
			LPITEMIDLIST pFullIDLS = ILCombine(m_pFolderIDLS,(LPITEMIDLIST)GetItemData(aryItemIndex[i]));
			SHFILEINFO fileInfo={0};
			if(SHGetFileInfo((LPCTSTR)pFullIDLS,0,&fileInfo,sizeof(fileInfo),SHGFI_PIDL|SHGFI_ATTRIBUTES|SHGFI_DISPLAYNAME) != 0)
			{
				
				if(fileInfo.dwAttributes & SFGAO_FILESYSTEM)//�ж��Ƿ����ļ�ϵͳ�����ļ���
				{

					bool bIsFile = false;
					if(fileInfo.dwAttributes & SFGAO_FOLDER)//�ж��Ƿ����ļ�ϵͳ�����ļ���,�ų�zip�ļ�
					{
						if(fileInfo.dwAttributes & SFGAO_STREAM) 
						{
							int nSysID = GetWindowsSystemID();
							if(nSysID == WINDOWS_XP || nSysID == WINDOWS_VISTA || nSysID == WINDOWS_2003)
							{
									bIsFile = true;//zip�ļ�
							}
						}
					}
					else
					{
						bIsFile = true;
					}

					if(!bIsFile)//�ļ��е�����.zip�ļ�
					{
						int nOption = 0;
						if(CDlgAddFileItemOption::ShowAndGetOption(nOption,fileInfo.szDisplayName))
						{
							
							GetParent()->SendMessage(SHELLCONTENTLIST_ITEMDBCLICK_NOTIFY,nOption,(LPARAM)pFullIDLS);
							
						}
					}
					else//��ͨ�ļ�
					{
						GetParent()->SendMessage(SHELLCONTENTLIST_ITEMDBCLICK_NOTIFY,TRUE,(LPARAM)pFullIDLS);
					}
				}
				
			}
			ILFree(pFullIDLS);
		}
	}
}

BEGIN_MESSAGE_MAP(CShellFolderContentList, CListCtrl)
	ON_NOTIFY_REFLECT(NM_DBLCLK,OnItemDbClick)
	ON_NOTIFY_REFLECT(NM_RCLICK, &CShellFolderContentList::OnNMRclick)
END_MESSAGE_MAP()

void CShellFolderContentList::OnItemDbClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pInfoStruct = (LPNMITEMACTIVATE)pNMHDR;

	CArray<int> aryItemIndex;
	aryItemIndex.Add(pInfoStruct->iItem);
	SendAddItemMsg(aryItemIndex);
}

// CShellFolderContentList ��Ϣ�������



void CShellFolderContentList::OnNMRclick(NMHDR *pNMHDR, LRESULT *pResult)
{
	//SetFocus();

	//if((nFlags&MK_CONTROL)||(nFlags&MK_SHIFT))
	//	return;

	//UINT uFlags;
	//int  iItemHit = HitTest(point,&uFlags);
	//POSITION pos;
	//
	//if(iItemHit>=0 && ((uFlags&LVHT_ONITEM)!=0))
	//{
	//	CPoint sc_point = point;
	//	ClientToScreen(&sc_point);

	//	m_menuItem.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON,sc_point.x,sc_point.y,this);
	//}

	
	*pResult = 0;
}

void CShellFolderContentList::OnRButtonDown(UINT nFlags, CPoint point)
{
	SetFocus();

	if((nFlags&MK_CONTROL)||(nFlags&MK_SHIFT))
		return;

	UINT uFlags;
	int  iItemHit = HitTest(point,&uFlags);
	POSITION pos;
	
	if(iItemHit>=0 && ((uFlags&LVHT_ONITEM)!=0))
	{
		CPoint sc_point = point;
		ClientToScreen(&sc_point);

		//����ѡ����Ŀ
		bool bIn = false;
		pos = GetFirstSelectedItemPosition();
		while(pos != NULL)
		{
			int iItem=GetNextSelectedItem(pos);
			if(iItem == iItemHit) //�����Ŀ��ѡ����Ŀ֮��
			{
				bIn = true;
				break;
			}
		}
		if(!bIn)//�����Ŀû��ѡ����Ŀ֮��
		{

			//������ѡ����Ŀ��Ϊ��ѡ��
			pos = GetFirstSelectedItemPosition();
			while(pos != NULL)
			{
				int iItem=GetNextSelectedItem(pos);
				SetItemState(iItem,0,LVIS_SELECTED);
			}
			//ѡ�е����Ŀ
			SetItemState(iItemHit,LVIS_SELECTED,LVIS_SELECTED);
		}
		//�������Ŀ��λ������Ŀ
		SetItemState(iItemHit,LVIS_FOCUSED,LVIS_FOCUSED);
		
		//���ϵͳ�˵�
		CMenu ResMenu;
		ResMenu.LoadMenu(IDR_SHELLFOLDER_CONTENT_LIST);
		LPITEMIDLIST* aryOflpItemIDLs = new LPITEMIDLIST[GetSelectedCount()];
		if(aryOflpItemIDLs != NULL)
		{
			int i= 0;
			pos = GetFirstSelectedItemPosition();
			while(pos != NULL)
			{
				int iItem=GetNextSelectedItem(pos);
				aryOflpItemIDLs[i] = (LPITEMIDLIST)GetItemData(iItem);
				++i;
			}

			if(m_pIContextMenuOfItem != NULL)
			{
				m_pIContextMenuOfItem->Release();
				m_pIContextMenuOfItem = NULL;
			}
			if(m_pIShellFolder->GetUIObjectOf(GetParent()->m_hWnd,GetSelectedCount(),(LPCITEMIDLIST*)aryOflpItemIDLs,
				IID_IContextMenu,0,(void**)&m_pIContextMenuOfItem) == S_OK)
			{

				m_pIContextMenuOfItem->QueryContextMenu(ResMenu.GetSubMenu(0)->m_hMenu,2,
					ID_SHELLFOLDERLIST_ADD+1,ID_SHELLFOLDERLIST_ADD+1000,CMF_RESERVED& CMF_NORMAL);

				//CBuffer bufVerb(MAX_PATH*2);
				//HRESULT  hResult = pIContextMenu->GetCommandString(0,GCS_VERBA,NULL,(LPSTR)bufVerb.GetBufP(),bufVerb.GetBufLen()/2);

				//INT J=0;
			}
			delete aryOflpItemIDLs;
		}
		
		ResMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON,sc_point.x,sc_point.y,this);
	}
	else//û�е�����Ŀ
	{
		//������ѡ����Ŀ��Ϊ��ѡ��
		pos = GetFirstSelectedItemPosition();
		while(pos != NULL)
		{
			int iItem=GetNextSelectedItem(pos);
			SetItemState(iItem,0,LVIS_SELECTED);
		}
	}
}

BOOL CShellFolderContentList::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->hwnd == m_hWnd && pMsg->message == WM_RBUTTONDOWN)
	{
		CPoint point;
		point.x = LOWORD(pMsg->lParam);
		point.y = HIWORD(pMsg->lParam);
		OnRButtonDown((UINT)pMsg->wParam,point);
		return TRUE;
	}
	else if(pMsg->hwnd == m_hWnd &&  pMsg->message == WM_COMMAND 
		&& LOWORD(pMsg->wParam) >= ID_SHELLFOLDERLIST_ADD)
	{
		if(LOWORD(pMsg->wParam) == ID_SHELLFOLDERLIST_ADD)
		{
			CArray<int> aryItemIndex;
			POSITION pos = GetFirstSelectedItemPosition();
			while(pos != NULL)
			{
				aryItemIndex.Add(GetNextSelectedItem(pos));
			}
			SendAddItemMsg(aryItemIndex);
		}
		else//�ļ�ϵͳ�˵���
		{
			CBuffer bufVerb(MAX_PATH*2+2);
			CString strVerb;
			if(NOERROR  == m_pIContextMenuOfItem->GetCommandString(LOWORD(pMsg->wParam) - ID_SHELLFOLDERLIST_ADD -1 ,
				GCS_VERBW,NULL,(LPSTR)bufVerb.GetBufP(),bufVerb.GetBufLen()/2))
				strVerb = (LPCTSTR)bufVerb.GetBufP();


			CMINVOKECOMMANDINFO command_info={0};
			command_info.cbSize = sizeof(command_info);
			command_info.hwnd = ::GetDesktopWindow();//GetParent()->m_hWnd;
			command_info.lpVerb = (LPCSTR)(0x0000ffff & (LOWORD(pMsg->wParam) - ID_SHELLFOLDERLIST_ADD -1));

			if(strVerb != L"Pigeonhole")
			m_pIContextMenuOfItem->InvokeCommand( &command_info);

			//ˢ����ʾ����
			if(strVerb == L"delete")
				ShowContent(m_pFolderIDLS,m_pIShellFolder);
		}

		return TRUE;
	}
	else
		return CListCtrl::PreTranslateMessage(pMsg);
}
