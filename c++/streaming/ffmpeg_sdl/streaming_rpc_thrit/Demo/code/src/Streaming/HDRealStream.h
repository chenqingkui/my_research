#ifndef _HD_REAL_STREAM_H_
#define _HD_REAL_STREAM_H_
#include "UsedDef.h"
#ifdef GDI_PLUS
#include <gdiplus.h>
using namespace Gdiplus;
#endif
typedef int (*PStreamFunc)(void*,int,void*,int,void*,void*);

struct StreamFuncMeta
{
	PStreamFunc pFunc;
	void*		pUser1;
	void*		pUser2;
	StreamFuncMeta()
	{
		pFunc = 0;
		pUser1 = 0;
		pUser2 = 0;
	}
};
class CHDRealStream
{
	private:
		int m_nWidth;
		int m_nHeight;
		void* m_pDisplayHWND;
		void* m_pRtspClient;
		int	  m_nPlayStatus;
		void* m_pSDLPlay;
		void* m_pDecoder;
		int   m_nWorkStatus;
		void* m_pSDLThread;
		void* m_hLock;
		#ifdef GDI_PLUS
			ULONG_PTR	m_GdiplusToken;
			Gdiplus::GdiplusStartupInput m_GdiplusStartupInput;
		#endif

		StreamFuncMeta m_streamMeta;
	private:
		int destroySDLPlaySrv();
	public:
		CHDRealStream();
		~CHDRealStream();
	public:
		int closeDecode(bool bLock = true);
		int closeStream(bool bLock = true);
	private:
		int createLock();
		int destroyLock();
	public:
		int lock();
		int unlock();
	public:
	    int showWindow();
		int start(const char* pszURL);
		int stop();
		int init();
		int unInit();
		int playVideo();
		int pauseVideo();
		void* getDiplayHWND(bool bLock = true);
		void* setDisplayHWND(void* pWnd,bool bLock = true);
		void* getRtspClientHdl(bool bLock = true);
		void* getSDLPlayHdl(bool bLock = true);
		int   setSDLPlayHdl(void* pSDLHdl,bool bLock = true);
		void* getDecoderHdl(bool bLock = true);
		int  getPlayStatus(bool bLock = true);
		int  setPlayStatus(int nValue,bool bLock = true);
		int setStreamWidth(int nValue,bool bLock = true);
		int setStreamHeight(int nValue,bool bLock = true);
		int getStreamWidth(bool bLock = true);
		int getStreamHeight(bool bLock = true);
		int getWorkStatus(bool bLock = true);
		int setWorkStatus(int nValue,bool bLock = true);
		int setSdlThread(void* pThread,bool bLock = true);
		void* getSdlThread(bool bLock = true);
		int	setStreamFuncMeta(PStreamFunc pFunc,void* pUser1,void* pUser2);
		StreamFuncMeta getStreamFuncMeta();
};
#endif