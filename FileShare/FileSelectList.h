#pragma once

#include "ItemSelTransfer.h"
// CFileSelectList

class CFileSelectList : public CListCtrl
{
	DECLARE_DYNAMIC(CFileSelectList)

public:
	CFileSelectList();
	virtual ~CFileSelectList();

	bool AdInt();

	bool AddFileItem(LPITEMIDLIST pFullIDLS,int nOption);
	void AddUrlItem(const CString& strItemName,const CString& strItemUrl);

	void Export(CArray<TRANSFER_ITEMSEL_STRUCT>& aryItem,CImageList& ImageList);

	void DelSelItems();
	BOOL DelAllItems();
protected:
	bool IsAlreayIn(const CString& strPath);
protected:
	CImageList m_ImageListNormal;
	int m_IndexUrlIcon;
	CMap<int,int,int,int> m_iSys2Self;

	struct ItemDataType
	{
		CString strItemUrl;
	};

protected:
	//afx_msg void OnChar(UINT nChar,UINT nRepCnt,UINT nFlags);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
};


