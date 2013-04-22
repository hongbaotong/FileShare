// FileShare.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "FileShare.h"
#include "FileShareDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFileShareApp

BEGIN_MESSAGE_MAP(CFileShareApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CFileShareApp ����

CFileShareApp::CFileShareApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
	
}


// Ψһ��һ�� CFileShareApp ����

CFileShareApp theApp;

// CFileShareApp ��ʼ��

BOOL CFileShareApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

	m_SendDataEvent.SetEvent();
	CoInitialize(NULL);
	m_ShareInfo=new CShareInfo();
	m_ShareInfo->Load(GetExeFolderPath()+L"\\"+FS_SHAREINFO);
	
	m_NiThread=new CNiThread();
	m_NiThread->CreateThread();

	m_TaskThread=new CTaskThread[TASK_THREAD_NUM];
	for(int i=0;i<TASK_THREAD_NUM;i++)
	{
		m_TaskThread[i].CreateThread();
	}
	
	CFileShareDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˴����ô����ʱ�á�ȷ�������ر�
		//  �Ի���Ĵ���
		
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ�á�ȡ�������ر�
		//  �Ի���Ĵ���
	}
	
	
	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}

CFileShareApp* GetSvrApp()
{
	return (CFileShareApp*)AfxGetApp();
}

void CFileShareApp::CommitTask(CBuffer* pDataToProc, CString strClientAddr)
{
	TASK_INFO_TYPE *pTaskInfo;
	pTaskInfo=new TASK_INFO_TYPE;
	pTaskInfo->pDataToProc=pDataToProc;
	pTaskInfo->strCliAddr=strClientAddr;

	m_TaskQueue.PutTask(pTaskInfo);
}

int CFileShareApp::ExitInstance()
{
	// TODO: �ڴ����ר�ô����/����û���
	m_NiThread->PostThreadMessageW(WM_QUIT,NULL,NULL);

	for(int i=0;i<TASK_THREAD_NUM;i++)
	{
		m_TaskThread[i].PostThreadMessageW(WM_QUIT,NULL,NULL);
	}

	SAFE_DELETE(m_ShareInfo);

	return CWinApp::ExitInstance();
}

void CFileShareApp::SendData(CBuffer bufToSend, CString strCliAddr)
{
	m_NiThread->SendData(bufToSend,strCliAddr);
}

void CFileShareApp::SendFile(CBuffer bufToSend, CString strCliAddr)
{
	m_NiThread->SendFile(bufToSend,strCliAddr);
}

TASK_INFO_TYPE* CFileShareApp::GetTask(void)
{
	return m_TaskQueue.GetTask();
}


