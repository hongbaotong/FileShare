#pragma once

#include "ItemSelectPage.h"
#include "ItemBelongPage.h"
#include "ItemSelTransfer.h"


//ҳ���,����������ʼҳ
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
	//�鵵����Ŀ¼
	CString& m_strWorkDir;

protected:
	static bool st_bPop;
protected:
	virtual BOOL OnInitDialog( );
protected:
	CItemSelectPage m_ItemSelectPage;
	CItemBelongPage m_ItemBelongPage;


	//�鵵��Ŀ����
	CArray<CFileSetStore::ITEM_PIGEONHOLE_STRUCT>& m_aryrefPigeonholeItem;

	//ѡ���ļ���Ŀ���ݻ���
	CImageList		m_ImgListSelected;
	CArray<TRANSFER_ITEMSEL_STRUCT> m_aryItemSel;
	void ClearSelectedItemsBuf();

	friend CItemSelectPage;
	friend CItemBelongPage;
protected:
	DECLARE_MESSAGE_MAP()
};


