// FolderDialog.cpp: implementation of the CFolderDialog class.
// Download by http://www.codefans.net
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FolderDialog.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFolderDialog::CFolderDialog(LPCITEMIDLIST pidlRoot, LPTSTR lpszDisplayName, 
	LPCTSTR lpszTitle, UINT nFlags) 
{
	memset(&m_bi, 0, sizeof(BROWSEINFO));
	m_bi.pidlRoot = pidlRoot;
	m_bi.pszDisplayName = lpszDisplayName;
	m_bi.lpszTitle = lpszTitle;
	m_bi.ulFlags = nFlags;

	m_pidl = NULL;
}

CFolderDialog::~CFolderDialog()
{

}

int CFolderDialog::DoModal()
{
	int nResult = 0;

	m_pidl = SHBrowseForFolder(&m_bi);

	if (m_pidl != NULL)
	{
		nResult = IDOK;
	}
	else
	{
		nResult = IDCANCEL;
	}

	return nResult;
} 

CString CFolderDialog::GetPathName()
{
	CString strPathName = _T("");
	TCHAR szPathName[MAX_PATH];

	if(::SHGetPathFromIDList(m_pidl, szPathName))
	{
		strPathName = szPathName;
	}

	return strPathName;
}

CString CFolderDialog::GetFolderName(void)
{
	CString strPathName=GetPathName();

	if(strPathName.GetLength()>3)
	{
		int nPos = strPathName.Find('\\');
		CString sSubStr = strPathName;
		while (nPos)
		{
			sSubStr = sSubStr.Mid(nPos+1,sSubStr.GetLength()-nPos);  //取'\'右边字符串
			nPos = sSubStr.Find('\\');   //不包含'\'，函数值返回-1 
			if (nPos==-1)   
			{
				nPos = 0;
			}
		}
		return sSubStr;
	}
	else
	{
		return strPathName;
	}
}
