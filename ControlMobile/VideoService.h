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
	void start(SDLWindow *mSDLWindow);
	void stop();
private:    
    bool isRun;
    bool isStop;

    uint16_t out_sampleRateInHz;
    uint16_t out_channelConfig;
    uint16_t out_audioFormat;

	HANDLE pid_ffplay;  
	static DWORD CALLBACK ffplayThread(LPVOID);
	DWORD ffplay();

	SDLWindow *mSDLWindow;
};

