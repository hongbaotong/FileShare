#pragma once
#include "FileShare.h"



//��������
#define GET_SHARELIST_REQ 0

//��Ӧ����
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
	//BOOL HandleRequest(int nReqType);// ���������
	BOOL HandleResponse(int nRspType);//�����Ӧ��
private:
	CSocket *m_ConnSocket;
	CString m_strSvrAddr;
	UINT m_uPort;
	CArray<FILE_INFO> m_arrFileList;
	void ConnSvr(void);
private:
	int m_nCliSvrState;// ��ʾ�ͻ����������������״̬
	CPackageParser* m_pInPackage;
	CPackageParser* m_pOutPackage;
	
	// �����ļ�����
	ULONGLONG m_uFileLen;
};
