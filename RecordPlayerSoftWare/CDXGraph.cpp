//
// CDXGraph.cpp
//

#include "stdafx.h"
#include <streams.h>
#include "CDXGraph.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#pragma comment(lib,".\\depend\\lib\\strmbasd.lib")
#pragma comment(lib,".\\depend\\lib\\uuid.lib")
#pragma comment(lib,".\\depend\\lib\\winmm.lib")


////////////////////////////////////////////////////////////////////////////////
CDXGraph::CDXGraph()
{
	mGraph        = NULL;
	mMediaControl = NULL;
	mEvent        = NULL;
	mBasicVideo   = NULL;
	mBasicAudio   = NULL;
	mVideoWindow  = NULL;
	mSeeking      = NULL;

	mObjectTableEntry = 0;
}

CDXGraph::~CDXGraph()
{
	Release();
}

bool CDXGraph::Create(void)
{
	if (!mGraph)
	{
		//create the Filter Graph Manager
		if (SUCCEEDED(CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER,
			IID_IGraphBuilder, (void **)&mGraph)))
		{
			
			   //注册滤波图表管理器
			   AddToObjectTable();
			   return QueryInterfaces();
		}
		mGraph = 0;
	}
	return false;
}


HRESULT CDXGraph::AddFilterByCLSID(IGraphBuilder *pGraph,  const GUID& clsid, LPCWSTR wszName, IBaseFilter **ppF,bool bsoureflag) 
{  
	if(!pGraph || ! ppF)
		return E_POINTER;  
	*ppF = 0;  
	IBaseFilter *pF = 0;  
	HRESULT hr = CoCreateInstance(clsid,0,CLSCTX_INPROC_SERVER,IID_IBaseFilter,reinterpret_cast<LPVOID*>(&pF));  
	if (SUCCEEDED(hr))  
	{   
	  if (bsoureflag)
	  {

		   hr = pGraph->AddSourceFilter(wszName,wszName,&pF);  
	  }
	  else
	  {
           hr = pGraph->AddFilter(pF, wszName);  
	   
	  }
	  if (SUCCEEDED(hr))  
		  *ppF = pF;  
	  else  
		  pF->Release();
    }  
	return hr;  
}  

bool CDXGraph::QueryInterfaces(void)
{
	
	if (mGraph)
	{
		//查询组件是否支持列出的接口
		HRESULT hr = NOERROR;
		hr |= mGraph->QueryInterface(IID_IMediaControl, (void **)&mMediaControl);
		hr |= mGraph->QueryInterface(IID_IMediaEventEx, (void **)&mEvent);
		hr |= mGraph->QueryInterface(IID_IBasicVideo, (void **)&mBasicVideo);
		hr |= mGraph->QueryInterface(IID_IBasicAudio, (void **)&mBasicAudio);
		hr |= mGraph->QueryInterface(IID_IVideoWindow, (void **)&mVideoWindow);
		hr |= mGraph->QueryInterface(IID_IMediaSeeking, (void **)&mSeeking);
		if (mSeeking)
		{
			//设置时间格式：用于播放速度和位置控制
			mSeeking->SetTimeFormat(&TIME_FORMAT_MEDIA_TIME);
		}
		return SUCCEEDED(hr);
	}
	return false;
}

void CDXGraph::Release(void)
{
	if (mSeeking)
	{
		mSeeking->Release();
		mSeeking = NULL;
	}
	if (mMediaControl)
	{
		mMediaControl->Release();
		mMediaControl = NULL;
	}
	if (mEvent)
	{
		mEvent->Release();
		mEvent = NULL;
	}
	if (mBasicVideo)
	{
		mBasicVideo->Release();
		mBasicVideo = NULL;
	}
	if (mBasicAudio)
	{
		mBasicAudio->Release();
		mBasicAudio = NULL;
	}
	if (mVideoWindow)
	{
		mVideoWindow->put_Visible(OAFALSE);
		mVideoWindow->put_MessageDrain((OAHWND)NULL);
		mVideoWindow->put_Owner(OAHWND(0));
		mVideoWindow->Release();
		mVideoWindow = NULL;
	}
	RemoveFromObjectTable();
	if (mGraph) 
	{
		mGraph->Release(); 
		mGraph = NULL;
	}
}

bool CDXGraph::Attach(IGraphBuilder * inGraphBuilder)
{
	Release();

	if (inGraphBuilder)
	{
		inGraphBuilder->AddRef();
		mGraph = inGraphBuilder;

		AddToObjectTable();
		return QueryInterfaces();
	}
	return true;
}

IGraphBuilder * CDXGraph::GetGraph(void)
{
	return mGraph;
}

IMediaEventEx * CDXGraph::GetEventHandle(void)
{
	return mEvent;
}

HRESULT CDXGraph::GetUnconnectedPin(IBaseFilter* pFilter,PIN_DIRECTION PinDir,IPin**ppPin)
{

	*ppPin=NULL;
	IEnumPins* pEnum=NULL;
	IPin* pPin=NULL;
	HRESULT hr=pFilter->EnumPins(&pEnum);
	if (FAILED(hr))
	{
		return hr;
	}

	while(pEnum->Next(1,&pPin,NULL)==S_OK)
	{
		PIN_DIRECTION ThisPinDir;
		pPin->QueryDirection(&ThisPinDir);
		if (ThisPinDir==PinDir)
		{
			IPin* pTmp=NULL;
			hr=pPin->ConnectedTo(&pTmp);
			if (SUCCEEDED(hr))
			{
				pTmp->Release();
			}
			else
			{
				pEnum->Release();
				*ppPin=pPin;
				return S_OK;

			}
		}

		pPin->Release();

	}

	pEnum->Release();

	return E_FAIL;


}
// Connect filter from the upstream output pin to the downstream input pin
bool CDXGraph::ConnectFilters(IPin * inOutputPin, IPin * inInputPin, 
							  const AM_MEDIA_TYPE * inMediaType)
{
	if (mGraph && inOutputPin && inInputPin)
	{
		HRESULT hr = mGraph->ConnectDirect(inOutputPin, inInputPin, inMediaType);
		return SUCCEEDED(hr) ? true : false;
	}
	return false;
}

void CDXGraph::DisconnectFilters(IPin * inOutputPin)
{
	if (mGraph && inOutputPin)
	{
		HRESULT hr = mGraph->Disconnect(inOutputPin);
	}
}

bool CDXGraph::SetDisplayWindow(HWND inWindow)
{	
	if (mVideoWindow)
	{
	//	long lVisible;
	//	mVideoWindow->get_Visible(&lVisible);
		// Hide the video window first
		mVideoWindow->put_Visible(OAFALSE);
		mVideoWindow->put_Owner((OAHWND)inWindow);

		RECT windowRect;
		::GetClientRect(inWindow, &windowRect);
		mVideoWindow->put_Left(0);
		mVideoWindow->put_Top(0);
		mVideoWindow->put_Width(windowRect.right - windowRect.left);
		mVideoWindow->put_Height(windowRect.bottom - windowRect.top);
		mVideoWindow->put_WindowStyle(WS_CHILD|WS_CLIPCHILDREN|WS_CLIPSIBLINGS);
		//mVideoWindow->SetWindowForeground(OATRUE);

		mVideoWindow->put_MessageDrain((OAHWND) inWindow);
		// Restore the video window
		if (inWindow != NULL)
		{
			mVideoWindow->put_Visible(OATRUE);
		}
		else
		{
			mVideoWindow->put_Visible(OAFALSE);
		}
		return true;
	}
	return false;
}

bool CDXGraph::ResizeVideoWindow(long inLeft, long inTop, long inWidth, long inHeight)
{
	if (mVideoWindow)
	{
		long lVisible = OATRUE;
		mVideoWindow->get_Visible(&lVisible);
		// Hide the video window first
		mVideoWindow->put_Visible(OAFALSE);

		mVideoWindow->put_Left(inLeft);
		mVideoWindow->put_Top(inTop);
		mVideoWindow->put_Width(inWidth);
		mVideoWindow->put_Height(inHeight);
	
		// Restore the video window
		mVideoWindow->put_Visible(lVisible);
		return true;
	}
	return false;
}

bool CDXGraph::SetNotifyWindow(HWND inWindow)
{
	if (mEvent)
	{
		mEvent->SetNotifyWindow((OAHWND)inWindow, WM_GRAPHNOTIFY, 0);
		return true;
	}
	return false;
}

void CDXGraph::HandleEvent(WPARAM inWParam, LPARAM inLParam)
{
	if (mEvent)
	{
		LONG eventCode = 0, eventParam1 = 0, eventParam2 = 0;
		while (SUCCEEDED(mEvent->GetEvent(&eventCode, &eventParam1, &eventParam2, 0)))
		{
			mEvent->FreeEventParams(eventCode, eventParam1, eventParam2);
			switch (eventCode)
			{
			case EC_COMPLETE:
				break;

			case EC_USERABORT:
			case EC_ERRORABORT:
				break;

			default:
				break;
			}
		}
	}
}

bool CDXGraph::Run(void)
{
	if (mGraph && mMediaControl)
	{
		if (!IsRunning())
		{
			if (SUCCEEDED(mMediaControl->Run()))
			{
				return true;
			}
		}
		else
		{
			return true;
		}
	}
	return false;
}

bool CDXGraph::Stop(void)
{
	if (mGraph && mMediaControl)
	{
		if (!IsStopped())
		{	
			if (SUCCEEDED(mMediaControl->Stop()))
			{
				return true;
			}
		}
		else
		{
			return true;
		}
	}
	return false;
}

bool CDXGraph::Pause(void)
{
	if (mGraph && mMediaControl)
	{
		if (!IsPaused())
		{	
			if (SUCCEEDED(mMediaControl->Pause()))
			{
				return true;
			}
		}
		else
		{
			return true;
		}
	}
	return false;
}

bool CDXGraph::IsRunning(void)
{
	if (mGraph && mMediaControl)
	{
		OAFilterState state = State_Stopped;
		if (SUCCEEDED(mMediaControl->GetState(10, &state)))
		{
			return state == State_Running;
		}
	}
	return false;
}

bool CDXGraph::IsStopped(void)
{
	if (mGraph && mMediaControl)
	{
		OAFilterState state = State_Stopped;
		if (SUCCEEDED(mMediaControl->GetState(10, &state)))
		{
			return state == State_Stopped;
		}
	}
	return false;
}

bool CDXGraph::IsPaused(void)
{
	if (mGraph && mMediaControl)
	{
		OAFilterState state = State_Stopped;
		if (SUCCEEDED(mMediaControl->GetState(10, &state)))
		{
			return state == State_Paused;
		}
	}
	return false;
}

bool CDXGraph::SetFullScreen(BOOL inEnabled)
{
	if (mVideoWindow)
	{
		HRESULT hr = mVideoWindow->put_FullScreenMode(inEnabled ? OATRUE : OAFALSE);
		return SUCCEEDED(hr);
	}
	return false;
}

bool CDXGraph::GetFullScreen(void)
{
	if (mVideoWindow)
	{
		long  fullScreenMode = OAFALSE;
		mVideoWindow->get_FullScreenMode(&fullScreenMode);
		return (fullScreenMode == OATRUE);
	}
	return false;
}

// IMediaSeeking features
bool CDXGraph::GetCurrentPosition(double * outPosition)
{
	if (mSeeking)
	{
		__int64 position = 0;
		if (SUCCEEDED(mSeeking->GetCurrentPosition(&position)))
		{
			*outPosition = ((double)position) / 10000000; //单位是100ns，因此转换为s则要除以 10000000
			return true;
		}
	}
	return false;
}

bool CDXGraph::GetStopPosition(double * outPosition)
{
	if (mSeeking)
	{
		__int64 position = 0;
		if (SUCCEEDED(mSeeking->GetStopPosition(&position)))
		{
			*outPosition = ((double)position) / 10000000.;
			return true;
		}
	}
	return false;
}

bool CDXGraph::SetCurrentPosition(double inPosition)
{
	if (mSeeking)
	{
		__int64 one = 10000000;
		__int64 position = (__int64)(one * inPosition);
		HRESULT hr = mSeeking->SetPositions(&position, AM_SEEKING_AbsolutePositioning | AM_SEEKING_SeekToKeyFrame, 
			0, AM_SEEKING_NoPositioning);
		return SUCCEEDED(hr);
	}
	return false;
}

bool CDXGraph::SetStartStopPosition(double inStart, double inStop)
{
	if (mSeeking)
	{
		__int64 one = 10000000;
		__int64 startPos = (__int64)(one * inStart);
		__int64 stopPos  = (__int64)(one * inStop);
		HRESULT hr = mSeeking->SetPositions(&startPos, AM_SEEKING_AbsolutePositioning | AM_SEEKING_SeekToKeyFrame, 
			&stopPos, AM_SEEKING_AbsolutePositioning | AM_SEEKING_SeekToKeyFrame);
		return SUCCEEDED(hr);
	}
	return false;
}

bool CDXGraph::GetDuration(double * outDuration)
{
	if (mSeeking)
	{
		__int64 length = 0;
		if (SUCCEEDED(mSeeking->GetDuration(&length)))
		{
			*outDuration = ((double)length) / 10000000.;
			return true;
		}
	}
	return false;
}

bool CDXGraph::SetPlaybackRate(double inRate)
{
	if (mSeeking)
	{

		if (SUCCEEDED(mSeeking->SetRate(inRate)))
		{
			return true;
		}
	}
	return false;
}

// Attention: range from -10000 to 0, and 0 is FULL_VOLUME.
bool CDXGraph::SetAudioVolume(long inVolume)
{
	if (mBasicAudio!=NULL)
	{
		HRESULT hr = mBasicAudio->put_Volume(inVolume);
		return SUCCEEDED(hr);
	}
	return false;
}

long CDXGraph::GetAudioVolume(void)
{
	long volume = 0;
	if (mBasicAudio)
	{
		mBasicAudio->get_Volume(&volume);
	}
	return volume;
}

// Attention: range from -10000(left) to 10000(right), and 0 is both.
bool CDXGraph::SetAudioBalance(long inBalance)
{
	if (mBasicAudio)
	{
		HRESULT hr = mBasicAudio->put_Balance(inBalance);
		return SUCCEEDED(hr);
	}
	return false;
}

long CDXGraph::GetAudioBalance(void)
{
	long balance = 0;
	if (mBasicAudio)
	{
		mBasicAudio->get_Balance(&balance);
	}
	return balance;
}

bool CDXGraph::RenderFile(const char * inFile)
{
	if (mGraph)
	{
		if (IsAccMediaTypeFile(inFile))
		{
               //需要自己构建Filter
			   WCHAR    szFilePath[MAX_PATH];
		/*
		CP_ACP：ANSI字符集；
		-1:表示转换到字符串结尾
		*/
		    MultiByteToWideChar(CP_ACP, 0, inFile, -1, szFilePath, MAX_PATH);
			 
			IBaseFilter *pSrc=NULL;
			IBaseFilter *pAAcPraser=NULL; 
			IBaseFilter *pCoreAAcDecoder=NULL; 
			IBaseFilter *pAudioRender=NULL; 

			//增加源文件

           
			HRESULT hr = AddFilterByCLSID(mGraph, CLSID_AsyncReader,szFilePath, &pSrc,TRUE);   
			if (FAILED(hr))  
			{  

				return false;

			}  
			
			
		     		
		    hr= AddFilterByCLSID(mGraph, CLSID_AACParser,L"AAC Parser", &pAAcPraser);   
			if (FAILED(hr))  
			{  


				return false;

			}  

			hr = AddFilterByCLSID(mGraph, CLSID_CoreAACAudioDecoder,L"CoreAAC Audio Decoder", &pCoreAAcDecoder);   
			if (FAILED(hr))  
			{  


				return false;

			}  

   		   hr = AddFilterByCLSID(mGraph, CLSID_AudioRender,L"Default DirectSound Device", &pAudioRender);   
			if (FAILED(hr))  
			{  


				return false;

			}  
			//连接
			
            IPin*pSrcOutPin=NULL;
			hr=GetUnconnectedPin(pSrc,PINDIR_OUTPUT,&pSrcOutPin);
			if (FAILED(hr))  
			{  


				return false;

			}  
			
			IPin*ppAAcPraserInPin=NULL;
			hr=GetUnconnectedPin(pAAcPraser,PINDIR_INPUT,&ppAAcPraserInPin);
			if (FAILED(hr))  
			{  
				return false;

			}  
            mGraph->Connect(pSrcOutPin,ppAAcPraserInPin);
			if (FAILED(hr))  
			{  
				return false;

			}  

			IPin*ppAAcPraserOutPin=NULL;
			hr=GetUnconnectedPin(pAAcPraser,PINDIR_OUTPUT,&ppAAcPraserOutPin);
			if (FAILED(hr))  
			{  


				return false;

			}  
			IPin*pCoreAAcDecoderInPin=NULL;
			hr=GetUnconnectedPin(pCoreAAcDecoder,PINDIR_INPUT,&pCoreAAcDecoderInPin);
			if (FAILED(hr))  
			{  
				return false;

			}  
			mGraph->Connect(ppAAcPraserOutPin,pCoreAAcDecoderInPin);
			if (FAILED(hr))  
			{  
				return false;

			}  
			IPin*pCoreAAcDecoderOutPin=NULL;
			hr=GetUnconnectedPin(pCoreAAcDecoder,PINDIR_OUTPUT,&pCoreAAcDecoderOutPin);
			if (FAILED(hr))  
			{  


				return false;

			}  
			IPin*pAudioRenderInPin=NULL;
			hr=GetUnconnectedPin(pAudioRender,PINDIR_INPUT,&pAudioRenderInPin);
			if (FAILED(hr))  
			{  
				return false;

			}  
			mGraph->Connect(pCoreAAcDecoderOutPin,pAudioRenderInPin);
		
			if (FAILED(hr))  
			{  
				return false;

			}  


		}
		else
		{
            WCHAR    szFilePath[MAX_PATH];
		/*
		CP_ACP：ANSI字符集；
		-1:表示转换到字符串结尾
		*/
		    MultiByteToWideChar(CP_ACP, 0, inFile, -1, szFilePath, MAX_PATH);

		    if (SUCCEEDED(mGraph->RenderFile(szFilePath, NULL)))
		    {
			    return true;
		    }
		}
		
	}
	return TRUE;
}

bool CDXGraph::IsAccMediaTypeFile(const char * inFile)
{
	 //判断是否是ACC文件格式
	CString strFilename;
	strFilename.Format("%s",inFile);

	int pos=strFilename.Find(".aac",0);

	int length=strFilename.GetLength();

	if (pos>1&&(pos==length-4))
	{
        return TRUE;
	}
	else
	{
        pos=strFilename.Find(".AAC",0);
		if (pos>1&&(pos==length-4))
		{
			return TRUE;
		}
	}

    return FALSE;


}

bool CDXGraph::SnapshotBitmap(const char * outFile)
{
	if (mBasicVideo)
	{
		long bitmapSize = 0;
		if (SUCCEEDED(mBasicVideo->GetCurrentImage(&bitmapSize, 0)))
		{
			bool pass = false;
			unsigned char * buffer = new unsigned char[bitmapSize];
			if (SUCCEEDED(mBasicVideo->GetCurrentImage(&bitmapSize, (long *)buffer)))
			{
				BITMAPFILEHEADER	hdr;
				LPBITMAPINFOHEADER	lpbi;

				lpbi = (LPBITMAPINFOHEADER)buffer;

				int nColors = 1 << lpbi->biBitCount;
				if (nColors > 256)
					nColors = 0;

				hdr.bfType		= ((WORD) ('M' << 8) | 'B');	//always is "BM"
				hdr.bfSize		= bitmapSize + sizeof( hdr );
				hdr.bfReserved1 	= 0;
				hdr.bfReserved2 	= 0;
				hdr.bfOffBits		= (DWORD) (sizeof(BITMAPFILEHEADER) + lpbi->biSize +
						nColors * sizeof(RGBQUAD));

				//CFile bitmapFile((LPCTSTR)outFile, CFile::modeReadWrite | CFile::modeCreate | CFile::typeBinary);
				CFile bitmapFile((LPCTSTR)outFile, CFile::modeReadWrite | CFile::modeCreate | CFile::typeBinary);
				bitmapFile.Write(&hdr, sizeof(BITMAPFILEHEADER));
				bitmapFile.Write(buffer, bitmapSize);
				bitmapFile.Close();
				pass = true;
			}
			delete [] buffer;
			return pass;
		}
	}
	return false;
}





//////////////////////// For GraphEdit Dubug purpose /////////////////////////////
void CDXGraph::AddToObjectTable(void)
{
	IMoniker * pMoniker = 0;
    IRunningObjectTable * objectTable = 0;
    if (SUCCEEDED(GetRunningObjectTable(0, &objectTable))) 
	{
		WCHAR wsz[256];
		swprintf(wsz, L"FilterGraph %08p pid %08x", (DWORD_PTR)mGraph, GetCurrentProcessId());
		HRESULT hr = CreateItemMoniker(L"!", wsz, &pMoniker);
		if (SUCCEEDED(hr)) 
		{
			hr = objectTable->Register(0, mGraph, pMoniker, &mObjectTableEntry);
			pMoniker->Release();
		}
		objectTable->Release();
	}
}

void CDXGraph::RemoveFromObjectTable(void)
{
	IRunningObjectTable * objectTable = 0;
    if (SUCCEEDED(GetRunningObjectTable(0, &objectTable))) 
	{
        objectTable->Revoke(mObjectTableEntry);
        objectTable->Release();
		mObjectTableEntry = 0;
    }
}
