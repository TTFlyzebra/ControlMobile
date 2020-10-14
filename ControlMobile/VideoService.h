extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
};
#pragma once
class VideoService
{
public:
	VideoService(void);
	~VideoService(void);
	DWORD run();
private:
    u_char *sps;
    u_char *pps;
    AVFormatContext *pFormatCtx;
    AVCodecContext *pCodecCtx_video;
    AVCodecContext *pCodecCtx_audio;
    AVPacket *packet;
    AVFrame *frame;
    struct SwrContext* swr_cxt;
    u_char *audio_buf;
    bool isRun;
    bool isStop;

    uint16_t out_sampleRateInHz;
    uint16_t out_channelConfig;
    uint16_t out_audioFormat;

	static DWORD CALLBACK runThread(LPVOID); 
public:
	HANDLE m_ffmpeg;  

};

