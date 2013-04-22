#include "StdAfx.h"
#include "FileInfoProc.h"
#include "CHash.h"

CFileInfoProc::CFileInfoProc(void)
{
	//m_Fileinfo.nFileLength=0;
	//m_Fileinfo.strFileName=NULL;
	//m_Fileinfo.strFilePath=NULL;
	//m_Fileinfo.strFileType=NULL;
	//m_Fileinfo.strFileMd5=NULL;
}

CFileInfoProc::~CFileInfoProc(void)
{
}

FILE_INFO CFileInfoProc::GetFileInfo(CString strPath)
{
	FILE_INFO FileInfo;
	SHFILEINFO TypeInfo;
	CFile file;
	file.Open(strPath,CFile::modeRead);
	SHGetFileInfo(file.GetFilePath(),NULL,&TypeInfo,sizeof(TypeInfo),SHGFI_TYPENAME);
	ULONGLONG len=file.GetLength();
	CString strFileLen;



	if(len<1024)
	{
		strFileLen.Format(_T("%d ×Ö½Ú"),len);
	}else if(len<(1024*1024))
	{
		strFileLen.Format(_T("%.2lf KB"),(double)len/1024);
	}else if(len<(1024*1024*1024))
	{
		strFileLen.Format(_T("%.2lf MB"),(double)len/(1024*1024));
	}else
	{
		strFileLen.Format(_T("%.2lf GB"),(double)len/(1024*1024*1024));
	}
	FileInfo.strFileSize=strFileLen;
	FileInfo.strFileName=file.GetFileName();
	FileInfo.strFilePath=file.GetFilePath();
	file.Close();

	CHash m_Hash;
	m_Hash.SetHashOperation(FILE_HASH);
	m_Hash.SetHashFile(strPath);
	m_Hash.SetHashFormat(3);
	m_Hash.SetHashAlgorithm(5);
	FileInfo.strFileMd5=m_Hash.DoHash();


	//FileInfo.strFileType=(CString)TypeInfo.szTypeName;
	//FileInfo.nIcon=info.iIcon;
	return FileInfo;
}
