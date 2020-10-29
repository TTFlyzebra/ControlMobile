
// ControlMobileDlg.h : ͷ�ļ�
//
#include "resource.h"
#include "Controller.h"
#include "SoundService.h"
#include "VideoService.h"
#include "SDLWindow.h"

#pragma once


// CControlMobileDlg �Ի���
class CControlMobileDlg : public CDialogEx
{
// ����
public:
	CControlMobileDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_CONTROLMOBILE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
private:
	Controller *mController;
	SoundService *mSoundService;
	VideoService *mVideoService;
	SDLWindow *mSDLWindow;
public:
	afx_msg void OnBnClickedCloseSound();
	afx_msg void OnBnClickedCheck1();
	afx_msg void OnBnClickedPlayfile();
	afx_msg void OnBnClickedStopfile();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnDestroy();
};
