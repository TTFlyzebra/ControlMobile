extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
};
#include "SDLWindow.h"
#pragma once

struct videoBuffer
{
	char *yuvData[1280*720*3/2];
	int size;
};

class VideoService
{
public:
	VideoService();
	~VideoService(void);
	void initDisplayWindow(CWnd *pCwnd);
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

	HANDLE pid_ffplay;  
	static DWORD CALLBACK ffplayThread(LPVOID);
	DWORD ffplay();

	HANDLE pid_sdlplay;  
	static DWORD CALLBACK sdlplayThread(LPVOID);
	DWORD sdlplay();

	SDLWindow *mSDLWindow;
	CWnd *pCwnd;
	

};

