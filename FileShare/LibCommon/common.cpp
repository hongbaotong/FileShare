
#include "stdafx.h"
#include "common.h"
#include <Tlhelp32.h>
#include <stack>
#include <Psapi.h>
#include <shobjidl.h>
#include <wininet.h>
#include <afxdisp.h>        // MFC 自动化类


CString GetCommonPath(const CString& strPath1,const CString& strPath2)
{
	CString res;

	CCopyArray<CString> aryPath1 = Str2StrAry(strPath1,L"\\",false);
	CCopyArray<CString> aryPath2 = Str2StrAry(strPath2,L"\\",false);

	int iPathItem;
	for(iPathItem = 0;iPathItem<aryPath1.GetCount();++iPathItem)
	{
		if(aryPath1[iPathItem] != aryPath2[iPathItem])
			break;
	}
	if(iPathItem>0)
	{
		while(aryPath1.GetCount()>iPathItem)
			aryPath1.RemoveAt(aryPath1.GetCount()-1);

		res = StrAry2Str(aryPath1,L"\\");
	}
	else
		res = L"";

	return res;
}

bool SplitFilePath(const CString& strFullPathName,CString& strFolderPath,CString& strDriver,CString& strFName,CString& strExt)
{
	bool res = false;

	
	CString strTmp;
	int i = strFullPathName.Find(L"\\");
	if(i > 0 && i < strFullPathName.GetLength())
	{
		strTmp = strFullPathName.Left(i);
		if(strTmp.Find(L":") == 1)
		{
			strDriver = strTmp;
			int j = strFullPathName.ReverseFind(L'\\');
			if(j > 0 && j < strFullPathName.GetLength())
			{
				strFName = strFullPathName.Mid(j+1);
				
				int iDotExt = strFName.ReverseFind(L'.');
				if(iDotExt>=0)
					strExt = strFName.Mid(iDotExt+1);
				else
					strExt = L"";

				strFolderPath = strFullPathName.Left(j);
				res = true;
			}
		}
	}

	return res;

}

bool IsFileExist(const CString& strPath)
{
	bool res = false;

	WIN32_FIND_DATA	find_data={0};
	HANDLE hFind =FindFirstFile(strPath,&find_data);
	bool bExist = false;

	if(hFind != INVALID_HANDLE_VALUE)
	{
		if((find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
			res = true;
		FindClose(hFind);
	}

	return res;
}

bool DelFilesByWildPath(const CString& strWildPath)
{
	bool res = false;

	CString strFolderPath,strDriver,strMainName,strExtName;
	if(SplitFilePath(strWildPath,strFolderPath,strDriver,strMainName,strExtName))
	{

		HANDLE hFind;
		WIN32_FIND_DATA datFind;

		hFind = FindFirstFile(strWildPath,&datFind);
		if(hFind != INVALID_HANDLE_VALUE)
		{
			res = true;

			CString strPathDel;
			do
			{
				if(!(datFind.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY))
				{
					strPathDel.Format(_T("%s\\%s"),strFolderPath,datFind.cFileName);
					if(!DeleteFile(strPathDel))
						res = false;
				}
			}
			while(FindNextFile(hFind,&datFind));
			FindClose(hFind);
		}

	}

	return res;
}

bool MoveFileByWildPath(const CString& strSrcPath,const CString& strDestFolder)
{
	bool res = false;


	CString strFolderPath,strDriver,strMainName,strExtName;
	if(SplitFilePath(strSrcPath,strFolderPath,strDriver,strMainName,strExtName))
	{
		HANDLE hFind;
		WIN32_FIND_DATA datFind;

		hFind = FindFirstFile(strSrcPath,&datFind);
		if(hFind != INVALID_HANDLE_VALUE)
		{
			res = true;

			CString strFilePath,strDestPath;
			do
			{
				if(!(datFind.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY))
				{
					strFilePath.Format(_T("%s\\%s"),strFolderPath,datFind.cFileName);
					strDestPath.Format(_T("%s\\%s"),strDestFolder,datFind.cFileName);
					
					if(!MoveFileEx(strFilePath,strDestPath,MOVEFILE_WRITE_THROUGH|MOVEFILE_REPLACE_EXISTING|MOVEFILE_COPY_ALLOWED))
						res = false;
				}
			}
			while(FindNextFile(hFind,&datFind));
			FindClose(hFind);
		}

	}

	return res;
}

bool CreateDirectoryRecuersively(const CString& strPath)
{
	bool res = false;
	
	CCopyArray<CString> aryPathItem = Str2StrAry(strPath,L"\\",false);
	CString strPathRecursive;
	int i;
	for(i=0;i<aryPathItem.GetCount();++i)
	{
		if(i == 0)
		{
			strPathRecursive = aryPathItem[0];
			continue;
		}
		else
			strPathRecursive += CString(L"\\") + aryPathItem[i];

		//HANDLE hDirHandle = CreateFile(strPathRecursive,
		//	GENERIC_READ,
		//	FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,
		//	NULL,
		//	OPEN_EXISTING,
		//	FILE_FLAG_BACKUP_SEMANTICS,
		//	NULL);
		//
		//DWORD dwErr = GetLastError();

		//if(hDirHandle != INVALID_HANDLE_VALUE)
		//{
		//	CloseHandle(hDirHandle);
		//	continue;
		//}
		//else
		//{
		//	if(!CreateDirectory(strPathRecursive,NULL))
		//		break;
		//}
		
		WIN32_FIND_DATA	find_data={0};
		HANDLE hFind =FindFirstFile(strPathRecursive,&find_data);
		bool bExist = false;

		if(hFind != INVALID_HANDLE_VALUE)
		{
			if(find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				bExist = true;
			FindClose(hFind);
		}
		if(!bExist)
		{
			if(!CreateDirectory(strPathRecursive,NULL))
				break;
		}
	}

	if(i == aryPathItem.GetCount())
		res = true;
	return res;
}

bool RemoveDirectoryRecuersively(const CString& strRoot)
{
	bool res = true;

	CString strPathChild;
	strPathChild.Format(_T("%s\\*"),strRoot);
	WIN32_FIND_DATA datFind;
	HANDLE hFind =FindFirstFile(strPathChild,&datFind);
	if(hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if(datFind.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if(CString(datFind.cFileName)!=_T(".") && CString(datFind.cFileName)!=_T(".."))
				{
					CString strChildFolder;
					strChildFolder.Format(_T("%s\\%s"),strRoot,datFind.cFileName);
					if(!RemoveDirectoryRecuersively(strChildFolder))
					{
						res = false;
						break;
					}

				}
			}
			else
			{
				CString strChildFile;
				strChildFile.Format(_T("%s\\%s"),strRoot,datFind.cFileName);
				if(!DeleteFile(strChildFile))
				{
					res = false;
					break;
				}
			}
		}
		while(FindNextFile(hFind,&datFind));

		FindClose(hFind);
	}


	if(res)
	{
		res = RemoveDirectory(strRoot);
	}

	return res;
}

bool IsFolder(const CString& strPath)
{
	bool res = false;

	SHFILEINFO fileInfo={0};
	if(SHGetFileInfo(strPath,0,&fileInfo,sizeof(fileInfo),SHGFI_ATTRIBUTES|SHGFI_DISPLAYNAME) != 0)//取文件信息
	{
		if(fileInfo.dwAttributes & SFGAO_FILESYSTEM)//判断是否是文件系统对象
		{
			if(fileInfo.dwAttributes & SFGAO_FOLDER)//判断是否是文件系统对象文件夹,排除zip文件
			{
				if(fileInfo.dwAttributes & SFGAO_STREAM) 
				{
					int nSysID = GetWindowsSystemID();
					if(nSysID == WINDOWS_XP || nSysID == WINDOWS_VISTA || nSysID == WINDOWS_2003)
					{
							res = false;//zip文件
					}
					else
					{
						res = true;
					}
				}
				else
				{
					res = true;
				}
			}
		}
	}

	return res;
}

CString GetExeFolderPath()
{
	CString res;

	HANDLE hProcess = GetCurrentProcess();

	CBuffer bufHMODULE(10*sizeof(HMODULE));
	HMODULE* pHMODULEs = (HMODULE*)bufHMODULE.GetBufP();
	DWORD dwNeed;
	if(EnumProcessModules(hProcess,pHMODULEs,bufHMODULE.GetBufLen(),&dwNeed))
	{
		if(dwNeed>bufHMODULE.GetBufLen())
		{
			bufHMODULE.Alloc(dwNeed);
			pHMODULEs = (HMODULE*)bufHMODULE.GetBufP();
			EnumProcessModules(hProcess,pHMODULEs,bufHMODULE.GetBufLen(),&dwNeed);
		}

		TCHAR szModulePath[MAX_PATH+1];
		for(int i=0;i<dwNeed/sizeof(sizeof(HMODULE));++i)
		{
			GetModuleFileName(pHMODULEs[i],szModulePath,MAX_PATH+1);
			CString strFolderPath,strDirver,strFileName,strExtName;
			SplitFilePath(szModulePath,strFolderPath,strDirver,strFileName,strExtName);
			strExtName.MakeLower();
			if(strExtName == _T("exe"))
			{
				res =strFolderPath;
			}
		}
	}


	return res;
}

bool TreeCtrl_IsChildItem(HTREEITEM hItem,HTREEITEM hParentItem,const CTreeCtrl& ctrlTree)
{
	bool res = false;

	if(hItem != NULL)
	{
		HTREEITEM hDirectParentItem = ctrlTree.GetParentItem(hItem);
		if(hDirectParentItem == NULL)
			res = false;
		if(hDirectParentItem == hParentItem)
			res = true;
		else
			res = TreeCtrl_IsChildItem(hDirectParentItem,hParentItem,ctrlTree);
	}

	return res;
}
CCopyArray<HTREEITEM>  TreeCtrl_FindItemByTxt(const CString& strItemTxt,const CTreeCtrl& ctrlTree,HTREEITEM hItem)
{
	CCopyArray<HTREEITEM> res ;
	if(hItem == NULL)
		hItem = ctrlTree.GetRootItem();
	if(hItem != NULL && strItemTxt != "")
	{
		if(ctrlTree.GetItemText(hItem) == strItemTxt)
		{
			res.Add(hItem);
		}
		else
		{
			HTREEITEM hChildItem; 
			for(hChildItem = ctrlTree.GetChildItem(hItem);hChildItem != NULL; hChildItem = ctrlTree.GetNextSiblingItem(hChildItem))
			{
				CCopyArray<HTREEITEM> child_res = TreeCtrl_FindItemByTxt(strItemTxt,ctrlTree,hChildItem);
				for(int i = 0 ;i < child_res.GetSize(); ++i)
				{
					res.Add(child_res[i]);
				}
			}
		}
	}
	return res;
}

void TreeCtrl_ExpandItemPath(CTreeCtrl& ctrlTree,HTREEITEM hItem)
{
	HTREEITEM hParentItem = ctrlTree.GetParentItem(hItem);
	if(hParentItem != NULL)
	{
		ctrlTree.Expand(hParentItem,TVE_EXPAND);
		TreeCtrl_ExpandItemPath(ctrlTree,hParentItem);
	}
}

void TreeCtrl_CollapseTree(CTreeCtrl& ctrlTree,HTREEITEM hItem)
{
	for(HTREEITEM hChild=ctrlTree.GetChildItem(hItem);hChild!=NULL;hChild=ctrlTree.GetNextSiblingItem(hChild))
	{
		TreeCtrl_CollapseTree(ctrlTree,hChild);
	}
	ctrlTree.Expand(hItem,TVE_COLLAPSE);
}


int Compare_Str(const CString& str1,const CString& str2)
{
	//return str1.Compare(str2);
	//按照本地内码比较,拼音排序
	CBuffer bufStr1(str1.GetLength()*4+2),bufStr2(str2.GetLength()*4+2);

	int nMultiByte = WideCharToMultiByte(CP_ACP,0,(LPCWSTR)str1,str1.GetLength(),(LPSTR)bufStr1.GetBufP(),bufStr1.GetBufLen(),NULL,NULL);
	bufStr1.SetDataLen(nMultiByte);

	nMultiByte = WideCharToMultiByte(CP_ACP,0,(LPCWSTR)str2,str2.GetLength(),(LPSTR)bufStr2.GetBufP(),bufStr2.GetBufLen(),NULL,NULL);
	bufStr2.SetDataLen(nMultiByte);

	return strcmp((LPCSTR)bufStr1.GetBufP(),(LPCSTR)bufStr2.GetBufP());


}
int CompareInt(const int& A,const int& B)
{
	int res;

	res = A - B;
	if(res!=0)
		res = res/abs(res);

	return res;
}

//CCopyArray<CString> Str2StrAry(const CString& strStr,const CString& strDelimiter,bool bSort)
//{
//	CCopyArray<CString>  res;
//	int i = 0;
//
//	if(bSort)
//	{
//		for(CString strTok = strStr.Tokenize(strDelimiter,i);i>=0;strTok = strStr.Tokenize(strDelimiter,i))
//		InsertToOrderArray(res,strTok,Compare_Str);
//	}
//	else
//	{
//		for(CString strTok = strStr.Tokenize(strDelimiter,i);i>=0;strTok = strStr.Tokenize(strDelimiter,i))
//			res.Add(strTok);
//	}
//	return res;
//}

CCopyArray<CString> Str2StrAry(const CString& strStr,const CString& strDelimiter,bool bSort)
{
	CCopyArray<CString>  res;
	int i = 0;

	if(strStr.GetLength()>0 && strDelimiter.GetLength()>0)
	{
		if(bSort)
		{
			for(CString strTok = strStr.Tokenize(strDelimiter,i);i>=0;strTok = strStr.Tokenize(strDelimiter,i))
			InsertToOrderArray(res,strTok,Compare_Str);
		}
		else
		{
			int iOld =0;
			for(int i = strStr.Find(strDelimiter,0);i>=0;i = strStr.Find(strDelimiter,i+1))
			{
				res.Add(strStr.Mid(iOld,i-iOld));
				iOld = i + strDelimiter.GetLength();
			}
			res.Add(strStr.Mid(iOld));
		}
	}

	return res;
}


CString StrAry2Str(const CArray<CString>& aryStr,const CString& strDelimiter)
{
	CString res;
	for(int i= 0;i < aryStr.GetCount();++i)
	{
		if(i == 0)
			res = aryStr[0];
		else
			res += strDelimiter + aryStr[i];
	}
	return res;
}


int FindItemPropByNameInList(const CString& strPropName,const CArray<CString>& aryPropList)
{
	int res = -1;
	
	int iCount = aryPropList.GetCount();
	for(int i=0;i<iCount;++i)
	{
		CCopyArray<CString> aryPiceOfProp = Str2StrAry(aryPropList[i],L";",false);
		if(aryPiceOfProp.GetCount()>0 && aryPiceOfProp[0] == strPropName)
		{
			res = i;
			break;
		}

	}
	
	return  res;
}

int CompareItemProp(const CString& strValue1,const CString& strValue2,const CString& strType)
{
	int res = 0;

	if(strType == L"NUMBER")
	{
		double dbValue1 =_wtof(strValue1);
		double dbValue2 =_wtof(strValue2);

		if(dbValue1 == dbValue2)
			res = 0;
		else if(dbValue1>dbValue2)
			res = 1;
		else
			res = -1;
	}
	else if(strType == L"STRING")
	{
		res = strValue1.Compare(strValue2);
	}

	return res;
}

CCopyArray<CString> CombineSetItemPropList(const CArray<CString>& aryItemPropList1,const CArray<CString>& aryItemPropList2,bool& bHasConflict)
{
	CCopyArray<CString> res;

	bHasConflict = false;
	res.Copy(aryItemPropList1);

	int iCount = aryItemPropList2.GetCount();
	for(int i=0;i<iCount;++i)
	{
		CCopyArray<CString> aryPicePropList2 = Str2StrAry(aryItemPropList2[i],L";",false);
		int iIndex = FindItemPropByNameInList(aryPicePropList2[0],res);
		if(iIndex<0)
		{
			res.Add(aryItemPropList2[i]);
		}
		else
		{
			CCopyArray<CString> aryPiceResProp = Str2StrAry(res[iIndex],L";",false);

			if(aryPiceResProp[1] != aryPicePropList2[1])
				bHasConflict = true;
		}
	}

	return res;
}

CCopyArray<CString> CommonItemPropList(const CArray<CString>& aryItemPropList1,const CArray<CString>& aryItemPropList2)
{
	CCopyArray<CString> res;

	res.Copy(aryItemPropList1);

	int iCount = res.GetCount();
	for(int i=iCount-1;i>=0;--i)
	{
		CCopyArray<CString> aryPicePropRes = Str2StrAry(res[i],L";",false);
		int iIndex = FindItemPropByNameInList(aryPicePropRes[0],aryItemPropList2);
		if(iIndex<0)
		{
			res.RemoveAt(i);
		}
		else
		{
			CCopyArray<CString> aryPiceList2Prop = Str2StrAry(aryItemPropList2[iIndex],L";",false);

			if(aryPicePropRes[1] != L"~不相同~")
			if(aryPicePropRes[1] != aryPiceList2Prop[1])
			{
				aryPicePropRes[1] = L"~不相同~";
				res.RemoveAt(i);
				res.InsertAt(i,StrAry2Str(aryPicePropRes));
			}
		}
	}

	return res;
}
CCopyArray<CString> DiffSetItemPropList(const CArray<CString>& aryItemPropList1,const CArray<CString>& aryItemPropList2)
{
	CCopyArray<CString> res;

	res.Copy(aryItemPropList1);

	int iCount = aryItemPropList2.GetCount();
	for(int i=0;i<iCount;++i)
	{
		CCopyArray<CString> aryPicePropList2 = Str2StrAry(aryItemPropList2[i],L";",false);
		int iIndex = FindItemPropByNameInList(aryPicePropList2[0],res);
		if(iIndex>=0)
		{
			CCopyArray<CString> aryPiceOfResProp = Str2StrAry(res[iIndex],L";",false);
			if(aryPicePropList2[1] == aryPiceOfResProp[1])//类型相同则认为是同一个属性
				res.RemoveAt(iIndex);
		}
	}
	return res;
}


int GetWndFontWidth(CWnd* pWnd)
{
	int res = 0;

	CPaintDC DCWnd(pWnd);
	TEXTMETRIC txtinfo={0};
	DCWnd.GetTextMetrics(&txtinfo);
	res = txtinfo.tmAveCharWidth;

	if(res ==0)
		res = 7;

	return res;
}
bool InRect(int x,int y,RECT rect)
{
	bool res = false;
	if(x>=rect.left && x<=rect.right && y>=rect.top && y<= rect.bottom)
		res = true;

	return res;
}
void GetWndFontSize(CWnd* pWnd,SIZE& szFont)
{
	CPaintDC DCWnd(pWnd);
	TEXTMETRIC txtinfo={0};
	DCWnd.GetTextMetrics(&txtinfo);

	szFont.cx = txtinfo.tmAveCharWidth;
	szFont.cy = txtinfo.tmHeight;
}

bool NameNoInValidChar(const CString& strName)
{
	bool res =true;
	
	if(strName.FindOneOf(L"<>/\\|;:\"?.+-() ^")>=0)
		res = false;

	return res;
}

int GetWindowsSystemID()
{
	int res = WINDOWS_UNKNOW;

	OSVERSIONINFO ver_info={0};
	ver_info.dwOSVersionInfoSize = sizeof(ver_info);
	if(GetVersionEx(&ver_info))
	{
		switch(ver_info.dwPlatformId)
		{
		case VER_PLATFORM_WIN32_NT:
			if(ver_info.dwMajorVersion == 4)
			{
				res = WINDOWS_NT4;
			}
			else if(ver_info.dwMajorVersion == 5)
			{
				if(ver_info.dwMinorVersion == 0)
					res = WINDOWS_2000;
				else if(ver_info.dwMinorVersion == 1)
					res= WINDOWS_XP;
				else if(ver_info.dwMinorVersion == 2)
					res = WINDOWS_2003;
					
			}
			else if(ver_info.dwMajorVersion == 6)
			{
				res = WINDOWS_VISTA;
			}
			break;
		case VER_PLATFORM_WIN32_WINDOWS:
			if(ver_info.dwMajorVersion == 4)
			{
				if(ver_info.dwMinorVersion == 0)
					res = WINDOWS_95;
				else if(ver_info.dwMinorVersion == 10)
					res = WINDOWS_98;
				else if(ver_info.dwMinorVersion == 90)
					res = WINDOWS_ME;
			}
			break;
		case VER_PLATFORM_WIN32s:
			res = WINDOWS_31;
			break;
		}
	}

	return res;
}

HTREEITEM FindSetNodeByNameInTree(const CTreeCtrl* pTree,const CString& strSetName,HTREEITEM hStartItem,bool& bStart,HTREEITEM hRootItem)
{
	HTREEITEM res = NULL;

	if(hRootItem != NULL)
	{
		if(!bStart)
		{
			if(hStartItem == hRootItem)
				bStart = true;
		}
		else
		{
			CString strItemName = pTree->GetItemText(hRootItem);
			strItemName.MakeLower();
			if(strItemName.Find(strSetName)>=0)
				res = hRootItem;
		}

		if(res == NULL)
		{
			res = FindSetNodeByNameInTree(pTree,strSetName,hStartItem,bStart,pTree->GetChildItem(hRootItem));
		}

		if(res == NULL)
		{
			res = FindSetNodeByNameInTree(pTree,strSetName,hStartItem,bStart,pTree->GetNextSiblingItem(hRootItem));
		}

	}

	return res;

}

void WriteLog(const CString& strLog)
{
//#ifdef _DEBUG
	CString strWrite = strLog+L"\r\n";
	HANDLE hOutFile = CreateFile(L"C:\\Log\\FSS.log",GENERIC_WRITE,\
			0,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
		DWORD dwError = GetLastError();
		if(INVALID_HANDLE_VALUE != hOutFile)
		{
			SetFilePointer(hOutFile,0,NULL,FILE_END);
			//Uincode 转本地码
			CBuffer bufANSI;
			if(bufANSI.Alloc(strWrite.GetLength()*4+2))
			{
				int nANSI_Len = WideCharToMultiByte(CP_ACP,0,(LPCTSTR)strWrite,strWrite.GetLength(),(LPSTR)bufANSI.GetBufP(),bufANSI.GetBufLen(),NULL,NULL);
				if(nANSI_Len>0)
				{
					bufANSI.SetDataLen(nANSI_Len);
					DWORD dwWriteLen;
					if(!WriteFile(hOutFile,bufANSI.GetBufP(),bufANSI.GetDataLen(),&dwWriteLen,NULL))
					{
					}
				}
			}
			CloseHandle(hOutFile);
		}
//#endif
}

bool CopyXmlTreeNodes(MSXML2::IXMLDOMDocumentPtr& pDestXmlDoc,MSXML2::IXMLDOMElementPtr& pDestParentNode,MSXML2::IXMLDOMNodeListPtr& pSrcNodes)
{
	bool res = false;
	try
	{
		for(int i=0;i<pSrcNodes->length;++i)
		{
			MSXML2::IXMLDOMElementPtr pSrcNode = pSrcNodes->item[i];
			if(pSrcNode != NULL)
			{
				//CString strNodeName = pSrcNode->nodeName;
				//CString strNodeText = pSrcNode->text;

				MSXML2::IXMLDOMElementPtr pNodeCpy = pDestXmlDoc->createElement(pSrcNode->nodeName);
				if(pNodeCpy != NULL)
				{
					pDestParentNode->appendChild(pNodeCpy);

					//拷贝属性
					MSXML2::IXMLDOMNamedNodeMapPtr pAttrList = pSrcNode->attributes;
					for(int j=0;j<pAttrList->length;++j)
					{
						CString strAttrName = pAttrList->item[j]->nodeName;
						CString strAttrVaule = pAttrList->item[j]->nodeValue;

						pNodeCpy->setAttribute(pAttrList->item[j]->nodeName,pAttrList->item[j]->nodeValue);
					}
					
					MSXML2::IXMLDOMNodeListPtr pChildList = pSrcNode->selectNodes(_T("./*"));
					if(pChildList->length > 0)
					{
						res = CopyXmlTreeNodes(pDestXmlDoc,pNodeCpy,pChildList);//拷贝子树
					}
					else
					{
						pNodeCpy->text = pSrcNode->text;//拷贝节点值
						res = true;
					}
				}
			}
		}
	}
	catch(...)
	{
	}
	return res;
}



//格式化文件大小字符串
CString BuildFileSizeStr(DWORD dwLow,DWORD dwHi)
{
	CString res;

	if(dwHi==0)
	{
		if(dwLow<1024)
			res.Format(L"%u B",dwLow);
		else if(dwLow<1024*1024)
			res.Format(L"%.1f KB",dwLow/1024.0);
		else if(dwLow<1024*1024*1024)
			res.Format(L"%.1f MB",dwLow/(1024.0*1024.0));
		else
			res.Format(L"%.1f GB",dwLow/(1024.0*1024.0*1024.0));
	}
	else
	{
		res.Format(L"%.1f GB",dwHi*4+dwLow/(1024.0*1024.0*1024.0));
	}

	return res;
}



CString FilePath2Url(const CString& strFilePath)
{
	CString res = strFilePath;
	
	res.Replace(_T("\\"),_T("/"));
	res.Replace(_T(" "),_T("%20"));
	res = _T("file:///") + res;

	return res;
}

CString UrlEscapeEncode(const CString& strSrcCode)
{
	CString res;

	CBuffer bufUtf8(strSrcCode.GetLength()*4+2);
	int nToLen = WideCharToMultiByte(CP_UTF8,0,(LPCTSTR)strSrcCode,strSrcCode.GetLength(),(LPSTR)bufUtf8.GetBufP(),bufUtf8.GetBufLen(),NULL,NULL);
	bufUtf8.SetDataLen(nToLen);

	unsigned  char *pChar = bufUtf8.GetBufP();
	for(int i=0;i<bufUtf8.GetDataLen();++i)
	{
		if(pChar[i]>=32 && pChar[i]<=126  
			&& pChar[i]!='/' && pChar[i]!=':' && pChar[i]!=';' && pChar[i]!='#' 
			&& pChar[i]!='?' && pChar[i]!='%' && pChar[i]!='+' && pChar[i]!='-' 
			&& pChar[i]!='@'  && pChar[i]!='=' && pChar[i]!='$' && pChar[i]!='&' &&  pChar[i]!='\"' && pChar[i]!='\'')
		{
			res.AppendChar(pChar[i]);
		}
		else
		{
			CString strEncode;
			strEncode.Format(_T("%%%02X"),pChar[i]);

			res.Append(strEncode);
		}
	}

	return res;
}


CString UrlEscapeDecode(const CString& strEnCode)
{
	CString res;

	CBuffer bufUtf8(strEnCode.GetLength()+2);
	int nCharCount = 0;
	for(int i=0;i<strEnCode.GetLength();++i)
	{
		unsigned char cOne;
		if(strEnCode.GetAt(i) == L'%')
		{
			WCHAR* pEndChar;
			cOne = wcstoul(strEnCode.Mid(i+1,2),&pEndChar,16);
			i += 2;
		}
		else
		{
			cOne = strEnCode.GetAt(i);
		}
		++nCharCount;
		bufUtf8.AppendData(&cOne,1);
	}

	CBuffer bufUtf16(nCharCount*2+2);
	MultiByteToWideChar(CP_UTF8,0,(LPCSTR)bufUtf8.GetBufP(),nCharCount,(LPWSTR)bufUtf16.GetBufP(),bufUtf16.GetBufLen()/2);
	res = (LPWSTR)bufUtf16.GetBufP();

	return res;
}

bool SplitUrl(const CString& strUrl,CString& strProtocol,CString& strHost,CString& strPath,CString& strFileName,CString& strExtName,CArray<CString>* pAryParam)
{
	bool res = false;

	CCopyArray<CString> aryUrlAndParam = Str2StrAry(strUrl,_T("?"),false);


	CCopyArray<CString> aryPiceofUrl = Str2StrAry(aryUrlAndParam[0],_T("://"),false);
	if(aryPiceofUrl.GetCount() == 2)
	{
		strProtocol = aryPiceofUrl[0];
		strProtocol.MakeLower();
		if(strProtocol == _T("file"))
		{
			strPath = aryPiceofUrl[1].Mid(1);
			strPath.Replace(_T("/"),_T("\\"));
			strPath.Replace(_T("%20"),_T(" "));
			res = true;

		}
		else
		{
			CCopyArray<CString> aryPiceofUrl2 = Str2StrAry(aryPiceofUrl[1],_T("/"),false);
			if(aryPiceofUrl2.GetCount()>=1)
			{
				strHost = aryPiceofUrl2[0];
				aryPiceofUrl2.RemoveAt(0);
				strPath = CString(_T("/")) + StrAry2Str(aryPiceofUrl2,_T("/"));
				res = true;
			}
		}

		CCopyArray<CString> aryPiceofUrl3 = Str2StrAry(aryPiceofUrl[1],_T("/"),false);
		if(aryPiceofUrl3.GetCount()>=2)
		{
			CCopyArray<CString> aryPiceofUrl4 = Str2StrAry(aryPiceofUrl3.GetAt(aryPiceofUrl3.GetCount()-1),_T("?"),false);
			if(aryPiceofUrl4.GetCount()>=1)
			{
				CCopyArray<CString> aryPiceofUrl5 = Str2StrAry(aryPiceofUrl4[0],_T("."),false);
				//if(aryPiceofUrl5.GetCount()>=2)
				{
					strFileName = aryPiceofUrl4[0];
					strFileName.Replace(_T("%20"),_T(" "));
					if(aryPiceofUrl5.GetCount()>=2)
						strExtName = aryPiceofUrl5.GetAt(aryPiceofUrl5.GetCount()-1);
				}
			}
		}
	}

	if(res && pAryParam!=NULL && aryUrlAndParam.GetCount()>1)
	{
		CCopyArray<CString> aryParam = Str2StrAry(aryUrlAndParam[1],_T("&"),false);
		pAryParam->Copy(aryParam);
	}

	return res;
}

DWORD FindProcess(const CString& strExeName)
{
	DWORD res = 0;
	HANDLE  hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if(INVALID_HANDLE_VALUE!=hSnapshot)
	{
		PROCESSENTRY32  ProcessEntry = {0};
		ProcessEntry.dwSize = sizeof(ProcessEntry);
		for(BOOL bSuc = Process32First(hSnapshot,&ProcessEntry);bSuc;bSuc = Process32Next(hSnapshot,&ProcessEntry))
		{
			if(strExeName.CompareNoCase(ProcessEntry.szExeFile) == 0)
			{
				res = ProcessEntry.th32ProcessID;
				break;
			}
		}

		DWORD err = GetLastError();
		CloseHandle(hSnapshot);
	}
	return res;
}

bool KillProcess(DWORD dwProcessID)
{
	bool res = false;
	if(dwProcessID != 0)
	{
		HANDLE hProcess = OpenProcess(PROCESS_TERMINATE,FALSE,dwProcessID);
		if(hProcess != INVALID_HANDLE_VALUE)
		{
			TerminateProcess(hProcess,0);
			CloseHandle(hProcess);
		}
	}
	return res;
}
bool KillProcess(const CString& strExeName)
{
	bool res = false;
	DWORD dwProcessID = FindProcess(strExeName);
	res = KillProcess(dwProcessID);
	return res;
}

int FindMatchBracket(const CString& strStr,TCHAR tcBracket,int iPos)
{
	int res = -1;

	TCHAR tcMatchBracket = 0;
	switch(tcBracket)
	{
	case _T('('):
		tcMatchBracket = _T(')');
		break;
	case _T('\"'):
		tcMatchBracket = _T('\"');
		break;
	case _T('{'):
		tcMatchBracket = _T('}');

	}
	std::stack<TCHAR> stackBraket;
	
	if(tcMatchBracket!=0 && iPos>=0 && strStr.GetAt(0)==tcBracket)
	{
		int iBraketCount = 0;
		LPCTSTR wzStr = (LPCTSTR)strStr;
		for(int i = iPos+1;i<strStr.GetLength();++i)
		{
			if(wzStr[i] == tcBracket)
			{
				++iBraketCount;
			}
			else if(wzStr[i] == tcMatchBracket)
			{
				--iBraketCount;
				if(iBraketCount<0)
				{
					res = i;
					break;
				}
			}
		}
	}

	return res;
}

SYSTEMTIME OffsetDate(const SYSTEMTIME& strBaseDate,int nOffsetDay)
{
	SYSTEMTIME res = strBaseDate;
	
	//循环
	while(nOffsetDay != 0)
	{
		if(nOffsetDay>0)
		{
			if(res.wMonth == 12)//年边界
			{
				if(nOffsetDay+res.wDay <= 31)
				{
					res.wDay += nOffsetDay;
					nOffsetDay = 0;
				}
				else
				{
					++res.wYear;
					res.wMonth = 1;
					nOffsetDay -= (32 - res.wDay); 
					res.wDay = 1;
					
				}
			}
			else
			{
				int nUpper;
				if(res.wMonth == 2)
				{
					if((res.wYear%4==0 && res.wYear%100!=0) || (res.wYear%400==0))
						nUpper = 30;
					else
						nUpper = 28;
				}
				else if(res.wMonth == 1 || res.wMonth == 3 || res.wMonth == 5 || res.wMonth == 7 || res.wMonth == 8 || res.wMonth == 10)
				{
					nUpper = 31;
				}
				else
				{
					nUpper = 30;
				}

				if(res.wDay + nOffsetDay<=nUpper)
				{
					res.wDay += nOffsetDay;
					nOffsetDay = 0;
				}
				else
				{
					++res.wMonth;
					nOffsetDay -= nUpper+1 - res.wDay;
					res.wDay = 1;
				}

			}

		}
		else
		{
			if(nOffsetDay>res.wDay)//没有跨过月边界
			{
				res.wDay += nOffsetDay;
				nOffsetDay = 0;
			}
			else
			{
				//新偏移量
				nOffsetDay += res.wDay;
				//新日期
				switch(res.wMonth)
				{
				case 1://年边界
					--res.wYear;
					res.wMonth = 12;
					res.wDay = 31;
					break;
				case 3://闰年月判断
					--res.wMonth;
					if((res.wYear%4==0 && res.wYear%100!=0) || (res.wYear%400==0))
					{
						res.wDay = 30;
					}
					else
					{
						res.wDay = 28;
					}
					break;
				case 2:
				case 4:
				case 6:
				case 8:
				case 9:
				case 11://前面是大月
					--res.wMonth;
					res.wDay = 31;
					break;
				default://前面是小月
					--res.wMonth;
					res.wDay = 30;
				}
			}
		}
	}


	return res;
}

int CompareDate(const SYSTEMTIME& A,const SYSTEMTIME& B)
{
	int res = 0;

	int d = ((A.wYear - B.wYear)*12 + (A.wMonth - B.wMonth))*31 +(A.wDay-B.wDay);
	if(d>0)
		res = 1;
	else if(d<0)
		res = -1;
	else
	{
		int dd = (((A.wHour - B.wHour)*60 + (A.wMinute - B.wMinute))*60 + (A.wSecond -B.wSecond))*1000 + (A.wMilliseconds - B.wMilliseconds);
		if(dd>0)
			res = 1;
		else if(dd<0)
			res = -1;
		else
			res = 0;
	}

	return res;
}

//CString GetLnkTypeItemDes(const CString& strItemDesUrl)
//{
//	CString res;
//
//	HINTERNET hRoot = InternetOpen(_T("Internet Explorer"),INTERNET_OPEN_TYPE_DIRECT,NULL,NULL,0);
//	if( hRoot != NULL)
//	{
//
//		HINTERNET hFile = InternetOpenUrl(hRoot,strItemDesUrl,NULL,0,INTERNET_FLAG_RELOAD,NULL);
//		if(hFile != NULL)
//		{
//
//			CBuffer bufData(160*1024);
//
//			DWORD dwRead = 0;
//			while(InternetReadFile(hFile,bufData.GetBufP()+bufData.GetDataLen(),16*1024,&dwRead))
//			{
//				if(dwRead==0)
//				{
//					//UTF8转UTF16
//					CBuffer bufUTF16(bufData.GetDataLen()*2+2);
//					MultiByteToWideChar(CP_UTF8,0,(LPSTR)bufData.GetBufP(),bufData.GetDataLen(),(LPWSTR)bufUTF16.GetBufP(),bufUTF16.GetBufLen()/2);
//					try
//					{
//						MSXML2::IXMLDOMDocumentPtr pLnkTypeItemDesXmlDoc;
//						pLnkTypeItemDesXmlDoc.CreateInstance(__uuidof(MSXML2::FreeThreadedDOMDocument));
//						pLnkTypeItemDesXmlDoc->loadXML((LPWSTR)bufUTF16.GetBufP());
//						res = (LPCTSTR)pLnkTypeItemDesXmlDoc->selectSingleNode(_T("description"))->text;
//					}
//					catch(COleException* expOle)
//					{
//						expOle->Delete();
//					}
//					catch(...)
//					{
//					}
//
//					break;
//				}
//
//				bufData.SetDataLen(bufData.GetDataLen()+dwRead);
//
//				if(bufData.GetDataLen()+16*1024>bufData.GetBufLen())
//				{
//					bufData.ReAlloc(bufData.GetBufLen()+160*1024);
//				}
//			}
//
//			InternetCloseHandle(hFile);
//		}
//
//		InternetCloseHandle(hRoot);
//	}
//
//	return res;
//}

UINT CStringToUINT(CString strSrc)
{
	UINT res;

	if(strSrc.Find(_T("0x"))==0)
		_stscanf_s((LPCTSTR)strSrc,_T("%x"),&res);
	else
		_stscanf_s((LPCTSTR)strSrc,_T("%d"),&res);

	return res;
}