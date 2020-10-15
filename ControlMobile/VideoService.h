extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
};
#include "SDLWindow.h"
#pragma once
class VideoService
{
public:
	VideoService();
	~VideoService(void);
	void initDisplayWindow(CWnd *pCwnd);
	DWORD run();
private:
    u_char *sps;
    u_char *pps;
    AVFormatContext *pFormatCtx;
    AVCodecContext *pCodecCtx_video;
    AVCodecContext *pCodecCtx_audio;
    AVPacket *packet;
    AVFrame *frame;
	AVFrame *fly_frame;
    struct SwrContext* swr_cxt;
    u_char *audio_buf;
    bool isRun;
    bool isStop;

    uint16_t out_sampleRateInHz;
    uint16_t out_channelConfig;
    uint16_t out_audioFormat;

	static DWORD CALLBACK runThread(LPVOID);

	SDLWindow *mSDLWindow;
	CWnd *pCwnd;
public:
	HANDLE m_ffmpeg;  

};

