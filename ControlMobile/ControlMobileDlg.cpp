
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

	//��ʼ��WSA
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
	delete mController;
	mSoundService->stopSpeak();	
	mSoundService->stopPlay();	
	delete mSoundService;
	mVideoService->stop();
	delete mVideoService;
	mSDLWindow->destory();
	delete mSDLWindow;

	WSACleanup();//�ͷ���Դ�Ĳ���

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
	// TODO: �ڴ����ר�ô����/����û���

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
