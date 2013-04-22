// FileSelectList.cpp : 实现文件
//

#include "stdafx.h"
#include "FileShare.h"
#include "LibCommon/common.h"
#include "FileSelectList.h"
#include "DlgAddFileitemOption.h"


// CFileSelectList

IMPLEMENT_DYNAMIC(CFileSelectList, CListCtrl)

CFileSelectList::CFileSelectList():m_IndexUrlIcon(0)
{

}

CFileSelectList::~CFileSelectList()
{
}

bool CFileSelectList::AdInt()
{
	bool res = false;

	SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

	m_ImageListNormal.Create(16,16,ILC_COLORDDB,10,1);
	m_ImageListNormal.SetBkColor(RGB(0,0,0));
	SetImageList(&m_ImageListNormal,LVSIL_SMALL);
	//HICON hIcon = AfxGetApp()->LoadIcon(IDI_URL);
	//if(hIcon != NULL)
	//{
	//	m_IndexUrlIcon = m_ImageListNormal.Add(hIcon);
	//	DestroyIcon(hIcon);
	//}

	int iFontWidth = GetWndFontWidth(this);

	LVCOLUMN lvc={0};
	lvc.mask = LVCF_TEXT|LVCF_WIDTH;
	lvc.pszText = L"名称";
	lvc.cchTextMax = wcslen( L"名称");
	lvc.iSubItem = 0;
	lvc.cx = 30*iFontWidth;
	int i = InsertColumn(0,&lvc);

	memset(&lvc,0,sizeof(lvc));
	lvc.mask = LVCF_TEXT|LVCF_WIDTH;
	lvc.pszText = L"路径";
	lvc.cchTextMax = wcslen(L"路径");
	lvc.iSubItem = 1;
	lvc.cx = 50*iFontWidth;
	i=	InsertColumn(1,&lvc);

	res =true;
	return res;
}

bool CFileSelectList::IsAlreayIn(const CString& strPath)
{
	bool res = false;
	for(int i = 0;i<GetItemCount();++i)
	{
		//CString strItemPath = GetItemText(i,1)+GetItemText(i,0);
		//if(strItemPath == strPath)
		//{
		//	res = true;
		//	break;
		//}
		if(((ItemDataType*)GetItemData(i))->strItemUrl == strPath)
		{
			res = true;
			break;
		}
	}
	return res;
}

bool CFileSelectList::AddFileItem(LPITEMIDLIST pFullIDLS,int nOption)
{
	bool res = false;

	SHFILEINFO fileInfo={0};
	if(SHGetFileInfo((LPCTSTR)pFullIDLS,0,&fileInfo,sizeof(fileInfo),SHGFI_PIDL |SHGFI_DISPLAYNAME|SHGFI_ATTRIBUTES|SHGFI_ICON|SHGFI_SMALLICON) != 0)
	{
		if(fileInfo.dwAttributes & SFGAO_FILESYSTEM)
		{

			bool bIsFile = false;
			if(fileInfo.dwAttributes & SFGAO_FOLDER)//判断是否是文件系统对象文件夹,排除zip文件
			{
				if(fileInfo.dwAttributes & SFGAO_STREAM) 
				{
					int nSysID = GetWindowsSystemID();
					if(nSysID == WINDOWS_XP || nSysID == WINDOWS_VISTA || nSysID == WINDOWS_2003)
					{
							bIsFile = true;//zip文件
					}
				}
			}
			else
			{
				bIsFile = true;
			}

			if(!bIsFile) //是文件夹，但不是.zip文件
			{
				IShellFolder *pIParentFolder,*pICurFolder;
				LPITEMIDLIST pLocalIDLS;

				if(nOption & UNIFOLDER_PIG)//作为整体归档
				{
					goto PIG_FILE;
				}
				else//归档下面的文件
				{
					if(S_OK == SHBindToParent(pFullIDLS,IID_IShellFolder,(void**)&pIParentFolder,(LPCITEMIDLIST*)&pLocalIDLS))
					{
						if(S_OK == pIParentFolder->BindToObject(pLocalIDLS,NULL,IID_IShellFolder,(void**)&pICurFolder))
						{
							IEnumIDList* pIEnum;
							SHCONTF enumFlag = (nOption&FOLDER_RECURSIVEG_PIG)?(SHCONTF_FOLDERS|SHCONTF_NONFOLDERS):SHCONTF_NONFOLDERS;
							if(S_OK == pICurFolder->EnumObjects(NULL,enumFlag,&pIEnum))
							{
								for(HRESULT call_res = pIEnum->Next(1,&pLocalIDLS,NULL);call_res == S_OK;call_res = pIEnum->Next(1,&pLocalIDLS,NULL))
								{
									LPITEMIDLIST pFullChildIDLST = ILCombine(pFullIDLS,pLocalIDLS);
									if(pFullChildIDLST != NULL)
									{
										AddFileItem(pFullChildIDLST,nOption);
										ILFree(pFullChildIDLST);
									}

									CoTaskMemFree(pLocalIDLS);
								}
								pIEnum->Release();
							}
							pICurFolder->Release();
						}
						pIParentFolder->Release();
					}
				}

			}
			else //is file
			{
PIG_FILE:
				TCHAR tcbufPath[MAX_PATH];
				if(SHGetPathFromIDList(pFullIDLS,tcbufPath))
				{
					if(!IsAlreayIn(FilePath2Url(CString(tcbufPath))))
					{
						CString strPathFolder,strFName,strDriver,strExt;
						SplitFilePath(CString(tcbufPath),strPathFolder,strDriver,strFName,strExt);

						LVITEM lvit = {0};
						lvit.mask = LVIF_IMAGE|LVIF_TEXT;
						lvit.iItem = GetItemCount();
						lvit.pszText = (LPWSTR)(LPCTSTR)strFName;
						lvit.cchTextMax = strFName.GetLength();

						int iImg;
						if(!m_iSys2Self.Lookup(fileInfo.iIcon,iImg))
						{

							//CImageList *pImgLs = GetImageList(LVSIL_SMALL);
							iImg = GetImageList(LVSIL_SMALL)->Add(fileInfo.hIcon);
							m_iSys2Self.SetAt(fileInfo.iIcon,iImg);
						}

						lvit.iImage = iImg;
						int iItem = InsertItem(&lvit);
						//SetItem(iItem,1,LVIF_TEXT,strPathFolder+L"\\",0,0,0,0);
						SetItemText(iItem,1,tcbufPath);

						ItemDataType* pItemData = new ItemDataType;
						pItemData->strItemUrl = FilePath2Url(tcbufPath);
						SetItemData(iItem,(DWORD_PTR)pItemData);

					}
				}
			}
		}
		DestroyIcon(fileInfo.hIcon);
	}
	
	return res;
}

void CFileSelectList::AddUrlItem(const CString& strItemName,const CString& strItemUrl)
{
	ItemDataType *pItemData = new ItemDataType;
	pItemData->strItemUrl = strItemUrl;

	LVITEM lvit = {0};
	lvit.mask = LVIF_IMAGE|LVIF_TEXT|LVIF_PARAM;
	lvit.iItem = GetItemCount();
	lvit.pszText = (LPWSTR)(LPCTSTR)strItemName;
	lvit.cchTextMax = strItemName.GetLength();
	lvit.lParam = (LPARAM)pItemData;

	lvit.iImage = m_IndexUrlIcon;
	int iItem = InsertItem(&lvit);

	SetItemText(iItem,1,strItemUrl);

	
}

void CFileSelectList::Export(CArray<TRANSFER_ITEMSEL_STRUCT>& aryItem,CImageList& ImageList)
{
	//导出图标
	//while(ImageList.GetImageCount()>0)
	//	ImageList.Remove(0);
	//for(int i = 0;i < m_ImageListNormal.GetImageCount();++i)
	//{
	//	HICON hIcon = m_ImageListNormal.ExtractIcon(i);
	//	if(hIcon)
	//	{
	//		ImageList.Add(hIcon);
	//		DestroyIcon(hIcon);
	//	}

	//}
	aryItem.RemoveAll();
	int iCount = GetItemCount();
	for(int i = 0;i < iCount; ++i)
	{
		CString strPath;
		TCHAR tcBuf[MAX_PATH];


		LVITEM lvit = {0};
		lvit.iItem = i;
		lvit.iSubItem = 0;
		lvit.pszText = tcBuf;
		lvit.cchTextMax = sizeof(tcBuf);
		lvit.mask = LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM;
		GetItem(&lvit);

		TRANSFER_ITEMSEL_STRUCT tsi={0};
		tsi.strItemName = lvit.pszText;
		tsi.strItemPath = ((ItemDataType*)lvit.lParam)->strItemUrl;//GetItemText(i,1) + tsi.strItemName;
		tsi.iItemImg = lvit.iImage;

		aryItem.Add(tsi);
	}

}


void  CFileSelectList::DelSelItems()
{
	POSITION pos = GetFirstSelectedItemPosition();
	CArray<int> aryIndex;
	while(pos != NULL)
	{
		aryIndex.InsertAt(0,GetNextSelectedItem(pos));
	}

	for(int i= 0;i<aryIndex.GetCount();++i)
	{
		ItemDataType* pItemData = (ItemDataType*)GetItemData(aryIndex[i]);
		SAFE_DELETE(pItemData)
		DeleteItem(aryIndex[i]);
	}
}

BOOL  CFileSelectList::DelAllItems()
{
	while(GetItemCount()>0)
	{
		ItemDataType* pItemData = (ItemDataType*)GetItemData(0);
		SAFE_DELETE(pItemData)
		DeleteItem(0);
	}
	return TRUE;
}


BEGIN_MESSAGE_MAP(CFileSelectList, CListCtrl)
//	ON_WM_CHAR()
ON_WM_DESTROY()
END_MESSAGE_MAP()



// CFileSelectList 消息处理程序

//void CFileSelectList::OnChar(UINT nChar,UINT nRepCnt,UINT nFlags)
//{
//	CListCtrl::OnKeyDown(nChar,nRepCnt,nFlags);
//}

BOOL CFileSelectList::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_DELETE)
	{
		for(int i = GetItemCount() - 1;i >= 0;--i)
		{
			if(GetItemState(i,LVIS_SELECTED )&LVIS_SELECTED)
			{
				ItemDataType* pItemData = (ItemDataType*)GetItemData(i);
				SAFE_DELETE(pItemData);
				DeleteItem(i);
			}
		}
	}
	return CListCtrl::PreTranslateMessage(pMsg);
}


void CFileSelectList::OnDestroy()
{
	while(GetItemCount()>0)
	{
		ItemDataType* pItemData = (ItemDataType*)GetItemData(0);
		SAFE_DELETE(pItemData);
		DeleteItem(0);
	}

	CListCtrl::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
}
