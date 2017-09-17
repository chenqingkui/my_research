#include "StreamExport.h"
#include "HDRealStream.h"
#include <stdio.h>
#include <list>
#include "windows.h"
//#include "Process.h"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef  	int 		ScErr;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "WebSocketClient.hpp"
#include <iostream>
#include <string>
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
using namespace WEB::CLIENT;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct StreamMeta
{
	void* pBuf ;
	int	  nBuf;
	int	  nType;
	StreamMeta()
	{
		pBuf = NULL;
		nBuf = 0;
		nType = 0;
	}
};
struct StreamConnectMeta
{
	void* pCli ;
	void* pID;
	StreamConnectMeta()
	{
		pCli = NULL;
		pID = NULL;
	}
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int recv_stream_func(void*,int,void*,int,void*,void*);
unsigned long __stdcall proc_stream_func(void* pData);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string msg_id = "";
int	    closed = 0;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static int on_ops_wss_msg_call(void* id,const char* type,const char* value_type,const void* p_value,int n_value_size,
	void* p_user_1,void* p_user_2){
		if(type != NULL && value_type != NULL && p_value != NULL){
			std::string call_type = type;
			if(call_type != "message"){
				return -1;
			}
			std::string val_type = value_type;
			if(val_type == "text")
			{
				std::string val_msg;
				for(int index = 0;index < n_value_size;index++){
					val_msg.push_back(((char*)p_value)[index]);
				}
				if(msg_id == ""){
					//ms_ws_client::get_con_id_from_json_msg(val_msg,msg_id);	
				}

				int nGBKSize = val_msg.length()*2;
				char* pGBK = new char[nGBKSize];
				if(pGBK != NULL)
				{
					memset(pGBK,0,nGBKSize);
					int nNeedSize = 0;
					//VT_Web_ConvertUTF8ToGBK(val_msg.c_str(),pGBK,nGBKSize,&nNeedSize);
					std::cout<<__FUNCTION__<<value_type<<pGBK<<std::endl;
					delete[] pGBK;
				}
				//std::cout<<__FUNCTION__<<value_type<<val_msg<<std::endl;
			}
		}	
		printf("%s %d %d \n",__FUNCTION__,p_user_1,p_user_2);
		return 0;
}
/*
int testExist(void* pTest,int nSize)
{
void* pNull = 0;
memcpy(pTest,&pNull,sizeof(void*));
return 0;
}*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static int on_ops_wss_close_call(void* id,const char* type,const char* value_type,const void* p_value,
	int n_value_size,void* p_user_1,void* p_user_2)
{
	return 0;
	//void* pTest = NULL;
	//testExist(&pTest,4);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static int on_ops_wss_open_call(void* id,const char* type,const char* value_type,const void* p_value,
	int n_value_size,void* p_user_1,void* p_user_2)
{


	return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CStreamExport::CStreamExport()
{
	m_pStream = NULL;
	m_pStreamList = NULL;	
	m_hLock = NULL;
	m_pSendThread = NULL;
	m_pStreamCon = NULL;
	m_pStreamConMeta = NULL;

	CHDRealStream* pStream = new CHDRealStream();
	if(pStream != NULL)
	{
		pStream->init();
		//HWND  hWndHDRealStream = NULL;
		//GetDlgItem(IDC_STATIC_HD_REALSTREAM_VIEW,&hWndHDRealStream);
		//pStream->setDisplayHWND((void*)hWndHDRealStream);
		pStream->setStreamFuncMeta(recv_stream_func,this,NULL);
		m_pStream = pStream;
	}

	createLock();

	lock();
	m_pStreamList = new std::list<StreamMeta>;
	unlock();
	m_pSendThread  = createThread();

	m_pStreamConMeta = new StreamConnectMeta;

}
int CStreamExport::startStream(const char* pStreamId,const char* psrcUrl,const char* pDstUrl)
{
	if(psrcUrl == NULL)
	{
		return -1;
	}
	int status = 0;
	CHDRealStream* pStream  = (CHDRealStream*)m_pStream;
	if(pStream != NULL)
	{
		status = pStream->start(psrcUrl);
	}
	createStreamConnect(pDstUrl);
	return 0;
}

int CStreamExport::stopStream(const char* pStreamId,const char* psrcUrl)
{
	if(psrcUrl == NULL)
	{
		return -1;
	}
	int status = 0;
	CHDRealStream* pStream  = (CHDRealStream*)m_pStream;
	if(pStream != NULL)
	{
		status = pStream->stop();
	}
	stopStreamConnect(psrcUrl);
	return 0;
}
int  CStreamExport::sendStream(const char* pDstUrl,void* pData,int nDataSize)
{
	return 0;
}

int  CStreamExport::recvStream(int nType,void* pData,int nDataSize)
{
	if(pData == NULL)
	{
		return -1;
	}
	//void* pMetaData = (void*)malloc(nDataSize);
	void* pMetaData = new char[nDataSize];
	if(pMetaData == NULL)
	{
		return -1;
	}
	memcpy(pMetaData,pData,nDataSize);
	lock();
	std::list<StreamMeta>* pStreamList = (std::list<StreamMeta>*)(m_pStreamList); 
	if(pStreamList != NULL)
	{		
		StreamMeta streamMetaInfo;
		streamMetaInfo.pBuf = pMetaData;
		streamMetaInfo.nType = nType;
		streamMetaInfo.nBuf = nDataSize;
		pStreamList->push_back(streamMetaInfo);

	}
	unlock();
	return 0;
}

int CStreamExport::createLock()
{
#ifdef WIN32
	m_hLock = (void*)malloc(sizeof(CRITICAL_SECTION));
	InitializeCriticalSection((LPCRITICAL_SECTION)m_hLock);
#endif
	return 0;
}
int CStreamExport::destroyLock()
{
#ifdef WIN32
	if(m_hLock != NULL)
	{
		DeleteCriticalSection((LPCRITICAL_SECTION)m_hLock);
	}
#endif
	return 0;
}
int CStreamExport::lock()
{
#ifdef WIN32
	if(m_hLock != NULL)
	{
		EnterCriticalSection((LPCRITICAL_SECTION)m_hLock);
	}
#endif
	return 0;
}

int CStreamExport::unlock()
{
#ifdef WIN32
	if(m_hLock != NULL)
	{
		LeaveCriticalSection((LPCRITICAL_SECTION)m_hLock);
	}
#endif
	return 0;
}

void* CStreamExport::createThread()
{
	void* pThread = NULL;
#ifdef WIN32
	HANDLE  hThread;
	DWORD   threadId;
	hThread = CreateThread(NULL,0,proc_stream_func,this,0,&threadId);
	pThread = hThread;
#else


#endif
	return pThread;
}
int CStreamExport::sendStream(void* pStream,void* pID,void* pData,int nDataSize)
{
	int nStatus =  0;
	CWebSocketClient* pClient = (CWebSocketClient*)pStream;
	if(pClient != NULL)
	{

		pClient->sendBinary(pData,nDataSize);
	}
	return nStatus;
}

int CStreamExport::popStreamMeta(void*& pBuf,int& nBufLen, int& nType)
{
	StreamMeta meta;
	memset(&meta,0,sizeof(meta));
	lock();
	std::list<StreamMeta>* pStreamList = (std::list<StreamMeta>*)(m_pStreamList); 
	if(pStreamList != NULL)
	{
		if(!pStreamList->empty())
		{
			meta = pStreamList->front();
			pBuf = meta.pBuf;
			nBufLen = meta.nBuf;
			nType = meta.nType;
			pStreamList->pop_front();
		}
	}
	unlock();
	return 0;
}
void* CStreamExport::createStreamConnect(const char* pDstUrl)
{
	if(pDstUrl == NULL)
	{
		return NULL;
	}
	void* pConnect = NULL;
	void* pID = NULL;
	getStreamHandle(pConnect,pID);
	if(pConnect != NULL)
	{
		CWebSocketClient* pWSCli = (CWebSocketClient*)pConnect;
		delete pWSCli;
		pConnect = NULL;
		pID = NULL;
	}
	pConnect = new CWebSocketClient();
	connectWSS(pConnect,pDstUrl,pID);
	setStreamHandle(pConnect,pID);
	return 0;
}
int	  CStreamExport::stopStreamConnect(const char* pszUrl)
{
	void* pStream = NULL;
	void* pID = NULL;
	getStreamHandle(pStream,pID);
	CWebSocketClient* pClient = (CWebSocketClient*)pStream;
	if(pClient != NULL)
	{
		delete pClient;
		pClient = NULL;
	}
	setStreamHandle(NULL,NULL);
	return 0;
}
int CStreamExport::connectWSS(void*& pWSCli,const char* pszUrl,void*& pWSId)
{

	std::string	ws_uri = "ws://localhost:9002";
	if(pszUrl != NULL)
	{
		ws_uri = pszUrl;
	}
	std::cout <<"websocket url:"<<ws_uri<<std::endl;
	
	void* pID = NULL;

	CWebSocketClient* pClient = (CWebSocketClient*)pWSCli;
	if(pClient != NULL)
	{
		pClient->connect(ws_uri.c_str());
	}
	//当前ws client回调出来的id和当前指针一样
	pID = pWSCli;

	return 0;
}


int CStreamExport::getStreamHandle(void*& pWSCli,void*& pID)
{
	lock();
	StreamConnectMeta* pMeta = NULL;
	pMeta = (StreamConnectMeta*)m_pStreamConMeta;
	if(pMeta != NULL)
	{
		pWSCli = pMeta->pCli;
		pID = pMeta->pID;
	}
	unlock();
	return 0;
}
int	CStreamExport::setStreamHandle(void* pWSCli,void* pID)
{
	lock();
	StreamConnectMeta* pMeta = NULL;
	pMeta = (StreamConnectMeta*)m_pStreamConMeta;
	if(pMeta != NULL)
	{
		pMeta->pCli = pWSCli;
		pMeta->pID = pID;
	}
	unlock();
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int recv_stream_func(void* hStream,int nType,void* pBuf,int nBuf,void* pUser1,void* pUser2)
{
	CStreamExport* pExport = (CStreamExport*)pUser1;
	if(pExport != NULL)
	{
		pExport->recvStream(nType,pBuf,nBuf);
	}
	return 0;
}

unsigned long __stdcall proc_stream_func(void* pData)
{
	CStreamExport* pExport = (CStreamExport*)pData;
	while(1)
	{
		if(pExport != NULL)
		{
			//pExport->sendStream(
			StreamMeta meta;
			pExport->popStreamMeta(meta.pBuf,meta.nBuf,meta.nType);
			if(meta.pBuf != NULL)
			{
				void* pStream = NULL;
				void* pID = NULL;
				pExport->getStreamHandle(pStream,pID);
				
				pExport->sendStream(pStream,pID,meta.pBuf,meta.nBuf);
					
				//free(meta.pBuf);
				delete [] meta.pBuf;

				meta.pBuf = NULL;
			}
			else
			{

			}
		}
#ifdef WIN32
		Sleep(10);
#endif
	}

}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////