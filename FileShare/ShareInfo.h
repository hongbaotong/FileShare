#pragma once
#include "LibCommon/common.h"
#include "FileInfoProc.h"

class CShareInfo
{
public:
	CShareInfo(void);
public:
	~CShareInfo(void);
protected:
	MSXML2::IXMLDOMDocumentPtr m_pXmlDoc;
	CString m_strConfPath;
public:
	BOOL Load(CString strConfPath);
public:
	void Save(void);
public:
	BOOL SetServerName(CString strSvrName);
public:
	UINT GetSvrPort(void);
public:
	void AddItem(CString strItemName, CString strPath, CString strSize, CString strMD5);
public:
	BOOL DelItem(CString strItemName);
public:
	BOOL GetItemList(CArray<FILE_INFO> &arrItemList);
public:
	CString GetSvrName(void);
public:
	CString GetFilePath(CString strFileName);
};
