// PigeonholeSheet.cpp : 实现文件
//

#include "stdafx.h"
#include "FSS.h"
#include "PigeonholeSheet.h"


// CPigeonholeSheet

IMPLEMENT_DYNAMIC(CPigeonholeSheet, CPropertySheet)


//CPigeonholeSheet::CPigeonholeSheet(CArray<CString>& aryItemName,CArray<CString>& aryBelongSets,CArray<CString>& aryStorePathSet,CArray<CString>& aryItemFullPath,int iStart,CWnd* pParentWnd, UINT iSelectPage)
//	:CPropertySheet(L"归档", pParentWnd, iSelectPage)
//	,m_aryExportItemName(aryItemName)
//	,m_aryExportBelongSets(aryBelongSets)
//	,m_aryExportStorePathSet(aryStorePathSet)
//	,m_aryExportItemFullPath(aryItemFullPath)
//	,m_aryrefPigeonholeItem(NULL)
//{
//	//创建选中文件项目的传递缓冲的图像列表
//	m_ImgListSelected.Create(16,16,ILC_COLORDDB,10,1);
//	m_ImgListSelected.SetBkColor(RGB(255,255,255));
//
//	SetWizardMode();
//
//	switch(iStart)
//	{
//	default:
//	case FILEITEM_SELECT_PAGE:
//		AddPage(&m_ItemSelectPage);
//		AddPage(&m_ItemBelongPage);
//		break;
//	case FILEITEM_BELONG_PAGE:
//		AddPage(&m_ItemBelongPage);
//		//如果是再归档.初始化选中项目信息
//		int iItemCount = aryItemName.GetCount();
//		for(int i=0;i<iItemCount;++i)
//		{
//			//LVITEM lvit={0};
//			//lvit.iItem = i;
//			//lvit.mask = LVIF_TEXT|LVIF_IMAGE;
//			//lvit.pszText = (LPCTSTR)aryItemName[i];
//			//lvit.iImage = ;
//
//			//m_aryItemsSeleted.
//		}
//
//	}
//
//	
//	
//
//	SetActivePage(0);
//
//}

bool CPigeonholeSheet::st_bPop = false;

CPigeonholeSheet::CPigeonholeSheet(CArray<CFileSetStore::ITEM_PIGEONHOLE_STRUCT>& aryPigeonholeItem,CString& strWorkDir,int iStart ,CWnd* pParentWnd)
:CPropertySheet(L"归档",pParentWnd,0),m_aryrefPigeonholeItem(aryPigeonholeItem),m_strWorkDir(strWorkDir),m_ItemBelongPage(strWorkDir)
{
	//创建选中文件项目的传递缓冲的图像列表
	m_ImgListSelected.Create(16,16,ILC_COLORDDB,10,1);
	m_ImgListSelected.SetBkColor(RGB(0,0,0));

	SetWizardMode();

	//如果工作目录为空，设置主界面工作目录为当前归档工作目录
	if(m_strWorkDir == _T(""))
		m_strWorkDir = ((CFSSApp*)AfxGetApp())->GetCurrWorkDir();


	switch(iStart)
	{
	default:
	case FILEITEM_SELECT_PAGE:
		AddPage(&m_ItemSelectPage);
		AddPage(&m_ItemBelongPage);
		break;
	case FILEITEM_BELONG_PAGE:
		AddPage(&m_ItemBelongPage);
		//如果是再归档.初始化选中项目信息
		int iItemCount = aryPigeonholeItem.GetCount();
		for(int i=0;i<iItemCount;++i)
		{
			CFileSetStore::ITEM_PIGEONHOLE_STRUCT item_pigeonhole = aryPigeonholeItem[i];
			TRANSFER_ITEMSEL_STRUCT tsi;
			tsi.strItemName = item_pigeonhole.strItemName;
			tsi.strItemPath = item_pigeonhole.strItemFullPath;
			tsi.strBelogSets = item_pigeonhole.strBelongSets;
			
			//SHFILEINFO fileInfo={0};
			//CMap<int,int,int,int> Imgmap_Sys2Local;

			//if(0 != SHGetFileInfo(item_pigeonhole.strItemFullPath,0,&fileInfo,sizeof(fileInfo),SHGFI_ICON|SHGFI_SMALLICON))
			//{
			//	int iImg;
			//	if(!Imgmap_Sys2Local.Lookup(fileInfo.iIcon,iImg))
			//	{
			//		iImg = m_ImgListSelected.Add(fileInfo.hIcon);
			//		Imgmap_Sys2Local.SetAt(fileInfo.iIcon,iImg);
			//	}
			//	tsi.iItemImg = iImg;
			//	DestroyIcon(fileInfo.hIcon);
			//}
			//else
			//{
			//	tsi.iItemImg = -1;
			//}

			m_aryItemSel.Add(tsi);
		}

	}


	SetActivePage(0);
}

CPigeonholeSheet::~CPigeonholeSheet()
{
}

void CPigeonholeSheet::ClearSelectedItemsBuf()
{
	m_aryItemSel.RemoveAll();
	while(m_ImgListSelected.GetImageCount()>0)
	m_ImgListSelected.Remove(0);
}

BOOL CPigeonholeSheet::OnInitDialog( )
{
	CPropertySheet::OnInitDialog();

	return TRUE;
}


bool CPigeonholeSheet::ShowGetPigeonholeInfo(CArray<CFileSetStore::ITEM_PIGEONHOLE_STRUCT>& aryPigeonholeItem,CString& strWorkDir,int iStart)
{
	bool res = false;

	if(!st_bPop)
	{
		st_bPop = true;

		if(strWorkDir==_T(""))
			strWorkDir = ((CFSSApp*)AfxGetApp())->GetCurrWorkDir();
		CPigeonholeSheet sheet(aryPigeonholeItem,strWorkDir,iStart);
		if(sheet.DoModal() == ID_WIZFINISH)
			res = true;

		st_bPop = false;
	}

	return res;
}

bool CPigeonholeSheet::IsOnePop()
{
	return st_bPop;
}

//bool CPigeonholeSheet::ShowGetPigeonholeInfo(CArray<CString>& aryItemName,CArray<CString>& aryBelongSets,CArray<CString>& aryStorePathSet,CArray<CString>& aryItemFullPath,int iStart)
//{
//	bool res = false;
//
//	CPigeonholeSheet sheet(aryItemName,aryBelongSets,aryStorePathSet,aryItemFullPath);
//	if(sheet.DoModal() == ID_WIZFINISH)
//		res = true;
//
//	return res;
//
//}

BEGIN_MESSAGE_MAP(CPigeonholeSheet, CPropertySheet)
END_MESSAGE_MAP()


// CPigeonholeSheet 消息处理程序


