#include "StdAfx.h"
#include "PackageParser.h"
#include "CryptUnit.h"



CPackageParser::CPackageParser(void)
{
	CoInitialize(NULL);
	m_keyCrypt = HexStrToNumber(_T("9D73D047ACD735A9113C886DCC3462DA"));
	try
	{
		m_pXmlDoc.CreateInstance(__uuidof(MSXML2::DOMDocument));
	}
	catch(...)
	{
	}
}

CPackageParser::~CPackageParser(void)
{
	m_pXmlDoc = NULL;
}

BOOL CPackageParser::Load(CBuffer& bufSrc)
{
	//bufSrc 中为加密的UTF-8编码
	BOOL res = FALSE;

	//解密
	CBuffer bufPlainText;
	if(DecryptData(m_keyCrypt,SESSION_KEY,bufSrc,bufPlainText))
	{

		//转码
		CBuffer bufXmlText;
		
		bufXmlText.Alloc(bufPlainText.GetDataLen()*2+2);
		if(MultiByteToWideChar(CP_UTF8,0,(LPCSTR)bufPlainText.GetBufP(),bufPlainText.GetDataLen(),(WCHAR*)bufXmlText.GetBufP(),bufXmlText.GetBufLen()/2)>0)
		{
			try
			{
				res = m_pXmlDoc->loadXML((LPCTSTR)bufXmlText.GetBufP());
			}
			catch(...)
			{
			}
		}
	}

	return res;
}

BOOL CPackageParser::SaveToBuf(CBuffer& bufDest)
{
	BOOL res = FALSE;
	_bstr_t bsSrcXml;
	try
	{
		bsSrcXml = m_pXmlDoc->Getxml();
	}
	catch(...)
	{
	}
	if(bsSrcXml.length()>0)
	{
		//UNICODE转为UTF-8
		CBuffer bufUTF8;
		bufUTF8.Alloc(bsSrcXml.length()*4);
		//加密
		int nUTF8Len = WideCharToMultiByte(CP_UTF8,0,bsSrcXml.GetBSTR(),bsSrcXml.length(),(LPSTR)bufUTF8.GetBufP(),bufUTF8.GetBufLen(),NULL,NULL);
		if(nUTF8Len>0)
		{
			bufUTF8.SetDataLen(nUTF8Len);
			if(EncryptData(m_keyCrypt,SESSION_KEY,bufUTF8,bufDest))
				res = TRUE;
		}
		
		//bufDest.SetData((BYTE*)(bsSrcXml.GetBSTR())-4,(bsSrcXml.length()+1)*2+4);
	}
	return res;
}



CPackageParser::RECORD_HANDLE CPackageParser::GetRecord(int nIndex)
{
	RECORD_HANDLE res = NULL;

	try
	{
		CString strPath;
		strPath.Format(_T("./Record[%d]"),nIndex+1);
		res = m_pXmlDoc->selectSingleNode((LPCTSTR)strPath);
	}
	catch(...)
	{
	}

	return res;
}

CPackageParser::RECORD_HANDLE CPackageParser::EnumRecord(int& nIndex)
{
	RECORD_HANDLE res = NULL;

	if(nIndex>=0)
	{
		try
		{
			CString strPath;
			strPath.Format(_T("/Package/Record[%d]"),nIndex);
			res = m_pXmlDoc->selectSingleNode((LPCTSTR)strPath);
			if(res!=NULL)
				++nIndex;
			else
				nIndex = -1;
		}
		catch(...)
		{
		}
	}

	return res;
}

CString CPackageParser::GetFieldValue(RECORD_HANDLE hRecord,const CString& strName)
{
	CString res;

	try
	{
		CString strPath;
		strPath.Format(_T("./Field[@Name=\"%s\"]"),strName);
		MSXML2::IXMLDOMNodePtr pNode = hRecord->selectSingleNode((LPCTSTR)strPath);
		if(pNode!=NULL)
			res = pNode->text.GetBSTR();
	}
	catch(...)
	{
	}

	return res;
}

BOOL CPackageParser::CreateNewPackage()
{
	BOOL res = FALSE;
	try
	{
		m_pXmlDoc->loadXML(_T("<Package></Package>"));
		res = TRUE;
	}
	catch(...)
	{
	}
	return res;
}
CPackageParser::RECORD_HANDLE CPackageParser::CreateNewRecord(int* pIndex)
{
	RECORD_HANDLE res;

	try
	{
		MSXML2::IXMLDOMElementPtr pRecord = m_pXmlDoc->createElement(_T("Record"));
		res = m_pXmlDoc->documentElement->appendChild(pRecord);
	}
	catch(...)
	{
	}

	return res;
}

BOOL CPackageParser::SetFieldValue(RECORD_HANDLE hRecord,const CString& strName,const CString& strValue)
{
	BOOL res = FALSE;

	try
	{
		CString strPath;
		strPath.Format(_T("./Field[@Name=\"%s\"]"),strName);
		MSXML2::IXMLDOMElementPtr pFieldNode = hRecord->selectSingleNode((LPCTSTR)strPath);
		if(pFieldNode == NULL)//没有，则创建
		{
			pFieldNode = m_pXmlDoc->createElement(_T("Field"));
			pFieldNode->setAttribute(_T("Name"),_variant_t((LPCTSTR)strName));
			hRecord->appendChild(pFieldNode);
		}
		pFieldNode->Puttext((LPCTSTR)strValue);
		res = TRUE;
	}
	catch(...)
	{
	}

	return res;
}

CString CPackageParser::GetObjDir()
{
	CString res;

	try
	{
		res = m_pXmlDoc->documentElement->getAttribute(_T("ObjDir"));
	}
	catch(...)
	{
	}

	return res;
}

void CPackageParser::SetObjDir(const CString& strObjDir)
{
	try
	{
		m_pXmlDoc->documentElement->setAttribute(_T("ObjDir"),(LPCTSTR)strObjDir);
	}
	catch(...)
	{
	}
}


CString CPackageParser::GetPackageProc(CString& strSubProc)
{
	CString res;

	CString strProc;

	try
	{
		strProc = m_pXmlDoc->documentElement->getAttribute(_T("Procedure"));
		
	}
	catch(...)
	{
	}

	if(strProc.GetLength()>0)
	{
		CCopyArray<CString> aryPiceof = Str2StrAry(strProc,_T("/"),false);
		res = aryPiceof[0];
		if(aryPiceof.GetCount()>1)
			strSubProc = aryPiceof[1];
	}

	return res;
}

void CPackageParser::SetPackageProc(const CString& strMainProc,const CString& strSubProc)
{
	if(strMainProc.GetLength()>0)
	{
		CString strProc;
		if(strSubProc.GetLength()>0)
			strProc.Format(_T("%s/%s"),strMainProc,strSubProc);
		else
			strProc = strMainProc;

		try
		{
			m_pXmlDoc->documentElement->setAttribute(_T("Procedure"),(LPCTSTR)strProc);
		}
		catch(...)
		{
		}

	}
}

CString CPackageParser::GetRecordType(RECORD_HANDLE hRecord)
{
	CString res;

	try
	{
		res = hRecord->getAttribute(_T("Type"));
	}
	catch(...)
	{
	}

	return res;
}

void CPackageParser::SetRecordType(RECORD_HANDLE hRecord,const CString strType)
{
	if(strType.GetLength()>0)
	{
		try
		{
			hRecord->setAttribute(_T("Type"),(LPCTSTR)strType);
		}
		catch(...)
		{
		}
	}
}

void CPackageParser::SetProcResult(const CString& strResult)
{
	try
	{
		m_pXmlDoc->documentElement->setAttribute(_T("Result"),(LPCTSTR)strResult);
	}
	catch(...)
	{
	}

}

CString CPackageParser::GetProcResult()
{
	CString res;

	try
	{
		res = m_pXmlDoc->documentElement->getAttribute(_T("Result"));
	}
	catch(...)
	{
	}

	return res;

}
