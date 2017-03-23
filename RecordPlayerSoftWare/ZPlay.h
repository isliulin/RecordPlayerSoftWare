#pragma once

#include "libzplay.h"

using namespace libZPlay;



class CZPlay
{
public:
	CZPlay(void);
	~CZPlay(void);

	BOOL  InitObject();
	void  DestroyObject();

	BOOL  OpenStream(char* buf, unsigned int len,TStreamFormat  tsformat);

	BOOL  Play();  //开始播放

    
   
public:
	
	ZPlay* m_Player; //播放类实例


};
