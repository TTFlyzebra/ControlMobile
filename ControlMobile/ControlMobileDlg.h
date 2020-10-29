
// ControlMobileDlg.h : 头文件
//
#include "resource.h"
#include "Controller.h"
#include "SoundService.h"
#include "VideoService.h"
#include "SDLWindow.h"

#pragma once


// CControlMobileDlg 对话框
class CControlMobileDlg : public CDialogEx
{
// 构造
public:
	CControlMobileDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_CONTROLMOBILE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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
