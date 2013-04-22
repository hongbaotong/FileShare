#pragma once
//#include "CHash/CHash.h"

typedef struct tagFILE_INFO{
//	int nIcon;
	CString strFileName;
	CString strFilePath;
//	CString strFileType;
	CString strFileSize;
	CString strFileMd5;
}FILE_INFO,*pFILE_INFO;


class CFileInfoProc
{
public:
	CFileInfoProc(void);
	~CFileInfoProc(void);
public:
	FILE_INFO GetFileInfo(CString strPath);
private:
	//CHash m_Hash;
};
