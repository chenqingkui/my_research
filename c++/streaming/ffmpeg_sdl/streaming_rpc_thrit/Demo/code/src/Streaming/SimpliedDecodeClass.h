#ifndef  _SIMPLIED_DECODE_CLASS_H_
#define  _SIMPLIED_DECODE_CLASS_H_
#include  "SimpliedDecodePriDef.h"
#include <map>
#include <list>
using namespace std;

typedef struct SimpliedStreamElem
{
	void*		pData;
	int         nDataLen;
}SimpliedStreamElem;

class CSimpliedDecode
{
	private:
		DECODE_FORMAT 							streamFormat;
		DECODED_PIXEL_FORMAT					pixelFromat;
		map<int,SimpliedDecodeFuncInf>  		mapDecFuncInf;
		int										decFuncInfCnt;
		void*									m_pCodec;
		void*									m_pCodecCtx;
		void*									m_pCodecParserCtx;
		int										m_codec_id;
	private:
		void*									m_hEvent;
		void*									m_hLock;
		void*									m_hThread;
		bool									m_bThreadQuit;
	private:
		list<SimpliedStreamElem>				m_listStreamElem;
	public:
		CSimpliedDecode(DECODE_FORMAT streamFormat,DECODED_PIXEL_FORMAT pixelFromat);
		~CSimpliedDecode();
	public:
		int addDecodeFuncInf(PDecodedDataCallBack pFunc,void* pUserData);
		int delDecodeFuncInf(int funcFlag);
		int	decodeStream(void* pInputData,int nInputDataLen,const char* pVer,void* pDecOutData,int* pDecOutDataLen);
		int inputStream(void* pInputData,int nInputDataLen);
		int start();
		int stop();


		int createEvent();
		int waitEvent();
		int signalEvent();
		int destroyEvent();
		
		int createLock();
		int destroyLock();
		int lock();
		int unlock();

		int createThread(unsigned int (__stdcall *pThreadFunc)(void* pParam));
		int exitThread();
		int destroyThread();
		bool isThreadQuit();

		unsigned int getStreamListSize();
		int popListStreamElemFront(SimpliedStreamElem* pElem);

	private:
		int decodedFrameNotify(void* pCtx,void* pFrm);
		int destroyStreamList();
};

#endif