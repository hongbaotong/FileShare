#pragma once
#import "msxml2.dll"
#include "LibCommon/common.h"

class CPackageParser
{
public:
	CPackageParser(void);
	~CPackageParser(void);

	BOOL Load(CBuffer& bufSrc);
	BOOL SaveToBuf(CBuffer& bufDest);
	

	typedef MSXML2::IXMLDOMElementPtr RECORD_HANDLE;

	RECORD_HANDLE GetRecord(int nIndex);
	RECORD_HANDLE EnumRecord(int& nIndex);

	CString GetFieldValue(RECORD_HANDLE hRecord,const CString& strName);
	BOOL SetFieldValue(RECORD_HANDLE hRecord,const CString& strName,const CString& strValue);

	BOOL CreateNewPackage();
	CString GetPackageProc(CString& strSubProc);
	void SetPackageProc(const CString& strMainProc,const CString& strSubProc);
	CString GetObjDir();//��ȡĿ��Ŀ¼
	void SetObjDir(const CString& strObjDir);

	RECORD_HANDLE CreateNewRecord(int* pIndex=NULL);
	CString GetRecordType(RECORD_HANDLE hRecord);
	void SetRecordType(RECORD_HANDLE hRecord,const CString strType);

	//������̷���ֵ
	void SetProcResult(const CString& strResult);
	CString GetProcResult();


protected:
	MSXML2::IXMLDOMDocumentPtr m_pXmlDoc;

	//������Կ
	CBuffer m_keyCrypt;
};
