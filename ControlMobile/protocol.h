//
// Created by FlyZebra on 2020/7/30 0030.
//

#ifndef ANDROID_PROTOCOL_H
#define ANDROID_PROTOCOL_H

#if __cplusplus
extern "C" {
#endif
//��Ƶ���Žӿ�
//׼�����Žӿڣ��ֻ�-->PC ��
//��ʼ����	2�ֽ�	Unsigned Integer	�̶��Ŀ�ʼ����0x7ea5
//��Ϣ����	4�ֽ�	Unsigned Integer	��Ϣ�ܳ��ȣ���������ʼ�ͽ�����
//Э��汾	2�ֽ�	Unsigned Integer	Э��汾�ţ���λ��ǰ����1��ʼ
//��ϢID	2�ֽ�	Unsigned Integer	�����0x044a
//������	2�ֽ�	Unsigned Integer	�����ʣ���λHz���磺16000Hz
//ͨ����	2�ֽ�	Unsigned Integer	��������4 ˫������   12
//��Ƶ��ʽ	2�ֽ�	Unsigned Integer	2��ENCODING_PCM_16BIT 3��ENCODING_PCM_8BIT 4��ENCODING_PCM_FLOAT
//��������	2�ֽ�	Unsigned Integer	�̶��Ľ�������0x7e0d
const static byte PC_START_PLAY[18] = {0x7e,0xa5,0x00,0x00,0x00,0x0e,0x00,0x01,0x04,0x4a,0x3e,0x80,0x00,0x0c,0x00,0x02,0x7e,0x0d};

//��Ƶ���ݴ���ӿڣ��ֻ�-->PC ��
//��ʼ����	2�ֽ�	Unsigned Integer	�̶��Ŀ�ʼ����0x7ea5
//��Ϣ����	4�ֽ�	Unsigned Integer	��Ϣ�ܳ��ȣ���������ʼ�ͽ�����
//Э��汾	2�ֽ�	Unsigned Integer	Э��汾�ţ���λ��ǰ����1��ʼ
//��ϢID	2�ֽ�	Unsigned Integer	�����0x044b
//���ݳ���	4�ֽ�	Unsigned Integer	data len
//����	x	byte	��Ƶ����
//��������	2�ֽ�	Unsigned Integer	�̶��Ľ�������0x7e0d
const static byte PC_PLAY_DATA[16] = {0x7e,0xa5,0x00,0x00,0x00,0x0c,0x00,0x01,0x04,0x4b,0x00,0x00,0x00,0x00,0x7e,0x0d};

//��ͣ���Žӿڣ��ֻ�-->PC ��
//��ʼ����	2�ֽ�	Unsigned Integer	�̶��Ŀ�ʼ����0x7ea5
//��Ϣ����	4�ֽ�	Unsigned Integer	��Ϣ�ܳ��ȣ���������ʼ�ͽ�����
//Э��汾	2�ֽ�	Unsigned Integer	Э��汾�ţ���λ��ǰ����1��ʼ
//��ϢID	2�ֽ�	Unsigned Integer	�����0x044c
//��������	2�ֽ�	Unsigned Integer	�̶��Ľ�������0x7e0d
const static byte PC_PAUSE_PLAY[12] = {0x7e,0xa5,0x00,0x00,0x00,0x08,0x00,0x01,0x04,0x4c,0x7e,0x0d};

//������ɽӿ� ���ֻ�-->PC ��
//��ʼ����	2�ֽ�	Unsigned Integer	�̶��Ŀ�ʼ����0x7ea5
//��Ϣ����	4�ֽ�	Unsigned Integer	��Ϣ�ܳ��ȣ���������ʼ�ͽ�����
//Э��汾	2�ֽ�	Unsigned Integer	Э��汾�ţ���λ��ǰ����1��ʼ
//��ϢID	2�ֽ�	Unsigned Integer	�����0x044d
//��������	2�ֽ�	Unsigned Integer	�̶��Ľ�������0x7e0d
const static byte PC_STOP_PLAY[12] = {0x7e,0xa5,0x00,0x00,0x00,0x08,0x00,0x01,0x04,0x4d,0x7e,0x0d};

//��Ƶ¼�ƽӿ�
//׼��¼���ӿ� ���ֻ�-->PC ��
//��ʼ����	2�ֽ�	Unsigned Integer	�̶��Ŀ�ʼ����0x7ea5
//��Ϣ����	4�ֽ�	Unsigned Integer	��Ϣ�ܳ��ȣ���������ʼ�ͽ�����
//Э��汾	2�ֽ�	Unsigned Integer	Э��汾�ţ���λ��ǰ����1��ʼ
//��ϢID	2�ֽ�	Unsigned Integer	�����0x0450
//������	2�ֽ�	Unsigned Integer	�����ʣ���λHz���磺16000Hz
//ͨ����	2�ֽ�	Unsigned Integer	��������4 ˫������12
//��Ƶ��ʽ	2�ֽ�	Unsigned Integer	2��ENCODING_PCM_16BIT 3��ENCODING_PCM_8BIT 4��ENCODING_PCM_FLOAT
//�����С	2�ֽ�	Unsigned Integer	�����ֽ���
//��������	2�ֽ�	Unsigned Integer	�̶��Ľ�������0x7e0d
const static byte PC_OPEN_SPEAK[20] = {0x7e,0xa5,0x00,0x00,0x00,0x10,0x00,0x01,0x04,0x50,0x1f,0x40,0x00,0x0c,0x00,0x02,0x01,0x40,0x7e,0x0d};

//��Ƶ���ݽӿڣ�PC��>�ֻ� ��
//��ʼ����	2�ֽ�	Unsigned Integer	�̶��Ŀ�ʼ����0x7ea5
//��Ϣ����	4�ֽ�	Unsigned Integer	��Ϣ�ܳ��ȣ���������ʼ�ͽ�����
//Э��汾	2�ֽ�	Unsigned Integer	Э��汾�ţ���λ��ǰ����1��ʼ
//��ϢID	2�ֽ�	Unsigned Integer	�����0x0451
//���ݳ���	4�ֽ�	Unsigned Integer	data len
//����	x	byte	��Ƶ����
//��������	2�ֽ�	Unsigned Integer	�̶��Ľ�������0x7e0d
const static byte PC_SPEAK_DATA[16] = {0x7e,0xa5,0x00,0x00,0x00,0x08,0x00,0x01,0x04,0x51,0x00,0x00,0x00,0x00,0x7e,0x0d};

//¼�������ӿڣ��ֻ�-->PC ��
//��ʼ����	2�ֽ�	Unsigned Integer	�̶��Ŀ�ʼ����0x7ea5
//��Ϣ����	4�ֽ�	Unsigned Integer	��Ϣ�ܳ��ȣ���������ʼ�ͽ�����
//Э��汾	2�ֽ�	Unsigned Integer	Э��汾�ţ���λ��ǰ����1��ʼ
//��ϢID	2�ֽ�	Unsigned Integer	�����0x0452
//��������	2�ֽ�	Unsigned Integer	�̶��Ľ�������0x7e0d
const static byte PC_CLOSE_SPEAK[12] = {0x7e,0xa5,0x00,0x00,0x00,0x08,0x00,0x01,0x04,0x52,0x7e,0x0d};

#if __cplusplus
};  // extern "C"
#endif
#endif //ANDROID_PROTOCOL_H
