// ShellFolderTree.cpp : 实现文件
//

#include "stdafx.h"
#include "ShellFolderTree.h"


// CShellFolderTree

IMPLEMENT_DYNAMIC(CShellFolderTree, CTreeCtrl)

CShellFolderTree::CShellFolderTree()
{

}

CShellFolderTree::~CShellFolderTree()
{
	m_hWnd=NULL;
}
HWND CShellFolderTree::Detach( )
{
	ReleaseFolderInterface(GetRootItem());
	m_ImageLsitNorlmal.Detach();
	return CTreeCtrl::Detach();
}

void CShellFolderTree::ReleaseFolderInterface(HTREEITEM hItem)
{
	if(hItem!=NULL)
	{
		TreeNodeInfoStruct* pNodeInfo = (TreeNodeInfoStruct*)GetItemData(hItem);
		if(pNodeInfo != NULL)
		{
			if(pNodeInfo->pIFolder != NULL)
				pNodeInfo->pIFolder->Release();
			if(pNodeInfo->pIDLS != NULL)
				ILFree(pNodeInfo->pIDLS);
			delete pNodeInfo;
		}
		for(HTREEITEM hChild = GetChildItem(hItem);hChild != NULL;hChild = GetNextSiblingItem(hChild))
		{
			ReleaseFolderInterface(hChild);
		}
	}
}

bool CShellFolderTree::AdInit()
{
	bool res = false;

	m_ImageLsitNorlmal.Create(16,16,ILC_COLORDDB,10,1);
	m_ImageLsitNorlmal.SetBkColor(RGB(255,255,255));
	SetImageList(&m_ImageLsitNorlmal,TVSIL_NORMAL);


	LPITEMIDLIST  pIDLS;
	if(S_OK == SHGetFolderLocation(NULL,CSIDL_DESKTOP,NULL,0,&pIDLS))
	{
		SHFILEINFO fileInfo={0};
		if(SHGetFileInfo((LPCTSTR)pIDLS,0,&fileInfo,sizeof(fileInfo),SHGFI_PIDL |SHGFI_DISPLAYNAME|SHGFI_ICON) != 0 )
		{
			int iImg = GetImageList(TVSIL_NORMAL)->Add(fileInfo.hIcon);
			m_iSys2Self.SetAt(fileInfo.iIcon,iImg);
			DestroyIcon(fileInfo.hIcon);

			TreeNodeInfoStruct* pNodeInfo = NULL;

			TVINSERTSTRUCT tvItem={0};
			tvItem.hParent = NULL;
			tvItem.item.mask = TVIF_TEXT|TVIF_IMAGE|TVIF_SELECTEDIMAGE;
			tvItem.item.iImage = iImg;
			tvItem.item.iSelectedImage = iImg;
			tvItem.item.pszText = fileInfo.szDisplayName;
			pNodeInfo = new TreeNodeInfoStruct;
			if(pNodeInfo != NULL)
			{
				SHGetDesktopFolder(&pNodeInfo->pIFolder);
				pNodeInfo->pIDLS = ILClone(pIDLS);
				tvItem.item.mask |= TVIF_PARAM;
				tvItem.item.lParam = (LPARAM)pNodeInfo;
			}
			
			HTREEITEM  hRootItem = InsertItem(&tvItem);
			AddChildNode(hRootItem);

			res = true;
		}
		
		CoTaskMemFree(pIDLS);
		
	}
	
	return res;
}
void CShellFolderTree::AddChildNode(HTREEITEM hItem)
{
	if(hItem != NULL && GetChildItem(hItem) == NULL)
	{
		IShellFolder* pIFolderParent = ((TreeNodeInfoStruct*)GetItemData(hItem))->pIFolder;
		if(pIFolderParent != NULL)
		{
			IEnumIDList* pIEnum; 
			if(S_OK == pIFolderParent->EnumObjects(0,SHCONTF_FOLDERS,&pIEnum))
			{
				LPITEMIDLIST pLocalIDLS;
				for(HRESULT call_res = pIEnum->Next(1,&pLocalIDLS,NULL);call_res == S_OK;call_res = pIEnum->Next(1,&pLocalIDLS,NULL))
				{
					LPITEMIDLIST pFullIDLS = ILCombine(((TreeNodeInfoStruct*)GetItemData(hItem))->pIDLS,pLocalIDLS);
					SHFILEINFO fileInfo={0};
					if(SHGetFileInfo((LPCTSTR)pFullIDLS,0,&fileInfo,sizeof(fileInfo),SHGFI_PIDL |SHGFI_DISPLAYNAME|SHGFI_ATTRIBUTES|SHGFI_ICON|SHGFI_SMALLICON) != 0)
					{

						int nSysID = GetWindowsSystemID();
						if(nSysID == WINDOWS_XP || nSysID == WINDOWS_VISTA || nSysID == WINDOWS_2003)
						{
							if(fileInfo.dwAttributes & SFGAO_STREAM)//是zip文件
								continue;
						}

						int iImg;
						if(!m_iSys2Self.Lookup(fileInfo.iIcon,iImg))
						{
							iImg = GetImageList(TVSIL_NORMAL)->Add(fileInfo.hIcon);
							m_iSys2Self.SetAt(fileInfo.iIcon,iImg);
						}
						DestroyIcon(fileInfo.hIcon);
				
						TVINSERTSTRUCT tvItem={0};
						tvItem.hParent = hItem;
						tvItem.item.mask = TVIF_TEXT|TVIF_IMAGE|TVIF_SELECTEDIMAGE;
						tvItem.item.iImage = iImg;
						tvItem.item.iSelectedImage = iImg;
						tvItem.item.pszText = fileInfo.szDisplayName;

						TreeNodeInfoStruct *pNodeInfo = new TreeNodeInfoStruct;
						if(pNodeInfo != NULL)
						{
							pIFolderParent->BindToObject(pLocalIDLS,NULL,IID_IShellFolder,(LPVOID*)&pNodeInfo->pIFolder);
							pNodeInfo->pIDLS = pFullIDLS;
							tvItem.item.mask |= TVIF_PARAM;
							tvItem.item.lParam = (LPARAM)pNodeInfo;
						}

						//按照语音顺序排序
						tvItem.hInsertAfter = TVI_FIRST;
						for(HTREEITEM hChildItem = GetChildItem(hItem);hChildItem != NULL;hChildItem = GetNextSiblingItem(hChildItem))
						{
							CString strChildFolderName = GetItemText(hChildItem);
							if(Compare_Str(fileInfo.szDisplayName,strChildFolderName)>0)
								tvItem.hInsertAfter = hChildItem;
							else
								break;
						}

						InsertItem(&tvItem);
					}
					CoTaskMemFree(pLocalIDLS);
				}
				pIEnum->Release();
			}

		}
	}

}

BEGIN_MESSAGE_MAP(CShellFolderTree, CTreeCtrl)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, &CShellFolderTree::OnTvnSelchanged)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, &CShellFolderTree::OnTvnItemexpanded)
END_MESSAGE_MAP()



// CShellFolderTree 消息处理程序



void CShellFolderTree::OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	if(pNMTreeView->itemOld.hItem == NULL || !TreeCtrl_IsChildItem(pNMTreeView->itemOld.hItem,pNMTreeView->itemNew.hItem,*this))
		Expand(pNMTreeView->itemNew.hItem,TVE_EXPAND);

	GetParent()->SendMessage(SHELLTREE_SELECTEDCHANGE_NOTIFY,(WPARAM)((TreeNodeInfoStruct*)GetItemData(pNMTreeView->itemNew.hItem))->pIDLS,(LPARAM)((TreeNodeInfoStruct*)GetItemData(pNMTreeView->itemNew.hItem))->pIFolder);

	*pResult = 0;
}

void CShellFolderTree::OnTvnItemexpanded(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	
	if(pNMTreeView->itemNew.state & TVIS_EXPANDED)
	{
		CString strItem = GetItemText(pNMTreeView->itemNew.hItem);

		for(HTREEITEM hChild = GetChildItem(pNMTreeView->itemNew.hItem);hChild != NULL;hChild = GetNextSiblingItem(hChild))
		{
			CString strChildItem = GetItemText(hChild);
			AddChildNode(hChild);

			//if(GetChildItem(hChild) == NULL)
			//{
			//	IShellFolder* pIFolder = (IShellFolder*)GetItemData(hChild);
			//	if(pIFolder != NULL)
			//	{
			//		IEnumIDList* pIEnum; 
			//		if(S_OK == pIFolder->EnumObjects(0,SHCONTF_FOLDERS,&pIEnum))
			//		{
			//			LPITEMIDLIST pIDLS;
			//			for(HRESULT call_res = pIEnum->Next(1,&pIDLS,NULL);call_res == S_OK;call_res = pIEnum->Next(1,&pIDLS,NULL))
			//			{
			//				SHFILEINFO fileInfo={0};
			//				DWORD_PTR res = SHGetFileInfo((LPCTSTR)pIDLS,0,&fileInfo,sizeof(fileInfo),SHGFI_PIDL |SHGFI_DISPLAYNAME|SHGFI_TYPENAME|SHGFI_ICON|SHGFI_SMALLICON);

			//				int iImg = GetImageList(TVSIL_NORMAL)->Add(fileInfo.hIcon);
			//				DestroyIcon(fileInfo.hIcon);

			//				TVINSERTSTRUCT tvItem={0};
			//				tvItem.hParent = hChild;
			//				tvItem.item.mask = TVIF_TEXT|TVIF_IMAGE;
			//				tvItem.item.iImage = iImg;
			//				tvItem.item.pszText = fileInfo.szDisplayName;
			//				IShellFolder* pIFolderSub;
			//				if(S_OK == pIFolder->BindToObject(pIDLS,NULL,IID_IShellFolder,(LPVOID*)&pIFolderSub))
			//				{
			//					tvItem.item.mask |= TVIF_PARAM;
			//					tvItem.item.lParam = (LPARAM)pIFolderSub;
			//				}
			//				InsertItem(&tvItem);
			//			}
			//		}
			//	}
			//}
		}
	}
	

	
	*pResult = 0;
}
