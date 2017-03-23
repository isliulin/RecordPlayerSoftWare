//
// CDXGraph.h
//

#ifndef __H_CDXGraph__
#define __H_CDXGraph__

#include <..//BaseClass//streams.h>
// Filter graph notification to the specified window
#define WM_GRAPHNOTIFY  (WM_USER+20)
#include <initguid.h>
//3C78B8E2-6C4D-11D1-AEE2-0000F7754B98
DEFINE_GUID(CLSID_AACParser,0x3C78B8E2, 0x6C4D, 0x11D1, 0xAE, 0xE2, 0x00, 0x00, 0xF7, 0x75, 0x4B, 0x98);
//6AC7C19E-8CA0-4E3D-9A9F-2881DE29E0AC
DEFINE_GUID(CLSID_CoreAACAudioDecoder,0x6AC7C19E,0x8CA0,0x4E3D,0x9A,0x9F,0x28,0x81,0xDE,0x29,0xE0,0xAC);


class CDXGraph
{
private:
	IGraphBuilder *     mGraph;  //This interface provides methods that enable an application to build a filter graph. The Filter Graph Manager implements this interface.
	IMediaControl *		mMediaControl;//The IMediaControl interface provides methods for controlling the flow of data through the filter graph. 
	IMediaEventEx *		mEvent;//The IMediaEventEx interface inherits the IMediaEvent interface, which contains methods for retrieving event notifications and for overriding the filter graph's default handling of events.
	IBasicVideo *		mBasicVideo; //����getcurrentImage����ֻ������ϰ汾��DirectShow���ٵĻ�����Ч
	/*
	The IBasicVideo interface enables applications to set video properties 
	such as the destination and source rectangles. 
	This interface is implemented on the Video Renderer filter,
	but is exposed to applications through the Filter Graph Manager. 
	Applications should always retrieve this interface from the Filter Graph Manager.  
	*/
	IBasicAudio *		mBasicAudio;
	/*
	The IBasicAudio interface enables applications to control 
	the volume and balance of the audio stream.
	*/
	IVideoWindow  *		mVideoWindow;
	/*

	The IVideoWindow object manages the window of a video renderer. 
	To use this object, declare a variable of type IVideoWindow and
	set it equal to the FilgraphManager object:
	*/
	IMediaSeeking *		mSeeking;
	/*
	The IMediaSeeking interface contains methods for seeking to a position within a stream, 
	and for setting the playback rate. The Filter Graph Manager exposes this interface,
	and so do individual filters or pins.
	Applications should query the Filter Graph Manager for the interface. 
	*/



	DWORD				mObjectTableEntry; 

public:
	CDXGraph();
	virtual ~CDXGraph();

public:
	virtual bool Create(void);  //���ڴ���һ��IGraphBuilder���ڹ���������ѯ�����ӿ��Ƿ���
	virtual void Release(void);
	virtual bool Attach(IGraphBuilder * inGraphBuilder);

	IGraphBuilder * GetGraph(void); // Not outstanding reference count
	IMediaEventEx * GetEventHandle(void);

	bool ConnectFilters(IPin * inOutputPin, IPin * inInputPin, const AM_MEDIA_TYPE * inMediaType = 0);
	void DisconnectFilters(IPin * inOutputPin);

	bool SetDisplayWindow(HWND inWindow);
	bool SetNotifyWindow(HWND inWindow);
	bool ResizeVideoWindow(long inLeft, long inTop, long inWidth, long inHeight);
	void HandleEvent(WPARAM inWParam, LPARAM inLParam);

	bool Run(void);        // Control filter graph
	bool Stop(void);
	bool Pause(void);
	bool IsRunning(void);  // Filter graph status
	bool IsStopped(void);
	bool IsPaused(void);

	bool SetFullScreen(BOOL inEnabled);
	bool GetFullScreen(void);

	// IMediaSeeking
	bool GetCurrentPosition(double * outPosition);//��ȡ��ǰ����λ��
	bool GetStopPosition(double * outPosition); //��ȡֹͣλ��
	bool SetCurrentPosition(double inPosition); //���õ�ǰλ��
	bool SetStartStopPosition(double inStart, double inStop);//���ÿ�ʼ��ֹͣλ��
	bool GetDuration(double * outDuration); //��ȡ��������ʱ�䳤��Retrieves the duration of the stream.
	bool SetPlaybackRate(double inRate); //���ò�������

	// Attention: range from -10000 to 0, and 0 is FULL_VOLUME.
	bool SetAudioVolume(long inVolume);
	long GetAudioVolume(void);
	// Attention: range from -10000(left) to 10000(right), and 0 is both.
	bool SetAudioBalance(long inBalance);
	long GetAudioBalance(void);

	bool RenderFile(const char * inFile);  //��Ⱦ��ʾ�ļ�
	bool SnapshotBitmap(const char * outFile); //ץͼ

	//ACC���빹��Filter Graph
	bool IsAccMediaTypeFile(const char * inFile);
	HRESULT AddFilterByCLSID(IGraphBuilder *pGraph, // Pointer to the Filter Graph Manager.  
		                     const GUID& clsid, // CLSID of the filter to create.  
		                     LPCWSTR wszName, // A name for the filter.  
		                     IBaseFilter **ppF,bool bSourceFlag=false); // Receives a pointer to the filter. 

   

private:
	void AddToObjectTable(void) ;
	void RemoveFromObjectTable(void);
	HRESULT GetUnconnectedPin(IBaseFilter* pFilter,PIN_DIRECTION PinDir,IPin**ppPin);//��ȡFilter�ϵ�δ����Pin
	
	bool QueryInterfaces(void);
};

#endif // __H_CDXGraph__