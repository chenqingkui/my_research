#include "CachedStream.h"
#ifdef WIN32
#include <windows.h>
#include <process.h>  
#endif
/////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned int  __stdcall CachedStreamNotifyThreadFunc(void* pParam);
/////////////////////////////////////////////////////////////////////////////////////////////////////////
CCachedStream::CCachedStream()
{
	m_hEvent = NULL;
	m_hLock = NULL;
	m_hThread = NULL;
	m_bThreadQuit = NULL;
	funcInfCnt = 0;
	notifyStatus = CACHED_STREAM_NOTIFY_STATUS_ENABLE;
}
CCachedStream::~CCachedStream()
{
	
}
int CCachedStream::start()
{
	createEvent();
	createLock();
	createThread(CachedStreamNotifyThreadFunc);
	return 0;
}
int CCachedStream::stop()
{
	destroyThread();
	destroyStreamList();
	destroyEvent();
	destroyLock();
	return 0;
}
int CCachedStream::inputStream(void* pInputData,int nInputDataLen)
{
	if(pInputData == NULL || nInputDataLen <= 0)
	{
		return -1;
	}
	void* pStoreData = malloc(nInputDataLen);
	if(pStoreData == NULL)
	{
		return -1;
	}
	//拷贝
	memcpy(pStoreData,pInputData,nInputDataLen);

	//加锁
	lock();

	CachedStreamElem ssElem;
	ssElem.stream = pStoreData;
	ssElem.streamLen = nInputDataLen;
	//入队列
	m_listStreamElem.push_back(ssElem);
	//over 100 remove first
	if(m_listStreamElem.size() > 1000)
	{
		CachedStreamElem ssPopElem;
		ssPopElem = m_listStreamElem.front();
		m_listStreamElem.pop_front();
		if(ssPopElem.stream != NULL)
		{
			free(ssPopElem.stream);
			ssPopElem.stream = NULL;
		}
	}
	//解锁
	unlock();

	//通知
	signalEvent();

	return 0;
}
int CCachedStream::addNotifyFuncInf(int (*pNotify)(void* pParam,int nParamLen,void* pUserData),void* pUserData)
{
	CachedStreamNotifyFuncInf funcInf;
	funcInf.pFunc = pNotify;
	funcInf.pUserData = pUserData;
	lock();
	int funcFlag = funcInfCnt++;
	unlock();
	m_mapFuncInf.insert(make_pair(funcFlag,funcInf));
	return funcFlag;
}
int CCachedStream::delNotifyFuncInf(int funcFlag)
{
	map<int,CachedStreamNotifyFuncInf>::iterator mapIter ;
	mapIter = m_mapFuncInf.find(funcFlag);
	if(mapIter != m_mapFuncInf.end())
	{
		m_mapFuncInf.erase(funcFlag);
		return 0;
	}
	return -1;
}
int CCachedStream::setNotifyStatus(CACHED_STREAM_NOTIFY_STATUS status)
{
	lock();
	notifyStatus = status; 
	unlock();
	signalEvent();
	return 0;
}
CACHED_STREAM_NOTIFY_STATUS CCachedStream::getNotifyStatus()
{
	CACHED_STREAM_NOTIFY_STATUS status;
	lock();
	status = notifyStatus;
	unlock();
	return status;
}
int CCachedStream::createEvent()
{
	#ifdef WIN32
		m_hEvent = (void*)CreateEvent(NULL,0,0,"CCachedStream");
	#endif
	return 0;
}
int CCachedStream::waitEvent()
{
	#ifdef WIN32
		if(m_hEvent != NULL)
		{
			WaitForSingleObject(m_hEvent,INFINITE);
		}
	#endif
	return 0;
}
int CCachedStream::signalEvent()
{
	#ifdef WIN32
		if(m_hEvent != NULL)
		{
			SetEvent(m_hEvent);
		}
	#endif
	return 0;
}
int CCachedStream::destroyEvent()
{
	#ifdef WIN32
		if(m_hEvent != NULL)
		{
			CloseHandle(m_hEvent);
			m_hEvent = NULL;
		}
	#endif
	return 0;
}
int CCachedStream::createLock()
{
	#ifdef WIN32
		m_hLock = (void*)malloc(sizeof(CRITICAL_SECTION));
		InitializeCriticalSection((LPCRITICAL_SECTION)m_hLock);
	#endif
	return 0;
}
int CCachedStream::destroyLock()
{
	#ifdef WIN32
		if(m_hLock != NULL)
		{
			DeleteCriticalSection((LPCRITICAL_SECTION)m_hLock);
		}
	#endif
	return 0;
}
int CCachedStream::lock()
{
	#ifdef WIN32
		if(m_hLock != NULL)
		{
			EnterCriticalSection((LPCRITICAL_SECTION)m_hLock);
		}
	#endif
	return 0;
}
int CCachedStream::unlock()
{
	#ifdef WIN32
		if(m_hLock != NULL)
		{
			LeaveCriticalSection((LPCRITICAL_SECTION)m_hLock);
		}
	#endif
	return 0;
}


int CCachedStream::createThread(unsigned int (__stdcall *pThreadFunc)(void* pParam))
{
	#ifdef WIN32
		m_hThread = (void*)_beginthreadex(NULL,0,pThreadFunc,this,0,NULL);
	#endif
	return 0;
}
int CCachedStream::destroyThread()
{
	#ifdef WIN32
		if(m_hThread != NULL)
		{
			m_bThreadQuit = true;
		}
	#endif
	return 0;
}
int  CCachedStream::exitThread()
{
	#ifdef WIN32
		if(m_hThread != NULL)
		{
			_endthreadex(0);
			m_hThread = NULL;
		}
	#endif
	return 0;
}
unsigned int CCachedStream::getStreamListSize()
{
	unsigned int uListSize = 0;
	lock();
	uListSize = m_listStreamElem.size();
	unlock();
	return uListSize;
}
int CCachedStream::popListStreamElemFront(CachedStreamElem* pElem)
{
	if(pElem == NULL)
	{
		return -1;
	}
	lock();
	*pElem = m_listStreamElem.front();
	m_listStreamElem.pop_front();
	unlock();
	return 0;
}
int CCachedStream::destroyStreamList()
{
	lock();
	list<CachedStreamElem>::iterator listIter = m_listStreamElem.begin();
	for(;listIter != m_listStreamElem.end();listIter++)
	{
		if(listIter->stream != NULL)
		{
			free(listIter->stream);
			listIter->stream  = NULL;
			listIter->streamLen = 0;
		}
	}
	m_listStreamElem.clear();
	unlock();
	return 0;
}

int CCachedStream::noitfyStreamFunc(CachedStreamElem * pStreamElem)
{
	if(pStreamElem == NULL)
	{
		return -1;
	}
	lock();
	map<int,CachedStreamNotifyFuncInf>::iterator mapIter = m_mapFuncInf.begin() ;	
	for(;mapIter != m_mapFuncInf.end();mapIter++)
	{
		if(mapIter->second.pFunc != NULL)
		{
			mapIter->second.pFunc(pStreamElem->stream,pStreamElem->streamLen,mapIter->second.pUserData);
		}
	}
	unlock();
	return 0;
}
bool CCachedStream::isThreadQuit()
{
	bool bQuit = false;	
	lock();
	bQuit = m_bThreadQuit;
	unlock();
	return bQuit;
}
unsigned int __stdcall  CachedStreamNotifyThreadFunc(void* pParam)
{
	CCachedStream* pCCS = (CCachedStream*)pParam;
	while(pCCS != NULL && !pCCS->isThreadQuit())
	{
		unsigned  listSize = 0;
		CachedStreamElem ssElem;
		memset(&ssElem,0,sizeof(ssElem));
		int popStreamOk = 0;
		int status = 0;
		pCCS->waitEvent();
		if(pCCS->getNotifyStatus() == CACHED_STREAM_NOTIFY_STATUS_DISABLE)
		{
			continue;
		}
		do
		{
			listSize = pCCS->getStreamListSize();
			if(listSize == 0)
			{
				break;
			}
			status = pCCS->popListStreamElemFront(&ssElem);
			if(status == 0)
			{
				popStreamOk = 1;
			}
			if(popStreamOk)
			{
				
				if(ssElem.stream != NULL)
				{
					pCCS->noitfyStreamFunc(&ssElem);
					free(ssElem.stream);
					ssElem.stream = NULL;
					ssElem.streamLen = 0;
				}
			}
		}while(listSize > 0);
	}while(1);
	pCCS->exitThread();
}