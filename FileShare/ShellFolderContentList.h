#pragma once
#include "LibCommon/common.h"

// CShellFolderContentList

class CShellFolderContentList : public CListCtrl
{
	DECLARE_DYNAMIC(CShellFolderContentList)

public:
	CShellFolderContentList();
	virtual ~CShellFolderContentList();

	bool AdInit();
	HWND Detach( );
	void ShowContent(LPITEMIDLIST pIDLS,IShellFolder* pIFolder);
	void SendAddItemMsg(const CArray<int>& aryItemIndex);
protected:
	void ClearContent();
protected:
	CImageList m_ImageListNormal;
	CMap<int,int,int,int> m_iSys2Self;

	
	LPITEMIDLIST m_pFolderIDLS;
	IShellFolder* m_pIShellFolder;
	IContextMenu* m_pIContextMenuOfItem;
protected:
	afx_msg void OnItemDbClick(NMHDR *pNMHDR, LRESULT *pResult);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMRclick(NMHDR *pNMHDR, LRESULT *pResult);
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void OnRButtonDown(UINT nFlags, CPoint point);
};


