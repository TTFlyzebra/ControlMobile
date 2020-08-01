//
// Created by FlyZebra on 2020/7/30 0030.
//

#ifndef ANDROID_PROTOCOL_H
#define ANDROID_PROTOCOL_H

#if __cplusplus
extern "C" {
#endif
//音频播放接口
//准备播放接口（手机-->PC ）
//开始符号	2字节	Unsigned Integer	固定的开始符号0x7ea5
//消息长度	4字节	Unsigned Integer	消息总长度，不包括开始和结束符
//协议版本	2字节	Unsigned Integer	协议版本号，高位在前，从1开始
//消息ID	2字节	Unsigned Integer	命令号0x044a
//采样率	2字节	Unsigned Integer	采样率，单位Hz，如：16000Hz
//通道数	2字节	Unsigned Integer	单声道：4 双声道：   12
//音频格式	2字节	Unsigned Integer	2：ENCODING_PCM_16BIT 3：ENCODING_PCM_8BIT 4：ENCODING_PCM_FLOAT
//结束符号	2字节	Unsigned Integer	固定的结束符号0x7e0d
const static byte PC_START_PLAY[18] = {0x7e,0xa5,0x00,0x00,0x00,0x0e,0x00,0x01,0x04,0x4a,0x3e,0x80,0x00,0x0c,0x00,0x02,0x7e,0x0d};

//音频数据传输接口（手机-->PC ）
//开始符号	2字节	Unsigned Integer	固定的开始符号0x7ea5
//消息长度	4字节	Unsigned Integer	消息总长度，不包括开始和结束符
//协议版本	2字节	Unsigned Integer	协议版本号，高位在前，从1开始
//消息ID	2字节	Unsigned Integer	命令号0x044b
//数据长度	4字节	Unsigned Integer	data len
//数据	x	byte	音频数据
//结束符号	2字节	Unsigned Integer	固定的结束符号0x7e0d
const static byte PC_PLAY_DATA[16] = {0x7e,0xa5,0x00,0x00,0x00,0x0c,0x00,0x01,0x04,0x4b,0x00,0x00,0x00,0x00,0x7e,0x0d};

//暂停播放接口（手机-->PC ）
//开始符号	2字节	Unsigned Integer	固定的开始符号0x7ea5
//消息长度	4字节	Unsigned Integer	消息总长度，不包括开始和结束符
//协议版本	2字节	Unsigned Integer	协议版本号，高位在前，从1开始
//消息ID	2字节	Unsigned Integer	命令号0x044c
//结束符号	2字节	Unsigned Integer	固定的结束符号0x7e0d
const static byte PC_PAUSE_PLAY[12] = {0x7e,0xa5,0x00,0x00,0x00,0x08,0x00,0x01,0x04,0x4c,0x7e,0x0d};

//播放完成接口 （手机-->PC ）
//开始符号	2字节	Unsigned Integer	固定的开始符号0x7ea5
//消息长度	4字节	Unsigned Integer	消息总长度，不包括开始和结束符
//协议版本	2字节	Unsigned Integer	协议版本号，高位在前，从1开始
//消息ID	2字节	Unsigned Integer	命令号0x044d
//结束符号	2字节	Unsigned Integer	固定的结束符号0x7e0d
const static byte PC_STOP_PLAY[12] = {0x7e,0xa5,0x00,0x00,0x00,0x08,0x00,0x01,0x04,0x4d,0x7e,0x0d};

//音频录制接口
//准备录音接口 （手机-->PC ）
//开始符号	2字节	Unsigned Integer	固定的开始符号0x7ea5
//消息长度	4字节	Unsigned Integer	消息总长度，不包括开始和结束符
//协议版本	2字节	Unsigned Integer	协议版本号，高位在前，从1开始
//消息ID	2字节	Unsigned Integer	命令号0x0450
//采样率	2字节	Unsigned Integer	采样率，单位Hz，如：16000Hz
//通道数	2字节	Unsigned Integer	单声道：4 双声道：12
//音频格式	2字节	Unsigned Integer	2：ENCODING_PCM_16BIT 3：ENCODING_PCM_8BIT 4：ENCODING_PCM_FLOAT
//缓冲大小	2字节	Unsigned Integer	缓冲字节数
//结束符号	2字节	Unsigned Integer	固定的结束符号0x7e0d
const static byte PC_OPEN_SPEAK[20] = {0x7e,0xa5,0x00,0x00,0x00,0x10,0x00,0x01,0x04,0x50,0x1f,0x40,0x00,0x0c,0x00,0x02,0x01,0x40,0x7e,0x0d};

//音频数据接口（PC—>手机 ）
//开始符号	2字节	Unsigned Integer	固定的开始符号0x7ea5
//消息长度	4字节	Unsigned Integer	消息总长度，不包括开始和结束符
//协议版本	2字节	Unsigned Integer	协议版本号，高位在前，从1开始
//消息ID	2字节	Unsigned Integer	命令号0x0451
//数据长度	4字节	Unsigned Integer	data len
//数据	x	byte	音频数据
//结束符号	2字节	Unsigned Integer	固定的结束符号0x7e0d
const static byte PC_SPEAK_DATA[16] = {0x7e,0xa5,0x00,0x00,0x00,0x08,0x00,0x01,0x04,0x51,0x00,0x00,0x00,0x00,0x7e,0x0d};

//录音结束接口（手机-->PC ）
//开始符号	2字节	Unsigned Integer	固定的开始符号0x7ea5
//消息长度	4字节	Unsigned Integer	消息总长度，不包括开始和结束符
//协议版本	2字节	Unsigned Integer	协议版本号，高位在前，从1开始
//消息ID	2字节	Unsigned Integer	命令号0x0452
//结束符号	2字节	Unsigned Integer	固定的结束符号0x7e0d
const static byte PC_CLOSE_SPEAK[12] = {0x7e,0xa5,0x00,0x00,0x00,0x08,0x00,0x01,0x04,0x52,0x7e,0x0d};

#if __cplusplus
};  // extern "C"
#endif
#endif //ANDROID_PROTOCOL_H
