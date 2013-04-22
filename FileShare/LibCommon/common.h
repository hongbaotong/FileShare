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

//���ں���
SYSTEMTIME OffsetDate(const SYSTEMTIME& strBaseDate,int nOffsetDay);//����ƫ�ƺ������
int CompareDate(const SYSTEMTIME& A,const SYSTEMTIME& B);//�Ƚ������Ⱥ�

//��־����
void WriteLog(const CString& strLog);

//ϵͳ���̹�����
DWORD FindProcess(const CString& strExeName);
bool KillProcess(DWORD dwProcessID);
bool KillProcess(const CString& strExeName);


//�ļ�ϵͳ�йغ���
bool SplitFilePath(const CString& strFullPathName,CString& strFolderPath,CString& strDriver,CString& strFName,CString& strExt);
bool CreateDirectoryRecuersively(const CString& strPath);
bool RemoveDirectoryRecuersively(const CString& strRoot);
bool DelFilesByWildPath(const CString& strWildPath);
bool MoveFileByWildPath(const CString& strSrcPath,const CString& strDestFolder);
CString GetCommonPath(const CString& strPath1,const CString& strPath2);
bool IsFileExist(const CString& strPath);
bool IsFolder(const CString& strPath);

//��ȡִ���ļ�·��
CString GetExeFolderPath();

//URL������
CString FilePath2Url(const CString& strFilePath);
bool SplitUrl(const CString& strUrl,CString& strProtocol,CString& strHost,CString& strPath,CString& strFileName,CString& strExtName,CArray<CString>* pAryParam=NULL);
CString UrlEscapeEncode(const CString& strSrcCode);
CString UrlEscapeDecode(const CString& strEnCode);

//��ʽ���ļ���С�ַ���
CString BuildFileSizeStr(DWORD dwLow,DWORD dwHi);

//��������ƥ��
int FindMatchBracket(const CString& strStr,TCHAR tcBracket,int iPos); 

//���ؼ�����
CCopyArray<HTREEITEM>  TreeCtrl_FindItemByTxt(const CString& strItemTxt,const CTreeCtrl& ctrlTree,HTREEITEM hItem = NULL);
bool TreeCtrl_IsChildItem(HTREEITEM hItem,HTREEITEM hParentItem,const CTreeCtrl& ctrlTree);
void TreeCtrl_ExpandItemPath(CTreeCtrl& ctrlTree,HTREEITEM hItem);
void TreeCtrl_CollapseTree(CTreeCtrl& ctrlTree,HTREEITEM hItem);

//��ȡ���ӷ�ʽ����ĿժҪ
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
//2010.10.22����bRepeatKey����������ʾ�Ƿ������ֵ�ظ��ļ�¼�����б�
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

	//����ڴ��������
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

//�����б���������
int FindItemPropByNameInList(const CString& strPropName,const CArray<CString>& aryPropList);
//���ԱȽ�
int CompareItemProp(const CString& strValue1,const CString& strValue2,const CString& strType);
//�����б�ϲ�
CCopyArray<CString> CombineSetItemPropList(const CArray<CString>& aryItemPropList1,const CArray<CString>& aryItemPropList2,bool& bHasConflict);
//�������б���
CCopyArray<CString> CommonItemPropList(const CArray<CString>& aryItemPropList1,const CArray<CString>& aryItemPropList2);
//�����б����
CCopyArray<CString> DiffSetItemPropList(const CArray<CString>& aryItemPropList1,const CArray<CString>& aryItemPropList2);

//��ʾ��غ���
#define RECT_WIDTH(rect) (rect.right - rect.left)
#define RECT_HEIGHT(rect) (rect.bottom - rect.top)
bool InRect(int x,int y,RECT rect);
int GetWndFontWidth(CWnd* pWnd);
void GetWndFontSize(CWnd* pWnd,SIZE& szFont);

//���ݺϷ���У�麯��
bool NameNoInValidChar(const CString& strName);

//XML ��Documnet�����ṹ��������
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
////��ʾ�ļ��������߼�����
//#define DISPLASET_ORIGIN	0
//#define DISPLASET_AND		1
//#define DISPLASET_OR		2
//#define DISPLASET_NO		3
//#define DISPLASET_INTO		4
//
//
////filesetstore �鵵��Ϣ
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
////Shell�鵵������Ϣ
//#define WM_SHELL_PIGENOHOLE								WM_FSS_BASE + 51
//
////ϵͳ���̻ص���Ϣ
//#define WM_SHELLICON_CALLBACK							WM_FSS_BASE + 61
//
////����ʵ�弯������Ϣ
////#define WM_FIND_ENTITYSET								WM_FSS_BASE + 71
////#define FIND_ENTITYSET_CTRLF							0
////#define FIND_ENTITYSET_F3								1
//
////��ѯ���ʽ�����س���Ϣ
//#define WM_QUERYEXPEDIT_RETURN							WM_FSS_BASE + 81
//
////�鵵��������ʵ���ļ���֪ͨ��Ϣ
//#define WM_NEWCHILDSET_IN_PIG							WM_FSS_BASE + 91
//
////�������������Ŀѡ��֪ͨ��Ϣ
//#define WM_QUERYRESULT_ITEMSEL_CHG						WM_FSS_BASE + 92
//
////ɾ����Ŀ,����ļ�����Webҳ��
//#define WM_RIGHTVIEW2MAINWND_DEL_ITEM					WM_FSS_BASE + 93
////�ļ�����Webҳ��װ�����
//#define WM_CONTENTVIEW_LOADED							WM_FSS_BASE + 94
////�ͻ����̱߳���
//#define WM_CLITHREAD_ERROR								WM_FSS_BASE + 95
////���ݿ�������
//#define WM_CLITHREAD_SETSTORE_UPDATE					WM_FSS_BASE + 96
////�ύ����
//#define WM_CLITHREAD_COMMIT_UPDATE						WM_FSS_BASE + 97
////��ǰ����Ŀ¼�ı�
//#define WM_WORKDIR_CHG									WM_FSS_BASE + 98
////Ŀ¼������
//#define WM_DIRNAME_RE									WM_FSS_BASE + 99
////Ŀ¼�б����:���ӡ�������ɾ��
//#define WM_DIRLIST_CHG									WM_FSS_BASE + 100
////����������
//#define WM_LOAD_DATA                                    WM_FSS_BASE+ 101
////����������Ϣ
//#define WM_COMMIT_QUERYCOMMAND							WM_FSS_BASE + 102
//#define WM_GET_QUERYSTATE								WM_FSS_BASE + 103
////�����ǩ�鵵ͬ��ժҪҳ��		
//#define WM_FASTTAG_SYN_DES								WM_FSS_BASE + 104	
////HyperLink�����
//#define WM_HYPERLNK_CLICK								WM_FSS_BASE + 105
////RightView�۵�״̬֪ͨ
//#define WM_RIGHTVIEW_FOLDCHG							WM_FSS_BASE + 106

//���ϵͳID
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


//�ļ��鳤�ȶ���
#define KB															1024
#define MB															(KB*KB)
#define BLOCK_SIZE													(256*KB)	//256KB

//���ϵͳ���ͱ�־ 
int GetWindowsSystemID();

//�����ؼ��в����ļ���
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