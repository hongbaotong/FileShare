#include "StdAfx.h"
#include "ShareInfo.h"
#include "LibCommon/common.h"

CShareInfo::CShareInfo(void)
{
	
	try
	{
		m_pXmlDoc.CreateInstance(__uuidof(MSXML2::FreeThreadedDOMDocument));
	}
	catch(...)
	{
	}
}

CShareInfo::~CShareInfo(void)
{
}

BOOL CShareInfo::Load(CString strConfPath)
{
	BOOL res = FALSE;

	try
	{
		if(m_pXmlDoc!=NULL)
		{
			_variant_t varSrc = (LPCTSTR)strConfPath;
			res = m_pXmlDoc->load(varSrc);
			if(res)
				m_strConfPath = strConfPath;
		}

	}
	catch(...)
	{
	}

	return res;
}

void CShareInfo::Save(void)
{
	try
	{
		m_pXmlDoc->save((LPCTSTR)m_strConfPath);
	}
	catch(...)
	{

	}
}

BOOL CShareInfo::SetServerName(CString strSvrName)
{
	BOOL res=FALSE;

	try
	{
		MSXML2::IXMLDOMElementPtr pXMLNode = NULL;
		pXMLNode=m_pXmlDoc->selectSingleNode(_T("/FileShare"));
		pXMLNode->setAttribute(_T("ServerName"),(LPCTSTR)strSvrName);
		Save();
		res=TRUE;
	}
	catch(...)
	{

	}

	return res;
}

UINT CShareInfo::GetSvrPort(void)
{
	UINT res;
	CString strPort;
	try
	{
		MSXML2::IXMLDOMElementPtr pXMLNode = NULL;
		pXMLNode=m_pXmlDoc->selectSingleNode(_T("/FileShare"));
		strPort=pXMLNode->getAttribute(_T("Port"));

		res=CStringToUINT(strPort);

	}
	catch(...)
	{

	}

	return res;
}

void CShareInfo::AddItem(CString strItemName, CString strPath, CString strSize, CString strMD5)
{
	try
	{
		MSXML2::IXMLDOMElementPtr pShareList=m_pXmlDoc->selectSingleNode(_T("/FileShare/ShareList"));
		MSXML2::IXMLDOMElementPtr pNewNode = m_pXmlDoc->createElement(_T("ShareItem"));

		pNewNode->setAttribute(_T("ItemName"),(LPCTSTR)strItemName);
		pNewNode->setAttribute(_T("Path"),(LPCTSTR)strPath);
		pNewNode->setAttribute(_T("Size"),(LPCTSTR)strSize);
		pNewNode->setAttribute(_T("MD5"),(LPCTSTR)strMD5);

		pShareList->appendChild(m_pXmlDoc->createTextNode("\n\t\t"));
		pShareList->appendChild(pNewNode);
		pShareList->appendChild(m_pXmlDoc->createTextNode("\n\t"));
		Save();
		
	}
	catch(...)
	{

	}
}

BOOL CShareInfo::DelItem(CString strItemName)
{
	BOOL res=FALSE;
	try
	{
		MSXML2::IXMLDOMNodeListPtr pItemList=m_pXmlDoc->selectNodes(_T("/FileShare/ShareList/ShareItem"));
		MSXML2::IXMLDOMElementPtr pItemToDel=NULL;
		MSXML2::IXMLDOMElementPtr pShareList=m_pXmlDoc->selectSingleNode(_T("/FileShare/ShareList"));
		for(int i=0;i<pItemList->length;i++)
		{
			pItemToDel=pItemList->Getitem(i);
			if(pItemToDel->getAttribute(_T("ItemName"))==strItemName)
			{
				pShareList->removeChild(pItemToDel);
				res=TRUE;
			}
		}
		if(res)
		{
			Save();
		}
	}
	catch(...)
	{
	}
	return res;
}

BOOL CShareInfo::GetItemList(CArray<FILE_INFO> &arrItemList)
{
	FILE_INFO fileinfo;
	BOOL res=FALSE;
	try
	{
		MSXML2::IXMLDOMNodeListPtr pItemList=m_pXmlDoc->selectNodes(_T("/FileShare/ShareList/ShareItem"));
		MSXML2::IXMLDOMElementPtr pShareItem;
		for(int i=0;i<pItemList->length;i++)
		{
			pShareItem=pItemList->Getitem(i);
			fileinfo.strFileName=pShareItem->getAttribute(_T("ItemName"));
			fileinfo.strFilePath=pShareItem->getAttribute(_T("Path"));
			fileinfo.strFileSize=pShareItem->getAttribute(_T("Size"));
			fileinfo.strFileMd5=pShareItem->getAttribute(_T("MD5"));
			arrItemList.Add(fileinfo);
			res=TRUE;
		}
	}
	catch(...)
	{
	}
	return res;
}

CString CShareInfo::GetSvrName(void)
{
	CString strSvrName;
	try
	{
		MSXML2::IXMLDOMElementPtr pXMLNode = NULL;
		pXMLNode=m_pXmlDoc->selectSingleNode(_T("/FileShare"));
		strSvrName=pXMLNode->getAttribute(_T("ServerName"));
		
	}
	catch(...)
	{

	}

	return strSvrName;
}

CString CShareInfo::GetFilePath(CString strFileName)
{
	CString res;
	try
	{
		MSXML2::IXMLDOMNodeListPtr pItemList=m_pXmlDoc->selectNodes(_T("/FileShare/ShareList/ShareItem"));
		MSXML2::IXMLDOMElementPtr pItemToSend=NULL;
		
		for(int i=0;i<pItemList->length;i++)
		{
			pItemToSend=pItemList->Getitem(i);
			if(pItemToSend->getAttribute(_T("ItemName"))==strFileName)
			{
				res=pItemToSend->getAttribute(_T("Path"));
				break;
			}
		}
	}
	catch(...)
	{
	}

	return res;
}
