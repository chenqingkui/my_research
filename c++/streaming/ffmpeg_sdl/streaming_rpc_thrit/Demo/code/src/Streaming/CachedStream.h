#ifndef _CACHED_STREAM_H_
#define _CACHED_STREAM_H_
//////////////////////////////////////////////////////////////////////////////////////////////////
#include <list>
#include <map>
using namespace std;
//////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct CachedStreamElem
{
	void*	stream;
	int		streamLen;
}CachedStreamElem;

typedef struct CachedStreamNotifyFuncInf
{
	int (*pFunc)(void* pStream,int nStreamLen,void* pUserData);
	void* pUserData;
}CachedStreamNotifyFuncInf;

typedef enum CACHED_STREAM_NOTIFY_STATUS
{
	CACHED_STREAM_NOTIFY_STATUS_ENABLE     		= 1,
	CACHED_STREAM_NOTIFY_STATUS_DISABLE	 		= 2,
}CACHED_STREAM_NOTIFY_STATUS;
//////////////////////////////////////////////////////////////////////////////////////////////////
class CCachedStream
{
	private:
		list<CachedStreamElem>				    m_listStreamElem;
		map<int,CachedStreamNotifyFuncInf>      m_mapFuncInf;
		int										funcInfCnt;
		CACHED_STREAM_NOTIFY_STATUS				notifyStatus;
	private:
		void*									m_hEvent;
		void*									m_hLock;
		void*									m_hThread;
		bool									m_bThreadQuit;
	public:
		CCachedStream();
		~CCachedStream();
	private:	
		int createEvent();
		int destroyEvent();
		int createLock();
		int destroyLock();
		int lock();
		int unlock();

		int createThread(unsigned int (__stdcall *pThreadFunc)(void* pParam));
		
		int destroyThread();
		int destroyStreamList();
	public:
		int waitEvent();
		int signalEvent();
		int exitThread();
	public:
		unsigned int getStreamListSize();
		int popListStreamElemFront(CachedStreamElem* pElem);
		int inputStream(void* pInputData,int nInputDataLen);
		int start();
		int stop();
		int addNotifyFuncInf(int (*pNotify)(void* pParam,int nParamLen,void* pUserData),void* pUserData);
		int delNotifyFuncInf(int funcFlag);
		int setNotifyStatus(CACHED_STREAM_NOTIFY_STATUS status);
		CACHED_STREAM_NOTIFY_STATUS getNotifyStatus();
		int noitfyStreamFunc(CachedStreamElem * pStreamElem);
		bool isThreadQuit();
};
#endif
