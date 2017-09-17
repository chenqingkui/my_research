//#include "stdafx.h"
#include "HDRealStream.h"

#undef WIN32_LEAN_AND_MEAN

#include <iostream>
#include "windows.h"
#include <list> 
using namespace std;

#include "GdiPlusDisplay.h"

#include "UsedDef.h"

#include "SDLDisplay.h"

#ifdef SDL_SHOW
#include "sdl/SDL_thread.h"
#endif

#ifdef GDI_PLUS
#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment(lib, "gdiplus.lib")
#endif


#ifdef SIMPLIED_DECODE
#include "SimpliedDecode.h"
#endif

#ifdef SIMPLIED_STREAM
#include "SimpliedStreamClass.h"
#endif
#ifdef SIMPLIED_DECODE
int TestDecodedDataCallBack(const char* pVer,const void* pData, const int nDataSize,const void* pUserData);
#endif

#ifdef SIMPLIED_STREAM
int    SimpliedStream_StreamCallBack(void* pHandle,int nType,void* pBuff,int nBuffSize,void* pUserData1,void* pUserData2);
#endif

#ifdef SDL_SHOW
int TestPauseVideo(void* pParam);
int TestPlayVideo(void* pParam);
#endif

#ifdef SDL_SHOW
int SDL_worker_thread(void *pData)
{
	CHDRealStream* pStream = (CHDRealStream*)pData;
	if(pStream == NULL)
	{
		return -1 ;
	}
#ifdef SDL_USE_HWND
	RECT disHWNDRect;
	int nHWNDWidth = SHOW_WINDOW_WIDTH;
	int nHWNDHeight = SHOW_WINDOW_HEIGHT;
	if(pStream->getDiplayHWND() != NULL)
	{
		GetClientRect((HWND)(pStream->getDiplayHWND()),&disHWNDRect);
		nHWNDWidth = disHWNDRect.right - disHWNDRect.left + 1;
		nHWNDHeight = disHWNDRect.bottom - disHWNDRect.top + 1;
	}
	void* pPlayHndl = CreateSDLPlay(pStream->getDiplayHWND(),nHWNDWidth,nHWNDHeight,pStream->getStreamWidth(),pStream->getStreamHeight());
	pStream->setSDLPlayHdl(pPlayHndl);
#else
	void* pPlayHndl = CreateSDLPlay(NULL,SHOW_WINDOW_WIDTH,SHOW_WINDOW_HEIGHT,pStream->getStreamWidth(),pStream->getStreamHeight());

	RegistSDLEvent(pPlayHndl,"Pause",TestPauseVideo);
	RegistSDLEvent(pPlayHndl,"Play",TestPlayVideo);
	pStream->setSDLPlayHdl(pPlayHndl);
	void* pSDLPlay = pStream->getSDLPlayHdl();
	int nSDLPlay = SDLWorker(pSDLPlay,pStream);
	pStream->lock();
	if(pSDLPlay != NULL)
	{
		DestroySDLPlay(&pSDLPlay);
	}
	pStream->setSDLPlayHdl(NULL,false);
	pStream->unlock();
	//如果是从窗口点叉而来的退出则再调一次Stop;
	pStream->setWorkStatus(0);
	pStream->closeStream();
	pStream->closeDecode();
	pStream->setSdlThread(NULL);
#endif
	return 0;
}
#endif


#ifdef SDL_SHOW
int TestPauseVideo(void* pParam)
{
	CHDRealStream* pStream = (CHDRealStream*)pParam;
	if(pStream == NULL)
	{
		return -1 ;
	}
	pStream->setPlayStatus(0);
	return 0;
}
int TestPlayVideo(void* pParam)
{
	CHDRealStream* pStream = (CHDRealStream*)pParam;
	if(pStream == NULL)
	{
		return -1 ;
	}
	pStream->setPlayStatus(1);
	return 0;
}
#endif


 CHDRealStream::CHDRealStream()
 {
	m_nWidth = 0;
	m_nHeight = 0;
	m_pDisplayHWND = NULL;
	m_pRtspClient = NULL;
	m_nPlayStatus = 1;
	m_pSDLPlay = NULL;
	m_pDecoder = NULL;
	m_nWorkStatus = 0;
	m_pSDLThread = NULL;
	m_hLock = NULL;
 } 
 CHDRealStream::~CHDRealStream()
 {
 } 
 int CHDRealStream::init()
 {
	#ifdef GDI_PLUS
		GdiplusStartup(&m_GdiplusToken,&m_GdiplusStartupInput,NULL);
	#endif

	createLock();
	return 0;
 }
 int CHDRealStream::unInit()
 {
	#ifdef GDI_PLUS
		//程序结束前关闭GDI+
		GdiplusShutdown(m_GdiplusToken);
	#endif
	destroyLock();
	return 0;
 }
 int CHDRealStream::start(const char* pszURL)
 {
    //如何已经打开了再次打开则先关闭之前的
	stop();
	int nStatus = 0;
	if(pszURL == NULL)
	{
		return -1;
	}
	#ifdef SIMPLIED_STREAM
		CSimpliedStream* pSimpliedRtsp =  new CSimpliedStream();
		if(pSimpliedRtsp != NULL)
		{
			pSimpliedRtsp->init();
			pSimpliedRtsp->openStream(pszURL,SimpliedStream_StreamCallBack,this,NULL);
		}
		void* hClient = (void*)pSimpliedRtsp;
	#endif
	lock();
	m_pRtspClient = hClient;
	if(hClient != NULL)
	{

		#ifdef		 SIMPLIED_DECODE
			m_pDecoder	=  CreateSimpliedDecode(DECODE_FORMAT_H264,DECODE_FORMAT_YUV_420P);
			if(m_pDecoder != NULL)
			{
				AddDecodeFunc(m_pDecoder,TestDecodedDataCallBack,(void*)this);
				StartSimpliedDecode(m_pDecoder);
			}
		#endif
	}
	m_nWorkStatus = 1;
	unlock();
	return 0;
 }
 int CHDRealStream::stop()
 {
	lock();
	m_nWorkStatus = 0;
	unlock();
	closeStream();
	closeDecode();
	
	#ifdef SDL_SHOW
		#ifdef SDL_USE_HWND
			destroySDLPlaySrv();
		#else
			if(m_pSDLPlay != NULL)
			{
				SDLPlaySignalQuitEvent(m_pSDLThread);
			}
		#endif
	#endif
	return 0;
 }
int CHDRealStream::closeDecode(bool bLock)
{
	bool bLockFlag = bLock;
	void* pDecode = NULL; 
	if(bLockFlag)
	{
		lock();
	}
	if(m_pDecoder != NULL)
	{
		pDecode = m_pDecoder;
		m_pDecoder = NULL;
		m_nWidth = 0;
		m_nHeight = 0;
	}
	if(bLockFlag)
	{
		unlock();
	}
	#ifdef		 SIMPLIED_DECODE
		if(pDecode != NULL)
		{
			StopSimpliedDecode(pDecode);
			DestorySimpliedDecode(pDecode);
			pDecode = NULL;
		}
	#endif
	
	
	return 0;
}
int CHDRealStream::closeStream(bool bLock)
{
	
	bool bLockFlag = bLock;
	void* hRtspClient = NULL;
	if(bLockFlag)
	{
		lock();
	}
	if(m_pRtspClient != NULL)
	{
		hRtspClient = m_pRtspClient;
		m_pRtspClient = NULL;
	}
	if(bLockFlag)
	{
		unlock();
	}
	if(hRtspClient != NULL)
	{
#ifdef SIMPLIED_STREAM
		CSimpliedStream* pSimpleRtsp = (CSimpliedStream*)hRtspClient;
		delete pSimpleRtsp;
		pSimpleRtsp = NULL;
#endif
	}
	return 0;
}
 int  CHDRealStream::playVideo()
 {
	return 0;
 } 
 int CHDRealStream::pauseVideo()
 {
	return 0;
 } 
 
 void*  CHDRealStream::getRtspClientHdl(bool bLock)
 {
	void* pHdl = NULL;
	if(bLock)
	{
		lock();
		pHdl = m_pRtspClient;
		unlock();
	}
	else
	{
		pHdl = m_pRtspClient;
	}
	return pHdl;
 }
void*  CHDRealStream::getSDLPlayHdl(bool bLock )
{
	void* pHdl = NULL;
	if(bLock)
	{
		lock();
		pHdl = m_pSDLPlay;
		unlock();
	}
	else
	{
		pHdl = m_pSDLPlay;
	}
	return pHdl;
}
void*  CHDRealStream::getDecoderHdl(bool bLock)
{
	void* pHdl =  NULL;
	if(bLock)
	{
		lock();
		pHdl = m_pDecoder;
		unlock();
	}
	else
	{
		pHdl = m_pDecoder;
	}
	return pHdl;
}
void* CHDRealStream::getDiplayHWND(bool bLock )
{
	void* pHdl = NULL;
	if(bLock)
	{
		lock();
		pHdl =  m_pDisplayHWND;
		unlock();
	}
	else
	{
		pHdl =  m_pDisplayHWND;
	}
	return pHdl;
}
void* CHDRealStream::setDisplayHWND(void* pWnd,bool bLock)
{
	if(bLock)
	{
		lock();
		m_pDisplayHWND = (HWND)pWnd;
		unlock();
	}
	else
	{
		lock();
		m_pDisplayHWND = (HWND)pWnd;
		unlock();
	}
	return 0;
}
int   CHDRealStream::setSDLPlayHdl(void* pSDLHdl,bool bLock)
{
	if(bLock)
	{
		lock();
		m_pSDLPlay = pSDLHdl;
		unlock();
	}
	else
	{
		m_pSDLPlay = pSDLHdl;
	}
	return 0;
}
int  CHDRealStream::setPlayStatus(int nValue,bool bLock)
{
	if(bLock)
	{
		lock();
		m_nPlayStatus = nValue;
		unlock();
	}
	else
	{
		m_nPlayStatus = nValue;
	}
	return 0;
}
int  CHDRealStream::getPlayStatus(bool bLock)
{
	int nStatus = 0;
	if(bLock)
	{
		lock();
		nStatus = m_nPlayStatus;
		unlock();	
	}
	else
	{
		nStatus = m_nPlayStatus;
	}
	return nStatus;
}

int CHDRealStream::setStreamWidth(int nValue,bool bLock)
{
	if(bLock)
	{
		lock();
		m_nWidth  = nValue;
		unlock();
	}
	else
	{
		m_nWidth  = nValue;
	}
	return 0;
}             
int CHDRealStream::setStreamHeight(int nValue,bool bLock)
{
	if(bLock)
	{
		lock();
		m_nHeight = nValue;
		unlock();
	}
	else
	{
		m_nHeight = nValue;
	}
	return 0;
}
int CHDRealStream::getStreamWidth(bool bLock)
{
	int nValue = 0;
	if(bLock)
	{
		lock();
		nValue = m_nWidth;
		unlock();
	}
	else
	{
		nValue = m_nWidth;
	}
	return nValue;
}
int CHDRealStream::getStreamHeight(bool bLock)
{
	int nValue = 0;
	if(bLock)
	{
		lock();
		nValue = m_nHeight;
		unlock();
	}
	else
	{
		nValue = m_nHeight;
	}
	return nValue;
}
int CHDRealStream::getWorkStatus(bool bLock)
{
	int nValue = 0;
	if(bLock)
	{
		lock();
		nValue = m_nWorkStatus;
		unlock();
	}
	else
	{
		 nValue = m_nWorkStatus;
	}
	return nValue;
}
int CHDRealStream::setWorkStatus(int nValue,bool bLock)
{
	if(bLock)
	{
		lock();
		m_nWorkStatus = nValue;
		unlock();
	}
	else
	{
		m_nWorkStatus = nValue;
	}
	return 0;
}

int CHDRealStream::setSdlThread(void* pThread,bool bLock)
{
	if(bLock)
	{
		lock();
		m_pSDLThread = pThread;
		unlock();
	}
	else
	{
		m_pSDLThread = pThread;
	}
	return 0;
}
void* CHDRealStream::getSdlThread(bool bLock)
{
	void* pValue =  NULL;
	if(bLock)
	{
		lock();
		pValue = m_pSDLThread;
		unlock();
	}
	else
	{
		pValue = m_pSDLThread;
	}
	return pValue;
}
int  CHDRealStream::destroySDLPlaySrv()
{
	lock();
	void* pSDLPlay = getSDLPlayHdl();
	if(pSDLPlay != NULL)
	{
		DestroySDLPlay(&pSDLPlay);
	}
	setSDLPlayHdl(NULL,false);
	unlock();
	return 0;
}
int CHDRealStream::createLock()
{
	#ifdef WIN32
		m_hLock = (void*)malloc(sizeof(CRITICAL_SECTION));
		InitializeCriticalSection((LPCRITICAL_SECTION)m_hLock);
	#endif
	return 0;
}
int CHDRealStream::destroyLock()
{
	#ifdef WIN32
		if(m_hLock != NULL)
		{
			DeleteCriticalSection((LPCRITICAL_SECTION)m_hLock);
		}
	#endif
	return 0;
}
int CHDRealStream::lock()
{
	#ifdef WIN32
		if(m_hLock != NULL)
		{
			EnterCriticalSection((LPCRITICAL_SECTION)m_hLock);
		}
	#endif
	return 0;
}

int CHDRealStream::unlock()
{
	#ifdef WIN32
		if(m_hLock != NULL)
		{
			LeaveCriticalSection((LPCRITICAL_SECTION)m_hLock);
		}
	#endif
	return 0;
}


int	CHDRealStream::setStreamFuncMeta(PStreamFunc pFunc,void* pUser1,void* pUser2)
{
	m_streamMeta.pUser1 = pUser1;
	m_streamMeta.pUser2 = pUser2;
	m_streamMeta.pFunc = pFunc;
	return 0;
}


StreamFuncMeta	CHDRealStream::getStreamFuncMeta()
{
	StreamFuncMeta meta;
	meta = m_streamMeta;
	return meta;
}

#ifdef SIMPLIED_DECODE
int TestDecodedDataCallBack(const char* pVer,const void* pData, const int nDataSize,const void* pUserData)
{
	if(pVer == NULL  || pData == NULL)
	{
		return -1;
	}
	CHDRealStream* pStream = (CHDRealStream*)pUserData;
	if(pStream == NULL)
	{
		return  -1;
	}
	if(strcmp(pVer,"V0.1") == 0)
	{
		int nIndex = 0;
		int nCounts = nDataSize / sizeof(SimpliedDecodedDataVer1);
		for(nIndex = 0;nIndex < nCounts;nIndex++)
		{
			SimpliedDecodedDataVer1* pDataVer1 = (SimpliedDecodedDataVer1*)pData;
			printf("%d %d\n",pDataVer1->decodedFrame.width,pDataVer1->decodedFrame.height,pDataVer1->decodedFrame.pChl[0]);
			#ifdef	SHOW_WINDOW
				#ifdef GDI_PLUS
					GDIPlusRenderYUV420P(pStream->getDiplayHWND(false),pDataVer1->decodedFrame.pChl[0],
									(pDataVer1->decodedFrame.chlLen[0] + pDataVer1->decodedFrame.chlLen[1]+pDataVer1->decodedFrame.chlLen[2]),
									pDataVer1->decodedFrame.width,pDataVer1->decodedFrame.height);
				#else
				
					GDIRenderYUV420P(pStream->getSDLPlayHdl(false),pDataVer1->decodedFrame.pChl[0],
							(pDataVer1->decodedFrame.chlLen[0] + pDataVer1->decodedFrame.chlLen[1]+pDataVer1->decodedFrame.chlLen[2]),
							pDataVer1->decodedFrame.width,pDataVer1->decodedFrame.height);
				
				#endif
			#endif
			#ifdef SDL_SHOW
				
				int nWidth = 1920;
				int nHeight = 1080;
				nWidth = pStream->getStreamWidth(false);
				nHeight = pStream->getStreamHeight(false);
				pStream->lock();
				bool bWidthHeightOk = false;
				if(nWidth == 0)
				{	
					bWidthHeightOk = true;
				}
				if(nHeight == 0)
				{
					bWidthHeightOk = true;
				}
				if(bWidthHeightOk && pStream != NULL && pStream->getSDLPlayHdl(false) == NULL && pStream->getWorkStatus(false) == 1)
				{
					pStream->setStreamWidth(pDataVer1->decodedFrame.width,false);
					pStream->setStreamHeight(pDataVer1->decodedFrame.height,false);

					#ifdef SDL_SHOW
						SDL_Thread * sdl_work_tid = SDL_CreateThread(SDL_worker_thread,NULL,(void*)pStream);
						pStream->setSdlThread((void*)sdl_work_tid,false);
					#endif
				}
				pStream->unlock();

				int width = pDataVer1->decodedFrame.width;
				int height = pDataVer1->decodedFrame.height;

				void* pYUV420P[3] = {0,0,0};
				int nYUV420PLineSize[3] = {0,0,0};
				pYUV420P[0] = pDataVer1->decodedFrame.pChl[0];
				pYUV420P[1] = pDataVer1->decodedFrame.pChl[1];
				pYUV420P[2] = pDataVer1->decodedFrame.pChl[2];
				nYUV420PLineSize[0] = pDataVer1->decodedFrame.chlLineSize[0];
				nYUV420PLineSize[1] = pDataVer1->decodedFrame.chlLineSize[1];
				nYUV420PLineSize[2] = pDataVer1->decodedFrame.chlLineSize[2];
				SDLDisplayYUV420P(pStream->getSDLPlayHdl(false),pYUV420P,nYUV420PLineSize,width,height);
			#endif
		}
	}
	return 0;
}
#endif

#ifdef SIMPLIED_STREAM
int    SimpliedStream_StreamCallBack(void* pHandle,int nType,void* pBuff,int nBuffSize,void* pUserData1,void* pUserData2)
{
	
	CHDRealStream* pStream = (CHDRealStream*)pUserData1;
	if(pStream == NULL)
	{
		return  -1;
	}
	pStream->lock();
	int nPlayStatus =  pStream->getPlayStatus(false);
	void* hDecode = pStream->getDecoderHdl(false);
	if(nPlayStatus == 0)
	{
		pStream->unlock();
		return -1;
	}
    hDecode = pStream->getDecoderHdl();
	printf("recv stream type:%d pBuffer:%p size:%d\n",nType,pBuff,nBuffSize);
	//return ;
	if(hDecode != NULL)
	{
			#ifdef		 SIMPLIED_DECODE
				if(hDecode != NULL)
				{
					//DecodeStream(g_hSimpliedDecode,pBuffer,dwBufSize,NULL,NULL,NULL);
					InputStream(hDecode,pBuff,nBuffSize);
				}
			#endif
		
	}
	StreamFuncMeta meta = pStream->getStreamFuncMeta();
	printf("stream meta func :%p %p %p \n",meta.pFunc,meta.pUser1,meta.pUser2);
	if(meta.pFunc != NULL)
	{
		meta.pFunc(NULL,0,pBuff,nBuffSize,meta.pUser1,meta.pUser2);
	}
	pStream->unlock();
	return 0;
}
#endif

