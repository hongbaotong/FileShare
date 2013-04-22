#pragma once
#include "FileShare.h"



//请求类型
#define GET_SHARELIST_REQ 0

//回应类型
#define RCV_SVR_SHARELIST 0
#define RCV_SVR_FILE 1

class CClientTask
{
public:
	CClientTask(void);
	CClientTask(CString strSvrAddr,UINT uPort);
	CClientTask(HOST_INFO *pHostInfo);
public:
	~CClientTask(void);	
public:
	BOOL SendRequest();
	BOOL RecvResponse(void);
	BOOL RequestServer();
	void GetShareList(CArray<FILE_INFO> &arrFileList);
	void GetFile(CString strFileName);
	void InitMember(CString strSvrAddr, UINT uPort);
	//BOOL HandleRequest(int nReqType);// 处理请求包
	BOOL HandleResponse(int nRspType);//处理回应包
private:
	CSocket *m_ConnSocket;
	CString m_strSvrAddr;
	UINT m_uPort;
	CArray<FILE_INFO> m_arrFileList;
	void ConnSvr(void);
private:
	int m_nCliSvrState;// 表示客户端与服务器的连接状态
	CPackageParser* m_pInPackage;
	CPackageParser* m_pOutPackage;
	
	// 接收文件长度
	ULONGLONG m_uFileLen;
};
