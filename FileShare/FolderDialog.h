// FolderDialog.h: interface for the CFolderDialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(FOLDERDIALOG_H__INCLUDED_)
#define AFX_FOLDERDIALOG_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef	BIF_NEWDIALOGSTYLE
#define	BIF_NEWDIALOGSTYLE	0x0040
#endif

#ifndef	BIF_USENEWUI
#define	BIF_USENEWUI		0x0050
#endif


class CFolderDialog : public CObject  
{
public:
	CFolderDialog(LPCITEMIDLIST pidlRoot, LPTSTR lpszDisplayName, 
		LPCTSTR lpszTitle, UINT nFlags);
	virtual ~CFolderDialog();

	int DoModal();
	CString GetPathName();

	BROWSEINFO m_bi;
	LPCITEMIDLIST m_pidl;
public:
	CString GetFolderName(void);
};

#endif // !defined(FOLDERDIALOG_H__INCLUDED_)
