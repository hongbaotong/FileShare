// FileShareDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "FileShare.h"
#include "FileShareDlg.h"
#include "FileShareList.h"
#include "HostShareList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CFileShareDlg 对话框




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


// CFileShareDlg 消息处理程序

BOOL CFileShareDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	//m_SocketThread.CreateThread();
	SetDlgItemText(IDC_EDIT_SVRNAME,GetSvrApp()->m_ShareInfo->GetSvrName());
	m_pTab=(CTabCtrl*)GetDlgItem(IDC_TAB);
	m_pTab->InsertItem(0,_T("本地共享列表"));
	m_pTab->InsertItem(1,_T("局域网共享列表"));

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
	m_pTaskList.InsertColumn(1,_T("任务名"),LVCFMT_CENTER,120,1);
	m_pTaskList.InsertColumn(2,_T("大小"),LVCFMT_CENTER,120,2);
	m_pTaskList.InsertColumn(3,_T("进度"),LVCFMT_CENTER,120,3);
	m_pTaskList.InsertColumn(4,_T("剩余时间"),LVCFMT_CENTER,120,4);
	m_pTaskList.InsertColumn(5,_T("速度"),LVCFMT_CENTER,120,5);
	
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CFileShareDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
//
HCURSOR CFileShareDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CFileShareDlg::OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
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
	// TODO: 在此添加控件通知处理程序代码
	CString strName;
	GetDlgItemText(IDC_EDIT_SVRNAME,strName);
	if(GetSvrApp()->m_ShareInfo->SetServerName(strName))
	{
		MessageBox(_T("修改成功"));
	}
}

void CFileShareDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	//CDialog::OnOK();
}
