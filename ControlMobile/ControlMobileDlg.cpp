
// ControlMobileDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ControlMobile.h"
#include "ControlMobileDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
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

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CControlMobileDlg �Ի���



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
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CControlMobileDlg ��Ϣ�������

BOOL CControlMobileDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
    CWnd::SetWindowPos(NULL,0,0,520,680,SWP_NOZORDER|SWP_NOMOVE);
	GetDlgItem(IDC_VIDEO)->SetWindowPos( NULL,1,1,361,641,SWP_NOZORDER | SWP_NOMOVE );

	//��ʼ��WSA
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(sockVersion, &wsaData) != 0)
	{
		TRACE("WSAStartup error !\n");
	}

	GetDlgItem(IDC_VIDEO)->SetFocus();

	mController = new Controller();
	mSoundService = new SoundService();
	mVideoService = new VideoService();
	mSDLWindow = new SDLWindow();
	mSDLWindow->createWindow(this);	
	mVideoService->start(mSDLWindow);
	mSoundService->startPlay();		
	mController->start();

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CControlMobileDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
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
		//����Ϊ��ɫ����  
		CRect   rect;    
        CPaintDC   dc(this);    
        GetClientRect(rect);    
        dc.FillSolidRect(rect,RGB(240,240,240));  

		CRect lRect;
		GetDlgItem(IDC_VIDEO)->GetWindowRect(&lRect);
		ScreenToClient(&lRect);
		dc.FillSolidRect(lRect,RGB(0,0,0));  

		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CControlMobileDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CControlMobileDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//CDialogEx::OnOK();
	mSoundService->startPlay();	
}


void CControlMobileDlg::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	mController->stop();
	mSoundService->stopSpeak();	
	mSoundService->stopPlay();	
	mVideoService->stop();
	mSDLWindow->destory();

	CDialogEx::OnCancel();
}


void CControlMobileDlg::OnBnClickedCloseSound()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	mSoundService->stopPlay();
}


void CControlMobileDlg::OnBnClickedCheck1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


void CControlMobileDlg::OnBnClickedPlayfile()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	mSoundService->startSpeak();
}


void CControlMobileDlg::OnBnClickedStopfile()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	mSoundService->stopSpeak();
}


BOOL CControlMobileDlg::PreTranslateMessage(MSG* pMsg)
{
	bool isInVideo = GetDlgItem(IDC_VIDEO)->GetSafeHwnd() == pMsg->hwnd;
	SDL_MouseButtonEvent button;
	CRect lRect;
	GetDlgItem(IDC_VIDEO)->GetWindowRect(&lRect);
	button.button=0;
	switch (pMsg->message)
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
		if(isInVideo){
			button.type = pMsg->message==WM_LBUTTONDOWN?SDL_MOUSEBUTTONDOWN:SDL_MOUSEBUTTONUP;
			button.button = 1;
		}
		break;
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
		if(isInVideo){
			button.type = pMsg->message==WM_MBUTTONDOWN?SDL_MOUSEBUTTONDOWN:SDL_MOUSEBUTTONUP;
			button.button = 2;
		}
		break;
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
		if(isInVideo){
			button.type = pMsg->message==WM_RBUTTONDOWN?SDL_MOUSEBUTTONDOWN:SDL_MOUSEBUTTONUP;
			button.button = 3;
		}
		break;
	case WM_MOUSEHWHEEL:
		SDL_MouseWheelEvent wEvent;
		TRACE("WM_MOUSEHWHEEL\n");
		wEvent.x=pMsg->pt.x-lRect.left;
		wEvent.y=pMsg->pt.y-lRect.top;
		mController->sendMouseWheelEvent(&wEvent);
		break; 
	case WM_MOUSEMOVE:
		SDL_MouseMotionEvent mMotionEvent;
		mMotionEvent.x=pMsg->pt.x-lRect.left;
		mMotionEvent.y=pMsg->pt.y-lRect.top;
		mController->sendMouseMotionEvent(&mMotionEvent);
		break;
	}
	if(button.button>0){
		button.x=pMsg->pt.x-lRect.left;
		button.y=pMsg->pt.y-lRect.top;
		mController->sendMouseButtonEvent(&button);
	}		

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CControlMobileDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	Sleep(1000);
	delete mController;
	delete mSoundService;
	delete mVideoService;
	delete mSDLWindow;
	WSACleanup();//�ͷ���Դ�Ĳ���
	// TODO: �ڴ˴������Ϣ����������
}
