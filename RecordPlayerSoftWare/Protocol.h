#ifndef _PROTOCOL_H
#define _PROTOCOL_H

#pragma once
//����ͨ��Э��
//����TCP���ݰ�ͷ
#pragma pack(push)//�������״̬
#pragma pack(1)

#define PACK_HEAD_LENGTH  8 //��ͷ�ĳ���
#define MAX_PACKAGE 1400

#define  FILE_TRANSFER_INFO  0x02//�ļ���Ϣ
#define  FILE_TRANSFER_START 0x03//�ļ����俪ʼ
#define  FILE_TRANSFER_ABORT 0x04 //�ļ�������ֹ


typedef struct tagTDGRAMHEADER
{
   BYTE Tag[2];  //��ǣ�Ĭ��0x00,0xFF
   BYTE Comand;  //ָ���룺0x01--�����ļ�����//0x02--�ļ���Ϣ//-//0x03--�뿪ʼ�����ļ�����
   BYTE Reverse;
   BYTE DataLen[4]; //���ݰ�����
   BYTE rdata[MAX_PACKAGE]; //����
} TDGRAMHEADER, *PTDGRAMHEADER;



#pragma pack(pop)//�������״̬

#endif