
// ControlMobileDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ControlMobile.h"
#include "ControlMobileDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
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

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CControlMobileDlg 对话框



CControlMobileDlg::CControlMobileDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CControlMobileDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CControlMobileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CControlMobileDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CControlMobileDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CControlMobileDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_CLOSE_SOUND, &CControlMobileDlg::OnBnClickedCloseSound)
	ON_BN_CLICKED(IDC_CHECK1, &CControlMobileDlg::OnBnClickedCheck1)
	ON_BN_CLICKED(IDC_PLAYFILE, &CControlMobileDlg::OnBnClickedPlayfile)
	ON_BN_CLICKED(IDC_STOPFILE, &CControlMobileDlg::OnBnClickedStopfile)
END_MESSAGE_MAP()


// CControlMobileDlg 消息处理程序

BOOL CControlMobileDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
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

	//初始化WSA
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(sockVersion, &wsaData) != 0)
	{
		TRACE("WSAStartup error !\n");
	}

	mController = new Controller();
	mSoundService = new SoundService();
	mVideoService = new VideoService();
	mSDLWindow = new SDLWindow();

	mController->start();
	mVideoService->start(mSDLWindow);
	mSoundService->startPlay();	
	mSDLWindow->createWindow(this);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CControlMobileDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CControlMobileDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
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
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CControlMobileDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CControlMobileDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	//CDialogEx::OnOK();
	mSoundService->startPlay();	
}


void CControlMobileDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	mController->stop();
	delete mController;
	mSoundService->stopSpeak();	
	mSoundService->stopPlay();	
	delete mSoundService;
	mVideoService->stop();
	delete mVideoService;
	mSDLWindow->destory();
	delete mSDLWindow;

	WSACleanup();//释放资源的操作

	CDialogEx::OnCancel();
}


void CControlMobileDlg::OnBnClickedCloseSound()
{
	// TODO: 在此添加控件通知处理程序代码
	mSoundService->stopPlay();
}


void CControlMobileDlg::OnBnClickedCheck1()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CControlMobileDlg::OnBnClickedPlayfile()
{
	// TODO: 在此添加控件通知处理程序代码
	mSoundService->startSpeak();
}


void CControlMobileDlg::OnBnClickedStopfile()
{
	// TODO: 在此添加控件通知处理程序代码
	mSoundService->stopSpeak();
}


BOOL CControlMobileDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类

	if(pMsg->message==WM_LBUTTONDOWN)
	{
		TRACE("WM_LBUTTONDOWN\n");
	}

	if(pMsg->message==WM_LBUTTONUP)
	{
		TRACE("WM_LBUTTONUP\n");
	}
	if(pMsg->message==WM_RBUTTONDOWN)
	{
		TRACE("WM_RBUTTONDOWN\n");
	}

	if(pMsg->message==WM_RBUTTONUP)
	{
		TRACE("WM_RBUTTONUP\n");
	}
	if(pMsg->message==WM_KEYUP)
	{
		TRACE("WM_KEYUP\n");
	}

	if(pMsg->message==WM_KEYDOWN)
	{
		TRACE("WM_KEYDOWN\n");
	}


	return CDialogEx::PreTranslateMessage(pMsg);
}
