#include "stdafx.h"
#include "VideoService.h"


VideoService::VideoService(void)
{
	
	TRACE("VideoService\n");
	isStop = false;
	m_ffmpeg = CreateThread(NULL, 0, &VideoService::runThread, this, CREATE_SUSPENDED, NULL);  
	if (NULL!= m_ffmpeg) {  
		ResumeThread(m_ffmpeg);  
	}
	mSDLWindow = new SDLWindow();
}


VideoService::~VideoService(void)
{
	isStop = false;
	mSDLWindow->destory();
	TRACE("~VideoService\n");
}

void VideoService::initDisplayWindow(CWnd *p)
{
	this->pCwnd = p;
}


DWORD CALLBACK VideoService::runThread(LPVOID lp)
{
	TRACE("runThread\n");
	VideoService *mPtr = (VideoService *)lp;
	mPtr->isStop = false;
	return mPtr->run();
}

DWORD VideoService::run()
{
	TRACE("run\n");
	mSDLWindow->create(pCwnd);
	av_register_all();
	avformat_network_init();
	pFormatCtx = avformat_alloc_context();
	int ret = avformat_open_input(&pFormatCtx, "http://192.168.1.87/video/hyrz.mp4", nullptr, nullptr);
	if (ret != 0) {
		TRACE("Couldn't open file (ret:%d)\n", ret);
		return -1;
	}
	int totalSec = static_cast<int>(pFormatCtx->duration / AV_TIME_BASE);
	TRACE("video time  %dmin:%dsec\n", totalSec / 60, totalSec % 60);

	if (avformat_find_stream_info(pFormatCtx, nullptr) < 0) {
		TRACE("Could't find stream infomation\n");
		return -1;
	}
	int videoStream = -1;
	int audioStream = -1;
	for (int i = 0; i < pFormatCtx->nb_streams; i++) {
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStream = i;
            break;
        }
    }

	if (videoStream == -1) {
		TRACE("no find vedio_stream\n");
		return -1;
	}

	AVCodecParameters *pCodecPar_video = pFormatCtx->streams[videoStream]->codecpar;
	AVCodec *pCodec_video = avcodec_find_decoder(pCodecPar_video->codec_id);
	if (pCodec_video == nullptr) {
		TRACE(" not found decodec.\n");
		return -1;
	}
	pCodecCtx_video = avcodec_alloc_context3(pCodec_video);
	ret = avcodec_parameters_to_context(pCodecCtx_video, pCodecPar_video);
	if (ret < 0) {
		TRACE("avcodec_parameters_to_context() failed %d\n", ret);
		return -1;
	}
	if (avcodec_open2(pCodecCtx_video, pCodec_video, nullptr) < 0) {
		TRACE("Could not open decodec.\n");
		return -1;
	}

	//音频使用软解，初始化音频解码，音频不是必须存在
	for (int i = 0; i < pFormatCtx->nb_streams; i++) {
		if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
			audioStream = i;
			AVCodecParameters *pCodecPar_audio = pFormatCtx->streams[i]->codecpar;
			AVCodec *pCodec_audio = avcodec_find_decoder(pCodecPar_audio->codec_id);
			if (pCodec_audio != nullptr) {
				pCodecCtx_audio = avcodec_alloc_context3(pCodec_audio);
				ret = avcodec_parameters_to_context(pCodecCtx_audio, pCodecPar_audio);
			} else {
				TRACE("init audio codec failed 1!\n");
			}
			if (ret >= 0) {
				if (avcodec_open2(pCodecCtx_audio, pCodec_audio, nullptr) >= 0) {
					TRACE("find audioStream = %d, sampleRateInHz = %d, channelConfig=%d, audioFormat=%d\n",
						i, pCodecCtx_audio->sample_rate, pCodecCtx_audio->channels,
						pCodecCtx_audio->sample_fmt);
					swr_cxt = swr_alloc();
					swr_alloc_set_opts(
						swr_cxt,
						out_channelConfig,
						(AVSampleFormat) out_audioFormat,
						out_sampleRateInHz,
						pCodecCtx_audio->channel_layout,
						pCodecCtx_audio->sample_fmt,
						pCodecCtx_audio->sample_rate,
						0,
						nullptr);
					swr_init(swr_cxt);
					audio_buf = (uint8_t *) av_malloc(out_sampleRateInHz * 8);
					//callBack->javaOnAudioInfo(out_sampleRateInHz, out_channelConfig, out_audioFormat);
				} else {
					avcodec_close(pCodecCtx_audio);
					TRACE("init audio codec failed 2!\n");
				}
			} else {
				TRACE("init audio codec failed 3!\n");
			}
			break;
		}
	}
	frame = av_frame_alloc();
	packet = (AVPacket *) av_malloc(sizeof(AVPacket)); //分配一个packet
	while (!isStop && av_read_frame(pFormatCtx, packet) >= 0) {
		if (packet->stream_index == videoStream) {
			//软解视频
			//TODO::时间同步
			ret = avcodec_send_packet(pCodecCtx_video, packet);
			while (ret >= 0) {
				ret = avcodec_receive_frame(pCodecCtx_video, frame);
				if (ret >= 0) {
					//TRACE("frame->width=%d,frame->height=%d\n",frame->width,frame->height);
					auto * video_buf = (u_char *) malloc((frame->width*frame->height* 3 / 2) * sizeof(u_char));
					int start = 0;
                    memcpy(video_buf,frame->data[0],frame->width*frame->height);
                    start=start+frame->width*frame->height;
                    memcpy(video_buf + start,frame->data[1],frame->width*frame->height/4);
                    start=start+frame->width*frame->height/4;
                    memcpy(video_buf + start,frame->data[2],frame->width*frame->height/4);
					mSDLWindow->upData(video_buf);					
					free(video_buf);
				}
			}
		} else if (packet->stream_index == audioStream) {
			//ret = avcodec_send_packet(pCodecCtx_audio, packet);
			//while (ret >= 0) {
			//	ret = avcodec_receive_frame(pCodecCtx_audio, frame);
			//	if (ret >= 0) {
			//		int len = swr_convert(
			//			swr_cxt,
			//			&audio_buf,
			//			frame->nb_samples,
			//			(const uint8_t **) frame->data,
			//			frame->nb_samples);
			//		int out_buf_size = av_samples_get_buffer_size(
			//			NULL,
			//			av_get_channel_layout_nb_channels(out_channelConfig),
			//			frame->nb_samples,
			//			(AVSampleFormat) out_audioFormat,
			//			0);
			//		int size = out_buf_size * out_sampleRateInHz / frame->sample_rate;
			//		//callBack->javaOnAudioDecode(audio_buf, size);
			//	}
			//}
		}
		av_packet_unref(packet);
	}
	if (!isStop) {
		//callBack->javaOnStop();
	}
	free(sps);
	free(pps);
	swr_free(&swr_cxt);
	av_free(audio_buf);
	av_free(packet);
	av_frame_free(&frame);
	avcodec_close(pCodecCtx_video);
	avcodec_close(pCodecCtx_audio);
	avformat_close_input(&pFormatCtx);
	mSDLWindow->destory();
	return 0;
}