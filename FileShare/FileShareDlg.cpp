// FileShareDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "FileShare.h"
#include "FileShareDlg.h"
#include "FileShareList.h"
#include "HostShareList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CFileShareDlg �Ի���




CFileShareDlg::CFileShareDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFileShareDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFileShareDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TASK_LIST, m_pTaskList);
}

BEGIN_MESSAGE_MAP(CFileShareDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, &CFileShareDlg::OnTcnSelchangeTab)
	ON_BN_CLICKED(IDC_BTN_SETSVRNAME, &CFileShareDlg::OnBnClickedBtnSetsvrname)
END_MESSAGE_MAP()


// CFileShareDlg ��Ϣ�������

BOOL CFileShareDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	//m_SocketThread.CreateThread();
	SetDlgItemText(IDC_EDIT_SVRNAME,GetSvrApp()->m_ShareInfo->GetSvrName());
	m_pTab=(CTabCtrl*)GetDlgItem(IDC_TAB);
	m_pTab->InsertItem(0,_T("���ع����б�"));
	m_pTab->InsertItem(1,_T("�����������б�"));

	m_FileShareList.Create(IDD_FILE_LIST,(CWnd*)m_pTab);
	m_HostShareList.Create(IDD_HOST_LIST,(CWnd*)m_pTab);
	

	CRect rect;
	m_pTab->GetClientRect(&rect);
	rect.top+=20; 
	rect.bottom-=4; 
	rect.left+=2; 
	rect.right-=4; 
	m_FileShareList.MoveWindow(&rect);
	m_HostShareList.MoveWindow(&rect);
	
	m_FileShareList.ShowWindow(TRUE); 
	m_pTab->SetCurSel(0); 

	//m_pTaskList=(CTaskListCtrl*)GetDlgItem(IDC_TASK_LIST);
	m_pTaskList.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	m_pTaskList.InsertColumn(1,_T("������"),LVCFMT_CENTER,120,1);
	m_pTaskList.InsertColumn(2,_T("��С"),LVCFMT_CENTER,120,2);
	m_pTaskList.InsertColumn(3,_T("����"),LVCFMT_CENTER,120,3);
	m_pTaskList.InsertColumn(4,_T("ʣ��ʱ��"),LVCFMT_CENTER,120,4);
	m_pTaskList.InsertColumn(5,_T("�ٶ�"),LVCFMT_CENTER,120,5);
	
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CFileShareDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CFileShareDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ��������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù����ʾ��
//
HCURSOR CFileShareDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CFileShareDlg::OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int CurSel; 
	CurSel=m_pTab->GetCurSel(); 
	switch(CurSel) 
	{ 
	case 0: 
		m_FileShareList.ShowWindow(TRUE);
		m_HostShareList.ShowWindow(FALSE);
		break; 
	case 1: 
		m_FileShareList.ShowWindow(FALSE);
		m_HostShareList.ShowWindow(TRUE);
		break;
	default: ; 
	} 
	*pResult = 0;
}

void CFileShareDlg::OnBnClickedBtnSetsvrname()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strName;
	GetDlgItemText(IDC_EDIT_SVRNAME,strName);
	if(GetSvrApp()->m_ShareInfo->SetServerName(strName))
	{
		MessageBox(_T("�޸ĳɹ�"));
	}
}

void CFileShareDlg::OnOK()
{
	// TODO: �ڴ����ר�ô����/����û���

	//CDialog::OnOK();
}
