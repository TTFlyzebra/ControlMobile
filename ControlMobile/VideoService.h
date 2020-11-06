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

static bool isRunning;
static bool isStop;

class VideoService
{
public:
	VideoService();
	~VideoService(void);
	static int VideoService::interrupt_cb(void *ctx);
	void start(SDLWindow *mSDLWindow);
	void stop();
private:    

    uint16_t out_sampleRateInHz;
    uint16_t out_channelConfig;
    uint16_t out_audioFormat;

	HANDLE pid_ffplay;  
	static DWORD CALLBACK ffplayThread(LPVOID);
	DWORD ffplay();

	SDLWindow *mSDLWindow;
};

