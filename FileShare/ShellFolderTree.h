#pragma once
#include "LibCommon/common.h"

// CShellFolderTree

class CShellFolderTree : public CTreeCtrl
{
	DECLARE_DYNAMIC(CShellFolderTree)

public:
	CShellFolderTree();
	virtual ~CShellFolderTree();
	HWND Detach( );

	bool AdInit();
protected:
	void ReleaseFolderInterface(HTREEITEM hItem);
	void AddChildNode(HTREEITEM hItem);
protected:
	CMap<int,int,int,int> m_iSys2Self;
	CImageList  m_ImageLsitNorlmal;
	struct TreeNodeInfoStruct
	{
		IShellFolder* pIFolder;
		LPITEMIDLIST pIDLS;
	};
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult);
public:
	afx_msg void OnTvnItemexpanded(NMHDR *pNMHDR, LRESULT *pResult);
};


