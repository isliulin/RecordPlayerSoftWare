#include "StdAfx.h"
#include "ZPlay.h"




CZPlay::CZPlay(void)
{
    m_Player=NULL;
	InitObject();
}

CZPlay::~CZPlay(void)
{
    DestroyObject();
}

BOOL  CZPlay::InitObject()
{
    if (m_Player==NULL)
    {
		m_Player = CreateZPlay();
    }

	if (m_Player==NULL)
	{
		return FALSE;
	}

	return TRUE;
}

void  CZPlay::DestroyObject()
{
	if (m_Player)
	{
		m_Player->Release(); // destroy class
	}
	
}

BOOL CZPlay::OpenStream(char* buf, unsigned int len,TStreamFormat  tsformat)
{
	  int result= m_Player->OpenStream(1, 1, buf, len, tsformat);

	  if (result==0)
	  {
		  
			  
			  m_Player->Release();

			  return FALSE;
			  
			  
	  }

	  return TRUE;

}

BOOL CZPlay::Play()
{
	return m_Player->Play();
}