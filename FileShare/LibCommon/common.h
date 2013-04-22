#pragma once
#include "Buffer.h"
//class CBuffer;
#import "msxml2.dll"

#define SAFE_DELETE(p) if(p != NULL) {\
						delete p;\
						p = NULL;\
					}

#define SAFE_RELEASE(pInterface) if(pInterface != NULL) {\
						 pInterface->Release();\
						pInterface = NULL;\
					}


template<class TYPE>
class CCopyArray: public CArray<TYPE>
{
public:
	CCopyArray():CArray()
	{
	}
	CCopyArray(const CCopyArray& src)
	{
		this->Copy(src);
	}
	CCopyArray& operator = (const CCopyArray& src)
	{
		this->Copy(src);
		return *this;
	}
};

//日期函数
SYSTEMTIME OffsetDate(const SYSTEMTIME& strBaseDate,int nOffsetDay);//推算偏移后的日期
int CompareDate(const SYSTEMTIME& A,const SYSTEMTIME& B);//比较日期先后

//日志函数
void WriteLog(const CString& strLog);

//系统进程管理函数
DWORD FindProcess(const CString& strExeName);
bool KillProcess(DWORD dwProcessID);
bool KillProcess(const CString& strExeName);


//文件系统有关函数
bool SplitFilePath(const CString& strFullPathName,CString& strFolderPath,CString& strDriver,CString& strFName,CString& strExt);
bool CreateDirectoryRecuersively(const CString& strPath);
bool RemoveDirectoryRecuersively(const CString& strRoot);
bool DelFilesByWildPath(const CString& strWildPath);
bool MoveFileByWildPath(const CString& strSrcPath,const CString& strDestFolder);
CString GetCommonPath(const CString& strPath1,const CString& strPath2);
bool IsFileExist(const CString& strPath);
bool IsFolder(const CString& strPath);

//获取执行文件路径
CString GetExeFolderPath();

//URL处理函数
CString FilePath2Url(const CString& strFilePath);
bool SplitUrl(const CString& strUrl,CString& strProtocol,CString& strHost,CString& strPath,CString& strFileName,CString& strExtName,CArray<CString>* pAryParam=NULL);
CString UrlEscapeEncode(const CString& strSrcCode);
CString UrlEscapeDecode(const CString& strEnCode);

//格式化文件大小字符串
CString BuildFileSizeStr(DWORD dwLow,DWORD dwHi);

//引号括号匹配
int FindMatchBracket(const CString& strStr,TCHAR tcBracket,int iPos); 

//树控件函数
CCopyArray<HTREEITEM>  TreeCtrl_FindItemByTxt(const CString& strItemTxt,const CTreeCtrl& ctrlTree,HTREEITEM hItem = NULL);
bool TreeCtrl_IsChildItem(HTREEITEM hItem,HTREEITEM hParentItem,const CTreeCtrl& ctrlTree);
void TreeCtrl_ExpandItemPath(CTreeCtrl& ctrlTree,HTREEITEM hItem);
void TreeCtrl_CollapseTree(CTreeCtrl& ctrlTree,HTREEITEM hItem);

//获取链接方式的项目摘要
//CString GetLnkTypeItemDes(const CString& strItemDesUrl);


//void InsertStringToOrderArray(CArray<CString>& aryStr,const CString& strInsert);
//int FindStringInOrderArray(CArray<CString>& aryStr,const CString& strFind);

//template<class TYPE>
//void DelitemInOrderArray(CArray<TYPE>& aryStr,const TYPE& strInsert,int(*Compare)(const TYPE& a,const TYPE& b))
//{
//	int index = FindInOrderArray(aryStr,strInsert,Compare);
//	if(index >= 0 && Compare(aryStr[index],strInsert) == 0)
//		aryStr.RemoveAt(index);
//}

template<class TYPE>
void RemoveFromOrderArray(CArray<TYPE>& aryStr,const TYPE& strInsert,int(*Compare)(const TYPE& a,const TYPE& b),int nStart = 0,int nEnd = 0xfffffff)
{
	int index = FindInOrderArray(aryStr,strInsert,Compare,nStart,nEnd);
	if(index >= nStart && Compare(aryStr[index],strInsert) == 0)
		aryStr.RemoveAt(index);
}


/******************/
//2010.10.22增加bRepeatKey参数用来表示是否允许键值重复的记录插入列表
/******************/
template<class TYPE>
int InsertToOrderArray(CArray<TYPE>& aryStr,const TYPE& strInsert,int(*Compare)(const TYPE& a,const TYPE& b),int nStart = 0,int nEnd = 0xfffffff,bool bRepeatKey = false)
{
	int res = nStart-1;
	int index = FindInOrderArray(aryStr,strInsert,Compare,nStart,nEnd);
	if(index < nStart)
	{
		aryStr.InsertAt(index+1, strInsert);
	}
	else if(Compare(aryStr[index],strInsert) != 0)
	{
		aryStr.InsertAt(index+1, strInsert);
	}
	else if(bRepeatKey)
	{
		aryStr.InsertAt(index+1, strInsert);
	}
	res = index+1;

	return res;
}

template<class TYPE>
int FindInOrderArray(CArray<TYPE>& aryStr,const TYPE& strFind,int(*Compare)(const TYPE& a,const TYPE& b),int nStart = 0,int nEnd = 0xfffffff)
{
	int res = nStart-1;

	int iHead = nStart;
	int iTail = nEnd<=aryStr.GetSize()-1?nEnd:aryStr.GetSize()-1;

	if(iTail - iHead<0)
	{
		//res = -1; 
	}
	else if(Compare(strFind ,aryStr[iHead]) < 0 )
	{
		//res =iHead -1;
	}
	else if (Compare(strFind,aryStr[iTail])>=0)
	{
		res = iTail;
	}
	else
	{
		while(iHead<(iTail-1))
		{
			int i = (iHead+iTail)/2;
			int nCompareMid = Compare(strFind ,aryStr[i]);
			if( nCompareMid == 0 )
			{
				res = i;
				break;
			}
			else if(nCompareMid < 0)
			{
				iTail = i;
			}
			else //if(nCompareMid > 0)
			{
				iHead = i;
			}
		}
		if(res < nStart)
		res = iHead;
	}

	return res;

}

template<class TYPE>
int FindInOrderArrayWithParam(CArray<TYPE>& aryStr,const TYPE& strFind,int(*Compare)(const TYPE& a,const TYPE& b,void* pParam),void* pParam = NULL,int nStart = 0,int nEnd = 0xfffffff)
{
	int res = nStart-1;

	int iHead = nStart;
	int iTail = nEnd<=aryStr.GetSize()-1?nEnd:aryStr.GetSize()-1;

	if(iTail - iHead<0)
	{
		//res = -1; 
	}
	else if(Compare(strFind ,aryStr[iHead],pParam) < 0 )
	{
		//res =iHead -1;
	}
	else if (Compare(strFind,aryStr[iTail],pParam)>=0)
	{
		res = iTail;
	}
	else
	{
		while(iHead<(iTail-1))
		{
			int i = (iHead+iTail)/2;
			int nCompareMid = Compare(strFind ,aryStr[i],pParam);
			if( nCompareMid == 0 )
			{
				res = i;
				break;
			}
			else if(nCompareMid < 0)
			{
				iTail = i;
			}
			else //if(nCompareMid > 0)
			{
				iHead = i;
			}
		}
		if(res < nStart)
		res = iHead;
	}

	return res;

}
template<class TYPE>
int InsertToOrderArrayWithParam(CArray<TYPE>& aryStr,const TYPE& strInsert,int(*Compare)(const TYPE& a,const TYPE& b,void* pParam),void* pParam = NULL,int nStart = 0,int nEnd = 0xfffffff,bool bRepeatKey = false)
{
	int res = nStart-1;
	int index = FindInOrderArrayWithParam(aryStr,strInsert,Compare,pParam,nStart,nEnd);
	if(index < nStart)
	{
		aryStr.InsertAt(index+1, strInsert);
	}
	else if(Compare(aryStr[index],strInsert,pParam) != 0)
	{
		aryStr.InsertAt(index+1, strInsert);
	}
	else if(bRepeatKey)
	{
		aryStr.InsertAt(index+1, strInsert);
	}
	res = index+1;

	return res;
}
int Compare_Str(const CString& str1,const CString& str2);
int CompareInt(const int& A,const int& B);

template<class TYPE>
int FindInNoOrderArray(CArray<TYPE>& aryStr,const TYPE& strFind,int(*Compare)(const TYPE& a,const TYPE& b),int nStart = 0)
{
	int res = nStart-1;

	int iCount = aryStr.GetCount();
	for(int i=nStart;i<iCount;++i)
	{
		if(Compare(strFind ,aryStr[i]) == 0 )
		{
			res = i;
			break;
		}

	}
	return res;
}
template<class TYPE>
void RemoveFromNoOrderArray(CArray<TYPE>& aryStr,const TYPE& strInsert,int(*Compare)(const TYPE& a,const TYPE& b))
{
	int index = FindInNoOrderArray(aryStr,strInsert,Compare);
	if(index >= 0 )
		aryStr.RemoveAt(index);
}
template<class TYPE>
CCopyArray<TYPE> CombineOrderArray(CArray<TYPE>& A,CArray<TYPE>& B,int(*Compare)(const TYPE& a,const TYPE& b))
{
	CCopyArray<TYPE> res;

	//提高内存分配性能
	res.SetSize(0,A.GetCount()+B.GetCount());

	if(A.GetCount()>=B.GetCount())
	{
		res.Copy(A);
		int nCount = B.GetCount();
		int nInsertStart = 0;
		for(int i=0;i<nCount;++i)
		{
			nInsertStart = InsertToOrderArray(res,B[i],Compare,nInsertStart) ;
		}
	}
	else
	{
		res.Copy(B);
		int nCount = A.GetCount();
		int nInsertStart = 0;
		for(int i=0;i<nCount;++i)
		{
			nInsertStart = InsertToOrderArray(res,A[i],Compare,nInsertStart) ;
		}

	}
	return res;
}
template<class TYPE>
CCopyArray<TYPE> InterSectOrderArray(CArray<TYPE>& A,CArray<TYPE>& B,int(*Compare)(const TYPE& a,const TYPE& b))
{
	CCopyArray<TYPE> res;

	if(A.GetCount()<=B.GetCount())
	{
		res.Copy(A);
		int nCount = res.GetCount();
		int nFindEnd = 0xfffffff;
		for(int i= nCount-1;i>=0;--i)
		{
			int nIndex = FindInOrderArray(B,res[i],Compare,0,nFindEnd);
			if(nIndex<0 || res[i]!=B[nIndex])
				res.RemoveAt(i);
			nFindEnd = nIndex;
		}
	}
	else
	{
		res.Copy(B);
		int nCount = res.GetCount();
		int nFindEnd = 0xfffffff;
		for(int i= nCount-1;i>=0;--i)
		{
			int nIndex = FindInOrderArray(A,res[i],Compare,0,nFindEnd);
			if(nIndex<0 || res[i]!=A[nIndex])
				res.RemoveAt(i);
			nFindEnd = nIndex;
		}
	}
	return res;
}

template<class TYPE>
CCopyArray<TYPE> ComplementOrderArray(CArray<TYPE>& A,CArray<TYPE>& B,int(*Compare)(const TYPE& a,const TYPE& b))
{
	CCopyArray<TYPE> res;

	res.Copy(A);
	int nCount = res.GetCount();
	int nFindEnd = 0xfffffff;
	for(int i= nCount-1;i>=0;--i)
	{
		int nIndex = FindInOrderArray(B,res[i],Compare,0,nFindEnd);
		if(nIndex>=0 && Compare(res[i],B[nIndex])==0)
			res.RemoveAt(i);
		nFindEnd = nIndex;
		if(nFindEnd<0)
			break;
	}
	

	return res;
}

template<class TYPE>
CCopyArray<TYPE> NoOrderAry2OrderAry(CArray<TYPE>& A,int(*Compare)(const TYPE& a,const TYPE& b))
{
	CCopyArray<TYPE> res;

	for(int i=0;i<A.GetCount();++i)
	{
		InsertToOrderArray(res,A[i],Compare);
	}


	return res;
}

template<class TYPE>
int CompareAry(CArray<TYPE>& A,CArray<TYPE>& B,int(*Compare)(const TYPE& a,const TYPE& b))
{
	int res = 0;
	
//	if(A.GetCount()>0 && B.GetCount()>0)
	{
		for(int i=0;;++i)
		{
			if(i>=A.GetCount() || i>=B.GetCount())
				break;

			res = Compare(A[i],B[i]);
			if(res != 0)
				break;
		}

		if(res==0)
		{
			res = A.GetCount() - B.GetCount();
			if(res!=0)
				res = res/abs(res);
		}
	}
	//else if(A.GetCount()<=0)
	//{
	//	res = -1;
	//}
	//else if(B.GetCount()<=0)
	//{
	//	res = 1;
	//}

	return res;
}

template<class TYPE>
bool IsAryInclude(CArray<TYPE>& Include,CArray<TYPE>& beInclude,int(*Compare)(const TYPE& a,const TYPE& b))
{
	bool res = false;

	CCopyArray<TYPE> OrdInclude = NoOrderAry2OrderAry(Include,Compare);
	CCopyArray<TYPE> OrdBeInclude = NoOrderAry2OrderAry(beInclude,Compare);

	res =  true;
	for(int i=0;i<OrdBeInclude.GetCount();++i)
	{
		int nFind = FindInOrderArray(OrdInclude,OrdBeInclude[i],Compare);
		if(nFind>=0 && Compare(OrdInclude[nFind],OrdBeInclude[i])==0)
		{
		}
		else
		{
			res = false;
			break;
		}
	}

	return res;
}

CCopyArray<CString> Str2StrAry(const CString& strStr,const CString& strDelimiter = CString(";"),bool bSort = true);
CString StrAry2Str(const CArray<CString>& aryStr,const CString& strDelimiter = CString(";"));

//属性列表搜索函数
int FindItemPropByNameInList(const CString& strPropName,const CArray<CString>& aryPropList);
//属性比较
int CompareItemProp(const CString& strValue1,const CString& strValue2,const CString& strType);
//属性列表合并
CCopyArray<CString> CombineSetItemPropList(const CArray<CString>& aryItemPropList1,const CArray<CString>& aryItemPropList2,bool& bHasConflict);
//求属性列表交集
CCopyArray<CString> CommonItemPropList(const CArray<CString>& aryItemPropList1,const CArray<CString>& aryItemPropList2);
//属性列表求差
CCopyArray<CString> DiffSetItemPropList(const CArray<CString>& aryItemPropList1,const CArray<CString>& aryItemPropList2);

//显示相关函数
#define RECT_WIDTH(rect) (rect.right - rect.left)
#define RECT_HEIGHT(rect) (rect.bottom - rect.top)
bool InRect(int x,int y,RECT rect);
int GetWndFontWidth(CWnd* pWnd);
void GetWndFontSize(CWnd* pWnd,SIZE& szFont);

//数据合法性校验函数
bool NameNoInValidChar(const CString& strName);

//XML 跨Documnet子树结构拷贝函数
bool CopyXmlTreeNodes(MSXML2::IXMLDOMDocumentPtr& pDestXmlDoc,MSXML2::IXMLDOMElementPtr& pDestParentNode,MSXML2::IXMLDOMNodeListPtr& pSrcNodes);




//MESSAGE
#define WM_FSS_BASE WM_APP + 0x1000
#define SHELLTREE_SELECTEDCHANGE_NOTIFY					WM_FSS_BASE + 1
#define SHELLCONTENTLIST_ITEMDBCLICK_NOTIFY				WM_FSS_BASE + 2
#define ITEMEDITLIST_SELECT_CHANGED						WM_FSS_BASE + 3
#define SETCHKTREE_CHKSTATE_CHANGED						WM_FSS_BASE + 4
//
//#define QUERYRES_LIST_DBCLICK							WM_FSS_BASE + 5
//#define QUERYRES_LIST_RECLASSIFY						WM_FSS_BASE + 6
//#define QUERYRES_LIST_DELFILEITEM						WM_FSS_BASE + 7
//
//
////entity view
//#define	WM_ENTIYTREE_DISPLAYSET							WM_FSS_BASE + 21
//
////item property list
//#define	WM_ITEMPROPLIST_EDITBEGIN						WM_FSS_BASE + 31
//#define	WM_ITEMPROPLIST_EDITEND							WM_FSS_BASE + 32
//
////显示文件集内容逻辑参数
//#define DISPLASET_ORIGIN	0
//#define DISPLASET_AND		1
//#define DISPLASET_OR		2
//#define DISPLASET_NO		3
//#define DISPLASET_INTO		4
//
//
////filesetstore 归档消息
//#define	WM_FILESETSTORE_PIGEONHOLE						WM_FSS_BASE + 31
//#define	WM_FILESETSTORE_UNPIGEONHOLE					WM_FSS_BASE + 32
//#define WM_PGINNXTTH_ENDNOTIFY							WM_FSS_BASE + 33
//
////WPAPRM
//#define PIGEONHOLE_START	0
//#define PIGEONHOLE_PROCESS	1
//#define PIGEONHOLE_ERROR	2
//#define PIGEONHOLE_END		3
//
////RESULT
//#define	PIGEONHOLE_COTINUE	0
//#define	PIGEONHOLE_ABORT	1
//#define	PIGEONHOLE_RETRY	2
//
//
////Shell归档触发消息
//#define WM_SHELL_PIGENOHOLE								WM_FSS_BASE + 51
//
////系统托盘回调消息
//#define WM_SHELLICON_CALLBACK							WM_FSS_BASE + 61
//
////查找实体集公共消息
////#define WM_FIND_ENTITYSET								WM_FSS_BASE + 71
////#define FIND_ENTITYSET_CTRLF							0
////#define FIND_ENTITYSET_F3								1
//
////查询表达式输入框回车消息
//#define WM_QUERYEXPEDIT_RETURN							WM_FSS_BASE + 81
//
////归档窗口增加实体文件集通知消息
//#define WM_NEWCHILDSET_IN_PIG							WM_FSS_BASE + 91
//
////检索结果窗口项目选中通知消息
//#define WM_QUERYRESULT_ITEMSEL_CHG						WM_FSS_BASE + 92
//
////删除项目,清除文件内容Web页面
//#define WM_RIGHTVIEW2MAINWND_DEL_ITEM					WM_FSS_BASE + 93
////文件内容Web页面装载完毕
//#define WM_CONTENTVIEW_LOADED							WM_FSS_BASE + 94
////客户端线程报错
//#define WM_CLITHREAD_ERROR								WM_FSS_BASE + 95
////数据库更新完毕
//#define WM_CLITHREAD_SETSTORE_UPDATE					WM_FSS_BASE + 96
////提交更新
//#define WM_CLITHREAD_COMMIT_UPDATE						WM_FSS_BASE + 97
////当前工作目录改变
//#define WM_WORKDIR_CHG									WM_FSS_BASE + 98
////目录重命名
//#define WM_DIRNAME_RE									WM_FSS_BASE + 99
////目录列表更改:增加、改名、删除
//#define WM_DIRLIST_CHG									WM_FSS_BASE + 100
////进度条更新
//#define WM_LOAD_DATA                                    WM_FSS_BASE+ 101
////浮动窗口消息
//#define WM_COMMIT_QUERYCOMMAND							WM_FSS_BASE + 102
//#define WM_GET_QUERYSTATE								WM_FSS_BASE + 103
////输入标签归档同步摘要页面		
//#define WM_FASTTAG_SYN_DES								WM_FSS_BASE + 104	
////HyperLink被点击
//#define WM_HYPERLNK_CLICK								WM_FSS_BASE + 105
////RightView折叠状态通知
//#define WM_RIGHTVIEW_FOLDCHG							WM_FSS_BASE + 106

//获得系统ID
#define WINDOWS_UNKNOW	-1
#define WINDOWS_2000	0
#define WINDOWS_XP		1
#define WINDOWS_2003	2
#define WINDOWS_VISTA	3
#define WINDOWS_NT4		4
#define WINDOWS_95		5
#define WINDOWS_98		6
#define WINDOWS_ME		7
#define WINDOWS_31		8


//文件块长度定义
#define KB															1024
#define MB															(KB*KB)
#define BLOCK_SIZE													(256*KB)	//256KB

//获得系统类型标志 
int GetWindowsSystemID();

//在树控件中查找文件集
HTREEITEM FindSetNodeByNameInTree(const CTreeCtrl* pTree,const CString& strSetName,HTREEITEM hStartItem,bool& bStart,HTREEITEM hRootItem);


struct TASK_INFO_TYPE{
	CBuffer* pDataToProc;
	CString strCliAddr;

	TASK_INFO_TYPE()
	{
		pDataToProc=NULL;
	}
	~TASK_INFO_TYPE()
	{
		SAFE_DELETE(pDataToProc);
	}
};

UINT CStringToUINT(CString strSrc);