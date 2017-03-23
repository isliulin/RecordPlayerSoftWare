#ifndef _PROTOCOL_H
#define _PROTOCOL_H

#pragma once
//定义通信协议
//定义TCP数据包头
#pragma pack(push)//保存对齐状态
#pragma pack(1)

#define PACK_HEAD_LENGTH  8 //包头的长度
#define MAX_PACKAGE 1400

#define  FILE_TRANSFER_INFO  0x02//文件信息
#define  FILE_TRANSFER_START 0x03//文件传输开始
#define  FILE_TRANSFER_ABORT 0x04 //文件传输终止


typedef struct tagTDGRAMHEADER
{
   BYTE Tag[2];  //标记：默认0x00,0xFF
   BYTE Comand;  //指令码：0x01--请求文件下载//0x02--文件信息//-//0x03--请开始发送文件数据
   BYTE Reverse;
   BYTE DataLen[4]; //数据包长度
   BYTE rdata[MAX_PACKAGE]; //数据
} TDGRAMHEADER, *PTDGRAMHEADER;



#pragma pack(pop)//保存对齐状态

#endif