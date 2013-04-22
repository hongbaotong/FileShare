#pragma once

#include "ItemSelectPage.h"
#include "ItemBelongPage.h"
#include "ItemSelTransfer.h"


//页标记,用来设置起始页
#define FILEITEM_SELECT_PAGE 0
#define FILEITEM_BELONG_PAGE 1


// CPigeonholeSheet

class CPigeonholeSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CPigeonholeSheet)

public:
	CPigeonholeSheet(CArray<CFileSetStore::ITEM_PIGEONHOLE_STRUCT>& aryPigeonholeItem,CString& strWorkDir,int iStart = 0,CWnd* pParentWnd = NULL);
	virtual ~CPigeonholeSheet();

	static bool ShowGetPigeonholeInfo(CArray<CFileSetStore::ITEM_PIGEONHOLE_STRUCT>& aryPigeonholeItem,CString& strWorkDir,int iStart = 0);
	static bool IsOnePop();

public:
	//归档工作目录
	CString& m_strWorkDir;

protected:
	static bool st_bPop;
protected:
	virtual BOOL OnInitDialog( );
protected:
	CItemSelectPage m_ItemSelectPage;
	CItemBelongPage m_ItemBelongPage;


	//归档项目数组
	CArray<CFileSetStore::ITEM_PIGEONHOLE_STRUCT>& m_aryrefPigeonholeItem;

	//选中文件项目传递缓存
	CImageList		m_ImgListSelected;
	CArray<TRANSFER_ITEMSEL_STRUCT> m_aryItemSel;
	void ClearSelectedItemsBuf();

	friend CItemSelectPage;
	friend CItemBelongPage;
protected:
	DECLARE_MESSAGE_MAP()
};


